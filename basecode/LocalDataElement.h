/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _LOCAL_DATA_ELEMENT_H
#define _LOCAL_DATA_ELEMENT_H

class SrcFinfo;
class FuncOrder;

/**
 * This is the class for handling the local portion of a data element
 * that is distributed over many nodes.
 * Does block-wise partitioning between nodes.
 */
class LocalDataElement: public DataElement
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
		 */
		LocalDataElement( Id id, const Cinfo* c, const string& name,
			unsigned int numData = 1 );

		/**
		 * This constructor copies over the original n times. It is
		 * used for doing all copies, in Shell::innerCopyElements.
		 */
		LocalDataElement( Id id, const Element* orig, unsigned int n );

		/**
		 * Virtual Destructor
		 */
		~LocalDataElement();

		/**
		 * Virtual copier. Makes a copy of self.
		 */
		Element* copyElement( Id newParent, Id newId, unsigned int n,
			bool toGlobal ) const;

		/////////////////////////////////////////////////////////////////
		// Information access fields
		/////////////////////////////////////////////////////////////////

		/// Inherited virtual. Returns number of data entries over all nodes
		unsigned int numData() const;

		/// Inherited virtual. Returns index of first entry on this node
		unsigned int localDataStart() const;

		/// Inherited virtual. Returns node location of specified object
		unsigned int getNode( unsigned int dataId ) const;

		/// Inherited virtual. Returns start DataIndex on specified node
		unsigned int startDataIndex( unsigned int node ) const;

		/// Converts dataId to index on current node.
		unsigned int rawIndex( unsigned int dataId ) const;


		/// Inherited virtual. Reports if this is Global, which it isn't
		bool isGlobal() const {
			return false;
		}

		/// Inherited virtual.
		unsigned int getNumOnNode( unsigned int node ) const;

		/////////////////////////////////////////////////////////////////
		// data access stuff
		/////////////////////////////////////////////////////////////////

		/**
		 * the data() function is inherited from DataElement and used as is.
		 */

		/**
		 * Inherited virtual.
		 * Changes the total number of data entries on Element in entire
		 * simulation. Not permitted for
		 * FieldElements since they are just fields on the data.
		 */
		void resize( unsigned int newNumData );

		/////////////////////////////////////////////////////////////////
		// Utility function for figuring out node data assignments when
		// the data array is resized.
		/////////////////////////////////////////////////////////////////
		unsigned int setDataSize( unsigned int numData );

	private:
		/**
		 * This is the total number of data entries on this Element, in
		 * the entire simulation. Note that these
		 * entries do not have to be on this node, some may be farmed out
		 * to other nodes.
		 */
		unsigned int numData_;

		/**
		 * This is the number of data entries per node, except for possibly
		 * the last node if they don't divide evenly. Useful for
		 * intermediate calculations.
		 */
		unsigned int numPerNode_;

		/**
		 * Precomputed value for start index of data on this node.
		 */
		unsigned int localDataStart_;
};

#endif // _LOCAL_DATA_ELEMENT_H
