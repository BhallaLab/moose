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
#include "lookupVolumeFromMesh.h"
#include "ReacBase.h"

#define EPSILON 1e-15

static SrcFinfo2< double, double > *subOut() {
	static SrcFinfo2< double, double > subOut(
			"subOut",
			"Sends out increment of molecules on product each timestep"
			);
	return &subOut;
}

static SrcFinfo2< double, double > *prdOut() {
	static SrcFinfo2< double, double > prdOut(
			"prdOut",
			"Sends out increment of molecules on product each timestep"
			);
	return &prdOut;
}

const Cinfo* ReacBase::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ElementValueFinfo< ReacBase, double > kf(
			"numKf",
			"Forward rate constant, in # units",
			&ReacBase::setNumKf,
			&ReacBase::getNumKf
		);

		static ElementValueFinfo< ReacBase, double > kb(
			"numKb",
			"Reverse rate constant, in # units",
			&ReacBase::setNumKb,
			&ReacBase::getNumKb
		);

		static ElementValueFinfo< ReacBase, double > Kf(
			"Kf",
			"Forward rate constant, in concentration units",
			&ReacBase::setConcKf,
			&ReacBase::getConcKf
		);

		static ElementValueFinfo< ReacBase, double > Kb(
			"Kb",
			"Reverse rate constant, in concentration units",
			&ReacBase::setConcKb,
			&ReacBase::getConcKb
		);

		static ReadOnlyElementValueFinfo< ReacBase, unsigned int > numSub(
			"numSubstrates",
			"Number of substrates of reaction",
			&ReacBase::getNumSub
		);

		static ReadOnlyElementValueFinfo< ReacBase, unsigned int > numPrd(
			"numProducts",
			"Number of products of reaction",
			&ReacBase::getNumPrd
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< ReacBase >( &ReacBase::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< ReacBase >( &ReacBase::reinit ) );

		//////////////////////////////////////////////////////////////
		// Shared Msg Definitions
		//////////////////////////////////////////////////////////////

		static DestFinfo subDest( "subDest",
				"Handles # of molecules of substrate",
				new OpFunc1< ReacBase, double >( &ReacBase::sub ) );
		static DestFinfo prdDest( "prdDest",
				"Handles # of molecules of product",
				new OpFunc1< ReacBase, double >( &ReacBase::prd ) );
		static Finfo* subShared[] = {
			subOut(), &subDest
		};
		static Finfo* prdShared[] = {
			prdOut(), &prdDest
		};
		static SharedFinfo sub( "sub",
			"Connects to substrate pool",
			subShared, sizeof( subShared ) / sizeof( const Finfo* )
		);
		static SharedFinfo prd( "prd",
			"Connects to substrate pool",
			prdShared, sizeof( prdShared ) / sizeof( const Finfo* )
		);
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);


	static Finfo* reacFinfos[] = {
		&kf,	// Value
		&kb,	// Value
		&Kf,	// Value
		&Kb,	// Value
		&numSub,	// ReadOnlyValue
		&numPrd,	// ReadOnlyValue
		&sub,				// SharedFinfo
		&prd,				// SharedFinfo
		&proc,				// SharedFinfo
	};

	static string doc[] =
	{
		"Name", "ReacBase",
		"Author", "Upinder S. Bhalla, 2012, NCBS",
		"Description", "Base class for reactions. Provides the MOOSE API"
		"functions, but ruthlessly refers almost all of them to derived"
		"classes, which have to provide the man page output."
	};

	static ZeroSizeDinfo< int> dinfo;
	static Cinfo reacBaseCinfo (
		"ReacBase",
		Neutral::initCinfo(),
		reacFinfos,
		sizeof( reacFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string ),
		true // Ban creation, this is an abstract base class.
	);

	return &reacBaseCinfo;
}

 static const Cinfo* reacBaseCinfo = ReacBase::initCinfo();

