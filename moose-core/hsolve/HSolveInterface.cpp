/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "HSolveStruct.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"
#include "HSolve.h"


//////////////////////////////////////////////////////////////////////
// Mapping global Id to local index.
//////////////////////////////////////////////////////////////////////

unsigned int HSolve::localIndex( Id id ) const
{
	/* can easily replace with an array lookup with some holes, as
	 * done in the Stoich class. */
    map< Id, unsigned int >::const_iterator i;

    i = localIndex_.find( id );
    if ( i != localIndex_.end() )
        return i->second;

    assert( 0 );

    return ~0;
}

void HSolve::mapIds( vector< Id > id )
{
    for ( unsigned int i = 0; i < id.size(); ++i )
    {
        // We don't expect these Id's to have been registered already.
        assert( localIndex_.find( id[ i ] ) == localIndex_.end() );

        localIndex_[ id[ i ] ] = i;
    }
}

void HSolve::mapIds()
{
    mapIds( compartmentId_ );
    mapIds( caConcId_ );
    mapIds( channelId_ );
    //~ mapIds( gateId_ );

    // Doesn't seem to be needed. Perhaps even the externalChannelId_ vector
    // is not needed.
    //~ for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic )
    //~ mapIds( externalChannelId_[ ic ] );
}

//////////////////////////////////////////////////////////////////////
// HSolvePassive interface.
//////////////////////////////////////////////////////////////////////

double HSolve::getVm( Id id ) const
{
    assert(this);
    unsigned int index = localIndex( id );
    assert( index < V_.size() );
    return V_[ index ];
}

void HSolve::setVm( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < V_.size() );
    V_[ index ] = value;
}

double HSolve::getCm( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    return tree_[ index ].Cm;
}

void HSolve::setCm( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    tree_[ index ].Cm = value;
    // Also update data structures used for calculations.
	assert( tree_.size() == compartment_.size() );
	compartment_[index].CmByDt = 2.0 * value / dt_;
}

double HSolve::getEm( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    return tree_[ index ].Em;
}

void HSolve::setEm( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    tree_[ index ].Em = value;
    // Also update data structures used for calculations.
	assert( tree_.size() == compartment_.size() );
	compartment_[index].EmByRm = value / tree_[index].Rm;
}

double HSolve::getRm( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    return tree_[ index ].Rm;
}

void HSolve::setRm( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    tree_[ index ].Rm = value;
    // Also update data structures used for calculations.
	assert( tree_.size() == compartment_.size() );
	compartment_[index].EmByRm = tree_[index].Em / value;
}

double HSolve::getRa( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    return tree_[ index ].Ra;
}

void HSolve::setRa( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    tree_[ index ].Ra = value;
}

double HSolve::getInitVm( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    return tree_[ index ].initVm;
}

void HSolve::setInitVm( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < tree_.size() );
    tree_[ index ].initVm = value;
}

// Read-only
double HSolve::getIm( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < nCompt_ );

    double Im =
        compartment_[ index ].EmByRm - V_[ index ] / tree_[ index ].Rm;

    vector< CurrentStruct >::const_iterator icurrent;

    if ( index == 0 )
        icurrent = current_.begin();
    else
        icurrent = currentBoundary_[ index - 1 ];

    for ( ; icurrent < currentBoundary_[ index ]; ++icurrent )
        Im += ( icurrent->Ek - V_[ index ] ) * icurrent->Gk;

    assert( 2 * index + 1 < externalCurrent_.size() );
	Im += prevExtCurr_[2*index+1] - prevExtCurr_[2*index]*V_[index];
    return Im;
}

// Read-only
double HSolve::getIa( Id id ) const
{
    return 0;
}

double HSolve::getInject( Id id ) const
{
    unsigned int index = localIndex( id );
    // Not assert( index < inject_.size() ), because inject_ is a map.
    assert( index < nCompt_ );

    map< unsigned int, InjectStruct >::const_iterator i;

    i = inject_.find( index );
    if ( i != inject_.end() )
        return i->second.injectBasal;

    return 0.0;
}

void HSolve::setInject( Id id, double value )
{
    unsigned int index = localIndex( id );
    // Not assert( index < inject_.size() ), because inject_ is a map.
    assert( index < nCompt_ );
    inject_[ index ].injectBasal = value;
}

void HSolve::addInject( Id id, double value )
{
    unsigned int index = localIndex( id );
    // Not assert( index < inject_.size() ), because inject_ is a map.
    assert( index < nCompt_ );
    inject_[ index ].injectVarying += value;
}


//////////////////////////////////////////////////////////////////////
// HSolveActive interface.
//////////////////////////////////////////////////////////////////////

//~ const vector< Id >& HSolve::getCompartments() const
//~ {
//~ return compartmentId_;
//~ }
//~
//~ const vector< Id >& HSolve::getHHChannels() const
//~ {
//~ return channelId_;
//~ }
//~
//~ const vector< Id >& HSolve::getCaConcs() const
//~ {
//~ return caConcId_;
//~ }
//~
//~ const vector< vector< Id > >& HSolve::getExternalChannels() const
//~ {
//~ return externalChannelId_;
//~ }

