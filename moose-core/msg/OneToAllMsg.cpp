/**********************************************************************
 ** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "OneToAllMsg.h"

// Initializing static variables
Id OneToAllMsg::managerId_;
vector< OneToAllMsg* > OneToAllMsg::msg_;

OneToAllMsg::OneToAllMsg( Eref e1, Element* e2, unsigned int msgIndex )
	:
		Msg(
			ObjId( managerId_, (msgIndex != 0) ? msgIndex: msg_.size() ),
			e1.element(), e2
		   ),
		i1_( e1.dataIndex() )
{
	if ( msgIndex == 0 ) {
		msg_.push_back( this );
	} else {
		if ( msg_.size() <= msgIndex )
			msg_.resize( msgIndex + 1 );
		msg_[ msgIndex ] = this;
	}
}

OneToAllMsg::~OneToAllMsg()
{
	assert( mid_.dataIndex < msg_.size() );
	msg_[ mid_.dataIndex ] = 0; // ensure deleted ptr isn't reused.
}

Eref OneToAllMsg::firstTgt( const Eref& src ) const
{
	if ( src.element() == e1_ )
		return Eref( e2_, 0 );
	else if ( src.element() == e2_ )
		return Eref( e1_, i1_ );
	return Eref( 0, 0 );
}

void OneToAllMsg::sources( vector< vector < Eref > >& v ) const
{
	// Same single source for all targets.
	v.clear();
	vector< Eref > temp( 1, Eref( e1_, i1_ ) );
	v.assign( e2_->numData(), temp  );
}

void OneToAllMsg::targets( vector< vector< Eref > >& v ) const
{
	v.clear();
	v.resize( e1_->numData() );
	v[i1_].resize( 1, Eref( e2_, ALLDATA ) );
}

Id OneToAllMsg::managerId() const
{
	return OneToAllMsg::managerId_;
}

ObjId OneToAllMsg::findOtherEnd( ObjId f ) const
{
	if ( f.element() == e1() ) {
		if ( f.dataIndex == i1_ )
			return ObjId( e2()->id(), 0 );
	} else if ( f.element() == e2() ) {
		return ObjId( e1()->id(), i1_ );
	}

	return ObjId( 0, BADINDEX );
}

Msg* OneToAllMsg::copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const
{
	const Element* orig = origSrc.element();
	if ( n <= 1 ) {
		OneToAllMsg* ret = 0;
		if ( orig == e1() ) {
			ret = new OneToAllMsg(
					Eref( newSrc.element(), i1_ ), newTgt.element(), 0 );
			ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
		} else if ( orig == e2() ) {
			ret = new OneToAllMsg(
					Eref( newTgt.element(), i1_ ), newSrc.element(), 0 );
			ret->e2()->addMsgAndFunc( ret->mid(), fid, b );
		} else {
			assert( 0 );
		}
		return ret;
	} else {
		// Here we need a SliceMsg which goes from one 2-d array to another.
		cout << "Error: OneToAllMsg::copy: SliceToSliceMsg not yet implemented\n";
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////
// Here we set up the MsgManager portion of the class.
///////////////////////////////////////////////////////////////////////

const Cinfo* OneToAllMsg::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Field definitions.
	///////////////////////////////////////////////////////////////////
	static ValueFinfo< OneToAllMsg, DataId > i1(
		"i1",
		"DataId of source Element.",
		&OneToAllMsg::setI1,
		&OneToAllMsg::getI1
	);

	static Finfo* msgFinfos[] = {
		&i1,		// value
	};

	static Dinfo< short > dinfo;
	static Cinfo msgCinfo (
		"OneToAllMsg",	// name
		Msg::initCinfo(),				// base class
		msgFinfos,
		sizeof( msgFinfos ) / sizeof( Finfo* ),	// num Fields
		&dinfo
	);

	return &msgCinfo;
}

static const Cinfo* assignmentMsgCinfo = OneToAllMsg::initCinfo();

/**
 * Return the first DataId
 */
DataId OneToAllMsg::getI1() const
{
	return i1_;
}

void OneToAllMsg::setI1( DataId i1 )
{
	i1_ = i1;
	e1()->markRewired();
	e2()->markRewired();
}

/// Static function for Msg access
unsigned int OneToAllMsg::numMsg()
{
	return msg_.size();
}

/// Static function for Msg access
char* OneToAllMsg::lookupMsg( unsigned int index )
{
	assert( index < msg_.size() );
	return reinterpret_cast< char* >( msg_[index] );
}
