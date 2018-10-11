/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "Stats.h"
#include "SpikeStats.h"

const Cinfo* SpikeStats::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< SpikeStats, double > threshold(
			"threshold",
			"Spiking threshold. If Vm crosses this going up then the "
		   "SpikeStats object considers that a spike has happened and "
		   "adds it to the stats.",
			&SpikeStats::setThreshold,
			&SpikeStats::getThreshold
		);
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////

		static DestFinfo addSpike( "addSpike",
			"Handles spike event time input, converts into a rate "
			"to do stats upon.",
			new OpFunc1< SpikeStats, double >( &SpikeStats::addSpike ) );

		static DestFinfo Vm( "Vm",
			"Handles continuous voltage input, can be coming in much "
			"than update rate of the SpikeStats. Looks for transitions "
			"above threshold to register the arrival of a spike. "
			"Doesn't do another spike till Vm falls below threshold. ",
			new OpFunc1< SpikeStats, double >( &SpikeStats::Vm ) );

	static Finfo* statsFinfos[] = {
		&threshold,	// Value
		&addSpike,	// DestFinfo
		&Vm,	// DestFinfo
	};

	static string doc[] =
	{
		"Name", "SpikeStats",
		"Author", "Upi Bhalla Aug 2014",
		"Description",
		"Object to do some minimal stats on rate of a spike train. "
		"Derived from the Stats object and returns the same set of stats."
		"Can take either predigested spike event input, or can handle "
		"a continuous sampling of membrane potential Vm and decide if "
		"a spike has occured based on a threshold. "
	};

	static Dinfo< SpikeStats > dinfo;
	static Cinfo spikeStatsCinfo (
		"SpikeStats",
		Stats::initCinfo(),
		statsFinfos,
		sizeof( statsFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &spikeStatsCinfo;
}

static const Cinfo* spikeStatsCinfo = SpikeStats::initCinfo();

///////////////////////////////////////////////////////////////////////////
// class funcs
///////////////////////////////////////////////////////////////////////////

SpikeStats::SpikeStats()
	: Stats(),
		numSpikes_( 0 ), threshold_( 0.0 ), fired_( false )
{ ; }

///////////////////////////////////////////////////////////////////////////
// Process stuff.
///////////////////////////////////////////////////////////////////////////

void SpikeStats::vProcess( const Eref& e, ProcPtr p )
{
	double rate = static_cast< double >( numSpikes_ ) / p->dt;
	numSpikes_ = 0;
	Stats::input( rate );
}

void SpikeStats::vReinit( const Eref& e, ProcPtr p )
{
	Stats::vReinit( e, p );
	numSpikes_ = 0;
	fired_ = 0;
}
///////////////////////////////////////////////////////////////////////////
// DestFinfos
///////////////////////////////////////////////////////////////////////////

void SpikeStats::Vm( double v )
{
	if ( fired_ ) { // Wait for it to go below threshold
		if ( v < threshold_ )
			fired_ = false;
	} else {
		if ( v > threshold_ ) { // wait for it to go above threshold.
			fired_ = true;
			numSpikes_++;
		}
	}
}

void SpikeStats::addSpike( double t )
{
	numSpikes_ ++;
}

///////////////////////////////////////////////////////////////////////////
// Fields
///////////////////////////////////////////////////////////////////////////

double SpikeStats::getThreshold() const
{
	return threshold_;
}

void SpikeStats::setThreshold( double v )
{
	threshold_ = v;
}
