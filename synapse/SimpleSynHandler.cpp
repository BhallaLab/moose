/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "header.h"
#include "Synapse.h"
#include "SynEvent.h"
#include "SynHandlerBase.h"
#include "SimpleSynHandler.h"

const Cinfo* SimpleSynHandler::initCinfo()
{
	static string doc[] =
	{
		"Name", "SimpleSynHandler",
		"Author", "Upi Bhalla",
		"Description",
		"The SimpleSynHandler handles simple synapses without plasticity. "
		"It uses a priority queue to manage them."
	};

	static FieldElementFinfo< SynHandlerBase, Synapse > synFinfo(
		"synapse",
		"Sets up field Elements for synapse",
		Synapse::initCinfo(),
		&SynHandlerBase::getSynapse,
		&SynHandlerBase::setNumSynapses,
		&SynHandlerBase::getNumSynapses
	);

	static Finfo* synHandlerFinfos[] = {
		&synFinfo			// FieldElement
	};

	static Dinfo< SimpleSynHandler > dinfo;
	static Cinfo synHandlerCinfo (
		"SimpleSynHandler",
		SynHandlerBase::initCinfo(),
		synHandlerFinfos,
		sizeof( synHandlerFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &synHandlerCinfo;
}

static const Cinfo* synHandlerCinfo = SimpleSynHandler::initCinfo();

SimpleSynHandler::SimpleSynHandler()
{ ; }

SimpleSynHandler::~SimpleSynHandler()
{ ; }

SimpleSynHandler& SimpleSynHandler::operator=( const SimpleSynHandler& ssh)
{
	synapses_ = ssh.synapses_;
	for ( vector< Synapse >::iterator
					i = synapses_.begin(); i != synapses_.end(); ++i )
			i->setHandler( this );

	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();

	return *this;
}

void SimpleSynHandler::vSetNumSynapses( const unsigned int v )
{
	unsigned int prevSize = synapses_.size();
	synapses_.resize( v );
	for ( unsigned int i = prevSize; i < v; ++i )
		synapses_[i].setHandler( this );
}

unsigned int SimpleSynHandler::vGetNumSynapses() const
{
	return synapses_.size();
}

Synapse* SimpleSynHandler::vGetSynapse( unsigned int i )
{
	static Synapse dummy;
	if ( i < synapses_.size() )
		return &synapses_[i];
	cout << "Warning: SimpleSynHandler::getSynapse: index: " << i <<
		" is out of range: " << synapses_.size() << endl;
	return &dummy;
}

void SimpleSynHandler::addSpike(
				unsigned int index, double time, double weight )
{
	assert( index < synapses_.size() );
	events_.push( SynEvent( time, weight ) );
}

void SimpleSynHandler::vProcess( const Eref& e, ProcPtr p )
{
	double activation = 0.0;
	while( !events_.empty() && events_.top().time <= p->currTime ) {
        // Send out weight / dt for every spike
        //      Since it is an impulse active only for one dt,
        //      need to send it divided by dt.
        // Can connect activation to SynChan (double exp)
        //      or to LIF as an impulse to voltage.
        // See: http://www.genesis-sim.org/GENESIS/Hyperdoc/Manual-26.html#synchan
		activation += events_.top().weight / p->dt;
		events_.pop();
	}
	if ( activation != 0.0 )
		SynHandlerBase::activationOut()->send( e, activation );
}

void SimpleSynHandler::vReinit( const Eref& e, ProcPtr p )
{
	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();
}

unsigned int SimpleSynHandler::addSynapse()
{
	unsigned int newSynIndex = synapses_.size();
	synapses_.resize( newSynIndex + 1 );
	synapses_[newSynIndex].setHandler( this );
	return newSynIndex;
}


void SimpleSynHandler::dropSynapse( unsigned int msgLookup )
{
	assert( msgLookup < synapses_.size() );
	synapses_[msgLookup].setWeight( -1.0 );
}
