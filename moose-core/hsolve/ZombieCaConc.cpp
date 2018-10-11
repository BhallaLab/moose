/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../biophysics/CaConcBase.h"

#include "HinesMatrix.h"
#include "HSolveStruct.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"
#include "HSolve.h"
#include "ZombieCaConc.h"

const Cinfo* ZombieCaConc::initCinfo()
{
	static string doc[] =
	{
		"Name", "ZombieCaConc",
		"Author", "Upinder S. Bhalla, 2007, NCBS",
		"Description", "ZombieCaConc: Calcium concentration pool. Takes current from a "
				"channel and keeps track of calcium buildup and depletion by a "
				"single exponential process. ",
	};
	static Dinfo< ZombieCaConc > dinfo;
	static Cinfo zombieCaConcCinfo(
		"ZombieCaConc",
		CaConcBase::initCinfo(),
		0,
		0,
		&dinfo,
		doc,
		sizeof( doc )/ sizeof( string )
	);

	return &zombieCaConcCinfo;
}
///////////////////////////////////////////////////

static const Cinfo* zombieCaConcCinfo = ZombieCaConc::initCinfo();

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void ZombieCaConc::vSetCa( const Eref& e, double Ca )
{
	hsolve_->setCa( e.id(), Ca );
}

double ZombieCaConc::vGetCa( const Eref& e ) const
{
	return hsolve_->getCa( e.id() );
}

void ZombieCaConc::vSetCaBasal( const Eref& e , double CaBasal )
{
	hsolve_->setCa( e.id(), CaBasal );
}

double ZombieCaConc::vGetCaBasal( const Eref& e ) const
{
	return hsolve_->getCaBasal( e.id() );
}

void ZombieCaConc::vSetTau( const Eref& e , double tau )
{
	tau_ = tau;
	hsolve_->setTauB( e.id(), tau_, B_ );
}

double ZombieCaConc::vGetTau( const Eref& e ) const
{
	return tau_;
}

void ZombieCaConc::vSetB( const Eref& e , double B )
{
	B_ = B;
	hsolve_->setTauB( e.id(), tau_, B_ );
}

double ZombieCaConc::vGetB( const Eref& e ) const
{
	return B_;
}

void ZombieCaConc::vSetCeiling( const Eref& e , double ceiling )
{
	hsolve_->setCaCeiling( e.id(), ceiling );
}

double ZombieCaConc::vGetCeiling( const Eref& e ) const
{
	return hsolve_->getCaCeiling( e.id() );
}

void ZombieCaConc::vSetFloor( const Eref& e , double floor )
{
	hsolve_->setCaFloor( e.id(), floor );
}

double ZombieCaConc::vGetFloor( const Eref& e ) const
{
	return hsolve_->getCaFloor( e.id() );
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void ZombieCaConc::vReinit( const Eref& e, ProcPtr p )
{
	;
}

void ZombieCaConc::vProcess( const Eref& e, ProcPtr p )
{
	;
}

void ZombieCaConc::vCurrent( const Eref& e, double I )
{
	//~ activation_ += I;
	hsolve_->iCa( e.id(), I );
}

void ZombieCaConc::vCurrentFraction( const Eref& e, double I, double fraction )
{
	//~ activation_ += I * fraction;
}

void ZombieCaConc::vIncrease( const Eref& e, double I )
{
	//~ activation_ += fabs( I );
}

void ZombieCaConc::vDecrease( const Eref& e, double I )
{
	//~ activation_ -= fabs( I );
}

///////////////////////////////////////////////////
void ZombieCaConc::vSetSolver( const Eref& e, Id hsolve )
{
	if ( !hsolve.element()->cinfo()->isA( "HSolve" ) ) {
		cout << "Error: ZombieCaConc::vSetSolver: Object: " <<
				hsolve.path() << " is not an HSolve. Aborted\n";
		hsolve_ = 0;
		return;
	}
	hsolve_ = reinterpret_cast< HSolve* >( hsolve.eref().data() );
}
