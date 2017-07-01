/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include <math.h>
#include <cassert>
#include "Vec.h"
#include "../basecode/doubleEq.h"

Vec::Vec( double a0, double a1, double a2 )
				: a0_( a0 ), a1_( a1 ), a2_( a2 )
{;}

double Vec::length() const {
			return sqrt( a0_*a0_ + a1_*a1_ + a2_*a2_ );
}

double Vec::dotProduct( const Vec& other ) const {
			return a0_ * other.a0_ + a1_ * other.a1_ + a2_ * other.a2_;
}

Vec Vec::crossProduct( const Vec& other ) const {
	double c0 = a1_ * other.a2_ - a2_ * other.a1_;
	double c1 = a2_ * other.a0_ - a0_ * other.a2_;
	double c2 = a0_ * other.a1_ - a1_ * other.a0_;
	return Vec( c0, c1, c2 );
}

void Vec::unitLength() {
	double len = length();
	assert( len > 0.0 );
	a0_ /= len;
	a1_ /= len;
	a2_ /= len;
}

void Vec::orthogonalAxes( Vec& u, Vec& v ) const {
	assert( !doubleEq( length(), 0 ) );
	if ( doubleApprox( a1_, 0 ) && doubleApprox( a2_, 0 ) ) {
		u = crossProduct( Vec( a0_, a1_, a2_ - a0_ ) );
	} else {
		u = crossProduct( Vec( a0_ + a1_ + a2_, a1_, a2_ ) );
	}
	v = crossProduct( u );
	u.unitLength();
	v.unitLength();
}

Vec Vec::pointOnLine( const Vec& end, double k )
{
	return Vec(
					a0_ + k * ( end.a0_ - a0_ ),
					a1_ + k * ( end.a1_ - a1_ ),
					a2_ + k * ( end.a2_ - a2_ ) );
}

bool Vec::operator==( const Vec& other ) const {
	return
		doubleEq( a0_, other.a0_) &&
		doubleEq( a1_, other.a1_) &&
		doubleEq( a2_, other.a2_);
}

Vec Vec::operator-( const Vec& other ) const {
	return Vec( a0_ - other.a0_, a1_ - other.a1_, a2_ - other.a2_ );
}

Vec Vec::operator+( const Vec& other ) const {
	return Vec( a0_ + other.a0_, a1_ + other.a1_, a2_ + other.a2_ );
}

Vec Vec::operator*( const double& other ) const {
	return Vec( a0_ * other, a1_ * other, a2_ * other );
}


double Vec::distance( const Vec& other ) const
{
	return ( *this - other ).length();
}
