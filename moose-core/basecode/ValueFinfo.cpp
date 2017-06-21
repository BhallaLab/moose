/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

ValueFinfoBase::ValueFinfoBase( const string& name, const string& doc )
	: Finfo( name, doc ),
		set_( 0 ),
		get_( 0 )
{;}

DestFinfo* ValueFinfoBase::getFinfo() const {
	return get_;
}

vector< string > ValueFinfoBase::innerDest() const
{
	vector< string > ret;
	if ( set_ )
		ret.push_back( set_->name() );

	assert( get_ );
	ret.push_back( get_->name() );

	return ret;
}
