/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include <queue>
#include "HSolveStruct.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"
#include "HSolve.h"
#include "../biophysics/CompartmentBase.h"
#include "../biophysics/Compartment.h"
#include "../biophysics/CaConcBase.h"
#include "../biophysics/ChanBase.h"
#include "ZombieCaConc.h"
using namespace moose;
//~ #include "ZombieCompartment.h"
//~ #include "ZombieCaConc.h"

extern ostream& operator <<( ostream& s, const HinesMatrix& m );

const int HSolveActive::INSTANT_X = 1;
const int HSolveActive::INSTANT_Y = 2;
const int HSolveActive::INSTANT_Z = 4;

HSolveActive::HSolveActive()
{
    caAdvance_ = 1;

    // Default lookup table size
    //~ vDiv_ = 3000;    // for voltage
    //~ caDiv_ = 3000;   // for calcium
}

//////////////////////////////////////////////////////////////////////
// Solving differential equations
//////////////////////////////////////////////////////////////////////
void HSolveActive::step( ProcPtr info )
{
    if ( nCompt_ <= 0 )
        return;

    if ( !current_.size() )
    {
        current_.resize( channel_.size() );
    }

    advanceChannels( info->dt );
    calculateChannelCurrents();
    updateMatrix();
    HSolvePassive::forwardEliminate();
    HSolvePassive::backwardSubstitute();
    advanceCalcium();
    advanceSynChans( info );
    sendValues( info );
    sendSpikes( info );
    prevExtCurr_ = externalCurrent_;
    externalCurrent_.assign( externalCurrent_.size(), 0.0 );
}

void HSolveActive::calculateChannelCurrents()
{
    vector< ChannelStruct >::iterator ichan;
    vector< CurrentStruct >::iterator icurrent = current_.begin();

    if ( state_.size() != 0 )
    {
        double* istate = &state_[ 0 ];

        for ( ichan = channel_.begin(); ichan != channel_.end(); ++ichan )
        {
            ichan->process( istate, *icurrent );
            ++icurrent;
        }
    }
}

void HSolveActive::updateMatrix()
{
    /*
     * Copy contents of HJCopy_ into HJ_. Cannot do a vector assign() because
     * iterators to HJ_ get invalidated in MS VC++
     */
    if ( HJ_.size() != 0 )
        memcpy( &HJ_[ 0 ], &HJCopy_[ 0 ], sizeof( double ) * HJ_.size() );

    double GkSum, GkEkSum;
    vector< CurrentStruct >::iterator icurrent = current_.begin();
    vector< currentVecIter >::iterator iboundary = currentBoundary_.begin();
    vector< double >::iterator ihs = HS_.begin();
    vector< double >::iterator iv = V_.begin();

    vector< CompartmentStruct >::iterator ic;
    for ( ic = compartment_.begin(); ic != compartment_.end(); ++ic )
    {
        GkSum   = 0.0;
        GkEkSum = 0.0;
        for ( ; icurrent < *iboundary; ++icurrent )
        {
            GkSum   += icurrent->Gk;
            GkEkSum += icurrent->Gk * icurrent->Ek;
        }

        *ihs = *( 2 + ihs ) + GkSum;
        *( 3 + ihs ) = *iv * ic->CmByDt + ic->EmByRm + GkEkSum;

        ++iboundary, ihs += 4, ++iv;
    }

    map< unsigned int, InjectStruct >::iterator inject;
    for ( inject = inject_.begin(); inject != inject_.end(); ++inject )
    {
        unsigned int ic = inject->first;
        InjectStruct& value = inject->second;

        HS_[ 4 * ic + 3 ] += value.injectVarying + value.injectBasal;

        value.injectVarying = 0.0;
    }

    // Synapses are being handled as external channels.
    //~ double Gk, Ek;
    //~ vector< SynChanStruct >::iterator isyn;
    //~ for ( isyn = synchan_.begin(); isyn != synchan_.end(); ++isyn ) {
    //~ get< double >( isyn->elm_, synGkFinfo, Gk );
    //~ get< double >( isyn->elm_, synEkFinfo, Ek );
    //~
    //~ unsigned int ic = isyn->compt_;
    //~ HS_[ 4 * ic ] += Gk;
    //~ HS_[ 4 * ic + 3 ] += Gk * Ek;
    //~ }

    ihs = HS_.begin();
    vector< double >::iterator iec;
    for ( iec = externalCurrent_.begin(); iec != externalCurrent_.end(); iec += 2 )
    {
        *ihs += *iec;
        *( 3 + ihs ) += *( iec + 1 );

        ihs += 4;
    }

    stage_ = 0;    // Update done.
}

void HSolveActive::advanceCalcium()
{
    vector< double* >::iterator icatarget = caTarget_.begin();
    vector< double >::iterator ivmid = VMid_.begin();
    vector< CurrentStruct >::iterator icurrent = current_.begin();
    vector< currentVecIter >::iterator iboundary = currentBoundary_.begin();

    /*
     * caAdvance_: This flag determines how current flowing into a calcium pool
     * is computed. A value of 0 means that the membrane potential at the
     * beginning of the time-step is used for the calculation. This is how
     * GENESIS does its computations. A value of 1 means the membrane potential
     * at the middle of the time-step is used. This is the correct way of
     * integration, and is the default way.
     */
    if ( caAdvance_ == 1 )
    {
        for ( ; iboundary != currentBoundary_.end(); ++iboundary )
        {
            for ( ; icurrent < *iboundary; ++icurrent )
            {
                if ( *icatarget )
                    **icatarget += icurrent->Gk * ( icurrent->Ek - *ivmid );

                ++icatarget;
            }

            ++ivmid;
        }
    }
    else if ( caAdvance_ == 0 )
    {
        vector< double >::iterator iv = V_.begin();
        double v0;

        for ( ; iboundary != currentBoundary_.end(); ++iboundary )
        {
            for ( ; icurrent < *iboundary; ++icurrent )
            {
                if ( *icatarget )
                {
                    v0 = ( 2 * *ivmid - *iv );

                    **icatarget += icurrent->Gk * ( icurrent->Ek - v0 );
                }

                ++icatarget;
            }

            ++ivmid, ++iv;
        }
    }

    vector< CaConcStruct >::iterator icaconc;
    vector< double >::iterator icaactivation = caActivation_.begin();
    vector< double >::iterator ica = ca_.begin();
    for ( icaconc = caConc_.begin(); icaconc != caConc_.end(); ++icaconc )
    {
        *ica = icaconc->process( *icaactivation );
        ++ica, ++icaactivation;
    }

    caActivation_.assign( caActivation_.size(), 0.0 );
}

