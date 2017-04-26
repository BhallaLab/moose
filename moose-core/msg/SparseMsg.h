/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SPARSE_MSG_H
#define _SPARSE_MSG_H

/**
 * This is a parallelized sparse message.
 * It is a general message type optimized for sparse matrix like
 * projection patterns. For each source object[DataId] there can be a
 * target object[DataId].
 * For parallel/multithreaded use, we need to guarantee that all requests
 * to the same target object (and all its synapses) are on the same queue.
 * So it builds up a separate SparseMatrix for each thread.
 *
 * It has a function to do the node/thread decomposition to generate an
 * equivalent of the original sparse matrix, but using only the appropriate
 * RNG seed.
 *
 * A typical case is from an array of IntFire objects to an array of 
 * Synapses, which are array fields of IntFire objects.
 * The sparse connectivity maps between the source IntFire and target
 * Synapses.
 * The location of the entry in the sparse matrix provides the index of
 * the target IntFire.
 * The data value in the sparse matrix provides the index of the Synapse
 * at that specific connection.
 * This assumes that only one Synapse mediates a given connection between
 * any two IntFire objects.
 *
 * It is optimized for input coming on Element e1, and going to Element e2.
 * If you expect any significant backward data flow, please use 
 * BiSparseMsg.
 * It can be modified after creation to add or remove message entries.
 */
class SparseMsg: public Msg
{
	friend unsigned int Msg::initMsgManagers(); // for initializing Id.
	public:
		SparseMsg( Element* e1, Element* e2, unsigned int msgIndex );
		~SparseMsg();

		Eref firstTgt( const Eref& src ) const;

		void sources( vector< vector< Eref > >& v ) const;
		void targets( vector< vector< Eref > >& v ) const;
		
		unsigned int randomConnect( double probability );

		Id managerId() const;

		ObjId findOtherEnd( ObjId end ) const;

		Msg* copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const;

		/**
		 * Assigns the whole connection matrix
		 */
		void setMatrix( const SparseMatrix< unsigned int >& m );

		/**
		 * Returns the connection matrix
		 */
		SparseMatrix< unsigned int >& getMatrix();

		// Uses default addToQ function.

		/////////////////////////////////////////////////////////////////
		// Here we define the Element interface functions for SparseMsg
		/////////////////////////////////////////////////////////////////
		void setRandomConnectivity( double probability, long seed );
		double getProbability() const;
		void setProbability( double value );

		long getSeed() const;
		void setSeed( long value );

		void setEntry( unsigned int row, unsigned int column, 
			unsigned int value );

		void unsetEntry( unsigned int row, unsigned int column );

		// Still need to implement array field gets.

		unsigned int getNumRows() const;
		unsigned int getNumColumns() const;
		unsigned int getNumEntries() const;
		void clear();
		void transpose();

		/**
		 * Fills up the entire message based on pairs of src,dest (i.e.,
		 * row,column) values. All filled entries are set to zero.
		 */
		void pairFill( vector< unsigned int > src, 
						vector< unsigned int> dest );

		/**
		 * Fills up the entire message based on triplets of 
		 * src,destDataIndex,destFieldIndex
		 */
		void tripletFill( vector< unsigned int > src, 
					vector< unsigned int> dest,
					vector< unsigned int > field );
		
		/**
		 * Utility function to update all sorts of values after we've
		 * rebuilt the matrix.
		 */
		void updateAfterFill();

		/// Msg lookup functions
		static unsigned int numMsg();
		static char* lookupMsg( unsigned int index );

		static const Cinfo* initCinfo();

	private:
		SparseMatrix< unsigned int > matrix_;
		unsigned int numThreads_; // Number of threads to partition
		unsigned int nrows_; // The original size of the matrix.
		double p_;
		unsigned long seed_;
		static Id managerId_; // The Element that manages Sparse Msgs.
		static vector< SparseMsg* > msg_;
};

#endif // _SPARSE_MSG_H
