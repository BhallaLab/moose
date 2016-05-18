/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _NEUTRAL_H
#define _NEUTRAL_H


#ifdef  CYMOOSE

#include "../basecode/ProcInfo.h"

class Cinfo;

#endif     /* -----  CYMOOSE  ----- */

class Neutral
{
	public:
		friend istream& operator >>( istream& s, Neutral& d );
		friend ostream& operator <<( ostream& s, const Neutral& d );
		Neutral();

		/////////////////////////////////////////////////////////////////
		// Field access functions
		/////////////////////////////////////////////////////////////////

		/**
		 * Field access functions for the entire object. For Neutrals 
		 * the setThis function is a dummy: it doesn't do anything because
		 * the Neutral has no data to set. However, the function name acts
		 * as a placeholder and derived objects can override the function
		 * so that the entire object can be accessed as a field and also
		 * for inter-node data transfer.
		 */
		void setThis( Neutral v );

		/**
		 * Field access functions for the entire object. For Neutrals 
		 * the getThis function does return the Neutral object, but it
		 * has no data to set. However, the function name acts
		 * as a placeholder and derived objects can override the function
		 * so that the entire object can be accessed as a field and also
		 * used for inter-node data transfer.
		 */
		Neutral getThis() const;

		/**
		 * Field access functions for the name of the Element/Neutral
		 */
		void setName( const Eref& e, string name );
		string getName( const Eref& e ) const;

		/**
		 * Readonly field access function for getting all outgoing Msgs.
		 */
		vector< ObjId > getOutgoingMsgs( const Eref& e ) const;

		/**
		 * Readonly field access function for getting all incoming Msgs.
		 */
		vector< ObjId > getIncomingMsgs( const Eref& e ) const;

		/**
		 * Readonly field access function for getting Ids connected to
		 * current Id via specified Field.
		 * Field is specified by its name.
		 * Returns an empty vector if it fails.
		 */
		vector< Id > getNeighbors( const Eref& e, string field ) const;

		/**
		 * Return vector of target ObjIds
		 * called by the specified SrcFinfo, by the current object.
		 * Twin function to getMsgDestFunctions.
		 */
		vector< ObjId > getMsgDests( const Eref& e, string src ) const;

		/**
		 * Return vector of function names called on each target ObjId
		 * of the specified SrcFinfo, by the current object.
		 * Twin function to getMsgDests.
		 */
		vector< string > getMsgDestFunctions( 
						const Eref& e, string src ) const;

		/**
		 * Returns True if the object is derived from the specified class
		 */
		bool isA( const Eref& e, string className ) const;

		/**
		 * Simply returns own ObjId
		 */
		ObjId getObjId( const Eref& e ) const;

		/**
		 * Looks up the full Id info for the parent of the current Element
		 */
		ObjId getParent( const Eref& e ) const;

		/**
		 * Looks up all the Element children of the current Element
		 */
		vector< Id > getChildren( const Eref& e ) const;

		/**
		 * Builds a vector of all descendants of e
		 */
		unsigned int buildTree( const Eref& e, vector< Id >& tree ) const;

		/**
		 * Traverses to root, building path.
		 */
		string getPath( const Eref& e ) const;

		/**
		 * Looks up the Class name of the current Element
		 */
		string getClass( const Eref& e ) const;

		/**
		 * numData is the number of data entries on this Element.
		 */
		void setNumData( const Eref& e, unsigned int num );
		unsigned int getNumData( const Eref& e ) const;

		/**
		 * numField is the size of the field array on the FieldElement 
		 * specified by the Eref including its data index.
		 */
		void setNumField( const Eref& e, unsigned int num );
		unsigned int getNumField( const Eref& e ) const;

		/// Id is the Id of the object, converted to unsigned int.
		unsigned int getId( const Eref& e ) const;
		/// Index is the dataIndex of the object
		unsigned int getIndex( const Eref& e ) const;
		/// fieldIndex specifies field entry if FieldElement. Otherwise zero
		unsigned int getFieldIndex( const Eref& e ) const;

		/**
		 * The clock Tick specifies the timing and order of execution
		 * of the 'process' action of this object in the simulation. 
		 * The timing is set by assigning the appropriate dt to
		 * this tick in the Clock object.
		 * A value of -1 means that the object is disabled.
		 */
		void setTick( const Eref& e, int num );
		/// Returns clock tick of this object.
		int getTick( const Eref& e ) const;

		/// Returns dt associated with this object based on its clock tick.
		double getDt( const Eref& e ) const;

		/// Information function to return names of all value Finfos.
		vector< string > getValueFields( const Eref& e ) const;
		/// Information function to return names of all SrcFinfos.
		vector< string > getSourceFields( const Eref& e ) const;
		/// Information function to return names of all DestFinfos.
		vector< string > getDestFields( const Eref& e ) const;
		////////////////////////////////////////////////////////////
		// DestFinfo functions
		////////////////////////////////////////////////////////////

		/**
		 * Destroys Element and all children
		 */
		void destroy( const Eref& e, int stage );

		/**
		 * Request conversion of data into a blockDataHandler subclass,
		 * and to carry out node balancing of data as per args.
		 */
		void blockNodeBalance( const Eref& e, 
			unsigned int, unsigned int, unsigned int );

		/**
		 * Request conversion of data into a generalDataHandler subclass,
		 * and to carry out node balancing of data as per args.
		 */
		void generalNodeBalance( const Eref& e,
			unsigned int myNode, vector< unsigned int > nodeAssignment );
		

		////////////////////////////////////////////////////////////
		// Static utility functions
		////////////////////////////////////////////////////////////

		/**
		 * Finds specific named child
		 */
		static Id child( const Eref& e, const string& name );

		/**
		 * Returns parent object
		 */
		static ObjId parent( const Eref& e );
		static ObjId parent( ObjId id );

		/**
		 * Checks if 'me' is a descendant of 'ancestor'
		 */
		static bool isDescendant( Id me, Id ancestor );

		/**
		 * Standard initialization function, used whenever we want to
		 * look up the class Cinfo
		 */
		static const Cinfo* initCinfo();

		/**
		 * return ids of all the children in ret.
		 */
		static void children( const Eref& e, vector< Id >& ret );

		/**
		 * Finds the path of element e
		 */
		static string path( const Eref& e );

		/**
		 * Checks if specified field is a global, typically because it is
		 * present on the Element and therefore should be assigned uniformly
		 * on all nodes
		 */
		static bool isGlobalField( const string& field );

	private:
		// string name_;
};

#endif // _NEUTRAL_H
