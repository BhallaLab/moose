/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/global.h"
#include "../basecode/ElementValueFinfo.h"
#include "HSolveStruct.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"
#include "HSolve.h"
#include "../biophysics/Compartment.h"
#include "ZombieCompartment.h"
#include "../biophysics/CaConcBase.h"
#include "ZombieCaConc.h"
#include "../biophysics/HHGate.h"
#include "../biophysics/ChanBase.h"
#include "../biophysics/ChanCommon.h"
#include "../biophysics/HHChannel.h"
#include "../biophysics/CaConc.h"
#include "ZombieHHChannel.h"
#include "../shell/Shell.h"

#include <chrono>
using namespace std::chrono;

// defined in global.h 
extern map<string, double> solverProfMap;

const Cinfo* HSolve::initCinfo()
{
    static DestFinfo process(
        "process",
        "Handles 'process' call: Solver advances by one time-step.",
        new ProcOpFunc< HSolve >( &HSolve::process )
    );

    static DestFinfo reinit(
        "reinit",
        "Handles 'reinit' call: Solver reads in model.",
        new ProcOpFunc< HSolve >( &HSolve::reinit )
    );

    static Finfo* processShared[] =
    {
        &process,
        &reinit
    };

    static SharedFinfo proc(
        "proc",
        "Handles 'reinit' and 'process' calls from a clock.",
        processShared,
        sizeof( processShared ) / sizeof( Finfo* )
    );

    static ValueFinfo< HSolve, Id > seed(
        "seed",
        "Use this field to specify path to a 'seed' compartment, that is, "
        "any compartment within a neuron. The HSolve object uses this seed as "
        "a handle to discover the rest of the neuronal model, which means all "
        "the remaining compartments, channels, synapses, etc.",
        &HSolve::setSeed,
        &HSolve::getSeed
    );

    static ElementValueFinfo< HSolve, string > target(
        "target",
        "Specifies the path to a compartmental model to be taken over. "
        "This can be the path to any container object that has the model "
        "under it (found by performing a deep search). Alternatively, this "
        "can also be the path to any compartment within the neuron. This "
        "compartment will be used as a handle to discover the rest of the "
        "model, which means all the remaining compartments, channels, "
        "synapses, etc.",
        &HSolve::setPath,
        &HSolve::getPath
    );

    static ValueFinfo< HSolve, double > dt(
        "dt",
        "The time-step for this solver.",
        &HSolve::setDt,
        &HSolve::getDt
    );

    static ValueFinfo< HSolve, int > caAdvance(
        "caAdvance",
        "This flag determines how current flowing into a calcium pool "
        "is computed. A value of 0 means that the membrane potential at the "
        "beginning of the time-step is used for the calculation. This is how "
        "GENESIS does its computations. A value of 1 means the membrane "
        "potential at the middle of the time-step is used. This is the "
        "correct way of integration, and is the default way.",
        &HSolve::setCaAdvance,
        &HSolve::getCaAdvance
    );

    static ValueFinfo< HSolve, int > vDiv(
        "vDiv",
        "Specifies number of divisions for lookup tables of voltage-sensitive "
        "channels.",
        &HSolve::setVDiv,
        &HSolve::getVDiv
    );

    static ValueFinfo< HSolve, double > vMin(
        "vMin",
        "Specifies the lower bound for lookup tables of voltage-sensitive "
        "channels. Default is to automatically decide based on the tables of "
        "the channels that the solver reads in.",
        &HSolve::setVMin,
        &HSolve::getVMin
    );

    static ValueFinfo< HSolve, double > vMax(
        "vMax",
        "Specifies the upper bound for lookup tables of voltage-sensitive "
        "channels. Default is to automatically decide based on the tables of "
        "the channels that the solver reads in.",
        &HSolve::setVMax,
        &HSolve::getVMax
    );

    static ValueFinfo< HSolve, int > caDiv(
        "caDiv",
        "Specifies number of divisions for lookup tables of calcium-sensitive "
        "channels.",
        &HSolve::setCaDiv,
        &HSolve::getCaDiv
    );

    static ValueFinfo< HSolve, double > caMin(
        "caMin",
        "Specifies the lower bound for lookup tables of calcium-sensitive "
        "channels. Default is to automatically decide based on the tables of "
        "the channels that the solver reads in.",
        &HSolve::setCaMin,
        &HSolve::getCaMin
    );

    static ValueFinfo< HSolve, double > caMax(
        "caMax",
        "Specifies the upper bound for lookup tables of calcium-sensitive "
        "channels. Default is to automatically decide based on the tables of "
        "the channels that the solver reads in.",
        &HSolve::setCaMax,
        &HSolve::getCaMax
    );

    static Finfo* hsolveFinfos[] =
    {
        &seed,              // Value
        &target,              // Value
        &dt,                // Value
        &caAdvance,         // Value
        &vDiv,              // Value
        &vMin,              // Value
        &vMax,              // Value
        &caDiv,             // Value
        &caMin,             // Value
        &caMax,             // Value
        &proc,              // Shared
    };

    static string doc[] =
    {
        "Name",             "HSolve",
        "Author",           "Niraj Dudani, 2007, NCBS",
        "Description",      "HSolve: Hines solver, for solving "
        "branching neuron models.",
    };

    static Dinfo< HSolve > dinfo;
    static Cinfo hsolveCinfo(
        "HSolve",
        Neutral::initCinfo(),
        hsolveFinfos,
        sizeof( hsolveFinfos ) / sizeof( Finfo* ),
		&dinfo,
        doc,
        sizeof(doc)/sizeof(string)
    );

    return &hsolveCinfo;
}

