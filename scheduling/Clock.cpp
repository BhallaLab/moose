/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * The Clock manages simulation scheduling, in a close
 * collaboration with the Tick.
 * This version does this using an array of child Ticks, which
 * it manages directly.
 *
 * Simulation scheduling requires that certain functions of
 * groups of objects be called in a strict sequence according to
 * a range of timesteps, dt. For example, the numerical integration
 * function of a compartment may be called every 10 microseconds.
 * Furthermore, there may be ion channels in the simulation which also
 * have to be called every 10 microseconds, but always after all
 * the compartments have been called. Finally,
 * the graphical update function to plot the compartment voltage
 * may be called every 1 millisecond of simulation time.
 * The whole sequence has to be repeated till the runtime of the
 * simulation is complete.
 *
 * This scheduler uses integral multiples of the base timestep dt_.
 *
 * The system works like this:
 * 1. Assign times to each Tick. This is divided by dt_ and the rounded
 * 		value is used for the integral multiple. Zero means the tick is not
 * 		scheduled.
 * 2. The process call goes through all active ticks in order every
 * 		timestep. Each Tick increments its counter and decides if it is
 * 		time to fire.
 * 4. The Reinit call goes through all active ticks in order, just once.
 * 5. We connect up the Ticks to their target objects.
 * 6. We begin the simulation by calling 'start' or 'step' on the Clock.
 * 		'step' executes exactly one timestep (of the minimum dt_),
 * 		visiting all ticks as above..
 * 		'start' executes an integral number of such timesteps.
 * 7. To interrupt the simulation at some intermediate time, call 'stop'.
 * 		This lets the system complete its current step.
 * 8. To restart the simulation from where it left off, use the same
 * 		'start' or 'step' function on the Clock. As all the ticks
 * 		retain their state, the simulation can resume smoothly.
 */

#include "header.h"
#include "Clock.h"
#include "../utility/numutil.h"
#include "../utility/print_function.hpp"

#if PARALLELIZE_CLOCK_USING_CPP11_ASYNC
#include <future>
#endif

// Declaration of some static variables.
const unsigned int Clock::numTicks = 32;
/// minimumDt is smaller than any known event on the scales MOOSE handles.
const double minimumDt = 1e-7;
map< string, unsigned int > Clock::defaultTick_;
vector< double > Clock::defaultDt_;

///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////

static SrcFinfo0 *finished()
{
    static SrcFinfo0 finished(
        "finished",
        "Signal for completion of run"
    );
    return &finished;
}

// Static func for building the process vectors.
static vector< SrcFinfo1< ProcPtr >* > buildProcessVec( const string& name )
{
    vector< SrcFinfo1< ProcPtr >* > vec;
    vec.resize( Clock::numTicks );
    for ( unsigned int i = 0; i < Clock::numTicks; ++i )
    {
        stringstream ss;
        ss << name << i;
        stringstream ss2;
        ss2 << name << " for Tick " << i;
        vec[i] = new SrcFinfo1< ProcPtr >( ss.str(), ss2.str() );
    }
    return vec;
}

// This vector contains the SrcFinfos used for Process calls for each
// of the Ticks.
static vector< SrcFinfo1< ProcPtr >* >& processVec()
{
    static vector< SrcFinfo1< ProcPtr >* > processVec =
        buildProcessVec( "process" );
    return processVec;
}

static vector< SrcFinfo1< ProcPtr >* >& reinitVec()
{
    static vector< SrcFinfo1< ProcPtr >* > reinitVec =
        buildProcessVec( "reinit" );
    return reinitVec;
}

static vector< SharedFinfo *>& sharedProcVec()
{
    static vector< SharedFinfo* > vec;
    if ( vec.size() == 0 )
    {
        vec.resize( Clock::numTicks );
        for ( unsigned int i = 0; i < Clock::numTicks; ++i )
        {
            stringstream ss;
            Finfo* temp[2];
            temp[0] = processVec()[i];
            temp[1] = reinitVec()[i];
            ss << "proc" << i;
            vec[i] = new SharedFinfo( ss.str(),
                                      "Shared process/reinit message",
                                      temp, 2 );
        }
    }
    return vec;
}

