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
		void setResponseScale( double v );
 		double getResponseScale() const;
 		double getSeqActivation() const; // summed activation of syn chan
		void setWeightScale( double v );
 		double getWeightScale() const;
 		vector< double > getWeightScaleVec() const;
 		vector< double > getKernel() const;
 		vector< double > getHistory() const;

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

		// Time to store history. KernelDt defines num of rows
		double historyTime_;
		double seqDt_;	// Time step for successive entries in kernel
		// Scaling factor for sustained activation of synapse from response
		double responseScale_;
		// Scaling factor for weight changes in each synapse from response
		double weightScale_;

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
		vector< vector<  double > > kernel_;	//Kernel for seq selectivity
		RollingMatrix history_;	// Rows = time; cols = synInputs

		vector< Synapse > synapses_;
		priority_queue< PreSynEvent, vector< PreSynEvent >, CompareSynEvent > events_;


};

#endif // _SEQ_SYN_HANDLER_H