void HSolve::addGkEk( Id id, double Gk, double Ek )
{
    unsigned int index = localIndex( id );
    assert( 2 * index + 1 < externalCurrent_.size() );
    externalCurrent_[ 2 * index ] += Gk;
    externalCurrent_[ 2 * index + 1 ] += Gk * Ek;
}

void HSolve::addConc( Id id, double conc )
{
    unsigned int index = localIndex( id );
    assert(  index < externalCalcium_.size() );
    externalCalcium_[ index ] = conc;
}


void HSolve::setPowers(
    Id id,
    double Xpower,
    double Ypower,
    double Zpower )
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );
    channel_[ index ].setPowers( Xpower, Ypower, Zpower );
}

int HSolve::getInstant( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );
    return channel_[ index ].instant_;
}

void HSolve::setInstant( Id id, int instant )
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );
    channel_[ index ].instant_ = instant;
}

double HSolve::getHHChannelGbar( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );
    return channel_[ index ].Gbar_;
}

void HSolve::setHHChannelGbar( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );
    channel_[ index ].Gbar_ = value;

}

double HSolve::getEk( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < current_.size() );
    return current_[ index ].Ek;
}

void HSolve::setEk( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < current_.size() );
    current_[ index ].Ek = value;
}

double HSolve::getGk( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < current_.size() );
    return current_[ index ].Gk;
}

void HSolve::setGk( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < current_.size() );
    current_[ index ].Gk = value;
}

double HSolve::getIk( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < current_.size() );

    unsigned int comptIndex = chan2compt_[ index ];
    assert( comptIndex < V_.size() );

    return ( current_[ index ].Ek - V_[ comptIndex ] ) * current_[ index ].Gk;
}

double HSolve::getX( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );

    if ( channel_[ index ].Xpower_ == 0.0 )
        return 0.0;

    unsigned int stateIndex = chan2state_[ index ];
    assert( stateIndex < state_.size() );

    return state_[ stateIndex ];
}

void HSolve::setX( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );

    if ( channel_[ index ].Xpower_ == 0.0 )
        return;

    unsigned int stateIndex = chan2state_[ index ];
    assert( stateIndex < state_.size() );

    state_[ stateIndex ] = value;
}

double HSolve::getY( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );

    if ( channel_[ index ].Ypower_ == 0.0 )
        return 0.0;

    unsigned int stateIndex = chan2state_[ index ];

    if ( channel_[ index ].Xpower_ > 0.0 )
        ++stateIndex;

    assert( stateIndex < state_.size() );

    return state_[ stateIndex ];
}

void HSolve::setY( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );

    if ( channel_[ index ].Ypower_ == 0.0 )
        return;

    unsigned int stateIndex = chan2state_[ index ];

    if ( channel_[ index ].Xpower_ > 0.0 )
        ++stateIndex;

    assert( stateIndex < state_.size() );

    state_[ stateIndex ] = value;
}

double HSolve::getZ( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );

    if ( channel_[ index ].Zpower_ == 0.0 )
        return 0.0;

    unsigned int stateIndex = chan2state_[ index ];

    if ( channel_[ index ].Xpower_ > 0.0 )
        ++stateIndex;
    if ( channel_[ index ].Ypower_ > 0.0 )
        ++stateIndex;

    assert( stateIndex < state_.size() );

    return state_[ stateIndex ];
}

void HSolve::setZ( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );

    if ( channel_[ index ].Zpower_ == 0.0 )
        return;

    unsigned int stateIndex = chan2state_[ index ];

    if ( channel_[ index ].Xpower_ > 0.0 )
        ++stateIndex;
    if ( channel_[ index ].Ypower_ > 0.0 )
        ++stateIndex;

    assert( stateIndex < state_.size() );

    state_[ stateIndex ] = value;
}

void HSolve::setHHmodulation( Id id, double value )
{
    unsigned int index = localIndex( id );
    assert( index < channel_.size() );
	if ( value > 0.0 )
			channel_[index].modulation_ = value;
}

double HSolve::getCa( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );
    return ca_[ index ];
}

void HSolve::setCa( Id id, double Ca )
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );

    ca_[ index ] = Ca;
    caConc_[ index ].setCa( Ca );
}

void HSolve::iCa( Id id, double iCa )
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );

	caActivation_[index] += iCa;

	/*
    ca_[ index ] = Ca;
    caConc_[ index ].setCa( Ca );
	*/
}

double HSolve::getCaBasal( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );
    return caConc_[ index ].CaBasal_;
}

void HSolve::setCaBasal( Id id, double CaBasal )
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );

    caConc_[ index ].setCaBasal( CaBasal );
}

void HSolve::setTauB( Id id, double tau, double B )
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );

    caConc_[ index ].setTauB( tau, B, dt_ );
}

double HSolve::getCaCeiling( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );
    return caConc_[ index ].ceiling_;
}

void HSolve::setCaCeiling( Id id, double ceiling )
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );

    caConc_[ index ].ceiling_ = ceiling;
}

double HSolve::getCaFloor( Id id ) const
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );
    return caConc_[ index ].floor_;
}

void HSolve::setCaFloor( Id id, double floor )
{
    unsigned int index = localIndex( id );
    assert( index < caConc_.size() );

    caConc_[ index ].floor_ = floor;
}