static const Cinfo* hsolveCinfo = HSolve::initCinfo();

HSolve::HSolve()
    : dt_( 50e-6 )
{
}

HSolve::~HSolve()
{
    unzombify();
#if 0
    char* p = getenv( "MOOSE_SHOW_SOLVER_PERF" );
    if( p != NULL )
    {
        cout << "Info: HSolve took " << totalTime_ << " seconds and took " << numSteps_
             << " steps." << endl;

    }
#endif
}


///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void HSolve::process( const Eref& hsolve, ProcPtr p )
{
    t0_ = high_resolution_clock::now();
    this->HSolveActive::step( p );
    t1_ = high_resolution_clock::now();
    addSolverProf( "HSolve", duration_cast<duration<double>>(t1_ - t0_).count(), 1 );
}

void HSolve::reinit( const Eref& hsolve, ProcPtr p )
{
    dt_ = p->dt;
    this->HSolveActive::reinit( p );
}

void HSolve::zombify( Eref hsolve ) const
{
    vector< Id >::const_iterator i;
	vector< ObjId > temp;

    for ( i = compartmentId_.begin(); i != compartmentId_.end(); ++i )
		temp.push_back( ObjId( *i, 0 ) );
    for ( i = compartmentId_.begin(); i != compartmentId_.end(); ++i ) {
        CompartmentBase::zombify( i->eref().element(),
					   ZombieCompartment::initCinfo(), hsolve.id() );
	}

	temp.clear();
    for ( i = caConcId_.begin(); i != caConcId_.end(); ++i )
		temp.push_back( ObjId( *i, 0 ) );
	// Shell::dropClockMsgs( temp, "process" );
    for ( i = caConcId_.begin(); i != caConcId_.end(); ++i ) {
        CaConcBase::zombify( i->eref().element(), ZombieCaConc::initCinfo(), hsolve.id() );
	}

	temp.clear();
    for ( i = channelId_.begin(); i != channelId_.end(); ++i )
		temp.push_back( ObjId( *i, 0 ) );
    for ( i = channelId_.begin(); i != channelId_.end(); ++i ) {
        HHChannelBase::zombify( i->eref().element(),
						ZombieHHChannel::initCinfo(), hsolve.id() );
	}
}