void HSolveActive::advanceChannels( double dt )
{
    vector< double >::iterator iv;
    vector< double >::iterator istate = state_.begin();
    vector< int >::iterator ichannelcount = channelCount_.begin();
    vector< ChannelStruct >::iterator ichan = channel_.begin();
    vector< ChannelStruct >::iterator chanBoundary;
    vector< unsigned int >::iterator icacount = caCount_.begin();
    vector< double >::iterator ica = ca_.begin();
    vector< double >::iterator caBoundary;
    vector< LookupColumn >::iterator icolumn = column_.begin();
    vector< LookupRow >::iterator icarowcompt;
    vector< LookupRow* >::iterator icarow = caRow_.begin();
    vector< double >::iterator iextca = externalCalcium_.begin();

    LookupRow vRow;
    LookupRow dRow;
    double C1 = 0.0, C2 = 0.0;

    for ( iv = V_.begin(); iv != V_.end(); ++iv )
    {
        vTable_.row( *iv, vRow );
        icarowcompt = caRowCompt_.begin();
        caBoundary = ica + *icacount;
        for ( ; ica < caBoundary; ++ica )
        {
            caTable_.row( *ica, *icarowcompt );

            ++icarowcompt;
        }

        /*
         * Optimize by moving "if ( instant )" outside the loop, because it is
         * rarely used. May also be able to avoid "if ( power )".
         *
         * Or not: excellent branch predictors these days.
         *
         * Will be nice to test these optimizations.
         */
        chanBoundary = ichan + *ichannelcount;
        for ( ; ichan < chanBoundary; ++ichan )
        {

            caTable_.row( *iextca, dRow );

            if ( ichan->Xpower_ > 0.0 )
            {
                vTable_.lookup( *icolumn, vRow, C1, C2 );
                //~ *istate = *istate * C1 + C2;
                //~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
                if ( ichan->instant_ & INSTANT_X )
                    *istate = C1 / C2;
                else
                {
                    double temp = 1.0 + dt / 2.0 * C2;
                    *istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
                }

                ++icolumn, ++istate;
            }

            if ( ichan->Ypower_ > 0.0 )
            {
                vTable_.lookup( *icolumn, vRow, C1, C2 );
                //~ *istate = *istate * C1 + C2;
                //~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
                if ( ichan->instant_ & INSTANT_Y )
                    *istate = C1 / C2;
                else
                {
                    double temp = 1.0 + dt / 2.0 * C2;
                    *istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;

                }
                ++icolumn, ++istate;
            }

            if ( ichan->Zpower_ > 0.0 )
            {
                LookupRow* caRow = *icarow;

                if ( caRow )
                {
                    caTable_.lookup( *icolumn, *caRow, C1, C2 );

                }
                else if (*iextca >0)
                {
                    caTable_.lookup( *icolumn, dRow, C1, C2 );
                }
                else
                {
                    vTable_.lookup( *icolumn, vRow, C1, C2 );

                }

                //~ *istate = *istate * C1 + C2;
                //~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
                if ( ichan->instant_ & INSTANT_Z )
                    *istate = C1 / C2;
                else
                {
                    double temp = 1.0 + dt / 2.0 * C2;
                    *istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
                }

                ++icolumn, ++istate, ++icarow;

            }
            ++iextca;
        }

        ++ichannelcount, ++icacount;
    }
}

/**
 * SynChans are currently not under solver's control
 */
void HSolveActive::advanceSynChans( ProcPtr info )
{
    return;
}

void HSolveActive::sendSpikes( ProcPtr info )
{
    vector< SpikeGenStruct >::iterator ispike;
    for ( ispike = spikegen_.begin(); ispike != spikegen_.end(); ++ispike )
        ispike->send( info );
}

/**
 * This function dispatches state values via any source messages on biophysical
 * objects which have been taken over.
 *
 */
void HSolveActive::sendValues( ProcPtr info )
{
    vector< unsigned int >::iterator i;

    for ( i = outVm_.begin(); i != outVm_.end(); ++i )
    {
        Compartment::VmOut()->send(
            //~ ZombieCompartment::VmOut()->send(
            compartmentId_[ *i ].eref(),
            V_[ *i ]
        );
    }

    for ( i = outIk_.begin(); i != outIk_.end(); ++i )
    {

        unsigned int comptIndex = chan2compt_[ *i ];

        assert( comptIndex < V_.size() );

        ChanBase::IkOut()->send(channelId_[*i].eref(),
                                (current_[ *i ].Ek - V_[ comptIndex ]) * current_[ *i ].Gk);

    }

    for ( i = outCa_.begin(); i != outCa_.end(); ++i )
        //~ CaConc::concOut()->send(
        CaConcBase::concOut()->send(
            caConcId_[ *i ].eref(),
            ca_[ *i ]
        );
}
