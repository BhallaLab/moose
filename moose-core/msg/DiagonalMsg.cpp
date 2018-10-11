/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "DiagonalMsg.h"

// Static field declaration
Id DiagonalMsg::managerId_;
vector< DiagonalMsg* > DiagonalMsg::msg_;

DiagonalMsg::DiagonalMsg( Element* e1, Element* e2, unsigned int msgIndex )
	: Msg( ObjId( managerId_, (msgIndex != 0) ? msgIndex: msg_.size() ),
					e1, e2 ),
	stride_( 1 )
{
	if ( msgIndex == 0 ) {
		msg_.push_back( this );
	} else {
		if ( msg_.size() <= msgIndex )
			msg_.resize( msgIndex + 1 );
		msg_[ msgIndex ] = this;
	}
}

DiagonalMsg::~DiagonalMsg()
{
	assert( mid_.dataIndex < msg_.size() );
	msg_[ mid_.dataIndex ] = 0; // ensure deleted ptr isn't reused.
}

Eref DiagonalMsg::firstTgt( const Eref& src ) const
{
	if ( src.element() == e1_ ) {
		unsigned int nextData = src.dataIndex() + stride_;
		return Eref( e2_, nextData );
	}
	else if ( src.element() == e2_ ) {
		unsigned int nextData = src.dataIndex() - stride_;
		return Eref( e1_, nextData );
	}
	return Eref( 0, 0 );
}

void DiagonalMsg::sources( vector< vector < Eref > >& v ) const
{
	v.clear();
	v.resize( e2_->numData() );
	int j = -stride_;
	int numData1 = e1_->numData();
	for ( unsigned int i = 0; i < e2_->numData(); ++i ) {
		if ( j >= 0 && j < numData1 )
			v[i].resize( 1, Eref( e1_, j ) );
		j++;
	}
}

void DiagonalMsg::targets( vector< vector< Eref > >& v ) const
{
	v.clear();
	v.resize( e1_->numData() );
	int j = stride_;
	int numData2 = e2_->numData();
	for ( unsigned int i = 0; i < e1_->numData(); ++i ) {
		if ( j >= 0 && j < numData2 )
			v[i].resize( 1, Eref( e2_, j ) );
		j++;
	}
}

Id DiagonalMsg::managerId() const
{
	return DiagonalMsg::managerId_;
}

void DiagonalMsg::setStride( int stride )
{
	stride_ = stride;
	e1()->markRewired();
	e2()->markRewired();
}

int DiagonalMsg::getStride() const
{
	return stride_;
}

ObjId DiagonalMsg::findOtherEnd( ObjId f ) const
{
	if ( f.id.element() == e1() ) {
		int i2 = f.dataIndex + stride_;
		if ( i2 >= 0 ) {
			unsigned int ui2 = i2;
			if ( ui2 < e2()->numData() )
				return ObjId( e2()->id(), DataId( ui2 ) );
		}
	} else if ( f.id.element() == e2() ) {
		int i1 = f.dataIndex - stride_;
		if ( i1 >= 0 ) {
			unsigned int ui1 = i1;
			if ( ui1 < e1()->numData() )
				return ObjId( e1()->id(), DataId( ui1 ));
		}
	}
	return ObjId( 0, BADINDEX );
}

Msg* DiagonalMsg::copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const
{
	const Element* orig = origSrc.element();
	if ( n <= 1 ) {
		DiagonalMsg* ret = 0;
		if ( orig == e1() ) {
			ret = new DiagonalMsg( newSrc.element(), newTgt.element(), 0 );
			ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
		} else if ( orig == e2() ) {
			ret = new DiagonalMsg( newTgt.element(), newSrc.element(), 0 );
			ret->e2()->addMsgAndFunc( ret->mid(), fid, b );
		} else {
			assert( 0 );
		}
		ret->setStride( stride_ );
		return ret;
	} else {
		// Here we need a SliceMsg which goes from one 2-d array to another.
		cout << "Error: DiagonalMsg::copy: DiagonalSliceMsg not yet implemented\n";
		return 0;
	}
}

/// Static function for Msg access
unsigned int DiagonalMsg::numMsg()
{
	return msg_.size();
}

/// Static function for Msg access
char* DiagonalMsg::lookupMsg( unsigned int index )
{
	assert( index < msg_.size() );
	return reinterpret_cast< char* >( msg_[index] );
}

///////////////////////////////////////////////////////////////////////
// Here we set up the MsgManager portion of the class.
///////////////////////////////////////////////////////////////////////

const Cinfo* DiagonalMsg::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Field definitions.
	///////////////////////////////////////////////////////////////////
	static ValueFinfo< DiagonalMsg, int > stride(
		"stride",
		"The stride is the increment to the src DataId that gives the"
		"dest DataId. "
		"It can be positive or negative, but bounds checking"
		"takes place and it does not wrap around.",
		&DiagonalMsg::setStride,
		&DiagonalMsg::getStride
	);

	static Finfo* msgFinfos[] = {
		&stride,		// value
	};

	static Dinfo< short > dinfo;
	static Cinfo msgCinfo (
		"DiagonalMsg",	// name
		Msg::initCinfo(),				// base class
		msgFinfos,
		sizeof( msgFinfos ) / sizeof( Finfo* ),	// num Fields
		&dinfo
	);

	return &msgCinfo;
}

static const Cinfo* assignmentMsgCinfo = DiagonalMsg::initCinfo();
