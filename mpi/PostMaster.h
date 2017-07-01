/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This is how internode message passing works. I'll describe this at two
 * levels: the movement of data, and then the setup.
 *
 * Level 1: Movement of data.
 * 1. Object Sender sends out an argument A in a regular message.
 * 2. The SrcFinfo scans through targets in the MsgDigest via func,tgt pairs
 * 3. The off-node (Func,tgt-Eref) pair holds a HopFunc and a special
 * 	Eref. The main part of the Eref is the originating object. The
 * 	FieldIndex of the Eref is the target node.
 * 4. The HopFunc fills in the Send buffer of the postmaster. It uses
 * 	the target node info, stuffs in the ObjId of the originating object,
 * 	and converts and stuffs each of the arguments using Conv< A >.
 * 	Thus the Send buffer contents are a header with TgtInfo, and then the
 * 	actual arguments.
 * 5. This happens for all outgoing messages this cycle.
 * 6. Postmaster sends out buffers in process. It then waits for incoming
 * 	stuff in the recvBufs.
 * 7. The scene now shifts to the PostMaster on the remote node. In its
 * 	'process', the clearPending call is executed. It looks at the recvBuf
 * 	and extracts the tgtInfo. This tells it what the originating object
 * 	was, and what SrcFinfo to use for the outgoing message. !!!! fixme
 * 		(Currently I use BindIndex which ought to be right but is done
 * 		very indirectly. I need to check.)
 * 8. we call the SrcFinfo::sendBuffer call from the originating object.
 *  The sendBuffer call converts the arguments
 *  back from the bufer to their native form and despatches using the
 *  regular messaging. Note that the MsgDigest will have done the right
 *  thing here to set up the regular messaging even for off-node
 *  DataIndices on this element.
 * 9. Messages reach their targets.
 *
 * Level 2. Setup.
 * 1. Objects and messages set up the regular way. Objects may have
 * subsets of their Data arrays placed on different nodes. Messages
 * are globals, their contents are replicated on every node.
 * 2. When a SrcFinfo::send call is about to execute for the first time,
 * it is digested: Element::digestMessages. During this step each of the
 * messages is expanded by putTargetsInDigeest into target Erefs.
 * 3. The target Erefs are inspected in filterOffNodeTargets. Off-node
 * targets are removed, and we record each pair of Source DataId and node.
 * 4. We now call putOffNodeTargetsInDigest. This generates the
 * HopFunc by calling OpFunc::makeHopFunc with the fid of the SrcFinfo.
 * 5. putOffNodeTargetsInDigest then examines the Source/Node pairs and
 * creates HopFunc/Eref pairs which are stuffed into the msgDigest.
 * Note that these Erefs are the hacked version where the Eref specifies
 * the originating object, plus using its FieldIndex to specify the target
 * node.
 *
 * Possible optimization here would be to have a sendToAll buffer
 * that was filled when the digestMessages detected that a majority of
 * target nodes received a given message. A setup time complication, not
 * a runtime problem.
 */

#ifndef _POST_MASTER_H
#define _POST_MASTER_H

#ifdef USE_MPI
#include <mpi.h>
#endif

class TgtInfo {
	public:
		TgtInfo()
				: id_(),
				bindIndex_( 0 ), dataSize_( 0 )
		{;}

		Eref eref() const {
			return Eref( id_.eref() );
		}

		void set( ObjId id, unsigned int bindIndex, unsigned int size ) {
			id_ = id;
			bindIndex_ = bindIndex;
			dataSize_ = size;
		}

		unsigned int dataSize() const {
			return dataSize_;
		}

		unsigned int bindIndex() const {
			return bindIndex_;
		}

		static const unsigned int headerSize;
	private:
		ObjId id_;
		unsigned int bindIndex_;
		unsigned int dataSize_; // Does double duty for SetGet operations as a flag to indicate type of operation.
};

class PostMaster {
	public:
		PostMaster();
		unsigned int getNumNodes() const;
		unsigned int getMyNode() const;
		unsigned int getBufferSize() const;
		void setBufferSize( unsigned int size );
		void reinit( const Eref& e, ProcPtr p );
		void process( const Eref& e, ProcPtr p );

		/// All arrived messages and set calls are handled and cleared.
		void clearPending();
		/// Clears arrived set and get calls
		void clearPendingSetGet();
		/// Clears arrived messages.
		void clearPendingRecv();
		/// Checks that all sends have gone out
		void finalizeSends();

		/// Handles 'get' calls from another node, to an object on mynode.
		void handleRemoteGet( const Eref& e,
						const OpFunc* op, int requestingNode );

		void handleRemoteGetVec( const Eref& e,
						const OpFunc* op, int requestingNode );

		/// Returns pointer to Send buffer for filling in arguments.
		double* addToSendBuf( const Eref& e,
				unsigned int bindIndex, unsigned int size );
		/// Returns pointer to Set buffer for filling in arguments.
		double* addToSetBuf( const Eref& e,
			unsigned int opIndex, unsigned int size, unsigned int hopType );
		/// Sends off contets of Set buffer.
		void dispatchSetBuf( const Eref& e );

		/// Blocking call to get a value from a remote node.
		double* remoteGet( const Eref& e, unsigned int bindIndex );
		void remoteGetVec( const Eref& e, unsigned int bindIndex,
				vector< vector< double > >& getRecvBuf,
				vector< unsigned int >& size );
		void remoteFieldGetVec( const Eref& e, unsigned int bindIndex,
				vector< double >& getRecvBuf );

		static const unsigned int reserveBufSize;
		static const unsigned int setRecvBufSize;
		static const int MSGTAG;
		static const int SETTAG;
		static const int GETTAG;
		static const int RETURNTAG;
		static const int CONTROLTAG;
		static const int DIETAG;
		static const Cinfo* initCinfo();
	private:
		unsigned int recvBufSize_;
		// Used on master for sending, on others for receiving.
		vector< double > setSendBuf_;
		vector< double > setRecvBuf_;
		vector< vector< double > > sendBuf_;
		vector< vector< double > > recvBuf_;
		vector< unsigned int > sendSize_;
		vector< double > getHandlerBuf_; // Just enough for one TgtInfo.
#ifdef USE_MPI
		MPI_Request setSendReq_;
		MPI_Request setRecvReq_;
		MPI_Request getHandlerReq_;
		MPI_Status setSendStatus_;
		MPI_Status setRecvStatus_;
		vector< MPI_Request > recvReq_;
		vector< MPI_Request > sendReq_;
		vector< MPI_Status > doneStatus_;
#endif // USE_MPI
		vector< int > doneIndices_;
		int isSetSent_;
		int isSetRecv_;
		int setSendSize_;
		unsigned int numRecvDone_;
};

#endif	// _POST_MASTER_H
