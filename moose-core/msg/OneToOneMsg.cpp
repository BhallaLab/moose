/**********************************************************************
 ** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "OneToOneMsg.h"

// Initializing static variables
Id OneToOneMsg::managerId_;
vector< OneToOneMsg* > OneToOneMsg::msg_;

OneToOneMsg::OneToOneMsg( const Eref& e1, const Eref& e2,
				unsigned int msgIndex )
	: Msg( ObjId( managerId_, (msgIndex != 0) ? msgIndex: msg_.size() ),
					e1.element(), e2.element() ),
	i1_( e1.dataIndex() ),
	i2_( e2.dataIndex() )
{
	if ( msgIndex == 0 ) {
		msg_.push_back( this );
	} else {
		if ( msg_.size() <= msgIndex )
			msg_.resize( msgIndex + 1 );
		msg_[ msgIndex ] = this;
	}
}

OneToOneMsg::~OneToOneMsg()
{
	assert( mid_.dataIndex < msg_.size() );
	msg_[ mid_.dataIndex ] = 0; // ensure deleted ptr isn't reused.
}

/**
 * This is a little tricky because we might be mapping between
 * data entries and field entries here.
 * May wish also to apply to exec operations.
 * At this point, the effect of trying to go between regular
 * data entries and field entries is undefined.
 */
Eref OneToOneMsg::firstTgt( const Eref& src ) const
{
	if ( src.element() == e1_ ) {
		if ( e2_->hasFields() )
			return Eref( e2_, i2_, src.dataIndex() );
		else
			return Eref( e2_, src.dataIndex(), 0 );
	} else if ( src.element() == e2_ ) {
		return Eref( e1_, src.dataIndex() );
	}
	return Eref( 0, 0 );
}

// We don't permit e1 to have fields at this point.
void OneToOneMsg::sources( vector< vector< Eref > > & v) const
{
	v.resize( 0 );
	unsigned int n = e1_->numData();
	if ( e2_->hasFields() ) {
		if ( Eref( e2_, i2_ ).isDataHere() ) {
			assert( i2_ > e2_->localDataStart() );
			unsigned int nf = e2_->numField( i2_ - e2_->localDataStart() );
			if ( n > nf )
				n = nf;
			v.resize( n );
			for ( unsigned int i = 0; i < n; ++i ) {
				v[i].resize( 1, Eref( e1_, i ) );
			}
		}
	} else {
		if ( n > e2_->numData() )
			n = e2_->numData();
		v.resize( e2_->numData() );
		for ( unsigned int i = 0; i < n; ++i ) {
			v[i].resize( 1, Eref( e1_, i ) );
		}
	}
}

void OneToOneMsg::targets( vector< vector< Eref > > & v) const
{
	unsigned int n = e1_->numData();
	v.resize( e1_->numData() );
	if ( e2_->hasFields() ) {
		if ( Eref( e2_, i2_ ).isDataHere() ) {
			assert( i2_ > e2_->localDataStart() );
			unsigned int nf = e2_->numField( i2_ - e2_->localDataStart() );
			if ( n > nf )
				n = nf;
			for ( unsigned int i = 0; i < n; ++i )
				v[i].resize( 1, Eref( e2_, i2_, i ) );
		} else { // Don't know target node # of entries, so send all.
			unsigned int start = e1_->localDataStart();
			unsigned int end = start + e1_->numLocalData();
			for ( unsigned int i = start; i < end; ++i ) {
				v[i].resize( 1, Eref( e2_, i2_, i ) );
			}
		}
	} else {
		if ( n > e2_->numData() )
			n = e2_->numData();
		for ( unsigned int i = 0; i < n; ++i ) {
			v[i].resize( 1, Eref( e2_, i ) );
		}
	}
}

Id OneToOneMsg::managerId() const
{
	return OneToOneMsg::managerId_;
}

ObjId OneToOneMsg::findOtherEnd( ObjId f ) const
{
	if ( f.element() == e1() )
		return ObjId( e2()->id(), f.dataIndex );
	else if ( f.element() == e2() )
		return ObjId( e1()->id(), f.dataIndex );

	return ObjId( 0, BADINDEX );
}

Msg* OneToOneMsg::copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const
{
	const Element* orig = origSrc.element();
	// This works both for 1-copy and for n-copies
	OneToOneMsg* ret = 0;
	if ( orig == e1() ) {
		ret = new OneToOneMsg( newSrc.eref(), newTgt.eref(), 0 );
		ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
	} else if ( orig == e2() ) {
		ret = new OneToOneMsg( newTgt.eref(), newSrc.eref(), 0 );
		ret->e2()->addMsgAndFunc( ret->mid(), fid, b );
	} else
		assert( 0 );
	// ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
	return ret;
}

/// Static function for Msg access
unsigned int OneToOneMsg::numMsg()
{
	return msg_.size();
}

/// Static function for Msg access
char* OneToOneMsg::lookupMsg( unsigned int index )
{
	assert( index < msg_.size() );
	return reinterpret_cast< char* >( msg_[index] );
}

///////////////////////////////////////////////////////////////////////
// Here we set up the MsgManager portion of the class.
///////////////////////////////////////////////////////////////////////

const Cinfo* OneToOneMsg::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Field definitions. Nothing here.
	///////////////////////////////////////////////////////////////////

	static Dinfo< short > dinfo;
	static Cinfo msgCinfo (
		"OneToOneMsg",	// name
		Msg::initCinfo(),				// base class
		0,								// Finfo array
		0,								// Num Fields
		&dinfo
	);

	return &msgCinfo;
}

static const Cinfo* oneToOneMsgCinfo = OneToOneMsg::initCinfo();

