// PulseGen.cpp ---
//
// Filename: PulseGen.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Mon Feb 20 01:41:12 2012 (+0530)
// Version:
// Last-Updated: Mon May 27 11:49:31 2013 (+0530)
//           By: subha
//     Update #: 170
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
//
//
//

// Change log:
//
// 2012-02-20 01:41:16 (+0530) - Subha - started porting from old
// moose.
//

// Code:

#include "header.h"
#include "PulseGen.h"

static SrcFinfo1< double >* outputOut()
{
    static SrcFinfo1< double > outputOut( "output",
                                          "Current output level.");
    return &outputOut;
}

const Cinfo* PulseGen::initCinfo()
{
    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////
    static ReadOnlyValueFinfo< PulseGen, double > output("outputValue",
                                                 "Output amplitude",
                                                 &PulseGen::getOutput);
    static ValueFinfo< PulseGen, double > baseLevel("baseLevel",
                                                     "Basal level of the stimulus",
                                                     &PulseGen::setBaseLevel,
                                                     &PulseGen::getBaseLevel);
    static ValueFinfo< PulseGen, double > firstLevel("firstLevel",
                                                     "Amplitude of the first pulse in a sequence",
                                                     &PulseGen::setFirstLevel,
                                                     &PulseGen::getFirstLevel);
    static ValueFinfo< PulseGen, double > firstWidth("firstWidth",
                                                     "Width of the first pulse in a sequence",
                                                     &PulseGen::setFirstWidth,
                                                     &PulseGen::getFirstWidth);
    static ValueFinfo< PulseGen, double > firstDelay("firstDelay",
                                                     "Delay to start of the first pulse in a sequence",
                                                     &PulseGen::setFirstDelay,
                                                     &PulseGen::getFirstDelay);
    static ValueFinfo< PulseGen, double > secondLevel("secondLevel",
                                                     "Amplitude of the second pulse in a sequence",
                                                     &PulseGen::setSecondLevel,
                                                     &PulseGen::getSecondLevel);
    static ValueFinfo< PulseGen, double > secondWidth("secondWidth",
                                                     "Width of the second pulse in a sequence",
                                                     &PulseGen::setSecondWidth,
                                                     &PulseGen::getSecondWidth);
    static ValueFinfo< PulseGen, double > secondDelay("secondDelay",
                                                     "Delay to start of of the second pulse in a sequence",
                                                     &PulseGen::setSecondDelay,
                                                     &PulseGen::getSecondDelay);
    static ValueFinfo< PulseGen, unsigned int > count("count",
                                                     "Number of pulses in a sequence",
                                                     &PulseGen::setCount,
                                                     &PulseGen::getCount);
    static ValueFinfo< PulseGen, unsigned int > trigMode("trigMode",
                                                         "Trigger mode for pulses in the sequence.\n"
                                                         " 0 : free-running mode where it keeps looping its output\n"
                                                         " 1 : external trigger, where it is triggered by an external input (and"
                                                         " stops after creating the first train of pulses)\n"
                                                         " 2 : external gate mode, where it keeps generating the pulses in a"
                                                         " loop as long as the input is high.",
                                                     &PulseGen::setTrigMode,
                                                     &PulseGen::getTrigMode);
    static LookupValueFinfo < PulseGen, unsigned int, double > level("level",
                                                                     "Level of the pulse at specified index",
                                                                     &PulseGen::setLevel,
                                                                     &PulseGen::getLevel);
    static LookupValueFinfo < PulseGen, unsigned int, double > width("width",
                                                                     "Width of the pulse at specified index",
                                                                     &PulseGen::setWidth,
                                                                    &PulseGen::getWidth);
    static LookupValueFinfo < PulseGen, unsigned int, double > delay("delay",
                                                                    "Delay of the pulse at specified index",
                                                                    &PulseGen::setDelay,
                                                                    &PulseGen::getDelay);
    ///////////////////////////////////////////////////////////////
    // Dest messages
    ///////////////////////////////////////////////////////////////
    static DestFinfo levelIn("levelIn",
                             "Handle level value coming from other objects",
                             new OpFunc2< PulseGen, unsigned int, double >(&PulseGen::setLevel));
    static DestFinfo widthIn("widthIn",
                             "Handle width value coming from other objects",
                             new OpFunc2< PulseGen, unsigned int, double >(&PulseGen::setWidth));
    static DestFinfo delayIn("delayIn",
                             "Handle delay value coming from other objects",
                             new OpFunc2< PulseGen, unsigned int, double >(&PulseGen::setDelay));

    static DestFinfo input("input",
                           "Handle incoming input that determines gating/triggering onset. "
                           "Note that although this is a double field, the underlying field is"
                           " integer. So fractional part of input will be truncated",
                           new OpFunc1< PulseGen, double >(&PulseGen::input));
    ///////////////////////////////////////////////////////////////////
    // Shared messages
    ///////////////////////////////////////////////////////////////////
    static DestFinfo process( "process",
                              "Handles process call, updates internal time stamp.",
                              new ProcOpFunc< PulseGen >( &PulseGen::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles reinit call.",
                             new ProcOpFunc< PulseGen >( &PulseGen::reinit ) );
    static Finfo* processShared[] =
            {
		&process, &reinit
            };

    static SharedFinfo proc( "proc",
                             "This is a shared message to receive Process messages "
                             "from the scheduler objects."
                             "The first entry in the shared msg is a MsgDest "
                             "for the Process operation. It has a single argument, "
                             "ProcInfo, which holds lots of information about current "
                             "time, thread, dt and so on. The second entry is a MsgDest "
                             "for the Reinit operation. It also uses ProcInfo. ",
                             processShared, sizeof( processShared ) / sizeof( Finfo* )
                             );

    static Finfo* pulseGenFinfos[] = {
        &output,
        &baseLevel,
        &firstLevel,
        &firstWidth,
        &firstDelay,
        &secondLevel,
        &secondWidth,
        &secondDelay,
        &count,
        &trigMode,
        &level,
        &width,
        &delay,
        &input,
        outputOut(),
        &levelIn,
        &widthIn,
        &delayIn,
        &proc,
    };
    static string doc[] =
    {
        "Name", "PulseGen",
        "Author", "Subhasis Ray",
        "Description", "PulseGen: general purpose pulse generator. This can generate any "
        "number of pulses with specified level and duration.",
    };
    static Dinfo<PulseGen> dinfo;
    static Cinfo pulseGenCinfo("PulseGen",
                               Neutral::initCinfo(),
                               pulseGenFinfos,
                               sizeof(pulseGenFinfos)/sizeof(Finfo*),
                               &dinfo,
                               doc,
                               sizeof(doc)/sizeof(string));
    return & pulseGenCinfo;

}

static const Cinfo* pulseGenCinfo = PulseGen::initCinfo();

PulseGen::PulseGen()
{
    level_.reserve(2);
    width_.reserve(2);
    delay_.reserve(2);
    level_.resize(2);
    width_.resize(2);
    delay_.resize(2);
    level_.assign(2, 0.0);
    delay_.assign(2, 0.0);
    width_.assign(2, 0.0);
    output_ = 0.0;
    baseLevel_ = 0.0;
    trigTime_ = -1;
    trigMode_ = 0;
    prevInput_ = 0;
}

PulseGen::~PulseGen()

{
    ;
}

void PulseGen::setFirstLevel( double level)
{
    level_[0] = level;
}

double PulseGen::getFirstLevel() const
{
    return level_[0];
}

void PulseGen::setFirstWidth(double width)
{
    width_[0] = width;
}

double PulseGen::getFirstWidth() const
{
    return width_[0];
}
void PulseGen::setFirstDelay( double delay)
{
    delay_[0] = delay;
}
double PulseGen::getFirstDelay() const
{
    return delay_[0];
}

void PulseGen::setSecondLevel( double level)
{
    if (level_.size() >= 2){
        level_[1] = level;
    }
}

double PulseGen::getSecondLevel() const
{
    if (level_.size() >= 2){
        return level_[1];
    } else {
        return 0.0;
    }
}

void PulseGen::setSecondWidth(double width)
{
    if (width_.size() >= 2){
        width_[1] = width;
    }
}

double PulseGen::getSecondWidth() const
{
    if (width_.size() >= 2){
        return width_[1];
    } else {
        return 0.0;
    }
}
void PulseGen::setSecondDelay( double delay)
{
    if (delay_.size() >= 2){
        delay_[1] = delay;
    }
}
double PulseGen::getSecondDelay() const
{
    if (delay_.size() >= 2){
        return delay_[1];
    }
    return 0.0;
}

void PulseGen::setBaseLevel( double level)
{
    baseLevel_ = level;
}
double PulseGen::getBaseLevel() const
{
    return baseLevel_;
}
void PulseGen::setTrigMode(unsigned int mode)
{
    trigMode_ = mode;
}
unsigned int PulseGen::getTrigMode() const
{
    return trigMode_;
}
double PulseGen::getOutput() const
{
    return output_;
}

int PulseGen::getPreviousInput() const
{
    return prevInput_;
}

void PulseGen::setCount(unsigned int count)
{
    if (count <= 0){
        cout << "WARNING: invalid pulse count specified." << endl;
        return;
    }
    // we want to keep it compact - reserve will ensure we do not
    // overallocate in resize call
    level_.reserve(count);
    delay_.reserve(count);
    width_.reserve(count);
    level_.resize(count, 0.0);
    delay_.resize(count, 0.0);
    width_.resize(count, 0.0);
}

unsigned int PulseGen::getCount() const
{
    return level_.size();
}

double PulseGen::getLevel(unsigned int index) const
{
    if (index < level_.size()){
        return level_[index];
    } else {
        cout << "WARNING: PulseGen::getLevel - invalid index." << endl;
        return 0.0;
    }
}

void PulseGen::setLevel( unsigned int index, double level)
{
    if (index < level_.size()){
        level_[index] = level;
    } else {
        cout << "WARNING: PulseGen::setLevel - invalid index. First set the number of pulses by setting 'count' field." << endl;
    }
}

double PulseGen::getWidth(unsigned int index) const
{
    if (index < width_.size()){
        return width_[index];
    } else {
        cout << "WARNING: PulseGen::getWidth - invalid index." << endl;
        return 0.0;
    }
}
void PulseGen::setWidth(unsigned int index, double width)
{
    if ( index < width_.size()){
        width_[index] = width;
    } else {
        cout << "WARNING: PulseGen::setWidth - invalid index. First set the number of pulses by setting 'count' field." << endl;
    }
}
double PulseGen::getDelay(unsigned int index) const
{
    if ( index < delay_.size()){
        return delay_[index];
    } else {
        cout << "WARNING: PulseGen::getDelay - invalid index." << endl;
        return 0.0;
    }
}

void PulseGen::setDelay(unsigned int index, double delay)
{
    if (  index < delay_.size() ){
        delay_[index] = delay;
    } else {
        cout << "WARNING: PulseGen::setDelay - invalid index. First set the number of pulses by setting 'count' field." << endl;
    }
}


//////////////////////////////////////////////////////////////////
// Message dest functions.
//////////////////////////////////////////////////////////////////

void PulseGen::input( double value)
{
    input_ = value;
}


void PulseGen::process(const Eref& e, ProcPtr p )
{
    double currentTime = p->currTime;
    double period = width_[0] + delay_[0];
    double phase = 0.0;
    for (unsigned int ii = 1; ii < width_.size() && (width_[ii] > 0.0 || delay_[ii] > 0.0); ++ii){
        double incr = delay_[ii] + width_[ii] - width_[ii-1];
        if  (incr > 0){
            period += incr;
        }
    }
    switch (trigMode_){
        case PulseGen::FREE_RUN:
            phase = fmod(currentTime, period);
            break;
        case PulseGen::EXT_TRIG:
            if (input_ == 0){
                if (trigTime_ < 0){
                    phase = period;
                }else{
                    phase = currentTime - trigTime_;
                }
            } else {
                if (prevInput_ == 0){
                    trigTime_ = currentTime;
                }
                phase = currentTime - trigTime_;
            }
            prevInput_ = input_;
            break;
        case PulseGen::EXT_GATE:
            if(input_ == 0)
            {
                phase = period;		/* output = baselevel */
            }
            else
            {				/* gate high */
                if(prevInput_ == 0)
                {	/* low -> high */
                    trigTime_ = currentTime;
                }
                phase = fmod(currentTime - trigTime_, period);
            }
            prevInput_ = input_;
            break;
        default:
            cerr << "ERROR: PulseGen::newProcessFunc( const Conn* , ProcInfo ) - invalid triggerMode - " << trigMode_ << endl;
    }
    if (phase >= period){ // we have crossed all pulses
        output_ = baseLevel_;
        return;
    }
    // go through all pulse positions to check which pulse/interpulse
    // are we are in and set the output level accordingly
    for (unsigned int ii = 0; ii < width_.size(); ++ii){
        if (phase < delay_[ii]){ // we are in the baseline area - before ii-th pulse
            output_ = baseLevel_;
            break;
        } else if (phase < delay_[ii] + width_[ii]){ // we are inside th ii-th pulse
            output_ = level_[ii];
            break;
        }
        phase -= delay_[ii];
    }
    outputOut()->send(e, output_);
}


void PulseGen::reinit(const Eref& e, ProcPtr p)
{
    trigTime_ = -1;
    prevInput_ = 0;
    output_ = baseLevel_;
    input_ = 0;
    outputOut()->send(e, output_);
}

//
// PulseGen.cpp ends here
