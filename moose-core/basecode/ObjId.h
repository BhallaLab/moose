/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _OBJ_ID_H
#define _OBJ_ID_H

#include "Id.h"

/**
 * This class manages lookups for specific data entries in elements,
 * in a node-independent manner.
 * It is basically a composite of Id and DataIndex.
 */
class ObjId
{
	friend ostream& operator <<( ostream& s, const ObjId& i );
	friend istream& operator >>( istream& s, ObjId& i );
	public:
		//////////////////////////////////////////////////////////////
		//	ObjId creation
		//////////////////////////////////////////////////////////////
		/**
		 * Returns the root Id
		 */
		ObjId()
			: id(), dataIndex( 0 ), fieldIndex( 0 )
		{;}

		/**
		 * Creates a ObjId using specified Id and DataIndex
		 */
		ObjId( Id i, unsigned int d, unsigned int f = 0 )
			: id( i ), dataIndex( d ), fieldIndex( f )
		{;}

		ObjId( Id i )
			: id( i ), dataIndex( 0 ), fieldIndex( 0 )
		{;}

		/**
		 * Creates an ObjId by reading the path string
		 * Returns bad on failure.
		 */
		ObjId( const string& path );

		/**
		 * Returns the absolute path including all array indices.
		 */
		string path() const;

		/**
		 * Returns the Eref matching itself.
		 */
		Eref eref() const;

		/**
		 * For equality check
		 */
		bool operator==( const ObjId& other ) const;
		bool operator!=( const ObjId& other ) const;

		/**
		 * For sorting
		 */
		bool operator<( const ObjId& other ) const;

		/**
		 * True if the data is present on the current node. Always true for
		 * globals, which confuses the matter somewhat.
		 */
		bool isDataHere() const;

		/// Returns true if the Element is global.
		bool isGlobal() const;

		/// Returns true if we need to go off-node for calling operations
		bool isOffNode() const;

		/**
		 * Returns data entry for this object
		 */
		char* data() const;

		/**
		 * Returns Element part
		 */
		Element* element() const;

		/**
		 * Here are the data values.
		 */
		Id id;
		unsigned int dataIndex;
		unsigned int fieldIndex;

		/**
		 * True if the return value is bad: either returning a failure,
		 * or the DataIndex or FieldIndex is out of range. However, this
		 * is a node-local funtion so it can't report the FieldIndex status
		 * in all cases.
		 */
		bool bad() const;

	private:
};

#endif // _OBJ_ID_H
