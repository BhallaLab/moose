/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"

#include "../builtins/MooseParser.h"
#include "../builtins/Variable.h"
#include "../builtins/MooseParser.h"
#include "../builtins/Function.h"
#include "ZombieFunction.h"

#include "FuncTerm.h"
#include "RateTerm.h"
#include "../basecode/SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"

#define EPSILON 1e-15

const Cinfo* ZombieFunction::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions: mostly inherited from Function
    //////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions: All inherited from Function
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // SrcFinfo Definitions: All inherited from Function
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // SharedMsg Definitions: Override Function
    //////////////////////////////////////////////////////////////
    
    static DestFinfo process( "process",
            "Handles process call, updates internal time stamp.",
            new ProcOpFunc< ZombieFunction >( &ZombieFunction::process) );

    static DestFinfo reinit( "reinit",
            "Handles reinit call.",
            new ProcOpFunc< ZombieFunction >( &ZombieFunction::reinit ) );

    static Finfo* processShared[] =
    {
        &process, &reinit
    };

    static SharedFinfo proc( "proc",
            "This is a shared message to receive Process messages "
            "from the scheduler objects."
            "The first entry in the shared msg is a MsgDest "
            "for the Process operation. It has a single argument, "
            "ProcInfo, which holds lots of information about current "
            "time, thread, dt and so on. The second entry is a MsgDest "
            "for the Reinit operation. It also uses ProcInfo. ",
            processShared, sizeof( processShared ) / sizeof( Finfo* )
            );

    // Note that here the isOneZombie_ flag on the Dinfo constructor is
    // true. This means that the duplicate and copy operations only make
    // one copy, regardless of how big the array of zombie pools.
    // The assumption is that each Id has a single pool, which can be
    // present in many voxels.
    static Finfo *functionFinfos[] =
    {
        &proc,
    };

    static string doc[] =
    {
        "Name", "ZombieFunction",
        "Author", "Upi Bhalla",
        "Description",
        "ZombieFunction: Takes over Function, which is a general "
        "purpose function calculator using real numbers."
    };

    static Dinfo< ZombieFunction > dinfo;
    static Cinfo zombieFunctionCinfo (
        "ZombieFunction",
        Function::initCinfo(),
        functionFinfos,
        sizeof(functionFinfos) / sizeof(Finfo*),
        &dinfo,
        doc,
        sizeof(doc)/sizeof(string)
    );

    return &zombieFunctionCinfo;
}




//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* zombieFunctionCinfo = ZombieFunction::initCinfo();

ZombieFunction::ZombieFunction()
{;}

ZombieFunction::~ZombieFunction()
{;}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////
void ZombieFunction::process(const Eref &e, ProcPtr p)
{
    t_ = p->currTime;
}

void ZombieFunction::reinit(const Eref &e, ProcPtr p)
{;}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void ZombieFunction::setExpr( const Eref& e, string v )
{
    // Function::setExpr( e, v );
    if ( stoich_ )
    {
        Stoich* s = reinterpret_cast< Stoich* >( stoich_ );
        s->setFunctionExpr( e, v );
    }
    else
    {
        // I had this warning here but it is triggered needlessly when we
        // do an assignment of the Zombie function. So removed.
        // cout << "Warning: ZombieFunction::setExpr: Stoich not set.\n";
    }
}

//////////////////////////////////////////////////////////////
// Zombie conversion functions.
//////////////////////////////////////////////////////////////

void ZombieFunction::setSolver( Id ksolve, Id dsolve )
{
    if ( ksolve.element()->cinfo()->isA( "Ksolve" ) ||
            ksolve.element()->cinfo()->isA( "Gsolve" ) )
    {
        Id sid = Field< Id >::get( ksolve, "stoich" );
        stoich_ = ObjId( sid, 0 ).data();
        if ( stoich_ == 0 )
            cout << "Warning:ZombieFunction::setSolver: Empty Stoich on Ksolve" << ksolve.path() << endl;
    }
    else if ( ksolve == Id() )
    {
        stoich_ = 0;
    }
    else
    {
        cout << "Warning:ZombieFunction::setSolver: solver class " <<
             ksolve.element()->cinfo()->name() <<
             " not known.\nShould be Ksolve or Gsolve\n";
        stoich_ = 0;
    }

    /*
    if ( dsolve.element()->cinfo()->isA( "Dsolve" ) ) {
    		dsolve_= ObjId( dsolve, 0 ).data();
    } else if ( dsolve == Id() ) {
    		dsolve_ = 0;
    } else {
    		cout << "Warning:ZombieFunction::vSetSolver: solver class " <<
    				dsolve.element()->cinfo()->name() <<
    				" not known.\nShould be Dsolve\n";
    		dsolve_ = 0;
    }
    */
}

void ZombieFunction::zombify( Element* orig, const Cinfo* zClass,
                              Id ksolve, Id dsolve )
{
    //cout << "ZombieFunction::zombify: " << orig->id().path() << endl;
    if ( orig->cinfo() == zClass )
        return;
    // unsigned int start = orig->localDataStart();
    unsigned int num = orig->numLocalData();
    if ( num == 0 )
        return;
    if ( num > 1 )
        cout << "ZombieFunction::zombify: Warning: ZombieFunction doesn't\n"
             "handle volumes yet. Proceeding without this.\n";

    Function* f = reinterpret_cast< Function *>( Eref( orig, 0 ).data() );
    Function temp = *f;
    orig->zombieSwap( zClass );
    if ( zClass == ZombieFunction::initCinfo() )   // call SetSolver
    {
        ZombieFunction* zf = reinterpret_cast<ZombieFunction*>(Eref( orig, 0 ).data() );
        *zf = *static_cast< ZombieFunction* >(&temp);
        zf->setSolver( ksolve, dsolve );
    }
    else
    {
        Function* nf =
            reinterpret_cast< Function *>(Eref( orig, 0 ).data());
        *nf = temp;
    }

    /*
    // We can swap the class because the class data is identical, just
    // the moose expr and process handlers are different.
    if ( orig->cinfo() == ZombieFunction::initCinfo() ) { // unzombify
    	orig->replaceCinfo( Function::initCinfo() );
    } else { // zombify
    	orig->replaceCinfo( ZombieFunction::initCinfo() );
    	ZombieFunction* zf = reinterpret_cast< ZombieFunction *>(
    					Eref( orig, 0 ).data() );
    	zf->setSolver( ksolve, dsolve );
    }
    */
}