//////////////////////////////////////////////////////////////
// ReacBase internal functions
//////////////////////////////////////////////////////////////


ReacBase::ReacBase( )
	: concKf_( 0.1 ), concKb_( 0.2 )
{
	;
}

ReacBase::~ReacBase( )
{;}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void ReacBase::sub( double v )
{
	vSub( v );
}

void ReacBase::prd( double v )
{
	vPrd( v );
}

void ReacBase::process( const Eref& e, ProcPtr p )
{
	vProcess( e, p );
}

void ReacBase::reinit( const Eref& e, ProcPtr p )
{
	vReinit( e, p );
}

void ReacBase::remesh( const Eref& e )
{
	vRemesh( e );
}
//////////////////////////////////////////////////////////////
//
// Virtual MsgDest Definitions, all dummies, but many derived classes
// will want to use these dummies.
//////////////////////////////////////////////////////////////

void ReacBase::vSub( double v )
{
		;
}

void ReacBase::vPrd( double v )
{
		;
}

void ReacBase::vProcess( const Eref& e, ProcPtr p )
{
		;
}

void ReacBase::vReinit( const Eref& e, ProcPtr p )
{
		;
}

void ReacBase::vRemesh( const Eref& e )
{
		;
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void ReacBase::setNumKf( const Eref& e, double v )
{
	vSetNumKf( e, v );
}

double ReacBase::getNumKf( const Eref& e) const
{
	return vGetNumKf( e );
}

void ReacBase::setNumKb( const Eref& e, double v )
{
	vSetNumKb( e, v );
}

double ReacBase::getNumKb( const Eref& e ) const
{
	return vGetNumKb( e );
}

////////////////////////////////////////////////////////////////////////

void ReacBase::setConcKf( const Eref& e, double v )
{
	vSetConcKf( e, v );
}

double ReacBase::getConcKf( const Eref& e ) const
{
	return vGetConcKf( e );
}

void ReacBase::setConcKb( const Eref& e, double v )
{
	vSetConcKb( e, v );
}

double ReacBase::getConcKb( const Eref& e ) const
{
	return vGetConcKb( e );
}

unsigned int ReacBase::getNumSub( const Eref& e ) const
{
	const vector< MsgFuncBinding >* mfb =
		e.element()->getMsgAndFunc( subOut()->getBindIndex() );
	assert( mfb );
	return ( mfb->size() );
}

unsigned int ReacBase::getNumPrd( const Eref& e ) const
{
	const vector< MsgFuncBinding >* mfb =
		e.element()->getMsgAndFunc( prdOut()->getBindIndex() );
	assert( mfb );
	return ( mfb->size() );
}

////////////////////////////////////////////////////////////////////////
// Zombie conversion routine to convert between Reac subclasses.
////////////////////////////////////////////////////////////////////////
// static func

void ReacBase::zombify( Element* orig, const Cinfo* zClass, Id solver )
{
	if ( orig->cinfo() == zClass )
		return;
	unsigned int start = orig->localDataStart();
	unsigned int num = orig->numLocalData();
	if ( num == 0 )
			return;
	vector< double > concKf( num, 0.0 );
	vector< double > concKb( num, 0.0 );
	for ( unsigned int i = 0; i < num; ++i ) {
		Eref er( orig, i + start );
		const ReacBase* rb =
			reinterpret_cast< const ReacBase* >( er.data() );
		concKf[ i ] = rb->getConcKf( er );
		concKb[ i ] = rb->getConcKb( er );
	}
	orig->zombieSwap( zClass );
	for ( unsigned int i = 0; i < num; ++i ) {
		Eref er( orig, i + start );
		ReacBase* rb = reinterpret_cast< ReacBase* >( er.data() );
		rb->setSolver( solver, orig->id() );
		rb->setConcKf( er, concKf[i] );
		rb->setConcKb( er, concKb[i] );
	}
}

// Virtual func: default does nothing.
void ReacBase::setSolver( Id solver, Id orig )
{
	;
}

