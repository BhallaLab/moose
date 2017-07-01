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

const unsigned char MooseSendHop = 0;
const unsigned char MooseSetHop = 1;
const unsigned char MooseSetVecHop = 2;
const unsigned char MooseGetHop = 4;
const unsigned char MooseGetVecHop = 5;
const unsigned char MooseReturnHop = 8;
const unsigned char MooseTestHop = 255;

vector< OpFunc* >& OpFunc::ops()
{
	static vector< OpFunc* > op;
	return op;
}

OpFunc::OpFunc()
{
	opIndex_ = ops().size();
	ops().push_back( this );
}

const OpFunc* OpFunc0Base::makeHopFunc( HopIndex hopIndex ) const
{
	return new HopFunc0( hopIndex );
}

void OpFunc0Base::opBuffer( const Eref& e, double* buf ) const
{
	op( e );
}

const OpFunc* OpFunc::lookop( unsigned int opIndex )
{
	assert ( opIndex < ops().size() );
	return ops()[ opIndex ];
}

// Static function
unsigned int OpFunc::rebuildOpIndex()
{
	for( vector< OpFunc* >::iterator
		i = ops().begin(); i != ops().end(); ++i ) {
		(*i)->opIndex_ = ~0U;
	}
	return ops().size();
}

bool OpFunc::setIndex( unsigned int i ) // Should only be called by Cinfo.
{
	if( opIndex_ == ~0U ) {
		opIndex_ = i;
		ops()[i] = this;
		return true;
	}
	/*
	cout << " OpFunc " << rttiType() <<
			" already setup. (old,new) index = (" <<
			opIndex_ << ", " << i << " )\n";
			*/
	return false;
}