const Cinfo* Clock::initCinfo()
{
    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////
    static ValueFinfo< Clock, double > dt(
        "baseDt",
        "Base timestep for simulation. This is the smallest dt out "
        "of all the clock ticks. By definition all other timesteps "
        "are integral multiples of this, and are rounded to "
        "ensure that this is the case . ",
        &Clock::setDt,
        &Clock::getDt
    );
    static ReadOnlyValueFinfo< Clock, double > runTime(
        "runTime",
        "Duration to run the simulation",
        &Clock::getRunTime
    );
    static ReadOnlyValueFinfo< Clock, double > currentTime(
        "currentTime",
        "Current simulation time",
        &Clock::getCurrentTime
    );
    static ReadOnlyValueFinfo< Clock, unsigned long > nsteps(
        "nsteps",
        "Number of steps to advance the simulation, in units of the smallest timestep on the clock ticks",
        &Clock::getNsteps
    );
    static ReadOnlyValueFinfo< Clock, unsigned int > numTicks(
        "numTicks",
        "Number of clock ticks",
        &Clock::getNumTicks
    );
    static ReadOnlyValueFinfo< Clock, unsigned int > stride(
        "stride",
        "Number by which the simulation advances the current step on each cycle. stride = smallest active timestep/smallest defined timestep.",
        &Clock::getStride
    );
    static ReadOnlyValueFinfo< Clock, unsigned long > currentStep(
        "currentStep",
        "Current simulation step",
        &Clock::getCurrentStep
    );

    static ReadOnlyValueFinfo< Clock, vector< double > > dts(
        "dts",
        "Utility function returning the dt (timestep) of all ticks.",
        &Clock::getDts
    );

    static ReadOnlyValueFinfo< Clock, bool > isRunning(
        "isRunning",
        "Utility function to report if simulation is in progress.",
        &Clock::isRunning
    );

    static LookupValueFinfo< Clock, unsigned int, unsigned int >
    tickStep(
        "tickStep",
        "Step size of specified Tick, as integral multiple of dt_"
        " A zero step size means that the Tick is inactive",
        &Clock::setTickStep,
        &Clock::getTickStep
    );

    static LookupValueFinfo< Clock, unsigned int, double > tickDt(
        "tickDt",
        "Timestep dt of specified Tick. Always integral multiple of "
        "dt_. If you assign a non-integer multiple it will round off. "
        " A zero timestep means that the Tick is inactive",
        &Clock::setTickDt,
        &Clock::getTickDt
    );

    static ReadOnlyLookupValueFinfo< Clock, string, unsigned int > defaultTick(
        "defaultTick",
        "Looks up the default Tick to use for the specified class. "
        "If no tick is assigned, as for classes without a process "
        "operation or zombie classes, the tick is ~0U. "
        "If nothing can be found returns 0 and emits a warning.",
        &Clock::getDefaultTick
    );
    ///////////////////////////////////////////////////////
    // Shared definitions
    ///////////////////////////////////////////////////////
    static vector< SharedFinfo *> procs = sharedProcVec();

    ///////////////////////////////////////////////////////
    // MsgDest definitions
    ///////////////////////////////////////////////////////
    static DestFinfo start( "start",
                            "Sets off the simulation for the specified duration",
                            new EpFunc2< Clock, double, bool >(&Clock::handleStart )
                          );

    static DestFinfo step( "step",
                           "Sets off the simulation for the specified # of steps. "
                           "Here each step advances the simulation by the timestep of the "
                           "smallest tick that is actually in use. ",
                           new EpFunc1< Clock, unsigned long >(&Clock::handleStep )
                         );

    static DestFinfo stop( "stop",
                           "Halts the simulation, with option to restart seamlessly",
                           new OpFunc0< Clock >(&Clock::stop )
                         );

    static DestFinfo reinit( "reinit",
                             "Zeroes out all ticks, starts at t = 0",
                             new EpFunc0< Clock >(&Clock::handleReinit )
                           );

    static Finfo* clockControlFinfos[] =
    {
        &start, &step, &stop, &reinit,
    };
    ///////////////////////////////////////////////////////
    // SharedFinfo for Shell to control Clock
    ///////////////////////////////////////////////////////
    static SharedFinfo clockControl( "clockControl",
                                     "Controls all scheduling aspects of Clock, usually from Shell",
                                     clockControlFinfos,
                                     sizeof( clockControlFinfos ) / sizeof( Finfo* )
                                   );

    static Finfo* clockFinfos[] =
    {
        // Fields
        &dt,				// Value
        &runTime,			// ReadOnlyValue
        &currentTime,		// ReadOnlyValue
        &nsteps,			// ReadOnlyValue
        &numTicks,			// ReadOnlyValue
        &stride,			// ReadOnlyValue
        &currentStep,		// ReadOnlyValue
        &dts,				// ReadOnlyValue
        &isRunning,			// ReadOnlyValue
        &tickStep,			// LookupValue
        &tickDt,			// LookupValue
        &defaultTick,		// ReadOnlyLookupValue
        &clockControl,		// Shared
        finished(),			// Src
        procs[0],				// Src
        procs[1],				// Src
        procs[2],				// Src
        procs[3],				// Src
        procs[4],				// Src
        procs[5],				// Src
        procs[6],				// Src
        procs[7],				// Src
        procs[8],				// Src
        procs[9],				// Src
        procs[10],				// Src
        procs[11],				// Src
        procs[12],				// Src
        procs[13],				// Src
        procs[14],				// Src
        procs[15],				// Src
        procs[16],				// Src
        procs[17],				// Src
        procs[18],				// Src
        procs[19],				// Src
        procs[20],				// Src
        procs[21],				// Src
        procs[22],				// Src
        procs[23],				// Src
        procs[24],				// Src
        procs[25],				// Src
        procs[26],				// Src
        procs[27],				// Src
        procs[28],				// Src
        procs[29],				// Src
        procs[30],				// Src
        procs[31],				// Src
    };

    static string doc[] =
    {
        "Name", "Clock",
        "Author", "Upinder S. Bhalla, Nov 2013, NCBS",
        "Description",

        "Every object scheduled for operations in MOOSE is connected to one"
        "of the 'Tick' entries on the Clock.\n"
        "The Clock manages 32 'Ticks', each of which has its own dt,"
        "which is an integral multiple of the clock baseDt_. "
        "On every clock step the ticks are examined to see which of them"
        "is due for updating. When a tick is updated, the 'process' call "
        "of all the objects scheduled on that tick is called. "
        "Order of execution: If a subset of ticks are scheduled for "
        "execution at a given timestep, then they will be executed in "
        "numerical order, lowest tick first and highest last. "
        "There is no guarantee of execution order for objects within "
        "a clock tick.\n"
        "The clock provides default scheduling for all objects which "
        "can be accessed using Clock::lookupDefaultTick( className ). "
        "Specific items of note are that the output/file dump objects are "
        "second-last, and the postmaster is last on the order of Ticks. "
        "The clock also starts up with some default timesteps for each "
        "of these ticks, and this can be overridden using the shell "
        "command setClock, or by directly assigning tickStep values on the "
        "clock object."
        "Which objects use which tick? As a rule of thumb, try this: \n"
        "Electrical/compartmental model calculations: Ticks 0-7 \n"
        "Tables and output objects for electrical output: Tick 8 \n"
        "Diffusion solver: Tick 10 \n"
        "Chemical/compartmental model calculations: Ticks 11-17\n"
        "Tables and output objects for chemical output: Tick 18 \n"
        "Unassigned: Ticks 20-29 \n"
        "Special: 30-31 \n"
        "Data output is a bit special, since you may want to store data "
        "at different rates for electrical and chemical processes in the "
        "same model. Here you will have to specifically assign distinct "
        "clock ticks for the tables/fileIO objects handling output at "
        "different time-resolutions. Typically one uses tick 8 and 18.\n"
        "Here are the detailed mappings of class to tick.\n"
        "   Class              Tick       dt \n"
        "   DiffAmp             0       50e-6\n"
        "   Interpol            0       50e-6\n"
        "   PIDController       0       50e-6\n"
        "   PulseGen            0       50e-6\n"
        "   StimulusTable       0       50e-6\n"
        "   testSched           0       50e-6\n"
        "   VClamp              0       50e-6\n"
        "   SynHandlerBase      1       50e-6\n"
        "   SimpleSynHandler    1       50e-6\n"
        "   STDPSynHandler      1       50e-6\n"
        "   GraupnerBrunel2012CaPlasticitySynHandler    1        50e-6\n"
        "   SeqSynHandler       1       50e-6\n"
        "	CaConc				1		50e-6\n"
        "	CaConcBase			1		50e-6\n"
        "	DifShell			1		50e-6\n"
	    "	DifShellBase		1		50e-6\n"
	    "   MMPump              1       50e-6\n"
	    "	DifBuffer			1		50e-6\n"
	    "	DifBufferBase		1		50e-6\n"
        "	MgBlock				1		50e-6\n"
        "	Nernst				1		50e-6\n"
        "	RandSpike			1		50e-6\n"
        "	ChanBase			2		50e-6\n"
        "	IntFire				2		50e-6\n"
        "	IntFireBase			2		50e-6\n"
        "	LIF					2		50e-6\n"
        "	QIF					2		50e-6\n"
        "	ExIF				2		50e-6\n"
        "	AdExIF				2		50e-6\n"
        "	AdThreshIF			2		50e-6\n"
        "	IzhIF				2		50e-6\n"
        "	IzhikevichNrn		2		50e-6\n"
        "	SynChan				2		50e-6\n"
        "	NMDAChan			2		50e-6\n"
        "	GapJunction			2		50e-6\n"
        "	HHChannel			2		50e-6\n"
        "	HHChannel2D			2		50e-6\n"
        "	Leakage				2		50e-6\n"
        "	MarkovChannel		2		50e-6\n"
        "	MarkovGslSolver		2		50e-6\n"
        "	MarkovRateTable		2		50e-6\n"
        "	MarkovSolver		2		50e-6\n"
        "	MarkovSolverBase	2		50e-6\n"
        "	RC					2		50e-6\n"
        "	Compartment (init)	3		50e-6\n"
        "	CompartmentBase (init )		3		50e-6\n"
        "	SymCompartment	(init)		3		50e-6\n"
        "	Compartment 		4		50e-6\n"
        "	CompartmentBase		4		50e-6\n"
        "	SymCompartment		4		50e-6\n"
        "	SpikeGen			5		50e-6\n"
        "	HSolve				6		50e-6\n"
        "	SpikeStats			7		50e-6\n"
        "	Table				8		0.1e-3\n"
        "	TimeTable			8		0.1e-3\n"

        "	Dsolve				10		0.01\n"
        "	Adaptor				11		0.1\n"
        "	Func				12		0.1\n"
        "	Function			12		0.1\n"
        "	Arith				12		0.1\n"
        "	BufPool				13		0.1\n"
        "	Pool				13		0.1\n"
        "	PoolBase			13		0.1\n"
        "	CplxEnzBase			14		0.1\n"
        "	Enz					14		0.1\n"
        "	EnzBase				14		0.1\n"
        "	MMenz				14		0.1\n"
        "	Reac				14		0.1\n"
        "	ReacBase			14		0.1\n"
        "	Gsolve	(init)		15		0.1\n"
        "	Ksolve	(init)		15		0.1\n"
        "	Gsolve				16		0.1\n"
        "	Ksolve				16		0.1\n"
        "	Stats				17		0.1\n"
        "	Table2				18		1\n"
        "	Streamer			19		10\n"

        "	HDF5DataWriter		30		1\n"
        "	HDF5WriterBase		30		1\n"
        "	NSDFWriter			30		1\n"
        "   PyRun           	30      1\n"
        "	PostMaster			31		0.01\n"
        "	\n"
        "	Note that the other classes are not scheduled at all.",
    };

    static Dinfo< Clock > dinfo;
    static Cinfo clockCinfo(
        "Clock",
        // "Clock class handles sequencing of operations in simulations",
        Neutral::initCinfo(),
        clockFinfos,
        sizeof(clockFinfos)/sizeof(Finfo *),
        &dinfo,
        doc,
        sizeof(doc)/sizeof(string)
    );

    return &clockCinfo;
}

