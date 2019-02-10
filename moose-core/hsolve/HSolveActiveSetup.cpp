/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/


#include "HSolveActive.h"

//////////////////////////////////////////////////////////////////////
// Setup of data structures
//////////////////////////////////////////////////////////////////////

void HSolveActive::setup( Id seed, double dt )
{
    //~ cout << ".. HA.setup()" << endl;

    this->HSolvePassive::setup( seed, dt );

    readHHChannels();
    readGates();
    readCalcium();
    createLookupTables();
    readSynapses(); // Reads SynChans, SpikeGens. Drops process msg for SpikeGens.
    readExternalChannels();
    manageOutgoingMessages(); // Manages messages going out from the cell's components.

    //~ reinit();
    cleanup();

    //~ cout << "# of compartments: " << compartmentId_.size() << "." << endl;
    //~ cout << "# of channels: " << channelId_.size() << "." << endl;
    //~ cout << "# of gates: " << gateId_.size() << "." << endl;
    //~ cout << "# of states: " << state_.size() << "." << endl;
    //~ cout << "# of Ca pools: " << caConc_.size() << "." << endl;
    //~ cout << "# of SynChans: " << synchan_.size() << "." << endl;
    //~ cout << "# of SpikeGens: " << spikegen_.size() << "." << endl;
}

void HSolveActive::reinit( ProcPtr info )
{
    externalCurrent_.assign( externalCurrent_.size(), 0.0 );

    reinitSpikeGens( info );
    reinitCompartments();
    reinitCalcium();
    reinitChannels();
    sendValues( info );
}

void HSolveActive::reinitSpikeGens( ProcPtr info )
{
    vector< SpikeGenStruct >::iterator ispike;
    for ( ispike = spikegen_.begin(); ispike != spikegen_.end(); ++ispike )
        ispike->reinit( info );
}

void HSolveActive::reinitCompartments()
{
    for ( unsigned int ic = 0; ic < nCompt_; ++ic )
        V_[ ic ] = tree_[ ic ].initVm;
}

void HSolveActive::reinitCalcium()
{
    caActivation_.assign( caActivation_.size(), 0.0 );

    for ( unsigned int i = 0; i < ca_.size(); ++i )
    {
        caConc_[ i ].c_ = 0.0;
        ca_[ i ] = caConc_[ i ].CaBasal_;
    }
}

void HSolveActive::reinitChannels()
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
    vector< double>::iterator iextca = externalCalcium_.begin();

    LookupRow vRow;
    LookupRow dRow;
    double C1, C2;
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

        chanBoundary = ichan + *ichannelcount;
        for ( ; ichan < chanBoundary; ++ichan )
        {

	  caTable_.row( *iextca, dRow );

            if ( ichan->Xpower_ > 0.0 )
            {
                vTable_.lookup( *icolumn, vRow, C1, C2 );

                *istate = C1 / C2;

                ++icolumn, ++istate;
            }

            if ( ichan->Ypower_ > 0.0 )
            {
                vTable_.lookup( *icolumn, vRow, C1, C2 );

                *istate = C1 / C2;

                ++icolumn, ++istate;
            }

            if ( ichan->Zpower_ > 0.0 )
            {
                LookupRow* caRow = *icarow;

                if ( caRow )
                {
                    caTable_.lookup( *icolumn, *caRow, C1, C2 );
                }
		else if (*iextca>0)
		  caTable_.lookup( *icolumn, dRow, C1, C2 );
                else
                {
                    vTable_.lookup( *icolumn, vRow, C1, C2 );
                }

                *istate = C1 / C2;

                ++icolumn, ++istate, ++icarow;
            }

	    ++iextca;
        }

        ++ichannelcount, ++icacount;
    }
}

