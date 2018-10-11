/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include <fstream>
#include "TableBase.h"
#include "StimulusTable.h"

static SrcFinfo1< double > *output() {
	static SrcFinfo1< double > output (
			"output",
			"Sends out tabulated data according to lookup parameters."
			);
	return &output;
}

const Cinfo* StimulusTable::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< StimulusTable, double > startTime(
			"startTime",
			"Start time used when table is emitting values. For lookup"
			"values below this, the table just sends out its zero entry."
			"Corresponds to zeroth entry of table.",
			&StimulusTable::setStartTime,
			&StimulusTable::getStartTime
		);

		static ValueFinfo< StimulusTable, double > stopTime(
			"stopTime",
			"Time to stop emitting values."
			"If time exceeds this, then the table sends out its last entry."
			"The stopTime corresponds to the last entry of table.",
			&StimulusTable::setStopTime,
			&StimulusTable::getStopTime
		);

		static ValueFinfo< StimulusTable, double > loopTime(
			"loopTime",
			"If looping, this is the time between successive cycle starts."
			"Defaults to the difference between stopTime and startTime, "
			"so that the output waveform cycles with precisely the same "
			"duration as the table contents."
			"If larger than stopTime - startTime, then it pauses at the "
			"last table value till it is time to "
			"go around again."
			"If smaller than stopTime - startTime, then it begins the next "
			"cycle even before the first one has reached the end of the "
			"table.",
			&StimulusTable::setLoopTime,
			&StimulusTable::getLoopTime
		);

		static ValueFinfo< StimulusTable, double > stepSize(
			"stepSize",
			"Increment in lookup (x) value on every timestep. If it is"
			"less than or equal to zero, the StimulusTable uses the current time"
			"as the lookup value.",
			&StimulusTable::setStepSize,
			&StimulusTable::getStepSize
		);

		static ValueFinfo< StimulusTable, double > stepPosition(
			"stepPosition",
			"Current value of lookup (x) value."
			"If stepSize is less than or equal to zero, this is set to"
			"the current time to use as the lookup value.",
			&StimulusTable::setStepPosition,
			&StimulusTable::getStepPosition
		);

		static ValueFinfo< StimulusTable, bool > doLoop(
			"doLoop",
			"Flag: Should it loop around to startTime once it has reached"
			"stopTime. Default (zero) is to do a single pass.",
			&StimulusTable::setDoLoop,
			&StimulusTable::getDoLoop
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////

		static DestFinfo process( "process",
			"Handles process call, updates internal time stamp.",
			new ProcOpFunc< StimulusTable >( &StimulusTable::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call.",
			new ProcOpFunc< StimulusTable >( &StimulusTable::reinit ) );
		//////////////////////////////////////////////////////////////
		// SharedMsg Definitions
		//////////////////////////////////////////////////////////////
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* stimulusTableFinfos[] = {
		&startTime,
		&stopTime,
		&loopTime,
		&stepSize,
		&stepPosition,
		&doLoop,
		output(),		// SrcFinfo
		&proc,			// SharedFinfo
	};

	static Dinfo< StimulusTable > dinfo;
	static Cinfo stimulusTableCinfo (
		"StimulusTable",
		TableBase::initCinfo(),
		stimulusTableFinfos,
		sizeof( stimulusTableFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &stimulusTableCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* stimulusTableCinfo = StimulusTable::initCinfo();

StimulusTable::StimulusTable()
	: start_( 0 ), stop_( 1 ), loopTime_( 1 ),
		stepSize_( 0 ), stepPosition_( 0 ),
	doLoop_( 0 )
{ ; }

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void StimulusTable::process( const Eref& e, ProcPtr p )
{
	if ( stepSize_ > 0 )
		stepPosition_ += stepSize_;
	else
		stepPosition_ = p->currTime;

	double lookupPosition = stepPosition_;
	if ( doLoop_ && ( stepPosition_ > start_ + loopTime_ ) ) {
		unsigned int i = floor( ( stepPosition_ - start_ ) / loopTime_ );
			lookupPosition = stepPosition_ - loopTime_ * i;
	}

	double y = interpolate( start_, stop_, lookupPosition );
	setOutputValue( y );

	output()->send( e, y );
}

void StimulusTable::reinit( const Eref& e, ProcPtr p )
{
	stepPosition_ = 0.0;
	double y = interpolate( start_, stop_, stepPosition_ );
	setOutputValue( y );
	output()->send( e, y );
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void StimulusTable::setStartTime( double v )
{
	start_ = v;
}

double StimulusTable::getStartTime() const
{
	return start_;
}

void StimulusTable::setStopTime( double v )
{
	if ( doLoop_ && doubleEq( loopTime_, stop_ - start_ ) )
		loopTime_ = v - start_;
	stop_ = v;
}

double StimulusTable::getStopTime() const
{
	return stop_;
}

void StimulusTable::setLoopTime( double v )
{
	if ( loopTime_ >= 0 )
		loopTime_ = v;
	else
		cout << "StimulusTable::setLoopTime: Warning: Cannot set to " <<
			v << " as this value is below zero. Left unchanged at " <<
			loopTime_ << "\n";
}

double StimulusTable::getLoopTime() const
{
	return loopTime_;
}

void StimulusTable::setStepSize( double v )
{
	stepSize_ = v;
}

double StimulusTable::getStepSize() const
{
	return stepSize_;
}

void StimulusTable::setStepPosition( double v )
{
	stepPosition_ = v;
}

double StimulusTable::getStepPosition() const
{
	return stepPosition_;
}

void StimulusTable::setDoLoop( bool v )
{
	doLoop_ = v;
	if ( loopTime_ == 0 )
		loopTime_ = stop_ - start_;
}

bool StimulusTable::getDoLoop() const
{
	return doLoop_;
}

