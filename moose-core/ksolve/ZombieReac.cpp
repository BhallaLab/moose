/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../kinetics/ReacBase.h"
#include "../kinetics/lookupVolumeFromMesh.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "../basecode/SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"
#include "ZombieReac.h"

// #include "Reac.h"
// #include "ElementValueFinfo.h"
// #include "DataHandlerWrapper.h"

const Cinfo* ZombieReac::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions: All inherited.
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions: All inherited.
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// SharedMsg Definitions: All inherited
		//////////////////////////////////////////////////////////////

	static Dinfo< ZombieReac > dinfo( true );
	static Cinfo zombieReacCinfo (
		"ZombieReac",
		ReacBase::initCinfo(),
		0,
		0,
		&dinfo
	);

	return &zombieReacCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* zombieReacCinfo = ZombieReac::initCinfo();

static const SrcFinfo2< double, double >* subOut =
 	dynamic_cast< const SrcFinfo2< double, double >* >(
					zombieReacCinfo->findFinfo( "subOut" ) );

static const SrcFinfo2< double, double >* prdOut =
 	dynamic_cast< const SrcFinfo2< double, double >* >(
					zombieReacCinfo->findFinfo( "prdOut" ) );

ZombieReac::ZombieReac()
		: stoich_( 0 )
{;}

ZombieReac::~ZombieReac()
{;}


//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

/*
void ZombieReac::vRemesh( const Eref& e )
{
	stoich_->setReacKf( e, concKf_ );
	stoich_->setReacKb( e, concKb_ );
}
*/

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

// This conversion is deprecated, used mostly for kkit conversions.
void ZombieReac::vSetNumKf( const Eref& e, double v )
{
	double volScale = convertConcToNumRateUsingMesh( e, subOut, 0 );
	concKf_ = v * volScale;
	stoich_->setReacKf( e, concKf_ );
}

double ZombieReac::vGetNumKf( const Eref& e ) const
{
	// Return value for voxel 0. Conceivably I might want to use the
	// DataId part to specify which voxel to use, but that isn't in the
	// current definition for Reacs as being a single entity for the entire
	// compartment.
	double volScale = convertConcToNumRateUsingMesh( e, subOut, 0 );
	return concKf_ / volScale;
}

// Deprecated, used for kkit conversion backward compatibility
void ZombieReac::vSetNumKb( const Eref& e, double v )
{
	double volScale = convertConcToNumRateUsingMesh( e, prdOut, 0 );
	concKb_ = v * volScale;
	stoich_->setReacKb( e, concKb_ );
}

double ZombieReac::vGetNumKb( const Eref& e ) const
{
	double volScale = convertConcToNumRateUsingMesh( e, prdOut, 0 );
	return concKb_ / volScale;
}

void ZombieReac::vSetConcKf( const Eref& e, double v )
{
	concKf_ = v;
	stoich_->setReacKf( e, v );
}

double ZombieReac::vGetConcKf( const Eref& e ) const
{
	return concKf_;
}

void ZombieReac::vSetConcKb( const Eref& e, double v )
{
	concKb_ = v;
	stoich_->setReacKb( e, v );
}

double ZombieReac::vGetConcKb( const Eref& e ) const
{
	// double volScale = convertConcToNumRateUsingMesh( e, prdOut, 0 );
	// double kb = stoich_->getReacNumKb( e );
	// return kb / volScale;
	return concKb_;
}

//////////////////////////////////////////////////////////////
// Utility function
//////////////////////////////////////////////////////////////

// Virtual func called in zombify before fields are assigned.
void ZombieReac::setSolver( Id stoich, Id orig )
{
	assert( stoich != Id() );

	vector< Id > sub;
	vector< Id > prd;
	orig.element()->getNeighbors( sub, subOut );
	orig.element()->getNeighbors( prd, prdOut );

	assert( stoich.element()->cinfo()->isA( "Stoich" ) );
	stoich_ = reinterpret_cast< Stoich* >( stoich.eref().data( ) );
	stoich_->installReaction( orig, sub, prd );
}
