/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "../biophysics/CompartmentBase.h"
#include "../biophysics/Compartment.h"
#include "IntFireBase.h"

using namespace moose;
SrcFinfo1< double >* IntFireBase::spikeOut()
{
    static SrcFinfo1< double > spikeOut(
        "spikeOut",
        "Sends out spike events. The argument is the timestamp of "
        "the spike. "
    );
    return &spikeOut;
}

const Cinfo* IntFireBase::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////
    static ElementValueFinfo< IntFireBase, double > thresh(
        "thresh",
        "firing threshold",
        &IntFireBase::setThresh,
        &IntFireBase::getThresh
    );

    static ElementValueFinfo< IntFireBase, double > vReset(
        "vReset",
        "voltage is set to vReset after firing",
        &IntFireBase::setVReset,
        &IntFireBase::getVReset
    );

    static ElementValueFinfo< IntFireBase, double > refractoryPeriod(
        "refractoryPeriod",
        "Minimum time between successive spikes",
        &IntFireBase::setRefractoryPeriod,
        &IntFireBase::getRefractoryPeriod
    );

    static ReadOnlyElementValueFinfo< IntFireBase, double > lastEventTime(
        "lastEventTime",
        "Timestamp of last firing.",
        &IntFireBase::getLastEventTime
    );

    static ReadOnlyElementValueFinfo< IntFireBase, bool > hasFired(
        "hasFired",
        "The object has fired within the last timestep",
        &IntFireBase::hasFired
    );
    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////
    static DestFinfo activation(
        "activation",
        "Handles value of synaptic activation arriving on this object",
        new OpFunc1< IntFireBase, double >( &IntFireBase::activation ));

    //////////////////////////////////////////////////////////////

    static Finfo* intFireFinfos[] =
    {
        &thresh,				// Value
        &vReset,				// Value
        &refractoryPeriod,		// Value
        &hasFired,				// ReadOnlyValue
        &lastEventTime,			// ReadOnlyValue
        &activation,			// DestFinfo
        IntFireBase::spikeOut() // MsgSrc
    };

    static string doc[] =
    {
        "Name", "IntFireBase",
        "Author", "Upi Bhalla",
        "Description", "Base class for Integrate-and-fire compartment.",
    };
    static ZeroSizeDinfo< int > dinfo;
    static Cinfo intFireBaseCinfo(
        "IntFireBase",
        Compartment::initCinfo(),
        intFireFinfos,
        sizeof( intFireFinfos ) / sizeof (Finfo*),
        &dinfo,
        doc,
        sizeof(doc)/sizeof(string)
    );

    return &intFireBaseCinfo;
}

static const Cinfo* intFireBaseCinfo = IntFireBase::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the Compartment class functions.
//////////////////////////////////////////////////////////////////

IntFireBase::IntFireBase()
    :
    threshold_( 0.0 ),
    vReset_( 0.0 ),
    activation_( 0.0 ),
    refractT_( 0.0 ),
    lastEvent_( 0.0 ),
    fired_( false )
{;}

IntFireBase::~IntFireBase()
{
    ;
}

// Value Field access function definitions.
void IntFireBase::setThresh( const Eref& e, double val )
{
    threshold_ = val;
}

double IntFireBase::getThresh( const Eref& e ) const
{
    return threshold_;
}

void IntFireBase::setVReset( const Eref& e, double val )
{
    vReset_ = val;
}

double IntFireBase::getVReset( const Eref& e ) const
{
    return vReset_;
}

void IntFireBase::setRefractoryPeriod( const Eref& e, double val )
{
    refractT_ = val;
}

double IntFireBase::getRefractoryPeriod( const Eref& e ) const
{
    return refractT_;
}

double IntFireBase::getLastEventTime( const Eref& e ) const
{
    return lastEvent_;
}

bool IntFireBase::hasFired( const Eref& e ) const
{
    return fired_;
}

//////////////////////////////////////////////////////////////////
// IntFireBase::Dest function definitions.
//////////////////////////////////////////////////////////////////

void IntFireBase::activation( double v )
{
    activation_ += v;
}
