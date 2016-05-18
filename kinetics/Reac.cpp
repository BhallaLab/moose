/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
#include "lookupVolumeFromMesh.h"
#include "ReacBase.h"
#include "Reac.h"

#define EPSILON 1e-15
const Cinfo* Reac::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions: All inherited from ReacBase
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions: All inherited
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// Shared Msg Definitions: All inherited.
		//////////////////////////////////////////////////////////////
	static Dinfo< Reac > dinfo;
	static Cinfo reacCinfo (
		"Reac",
		ReacBase::initCinfo(),
		0,
		0,
		&dinfo
	);

	return &reacCinfo;
}

static const Cinfo* reacCinfo = Reac::initCinfo();

static const SrcFinfo2< double, double >* subOut = 
 	dynamic_cast< const SrcFinfo2< double, double >* >(
					reacCinfo->findFinfo( "subOut" ) );

static const SrcFinfo2< double, double >* prdOut = 
 	dynamic_cast< const SrcFinfo2< double, double >* >(
					reacCinfo->findFinfo( "prdOut" ) );

//////////////////////////////////////////////////////////////
// Reac internal functions
//////////////////////////////////////////////////////////////


Reac::Reac( )
		: kf_( 0.1 ), kb_( 0.2 ), sub_( 0.0 ), prd_( 0.0 )
{
	;
}

/*
Reac::Reac( double kf, double kb )
	: kf_( kf ), kb_( kb ), concKf_( 0.1 ), concKb_( 0.2 ),
		sub_( 0.0 ), prd_( 0.0 )
{
	;
}
*/

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void Reac::vSub( double v )
{
	sub_ *= v;
}

void Reac::vPrd( double v )
{
	prd_ *= v;
}

void Reac::vProcess( const Eref& e, ProcPtr p )
{
	prdOut->send( e, sub_, prd_ );
	subOut->send( e, prd_, sub_ );
	
	sub_ = kf_;
	prd_ = kb_;
}

void Reac::vReinit( const Eref& e, ProcPtr p )
{
	sub_ = kf_ = concKf_ /
		convertConcToNumRateUsingMesh( e, subOut, 0 );
	prd_ = kb_ = concKb_ / 
		convertConcToNumRateUsingMesh( e, prdOut, 0 );
}

void Reac::vRemesh( const Eref& e )
{
	kf_ = concKf_ / convertConcToNumRateUsingMesh( e, subOut, 0 );
	kb_ = concKb_ / convertConcToNumRateUsingMesh( e, prdOut, 0 );
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void Reac::vSetNumKf( const Eref& e, double v )
{
	sub_ = kf_ = v;
	double volScale = convertConcToNumRateUsingMesh( e, subOut, false );
	concKf_ = kf_ * volScale;
}

double Reac::vGetNumKf( const Eref& e) const
{
	double kf = concKf_ / convertConcToNumRateUsingMesh( e, subOut, false );
	return kf;
}

void Reac::vSetNumKb( const Eref& e, double v )
{
	prd_ = kb_ = v;
	/*
	double volScale = convertConcToNumRateUsingMesh( e, prdOut, true );
	vector< double > vols;
	getReactantVols( e, subOut, vols );
	assert( vols.size() > 0 );
	volScale /= (vols[0] * NA);
	*/

	double volScale = convertConcToNumRateUsingMesh( e, prdOut, false );
	concKb_ = kb_ * volScale;
}

double Reac::vGetNumKb( const Eref& e ) const
{
	double kb = concKb_ / convertConcToNumRateUsingMesh( e, prdOut, 0 );
	return kb;
}

void Reac::vSetConcKf( const Eref& e, double v )
{
	concKf_ = v;
	sub_ = kf_ = v / convertConcToNumRateUsingMesh( e, subOut, 0 );
}

double Reac::vGetConcKf( const Eref& e ) const
{
	return concKf_;
}

void Reac::vSetConcKb( const Eref& e, double v )
{
	concKb_ = v;
	prd_ = kb_ = v / convertConcToNumRateUsingMesh( e, prdOut, 0 );
}

double Reac::vGetConcKb( const Eref& e ) const
{
	return concKb_;
}
