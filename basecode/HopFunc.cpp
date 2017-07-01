/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"
#include "HopFunc.h"
#include "../mpi/PostMaster.h"
#include "../shell/Shell.h"

static double testBuf[4096];
static double* addToTestBuf( const Eref& e, unsigned int i,
				unsigned int size )
{
	TgtInfo* tgt = reinterpret_cast< TgtInfo* >( &testBuf[0] );
	tgt->set( e.objId(), i, size );
	return &testBuf[ TgtInfo::headerSize ];
}

const double* checkHopFuncTestBuffer()
{
	return &testBuf[0];
}


double* addToBuf( const Eref& er, HopIndex hopIndex, unsigned int size )
{
	static ObjId oi( 3 );
	static PostMaster* p = reinterpret_cast< PostMaster* >( oi.data() );
	if ( hopIndex.hopType() == MooseSendHop ) {
		return p->addToSendBuf( er, hopIndex.bindIndex(), size );
	} else if ( hopIndex.hopType() == MooseSetHop ||
			 hopIndex.hopType() == MooseSetVecHop ) {
		p->clearPendingSetGet(); // Cannot touch set buffer if pending.
		return p->addToSetBuf( er, hopIndex.bindIndex(),
						size, hopIndex.hopType() );
	} else if ( hopIndex.hopType() == MooseTestHop ) {
		return addToTestBuf( er, hopIndex.bindIndex(), size );
	}
	assert( 0 ); // Should not get here.
	return 0;
}

void dispatchBuffers( const Eref& e, HopIndex hopIndex )
{
	static ObjId oi( 3 );
	static PostMaster* p = reinterpret_cast< PostMaster* >( oi.data() );
	if ( Shell::numNodes() == 1 )
		return;
	if ( hopIndex.hopType() == MooseSetHop ||
	  	hopIndex.hopType() == MooseGetHop ) {
		p->dispatchSetBuf( e );
	}
	if ( hopIndex.hopType() == MooseSetVecHop ) {
		p->dispatchSetBuf( e );
	}
	// More complicated stuff for get operations.
}

double* remoteGet( const Eref& e, unsigned int bindIndex )
{
	static ObjId oi( 3 );
	static PostMaster* p = reinterpret_cast< PostMaster* >( oi.data() );
	return p->remoteGet( e, bindIndex );
}

void remoteGetVec( const Eref& e, unsigned int bindIndex,
				vector< vector< double > >& getRecvBuf,
				vector< unsigned int >& numOnNode )
{
	static ObjId oi( 3 );
	static PostMaster* p = reinterpret_cast< PostMaster* >( oi.data() );
	p->remoteGetVec( e, bindIndex, getRecvBuf, numOnNode );
}

void remoteFieldGetVec( const Eref& e, unsigned int bindIndex,
				vector< double >& getRecvBuf )
{
	static ObjId oi( 3 );
	static PostMaster* p = reinterpret_cast< PostMaster* >( oi.data() );
	p->remoteFieldGetVec( e, bindIndex, getRecvBuf );
}

unsigned int mooseNumNodes()
{
	return Shell::numNodes();
}

unsigned int mooseMyNode()
{
	return Shell::myNode();
}