static const Cinfo* clockCinfo = Clock::initCinfo();

///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
Clock::Clock()
    : runTime_( 0.0 ),
      currentTime_( 0.0 ),
      nSteps_( 0 ),
      currentStep_( 0 ),
      stride_( 1 ),
      dt_( 1.0 ),
      isRunning_( false ),
      doingReinit_( false ),
      info_(),
      ticks_( Clock::numTicks, 0 )
{
    buildDefaultTick();
    dt_ = defaultDt_[0];
    for ( unsigned int i = 0; i < Clock::numTicks; ++i )
    {
        ticks_[i] = round( defaultDt_[i] / dt_ );
    }
}

Clock::~Clock()
{
    if ( Msg::isLastTrump() )   // Clean up, end of the simulation.
    {
        for ( unsigned int i = 0; i < Clock::numTicks; ++i )
        {
            delete processVec()[i];
            delete reinitVec()[i];
            delete sharedProcVec()[i];
        }
    }
}
///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////
void Clock::setDt( double v)
{
    if ( isRunning_ )
    {
        cout << "Warning: Clock::setDt: Cannot change dt while simulation is running\n";
        return;
    }
    dt_ = v;
}

double Clock::getDt() const
{
    return dt_;
}
double Clock::getRunTime() const
{
    return runTime_;
}

