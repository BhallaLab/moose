/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <cmath>
#include "../basecode/header.h"
#include "../biophysics/SpikeGen.h"
#include "HSolveStruct.h"

void ChannelStruct::setPowers(
	double Xpower,
	double Ypower,
	double Zpower )
{
	Xpower_ = Xpower;
	takeXpower_ = selectPower( Xpower );

	Ypower_ = Ypower;
	takeYpower_ = selectPower( Ypower );

	Zpower_ = Zpower;
	takeZpower_ = selectPower( Zpower );
}

double ChannelStruct::powerN( double x, double p )
{
	if ( x > 0.0 )
		return exp( p * log( x ) );
	return 0.0;
}

PFDD ChannelStruct::selectPower( double power )
{
	if ( power == 0.0 )
		return powerN;
	else if ( power == 1.0 )
		return power1;
	else if ( power == 2.0 )
		return power2;
	else if ( power == 3.0 )
		return power3;
	else if ( power == 4.0 )
		return power4;
	else
		return powerN;
}

void ChannelStruct::process( double*& state, CurrentStruct& current )
{
	double fraction = modulation_;

	if( Xpower_ > 0.0 )
		fraction *= takeXpower_( *( state++ ), Xpower_ );
	if( Ypower_ > 0.0 )
		fraction *= takeYpower_( *( state++ ), Ypower_ );
	if( Zpower_ > 0.0 )
		fraction *= takeZpower_( *( state++ ), Zpower_ );

	current.Gk = Gbar_ * fraction;
}

void SpikeGenStruct::reinit( ProcPtr info  )
{
	SpikeGen* spike = reinterpret_cast< SpikeGen* >( e_.data() );

	spike->reinit( e_, info );
}

void SpikeGenStruct::send( ProcPtr info  )
{
	SpikeGen* spike = reinterpret_cast< SpikeGen* >( e_.data() );

	spike->handleVm( *Vm_ );
	spike->process( e_, info );
}

CaConcStruct::CaConcStruct()
	:
		c_( 0.0 ),
		CaBasal_( 0.0 ),
		factor1_( 0.0 ),
		factor2_( 0.0 ),
		ceiling_( 0.0 ),
		floor_( 0.0 )
{ ; }

CaConcStruct::CaConcStruct(
	double Ca,
	double CaBasal,
	double tau,
	double B,
	double ceiling,
	double floor,
	double dt )
{
	setCa( Ca );
	setCaBasal( CaBasal );
	setTauB( tau, B, dt );
	ceiling_ = ceiling;
	floor_ = floor;
}

void CaConcStruct::setCa( double Ca ) {
	c_ = Ca - CaBasal_;
}

void CaConcStruct::setCaBasal( double CaBasal ) {
	/*
	 * Also updating 'c_' here, so that only 'CaBasal' changes, and 'Ca'
	 * remains the same. This is good because otherwise one has to bother about
	 * the order in which 'setCa()' and 'setCaBasal()' are called.
	 *
	 * 'Ca' is:
	 *         Ca = CaBasal_ + c_
	 *
	 * if:
	 *         Ca_new = Ca_old
	 *
	 * then:
	 *         CaBasal_new + c_new = CaBasal_old + c_old
	 *
	 * so:
	 *         c_new = c_old + CaBasal_old - CaBasal_new
	 */
	c_ += CaBasal_ - CaBasal;
	CaBasal_ = CaBasal;
}

void CaConcStruct::setTauB( double tau, double B, double dt ) {
	factor1_ = 4.0 / ( 2.0 + dt / tau ) - 1.0;
	factor2_ = 2.0 * B * dt / ( 2.0 + dt / tau );
}

double CaConcStruct::process( double activation ) {
	c_ = factor1_ * c_ + factor2_ * activation;

	double ca = CaBasal_ + c_;

	if ( ceiling_ > 0 && ca > ceiling_ ) {
		ca = ceiling_;
		setCa( ca );
	}

	if ( ca < floor_ ) {
		ca = floor_;
		setCa( ca );
	}

	return ca;
}
