/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "VectorTable.h"
#include "../builtins/Interpol2D.h"
#include "MarkovRateTable.h"
#include "ChanBase.h"
#include "ChanCommon.h"
#include "MarkovChannel.h"

#if USE_GSL
#include <gsl/gsl_errno.h>
#endif

const Cinfo* MarkovChannel::initCinfo()
{
    ///////////////////////
    //Field information.
    ///////////////////////
    static ValueFinfo< MarkovChannel, double > ligandconc( "ligandConc",
            "Ligand concentration.",
            &MarkovChannel::setLigandConc,
            &MarkovChannel::getLigandConc
                                                         );

    static ValueFinfo< MarkovChannel, double > vm( "Vm",
            "Membrane voltage.",
            &MarkovChannel::setVm,
            &MarkovChannel::getVm
                                                 );

    static ValueFinfo< MarkovChannel, unsigned int > numstates( "numStates",
            "The number of states that the channel can occupy.",
            &MarkovChannel::setNumStates,
            &MarkovChannel::getNumStates
                                                              );


    static ValueFinfo< MarkovChannel, unsigned int > numopenstates( "numOpenStates",
            "The number of states which are open/conducting.",
            &MarkovChannel::setNumOpenStates,
            &MarkovChannel::getNumOpenStates
                                                                  );

    static ValueFinfo< MarkovChannel, vector< string > > labels("labels",
            "Labels for each state.",
            &MarkovChannel::setStateLabels,
            &MarkovChannel::getStateLabels
                                                               );

    static ReadOnlyValueFinfo< MarkovChannel, vector< double > > state( "state",
            "This is a row vector that contains the probabilities of finding the channel in each state.",
            &MarkovChannel::getState
                                                                      );

    static ValueFinfo< MarkovChannel, vector< double > > initialstate( "initialState",
            "This is a row vector that contains the probabilities of finding the channel in each state at t = 0. The state of the channel is reset to this value during a call to reinit()",
            &MarkovChannel::setInitialState,
            &MarkovChannel::getInitialState
                                                                     );

    static ValueFinfo< MarkovChannel, vector< double > > gbar( "gbar",
            "A row vector containing the conductance associated with each of the open/conducting states.",
            &MarkovChannel::setGbars,
            &MarkovChannel::getGbars
                                                             );

    //MsgDest functions
    static DestFinfo handleligandconc( "handleLigandConc",
            "Deals with incoming messages containing information of ligand concentration",
            new OpFunc1< MarkovChannel, double >(&MarkovChannel::handleLigandConc) );

    static DestFinfo handlestate("handleState",
            "Deals with incoming message from MarkovSolver object containing state information of the channel.\n",
            new OpFunc1< MarkovChannel, vector< double > >(&MarkovChannel::handleState) );

    ///////////////////////////////////////////
    static Finfo* MarkovChannelFinfos[] =
    {
        &ligandconc,
        &vm,
        &numstates,
        &numopenstates,
        &state,
        &initialstate,
        &labels,
        &gbar,
        &handleligandconc,
        &handlestate,
    };

    static string doc[] =
    {
        "Name", "MarkovChannel",
        "Author", "Vishaka Datta S, 2011, NCBS Bangalore",
        "Description", "MarkovChannel.\n"
        "It deals with ion channels which can be found in one of multiple states, "
        "some of which are conducting. This implementation assumes the occurence "
        "of first order kinetics to calculate the probabilities of the channel "
        "being found in all states. Further, the rates of transition between these "
        "states can be constant, voltage-dependent or ligand dependent (only one "
        "ligand species). The current flow obtained from the channel is calculated "
        "in a deterministic method by solving the system of differential equations "
        "obtained from the assumptions above."
    };

    static Dinfo< MarkovChannel > dinfo;
    static Cinfo MarkovChannelCinfo(
        "MarkovChannel",
        ChanBase::initCinfo(),
        MarkovChannelFinfos,
        sizeof( MarkovChannelFinfos )/ sizeof( Finfo* ),
        &dinfo,
        doc,
        sizeof(doc) / sizeof(string)
    );

    return &MarkovChannelCinfo;
}

static const Cinfo* markovChannelCinfo = MarkovChannel::initCinfo();

MarkovChannel::MarkovChannel() :
    g_(0),
    ligandConc_(0),
    numStates_(0),
    numOpenStates_(0)
{ ; }

MarkovChannel::MarkovChannel(unsigned int numStates, unsigned int numOpenStates) :
    g_(0), ligandConc_(0), numStates_(numStates), numOpenStates_(numOpenStates)
{
    stateLabels_.resize( numStates );
    state_.resize( numStates );
    initialState_.resize( numStates );
    Gbars_.resize( numOpenStates ) ;
}

MarkovChannel::~MarkovChannel( )
{
    ;
}

double MarkovChannel::getVm( ) const
{
    return Vm_;
}

void MarkovChannel::setVm( double Vm )
{
    Vm_ = Vm;
}

double MarkovChannel::getLigandConc( ) const
{
    return ligandConc_;
}

void MarkovChannel::setLigandConc( double ligandConc )
{
    ligandConc_ = ligandConc;
}

unsigned int MarkovChannel::getNumStates( ) const
{
    return numStates_;
}

void MarkovChannel::setNumStates( unsigned int numStates )
{
    numStates_ = numStates;
}

unsigned int MarkovChannel::getNumOpenStates( ) const
{
    return numOpenStates_;
}

void MarkovChannel::setNumOpenStates( unsigned int numOpenStates )
{
    numOpenStates_ = numOpenStates;
}

vector< string > MarkovChannel::getStateLabels( ) const
{
    return stateLabels_;
}

void MarkovChannel::setStateLabels( vector< string > stateLabels )
{
    stateLabels_ = stateLabels;
}

vector< double > MarkovChannel::getState ( ) const
{
    return state_;
}

vector< double > MarkovChannel::getInitialState() const
{
    return initialState_;
}

void MarkovChannel::setInitialState( vector< double > initialState )
{
    initialState_ = initialState;
    state_ = initialState;
}

vector< double > MarkovChannel::getGbars() const
{
    return Gbars_;
}

void MarkovChannel::setGbars( vector< double > Gbars )
{
    Gbars_ = Gbars;
}

/////////////////////////////
//MsgDest functions
////////////////////////////

void MarkovChannel::vProcess( const Eref& e, const ProcPtr p )
{
    g_ = 0.0;

    //Cannot use the Gbar_ variable of the ChanBase class. The conductance
    //Gk_ calculated here is the "expected conductance" of the channel due to its
    //stochastic nature.

    for( unsigned int i = 0; i < numOpenStates_; ++i )
        g_ += Gbars_[i] * state_[i];

    setGk( e, g_ );
    updateIk();

    sendProcessMsgs( e, p );
}

void MarkovChannel::vReinit( const Eref& e, const ProcPtr p )
{
    g_ = 0.0;

    if ( initialState_.empty() )
    {
        cerr << "MarkovChannel::reinit : Initial state has not been set.!\n";
        return;
    }
    state_ = initialState_;

    sendReinitMsgs( e, p );
}

void MarkovChannel::handleLigandConc( double ligandConc )
{
    ligandConc_ = ligandConc;
}

void MarkovChannel::handleState( vector< double > state )
{
    state_ = state;
}