double Clock::getCurrentTime() const
{
    return currentTime_;
}

unsigned long Clock::getNsteps() const
{
    return nSteps_;
}

unsigned long Clock::getCurrentStep() const
{
    return currentStep_;
}

unsigned int Clock::getNumTicks() const
{
    return numTicks;
}

unsigned int Clock::getStride() const
{
    return stride_;
}

vector< double > Clock::getDts() const
{
    vector< double > ret;
    for ( unsigned int i = 0; i < ticks_.size(); ++i )
    {
        ret.push_back( ticks_[ i ] * dt_ );
    }
    return ret;
}

bool Clock::isRunning() const
{
    return isRunning_;
}

bool Clock::isDoingReinit() const
{
    return doingReinit_;
}

bool Clock::checkTickNum( const string& funcName, unsigned int i ) const
{
    if ( isRunning_ || doingReinit_)
    {
        cout << "Warning: Clock::" << funcName <<
             ": Cannot change dt while simulation is running\n";
        return false;
    }
    if ( i >= Clock::numTicks )
    {
        cout << "Warning: Clock::" << funcName <<
             "( " << i << " ): Clock has only " <<
             Clock::numTicks << " ticks \n";
        return false;
    }
    return true;
}

void Clock::setTickStep( unsigned int i, unsigned int v )
{
    if ( checkTickNum( "setTickStep", i ) )
        ticks_[i] = v;
}
unsigned int Clock::getTickStep( unsigned int i ) const
{
    if ( i < Clock::numTicks )
        return ticks_[i];
    return 0;
}