void HSolve::unzombify() const
{
    vector< Id >::const_iterator i;

    for ( i = compartmentId_.begin(); i != compartmentId_.end(); ++i )
		if ( i->element() ) {
        	CompartmentBase::zombify( i->eref().element(),
					   Compartment::initCinfo(), Id() );
		}

    for ( i = caConcId_.begin(); i != caConcId_.end(); ++i )
		if ( i->element() ) {
        	CaConcBase::zombify( i->eref().element(), CaConc::initCinfo(), Id() );
		}

    for ( i = channelId_.begin(); i != channelId_.end(); ++i )
		if ( i->element() ) {
        	HHChannelBase::zombify( i->eref().element(),
						HHChannel::initCinfo(), Id() );
		}
}

void HSolve::setup( Eref hsolve )
{
    // Setup solver.
    this->HSolveActive::setup( seed_, dt_ );

    mapIds();
    zombify( hsolve );
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void HSolve::setSeed( Id seed )
{
    if ( !seed.element()->cinfo()->isA( "Compartment" ) )
    {
        cerr << "Error: HSolve::setSeed(): Seed object '" << seed.path()
             << "' is not derived from type 'Compartment'." << endl;
        return;
    }
    seed_ = seed;
}

Id HSolve::getSeed() const
{
    return seed_;
}

void HSolve::setPath( const Eref& hsolve, string path )
{
    if ( dt_ == 0.0 )
    {
        cerr << "Error: HSolve::setPath(): Must set 'dt' first.\n";
        return;
    }

    seed_ = deepSearchForCompartment( Id( path ) );

    if ( seed_ == Id() )
        cerr << "Warning: HSolve::setPath(): No compartments found at or below '"
             << path << "'.\n";
    else
    {
        // cout << "HSolve: Seed compartment found at '" << seed_.path() << "'.\n";
        path_ = path;
        setup( hsolve );
    }
}

string HSolve::getPath( const Eref& e) const
{
    return path_;
}

/**
 * This function performs a depth-first search (for a compartment) in the tree
 * with its root at 'base'. Returns (Id of) a compartment if found, else a
 * blank Id.
 */
Id HSolve::deepSearchForCompartment( Id base )
{
    /*
     * 'cstack' is a stack-of-stacks used to perform the depth-first search.
     *     The 0th entry in 'cstack' is a stack containing simply the base.
     *     The i-th entry in 'cstack' contains children of the node at the top
     *         of the stack at position ( i - 1 ).
     *     Hence, at any time, the top of the i-th stack is the i-th node on
     *     the ancestral path from the 'base' node to the 'current' node
     *     (more below) which is being examined. Also, the remaining nodes in
     *     the i-th stack are the siblings of this ancestor.
     *
     * 'current' is the node at the top of the top of 'cstack'. If this node is
     *     a Compartment, then the search is completed, returning 'current'.
     *     Otherwise, the children of 'current' are pushed onto 'cstack' for a
     *     deeper search. If the deeper search yields nothing, then this
     *     'current' node is discarded. When an entire stack of siblings is
     *     exhausted in this way, then this empty stack is discarded, and
     *     the search moves 1 level up.
     *
     * 'result' is a blank Id (moose root element) if the search failed.
     *     Otherwise, it is a compartment that was found under 'base'.
     */
    vector< vector< Id > > cstack( 1, vector< Id >( 1, base ) );
    Id current;
    Id result;

    while ( !cstack.empty() )
        if ( cstack.back().empty() )
        {
            cstack.pop_back();

            if ( !cstack.empty() )
                cstack.back().pop_back();
        }
        else
        {
            current = cstack.back().back();

            // if ( current()->cinfo() == moose::Compartment::initCinfo() )
            // Compartment is base class for SymCompartment.
            if ( current.element()->cinfo()->isA( "Compartment" ) )
            {
                result = current;
                break;
            }
            cstack.push_back( children( current ) );
        }

    return result;
}

vector< Id > HSolve::children( Id obj )
{
    //~ return Field< vector< Id > >::get( obj, "children" );
    //~ return Field< vector< Id > >::fastGet( obj.eref(), "children" );
    //~ return localGet< Neutral, vector< Id > >( obj.eref(), "children" );

    vector< Id > c;
    Neutral::children( obj.eref(), c );
    return c;
}

void HSolve::setDt( double dt )
{
    if ( dt < 0.0 )
    {
        cerr << "Error: HSolve: 'dt' must be positive.\n";
        return;
    }

    dt_ = dt;
}

double HSolve::getDt() const
{
    return dt_;
}

void HSolve::setCaAdvance( int caAdvance )
{
    if ( caAdvance != 0 && caAdvance != 1 )
    {
        cerr << "Error: HSolve: caAdvance should be either 0 or 1.\n";
        return;
    }

    caAdvance_ = caAdvance;
}

int HSolve::getCaAdvance() const
{
    return caAdvance_;
}

void HSolve::setVDiv( int vDiv )
{
    vDiv_ = vDiv;
}

int HSolve::getVDiv() const
{
    return vDiv_;
}

void HSolve::setVMin( double vMin )
{
    vMin_ = vMin;
}

double HSolve::getVMin() const
{
    return vMin_;
}

void HSolve::setVMax( double vMax )
{
    vMax_ = vMax;
}

double HSolve::getVMax() const
{
    return vMax_;
}

void HSolve::setCaDiv( int caDiv )
{
    caDiv_ = caDiv;
}

int HSolve::getCaDiv() const
{
    return caDiv_;
}

void HSolve::setCaMin( double caMin )
{
    caMin_ = caMin;
}

double HSolve::getCaMin() const
{
    return caMin_;
}

void HSolve::setCaMax( double caMax )
{
    caMax_ = caMax;
}

double HSolve::getCaMax() const
{
    return caMax_;
}

const set<string>& HSolve::handledClasses()
{
    static set<string> classes;
    if (classes.empty())
    {
        classes.insert("CaConc");
        classes.insert("ZombieCaConc");
        classes.insert("HHChannel");
        classes.insert("ZombieHHChannel");
        classes.insert("Compartment");
        classes.insert("SymCompartment");
        classes.insert("ZombieCompartment");
    }
    return classes;
}

/**
   Utility function to delete incoming messages on orig.
   To be used in zombifying elements.
*/
void HSolve::deleteIncomingMessages( Element * orig, const string finfo)
{
    const DestFinfo * concenDest = dynamic_cast<const DestFinfo*>(orig->cinfo()->findFinfo(finfo));
    assert(concenDest);
    ObjId mid = orig->findCaller(concenDest->getFid());
    while (! mid.bad())
    {
        const Msg * msg = Msg::getMsg(mid);
        assert(msg);
        ObjId other = msg->findOtherEnd(orig->id());
        Element * otherEl = other.id.element();
        if (otherEl &&  HSolve::handledClasses().find(otherEl->cinfo()->name()) != HSolve::handledClasses().end())
        {
            Msg::deleteMsg(mid);
        }
        else
        {
            break; // Have to do this otherwise it is an infinite loop
        }
        mid = orig->findCaller(concenDest->getFid());
    }
}

#if 0

/// crate test object and push it into the container vector
Id create_testobject(vector<Id> & container, string classname, Id parent, string name, vector<int> dims)
{
    Shell * shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    Id nid = shell->doCreate( classname, parent, name, dims );
    container.push_back(nid);
    return nid;
}

void clear_testobjects(vector<Id>& container)
{
    Shell * shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    while (!container.empty())
    {
        Id id = container.back();
        shell->doDelete(id);
        container.pop_back();
    }
}


void testHSolvePassiveSingleComp()
{
    Shell * shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    vector< int > dims( 1, 1 );
    vector<Id> to_cleanup;
    Id nid = create_testobject(to_cleanup, "Neuron", Id(), "n", dims );
    Id comptId = create_testobject(to_cleanup, "Compartment", nid, "compt", dims );
    Id hsolve = create_testobject(to_cleanup, "HSolve", nid, "solver", dims);
    Field<string>::set(hsolve, "target", nid.path());
    to_cleanup.push_back(nid);
    clear_testobjects(to_cleanup);
    cout << "." << flush;
}



#endif // if 0
