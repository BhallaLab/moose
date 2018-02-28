/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _STDP_SYN_HANDLER_H
#define _STDP_SYN_HANDLER_H

/*
class PreSynEvent: public SynEvent
{
	public:
		PreSynEvent()
			: SynEvent(),   // call the parent constructor with default args
                            // by default calls without args, so no need really
              synIndex( 0 )
		{}

		PreSynEvent( unsigned int i, double t, double w )
			: SynEvent(t,w),// call the parent constructor with given args
              synIndex( i )
		{;}

        unsigned int synIndex;
};

class PostSynEvent
{
	public:
		PostSynEvent()
			: time( 0.0 )
		{;}

		PostSynEvent( double t )
			: time( t )
		{;}

		double time;
};

struct ComparePostSynEvent
{
	bool operator()(const PostSynEvent& lhs, const PostSynEvent& rhs) const
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
class STDPSynHandler: public SynHandlerBase
{
	public:
		STDPSynHandler();
		~STDPSynHandler();
		STDPSynHandler& operator=( const STDPSynHandler& other );

		////////////////////////////////////////////////////////////////
		// Inherited virtual functions from SynHandlerBase
		////////////////////////////////////////////////////////////////
		void vSetNumSynapses( unsigned int num );
		unsigned int vGetNumSynapses() const;
		STDPSynapse* vGetSynapse( unsigned int i );
		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );
		/// Adds a new synapse, returns its index.
		unsigned int addSynapse();
		void dropSynapse( unsigned int droppedSynNumber );
		void addSpike( unsigned int index, double time, double weight );
		double getTopSpike( unsigned int index ) const;
		////////////////////////////////////////////////////////////////
		void addPostSpike( const Eref& e, double time );

		void setAPlus0( double v );
		double getAPlus0() const;
		void setTauPlus( double v );
		double getTauPlus() const;
        // note: APlus is in STDPSynapse as
        // it is different for each presynaptic neuron
        // connected to this 'aggregate-synapse'

		void setAMinus( double v );
		double getAMinus() const;
		void setTauMinus( double v );
		double getTauMinus() const;
		void setAMinus0( double v );
		double getAMinus0() const;

		void setWeightMax( double v );
		double getWeightMax() const;
		void setWeightMin( double v );
		double getWeightMin() const;

		static const Cinfo* initCinfo();
	private:
		vector< STDPSynapse > synapses_;
		priority_queue< PreSynEvent, vector< PreSynEvent >, CompareSynEvent > events_;
		priority_queue< PostSynEvent, vector< PostSynEvent >, ComparePostSynEvent > postEvents_;
		double aMinus_;
		double aMinus0_;
        double tauMinus_;
		double aPlus0_;
        double tauPlus_;
        double weightMax_;
        double weightMin_;
};

#endif // _STDP_SYN_HANDLER_H
