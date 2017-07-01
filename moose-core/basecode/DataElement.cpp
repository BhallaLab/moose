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

DataElement::DataElement( Id id, const Cinfo* c, const string& name,
	unsigned int numData )
	:
		Element( id, c, name )
{
	data_ = c->dinfo()->allocData( numData );
	numLocalData_ = numData;
	size_ = cinfo()->dinfo()->sizeIncrement();
	c->postCreationFunc( id, this );
}

/*
 * Used for copies. Note that it does NOT call the postCreation Func,
 * so FieldElements are copied rather than created by the Cinfo when
 * the parent element is created. This allows the copied FieldElements to
 * retain info from the originals.
 * Note that n is the number of DataEntries made. This may be _less_ than
 * the starting number of entries. If you want to copy an element tree
 * then you have to pre-multiply n by the original size.
 * startEntry is the starting index. It is expected that the subset used
 * will be contiguous from this index.
 */
DataElement::DataElement( Id id, const Element* orig,
				unsigned int n, unsigned int startEntry )
	:
		Element( id, orig->cinfo(), orig->getName() )
{
	numLocalData_ = n;
	size_ = cinfo()->dinfo()->sizeIncrement();
	data_ = cinfo()->dinfo()->copyData( orig->data( 0 ), orig->numData(),
					numLocalData_, startEntry );
	// cinfo_->postCreationFunc( id, this );
}

// Virtual destructor.
DataElement::~DataElement()
{
	// cout << "deleting element " << getName() << endl;
	cinfo()->dinfo()->destroyData( data_ );
	data_ = 0;
	// The base class destroys the messages.
}

/////////////////////////////////////////////////////////////////////////
// DataElement info functions
/////////////////////////////////////////////////////////////////////////

// virtual func.
unsigned int DataElement::numLocalData() const
{
	return numLocalData_;
}

// virtual func.
unsigned int DataElement::numField( unsigned int entry ) const
{
	return 1;
}

// virtual func.
unsigned int DataElement::totNumLocalField() const
{
	return numLocalData_;
}

/////////////////////////////////////////////////////////////////////////
// Data access functions.
/////////////////////////////////////////////////////////////////////////
// virtual func, overridden.
char* DataElement::data( unsigned int rawIndex, unsigned int fieldIndex ) const
{
	assert( rawIndex < numLocalData_ );
	// return data_ + ( rawIndex * cinfo()->dinfo()->size() );
	return data_ + rawIndex * size_;
}

/**
 * virtual func, overridden.
 * Here we resize the local data. This function would be called by
 * derived classes to do their own data management as per node
 * decomposition.
 */
void DataElement::resize( unsigned int newNumLocalData )
{
	numLocalData_ = newNumLocalData;
	char* temp = data_;
	data_ = cinfo()->dinfo()->copyData(
					temp, numLocalData_, newNumLocalData, 0 );
	cinfo()->dinfo()->destroyData( temp );
	numLocalData_ = newNumLocalData;
}

/////////////////////////////////////////////////////////////////////////
// Zombie stuff
/////////////////////////////////////////////////////////////////////////

void DataElement::zombieSwap( const Cinfo* zCinfo )
{
	cinfo()->dinfo()->destroyData( data_ );
	data_ = zCinfo->dinfo()->allocData( numLocalData_ );
	replaceCinfo( zCinfo );
	size_ = zCinfo->dinfo()->sizeIncrement();
	Element::zombieSwap( zCinfo ); // Handles clock tick reassignment.
}
