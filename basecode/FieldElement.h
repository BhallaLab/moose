/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _FIELD_ELEMENT_H
#define _FIELD_ELEMENT_H

/**
 * Specialization of Element class, used to look up array fields within
 * objects when those fields each need to have independent Element
 * capabilies such as messaging and subfield lookup.
 * Made automatically by Elements which have such fields.
 */
class FieldElement: public Element
{
	public:
		FieldElement( Id parent, Id kid, const Cinfo* c, const string& name,
			const FieldElementFinfoBase* fef
		);

		~FieldElement();

		/**
		 * Virtual copier. Makes a copy of self.
		 */
		Element* copyElement( Id newParent, Id newId, unsigned int n,
			bool toGlobal ) const;

		/////////////////////////////////////////////////////////////////
		/// Virtual: Returns number of data entries
		unsigned int numData() const;

		/// Virtual: Returns number of data entries on current node.
		unsigned int numLocalData() const;

		/// Virtual: Returns index of first data entry on this node.
		unsigned int localDataStart() const;

		/// Virtual: Returns number of field entries for specified data
		unsigned int numField( unsigned int rawIndex ) const;

		/// Virtual: Returns number of field entries on current node.
		unsigned int totNumLocalField() const;

		/// Virtual: Returns location of specified dataId
		unsigned int getNode( unsigned int dataId ) const;

		/// Virtual: Returns start DataIndex on specified node.
		unsigned int startDataIndex ( unsigned int node ) const;

		/// Virtual: Returns converted dataId to index on current node.
		unsigned int rawIndex( unsigned int dataId ) const;

		/**
		 * Virtual: True if this is a FieldElement having an array of
		 * fields on each data entry. So true in this case.
		 */
		bool hasFields() const {
			return true;
		}

		/// Virtual. Returns true if global, ie, if its parent is global
		bool isGlobal() const;

		/// Inherited virtual.
		unsigned int getNumOnNode( unsigned int node ) const;

		/////////////////////////////////////////////////////////////////
		// data access stuff
		/////////////////////////////////////////////////////////////////

		/**
		 * virtual.
		 * Looks up specified field field entry. First it finds the
		 * appropriate data entry from the rawIndex. Then it looks up
		 * the field using the lookupField.
		 * Returns the data entry specified by the rawIndex, fieldIndex.
		 *
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
		char* data( unsigned int rawIndex, unsigned int fieldIndex = 0 ) const;

		/**
		 * virtual
		 * Changes the number of entries in the data. Not permitted for
		 * FieldElements since they are just fields on the data.
		 */
		void resize( unsigned int newNumData );

		/**
		 * virtual.
		 * Changes the number of fields on the specified data entry.
		 */
		void resizeField(
				unsigned int rawIndex, unsigned int newNumField );

		/**
		 * Virtual: after replacing Cinfo of parent, we need to
		 * replace Cinfo and fef here. The zCinfo is the new Cinfo for this
		 * FieldElement.
		 */
		void zombieSwap( const Cinfo* zCinfo );

	private:
		Id parent_;
		const FieldElementFinfoBase* fef_;
};

#endif // _FIELD_ELEMENT_H
