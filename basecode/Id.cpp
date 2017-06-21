/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../shell/Shell.h"

//////////////////////////////////////////////////////////////
//	Id creation
//////////////////////////////////////////////////////////////

Id::Id()
// : id_( 0 ), index_( 0 )
    : id_( 0 )
{
    ;
}

Id::Id( unsigned int id )
    : id_( id )
{
    ;
}

Id::Id( const string& path, const string& separator )
{
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    assert( shell );
    id_ = shell->doFind( path ).id.id_;
}

Id::Id( const ObjId& oi )
    : id_( oi.id.id_ )
{
    ;
}

/**
 * Static func to extract an id from a string. We need to accept ids
 * out of the existing range, but it would be nice to have a heuristic
 * on how outrageous the incoming value is.
 */
Id Id::str2Id( const std::string& s )
{
    // unsigned int val = atoi( s.c_str() );
    return Id( s );
}

//////////////////////////////////////////////////////////////
//	Element array static access function. Private.
//////////////////////////////////////////////////////////////

vector< Element* >& Id::elements()
{
    static vector< Element* > e;
    return e;
}

//////////////////////////////////////////////////////////////
//	Id info
//////////////////////////////////////////////////////////////

// static func to convert id into a string.
string Id::id2str( Id id )
{
    return id.path();
}

// Function to convert it into its fully separated path.
string Id::path( const string& separator) const
{
    string ret = Neutral::path( eref() );
    // Trim off trailing []
    assert( ret.length() > 0 );
    // the 'back' operation is not supported by pre 2011 compilers

    while ( ret[ ret.length() - 1 ] == ']' )
    {
        size_t pos = ret.find_last_of( '[' );
        if ( pos != string::npos && pos > 0 )
        {
            ret = ret.substr( 0, pos );
        }
    }

    return ret;
}

/**
 * Here we work with a single big array of all ids. Off-node elements
 * are represented by their postmasters. When we hit a postmaster we
 * put the id into a special field on it. Note that this is horrendously
 * thread-unsafe.
 * \todo: I need to replace the off-node case with a wrapper Element
 * return. The object stored here will continue to be the postmaster,
 * and when this is detected it will put the postmaster ptr and the id
 * into the wrapper element. The wrapper's own id will be zero so it
 * can be safely deleted.
 * deprecated
Element* Id::operator()() const
{
	return elements()[ id_ ];
}
 */

/// Synonym for Id::operator()()
Element* Id::element() const
{
    return elements()[ id_ ];
}

/*
unsigned int Id::index() const
{
	return index_;
}
*/

Eref Id::eref() const
{
    return Eref( elements()[ id_ ], 0 );
    // return Eref( elements()[ id_ ], index_ );
}

// Static func.
Id Id::nextId()
{
    // Should really look up 'available' list.
    // Should really put the returned value onto the 'reserved' list
    // so they don't go dangling.
    Id ret( elements().size() );
    elements().push_back( 0 );
    return ret;
}

// Static func.
unsigned int Id::numIds()
{
    return elements().size();
}

void Id::bindIdToElement( Element* e )
{
    if ( elements().size() <= id_ )
    {
        if ( elements().size() % 1000 == 0 )
        {
            elements().reserve( elements().size() + 1000 );
        }
        elements().resize( id_ + 1, 0 );
    }
    assert( elements()[ id_ ] == 0 );
    /*
    if ( elements()[ id_ ] != 0 )
    	cout << "Warning: assigning Element to existing id " << id_ << "\n";
    	*/
    elements()[ id_ ] = e;
    // cout << "Id::bindIdToElement '" << e->getName() << "' = " << id_ << endl;
}

/*
Id Id::create( Element* e )
{
	Id ret( elements().size() );
	elements().push_back( e );
	return ret;
}
*/

void Id::destroy() const
{
    if ( elements()[ id_ ] )
    {
        // cout << "Id::destroy '" << elements()[ id_ ]->getName() << "' = " << id_ << endl;
        delete elements()[ id_ ];
        elements()[ id_ ] = 0;
        // Put id_ on 'available' list
    }
    else
    {
        cout << "Warning: Id::destroy: " << id_ << " already zeroed\n";
    }
}

void Id::zeroOut() const
{
    assert ( id_ < elements().size() );
    elements()[ id_ ] = 0;
}

unsigned int Id::value() const
{
    return id_;
}

void Id::clearAllElements()
{
    for ( vector< Element* >::iterator
            i = elements().begin(); i != elements().end(); ++i )
    {
        if ( *i )
        {
            (*i)->clearAllMsgs();
            delete *i;
        }
    }
}

//////////////////////////////////////////////////////////////
//	Id utility
//////////////////////////////////////////////////////////////

ostream& operator <<( ostream& s, const Id& i )
{
    s << i.id_;
    /*
    if ( i.index_ == 0 )
    	s << i.id_;
    else
    	s << i.id_ << "[" << i.index_ << "]";
    */
    return s;
}

istream& operator >>( istream& s, Id& i )
{
    s >> i.id_;
    return s;
}

/*
Id::Id( unsigned int id, unsigned int index )
	: id_( id ), index_( index )
{
	;
}
*/
