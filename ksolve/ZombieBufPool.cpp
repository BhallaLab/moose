/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "../basecode/header.h"
#include "../kinetics/PoolBase.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "ZombiePool.h"
#include "ZombieBufPool.h"
#include "../kinetics/lookupVolumeFromMesh.h"

// Entirely derived from ZombiePool. Only the zombification routines differ.
const Cinfo* ZombieBufPool::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions: use virtual functions to deal with, the
    // moose definitions are inherited.
    //////////////////////////////////////////////////////////////
    static Dinfo< ZombieBufPool > dinfo( true );
    static Cinfo zombieBufPoolCinfo (
        "ZombieBufPool",
        ZombiePool::initCinfo(),
        0,
        0,
        &dinfo
    );

    return &zombieBufPoolCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* zombieBufPoolCinfo = ZombieBufPool::initCinfo();

ZombieBufPool::ZombieBufPool()
{;}

ZombieBufPool::~ZombieBufPool()
{;}

//////////////////////////////////////////////////////////////
// Field functions
//////////////////////////////////////////////////////////////

void ZombieBufPool::vSetN( const Eref& e, double v )
{
    ZombiePool::vSetN( e, v );
    ZombiePool::vSetNinit( e, v );
}

void ZombieBufPool::vSetNinit( const Eref& e, double v )
{
    vSetN( e, v );
}

void ZombieBufPool::vSetConc( const Eref& e, double conc )
{
    double n = NA * conc * lookupVolumeFromMesh( e );
    vSetN( e, n );
}

void ZombieBufPool::vSetConcInit( const Eref& e, double conc )
{
    vSetConc( e, conc );
}

bool ZombieBufPool::vGetIsBuffered( const Eref& e ) const
{
    return true;
}
