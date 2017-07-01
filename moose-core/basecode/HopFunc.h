/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HOP_FUNC_H
#define _HOP_FUNC_H

double* addToBuf(
			const Eref& e, HopIndex hopIndex, unsigned int size );
void dispatchBuffers( const Eref& e, HopIndex hopIndex );
double* remoteGet( const Eref& e , unsigned int bindIndex );
void remoteGetVec( const Eref& e, unsigned int bindIndex,
				vector< vector< double > >& getRecvBuf,
				vector< unsigned int >& numOnNode );
void remoteFieldGetVec( const Eref& e, unsigned int bindIndex,
				vector< double >& getRecvBuf );
unsigned int mooseNumNodes();
unsigned int mooseMyNode();

/**
 * Function to hop across nodes. This one has no arguments, just tells the
 * remote object that an event has occurred.
 */
class HopFunc0: public OpFunc0Base
{
	public:
		HopFunc0( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}
		void op( const Eref& e ) const
		{
			addToBuf( e, hopIndex_, 0 );
			dispatchBuffers( e, hopIndex_ );
		}
	private:
		HopIndex hopIndex_;
};

// Function to hop across nodes, with one argument.
template < class A > class HopFunc1: public OpFunc1Base< A >
{
	public:
		HopFunc1( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}
		void op( const Eref& e, A arg ) const
		{
			double* buf = addToBuf( e, hopIndex_, Conv< A >::size( arg ) );
			Conv< A >::val2buf( arg, &buf );
			dispatchBuffers( e, hopIndex_ );
		}

		/// Executes the local vector assignment. Returns current arg index
		unsigned int localOpVec( Element* elm,
					const vector< A >& arg,
					const OpFunc1Base< A >* op,
					unsigned int k ) const
		{
			unsigned int numLocalData = elm->numLocalData();
			unsigned int start = elm->localDataStart();
			for ( unsigned int p = 0; p < numLocalData; ++p ) {
				unsigned int numField = elm->numField( p );
				for ( unsigned int q = 0; q < numField; ++q ) {
					Eref er( elm, p + start, q );
					op->op( er, arg[ k % arg.size() ] );
					k++;
				}
			}
			return k;
		}

		/// Executes the local vector assignment. Returns number of entries
		unsigned int localFieldOpVec( const Eref& er,
					const vector< A >& arg,
					const OpFunc1Base< A >* op )
				const
		{
			assert( er.getNode() == mooseMyNode() );
			unsigned int di = er.dataIndex();
			Element* elm = er.element();
			unsigned int numField =
					elm->numField( di - er.element()->localDataStart()  );
			for ( unsigned int q = 0; q < numField; ++q ) {
				Eref temp( elm, di, q );
				op->op( temp, arg[ q % arg.size() ] );
			}
			return numField;
		}

		/// Dispatches remote vector assignment. start and end are arg index
		unsigned int remoteOpVec( const Eref& er,
					const vector< A >& arg,
					const OpFunc1Base< A >* op,
					unsigned int start, unsigned int end ) const
		{
			unsigned int k = start;
			unsigned int nn = end - start;
			if ( mooseNumNodes() > 1 && nn > 0 ) {
				// nn includes dataIndices. FieldIndices are handled by
				// other functions.
					vector< A > temp( nn );
				// Have to do the insertion entry by entry because the
				// argument vector may wrap around.
				for ( unsigned int j = 0; j < nn; ++j ) {
					unsigned int x = k % arg.size();
					temp[j] = arg[x];
					k++;
				}
				double* buf = addToBuf( er, hopIndex_,
						Conv< vector< A > >::size( temp ) );
				Conv< vector< A > >::val2buf( temp, &buf );
				dispatchBuffers( er, hopIndex_ );
				// HopIndex says that it is a SetVec call.
			}
			return k;
		}

		void dataOpVec( const Eref& e, const vector< A >& arg,
				 const OpFunc1Base< A >* op ) const
		{
			Element* elm = e.element();
			vector< unsigned int > endOnNode( mooseNumNodes(), 0 );
			unsigned int lastEnd = 0;
			for ( unsigned int i = 0; i < mooseNumNodes(); ++i ) {
				endOnNode[i] = elm->getNumOnNode(i) + lastEnd;
				lastEnd = endOnNode[i];
			}
			unsigned int k = 0; // counter for index to arg vector.
			// The global case just sends all entries to all nodes.
			for ( unsigned int i = 0; i < mooseNumNodes(); ++i ) {
				if ( i == mooseMyNode() ) {
					k = localOpVec( elm, arg, op, k );
					assert( k == endOnNode[i] );
				} else {
					if ( !elm->isGlobal() ) {
						unsigned int start = elm->startDataIndex( i );
						if ( start < elm->numData() ) {
							Eref starter( elm,  start );
							assert( elm->getNode( starter.dataIndex() ) == i );
							k = remoteOpVec( starter, arg, op, k, endOnNode[i]);
						}
					}
				}
			}
			if ( elm->isGlobal() ) {
				Eref starter( elm,  0 );
				remoteOpVec( starter, arg, op, 0, arg.size() );
			}
		}

		void opVec( const Eref& er, const vector< A >& arg,
				 const OpFunc1Base< A >* op ) const
		{
			Element* elm = er.element();
			if ( elm->hasFields() ) {
				if ( er.getNode() == mooseMyNode() ) {
			// True for globals as well as regular objects on current node
					localFieldOpVec( er, arg, op );
				}
				if ( elm->isGlobal() || er.getNode() != mooseMyNode() ) {
					// Go just to the node where the fields reside, and
					// assign the vector there. May be all nodes if global.
					remoteOpVec( er, arg, op, 0, arg.size() );
				}
			} else {
				dataOpVec( er, arg, op );
			}
		}
	private:
		HopIndex hopIndex_;
};