void HSolveActive::readHHChannels()
{
    vector< Id >::iterator icompt;
    vector< Id >::iterator ichan;
    int nChannel = 0;
    double Gbar = 0.0, Ek = 0.0;
    double X = 0.0, Y = 0.0, Z = 0.0;
    double Xpower = 0.0, Ypower = 0.0, Zpower = 0.0;
    int instant = 0;

    for ( icompt = compartmentId_.begin(); icompt != compartmentId_.end(); ++icompt )
    {
        nChannel = HSolveUtils::hhchannels( *icompt, channelId_ );

        // todo: discard channels with Gbar = 0.0
        channelCount_.push_back( nChannel );

        ichan = channelId_.end() - nChannel;
        for ( ; ichan != channelId_.end(); ++ichan )
        {
            channel_.resize( channel_.size() + 1 );
            ChannelStruct& channel = channel_.back();

            current_.resize( current_.size() + 1 );
            CurrentStruct& current = current_.back();

            Gbar    = Field< double >::get( *ichan, "Gbar" );
            Ek    = Field< double >::get( *ichan, "Ek" );
            X    = Field< double >::get( *ichan, "X" );
            Y    = Field< double >::get( *ichan, "Y" );
            Z    = Field< double >::get( *ichan, "Z" );
            Xpower    = Field< double >::get( *ichan, "Xpower" );
            Ypower    = Field< double >::get( *ichan, "Ypower" );
            Zpower    = Field< double >::get( *ichan, "Zpower" );
            instant    = Field< int >::get( *ichan, "instant" );
            double modulation = Field< double >::get( *ichan, "modulation");

            current.Ek = Ek;

            channel.Gbar_ = Gbar;
            channel.setPowers( Xpower, Ypower, Zpower );
            channel.instant_ = instant;
            channel.modulation_ = modulation;

            /*
             * Map channel index to state index. This is useful in the
             * interface to find gate values.
             */
            chan2state_.push_back( state_.size() );

            if ( Xpower > 0.0 )
                state_.push_back( X );
            if ( Ypower > 0.0 )
                state_.push_back( Y );
            if ( Zpower > 0.0 )
                state_.push_back( Z );

            /*
             * Map channel index to compartment index. This is useful in the
             * interface to generate channel Ik values (since we then need the
             * compartment Vm).
             */
            chan2compt_.push_back( icompt - compartmentId_.begin() );
        }
    }

    int nCumulative = 0;
    currentBoundary_.resize( nCompt_ );
    for ( unsigned int ic = 0; ic < nCompt_; ++ic )
    {
        nCumulative += channelCount_[ ic ];
        currentBoundary_[ ic ] = current_.begin() + nCumulative;
    }
}

void HSolveActive::readGates()
{
    vector< Id >::iterator ichan;
    unsigned int nGates = 0;
    int useConcentration = 0;
    for ( ichan = channelId_.begin(); ichan != channelId_.end(); ++ichan )
    {
        nGates = HSolveUtils::gates( *ichan, gateId_ );
        gCaDepend_.insert( gCaDepend_.end(), nGates, 0 );
        useConcentration = Field< int >::get( *ichan, "useConcentration" );
        if ( useConcentration )
            gCaDepend_.back() = 1;
    }
}

void HSolveActive::readCalcium()
{
    double Ca, CaBasal, tau, B, ceiling, floor;
    vector< Id > caConcId;
    vector< int > caTargetIndex;
    map< Id, int > caConcIndex;
    int nTarget, nDepend = 0;
    vector< Id >::iterator iconc;

    caCount_.resize( nCompt_ );
    unsigned int ichan = 0;

    for ( unsigned int ic = 0; ic < nCompt_; ++ic )
    {

        unsigned int chanBoundary = ichan + channelCount_[ ic ];
        unsigned int nCa = caConc_.size();

        for ( ; ichan < chanBoundary; ++ichan )
        {
            caConcId.clear();

            nTarget = HSolveUtils::caTarget( channelId_[ ichan ], caConcId );
            if ( nTarget == 0 )
                // No calcium pools fed by this channel.
                caTargetIndex.push_back( -1 );

            nDepend = HSolveUtils::caDepend( channelId_[ ichan ], caConcId );

	    if ( nDepend == 0)
                // Channel does not depend on calcium.

	      caDependIndex_.push_back( -1 );


	    externalCalcium_.push_back(0);

            for ( iconc = caConcId.begin(); iconc != caConcId.end(); ++iconc )
                if ( caConcIndex.find( *iconc ) == caConcIndex.end() )
                {
                    caConcIndex[ *iconc ] = caCount_[ ic ];
                    ++caCount_[ ic ];

                    Ca = Field< double >::get( *iconc, "Ca" );
                    CaBasal = Field< double >::get( *iconc, "CaBasal" );
                    tau = Field< double >::get( *iconc, "tau" );
                    B = Field< double >::get( *iconc, "B" );
                    ceiling = Field< double >::get( *iconc, "ceiling" );
                    floor = Field< double >::get( *iconc, "floor" );

                    caConc_.push_back(
                        CaConcStruct(
                            Ca, CaBasal,
                            tau, B,
                            ceiling, floor,
                            dt_
                        )
                    );
                    caConcId_.push_back( *iconc );
                }

            if ( nTarget != 0 )
                caTargetIndex.push_back( caConcIndex[ caConcId.front() ] + nCa );
            if ( nDepend != 0 )
                caDependIndex_.push_back( caConcIndex[ caConcId.back() ] );


        }
    }



    caTarget_.resize( channel_.size() );
    ca_.resize( caConc_.size() );
    caActivation_.resize( caConc_.size() );

    for ( unsigned int ichan = 0; ichan < channel_.size(); ++ichan )
    {
        if ( caTargetIndex[ ichan ] == -1 )
            caTarget_[ ichan ] = 0;
        else
            caTarget_[ ichan ] = &caActivation_[ caTargetIndex[ ichan ] ];
    }


}

