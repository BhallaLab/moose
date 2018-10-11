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

static SrcFinfo1< vector< double >* > *requestOut() {
	static SrcFinfo1< vector< double >* > requestOut(
			"requestOut",
			"Sends request for a field to target object"
			);
	return &requestOut;
}

const Cinfo* Stats::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ReadOnlyValueFinfo< Stats, double > mean(
			"mean",
			"Mean of all sampled values or of spike rate.",
			&Stats::getMean
		);
		static ReadOnlyValueFinfo< Stats, double > sdev(
			"sdev",
			"Standard Deviation of all sampled values, or of rate.",
			&Stats::getSdev
		);
		static ReadOnlyValueFinfo< Stats, double > sum(
			"sum",
			"Sum of all sampled values, or total number of spikes.",
			&Stats::getSum
		);
		static ReadOnlyValueFinfo< Stats, unsigned int > num(
			"num",
			"Number of all sampled values, or total number of spikes.",
			&Stats::getNum
		);
		static ValueFinfo< Stats, unsigned int > windowLength(
			"windowLength",
			"Number of bins for windowed stats. "
			"Ignores windowing if this value is zero. ",
			&Stats::setWindowLength,
			&Stats::getWindowLength
		);
		static ReadOnlyValueFinfo< Stats, double > wmean(
			"wmean",
			"Mean of sampled values or of spike rate within window.",
			&Stats::getWmean
		);
		static ReadOnlyValueFinfo< Stats, double > wsdev(
			"wsdev",
			"Standard Deviation of sampled values, or rate, within window.",
			&Stats::getWsdev
		);
		static ReadOnlyValueFinfo< Stats, double > wsum(
			"wsum",
			"Sum of all sampled values, or total number of spikes, within window.",
			&Stats::getWsum
		);
		static ReadOnlyValueFinfo< Stats, unsigned int > wnum(
			"wnum",
			"Number of all sampled values, or total number of spikes, "
			"within window.",
			&Stats::getWnum
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< Stats >( &Stats::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< Stats >( &Stats::reinit ) );

		static DestFinfo input( "input",
			"Handles continuous value input as a time-series. "
			"Multiple inputs are allowed, they will be merged. ",
			new OpFunc1< Stats, double >( &Stats::input ) );

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

	static Finfo* statsFinfos[] = {
		&mean,	// ReadOnlyValue
		&sdev,	// ReadOnlyValue
		&sum,	// ReadOnlyValue
		&num,	// ReadOnlyValue
		&wmean,	// ReadOnlyValue
		&wsdev,	// ReadOnlyValue
		&wsum,	// ReadOnlyValue
		&wnum,	// ReadOnlyValue
		&windowLength,	// Value
		&input,		// DestFinfo
		requestOut(),		// SrcFinfo
		&proc		// SharedFinfo
	};

	static Dinfo< Stats > dinfo;
	static Cinfo statsCinfo (
		"Stats",
		Neutral::initCinfo(),
		statsFinfos,
		sizeof( statsFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &statsCinfo;
}

static const Cinfo* statsCinfo = Stats::initCinfo();

///////////////////////////////////////////////////////////////////////////
// Inner class funcs
///////////////////////////////////////////////////////////////////////////

Stats::Stats()
	:
	mean_( 0.0 ), sdev_( 0.0 ), sum_( 0.0 ), num_( 0 ),
	wmean_( 0.0 ), wsdev_( 0.0 ), wsum_( 0.0 ), wnum_( 0 ),
	sumsq_( 0.0 ), isWindowDirty_( true )
{
	;
}

///////////////////////////////////////////////////////////////////////////
// Process stuff.
///////////////////////////////////////////////////////////////////////////

void Stats::process( const Eref& e, ProcPtr p )
{
	this->vProcess( e, p );
}

void Stats::vProcess( const Eref& e, ProcPtr p )
{
	vector< double > v;
	requestOut()->send( e, &v );
	for ( vector< double >::const_iterator
					i = v.begin(); i != v.end(); ++i )
		input( *i );
}

void Stats::reinit( const Eref& e, ProcPtr p )
{
	this->vReinit( e, p );
}

void Stats::vReinit( const Eref& e, ProcPtr p )
{
	mean_ = 0.0;
	sdev_ = 0.0;
	sum_ = 0.0;
	num_ = 0;
	sumsq_ = 0.0;
	wmean_ = 0.0;
	wsdev_ = 0.0;
	wsum_ = 0.0;
	wnum_ = 0;
	samples_.assign( samples_.size(), 0.0 );
}
///////////////////////////////////////////////////////////////////////////
// DestFinfos
///////////////////////////////////////////////////////////////////////////

void Stats::input( double v )
{
	sum_ += v;
	sumsq_ += v * v;
	if ( samples_.size() > 0 )
		samples_[ num_ % samples_.size() ] = v;
	++num_;
	isWindowDirty_ = true;
}

///////////////////////////////////////////////////////////////////////////
// Fields
///////////////////////////////////////////////////////////////////////////

double Stats::getMean() const
{
	if ( num_ > 0 )
		return sum_ / num_;
	return 0.0;
}

double Stats::getSdev() const
{
	if ( num_ > 0 )
		return sqrt( ( sumsq_ - sum_ * sum_ / num_ ) / num_ );
	return 0.0;
}

double Stats::getSum() const
{
	return sum_;
}

unsigned int Stats::getNum() const
{
	return num_;
}

double Stats::getWmean() const
{
	doWindowCalculation();
	return wmean_;
}

double Stats::getWsdev() const
{
	doWindowCalculation();
	return wsdev_;
}

double Stats::getWsum() const
{
	doWindowCalculation();
	return wsum_;
}

unsigned int Stats::getWnum() const
{
	doWindowCalculation();
	return wnum_;
}

void Stats::setWindowLength( unsigned int len )
{
	if ( len < 1e6 ) {
		samples_.resize( len, 0.0 );
		isWindowDirty_ = true;
	} else {
		samples_.resize( 0 );
	}
}

unsigned int Stats::getWindowLength() const
{
	return samples_.size();
}

// Filthy function to const_cast the object. There is no particular
// reason other than the template requirements for this to be a const.
void Stats::doWindowCalculation() const
{
	Stats* temp = const_cast< Stats* >( this );
	temp->innerWindowCalculation();
}

void Stats::innerWindowCalculation()
{
	if ( isWindowDirty_ ) {
		double wsumsq = 0.0;
		wsum_ = 0.0;
		unsigned int max = samples_.size();
		if ( max > num_ )
			max = num_;
		for ( unsigned int i = 0; i < max; ++i ) {
			wsum_ += samples_[i];
			wsumsq += samples_[i] * samples_[i];
		}
		if ( max > 0 ) {
			wmean_ = wsum_ / max;
			wsdev_ = sqrt( ( wsumsq - wsum_ * wsum_ / max ) / max );
		}
		wnum_ = max;
		isWindowDirty_ = false;
	}
}
