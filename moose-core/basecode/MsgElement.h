/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _MSG_ELEMENT_H
#define _MSG_ELEMENT_H

class SrcFinfo;
class FuncOrder;

/**
 * This is a specialized Element class for managing Msg data.
 * Basically it just gives a wrapper for the vectors managed by each
 * of the Msg classes.
 */
class MsgElement: public Element
{
	public:
		/**
		 * This is the main constructor, used by Shell::innerCreate
		 * which makes most Elements. Also used to create base
		 * Elements to init the simulator in main.cpp.
		 * Id is the Id of the new Element
		 * Cinfo is the class
		 * name is its name
		 */
		MsgElement( Id id, const Cinfo* c, const string& name,
			 unsigned int( *numMsg )(),
			 char* ( *lookupMsg )( unsigned int ) )
				:
					Element( id, c, name ),
					numMsg_( numMsg ),
					lookupMsg_( lookupMsg )
	   	{;}

		/**
		 * Virtual Destructor. Nothing to do here.
		 */
		~MsgElement()
		{;}

		/**
		 * Virtual copier. Doesn't do anything. The copy
		 * happens at the lower level, involving the Msg classes
		 * and the MsgElement just manages them.
		 */
		Element* copyElement( Id newParent, Id newId, unsigned int n,
			bool toGlobal ) const
		{
			return 0;
		}

		/////////////////////////////////////////////////////////////////
		// Information access fields
		/////////////////////////////////////////////////////////////////

		/// Inherited virtual. Returns number of data entries
		unsigned int numData() const
		{
			return numMsg_();
		}

		/// Inherited virtual. Returns number of data entries
		unsigned int numLocalData() const
		{
			return numMsg_();
		}

		/// Inherited virtual. Returns index of first data entry on node.
		unsigned int localDataStart() const
		{
			return 0;
		}

		/// Inherited virtual. Returns number of field entries for specified data
		unsigned int numField( unsigned int rawIndex ) const
		{
				return 1;
		}

		unsigned int totNumLocalField() const
		{
			return numMsg_();
		}

		/// Inherited virtual. Just returns current node.
		unsigned int getNode( unsigned int dataIndex ) const
		{
				return Shell::myNode();
		}

		/// Inherited virtual. As this is a global we always return zero.
		unsigned int startDataIndex( unsigned int node ) const
		{
				return 0;
		}

		/// Inherited virtual. Just returns index
		unsigned int rawIndex( unsigned int dataIndex ) const
		{
				return dataIndex;
		}

		/**
		 * Inherited virtual
		 * True if this is a FieldElement having an array of fields
		 * on each data entry. Clearly not true for the MsgElement.
		 */
		bool hasFields() const {
			return false;
		}

		bool isGlobal() const {
			return true;
		}

		unsigned int getNumOnNode( unsigned int node ) const {
			return numMsg_();
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
						unsigned int fieldIndex = 0 ) const
		{
			return lookupMsg_( rawIndex );
		}

		/**
		 * Inherited virtual
		 * Changes the number of entries in the data.
		 * Doesn't do anything for the MsgElement.
		 */
		void resize( unsigned int newNumData )
		{;}

		/**
		 * Inherited virtual.
		 * Changes the number of fields on the specified data entry.
		 * Doesn't do anything for the MsgElement.
		 */
		void resizeField( unsigned int rawIndex, unsigned int newNumField )
		{;}

		/////////////////////////////////////////////////////////////////
		/**
		 * Virtual func. The parent does the data swap part, so here it is
		 * just the Cinfo we replace.
		 */
		void zombieSwap( const Cinfo* newCinfo ) {
			replaceCinfo( newCinfo );
			Element::zombieSwap( newCinfo );
		}


	private:
		/**
		 * Function to return the # of Messages.
		 */
		 unsigned int( *numMsg_ )();

		/**
		 * Function to return a pointer to the selected Msg.
		 */
		 char* ( *lookupMsg_ )( unsigned int );
};

#endif // _Msg_ELEMENT_H