void HSolveActive::createLookupTables()
{
    std::set< Id > caSet;
    std::set< Id > vSet;
    vector< Id > caGate;
    vector< Id > vGate;
    map< Id, unsigned int > gateSpecies;

    for ( unsigned int ig = 0; ig < gateId_.size(); ++ig )
        if ( gCaDepend_[ ig ] )
            caSet.insert( gateId_[ ig ] );
        else
            vSet.insert( gateId_[ ig ] );

    caGate.insert( caGate.end(), caSet.begin(), caSet.end() );
    vGate.insert( vGate.end(), vSet.begin(), vSet.end() );

    for ( unsigned int ig = 0; ig < caGate.size(); ++ig )
        gateSpecies[ caGate[ ig ] ] = ig;
    for ( unsigned int ig = 0; ig < vGate.size(); ++ig )
        gateSpecies[ vGate[ ig ] ] = ig;

    /*
     * Finding the smallest xmin and largest xmax across all gates' lookup
     * tables.
     *
     * # of divs is determined by finding the smallest dx (highest density).
     */
    vMin_ = numeric_limits< double >::max();
    vMax_ = numeric_limits< double >::min();
    double vDx = numeric_limits< double >::max();
    caMin_ = numeric_limits< double >::max();
    caMax_ = numeric_limits< double >::min();
    double caDx = numeric_limits< double >::max();

    double min;
    double max;
    unsigned int divs;
    double dx;

    for ( unsigned int ig = 0; ig < caGate.size(); ++ig )
    {
        min = Field< double >::get( caGate[ ig ], "min" );
        max = Field< double >::get( caGate[ ig ], "max" );
        divs = Field< unsigned int >::get( caGate[ ig ], "divs" );
        dx = ( max - min ) / divs;

        if ( min < caMin_ )
            caMin_ = min;
        if ( max > caMax_ )
            caMax_ = max;
        if ( dx < caDx )
            caDx = dx;
    }
    double caDiv = ( caMax_ - caMin_ ) / caDx;
    caDiv_ = static_cast< int >( caDiv + 0.5 ); // Round-off to nearest int.

    for ( unsigned int ig = 0; ig < vGate.size(); ++ig )
    {
        min = Field< double >::get( vGate[ ig ], "min" );
        max = Field< double >::get( vGate[ ig ], "max" );
        divs = Field< unsigned int >::get( vGate[ ig ], "divs" );
        dx = ( max - min ) / divs;

        if ( min < vMin_ )
            vMin_ = min;
        if ( max > vMax_ )
            vMax_ = max;
        if ( dx < vDx )
            vDx = dx;
    }
    double vDiv = ( vMax_ - vMin_ ) / vDx;
    vDiv_ = static_cast< int >( vDiv + 0.5 ); // Round-off to nearest int.

    caTable_ = LookupTable( caMin_, caMax_, caDiv_, caGate.size() );
    vTable_ = LookupTable( vMin_, vMax_, vDiv_, vGate.size() );

    vector< double > A, B;
    vector< double >::iterator ia, ib;
    // double a, b;
    //~ int AMode, BMode;
    //~ bool interpolate;

    // Calcium-dependent lookup tables
    HSolveUtils::Grid caGrid( caMin_, caMax_, caDiv_ );
    //~ if ( !caGate.empty() ) {
    //~ grid.resize( 1 + caDiv_ );
    //~ double dca = ( caMax_ - caMin_ ) / caDiv_;
    //~ for ( int igrid = 0; igrid <= caDiv_; ++igrid )
    //~ grid[ igrid ] = caMin_ + igrid * dca;
    //~ }

    for ( unsigned int ig = 0; ig < caGate.size(); ++ig )
    {
        HSolveUtils::rates( caGate[ ig ], caGrid, A, B );
        //~ HSolveUtils::modes( caGate[ ig ], AMode, BMode );
        //~ interpolate = ( AMode == 1 ) || ( BMode == 1 );

        ia = A.begin();
        ib = B.begin();
        for ( unsigned int igrid = 0; igrid < caGrid.size(); ++igrid )
        {
            // Use one of the optimized forms below, instead of A and B
            // directly. Also updated reinit() accordingly (for gate state).
//            a = *ia;
//            b = *ib;

            // *ia = ( 2.0 - dt_ * b ) / ( 2.0 + dt_ * b );
            // *ib = dt_ * a / ( 1.0 + dt_ * b / 2.0 );
            // *ia = dt_ * a;
           // *ib = 1.0 + dt_ * b / 2.0;
            ++ia, ++ib;
        }

        //~ caTable_.addColumns( ig, A, B, interpolate );
        caTable_.addColumns( ig, A, B );
    }

    // Voltage-dependent lookup tables
    HSolveUtils::Grid vGrid( vMin_, vMax_, vDiv_ );
    //~ if ( !vGate.empty() ) {
    //~ grid.resize( 1 + vDiv_ );
    //~ double dv = ( vMax_ - vMin_ ) / vDiv_;
    //~ for ( int igrid = 0; igrid <= vDiv_; ++igrid )
    //~ grid[ igrid ] = vMin_ + igrid * dv;
    //~ }


    for ( unsigned int ig = 0; ig < vGate.size(); ++ig )
    {
        //~ interpolate = HSolveUtils::get< HHGate, bool >( vGate[ ig ], "useInterpolation" );
        HSolveUtils::rates( vGate[ ig ], vGrid, A, B );
        //~ HSolveUtils::modes( vGate[ ig ], AMode, BMode );
        //~ interpolate = ( AMode == 1 ) || ( BMode == 1 );

        ia = A.begin();
        ib = B.begin();
        for ( unsigned int igrid = 0; igrid < vGrid.size(); ++igrid )
        {
            // Use one of the optimized forms below, instead of A and B
            // directly. Also updated reinit() accordingly (for gate state).
//            a = *ia;
//            b = *ib;

            // *ia = ( 2.0 - dt_ * b ) / ( 2.0 + dt_ * b );
            // *ib = dt_ * a / ( 1.0 + dt_ * b / 2.0 );
            // *ia = dt_ * a;
            // *ib = 1.0 + dt_ * b / 2.0;
            ++ia, ++ib;
        }

        //~ vTable_.addColumns( ig, A, B, interpolate );
        vTable_.addColumns( ig, A, B );
    }

    column_.reserve( gateId_.size() );
    for ( unsigned int ig = 0; ig < gateId_.size(); ++ig )
    {
        unsigned int species = gateSpecies[ gateId_[ ig ] ];

        LookupColumn column;
        if ( gCaDepend_[ ig ] )
            caTable_.column( species, column );
        else
            vTable_.column( species, column );

        column_.push_back( column );
    }

    ///////////////////!!!!!!!!!!
    unsigned int maxN = *( max_element( caCount_.begin(), caCount_.end() ) );
    caRowCompt_.resize( maxN );

    for ( unsigned int ichan = 0; ichan < channel_.size(); ++ichan )
    {
        if ( channel_[ ichan ].Zpower_ > 0.0 )
        {
            int index = caDependIndex_[ ichan ];

            if ( index == -1 )
                caRow_.push_back( 0 );
            else
                caRow_.push_back( &caRowCompt_[ index ] );
        }
    }


}

