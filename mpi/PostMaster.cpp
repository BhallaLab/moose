/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "PostMaster.h"
#include "../shell/Shell.h"

const unsigned int TgtInfo::headerSize =
		1 + ( sizeof( TgtInfo ) - 1 )/sizeof( double );

const unsigned int PostMaster::reserveBufSize = 1048576;
const unsigned int PostMaster::setRecvBufSize = 1048576;
const int PostMaster::MSGTAG = 1;
const int PostMaster::SETTAG = 2;
const int PostMaster::GETTAG = 3;
const int PostMaster::RETURNTAG = 4;
const int PostMaster::CONTROLTAG = 5;
const int PostMaster::DIETAG = 100;
PostMaster::PostMaster()
		:
				recvBufSize_( reserveBufSize ),
				setSendBuf_( setRecvBufSize, 0 ),
				setRecvBuf_( setRecvBufSize, 0 ),
				sendBuf_( Shell::numNodes() ),
				recvBuf_( Shell::numNodes() ),
				sendSize_( Shell::numNodes(), 0 ),
				getHandlerBuf_( TgtInfo::headerSize, 0 ),
				doneIndices_( Shell::numNodes(), 0 ),
				isSetSent_( 1 ), // Flag. Have any pending 'set' gone?
				isSetRecv_( 0 ), // Flag. Has some data come in?
				setSendSize_( 0 ),
				numRecvDone_( 0 )
{
	for ( unsigned int i = 0; i < Shell::numNodes(); ++i ) {
		sendBuf_[i].resize( reserveBufSize, 0 );
	}
#ifdef USE_MPI
	MPI_Barrier( MPI_COMM_WORLD );
	// Post recv for set calls
	MPI_Irecv( &setRecvBuf_[0], setRecvBufSize, MPI_DOUBLE,
					MPI_ANY_SOURCE,
					SETTAG, MPI_COMM_WORLD,
					&setRecvReq_
	);
	// Post recv for get calls.
	MPI_Irecv( &getHandlerBuf_[0], TgtInfo::headerSize, MPI_DOUBLE,
					MPI_ANY_SOURCE,
					GETTAG, MPI_COMM_WORLD,
					&getHandlerReq_
	);
	recvReq_.resize( Shell::numNodes() );
	sendReq_.resize( Shell::numNodes() );
	unsigned int k = 0;
	for ( unsigned int i = 0; i < Shell::numNodes(); ++i ) {
		// Set up the Recv already for later sends. This might be a problem
		// for some polling-based implementations, but let's try for now.
		MPI_Status temp;
		temp.MPI_SOURCE = temp.MPI_TAG = temp.MPI_ERROR = 0;
		doneStatus_.resize( Shell::numNodes(), temp );
		if ( i != Shell::myNode() ) {
			recvBuf_[i].resize( recvBufSize_, 0 );
			MPI_Irecv( &recvBuf_[i][0], recvBufSize_, MPI_DOUBLE,
				i, MSGTAG, MPI_COMM_WORLD,
				&recvReq_[k++]
				// Need to be careful about contiguous indexing for
				// the MPI_request array.
			);
		}
	}
#endif
}

