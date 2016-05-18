/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <typeinfo>
#include "header.h"

Finfo::Finfo( const string& name, const string& doc )
    : name_( name ), doc_( doc )
{
    ;
}

////////////////////////////////////////////////////////////////
// MOOSE class functions
////////////////////////////////////////////////////////////////

const Cinfo* Finfo::initCinfo()
{
    static ReadOnlyValueFinfo< FinfoWrapper, string > fieldName(
        "fieldName",
        "Name of field handled by Finfo",
        &FinfoWrapper::getName
    );

    static ReadOnlyValueFinfo< FinfoWrapper, string > docs(
        "docs",
        "Documentation for Finfo",
        &FinfoWrapper::docs
    );

    static ReadOnlyValueFinfo< FinfoWrapper, string > type(
        "type",
        "RTTI type info for this Finfo",
        &FinfoWrapper::type
    );

    static ReadOnlyValueFinfo< FinfoWrapper, vector< string > > src(
        "src",
        "Subsidiary SrcFinfos. Useful for SharedFinfos",
        &FinfoWrapper::src
    );
    static ReadOnlyValueFinfo< FinfoWrapper, vector< string > > dest(
        "dest",
        "Subsidiary DestFinfos. Useful for SharedFinfos",
        &FinfoWrapper::dest
    );


    static Finfo* finfoFinfos[] =
    {
        &fieldName,	// ReadOnlyValue
        &docs,		// ReadOnlyValue
        &type,		// ReadOnlyValue
        &src,		// ReadOnlyValue
        &dest,		// ReadOnlyValue
    };

    static Dinfo< Finfo* > dinfo;
    static Cinfo finfoCinfo (
        "Finfo",
        Neutral::initCinfo(),
        finfoFinfos,
        sizeof( finfoFinfos ) / sizeof( Finfo* ),
        &dinfo
    );

    return &finfoCinfo;
}

static const Cinfo* finfoCinfo = Finfo::initCinfo();

////////////////////////////////////////////////////////////////

const string& Finfo::name( ) const
{
    return name_;
}

const string& Finfo::docs( ) const
{
    return doc_;
}

////////////////////////////////////////////////////////////////
// Needed to access as a MOOSE field. Note that the Finfo is stored as a ptr
string FinfoWrapper::getName( ) const
{
    return f_->name();
}

string FinfoWrapper::docs( ) const
{
    return f_->docs();
}


string FinfoWrapper::type( ) const
{
    return f_->rttiType();
}

vector< string > FinfoWrapper::src( ) const
{
    return f_->innerSrc();
}

vector< string > FinfoWrapper::dest( ) const
{
    return f_->innerDest();
}


// Default virtual functions.
string Finfo::rttiType() const
{
    return typeid( *this ).name();
}

vector< string > Finfo::innerSrc() const
{
    static vector< string > ret;
    return ret;
}

vector< string > Finfo::innerDest() const
{
    static vector< string > ret;
    return ret;
}
