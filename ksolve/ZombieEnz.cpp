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
#include "../kinetics/CplxEnzBase.h"
#include "ZombieEnz.h"

#define EPSILON 1e-15

const Cinfo* ZombieEnz::initCinfo()
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

	static Dinfo< ZombieEnz > dinfo;
	static Cinfo zombieEnzCinfo (
		"ZombieEnz",
		CplxEnzBase::initCinfo(),
		0,
		0,
		&dinfo
	);

	return &zombieEnzCinfo;
}
//////////////////////////////////////////////////////////////

static const Cinfo* zombieEnzCinfo = ZombieEnz::initCinfo();

static const SrcFinfo2< double, double >* subOut =
	dynamic_cast< const SrcFinfo2< double, double >* >(
	zombieEnzCinfo->findFinfo( "subOut" ) );

//////////////////////////////////////////////////////////////
// ZombieEnz internal functions
//////////////////////////////////////////////////////////////

ZombieEnz::ZombieEnz( )
		:
				stoich_( 0 ),
				concK1_( 1.0 )
{ ; }

ZombieEnz::~ZombieEnz( )
{ ; }

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

/*
void ZombieEnz::vRemesh( const Eref& e )
{
	stoich_->setEnzK1( e, concK1_ );
}
*/


//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

// v is in number units.
void ZombieEnz::vSetK1( const Eref& e, double v )
{
	double volScale =
		convertConcToNumRateUsingMesh( e, subOut, true );

	concK1_ = v * volScale;
	stoich_->setEnzK1( e, concK1_ );
}

// k1 is In number units.
double ZombieEnz::vGetK1( const Eref& e ) const
{
	// return stoich_->getEnzNumK1( e );
	double volScale =
		convertConcToNumRateUsingMesh( e, subOut, true );

	return concK1_ / volScale;
}

void ZombieEnz::vSetK2( const Eref& e, double v )
{
	stoich_->setEnzK2( e, v );
}

double ZombieEnz::vGetK2( const Eref& e ) const
{
	return stoich_->getEnzK2( e );
}

void ZombieEnz::vSetKcat( const Eref& e, double v )
{
	double k2 = getK2( e );
	double k3 = getKcat( e );
	double ratio = 4.0;
	if ( v < EPSILON )
		v = EPSILON;
	if ( k3 > EPSILON ) {
		ratio = k2/k3;
	}
	double Km = (k2 + k3) / concK1_;
	concK1_ = v * (1.0 + ratio) / Km;

	stoich_->setEnzK1( e, concK1_ );
	stoich_->setEnzK3( e, v );
	stoich_->setEnzK2( e, v * ratio );
}

double ZombieEnz::vGetKcat( const Eref& e ) const
{
	return stoich_->getEnzK3( e );
}


void ZombieEnz::vSetKm( const Eref& e, double v )
{
	double k2 = getK2( e );
	double k3 = getKcat( e );
	concK1_ = ( k2 + k3 ) / v;
	stoich_->setEnzK1( e, concK1_ );
}

double ZombieEnz::vGetKm( const Eref& e ) const
{
	double k2 = getK2( e );
	double k3 = getKcat( e );

	return ( k2 + k3 ) / concK1_;
}

void ZombieEnz::vSetNumKm( const Eref& e, double v )
{
	double k2 = getK2( e );
	double k3 = getKcat( e );
	double volScale =
		convertConcToNumRateUsingMesh( e, subOut, 1 );
	concK1_ = ( k2 + k3 ) / ( v * volScale );

	stoich_->setEnzK1( e, concK1_ );
}

double ZombieEnz::vGetNumKm( const Eref& e ) const
{
	double k2 = vGetK2( e );
	double k3 = vGetKcat( e );
	double volScale =
		convertConcToNumRateUsingMesh( e, subOut, 1 );

	return volScale * ( k2 + k3 ) / concK1_;
}

void ZombieEnz::vSetRatio( const Eref& e, double v )
{
	double Km = getKm( e );
	double k2 = getK2( e );
	double k3 = getKcat( e );

	k2 = v * k3;

	stoich_->setEnzK2( e, k2 );
	double k1 = ( k2 + k3 ) / Km;

	setConcK1( e, k1 );
}

double ZombieEnz::vGetRatio( const Eref& e ) const
{
	double k2 = getK2( e );
	double k3 = getKcat( e );
	return k2 / k3;
}

void ZombieEnz::vSetConcK1( const Eref& e, double v )
{
	concK1_ = v;
	stoich_->setEnzK1( e, v );
}

double ZombieEnz::vGetConcK1( const Eref& e ) const
{
	return concK1_;
}

//////////////////////////////////////////////////////////////
// Utility function
//////////////////////////////////////////////////////////////

// static func
void ZombieEnz::setSolver( Id stoich, Id enz )
{
	static const Finfo* subFinfo = Cinfo::find("Enz")->findFinfo( "subOut");
	static const Finfo* prdFinfo = Cinfo::find("Enz")->findFinfo( "prdOut");
	static const Finfo* enzFinfo = Cinfo::find("Enz")->findFinfo( "enzOut");
	static const Finfo* cplxFinfo= Cinfo::find("Enz")->findFinfo("cplxOut");

	assert( subFinfo );
	assert( prdFinfo );
	assert( enzFinfo );
	assert( cplxFinfo );
	vector< Id > enzMols;
	vector< Id > cplxMols;
	bool isOK = true;
	unsigned int numReactants;
	numReactants = enz.element()->getNeighbors( enzMols, enzFinfo );
	bool hasEnz = ( numReactants == 1 );
	vector< Id > subs;
	numReactants = enz.element()->getNeighbors( subs, subFinfo );
	bool hasSubs = ( numReactants > 0 );
	numReactants = enz.element()->getNeighbors( cplxMols, cplxFinfo );
	bool hasCplx = ( numReactants == 1 );
	vector< Id > prds;
	numReactants = enz.element()->getNeighbors( prds, prdFinfo );
	bool hasPrds = ( numReactants > 0 );
	assert( stoich.element()->cinfo()->isA( "Stoich" ) );
	stoich_ = reinterpret_cast< Stoich* >( stoich.eref().data() );

	if ( hasEnz && hasSubs && hasCplx && hasPrds ) {
		stoich_->installEnzyme( enz, enzMols[0], cplxMols[0], subs, prds );
	} else {
		stoich_->installDummyEnzyme( enz, Id() );
		string msg = "";
		if ( !hasEnz ) msg = msg + " enzyme";
		if ( !hasCplx ) msg = msg + " enzyme-substrate complex";
		if ( !hasSubs ) msg = msg + " substrates";
		if ( !hasPrds ) msg = msg + " products";
		cout << "Warning: ZombieEnz:setSolver: Dangling Enz '" <<
			enz.path() << "':\nMissing " << msg << endl;
	}
}