/**
 * Reads in SynChans and SpikeGens.
 *
* Unlike Compartments, HHChannels, etc., neither of these are zombified.
 * In other words, their fields are not managed by HSolve, and their "process"
 * functions are invoked to do their calculations. For SynChans, the process
 * calls are made by their respective clocks, and hence the process message is
 * not dropped. On the other hand, we drop the SpikeGen process messages here,
 * and explicitly call the SpikeGen process() from the HSolve via a pointer.
 */
void HSolveActive::readSynapses()
{
    vector< Id > spikeId;
    vector< Id > synId;
    vector< Id >::iterator syn;
    vector< Id >::iterator spike;
    SynChanStruct synchan;

    for ( unsigned int ic = 0; ic < nCompt_; ++ic )
    {
        synId.clear();
        HSolveUtils::synchans( compartmentId_[ ic ], synId );
        for ( syn = synId.begin(); syn != synId.end(); ++syn )
        {
            synchan.compt_ = ic;
            synchan.elm_ = *syn;
            synchan_.push_back( synchan );
        }

        static const Finfo* procDest = SpikeGen::initCinfo()->findFinfo( "process");
        assert( procDest );
        const DestFinfo* df = dynamic_cast< const DestFinfo* >( procDest );
        assert( df );

        spikeId.clear();
        HSolveUtils::spikegens( compartmentId_[ ic ], spikeId );
        // Very unlikely that there will be >1 spikegens in a compartment,
        // but lets take care of it anyway.
        for ( spike = spikeId.begin(); spike != spikeId.end(); ++spike )
        {
            spikegen_.push_back(
                SpikeGenStruct( &V_[ ic ], spike->eref() )
            );

            ObjId mid = spike->element()->findCaller( df->getFid() );
            if ( ! mid.bad()  )
                Msg::deleteMsg( mid );
        }
    }
}

