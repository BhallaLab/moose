/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "../basecode/header.h"
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

#include "../kinetics/EnzBase.h"
#include "ZombieMMenz.h"

#define EPSILON 1e-15

const Cinfo* ZombieMMenz::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// Shared Msg Definitions
		//////////////////////////////////////////////////////////////

    static string doc[] = {
        "Name", "ZombieMMenz",
        "Author", "Upi Bhalla",
        "Description", "Zombie class for MM (Michaelis-Menten) enzyme."
	};
	static Dinfo< ZombieMMenz > dinfo;
	static Cinfo zombieMMenzCinfo (
		"ZombieMMenz",
		EnzBase::initCinfo(),
		0,
		0,
		&dinfo,
        doc,
        sizeof(doc)/sizeof(string)
	);

	return &zombieMMenzCinfo;
}

//////////////////////////////////////////////////////////////

static const Cinfo* zombieMMenzCinfo = ZombieMMenz::initCinfo();

static const SrcFinfo2< double, double >* subOut =
    dynamic_cast< const SrcFinfo2< double, double >* >(
	zombieMMenzCinfo->findFinfo( "subOut" ) );

static const SrcFinfo2< double, double >* prdOut =
	dynamic_cast< const SrcFinfo2< double, double >* >(
	zombieMMenzCinfo->findFinfo( "prdOut" ) );

//////////////////////////////////////////////////////////////
// ZombieMMenz internal functions
//////////////////////////////////////////////////////////////


ZombieMMenz::ZombieMMenz( )
	: Km_( 0.005 )
{;}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

/*
void ZombieMMenz::vRemesh( const Eref& e )
{
	// cout << "ZombieMMenz::remesh for " << e << endl;
	stoich_->setMMenzKm( e, Km_ );
}
*/

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

// Note that the units in the Stoich are where 1 uM = 1 molecule/voxel.
// So we set the conc terms directly.
void ZombieMMenz::vSetKm( const Eref& e, double v )
{
	Km_ = v;
	stoich_->setMMenzKm( e, v );
	//double volScale = convertConcToNumRateUsingMesh( e, subOut, 1 );
	//stoich_->setMMenzKm( e, v * volScale );
}

double ZombieMMenz::vGetKm( const Eref& e ) const
{
	return Km_;
}

void ZombieMMenz::vSetNumKm( const Eref& e, double v )
{
	double volScale = convertConcToNumRateUsingMesh( e, subOut, 1 );
	Km_ = v / volScale;
	stoich_->setMMenzKm( e, Km_ );
}

double ZombieMMenz::vGetNumKm( const Eref& e ) const
{
	double volScale = convertConcToNumRateUsingMesh( e, subOut, 1 );
	return Km_ * volScale;
	// return stoich_->getMMenzNumKm( e );
}

void ZombieMMenz::vSetKcat( const Eref& e, double v )
{
	if ( v < EPSILON )
		v = EPSILON;
	stoich_->setMMenzKcat( e, v );
}

double ZombieMMenz::vGetKcat( const Eref& e ) const
{
	return stoich_->getMMenzKcat( e );
}

//////////////////////////////////////////////////////////////
// Utility function
//////////////////////////////////////////////////////////////

void ZombieMMenz::setSolver( Id solver, Id enzId )
{
	static const DestFinfo* enzFinfo = dynamic_cast< const DestFinfo* >(
		EnzBase::initCinfo()->findFinfo( "enzDest" ) );
	static const SrcFinfo* subFinfo = dynamic_cast< const SrcFinfo* >(
		EnzBase::initCinfo()->findFinfo( "subOut" ) );
	static const SrcFinfo* prdFinfo = dynamic_cast< const SrcFinfo* >(
		EnzBase::initCinfo()->findFinfo( "prdOut" ) );
	assert( enzFinfo );
	assert( subFinfo );
	assert( prdFinfo );

	assert( solver.element()->cinfo()->isA( "Stoich" ) );
	stoich_ = reinterpret_cast< Stoich* >( solver.eref().data() );

	/// Now set up the RateTerm
	vector< Id > enzvec;
	vector< Id > subvec;
	vector< Id > prdvec;
	unsigned int num = enzId.element()->getNeighbors( enzvec, enzFinfo );
	num = enzId.element()->getNeighbors( subvec, subFinfo );
	num = enzId.element()->getNeighbors( prdvec, prdFinfo );
	stoich_->installMMenz( enzId, enzvec, subvec, prdvec );
}