/**
 * Deferred specification of function from OpFunc1Base, so it is after
 * the declaration of the HopFunc class to which it refers.
 */
template< class A >
const OpFunc* OpFunc1Base< A >::makeHopFunc( HopIndex hopIndex ) const
{
	return new HopFunc1< A >( hopIndex );
}

// Function to hop across nodes, with two arguments.
template < class A1, class A2 > class HopFunc2: public OpFunc2Base< A1, A2 >
{
        using OpFunc2Base<A1, A2>::opVec;

	public:
		HopFunc2( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}
		void op( const Eref& e, A1 arg1, A2 arg2 ) const
		{
			double* buf = addToBuf( e, hopIndex_,
				Conv< A1 >::size( arg1 ) + Conv< A2 >::size( arg2 ) );
			/*
			Conv< A1 >::val2buf( arg1, buf );
			Conv< A2 >::val2buf( arg2, buf + Conv< A1 >.size( arg1 ) );
			or
			buf = Conv< A1 >.val2buf( arg1, buf );
			Conv< A2 >::val2buf( arg2, buf );
			or
			*/
			Conv< A1 >::val2buf( arg1, &buf );
			Conv< A2 >::val2buf( arg2, &buf );
			dispatchBuffers( e, hopIndex_ );
		}

		void opVec( const Eref& e,
						const vector< A1 >& arg1,
						const vector< A1 >& arg2,
				 		const OpFunc2Base< A1, A2 >* op ) const
		{
			Element* elm = e.element();
			unsigned int k = 0; // counter for index to arg vector.
			if ( elm->isGlobal() ) {
				// Need to ensure that all nodes get the same args,
				// as opposed to below, where they are serial.
			}
			for ( unsigned int i = 0; i < mooseNumNodes(); ++i ) {
				if ( i == mooseMyNode() ) {
					unsigned int numData = elm->numLocalData();
					for ( unsigned int p = 0; p < numData; ++p ) {
						unsigned int numField = elm->numField( p );
						for ( unsigned int q = 0; q < numField; ++q ) {
							Eref er( elm, p, q );
							unsigned int x = k % arg1.size();
							unsigned int y = k % arg2.size();
							op->op( er, arg1[x], arg2[y] );
							k++;
						}
					}
				} else {
					unsigned int dataIndex = k;
					// nn includes dataIndices and if present fieldIndices
					// too. It may involve a query to the remote node.
					unsigned int nn = elm->getNumOnNode( i );
					vector< A1 > temp1( nn );
					vector< A2 > temp2( nn );
					// Have to do the insertion entry by entry because the
					// argument vectors may wrap around.
					for ( unsigned int j = 0; j < nn; ++j ) {
						unsigned int x = k % arg1.size();
						unsigned int y = k % arg2.size();
						temp1[j] = arg1[x];
						temp2[j] = arg2[y];
						k++;
					}
					double* buf = addToBuf( e, hopIndex_,
							Conv< vector< A1 > >::size( temp1 ) +
						   Conv< vector< A2 > >::size( temp2 ) );
					Conv< vector< A1 > >::val2buf( temp1, &buf );
					Conv< vector< A2 > >::val2buf( temp2, &buf );
					dispatchBuffers( Eref( elm, dataIndex ), hopIndex_ );
					// HopIndex says that it is a SetVec call.
				}
			}
		}
	private:
		HopIndex hopIndex_;
};

