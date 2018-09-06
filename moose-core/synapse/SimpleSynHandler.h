/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SIMPLE_SYN_HANDLER_H
#define _SIMPLE_SYN_HANDLER_H

#include <queue>

/*
class SynEvent
{
	public:
		SynEvent()
			: time( 0.0 ), weight( 0.0 )
		{;}

		SynEvent( double t, double w )
			: time( t ), weight( w )
		{;}

		double time;
		double weight;
};

struct CompareSynEvent
{
	bool operator()(const SynEvent& lhs, const SynEvent& rhs) const
	{
		// Note that this is backwards. We want the smallest timestamp
		// on the top of the events priority_queue.
		return lhs.time > rhs.time;
	}
};
*/

/**
 * This handles simple synapses without plasticity. It uses a priority
 * queue to manage them. This gets inefficient for large numbers of
 * synapses but is pretty robust.
 */
class SimpleSynHandler: public SynHandlerBase
{
	public:
		SimpleSynHandler();
		~SimpleSynHandler();
		SimpleSynHandler& operator=( const SimpleSynHandler& other );

		////////////////////////////////////////////////////////////////
		// Inherited virtual functions
		////////////////////////////////////////////////////////////////
		void vSetNumSynapses( unsigned int num );
		unsigned int vGetNumSynapses() const;
		Synapse* vGetSynapse( unsigned int i );
		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );
		/// Adds a new synapse, returns its index.
		unsigned int addSynapse();
		void dropSynapse( unsigned int droppedSynNumber );
		void addSpike( unsigned int index, double time, double weight );
		double getTopSpike( unsigned int index ) const;
		////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	private:
		vector< Synapse > synapses_;
		priority_queue< SynEvent, vector< SynEvent >, CompareSynEvent > events_;
};

#endif // _SIMPLE_SYN_HANDLER_H
