/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"
#include "Mstring.h"

const Cinfo* Mstring::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< Mstring, string > thisFinfo(
			"this",
			"Access function for entire Mstring object.",
			&Mstring::setThis,
			&Mstring::getThis
		);
		static ValueFinfo< Mstring, string > valueFinfo(
			"value",
			"Access function for value field of Mstring object,"
			"which happens also to be the entire contents of the object.",
			&Mstring::setThis,
			&Mstring::getThis
		);

	static Finfo* mStringFinfos[] = {
		&thisFinfo,	// Value
		&valueFinfo,	// Value
	};

	static Dinfo< Mstring > dinfo;
	static Cinfo mStringCinfo (
		"Mstring",
		Neutral::initCinfo(),
		mStringFinfos,
		sizeof( mStringFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &mStringCinfo;
}

static const Cinfo* mStringCinfo = Mstring::initCinfo();

Mstring::Mstring()
	: value_( "" )
{
	;
}

Mstring::Mstring( string val )
	: value_( val )
{
	;
}

void Mstring::setThis( string v )
{
	value_ = v;
}

string Mstring::getThis() const
{
	return value_;
}

