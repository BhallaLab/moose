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
//	ObjId I/O
//////////////////////////////////////////////////////////////

// Doesn't know how to deal with off-node bad fields.
bool ObjId::bad() const
{
    Element* elm = id.element();
    return ( elm == 0 ||
             dataIndex == BADINDEX ||
             fieldIndex == BADINDEX ||
             dataIndex >= elm->numData()
             /*
              * We have a bit of a problem here. The FieldElement can exist
              * with zero fields, and is invalid for field lookups but valid
              * for the element level lookups. I should rather not create the
              * FieldElements this way.
             || (
              	elm->getNode( dataIndex ) == Shell::myNode() &&
              	fieldIndex >= elm->numField( dataIndex - elm->localDataStart() )
             )
              */
           );
}

ostream& operator <<( ostream& s, const ObjId& i )
{
    if ( i.dataIndex == 0 && i.fieldIndex == 0 )
        s << i.id;
    else if ( i.fieldIndex == 0 )
        s << i.id << "[" << i.dataIndex << "]";
    else
        s << i.id << "[" << i.dataIndex << "][" << i.fieldIndex << "]";
    return s;
}

/**
 * need to complete implementation
 */
istream& operator >>( istream& s, ObjId& i )
{
    s >> i.id;
    return s;
}

//////////////////////////////////////////////////////////////
ObjId::ObjId( const string& path )
{
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    assert( shell );
    *this = shell->doFind( path );
}

Eref ObjId::eref() const
{
    return Eref( id.element(), dataIndex, fieldIndex );
}

bool ObjId::operator==( const ObjId& other ) const
{
    return ( id == other.id && dataIndex == other.dataIndex &&
             fieldIndex == other.fieldIndex );
}

bool ObjId::operator!=( const ObjId& other ) const
{
    return ( id != other.id || dataIndex != other.dataIndex ||
             fieldIndex != other.fieldIndex );
}

bool ObjId::operator<( const ObjId& other ) const
{
    return ( id < other.id ||
             (id == other.id && (
                  dataIndex < other.dataIndex  ||
                  ( dataIndex == other.dataIndex &&
                    fieldIndex < other.fieldIndex )
              )
             )
           );
}

bool ObjId::isDataHere() const
{
    return ( id.element()->getNode( dataIndex ) == Shell::myNode() );
}

bool ObjId::isGlobal() const
{
    return ( id.element()->isGlobal() );
}

bool ObjId::isOffNode() const
{
    return ( Shell::numNodes() > 1 &&
             ( id.element()->isGlobal() ||
               id.element()->getNode( dataIndex ) != Shell::myNode() )
           );
}

char* ObjId::data() const
{
    return id.element()->data(
            id.element()->rawIndex( dataIndex ), fieldIndex 
        );
}

string ObjId::path() const
{
    return Neutral::path( eref() );
}

Element* ObjId::element() const
{
    return id.element();
}
