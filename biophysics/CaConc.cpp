/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "CaConcBase.h"
#include "CaConc.h"


const Cinfo* CaConc::initCinfo()
{
	static string doc[] =
	{
		"Name", "CaConc \n",
		"Author", "Upinder S. Bhalla, 2014, NCBS \n",
		"Description", "CaConc: Calcium concentration pool. Takes current from a \n"
				"channel and keeps track of calcium buildup and depletion by a \n"
				"single exponential process. \n",
	};

        static Dinfo< CaConc > dinfo;

	static Cinfo CaConcCinfo(
		"CaConc",
		CaConcBase::initCinfo(),
		0,
		0,
		&dinfo,
		doc,
		sizeof(doc)/sizeof(string)
	);

	return &CaConcCinfo;
}
///////////////////////////////////////////////////

static const Cinfo* caConcCinfo = CaConc::initCinfo();

CaConc::CaConc()
	: CaConcBase(),
		Ca_( 0.0 ),
		CaBasal_( 0.0 ),
		tau_( 1.0 ),
		B_( 1.0 ),
		c_( 0.0 ),
		activation_( 0.0 ),
		ceiling_( 1.0e9 ),
		floor_( 0.0 )
{;}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void CaConc::vSetCa( const Eref& e, double Ca )
{
	Ca_ = Ca;
}
double CaConc::vGetCa( const Eref& e ) const
{
	return Ca_;
}

void CaConc::vSetCaBasal( const Eref& e, double CaBasal )
{
	CaBasal_ = CaBasal;
}
double CaConc::vGetCaBasal( const Eref& e ) const
{
	return CaBasal_;
}

void CaConc::vSetTau( const Eref& e, double tau )
{
	tau_ = tau;
}
double CaConc::vGetTau( const Eref& e ) const
{
	return tau_;
}

void CaConc::vSetB( const Eref& e, double B )
{
	B_ = B;
}
double CaConc::vGetB( const Eref& e ) const
{
	return B_;
}
void CaConc::vSetCeiling( const Eref& e, double ceiling )
{
    ceiling_ = ceiling;
}
double CaConc::vGetCeiling( const Eref& e ) const
{
	return ceiling_;
}

void CaConc::vSetFloor( const Eref& e, double floor )
{
    floor_ = floor;
}
double CaConc::vGetFloor( const Eref& e ) const
{
	return floor_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void CaConc::vReinit( const Eref& e, ProcPtr p )
{
	activation_ = 0.0;
	c_ = 0.0;
	Ca_ = CaBasal_;
	concOut()->send( e, Ca_ );
}

void CaConc::vProcess( const Eref& e, ProcPtr p )
{
	double x = exp( -p->dt / tau_ );
	Ca_ = CaBasal_ + c_ * x + ( B_ * activation_ * tau_ )  * (1.0 - x);
	if ( ceiling_ > 0.0 && Ca_ > ceiling_ ) {
		Ca_ = ceiling_;
	} else if ( Ca_ < floor_ ){
		Ca_ = floor_;
	}
	c_ = Ca_ - CaBasal_;
	concOut()->send( e, Ca_ );
	activation_ = 0;
}


void CaConc::vCurrent( const Eref& e, double I )
{
	activation_ += I;
}

void CaConc::vCurrentFraction( const Eref& e, double I, double fraction )
{
	activation_ += I * fraction;
}

void CaConc::vIncrease( const Eref& e, double I )
{
	activation_ += fabs( I );
}

void CaConc::vDecrease( const Eref& e, double I )
{
	activation_ -= fabs( I );
}

///////////////////////////////////////////////////
// Unit tests
///////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS
void testCaConc()
{
    /*
	CaConc cc;
	double tau = 0.10;
	double basal = 0.0001;

	cc.setCa( basal );
	cc.setCaBasal( basal );
	cc.setTau( tau );
	// Here we use a volume of 1e-15 m^3, i.e., a 10 micron cube.
	cc.setB( 5.2e-6 / 1e-15 );
	// Faraday constant = 96485.3415 s A / mol
	// Use a 1 pA input current. This should give (0.5e-12/F) moles/sec
	// influx, because Ca has valence of 2.
	// So we get 5.2e-18 moles/sec coming in.
	// Our volume is 1e-15 m^3
	// So our buildup should be at 5.2e-3 moles/m^3/sec = 5.2 uM/sec
	double curr = 1e-12;
	// This will settle when efflux = influx
	// dC/dt = B*Ik - C/tau = 0.
	// so Ca = CaBasal + tau * B * Ik =
	// 0.0001 + 0.1 * 5.2e-6 * 1e3 = 0.000626

	ProcInfo p;
	p.dt = 0.001;
	p.currTime = 0.0;
        Eref sheller(Id().eref());
        Shell * shell = reinterpret_cast<Shell*> (sheller.data());
        Id temp = shell->doCreate("CaConc", Id(), "caconc", 1);
        assert(temp.element()->getName() == "caconc");
	// Id tempId = Id::nextId();
	// Element temp( tempId, CaConc::initCinfo(), "temp", 0 );
	Eref er( &temp, 0 );
	cc.reinit( er, &p );

	double y;
	double conc;
	double delta = 0.0;
	for ( p.currTime = 0.0; p.currTime < 0.5; p.currTime += p.dt )
	{
		cc.current( curr );
		cc.process( er, &p );
		y = basal + 526.0e-6 * ( 1.0 - exp( -p.currTime / tau ) );
		conc = cc.getCa();
		delta += ( y - conc ) * ( y - conc );
	}
	assert( delta < 1e-6 );
	cout << "." << flush;
    */
}
#endif
