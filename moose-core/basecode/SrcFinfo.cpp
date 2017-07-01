/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"

/**
 * This set of classes define Message Sources. Their main job is to supply
 * a type-safe send operation, and to provide typechecking for it.
 */

const BindIndex SrcFinfo::BadBindIndex = 65535;

SrcFinfo::SrcFinfo( const string& name, const string& doc )
	: Finfo( name, doc ), bindIndex_( BadBindIndex )
{ ; }

void SrcFinfo::registerFinfo( Cinfo* c )
{
	bindIndex_ = c->registerBindIndex();
}


BindIndex SrcFinfo::getBindIndex() const
{
	// Treat this assertion as a warning that the SrcFinfo is being used
	// without initialization.
	assert( bindIndex_ != BadBindIndex );
	return bindIndex_;
}

void SrcFinfo::setBindIndex( BindIndex b )
{
	bindIndex_ = b;
}

bool SrcFinfo::checkTarget( const Finfo* target ) const
{
	const DestFinfo* d = dynamic_cast< const DestFinfo* >( target );
	if ( d ) {
		return d->getOpFunc()->checkFinfo( this );
	}
	return 0;
}

bool SrcFinfo::addMsg( const Finfo* target, ObjId mid, Element* src ) const
{
	const DestFinfo* d = dynamic_cast< const DestFinfo* >( target );
	if ( d ) {
		if ( d->getOpFunc()->checkFinfo( this ) ) {
			src->addMsgAndFunc( mid, d->getFid(), bindIndex_ );
			return 1;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////
/**
 * SrcFinfo0 sets up calls without any arguments.
 */
SrcFinfo0::SrcFinfo0( const string& name, const string& doc )
	: SrcFinfo( name, doc )
{ ; }

class OpFunc0Base;
void SrcFinfo0::send( const Eref& e ) const {
	const vector< MsgDigest >& md = e.msgDigest( getBindIndex() );
	for ( vector< MsgDigest >::const_iterator
		i = md.begin(); i != md.end(); ++i ) {
		const OpFunc0Base* f =
			dynamic_cast< const OpFunc0Base* >( i->func );
		assert( f );
		for ( vector< Eref >::const_iterator
			j = i->targets.begin(); j != i->targets.end(); ++j ) {
			if ( j->dataIndex() == ALLDATA ) {
				Element* e = j->element();
				unsigned int start = e->localDataStart();
				unsigned int end = start + e->numData();
				for ( unsigned int k = start; k < end; ++k )
					f->op( Eref( e, k ) );
			} else  {
				f->op( *j );
			}
		}
	}
}

void SrcFinfo0::sendBuffer( const Eref& e, double* buf ) const
{
	send( e );
}

