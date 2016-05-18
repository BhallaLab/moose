/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../shell/Shell.h"

Eref::Eref()
			: e_( 0 ), i_( 0 ), f_( 0 )
{
	;
}

Eref::Eref( Element* e, unsigned int index, unsigned int field )
			: e_( e ), i_( index ), f_( field )
{
	;
}

ostream& operator <<( ostream& s, const Eref& e )
{
	if ( e.i_ == 0 ) {
		if ( e.f_ == 0 )
			s << e.e_->getName();
		else
			s << e.e_->getName() << "[0][" << e.f_ << "]";
	} else {
		if ( e.f_ == 0 )
			s << e.e_->getName() << "[" << e.i_ << "]";
		else
			s << e.e_->getName() << "[" << e.i_ << "][" << e.f_ << "]";
	}
	return s;
}

char* Eref::data() const
{
	assert( isDataHere() );
	return e_->data( e_->rawIndex( i_ ), f_ );
}

bool Eref::isDataHere() const
{
	return ( e_->getNode( i_ ) == Shell::myNode() );
}

unsigned int Eref::getNode() const
{
	return e_->getNode( i_ );
}

ObjId Eref::objId() const
{
	return ObjId( e_->id(), i_, f_ );
}

Id Eref::id() const
{
	return e_->id();
}

const vector< MsgDigest >& Eref::msgDigest( unsigned int bindIndex ) const
{
	return e_->msgDigest( i_ * e_->cinfo()->numBindIndex() + bindIndex );
}