void HSolveActive::readExternalChannels()
{
    vector< string > filter;
    filter.push_back( "HHChannel" );
    //~ filter.push_back( "SynChan" );

    //~ externalChannelId_.resize( compartmentId_.size() );
    externalCurrent_.resize( 2 * compartmentId_.size(), 0.0 );
    prevExtCurr_.resize( externalCurrent_.size(), 0.0 );

    //~ for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic )
    //~ HSolveUtils::targets(
    //~ compartmentId_[ ic ],
    //~ "channel",
    //~ externalChannelId_[ ic ],
    //~ filter,
    //~ false    // include = false. That is, use filter to exclude.
    //~ );
}

void HSolveActive::manageOutgoingMessages()
{
    vector< Id > targets;
    vector< string > filter;

    /*
     * Going through all comparments, and finding out which ones have external
     * targets through the VmOut msg. External refers to objects that do not
     * belong the cell being managed by this HSolve. We find these by excluding
     * any HHChannels and SpikeGens from the VmOut targets. These will then
     * be used in HSolveActive::sendValues() to send out the messages behalf of
     * the original objects.
     */
    filter.push_back( "HHChannel" );
    filter.push_back( "SpikeGen" );
    for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic )
    {
        targets.clear();

        int nTargets = HSolveUtils::targets(
                           compartmentId_[ ic ],
                           "VmOut",
                           targets,
                           filter,
                           false    // include = false. That is, use filter to exclude.
                       );

        if ( nTargets )
            outVm_.push_back( ic );
    }

    /*
     * As before, going through all CaConcs, and finding any which have external
     * targets.
     */
    filter.clear();
    filter.push_back( "HHChannel" );
    for ( unsigned int ica = 0; ica < caConcId_.size(); ++ica )
    {
        targets.clear();

        int nTargets = HSolveUtils::targets(
                           caConcId_[ ica ],
                           "concOut",
                           targets,
                           filter,
                           false    // include = false. That is, use filter to exclude.
                       );

        if ( nTargets )
            outCa_.push_back( ica );
    }

    filter.clear();
    filter.push_back( "CaConc" );

    for ( unsigned int ik = 0; ik < channelId_.size(); ++ik )
    {
        targets.clear();

        int nTargets = HSolveUtils::targets(
                           channelId_[ ik ],
                           "IkOut",
                           targets,
                           filter,
                           false    // include = false. That is, use filter to exclude.
                       );

        if ( nTargets )
            outIk_.push_back( ik );


    }



}

void HSolveActive::cleanup()
{
//	compartmentId_.clear();
    gCaDepend_.clear();
    caDependIndex_.clear();
}
