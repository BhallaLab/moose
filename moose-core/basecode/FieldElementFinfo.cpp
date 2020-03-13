/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../shell/Shell.h"
#include "../msg/OneToOneDataIndexMsg.h"

void FieldElementFinfoBase::postCreationFunc( Id parent, Element* parentElm ) const 
{
    static const Finfo* pf = Neutral::initCinfo()->findFinfo( "parentMsg" );
    static const Finfo* f1 = Neutral::initCinfo()->findFinfo( "childOut" );

    if ( deferCreate_ )
        return;
    Id kid = Id::nextId();
    Element* e = new FieldElement( parent, kid, fieldCinfo_, name(), this );
    Msg* m = new OneToOneDataIndexMsg( parent.eref(), Eref( e, 0 ), 0 );
    assert( m );

    if ( !f1->addMsg( pf, m->mid(), parent.element() ) )
    {
        cout << "FieldElementFinfoBase::postCreationFunc: Error: \n" <<
             " unable to add parent->child msg from " <<
             parent.element()->getName() << " to " << name() << "\n";
        return;
    }
}
