/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "PoolBase.h"
#include "Pool.h"
#include "BufPool.h"
#include "lookupVolumeFromMesh.h"

#define EPSILON 1e-15

const Cinfo* BufPool::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////
    // SharedMsg Definitions
    //////////////////////////////////////////////////////////////

    static Dinfo< BufPool > dinfo;
    static Cinfo bufPoolCinfo (
        "BufPool",
        Pool::initCinfo(),
        0,
        0,
        &dinfo
    );

    return &bufPoolCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* bufPoolCinfo = BufPool::initCinfo();

BufPool::BufPool()
{;}

BufPool::~BufPool()
{;}

//////////////////////////////////////////////////////////////
// Field definitions
//////////////////////////////////////////////////////////////

void BufPool::vSetN( const Eref& e, double v )
{
    Pool::vSetN( e, v );
    Pool::vSetNinit( e, v );
}

void BufPool::vSetNinit( const Eref& e, double v )
{
    vSetN( e, v );
}

void BufPool::vSetConc( const Eref& e, double conc )
{
    double n = NA * conc * lookupVolumeFromMesh( e );
    vSetN( e, n );
}

void BufPool::vSetConcInit( const Eref& e, double conc )
{
    vSetConc( e, conc );
}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void BufPool::vProcess( const Eref& e, ProcPtr p )
{
    Pool::vReinit( e, p );
}

void BufPool::vReinit( const Eref& e, ProcPtr p )
{
    Pool::vReinit( e, p );
}


//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

