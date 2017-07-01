/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _GLOBAL_DATA_ELEMENT_H
#define _GLOBAL_DATA_ELEMENT_H

class SrcFinfo;
class FuncOrder;

/**
 * This is the usual class for managing the MOOSE interface.
 * Handles the data.
 * Elaborates on base class for how it handles destruction.
 * Does block-wise partitioning between nodes.
 */
class GlobalDataElement: public DataElement
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
		GlobalDataElement( Id id, const Cinfo* c, const string& name,
			unsigned int numData = 1 );

		/**
		 * This constructor copies over the original n times. It is
		 * used for doing all copies, in Shell::innerCopyElements.
		 */
		GlobalDataElement( Id id, const Element* orig, unsigned int n );

		/**
		 * Virtual Destructor
		 */
		~GlobalDataElement();

		/**
		 * Virtual copier. Makes a copy of self.
		 */
		Element* copyElement( Id newParent, Id newId, unsigned int n,
			bool toGlobal ) const;

		/////////////////////////////////////////////////////////////////
		// Information access fields
		/////////////////////////////////////////////////////////////////

		/// Inherited virtual. Returns number of data entries
		unsigned int numData() const;

		/// Inherited virtual. Returns index of first entry on this node
		unsigned int localDataStart() const {
			return 0;
		}

		/// Inherited virtual. Returns node location of specified object
		unsigned int getNode( unsigned int dataIndex ) const;

		/// Inherited virtual. Returns start dataIndex on specified node
		unsigned int startDataIndex( unsigned int node ) const
		{
			return 0;
		}

		/// Converts dataId to index on current node.
		unsigned int rawIndex( unsigned int dataId ) const;

		bool isGlobal() const {
			return true;
		}

		/// Inherited virtual.
		unsigned int getNumOnNode( unsigned int node ) const;

		/////////////////////////////////////////////////////////////////
		// data access stuff: All is just inherited.
		/////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////

	private:
		// All is inherited from DataElement.

};

#endif // _GLOBAL_DATA_ELEMENT_H