/**
 * A little nasty because we want to ensure that the main clock dt is
 * set intelligently from the assignment here.
 */
void Clock::setTickDt( unsigned int i, double v )
{
    unsigned int numUsed = 0;
    if ( v < minimumDt )
    {
        cout << "Warning: Clock::setTickDt: " << v <<
             " is smaller than minimum allowed timestep " <<
             minimumDt << endl;
        cout << "dt not set\n";
        return;
    }
    for ( unsigned int j = 0; j < numTicks; ++j )
        numUsed += ( ticks_[j] != 0 );

    if ( numUsed == 0 )
    {
        dt_ = v;
    }
    else if ( dt_ > v )
    {
        for ( unsigned int j = 0; j < numTicks; ++j )
            if ( ticks_[j] != 0 )
                ticks_[j] = round( ( ticks_[j] * dt_ ) / v );
        dt_ = v;
    }

    if ( checkTickNum( "setTickDt", i ) )
        ticks_[i] = round( v / dt_ );
}

double Clock::getTickDt( unsigned int i ) const
{
    if ( i < Clock::numTicks )
        return ticks_[i] * dt_;
    return 0.0;
}

unsigned int Clock::getDefaultTick( string s ) const
{
    return Clock::lookupDefaultTick( s );
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

/**
 * Does a graceful stop of the simulation, leaving so it can continue
 * cleanly with another step or start command.
 */
void Clock::stop()
{
    isRunning_ = 0;
}

/////////////////////////////////////////////////////////////////////
// Info functions
/////////////////////////////////////////////////////////////////////

/// Static function
void Clock::reportClock()
{
    const Clock* c = reinterpret_cast< const Clock* >( Id( 1 ).eref().data() );
    c->innerReportClock();
}

void Clock::innerReportClock() const
{
    cout << "reporting Clock: runTime= " << runTime_ <<
         ", currentTime= " << currentTime_ <<
         ", dt= " << dt_ << ", isRunning = " << isRunning_ << endl;
    cout << "Dts= ";
    for ( unsigned int i = 0; i < ticks_.size(); ++i )
    {
        cout <<  "tick[" << i << "] = " << ticks_[i] << "	" <<
             ticks_[i] * dt_ << endl;
    }
    cout << endl;
}

/////////////////////////////////////////////////////////////////////
// Core scheduling functions.
/////////////////////////////////////////////////////////////////////

void Clock::buildTicks( const Eref& e )
{
    activeTicks_.resize(0);
    activeTicksMap_.resize(0);
    stride_ = ~0U;
    for ( unsigned int i = 0; i < ticks_.size(); ++i )
    {
        if ( ticks_[i] > 0 &&
                e.element()->hasMsgs( processVec()[i]->getBindIndex() ) )
        {
            activeTicks_.push_back( ticks_[i] );
            activeTicksMap_.push_back( i );
            if ( ticks_[i] > 0 && stride_ > ticks_[i] )
                stride_ = ticks_[i];
        }
    }
    // Should really do the HCF of N numbers here to get the stride.
}

/**
 * Start has to happen gracefully: If the simulation was stopped for any
 * reason, it has to pick up where it left off.
 * The "runtime" argument is the additional time to run the simulation.
 */
void Clock::handleStart( const Eref& e, double runtime, bool notify )
{
    notify_ = notify;

    if ( stride_ == 0 || stride_ == ~0U )
        stride_ = 1;
    unsigned long n = round( runtime / ( stride_ * dt_ ) );

    handleStep( e, n );

}

void Clock::handleStep( const Eref& e, unsigned long numSteps )
{
    numSteps *= stride_;
    if ( isRunning_ || doingReinit_ )
    {
        cout << "Clock::handleStart: Warning: simulation already in progress.\n Command ignored\n";
        return;
    }

    time_t rawtime;
    struct tm * timeinfo;
    char now[80];

    buildTicks( e );
    assert( currentStep_ == nSteps_ );
    assert( activeTicks_.size() == activeTicksMap_.size() );
    nSteps_ += numSteps;
    runTime_ = nSteps_ * dt_;
    for ( isRunning_ = (activeTicks_.size() > 0 );
            isRunning_ && currentStep_ < nSteps_; currentStep_ += stride_ )
    {
        // Curr time is end of current step.
        unsigned long endStep = currentStep_ + stride_;
        currentTime_ = info_.currTime = dt_ * endStep;

#if PARALLELIZE_CLOCK_USING_CPP11_ASYNC

        // NOTE: It does not produce very promising results. The challenge here
        // is doing load-balancing.
        // TODO: To start with, we can put one solver on one thread and everything
        // else onto 1 thread. Each Hsove, Ksolve, and Gsolve can take its own
        // thread and rest are on different threads.

        unsigned int nTasks = activeTicks_.size( );
        unsigned int numThreads_ = 3;
        unsigned int blockSize = 1 + (nTasks / numThreads_);

        for( unsigned int i = 0; i < numThreads_; ++i  )
        {
            std::async( std::launch::async
                , [this,blockSize,i,nTasks,endStep,e]
                {
                    unsigned int mapI = i * blockSize;
                    // Get the block we want to run in paralle.
                    for( unsigned int ii = i * blockSize; ii < min((i+1) * blockSize, nTasks); ii++ )
                    {
                        unsigned int j = activeTicks_[ ii ];
                        if( endStep % j == 0  )
                        {
                             info_.dt = j * dt_;
                             processVec( )[ activeTicksMap_[mapI] ]->send( e, &info_ );
                        }
                        mapI++;
                    }
                }
            );
        }
#else
        vector< unsigned int >::const_iterator k = activeTicksMap_.begin();
        for ( vector< unsigned int>::iterator j =
                    activeTicks_.begin(); j != activeTicks_.end(); ++j )
        {
            if ( endStep % *j == 0 )
            {
                info_.dt = *j * dt_;
                processVec()[*k]->send( e, &info_ );
            }
            ++k;
        }
#endif

        // When 10% of simulation is over, notify user when notify_ is set to
        // true.
        if( notify_ )
        {
            if( fmod(100 * currentTime_ / runTime_ , 10.0) == 0.0 )
            {
                time( &rawtime );
                timeinfo = localtime( &rawtime );
                strftime(now, 80, "%c", timeinfo);
                cout << "@ " << now << ": " << 100 * currentTime_ / runTime_
                    << "% of total " << runTime_ << " seconds is over." << endl;
            }
        }

        if ( activeTicks_.size() == 0 )
            currentTime_ = runTime_;
    }

    info_.dt = dt_;
    isRunning_ = false;
    finished()->send( e );
}

/**
 * This is the dest function that sets off the reinit.
 */
void Clock::handleReinit( const Eref& e )
{
    if ( isRunning_ || doingReinit_ )
    {
        cout << "Clock::handleReinit: Warning: simulation already in progress.\n Command ignored\n";
        return;
    }
    currentTime_ = 0.0;
    currentStep_ = 0;
    nSteps_ = 0;
    buildTicks( e );
    doingReinit_ = true;
    // Curr time is end of current step.
    info_.currTime = 0.0;
    vector< unsigned int >::const_iterator k = activeTicksMap_.begin();
    for ( vector< unsigned int>::iterator j =
                activeTicks_.begin(); j != activeTicks_.end(); ++j )
    {
        info_.dt = *j * dt_;
        reinitVec()[*k++]->send( e, &info_ );
    }
    info_.dt = dt_;
    doingReinit_ = false;
}

/*
 * Useful function, only I don't need it yet. Was implemented for Dsolve
double Dsolve::findDt( const Eref& e )
{
	// Here is the horrible stuff to traverse the message to get the dt.
	const Finfo* f = Dsolve::initCinfo()->findFinfo( "reinit" );
	const DestFinfo* df = dynamic_cast< const DestFinfo* >( f );
	assert( df );
	unsigned int fid = df->getFid();
	ObjId caller = e.element()->findCaller( fid );
	const Msg* m = Msg::getMsg( caller );
	assert( m );
	vector< string > src = m->getSrcFieldsOnE1();
	assert( src.size() > 0 );
	string temp = src[0].substr( src[0].length() - 1 ); // reinitxx
	unsigned int tick = atoi( temp.c_str() );
	assert( tick < 10 );
	Id clock( 1 );
	assert( clock.element() == m->e1() );
	double dt = LookupField< unsigned int, double >::
			get( clock, "tickDt", tick );
	return dt;
}
*/

/**
 * This is the authoritative database of default tick assignments
 * for each class. This is here because one needs to see relative
 * locations of all assignments in order to do this.
 * This assumes that in cases where there are two ticks (such as
 * init and process) for a single class, then they are sequential.
 * Base classes are also represented here. They are checked as a fallback
 * if the more specific class lookup fails.
 * Certain classes, such as Zombies, explicitly do not get scheduled
 * even though they may inherit process messages. These are given
 * the tick ~0U.
 */
void Clock::buildDefaultTick()
{
    defaultTick_["DiffAmp"] = 0;
    defaultTick_["Interpol"] = 0;
    defaultTick_["PIDController"] = 0;
    defaultTick_["PulseGen"] = 0;
    defaultTick_["StimulusTable"] = 0;
    defaultTick_["testSched"] = 0;
    defaultTick_["VClamp"] = 0;
    defaultTick_["SynHandlerBase"] = 1;
    defaultTick_["SimpleSynHandler"] = 1;
    defaultTick_["STDPSynHandler"] = 1;
    defaultTick_["GraupnerBrunel2012CaPlasticitySynHandler"] = 1;
    defaultTick_["SeqSynHandler"] = 1;
    defaultTick_["CaConc"] = 1;
    defaultTick_["CaConcBase"] = 1;
    defaultTick_["DifShell"] = 1;
    defaultTick_["DifShellBase"] = 1;
    defaultTick_["MMPump"] =  1;
    defaultTick_["DifBuffer"] = 1;
    defaultTick_["DifBufferBase"] = 1;
    defaultTick_["MgBlock"] = 1;
    defaultTick_["Nernst"] = 1;
    defaultTick_["RandSpike"] = 1;
    defaultTick_["ChanBase"] = 2;
    defaultTick_["IntFire"] = 2;
    defaultTick_["IntFireBase"] = 2;
    defaultTick_["LIF"] = 2;
    defaultTick_["QIF"] = 2;
    defaultTick_["ExIF"] = 2;
    defaultTick_["AdExIF"] = 2;
    defaultTick_["AdThreshIF"] = 2;
    defaultTick_["IzhIF"] = 2;
    defaultTick_["IzhikevichNrn"] = 2;
    defaultTick_["SynChan"] = 2;
    defaultTick_["NMDAChan"] = 2;
    defaultTick_["GapJunction"] = 2;
    defaultTick_["HHChannel"] = 2;
    defaultTick_["HHChannel2D"] = 2;
    defaultTick_["Leakage"] = 2;
    defaultTick_["MarkovChannel"] = 2;
    defaultTick_["MarkovGslSolver"] = 2;
    defaultTick_["MarkovRateTable"] = 2;
    defaultTick_["MarkovSolver"] = 2;
    defaultTick_["MarkovSolverBase"] = 2;
    defaultTick_["RC"] = 2;
    defaultTick_["Compartment"] = 4; // Note these use an 'init', at t-1.
    defaultTick_["CompartmentBase"] = 4; // Uses 'init'
    defaultTick_["SymCompartment"] = 4; // Uses 'init'
    defaultTick_["SpikeGen"] = 5;
    defaultTick_["HSolve"] = 6;
    defaultTick_["SpikeStats"] = 7;
    defaultTick_["Table"] = 8;
    defaultTick_["TimeTable"] = 8;
    defaultTick_["Dsolve"] = 10;
    defaultTick_["Adaptor"] = 11;
    defaultTick_["Func"] = 12;
    defaultTick_["Function"] = 12;
    defaultTick_["Arith"] = 12;
    /*
    defaultTick_["FuncBase"] = 12;
    defaultTick_["FuncPool"] = 12;
    defaultTick_["MathFunc"] = 12;
    defaultTick_["SumFunc"] = 12;
    */
    defaultTick_["BufPool"] = 13;
    defaultTick_["Pool"] = 13;
    defaultTick_["PoolBase"] = 13;
    defaultTick_["CplxEnzBase"] = 14;
    defaultTick_["Enz"] = 14;
    defaultTick_["EnzBase"] = 14;
    defaultTick_["MMenz"] = 14;
    defaultTick_["Reac"] = 14;
    defaultTick_["ReacBase"] = 14;
    defaultTick_["Gsolve"] = 16; // Note this uses an 'init' at t-1
    defaultTick_["Ksolve"] = 16; // Note this uses an 'init' at t-1
    defaultTick_["Stats"] = 17;

    defaultTick_["Table2"] = 18;
    defaultTick_["Streamer"] = 19;
    defaultTick_["HDF5DataWriter"] = 30;
    defaultTick_["HDF5WriterBase"] = 30;
    defaultTick_["NSDFWriter"] = 30;
    defaultTick_["PyRun"] = 30;

    defaultTick_["PostMaster"] = 31;

    defaultTick_["Annotator"] = ~0U;
    defaultTick_["ChemCompt"] = ~0U;
    defaultTick_["Cinfo"] = ~0U;
    defaultTick_["Clock"] = ~0U;
    defaultTick_["CubeMesh"] = ~0U;
    defaultTick_["CylMesh"] = ~0U;
    defaultTick_["DiagonalMsg"] = ~0U;
    defaultTick_["Double"] = ~0U;
    defaultTick_["Finfo"] = ~0U;
    defaultTick_["Group"] = ~0U;
    defaultTick_["HHGate"] = ~0U;
    defaultTick_["HHGate2D"] = ~0U;
    defaultTick_["Interpol2D"] = ~0U;
    defaultTick_["Long"] = ~0U;
    defaultTick_["MeshEntry"] = ~0U;
    defaultTick_["Msg"] = ~0U;
    defaultTick_["Mstring"] = ~0U;
    defaultTick_["Neuron"] = ~0U;
    defaultTick_["NeuroMesh"] = ~0U;
    defaultTick_["Neutral"] = ~0U;
    defaultTick_["OneToAllMsg"] = ~0U;
    defaultTick_["OneToOneDataIndexMsg"] = ~0U;
    defaultTick_["OneToOneMsg"] = ~0U;
    defaultTick_["PsdMesh"] = ~0U;
    defaultTick_["Shell"] = ~0U;
    defaultTick_["SingleMsg"] = ~0U;
    defaultTick_["SparseMsg"] = ~0U;
    defaultTick_["Species"] = ~0U;
    defaultTick_["Spine"] = ~0U;
    defaultTick_["SpineMesh"] = ~0U;
    defaultTick_["SteadyState"] = ~0U;
    defaultTick_["Stoich"] = ~0U;
    defaultTick_["Synapse"] = ~0U;
    defaultTick_["TableBase"] = ~0U;
    defaultTick_["Unsigned"] = ~0U;
    defaultTick_["Variable"] = ~0U;
    defaultTick_["VectorTable"] = ~0U;
    defaultTick_["ZombieBufPool"] = ~0U;
    defaultTick_["ZombieCaConc"] = ~0U;
    defaultTick_["ZombieCompartment"] = ~0U;
    defaultTick_["ZombieEnz"] = ~0U;
    // defaultTick_["ZombieFuncPool"] = ~0U;
    defaultTick_["ZombieFunction"] = ~0U;
    defaultTick_["ZombieHHChannel"] = ~0U;
    defaultTick_["ZombieMMenz"] = ~0U;
    defaultTick_["ZombiePool"] = ~0U;
    defaultTick_["ZombieReac"] = ~0U;

    defaultDt_.assign( Clock::numTicks, 0.0 );
    defaultDt_[0] = 50.0e-6;
    defaultDt_[1] = 50.0e-6;
    defaultDt_[2] = 50.0e-6;
    defaultDt_[3] = 50.0e-6;
    defaultDt_[4] = 50.0e-6;
    defaultDt_[5] = 50.0e-6;
    defaultDt_[6] = 50.0e-6;
    defaultDt_[7] = 50.0e-6;
    defaultDt_[8] = 1.0e-4;                     // For the tables for electrical calculations
    defaultDt_[9] = 0.0;                        // Not assigned
    defaultDt_[10] = 0.01;                      // For diffusion.
    defaultDt_[11] = 0.1;
    defaultDt_[12] = 0.1;
    defaultDt_[13] = 0.1;
    defaultDt_[14] = 0.1;
    defaultDt_[15] = 0.1;
    defaultDt_[16] = 0.1;
    defaultDt_[17] = 0.1;
    defaultDt_[18] = 1;                         // For tables for chemical calculations.
    defaultDt_[19] = 10;                        // For Streamer

    // 20-29 are not assigned.
    defaultDt_[30] = 1;	// For the HDF writer
    defaultDt_[31] = 0.01; // For the postmaster.
}

// Static function
unsigned int Clock::lookupDefaultTick( const string& className )
{
    map< string, unsigned int >::const_iterator i =
        defaultTick_.find( className );
    if ( i == defaultTick_.end() )
    {
        cout << "Warning: unknown className: '" << className << "'.\n" <<
             "Advisable to update the defaultTick table in the Clock class.\n";
        return 0;
    }
    return i->second;
}
