/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <vector>
#include <cassert>
using namespace std;
#include "Stencil.h"

///////////////////////////////////////////////////////////////////////////
// utility funcs
///////////////////////////////////////////////////////////////////////////
void stencil1( vector< double >& f, int index, unsigned int n,
	double invSq,
	const vector< vector< double > >& S,
	const vector< double >& diffConst )
{
	const vector< double >& t0 = S[ index ];
	if ( index - static_cast< int >( n ) < 0 ) {
		const vector< double >& temp = S[ index + n ];
		for ( unsigned int i = 0; i < f.size(); ++i ) {
			f[i] += 2 * diffConst[i] * ( temp[i] - t0[i] ) * invSq;
		}
	} else if ( index + n >= S.size() ) {
		const vector< double >& temp = S[ index - n ];
		for ( unsigned int i = 0; i < f.size(); ++i ) {
			f[i] += 2 * diffConst[i] * ( temp[i] - t0[i] ) * invSq;
		}
	} else {
		const vector< double >& tminus = S[ index - n ];
		const vector< double >& tplus = S[ index + n ];
		for ( unsigned int i = 0; i < f.size(); ++i ) {
			f[i] += diffConst[i] * ( tminus[i] + tplus[i] - 2 * t0[i] ) * invSq;
		}
	}
}


/**
 * f is the flux vector, returned to caller
 * index is the meshIndex being computed
 * n is the number of mesh entries in this stencil. It is nx for the x axis
 * 		and nx*ny for the y axis.
 * offset is the stencil displacement: 1 for the x axis, nx for the y axis,
 * 		doesn't apply for the z axis as that will be done using stencil1.
 * invSq is 1/dx for the x axis, 1/dy for the y axis, and so on.
 * S is the matrix of [meshEntries][pools]
 * diffConst is the vector of [pools]
 */
void stencilN( vector< double >& f, int index, unsigned int n, int offset,
	double invSq, const vector< vector< double > >& S,
	const vector< double >& diffConst )
{
	const vector< double >& t0 = S[ index ];
	int rem = index % n;
	if ( rem < offset ) {
		const vector< double >& temp = S[ index + offset];
		for ( unsigned int i = 0; i < f.size(); ++i ) {
			f[i] += 2 * diffConst[i] * ( temp[i] - t0[i] ) * invSq;
		}
	} else if ( rem >= static_cast< int >( n ) - offset ) {
		const vector< double >& temp = S[ index - offset ];
		for ( unsigned int i = 0; i < f.size(); ++i ) {
			f[i] += 2 * diffConst[i] * ( temp[i] - t0[i] ) * invSq;
		}
	} else {
		const vector< double >& tminus = S[ index - offset ];
		const vector< double >& tplus = S[ index + offset ];
		for ( unsigned int i = 0; i < f.size(); ++i ) {
			f[i] += diffConst[i] * ( tminus[i] + tplus[i] - 2 * t0[i] ) * invSq;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// Stencil constructor
///////////////////////////////////////////////////////////////////////////

Stencil::Stencil()
{;}

Stencil::~Stencil()
{;}

///////////////////////////////////////////////////////////////////////////
// dummy stencil.
///////////////////////////////////////////////////////////////////////////
DummyStencil::DummyStencil()
{;}

DummyStencil::~DummyStencil()
{;}

void DummyStencil::addFlux( unsigned int meshIndex, vector< double >& f,
			const vector< vector< double > >& S,
			const vector< double >& diffConst ) const
{;}

///////////////////////////////////////////////////////////////////////////
// 1-D stencil.
///////////////////////////////////////////////////////////////////////////


LineStencil::LineStencil( double dx )
	: h_( dx )
{
	if ( dx <= 0 ) {
		h_ = 1;
		invHsq_ = 1;
	} else {
		invHsq_ = 1.0 / ( dx * dx );
	}
}


LineStencil::~LineStencil()
{;}

void LineStencil::addFlux( unsigned int meshIndex,
	vector< double >& f, const vector< vector< double > >& S,
	const vector< double >& diffConst ) const
{
	assert( f.size() <= S[0].size() );
	if ( S.size() < 2 )
		return;
	assert( meshIndex < S.size() );

	int index = meshIndex;
	stencil1( f, index, 1, invHsq_, S, diffConst ); // Diffusion in x
}

///////////////////////////////////////////////////////////////////////////
// 2-D stencil.
///////////////////////////////////////////////////////////////////////////

RectangleStencil::RectangleStencil( double dx, double dy, unsigned int nx )
	: dx_( dx ), dy_( dy ), nx_( nx )
{
	if ( dx <= 0 ) {
		dx_ = 1;
		invDxSq_ = 1;
	} else {
		invDxSq_ = 1.0 / ( dx * dx );
	}

	if ( dy <= 0 ) {
		dy_ = 1;
		invDySq_ = 1;
	} else {
		invDySq_ = 1.0 / ( dy * dy );
	}
}

RectangleStencil::~RectangleStencil()
{;}

// Assumes diffusion in xy plane
void RectangleStencil::addFlux( unsigned int meshIndex,
	vector< double >& f, const vector< vector< double > >& S,
	const vector< double >& diffConst ) const
{
	assert( f.size() <= S[0].size() );
	if ( S.size() < 2 * nx_ )
		return;
	assert( meshIndex < S.size() );
	int index = meshIndex;
	stencilN( f, index, nx_, 1, invDxSq_, S, diffConst ); // Diffusion in x
	stencil1( f, index, nx_, invDySq_, S, diffConst ); // Diffusion in y
}

///////////////////////////////////////////////////////////////////////////
// 3-D stencil.
///////////////////////////////////////////////////////////////////////////

CuboidStencil::CuboidStencil( double dx, double dy, double dz,
	unsigned int nx, unsigned int ny )
	: dx_( dx ), dy_( dy ), nx_( nx ), ny_( ny )
{
	if ( dx <= 0 ) {
		dx_ = 1;
		invDxSq_ = 1;
	} else {
		invDxSq_ = 1.0 / ( dx * dx );
	}

	if ( dy <= 0 ) {
		dy_ = 1;
		invDySq_ = 1;
	} else {
		invDySq_ = 1.0 / ( dy * dy );
	}

	if ( dz <= 0 ) {
		dz_ = 1;
		invDzSq_ = 1;
	} else {
		invDzSq_ = 1.0 / ( dz * dz );
	}
}


CuboidStencil::~CuboidStencil()
{;}

// Assumes diffusion in xy plane
void CuboidStencil::addFlux( unsigned int meshIndex,
	vector< double >& f, const vector< vector< double > >& S,
	const vector< double >& diffConst ) const
{
	assert( f.size() <= S[0].size() );
	if ( S.size() < 2 * nx_ )
		return;
	assert( meshIndex < S.size() );
	int index = meshIndex;
	stencilN( f, index, nx_, 1, invDxSq_, S, diffConst ); // Diffusion in x
	stencilN( f, index, nx_*ny_, nx_, invDxSq_, S, diffConst ); // Diff in y
	stencil1( f, index, nx_*ny_, invDzSq_, S, diffConst ); // Diffusion in z
}

