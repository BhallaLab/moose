/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "../basecode/header.h"
#include "Synapse.h"
#include "SynEvent.h" // only using the SynEvent class from this
#include "SynHandlerBase.h"
#include "STDPSynapse.h"
#include "STDPSynHandler.h"

const Cinfo* STDPSynHandler::initCinfo()
{
	static string doc[] =
	{
		"Name", "STDPSynHandler",
		"Author", "Aditya Gilra",
		"Description",
		"The STDPSynHandler handles synapses with spike timing dependent plasticity (STDP). "
		"It uses two priority queues to manage pre and post spikes."
	};

    static ValueFinfo< STDPSynHandler, double > aMinus(
        "aMinus",
        "aMinus is a post-synaptic variable that keeps a decaying 'history' of previous post-spike(s)"
        "and is used to update the synaptic weight when a pre-synaptic spike appears."
        "It determines the t_pre > t_post (pre after post) part of the STDP window.",
		&STDPSynHandler::setAMinus,
		&STDPSynHandler::getAMinus
    );

    static ValueFinfo< STDPSynHandler, double > aMinus0(
        "aMinus0",
        "aMinus0 is added to aMinus on every pre-spike",
		&STDPSynHandler::setAMinus0,
		&STDPSynHandler::getAMinus0
    );

    static ValueFinfo< STDPSynHandler, double > tauMinus(
        "tauMinus",
        "aMinus decays with tauMinus time constant",
		&STDPSynHandler::setTauMinus,
		&STDPSynHandler::getTauMinus
    );

    static ValueFinfo< STDPSynHandler, double > aPlus0(
        "aPlus0",
        "aPlus0 is added to aPlus on every pre-spike",
		&STDPSynHandler::setAPlus0,
		&STDPSynHandler::getAPlus0
    );

    static ValueFinfo< STDPSynHandler, double > tauPlus(
        "tauPlus",
        "aPlus decays with tauPlus time constant",
		&STDPSynHandler::setTauPlus,
		&STDPSynHandler::getTauPlus
    );

    static ValueFinfo< STDPSynHandler, double > weightMax(
        "weightMax",
        "an upper bound on the weight",
		&STDPSynHandler::setWeightMax,
		&STDPSynHandler::getWeightMax
    );

    static ValueFinfo< STDPSynHandler, double > weightMin(
        "weightMin",
        "a lower bound on the weight",
		&STDPSynHandler::setWeightMin,
		&STDPSynHandler::getWeightMin
    );

    static DestFinfo addPostSpike( "addPostSpike",
        "Handles arriving spike messages from post-synaptic neuron, inserts into postEvent queue.",
        new EpFunc1< STDPSynHandler, double >( &STDPSynHandler::addPostSpike ) );

	static FieldElementFinfo< SynHandlerBase, STDPSynapse > synFinfo(
		"synapse",
		"Sets up field Elements for synapse",
		STDPSynapse::initCinfo(),
        /*
           below SynHandlerBase::getSynapse is a function that returns Synapse*,
           but I need to cast the returned pointer to an STDPSynapse*.
           Since I take the address (&) of SynHandlerBase::getSynapse
           which is: Synapse* (SynHandlerBase::*)(unsigned int),
           I need to cast the address of the function to:
           STDPSynapse* (SynHandlerBase::*)(unsigned int).

           This is required to put STDPSynapse in above FieldElementFinfo definition,
           see FieldElementFinfo template class in basecode/FieldElementFinfo.h
        */
		(STDPSynapse* (SynHandlerBase::*)(unsigned int)) &SynHandlerBase::getSynapse,
		&SynHandlerBase::setNumSynapses,
		&SynHandlerBase::getNumSynapses
	);

	static Finfo* STDPSynHandlerFinfos[] = {
		&synFinfo,			// FieldElement
		&addPostSpike,		// DestFinfo
		&aMinus0,		    // Field
		&aMinus,		    // Field
		&tauMinus,   	    // Field
		&aPlus0,	        // Field
		&tauPlus,	        // Field
        &weightMax,         // Field
        &weightMin          // Field
	};

	static Dinfo< STDPSynHandler > dinfo;
	static Cinfo synHandlerCinfo (
		"STDPSynHandler",
		SynHandlerBase::initCinfo(),
		STDPSynHandlerFinfos,
		sizeof( STDPSynHandlerFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &synHandlerCinfo;
}

static const Cinfo* STDPSynHandlerCinfo = STDPSynHandler::initCinfo();

STDPSynHandler::STDPSynHandler()
{
    aMinus_ = 0.0;
    tauMinus_ = 1.0;
    aMinus0_ = 0.0;
    tauPlus_ = 1.0;
    aPlus0_ = 0.0;
    weightMin_ = 0.0;
    weightMax_ = 0.0;
}

STDPSynHandler::~STDPSynHandler()
{ ; }

STDPSynHandler& STDPSynHandler::operator=( const STDPSynHandler& ssh)
{
	synapses_ = ssh.synapses_;
	for ( vector< STDPSynapse >::iterator
					i = synapses_.begin(); i != synapses_.end(); ++i )
			i->setHandler( this );

	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();

	while( !postEvents_.empty() )
		postEvents_.pop();

	return *this;
}

void STDPSynHandler::vSetNumSynapses( const unsigned int v )
{
	unsigned int prevSize = synapses_.size();
	synapses_.resize( v );
	for ( unsigned int i = prevSize; i < v; ++i )
		synapses_[i].setHandler( this );
}

unsigned int STDPSynHandler::vGetNumSynapses() const
{
	return synapses_.size();
}

STDPSynapse* STDPSynHandler::vGetSynapse( unsigned int i )
{
	static STDPSynapse dummy;
	if ( i < synapses_.size() )
		return &synapses_[i];
	cout << "Warning: STDPSynHandler::getSynapse: index: " << i <<
		" is out of range: " << synapses_.size() << endl;
	return &dummy;
}

void STDPSynHandler::addSpike(
				unsigned int index, double time, double weight )
{
	assert( index < synapses_.size() );
	events_.push( PreSynEvent( index, time, weight ) );
}

double STDPSynHandler::getTopSpike( unsigned int index ) const
{
	if ( events_.empty() )
		return 0.0;
	return events_.top().time;
}

void STDPSynHandler::addPostSpike( const Eref& e, double time )
{
	postEvents_.push( PostSynEvent( time ) );
}

void STDPSynHandler::vProcess( const Eref& e, ProcPtr p )
{
	double activation = 0.0;

    // process pre-synaptic spike events for activation and STDP
	while( !events_.empty() && events_.top().time <= p->currTime ) {
        PreSynEvent currEvent = events_.top();

        unsigned int synIndex = currEvent.synIndex;
        // Warning, coder! 'STDPSynapse currSyn = synapses_[synIndex];' is wrong,
        // it creates a new, shallow-copied object.
        // We want only to point to the same object.
        STDPSynapse* currSynPtr = &synapses_[synIndex];

        // activate the synapse for every pre-spike
        // If the synapse has a delay, the weight could be updated during the delay!
        // currEvent.weight is the weight before the delay!
        // Might be better to use currSynPtr->getWeight()
        //      or even the latest updated weight below?!
        // Using currSynPtr->getWeight().
        // However, the weight update is done
        //          after sending the current weight to activation

        // See: http://www.genesis-sim.org/GENESIS/Hyperdoc/Manual-26.html#synchan
        // Send out weight / dt for every spike
        //      Since it is an impulse active only for one dt,
        //      need to send it divided by dt.
        // Can connect activation to SynChan (double exp)
        //      or to LIF as an impulse to voltage.
		//activation += currEvent.weight / p->dt;
        activation += currSynPtr->getWeight() / p->dt;

        // Maintain 'history' of pre-spikes in Aplus
        // Add aPlus0 to the aPlus for this synapse due to pre-spike
        currSynPtr->setAPlus( currSynPtr->getAPlus() + aPlus0_ );

        // Change weight by aMinus_ at each pre-spike
        // clip weight within [weightMin,weightMax]
        double newWeight = currEvent.weight + aMinus_;
        newWeight = std::max(weightMin_, std::min(newWeight, weightMax_));
        currSynPtr->setWeight( newWeight );

		events_.pop();
	}
	if ( activation != 0.0 )
		SynHandlerBase::activationOut()->send( e, activation );

    // process post-synaptic spike events for STDP
	while( !postEvents_.empty() && postEvents_.top().time <= p->currTime ) {
        // Add aMinus0 to the aMinus for this synapse
        aMinus_ += aMinus0_;

        // Change weight of all synapses by aPlus_ at each post-spike
        for (unsigned int i=0; i<synapses_.size(); i++) {
            // since aPlus_, tauPlus_ are private inside STDPSynapse,
            //   we use the set, get functions
            // Warning, coder! 'STDPSynapse currSyn = synapses_[i];' is wrong,
            // it creates a new, shallow-copied object.
            // We want only to point to the same object.
            STDPSynapse* currSynPtr = &synapses_[i];
            // clip weight within [weightMin,weightMax]
            double newWeight = currSynPtr->getWeight() + currSynPtr->getAPlus();
            newWeight = std::max(weightMin_, std::min(newWeight, weightMax_));
            currSynPtr->setWeight( newWeight );
        }

		postEvents_.pop();
	}

    // modify aPlus and aMinus at every time step
    // Future: I could make this event-driven. Would be faster.
    // Or have a field to set it to event-driven or continuous.
    double dt_ = p->dt;
    // decay aPlus for all pre-synaptic inputs
    for (unsigned int i=0; i<synapses_.size(); i++) {
        // forward Euler
        // since aPlus_, tauPlus_ are private inside STDPSynapse,
        //   we use the set, get functions
        // Warning, coder! 'STDPSynapse currSyn = synapses_[i];' is wrong,
        // it creates a new, shallow-copied object.
        // We want only to point to the same object.
        STDPSynapse* currSynPtr = &synapses_[i];
        currSynPtr->setAPlus( currSynPtr->getAPlus() * (1.0 - dt_/tauPlus_) );
    }
    // decay aMinus for this STDPSynHandler which sits on the post-synaptic compartment
    // forward Euler
    aMinus_ -= aMinus_/tauMinus_*dt_;

}

void STDPSynHandler::vReinit( const Eref& e, ProcPtr p )
{
	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();
	while( !postEvents_.empty() )
		postEvents_.pop();
}

unsigned int STDPSynHandler::addSynapse()
{
	unsigned int newSynIndex = synapses_.size();
	synapses_.resize( newSynIndex + 1 );
	synapses_[newSynIndex].setHandler( this );
	return newSynIndex;
}


void STDPSynHandler::dropSynapse( unsigned int msgLookup )
{
	assert( msgLookup < synapses_.size() );
	synapses_[msgLookup].setWeight( -1.0 );
}

void STDPSynHandler::setAMinus0( const double v )
{
	aMinus0_ = v;
}

double STDPSynHandler::getAMinus0() const
{
	return aMinus0_;
}

void STDPSynHandler::setAMinus( const double v )
{
	aMinus_ = v;
}

double STDPSynHandler::getAMinus() const
{
	return aMinus_;
}

void STDPSynHandler::setTauMinus( const double v )
{
	if ( rangeWarning( "tauMinus", v ) ) return;
	tauMinus_ = v;
}

double STDPSynHandler::getTauMinus() const
{
	return tauMinus_;
}

void STDPSynHandler::setAPlus0( const double v )
{
	aPlus0_ = v;
}

double STDPSynHandler::getAPlus0() const
{
	return aPlus0_;
}

void STDPSynHandler::setTauPlus( const double v )
{
	if ( rangeWarning( "tauPlus", v ) ) return;
	tauPlus_ = v;
}

double STDPSynHandler::getTauPlus() const
{
	return tauPlus_;
}

void STDPSynHandler::setWeightMax( const double v )
{
	weightMax_ = v;
}

double STDPSynHandler::getWeightMax() const
{
	return weightMax_;
}

void STDPSynHandler::setWeightMin( const double v )
{
	weightMin_ = v;
}

double STDPSynHandler::getWeightMin() const
{
	return weightMin_;
}
