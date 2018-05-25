// DiffAmp.cpp ---
//
// Filename: DiffAmp.cpp
// Description:
// Author: subhasis ray
// Maintainer:
// Created: Mon Dec 29 16:01:22 2008 (+0530)
// Version:
// Last-Updated: Tue Jun 11 17:00:33 2013 (+0530)
//           By: subha
//     Update #: 290
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
// This implements an equivalent of the diffamp object in GENESIS.
//
//
//

// Change log:
// 2008-12-30 16:21:19 (+0530) - Initial version.
// 2012-02-22 03:05:40 (+0530) - Ported to dh_branch
//
/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Messaging Object Oriented Simulation Environment,
 ** also known as GENESIS 3 base code.
 **           copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

// Code:

#include <cfloat>

#include "DiffAmp.h"

static SrcFinfo1< double >* outputOut()
{
    static SrcFinfo1< double > outputOut( "output",
                                          "Current output level.");
    return &outputOut;
}

const Cinfo* DiffAmp::initCinfo()
{
    static ValueFinfo<DiffAmp, double> gain( "gain",
                                             "Gain of the amplifier. The output of the amplifier is the difference"
                                             " between the totals in plus and minus inputs multiplied by the"
                                             " gain. Defaults to 1" ,
                                             &DiffAmp::setGain,
                                             &DiffAmp::getGain);
    static ValueFinfo<DiffAmp, double > saturation( "saturation",
                                                    "Saturation is the bound on the output. If output goes beyond the +/-"
                                                    "saturation range, it is truncated to the closer of +saturation and"
                                                    " -saturation. Defaults to the maximum double precision floating point"
                                                    " number representable on the system." ,
                                                    &DiffAmp::setSaturation,
                                                    &DiffAmp::getSaturation);

    static ReadOnlyValueFinfo<DiffAmp, double> output( "outputValue",
                                               "Output of the amplifier, i.e. gain * (plus - minus)." ,
                                               &DiffAmp::getOutput);
    ///////////////////////////////////////////////////////////////
    // Dest messages
    ///////////////////////////////////////////////////////////////

    static DestFinfo gainIn( "gainIn",
                             "Destination message to control gain dynamically.",
                             new OpFunc1<DiffAmp, double> (&DiffAmp::setGain));

    static DestFinfo plusIn( "plusIn",
                      "Positive input terminal of the amplifier. All the messages connected"
                      " here are summed up to get total positive input.",
                      new OpFunc1<DiffAmp, double> (&DiffAmp::plusFunc));

    static DestFinfo minusIn( "minusIn",
                      "Negative input terminal of the amplifier. All the messages connected"
                      " here are summed up to get total positive input.",
                      new OpFunc1<DiffAmp, double> (&DiffAmp::minusFunc));
    ///////////////////////////////////////////////////////////////////
    // Shared messages
    ///////////////////////////////////////////////////////////////////
    static DestFinfo process( "process",
                              "Handles process call, updates internal time stamp.",
                              new ProcOpFunc< DiffAmp >( &DiffAmp::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles reinit call.",
                             new ProcOpFunc< DiffAmp >( &DiffAmp::reinit ) );
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


    static Finfo * diffAmpFinfos[] = {
        &gain,
        &saturation,
        &output,
        &gainIn,
        &plusIn,
        &minusIn,
        outputOut(),
        &proc
    };
    static string doc[] = {
        "Name", "DiffAmp",
        "Author", "Subhasis Ray, 2008, NCBS",
        "Description", "A difference amplifier. "
        "Output is the difference between the total plus inputs and the total "
        "minus inputs multiplied by gain. Gain can be set statically as a field"
        " or can be a destination message and thus dynamically determined by the"
        " output of another object. Same as GENESIS diffamp object."
    };
    static Dinfo<DiffAmp> dinfo;
    static Cinfo diffAmpCinfo(
            "DiffAmp",
            Neutral::initCinfo(),
            diffAmpFinfos,
            sizeof(diffAmpFinfos)/sizeof(Finfo*),
            &dinfo,
            doc,
            sizeof(doc)/sizeof(string)
);

    return &diffAmpCinfo;
}

static const Cinfo* diffAmpCinfo = DiffAmp::initCinfo();
DiffAmp::DiffAmp():gain_(1.0), saturation_(DBL_MAX), plus_(0), minus_(0), output_(0)
{
}
DiffAmp::~DiffAmp(){
    ;
}
void DiffAmp::plusFunc(double input)
{
    plus_ += input;
}

void DiffAmp::minusFunc(double input)
{
    minus_ += input;
}

void DiffAmp::setGain(double gain)
{
    gain_ = gain;
}

void DiffAmp::setSaturation(double saturation)
{
    saturation_ = saturation;
}

double DiffAmp::getGain() const
{
    return gain_;
}

double DiffAmp::getSaturation() const
{
    return saturation_;
}

double DiffAmp::getOutput() const
{
    return output_;
}

void DiffAmp::process(const Eref& e, ProcPtr p)
{
    double output = gain_ * (plus_ - minus_);
    plus_ = 0.0;
    minus_ = 0.0;
    if ( output > saturation_ ) {
	output = saturation_;
    }
    if ( output < -saturation_ ) {
	output = -saturation_;
    }
    output_ = output;
    outputOut()->send(e, output_);
}

void DiffAmp::reinit(const Eref& e, ProcPtr p)
{
    // What is the right thing to do?? Should we actually do a process step??
    output_ = 0.0;
    plus_ = 0.0;
    minus_ = 0.0;
    outputOut()->send(e, output_);
}

//
// DiffAmp.cpp ends here
