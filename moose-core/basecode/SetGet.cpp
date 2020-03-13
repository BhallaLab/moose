/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SetGet.h"
#include "../shell/Shell.h"
#include "../shell/Neutral.h"

const OpFunc* SetGet::checkSet( const string& field, ObjId& tgt, FuncId& fid ) 
{
    // string field = "set_" + destField;
    const Finfo* f = tgt.element()->cinfo()->findFinfo( field );
    if ( !f )   // Could be a child element? Note that field name will
    {
        // change from set_<name> to just <name>
        string f2 = field.substr( 3 );
        Id child = Neutral::child( tgt.eref(), f2 );
        if ( child == Id() )
        {
            cout << "Error: SetGet:checkSet:: No field or child named '" <<
                 field << "' was found on\n" << tgt.id.path() << endl;
        }
        else
        {
            if ( field.substr( 0, 3 ) == "set" )
                f = child.element()->cinfo()->findFinfo( "setThis" );
            else if ( field.substr( 0, 3 ) == "get" )
                f = child.element()->cinfo()->findFinfo( "getThis" );
            assert( f ); // should always work as Neutral has the field.
            if ( child.element()->numData() == tgt.element()->numData() )
            {
                tgt = ObjId( child, tgt.dataIndex, tgt.fieldIndex );
                if ( !tgt.isDataHere() )
                    return 0;
            }
            else if ( child.element()->numData() <= 1 )
            {
                tgt = ObjId( child, 0 );
                if ( !tgt.isDataHere() )
                    return 0;
            }
            else
            {
                cout << "SetGet::checkSet: child index mismatch\n";
                return 0;
            }
        }
    }

    const DestFinfo* df = dynamic_cast< const DestFinfo* >( f );
    if ( !df )
        return 0;

    fid = df->getFid();
    const OpFunc* func = df->getOpFunc();
    assert( func );
    return func;
}

/////////////////////////////////////////////////////////////////////////

// Static function
bool SetGet::strGet( const ObjId& tgt, const string& field, string& ret )
{
    const Finfo* f = tgt.element()->cinfo()->findFinfo( field );
    if ( !f )
    {
        cout << Shell::myNode() << ": Error: SetGet::strGet: Field " <<
             field << " not found on Element " << tgt.element()->getName() <<
             endl;
        return 0;
    }
    return f->strGet( tgt.eref(), field, ret );
}

bool SetGet::strSet( const ObjId& tgt, const string& field, const string& v)
{
    const Finfo* f = tgt.element()->cinfo()->findFinfo( field );
    if ( !f )
    {
        cout << Shell::myNode() << ": Error: SetGet::strSet: Field " <<
             field << " not found on Element " << tgt.element()->getName() <<
             endl;
        return 0;
    }
    return f->strSet( tgt.eref(), field, v );
}
