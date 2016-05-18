/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../shell/Shell.h"

FieldElement::FieldElement( Id parent, Id self,
                            const Cinfo* c, const string& name,
                            const FieldElementFinfoBase* fef
                          )
    :
    Element( self, c, name ),
    parent_( parent ),
    fef_( fef )
{
    ;
}

/**
 * Virtual func: leave the clearing of the messages to
 * ElementBase, the data is cleared by the parent Element of
 * this field.
 */
FieldElement::~FieldElement()
{
    ;
}

Element* FieldElement::copyElement( Id newParent, Id newId,
                                    unsigned int n, bool toGlobal ) const
{
    return new FieldElement( newParent, newId, cinfo(), getName(), fef_ );
}

/////////////////////////////////////////////////////////////////////////
// Element info functions
/////////////////////////////////////////////////////////////////////////

unsigned int FieldElement::numData() const
{
    return parent_.element()->numData();
}

unsigned int FieldElement::numLocalData() const
{
    return parent_.element()->numLocalData();
}

unsigned int FieldElement::localDataStart() const
{
    return parent_.element()->localDataStart();
}

unsigned int FieldElement::numField( unsigned int rawIndex ) const
{
    const char* data = parent_.element()->data( rawIndex );
    assert( data );
    return fef_->getNumField( data );
}

unsigned int FieldElement::totNumLocalField() const
{
    unsigned int ret = 0;
    unsigned int nd = numLocalData();
    for ( unsigned int i = 0; i < nd; ++i )
        ret += numField( i );
    return ret;
}

unsigned int FieldElement::getNode( unsigned int dataId ) const
{
    return parent_.element()->getNode( dataId );
}

unsigned int FieldElement::startDataIndex( unsigned int node ) const
{
    return parent_.element()->startDataIndex( node );
}

unsigned int FieldElement::rawIndex( unsigned int dataId ) const
{
    return parent_.element()->rawIndex( dataId );
}

bool FieldElement::isGlobal() const
{
    return parent_.element()->isGlobal();
}

unsigned int FieldElement::getNumOnNode( unsigned int node ) const
{
    if ( node == Shell::myNode() || parent_.element()->isGlobal() )
    {
        return totNumLocalField();
    }
    // Here we need to refer to a postmaster function to get the
    // number of field entries on specified node. For now, a dummy.
    return totNumLocalField();
}

/////////////////////////////////////////////////////////////////////////

char* FieldElement::data( unsigned int rawIndex, unsigned int fieldIndex )
const
{
    char* data = parent_.element()->data( rawIndex );
    return fef_->lookupField( data, fieldIndex );
}

void FieldElement::resize( unsigned int newNumData )
{
    assert( 0 );
}

void FieldElement::resizeField(
    unsigned int rawIndex, unsigned int newNumField )
{
    char* data = parent_.element()->data( rawIndex );
    fef_->setNumField( data, newNumField );
}

/////////////////////////////////////////////////////////////////////////

void FieldElement::zombieSwap( const Cinfo* zCinfo )
{
    const Finfo* f = zCinfo->findFinfo( getName() );
    assert( f );

    const FieldElementFinfoBase* zfef = dynamic_cast<
                                        const FieldElementFinfoBase* >( f );
    assert( zfef );
    fef_ = zfef;
    replaceCinfo( zCinfo );
}
