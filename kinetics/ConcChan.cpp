/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "ConcChan.h"

static SrcFinfo2< double, double > *inPoolOut() {
	static SrcFinfo2< double, double > inPoolOut(
			"inPoolOut",
			"Sends out increment to molecules on inside of membrane"
			);
	return &inPoolOut;
}

static SrcFinfo2< double, double > *outPoolOut() {
	static SrcFinfo2< double, double > outPoolOut(
			"outPoolOut",
			"Sends out increment to molecules on outside of membrane"
			);
	return &outPoolOut;
}

const Cinfo* ConcChan::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< ConcChan, double > permeability(
			"permeability",
			"Permability, in units of vol/(#s) i.e., 1/(numconc.s) "
			"Flux (#/s) = permeability * N * (#out/vol_out - #in/vol_in)",
			&ConcChan::setPermeability,
			&ConcChan::getPermeability
		);
		static ValueFinfo< ConcChan, double > numChan(
			"numChan",
			"numChan is the number of molecules of the channel.",
			&ConcChan::setNumChan,
			&ConcChan::getNumChan
		);
		static ReadOnlyValueFinfo< ConcChan, double > flux(
			"flux",
			"Flux of molecules through channel, in units of #/s ",
			&ConcChan::getFlux
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< ConcChan >( &ConcChan::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< ConcChan >( &ConcChan::reinit ) );

		//////////////////////////////////////////////////////////////
		// Shared Msg Definitions
		//////////////////////////////////////////////////////////////

		static DestFinfo inPool( "inPool",
				"Handles # of molecules of pool inside membrane",
				new OpFunc1< ConcChan, double >( &ConcChan::inPool ) );
		static DestFinfo outPool( "outPool",
				"Handles # of molecules of pool outside membrane",
				new OpFunc1< ConcChan, double >( &ConcChan::outPool ) );
		static Finfo* inShared[] = {
			inPoolOut(), &inPool
		};
		static Finfo* outShared[] = {
			outPoolOut(), &outPool
		};
		static SharedFinfo in( "in",
			"Connects to pool on inside",
			inShared, sizeof( inShared ) / sizeof( const Finfo* )
		);
		static SharedFinfo out( "out",
			"Connects to pool on outside",
			outShared, sizeof( outShared ) / sizeof( const Finfo* )
		);
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);


	static Finfo* concChanFinfos[] = {
		&permeability,		// Value
		&numChan,			// Value
		&flux,				// ReadOnlyValue
		&in,				// SharedFinfo
		&out,				// SharedFinfo
		&proc,				// SharedFinfo
	};

	static string doc[] =
	{
		"Name", "ConcChan",
		"Author", "Upinder S. Bhalla, 2018, NCBS",
		"Description", "Channel for molecular flow between cellular "
		"compartments. Need not be ions, and the flux is not a current, "
		"but number of molecules/sec. ",
	};

	static Dinfo< ConcChan > dinfo;
	static Cinfo concChanCinfo (
		"ConcChan",
		Neutral::initCinfo(),
		concChanFinfos,
		sizeof( concChanFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &concChanCinfo;
}

 static const Cinfo* concChanCinfo = ConcChan::initCinfo();

//////////////////////////////////////////////////////////////
// ConcChan internal functions
//////////////////////////////////////////////////////////////


ConcChan::ConcChan( )
	: permeability_( 0.1 ), 
		n_( 0.0 ),
		flux_( 0.0 ),
		charge_( 0.0 ),
		Vm_( 0.0 )
{
	;
}

ConcChan::~ConcChan( )
{;}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////


void ConcChan::process( const Eref& e, ProcPtr p )
{
}

void ConcChan::reinit( const Eref& e, ProcPtr p )
{
}

void ConcChan::inPool( double conc )
{
}

void ConcChan::outPool( double conc )
{
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void ConcChan::setPermeability( double v )
{
	permeability_ = v;
}

double ConcChan::getPermeability() const
{
	return permeability_;
}

void ConcChan::setNumChan( double v )
{
	n_ = v;
}

double ConcChan::getNumChan() const
{
	return n_;
}

double ConcChan::getFlux() const
{
	return flux_;
}

//////////////////////////////////////////////////////////////
// Utility function
//////////////////////////////////////////////////////////////
double ConcChan::flux( double inConc, double outConc, double dt )
{
	return permeability_ * n_ * ( outConc - inConc ) * dt;
	// Later add stuff for voltage dependence.
	// Also use exp Euler
}
