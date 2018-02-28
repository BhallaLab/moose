/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2016 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SEQ_SYN_HANDLER_H
#define _SEQ_SYN_HANDLER_H

/**
 * This handles synapses organized sequentially. The parent class
 * SimpleSynHandler deals with the mechanics of data arrival.
 * Here the algorithm is
 * 0. Assume all synaptic input comes on a linear dendrite.
 * 1. Maintain a history of depth D for synaptic activity. May be simplest
 * to do as event list (sparse matrix) rather than full matrix.
 * 2. Maintain a kernel of how to weight time and space
 * 3. Here we have various options
 * 3.1 At each spike event, decide how to weight it based on history.
 * 3.2 At each timestep, compute an effective Gk and send to activation.
 * 	This will be moderately nasty to compute
 */
class SeqSynHandler: public SynHandlerBase
{
	public:
		SeqSynHandler();
		~SeqSynHandler();
		SeqSynHandler& operator=( const SeqSynHandler& other );

		////////////////////////////////////////////////////////////////
		// Over-ridden virtual functions
		////////////////////////////////////////////////////////////////
		void vSetNumSynapses( unsigned int num );
		unsigned int vGetNumSynapses() const;
		Synapse* vGetSynapse( unsigned int i );
		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );

		////////////////////////////////////////////////////////////////
		/// Adds a new synapse, returns its index.
		unsigned int addSynapse();
		void dropSynapse( unsigned int droppedSynNumber );
		void addSpike( unsigned int index, double time, double weight );
		double getTopSpike( unsigned int index ) const;
		////////////////////////////////////////////////////////////////
		// New fields.
		////////////////////////////////////////////////////////////////
		void setKernelEquation( string eq );
 		string getKernelEquation() const;
		void setKernelWidth( unsigned int v );
 		unsigned int getKernelWidth() const;
		void setSeqDt( double v );
 		double getSeqDt() const;
		void setHistoryTime( double v );
 		double getHistoryTime() const;
		void setBaseScale( double v );
 		double getBaseScale() const;
		void setSequenceScale( double v );
 		double getSequenceScale() const;
		void setSynapseOrder( vector< unsigned int> v );
 		vector< unsigned int> getSynapseOrder() const;
		void setSynapseOrderOption( int v );
 		int getSynapseOrderOption() const;
 		double getSeqActivation() const; // summed activation of syn chan
		void setPlasticityScale( double v );
 		double getPlasticityScale() const;
		void setSequencePower( double v );
 		double getSequencePower() const;
 		vector< double > getWeightScaleVec() const;
 		vector< double > getKernel() const;
 		vector< double > getHistory() const;

		////////////////////////////////////////////////////////////////
		// Utility func
		int numHistory() const;
		void refillSynapseOrder( unsigned int newSize );
		void fixSynapseOrder();
		////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	private:
		void updateKernel();
		/*
		 * Here I would like to put in a sparse matrix.
		 * Each timestep is a row
		 * Each column is a neuron
		 * Each value is the weight, though I could also look this up.
		 * I need to make it a circular buffer.
		 * The 'addRow' function inserts the non-zero entries representing
		 * 	neurons that are active on this timestep. As a circular buffer
		 * 	this needs to do some allocation juggling.
		 * Need a new function similar to computeRowRate, basically a
		 * dot product of input vector with specified row.
		 * Then run through all available places.
		 */
		string kernelEquation_;
		unsigned int kernelWidth_; // Width in terms of number of synapses

		/// Time to store history. KernelDt defines num of rows
		double historyTime_;
		double seqDt_;	// Time step for successive entries in kernel
		/// Scaling factor for baseline synaptic responses.
		double baseScale_;
		/// Scaling factor for sequence recognition responses.
		double sequenceScale_;

		/**
		 * Scaling factor for short-term plastic weight changes in each
		 * synapse arising from sequential input.
		 */
		double plasticityScale_;

		/**
		 * Exponent to use for the outcome of the sequential calculations.
		 * This is needed because linear summation of terms in the kernel
		 * means that a brief stong sequence match is no better than lots
		 * of successive low matches. In other words, 12345 is no better
		 * than 11111.
		 */
		double sequencePower_;

		///////////////////////////////////////////
		// Some readonly fields
		double seqActivation_; // global activation if sequence recognized

		// Weight scaling based on individual synapse sequence tuning.
		vector< double > weightScaleVec_;

		///////////////////////////////////////////
		// Tracks the spikes that came in recently, as input to correlation
		// analysis for sequence recognition.
		vector< double > latestSpikes_;

		///////////////////////////////////////////
		vector< vector<  double > > kernel_; ///Kernel for seq selectivity
		RollingMatrix history_;	/// Rows = time; cols = synInputs
		/**
		 * Remaps synapse order to avoid correlations based on presynaptic
		 * object Id order, or on connection building order. This
		 * undesirable ordering occurs even when random synaptic
		 * projections are used. User can alter as preferred. This is
		 * simply a look up table so that the incoming synapse index is
		 * remapped: index_on_Handler = synapseOrder_[original_syn_index]
		 * This must only have numbers from zero to numSynapses, and should
		 * normally have all the numbers.
		 */
		vector< unsigned int > synapseOrder_;

		/// Options: -2: User. -1: ordered. 0: random by system seed.
		/// > 0: Random by seed specified by this number
		int synapseOrderOption_;

		vector< Synapse > synapses_;
		priority_queue< PreSynEvent, vector< PreSynEvent >, CompareSynEvent > events_;


};

#endif // _SEQ_SYN_HANDLER_H