template< class A1, class A2 >
const OpFunc* OpFunc2Base< A1, A2 >::makeHopFunc(
				HopIndex hopIndex) const
{
	return new HopFunc2< A1, A2 >( hopIndex );
}

// Function to hop across nodes, with three arguments.
template < class A1, class A2, class A3 > class HopFunc3:
		public OpFunc3Base< A1, A2, A3 >
{
	public:
		HopFunc3( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}

		void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3 ) const
		{
			double* buf = addToBuf( e, hopIndex_,
				Conv< A1 >::size( arg1 ) + Conv< A2 >::size( arg2 ) +
				Conv< A3 >::size( arg3 ) );
			Conv< A1 >::val2buf( arg1, &buf );
			Conv< A2 >::val2buf( arg2, &buf );
			Conv< A3 >::val2buf( arg3, &buf );
			dispatchBuffers( e, hopIndex_ );
		}
	private:
		HopIndex hopIndex_;
};

template< class A1, class A2, class A3 >
const OpFunc* OpFunc3Base< A1, A2, A3 >::makeHopFunc(
				HopIndex hopIndex) const
{
	return new HopFunc3< A1, A2, A3 >( hopIndex );
}

// Function to hop across nodes, with three arguments.
template < class A1, class A2, class A3, class A4 > class HopFunc4:
		public OpFunc4Base< A1, A2, A3, A4 >
{
	public:
		HopFunc4( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}

		void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3, A4 arg4 ) const
		{
			double* buf = addToBuf( e, hopIndex_,
				Conv< A1 >::size( arg1 ) + Conv< A2 >::size( arg2 ) +
				Conv< A3 >::size( arg3 ) + Conv< A4 >::size( arg4 ) );
			Conv< A1 >::val2buf( arg1, &buf );
			Conv< A2 >::val2buf( arg2, &buf );
			Conv< A3 >::val2buf( arg3, &buf );
			Conv< A4 >::val2buf( arg4, &buf );
			dispatchBuffers( e, hopIndex_ );
		}
	private:
		HopIndex hopIndex_;
};

template< class A1, class A2, class A3, class A4 >
const OpFunc* OpFunc4Base< A1, A2, A3, A4 >::makeHopFunc(
				HopIndex hopIndex) const
{
	return new HopFunc4< A1, A2, A3, A4 >( hopIndex );
}

// Function to hop across nodes, with three arguments.
template < class A1, class A2, class A3, class A4, class A5 >
	class HopFunc5: public OpFunc5Base< A1, A2, A3, A4, A5 >
{
	public:
		HopFunc5( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}

		void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3,
						A4 arg4, A5 arg5 ) const
		{
			double* buf = addToBuf( e, hopIndex_,
				Conv< A1 >::size( arg1 ) + Conv< A2 >::size( arg2 ) +
				Conv< A3 >::size( arg3 ) + Conv< A4 >::size( arg4 ) +
				Conv< A5 >::size( arg5 ) );
			Conv< A1 >::val2buf( arg1, &buf );
			Conv< A2 >::val2buf( arg2, &buf );
			Conv< A3 >::val2buf( arg3, &buf );
			Conv< A4 >::val2buf( arg4, &buf );
			Conv< A5 >::val2buf( arg5, &buf );
			dispatchBuffers( e, hopIndex_ );
		}
	private:
		HopIndex hopIndex_;
};

template< class A1, class A2, class A3, class A4, class A5 >
const OpFunc* OpFunc5Base< A1, A2, A3, A4, A5 >::makeHopFunc(
				HopIndex hopIndex) const
{
	return new HopFunc5< A1, A2, A3, A4, A5 >( hopIndex );
}

// Function to hop across nodes, with three arguments.
template < class A1, class A2, class A3, class A4, class A5, class A6 >
	class HopFunc6: public OpFunc6Base< A1, A2, A3, A4, A5, A6 >
{
	public:
		HopFunc6( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}

		void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3,
						A4 arg4, A5 arg5, A6 arg6 ) const
		{
			double* buf = addToBuf( e, hopIndex_,
				Conv< A1 >::size( arg1 ) + Conv< A2 >::size( arg2 ) +
				Conv< A3 >::size( arg3 ) + Conv< A4 >::size( arg4 ) +
				Conv< A5 >::size( arg5 ) + Conv< A6 >::size( arg6 ) );
			Conv< A1 >::val2buf( arg1, &buf );
			Conv< A2 >::val2buf( arg2, &buf );
			Conv< A3 >::val2buf( arg3, &buf );
			Conv< A4 >::val2buf( arg4, &buf );
			Conv< A5 >::val2buf( arg5, &buf );
			Conv< A6 >::val2buf( arg6, &buf );
			dispatchBuffers( e, hopIndex_ );
		}
	private:
		HopIndex hopIndex_;
};

