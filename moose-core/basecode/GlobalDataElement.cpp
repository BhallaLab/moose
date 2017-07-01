/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "FuncOrder.h"
#include "../shell/Shell.h"

GlobalDataElement::GlobalDataElement( Id id, const Cinfo* c,
	const string& name, unsigned int numData )
	:
		DataElement( id, c, name, numData )
{;}


/*
 * Used for copies. Note that it does NOT call the postCreation Func,
 * so FieldElements are copied rather than created by the Cinfo when
 * the parent element is created. This allows the copied FieldElements to
 * retain info from the originals.
 * Note that n is the number of individual  dataEntries that are made.
 */
GlobalDataElement::GlobalDataElement( Id id, const Element* orig,
				unsigned int n )
	:
		DataElement( id, orig, n, 0 )
{;}

// Virtual destructor.
GlobalDataElement::~GlobalDataElement()
{;}

Element* GlobalDataElement::copyElement(
		Id newParent, Id newId, unsigned int n, bool toGlobal ) const
{
	if ( toGlobal )
		return new GlobalDataElement( newId, this, n );
	else
		return new LocalDataElement( newId, this, n );
}


/////////////////////////////////////////////////////////////////////////
// DataElement info functions
/////////////////////////////////////////////////////////////////////////

// virtual func.
unsigned int GlobalDataElement::numData() const
{
	return numLocalData();
}

unsigned int GlobalDataElement::getNode( unsigned int dataId ) const {
	// Assume numData = 95. DataId = 0-9: 0, DataId=80-89:8, DataId >= 90:9
	return Shell::myNode();
}

unsigned int GlobalDataElement::rawIndex( unsigned int dataId ) const {
	return dataId;
}

unsigned int GlobalDataElement::getNumOnNode( unsigned int node ) const
{
	return numLocalData();
}
