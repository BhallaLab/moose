/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _DATA_ELEMENT_H
#define _DATA_ELEMENT_H

class SrcFinfo;
class FuncOrder;

/**
 * This is the base class for managing the data in the MOOSE interface.
 * Elaborates on base class for how it handles destruction.
 * Subclasses of this handle special node decomposition.
 * This does basic data management with raw indices.
 */
class DataElement: public Element
{
	public:
		/**
		 * This is the main constructor, used by Shell::innerCreate
		 * which makes most Elements. Also used to create base
		 * Elements to init the simulator in main.cpp.
		 * Id is the Id of the new Element
		 * Cinfo is the class
		 * name is its name
		 * numData is the number of data entries, defaults to a singleton.
		 * The isGlobal flag specifies whether the created objects should
		 * be replicated on all nodes, or partitioned without replication.
		 */
		DataElement( Id id, const Cinfo* c, const string& name,
			unsigned int numData = 1 );

		/**
		 * This constructor copies over the original n times. It is
		 * used for doing all copies, in Shell::innerCopyElements.
		 */
		DataElement( Id id, const Element* orig, unsigned int n,
						unsigned int startEntry );

		/**
		 * Virtual Destructor
		 */
		~DataElement();

		/**
		 * copyElement function is defined only in derived classes.
		 */

		/////////////////////////////////////////////////////////////////
		// Information access fields
		/////////////////////////////////////////////////////////////////

		/// Defined only in derived classes: unsigned int numData() const;

		/// Inherited virtual. Returns number of data entries on this node
		unsigned int numLocalData() const;

		/// Define only in derived classes: getNode( unsigned int dataIndex ) const;

		/// Inherited virtual. Returns number of field entries for specified data
		unsigned int numField( unsigned int rawIndex ) const;

		/**
		 * Inherited virtual: Returns number of field entries on
		 * current node, same as numLocalData().
		 */
		unsigned int totNumLocalField() const;

		/// Do not define getNode() or rawIndex() funcs, those are derived

		/**
		 * Inherited virtual
		 * True if this is a FieldElement having an array of fields
		 * on each data entry. Clearly not true for the base Element.
		 */
		bool hasFields() const {
			return false;
		}

		/////////////////////////////////////////////////////////////////
		// data access stuff
		/////////////////////////////////////////////////////////////////

		/**
		 * Inherited virtual.
		 * Looks up specified field data entry. On regular objects just
		 * returns the data entry specified by the rawIndex.
		 * On FieldElements like synapses, does a second lookup on the
		 * field index.
		 * Note that the index is NOT a
		 * DataIndex: it is instead the raw index of the data on the current
		 * node. Index is also NOT the character offset, but the index
		 * to the data array in whatever type the data may be.
		 *
		 * The DataIndex has to be filtered through the nodeMap to
		 * find a) if the entry is here, and b) what its raw index is.
		 *
		 * Returns 0 if either index is out of range.
		 */
		char* data( unsigned int rawIndex,
						unsigned int fieldIndex = 0 ) const;

		/**
		 * Inherited virtual.
		 * Changes the total number of data entries on Element in entire
		 * simulation. Not permitted for
		 * FieldElements since they are just fields on the data.
		 */
		void resize( unsigned int newNumData );

		/**
		 * Inherited virtual.
		 * Changes the number of fields on the specified data entry.
		 * Doesn't do anything for the regular Element.
		 */
		void resizeField(
				unsigned int rawIndex, unsigned int newNumField )
			{;}

		/////////////////////////////////////////////////////////////////
		// Zombie stuff.

		/// Virtual func.
		void zombieSwap( const Cinfo* newCinfo );

	private:

		/**
		 * This points to an array holding the data for the Element.
		 */
		char* data_;

		/**
		 * This is the number of data entries on the current node.
		 */
		unsigned int numLocalData_;

		/**
		 * This is the size of the data. Can get from cinfo()->dinfo(),
		 * but this is called so often than it makes a measurable
		 * difference.
		 */
		unsigned int size_;
};

#endif // _DATA_ELEMENT_H