template< class A1, class A2, class A3, class A4, class A5, class A6 >
const OpFunc* OpFunc6Base< A1, A2, A3, A4, A5, A6 >::makeHopFunc(
				HopIndex hopIndex) const
{
	return new HopFunc6< A1, A2, A3, A4, A5, A6 >( hopIndex );
}


// Function to Get value after hop across nodes, with one argument.
template < class A > class GetHopFunc: public OpFunc1Base< A* >
{
	public:
		GetHopFunc( HopIndex hopIndex )
				: hopIndex_( hopIndex )
		{;}

		void op( const Eref& e, A* ret ) const
		{
			double* buf = remoteGet( e, hopIndex_.bindIndex() );
			*ret = Conv< A >::buf2val( &buf );
		}

		void getLocalFieldVec( const Eref& er, vector< A >& ret,
				 const GetOpFuncBase< A >* op ) const
		{
			unsigned int p = er.dataIndex();
			Element* elm = er.element();
			unsigned int numField = elm->numField(
							p - elm->localDataStart() );
			for ( unsigned int q = 0; q < numField; ++q ) {
				Eref temp( elm, p, q );
				ret.push_back( op->returnOp( temp ) );
			}
		}

		void getRemoteFieldVec( const Eref& e, vector< A >& ret,
				 const GetOpFuncBase< A >* op ) const
		{
			vector< double > buf;
			remoteFieldGetVec( e, hopIndex_.bindIndex(), buf );
			assert( buf.size() > 0 );
			unsigned int numField = buf[0];
			double* val = &buf[1]; // zeroth entry is numField.
			for ( unsigned int j = 0; j < numField; ++j ) {
				ret.push_back( Conv< A >::buf2val( &val ) );
			}
		}

		void getLocalVec( Element *elm, vector< A >& ret,
				 const GetOpFuncBase< A >* op ) const
		{
			unsigned int start = elm->localDataStart();
			unsigned int end = start + elm->numLocalData();
			for ( unsigned int p = start; p < end; ++p ) {
				Eref er( elm, p, 0 );
				ret.push_back( op->returnOp( er ) );
			}
		}

		void getMultiNodeVec( const Eref& e, vector< A >& ret,
				 const GetOpFuncBase< A >* op ) const
		{
			Element* elm = e.element();
			vector< vector< double > > buf;
			vector< unsigned int > numOnNode;
			remoteGetVec( e, hopIndex_.bindIndex(), buf, numOnNode );
			assert( numOnNode.size() == mooseNumNodes() );
			assert( buf.size() == mooseNumNodes() );
			assert( buf.size() == numOnNode.size() );
			for ( unsigned int i = 0; i < mooseNumNodes(); ++i ) {
				if ( i == mooseMyNode() ) {
					getLocalVec( elm, ret, op );
				} else {
					vector< double >& temp = buf[i];
					assert( static_cast< unsigned int >( temp[0] ) ==
											numOnNode[i] );
					double* val = &temp[1]; // zeroth entry is numOnNode.
					for ( unsigned int j = 0; j < numOnNode[i]; ++j ) {
					// val++; // Skip the index.
					// ret.push_back( Conv< A >::buf2val( &temp[k + 1] ) );
						ret.push_back( Conv< A >::buf2val( &val ) );
					}
				}
			}
		}

		void opGetVec( const Eref& e, vector< A >& ret,
				 const GetOpFuncBase< A >* op ) const
		{
			Element* elm = e.element();
			ret.clear();
			ret.reserve( elm->numData() );
			if ( elm->hasFields() ) {
				if ( e.getNode() == mooseMyNode() ) {
					getLocalFieldVec( e, ret, op );
				} else {
					getRemoteFieldVec( e, ret, op );
				}
			} else {
				if ( mooseNumNodes() == 1 || elm->isGlobal() ) {
					getLocalVec( elm, ret, op );
				} else {
					getMultiNodeVec( e, ret, op );
				}
			}
		}
	private:
		HopIndex hopIndex_;
};

/**
 * Deferred specification of function from OpFunc1Base, so it is after
 * the declaration of the HopFunc class to which it refers.
 */
template< class A >
const OpFunc* GetOpFuncBase< A >::makeHopFunc( HopIndex hopIndex ) const
{
	return new GetHopFunc< A >( hopIndex );
}

#endif // _HOP_FUNC_H
