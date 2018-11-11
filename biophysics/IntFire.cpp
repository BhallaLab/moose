/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "../basecode/header.h"
#include "IntFire.h"

static SrcFinfo1< double > *spikeOut() {
	static SrcFinfo1< double > spikeOut(
			"spikeOut",
			"Sends out spike events. The argument is the timestamp of "
			"the spike. "
			);
	return &spikeOut;
}

const Cinfo* IntFire::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< IntFire, double > Vm(
			"Vm",
			"Membrane potential",
			&IntFire::setVm,
			&IntFire::getVm
		);

		static ValueFinfo< IntFire, double > tau(
			"tau",
			"charging time-course",
			&IntFire::setTau,
			&IntFire::getTau
		);

		static ValueFinfo< IntFire, double > thresh(
			"thresh",
			"firing threshold",
			&IntFire::setThresh,
			&IntFire::getThresh
		);

		static ValueFinfo< IntFire, double > refractoryPeriod(
			"refractoryPeriod",
			"Minimum time between successive spikes",
			&IntFire::setRefractoryPeriod,
			&IntFire::getRefractoryPeriod
		);
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo activation( "activation",
			"Handles value of synaptic activation arriving on this IntFire",
			new OpFunc1< IntFire, double >( &IntFire::activation ) );

		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< IntFire >( &IntFire::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< IntFire >( &IntFire::reinit ) );
		//////////////////////////////////////////////////////////////
		// SharedFinfo Definitions
		//////////////////////////////////////////////////////////////
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* intFireFinfos[] = {
		&Vm,	// Value
		&tau,	// Value
		&thresh,				// Value
		&refractoryPeriod,		// Value
		&activation,		// DestFinfo
		&proc,					// SharedFinfo
		spikeOut(), 		// MsgSrc
	};

	static Dinfo< IntFire > dinfo;
	static Cinfo intFireCinfo (
		"IntFire",
		Neutral::initCinfo(),
		intFireFinfos,
		sizeof( intFireFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &intFireCinfo;
}

static const Cinfo* intFireCinfo = IntFire::initCinfo();

IntFire::IntFire()
	: Vm_( 0.0 ), thresh_( 0.0 ), tau_( 1.0 ),
		refractoryPeriod_( 0.1 ), lastSpike_( -0.1 ),
		activation_( 0.0 )
{
	;
}

IntFire::IntFire( double thresh, double tau )
	: Vm_( 0.0 ), thresh_( thresh ), tau_( tau ), refractoryPeriod_( 0.1 ), lastSpike_( -1.0 ), activation_( 0.0 )
{
	;
}

void IntFire::process( const Eref &e, ProcPtr p )
{
	static bool report = false;
	static unsigned int reportIndex = 0;
	if ( report && e.dataIndex() == reportIndex )
		cout << "	" << p->currTime << "," << Vm_;
	Vm_ += activation_;
	activation_ = 0.0;

	if ( Vm_ > thresh_ && (p->currTime - lastSpike_) > refractoryPeriod_ ) {
		spikeOut()->send( e, p->currTime );
		Vm_ = -1.0e-7;
		lastSpike_ = p->currTime;
	} else {
		Vm_ *= ( 1.0 - p->dt / tau_ );
	}
}

void IntFire::reinit( const Eref& e, ProcPtr p )
{
	Vm_ = 0.0;
	activation_ = 0.0;
}

void IntFire::setVm( const double v )
{
	Vm_ = v;
}

void IntFire::setTau( const double v )
{
	tau_ = v;
}

void IntFire::setThresh( const double v )
{
	thresh_ = v;
}

void IntFire::setRefractoryPeriod( const double v )
{
	refractoryPeriod_ = v;
	lastSpike_ = -v;
}

double IntFire::getVm() const
{
	return Vm_;
}

double IntFire::getTau() const
{
	return tau_;
}

double IntFire::getThresh() const
{
	return thresh_;
}

double IntFire::getRefractoryPeriod() const
{
	return refractoryPeriod_;
}

void IntFire::activation( double v )
{
	activation_ += v;
}
