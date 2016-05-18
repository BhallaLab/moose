/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ID_H
#define _ID_H

/**
 * This class manages id lookups for elements. Ids provide a uniform
 * handle for every object, independent of which node they are located on.
 */
class Id
{
	public:
		//////////////////////////////////////////////////////////////
		//	Id creation
		//////////////////////////////////////////////////////////////
		/**
		 * Returns the root Id
		 */
		Id();

		/**
		 * Creates an id with the specified Element number
		 */
		Id( unsigned int id );

		/**
		 * Returns an id found by traversing the specified path
		 */
		Id( const std::string& path, const std::string& separator = "/" );

		/**
		 * Downconverts an OjbId to an Id
		 */
		Id( const ObjId& oi );

		/**
		 * Destroys an Id. Doesn't do anything much.
		 */
		~Id(){}

		//////////////////////////////////////////////////////////////
		//	Element creation and deletion.
		//////////////////////////////////////////////////////////////

//		static Id create( Element* e );

		/**
		 * Reserves an id for assigning to an Element. Each time it is
		 * called a new id is reserved, even if previous ones have not been
		 * used yet.
		 */
		static Id nextId();


		/**
		 * Returns the number of Ids in use.
		 */
		static unsigned int numIds();

		/**
		 * The specified element is placed into current id.
		 */
		void bindIdToElement( Element* e ); 

		/**
		 * Cleanly deletes the associated Element, and zeroes out
		 * contents of elements vector at the location pointed to by
		 * this->id_.
		 */
		void destroy() const;
    
		//////////////////////////////////////////////////////////////
		//	Id info
		//////////////////////////////////////////////////////////////
		/**
		 * Returns the full pathname of the object on the id.
		 */
		std::string path( const std::string& separator = "/" ) const;


		/**
		 * Returns the Element pointed to by the id
		 * If it is off-node, returns an allocated wrapper element with 
		 * postmaster and id information. Calling func has to free it.
		 * This wrapper element may also point to UNKNOWN NODE, in which
		 * case the master node IdManager has to figure out where it
		 * belongs.
		 * Returns 0 on failure.
		 * Deprecated.
		Element* operator()() const;
		 */

		/**
		 * Returns the Element pointed to by the Id.
		 * Perhaps cleaner to use than operator()() as it is an explicit 
		 * function.
		 */
		Element* element() const;

//		unsigned int index() const;

		/**
		 * Returns the Eref to the element plus index
		 */
		Eref eref() const;

		/**
		 * Returns an id whose value is string-converted from the 
		 * specified string. 
		 */
		static Id str2Id( const std::string& s );

		/**
		 * Returns a string holding the ascii value of the id_ .
		 */
		static std::string id2str( Id id );

		unsigned int value() const;

		//////////////////////////////////////////////////////////////
		//	Comparisons between ids
		//////////////////////////////////////////////////////////////
		bool operator==( const Id& other ) const {
			// return id_ == other.id_ && index_ == other.index_;
			return id_ == other.id_;
		}

		bool operator!=( const Id& other ) const {
			// return id_ != other.id_ || index_ != other.index_;
			return id_ != other.id_;
		}

		bool operator<( const Id& other ) const {
		//	return ( id_ < other.id_ ) ||
		//		( id_ == other.id_ && index_ < other.index_ );
			return ( id_ < other.id_ );
		}

    // The follwoing two functions check if the Id is associated with
    // an existing element. Needed for handling objects that have been destroyed.
        static bool isValid(Id id)
        {
            return (id.id_ < elements().size()) && (elements()[id.id_] != 0);
        }

        static bool isValid(unsigned int id)
        {
            return (id < elements().size()) && (elements()[id] != 0);
        }                

		//////////////////////////////////////////////////////////////
		/**
		 * Used to clean up all Elements when the simulation is terminated
		 */
		static void clearAllElements();

		/**
		 * Used to clean out any specific Id.
		 */
		void zeroOut() const;
		//////////////////////////////////////////////////////////////

		friend ostream& operator <<( ostream& s, const Id& i );
		friend istream& operator >>( istream& s, Id& i );

	private:
		// static void setManager( Manager* m );
		unsigned int id_; // Unique identifier for Element*
//		unsigned int index_; // Index of array entry within element.
		static vector< Element* >& elements();
};

#endif // _ID_H