///////////////////////////////////////////////////////////////
// Moose class stuff.
///////////////////////////////////////////////////////////////
const Cinfo* PostMaster::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ReadOnlyValueFinfo< PostMaster, unsigned int > numNodes(
			"numNodes",
			"Returns number of nodes that simulation runs on.",
			&PostMaster::getNumNodes
		);
		static ReadOnlyValueFinfo< PostMaster, unsigned int > myNode(
			"myNode",
			"Returns index of current node.",
			&PostMaster::getMyNode
		);
		static ValueFinfo< PostMaster, unsigned int > bufferSize(
			"bufferSize",
			"Size of the send a receive buffers for each node.",
			&PostMaster::setBufferSize,
			&PostMaster::getBufferSize
		);
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new EpFunc1< PostMaster, ProcPtr >( &PostMaster::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new EpFunc1< PostMaster, ProcPtr >( &PostMaster::reinit ) );

		//////////////////////////////////////////////////////////////
		// SharedFinfo Definitions
		//////////////////////////////////////////////////////////////
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* postMasterFinfos[] = {
		&numNodes,	// ReadOnlyValue
		&myNode,	// ReadOnlyValue
		&bufferSize,	// ReadOnlyValue
		&proc		// SharedFinfo
	};

	static Dinfo< PostMaster > dinfo;
	static Cinfo postMasterCinfo (
		"PostMaster",
		Neutral::initCinfo(),
		postMasterFinfos,
		sizeof( postMasterFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &postMasterCinfo;
}

/**
 * Finalize all outgoing messages, clear the sendSize vector so that we
 * can handle another round of messages.
 */
void PostMaster::finalizeSends()
{
#ifdef USE_MPI
	static vector< MPI_Status > status( Shell::numNodes() );
	MPI_Waitall( Shell::numNodes() -1, &sendReq_[0], &status[0] );
	for (unsigned int i = 0; i < Shell::numNodes(); ++i ) {
		sendSize_[i] = 0;
	}
#endif
}

//
/**
 * PostMaster class: handles cross-node messaging using MPI.
 * Identical to the Process call: sends out what needs to go, and then
 * waits for any incoming messages and passes them on.
 */
void PostMaster::reinit( const Eref& e, ProcPtr p )
{
#ifdef USE_MPI
	// MPI_Barrier( MPI_COMM_WORLD );
	unsigned int reqIndex = 0;
	for ( unsigned int i = 0; i < Shell::numNodes(); ++i )
	{
		if ( i == Shell::myNode() ) continue;
		// MPI_scatter would have been better but it doesn't allow
		// one to post larger recvs than the actual data sent.
		MPI_Isend(
			&sendBuf_[i][0], sendSize_[i], MPI_DOUBLE,
			i, 		// Where to send to.
			MSGTAG, MPI_COMM_WORLD,
			&sendReq_[ reqIndex++ ]
		);
		clearPending(); // Try to interleave communications.
	}
	while ( numRecvDone_ < Shell::numNodes() -1 )
		clearPending();
	finalizeSends();
	MPI_Barrier( MPI_COMM_WORLD );
	numRecvDone_ = 0;
#endif
}

void PostMaster::process( const Eref& e, ProcPtr p )
{
#ifdef USE_MPI
	unsigned int reqIndex = 0;
	for ( unsigned int i = 0; i < Shell::numNodes(); ++i )
	{
		if ( i == Shell::myNode() ) continue;
		// MPI_scatter would have been better but it doesn't allow
		// one to post larger recvs than the actual data sent.
		MPI_Isend(
			&sendBuf_[i][0], sendSize_[i], MPI_DOUBLE,
			i, 		// Where to send to.
			MSGTAG, MPI_COMM_WORLD,
			&sendReq_[ reqIndex++ ]
		);
		clearPending(); // Try to interleave communications.
	}
	while ( numRecvDone_ < Shell::numNodes() -1 )
		clearPending();
	finalizeSends();
	MPI_Barrier( MPI_COMM_WORLD );
	numRecvDone_ = 0;
#endif
}

void PostMaster::clearPending()
{
	if ( Shell::numNodes() == 1 )
		return;
	clearPendingSetGet();
	clearPendingRecv();
}

void PostMaster::handleRemoteGet(
				const Eref& e, const OpFunc* op, int requestingNode )
{
#ifdef USE_MPI
	static double getReturnBuf[reserveBufSize];
	op->opBuffer( e, &getReturnBuf[0] ); // stuff return value into buf.
	// Send out the data. Blocking. Don't want any other gets till done
	int size = getReturnBuf[0];
	MPI_Send(
		&getReturnBuf[1], size, MPI_DOUBLE,
		requestingNode, 		// Where to send to.
		RETURNTAG, MPI_COMM_WORLD
	);
#endif // USE_MPI
}

/**
 * Collects all the values and stuffs into getReturnBuf.
 * Returns size of all contents of getReturnBuf, in doubles.
 * Puts number of returned values in getReturnBuf[0]. Note that these
 * are likely to differ if the values returned are not doubles.
 * Examines the eref to decide if this is a DataElement or a
 * FieldElement. If a DataElement, scans through all data entries to fill
 * the returnBuf. If a FieldElement, fills in field entries only of the
 * one specified DataId on this eref.
 */
int innerGetVec( const Eref& e, const OpFunc* op,
			   double* getReturnBuf	)
{
	static double buf[PostMaster::reserveBufSize];
	// Would like to use eref iterator here.
	Element* elm = e.element();
	unsigned int start = elm->localDataStart();
	int k = 1; // first entry is for numOnNode;
	if ( elm->hasFields() ) {
		DataId di = e.dataIndex();
		unsigned int numField = elm->numField( di - start );
		getReturnBuf[0] = numField;
		for ( unsigned int j = 0; j < numField; ++j ) {
			Eref er( elm, di, j );
			// stuff return value into buf.
			op->opBuffer( er, buf );
			unsigned int size = buf[0];
			memcpy( &getReturnBuf[k], &buf[1], size * sizeof( double ) );
			k += size;
		}
	} else {
		unsigned int end = start + elm->numLocalData();
		getReturnBuf[0] = elm->numLocalData();
		for ( unsigned int i = start; i < end; ++i ) {
			Eref er( elm, i, 0 );
			// stuff return value into buf.
			op->opBuffer( er, buf );
			unsigned int size = buf[0];
			memcpy( &getReturnBuf[k], &buf[1], size * sizeof( double ) );
			k += size;
		}
	}
	return k;
}

void PostMaster::handleRemoteGetVec(
				const Eref& e, const OpFunc* op, int requestingNode )
{
#ifdef USE_MPI
	static double getReturnBuf[reserveBufSize];
	int k = innerGetVec( e, op, getReturnBuf );
	// Send out the data. Blocking. Don't want any other gets till done
	MPI_Send(
		&getReturnBuf[0], k, MPI_DOUBLE,
		requestingNode, 		// Where to send to.
		RETURNTAG, MPI_COMM_WORLD
	);
#endif // USE_MPI
}

void PostMaster::clearPendingSetGet()
{
#ifdef USE_MPI
	// isSetSent_ is checked before doing another x-node set operation
	// in dispatchSetBuf.
	if ( !isSetSent_ ) {
		MPI_Test( &setSendReq_, &isSetSent_, &setSendStatus_ );
		assert ( isSetSent_ != MPI_UNDEFINED );
	}

	MPI_Test( &setRecvReq_, &isSetRecv_, &setRecvStatus_ );
	if ( isSetRecv_ && isSetRecv_ != MPI_UNDEFINED )
	{
		isSetRecv_ = 0;
		int requestingNode = setRecvStatus_.MPI_SOURCE;
		int count = 0;
		MPI_Get_count( &setRecvStatus_, MPI_DOUBLE, &count );
		// Immediately post another Recv. Needed because we may call
		// the clearPendingSetGet() function recursively. So copy
		// data to another buffer first.
		vector< double > temp( setRecvBuf_.begin(),
						setRecvBuf_.begin() + count );
		MPI_Irecv( &setRecvBuf_[0], setRecvBufSize, MPI_DOUBLE,
						MPI_ANY_SOURCE,
						SETTAG, MPI_COMM_WORLD,
						&setRecvReq_
		);

		double* buf = &temp[0];
		// Handle arrived Set call
		const TgtInfo* tgt = reinterpret_cast< const TgtInfo * >( buf );
		const Eref& e = tgt->eref();
		const OpFunc *op = OpFunc::lookop( tgt->bindIndex() );
		assert( op );
		if ( tgt->dataSize() == MooseSetHop ) {
			op->opBuffer( e, buf + TgtInfo::headerSize );
		} else if ( tgt->dataSize() == MooseSetVecHop ) {
			op->opVecBuffer( e, buf + TgtInfo::headerSize );
		} else if ( tgt->dataSize() == MooseGetHop ) {
			handleRemoteGet( e, op, requestingNode );
		} else if ( tgt->dataSize() == MooseGetVecHop ) {
			handleRemoteGetVec( e, op, requestingNode );
		}
	}
#endif // USE_MPI
}

/*
// Handles incoming 'get' request and posts stuff back to requestor.
void PostMaster::clearPendingGet()
{
	static double getReturnBuf[reserveBufSize];
	static MPI_Status getReturnStatus;
	int getRequestArrived = 0;
#ifdef USE_MPI
	MPI_Test( &getHandlerReq_, &getRequestArrived, &getReturnStatus );
	if ( getRequestArrived && getRequestArrived != MPI_UNDEFINED )
	{
		int requestingNode = getReturnStatus.MPI_SOURCE;
		TgtInfo* tgt = reinterpret_cast< TgtInfo* >( &getHandlerBuf_[0] );
		const Eref& e = tgt->fullEref();
		const OpFunc *op = OpFunc::lookop( tgt->bindIndex() );
		assert( op );
		op->opBuffer( e, &getReturnBuf[0] ); // stuff return value into buf.

		// Refresh the handler for incoming get requests.
		MPI_Irecv( &getHandlerBuf_[0], TgtInfo::headerSize, MPI_DOUBLE,
						MPI_ANY_SOURCE,
						GETTAG, MPI_COMM_WORLD,
						&getHandlerReq_
		);

		// Send out the data. Blocking. Don't want any other gets till done
		int size = getReturnBuf[0];
		MPI_Send(
			&getReturnBuf[1], size, MPI_DOUBLE,
			requestingNode, 		// Where to send to.
			RETURNTAG, MPI_COMM_WORLD
		);
	}
#endif // USE_MPI
}
*/

void PostMaster::clearPendingRecv()
{
#ifdef USE_MPI
	int done = 0;
	bool report = false; // for debugging
	MPI_Testsome( Shell::numNodes() -1, &recvReq_[0], &done,
					&doneIndices_[0], &doneStatus_[0] );
	if ( done == MPI_UNDEFINED )
		return;
	for ( int i = 0; i < done; ++i ) {
		int doneIndex = doneIndices_[i];
		unsigned int recvNode = doneIndex;
		if ( recvNode >= Shell::myNode() )
			recvNode += 1; // Skip myNode
		int recvSize = 0;
		MPI_Get_count( &doneStatus_[i], MPI_DOUBLE, &recvSize );
		int j = 0;
		assert( recvSize <= static_cast< int >( recvBufSize_ ) );
		double* buf = &recvBuf_[ recvNode ][0];
		if ( report ) {
			for ( int j = 0; j < recvSize; j += 4 ) {
				TgtInfo* tgt = reinterpret_cast< TgtInfo * >(buf + j);
				cout << j / 4 << ": " << tgt->eref().dataIndex() << ", " <<
					   	buf[j+3] << endl;
			}
		}
		while ( j < recvSize ) {
			const TgtInfo* tgt = reinterpret_cast< const TgtInfo * >( buf );
			const Eref& e = tgt->eref();
			const Finfo *f =
				e.element()->cinfo()->getSrcFinfo( tgt->bindIndex() );
			buf += TgtInfo::headerSize;
			const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( f );
			assert( sf );
			sf->sendBuffer( e, buf );
			buf += tgt->dataSize();
			j += TgtInfo::headerSize + tgt->dataSize();
			assert( buf - &recvBuf_[recvNode][0] == j );
		}
		// Post the next Irecv.
		unsigned int k = recvNode;
		if ( recvNode > Shell::myNode() )
			k--;
		MPI_Irecv( &recvBuf_[recvNode][0],
						recvBufSize_, MPI_DOUBLE,
						recvNode,
						MSGTAG, MPI_COMM_WORLD,
						&recvReq_[ k ]
						// Ensure we have contiguous entries in recvReq_
				 );
	}
	numRecvDone_ += done;
#endif
}

///////////////////////////////////////////////////////////////
// Data transfer and fillup operations.
///////////////////////////////////////////////////////////////

double* PostMaster::addToSendBuf( const Eref& e, unsigned int bindIndex,
		unsigned int size )
{
	unsigned int node = e.fieldIndex(); // nasty evil wicked hack
	unsigned int end = sendSize_[node];
	if ( end + TgtInfo::headerSize + size > recvBufSize_ ) {
		// Here we need to activate the fallback second send which will
		// deal with the big block. Also various routines for tracking
		// send size so we don't get too big or small.
		cerr << "Error: PostMaster::addToSendBuf on node " <<
				Shell::myNode() <<
				": Data size (" << size << ") goes past end of buffer\n";
		assert( 0 );
	}
	TgtInfo* tgt = reinterpret_cast< TgtInfo* >( &sendBuf_[node][end] );
	tgt->set( e.objId(), bindIndex, size );
	end += TgtInfo::headerSize;
	sendSize_[node] = end + size;
	return &sendBuf_[node][end];
}

double* PostMaster::addToSetBuf( const Eref& e, unsigned int opIndex,
						unsigned int size, unsigned int hopType )
{
	if ( TgtInfo::headerSize + size > setRecvBufSize ) {
		// Here we need to activate the fallback second send which will
		// deal with the big block. Also various routines for tracking
		// send size so we don't get too big or small.
		cerr << "Error: PostMaster::addToSetBuf on node " <<
				Shell::myNode() <<
				": Data size (" << size << ") goes past end of buffer\n";
		assert( 0 );
	}
	while ( isSetSent_ == 0 ) { // Can't add a set while old set is pending
		clearPending();
	}
	isSetSent_ = 0;
	TgtInfo* tgt = reinterpret_cast< TgtInfo* >( &setSendBuf_[0] );
	// tgt->set( e.id(), e.dataIndex(), opIndex, e.fieldIndex() );
	tgt->set( e.objId(), opIndex, hopType );
	unsigned int end = TgtInfo::headerSize;
	setSendSize_ = end + size;
	return &setSendBuf_[end];
}

void PostMaster::dispatchSetBuf( const Eref& e )
{
	assert ( e.element()->isGlobal() || e.getNode() != Shell::myNode() );
#ifdef USE_MPI
	if ( e.element()->isGlobal() ) {
		for ( unsigned int i = 0; i < Shell::numNodes(); ++i ) {
			if ( i != Shell::myNode() ) {
			// A bcast would be marginally more efficient, but would need
			// us to inform all target nodes to expect one. So just do
			// multiple sends.
				MPI_Isend(
					&setSendBuf_[0], setSendSize_, MPI_DOUBLE,
					i, 		// Where to send to.
					SETTAG, MPI_COMM_WORLD,
					&setSendReq_
		// Need to monitor all the sends to make sure they all complete
		// before permitting another 'set'
				);
			}
		}
	} else {
		MPI_Isend(
			&setSendBuf_[0], setSendSize_, MPI_DOUBLE,
				e.getNode(), 		// Where to send to.
				SETTAG, MPI_COMM_WORLD,
				&setSendReq_
		);
	}
#endif
}

/// This is a blocking call. However, it must not block other requests
// that come into the current node.
double* PostMaster::remoteGet( const Eref& e, unsigned int bindIndex )
{
	static double getRecvBuf[setRecvBufSize];
#ifdef USE_MPI
	static double getSendBuf[TgtInfo::headerSize];
	static MPI_Request getSendReq;
	static MPI_Request getRecvReq;
	static MPI_Status getSendStatus;

	while ( isSetSent_ == 0 ) {
			// Can't request a 'get' while old set is
			// pending, lest the 'get' depend on the 'set'.
		clearPending();
	}
	TgtInfo* tgt = reinterpret_cast< TgtInfo* >( &getSendBuf[0] );
	tgt->set( e.objId(), bindIndex, MooseGetHop );
	assert ( !e.element()->isGlobal() && e.getNode() != Shell::myNode() );
	// Post receive for return value.
	MPI_Irecv( 		&getRecvBuf[0],
					setRecvBufSize, MPI_DOUBLE,
					e.getNode(),
					RETURNTAG, MPI_COMM_WORLD,
					&getRecvReq
			 );
	// Now post send to request the data
	MPI_Isend(
		&getSendBuf[0], TgtInfo::headerSize, MPI_DOUBLE,
			e.getNode(), 		// Where to send to.
			SETTAG, MPI_COMM_WORLD,
			&getSendReq
	);
	int complete = 0;
	// Poll till the value comes back. We don't bother to
	// check what happened with the send.
	// While polling be sure to handle any other requests to avoid deadlock
	while ( !complete ) {
		MPI_Test( &getRecvReq, &complete, &getSendStatus );
		assert ( complete != MPI_UNDEFINED );
		clearPending();
	}
#endif
	return &getRecvBuf[0];
}

/// This is a blocking call. However, it must still handle other
//requests that come into the current node.
//  Here we request data only from the one node that holds the data,
//  since all field data is on a single DataEntry.
void PostMaster::remoteFieldGetVec( const Eref& e, unsigned int bindIndex,
	vector< double >& getRecvBuf )
{
#ifdef USE_MPI
	static double getSendBuf[TgtInfo::headerSize];
	static MPI_Request getSendReq;
	static MPI_Request getRecvReq;
	static MPI_Status doneStatus;
#endif
	unsigned int targetNode = e.getNode();
	assert( targetNode != Shell::myNode() );
	getRecvBuf.clear();
	getRecvBuf.resize( reserveBufSize );

#ifdef USE_MPI
	while ( isSetSent_ == 0 ) {
			// Can't request a 'get' while old set is
			// pending, lest the 'get' depend on the 'set'.
		clearPending();
	}
	TgtInfo* tgt = reinterpret_cast< TgtInfo* >( &getSendBuf[0] );
	tgt->set( e.objId(), bindIndex, MooseGetVecHop );
	assert ( !e.element()->isGlobal() );

	// Post receive for return value.
	MPI_Irecv( 	&getRecvBuf[0],
				reserveBufSize, MPI_DOUBLE,
				targetNode,
				RETURNTAG, MPI_COMM_WORLD,
				&getRecvReq
			);
	// Now post send to request the data
	MPI_Isend(
				&getSendBuf[0], TgtInfo::headerSize, MPI_DOUBLE,
				targetNode, 			// Where to send to.
				SETTAG, MPI_COMM_WORLD,
				&getSendReq
			);
	// Poll till the value comes back. We don't bother to
	// check what happened with the send.
	// While polling be sure to handle any other requests to avoid deadlock
	int done = 0;
	while( !done ) {
		MPI_Test( &getRecvReq, &done, &doneStatus );
		assert ( done != MPI_UNDEFINED );
		clearPending();
	}
	// Now we have the data back.
#endif
}

/// This is a blocking call. However, it must still handle other
//requests that come into the current node.
// getRecvBuf and size are already sized at numNodes.
// But getRecvBuf individual entries need to be sized.
void PostMaster::remoteGetVec( const Eref& e, unsigned int bindIndex,
	vector< vector< double > >& getRecvBuf,
	vector< unsigned int >& numOnNode )
{
#ifdef USE_MPI
	static double getSendBuf[TgtInfo::headerSize];
	static vector< MPI_Request > getSendReq( Shell::numNodes() );
	static vector< MPI_Request > getRecvReq( Shell::numNodes() );
	static vector< MPI_Status > doneStatus( Shell::numNodes() );
#endif
	static vector< double > temp( reserveBufSize, 0 );
	numOnNode.clear();
	numOnNode.resize( Shell::numNodes(), 0 );
	getRecvBuf.clear();
	getRecvBuf.resize( Shell::numNodes(), temp );

#ifdef USE_MPI
	while ( isSetSent_ == 0 ) {
			// Can't request a 'get' while old set is
			// pending, lest the 'get' depend on the 'set'.
		clearPending();
	}
	TgtInfo* tgt = reinterpret_cast< TgtInfo* >( &getSendBuf[0] );
	tgt->set( e.objId(), bindIndex, MooseGetVecHop );
	assert ( !e.element()->isGlobal() );

	unsigned int k = 0;
	for ( unsigned int i = 0; i < Shell::numNodes(); ++i ) {
		if ( i != Shell::myNode() ) {
			// Post receive for return value.
			MPI_Irecv( 	&getRecvBuf[i][0],
					reserveBufSize, MPI_DOUBLE,
					i,
					RETURNTAG, MPI_COMM_WORLD,
					&getRecvReq[k]
			);
			// Now post send to request the data
			MPI_Isend(
				&getSendBuf[0], TgtInfo::headerSize, MPI_DOUBLE,
				i, 			// Where to send to.
				SETTAG, MPI_COMM_WORLD,
				&getSendReq[k]
			);
			k++;
		}
	}
	// Poll till the value comes back. We don't bother to
	// check what happened with the send.
	// While polling be sure to handle any other requests to avoid deadlock
	int done = 0;
	unsigned int received = 0;
	vector< int > getDoneIndices( Shell::numNodes(), 0 );
	while( received < Shell::numNodes() - 1 ) {
		MPI_Testsome( Shell::numNodes() -1, &getRecvReq[0], &done,
					&getDoneIndices[0], &doneStatus[0] );
		if ( done == MPI_UNDEFINED )
			continue;
		received += done;
		for ( int i = 0; i < done; ++i ) {
			int doneIndex = getDoneIndices[i];
			unsigned int recvNode = doneIndex;
			if ( recvNode >= Shell::myNode() )
				recvNode += 1; // Skip myNode
			/*
			int recvSize = 0;
			MPI_Get_count( &doneStatus[doneIndex],
							MPI_DOUBLE, &recvSize );
			size[recvNode] = recvSize;
							*/
			numOnNode[recvNode] = getRecvBuf[recvNode][0];
		}
		clearPending();
	}
	// Now we have the whole mess back.
#endif
}

///////////////////////////////////////////////////////////////
// Fields
///////////////////////////////////////////////////////////////

unsigned int PostMaster::getNumNodes() const
{
	return Shell::numNodes();
}

unsigned int PostMaster::getMyNode() const
{
	return Shell::myNode();
}

unsigned int PostMaster::getBufferSize() const
{
	if ( sendBuf_.size() == 0 )
		return 0;

	return sendBuf_[0].size();
}

void PostMaster::setBufferSize( unsigned int size )
{
	for ( unsigned int i =0; i < sendBuf_.size(); ++i )
		sendBuf_[i].resize( size );
}
