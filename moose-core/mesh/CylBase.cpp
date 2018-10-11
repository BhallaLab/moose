/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/SparseMatrix.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "VoxelJunction.h"
#include "ChemCompt.h"
#include "MeshCompt.h"
#include "CubeMesh.h"
#include "CylBase.h"
#include "../utility/Vec.h"

extern const double PI; // defined in consts.cpp

CylBase::CylBase( double x, double y, double z,
					double dia, double length, unsigned int numDivs )
	:
		x_( x ),
		y_( y ),
		z_( z ),
		dia_( dia ),
		length_( length ),
		numDivs_( numDivs ),
		isCylinder_( false )
{
	;
}

CylBase::CylBase()
	:
		x_( 0.0 ),
		y_( 0.0 ),
		z_( 0.0 ),
		dia_( 1.0 ),
		length_( 1.0 ),
		numDivs_( 1.0 ),
		isCylinder_( false )
{
	;
}

void CylBase::setX( double v )
{
	x_ = v;
}

double CylBase::getX() const
{
	return x_;
}

void CylBase::setY( double v )
{
	y_ = v;
}

double CylBase::getY() const
{
	return y_;
}

void CylBase::setZ( double v )
{
	z_ = v;
}

double CylBase::getZ() const
{
	return z_;
}

void CylBase::setDia( double v )
{
	dia_ = v;
}

double CylBase::getDia() const
{
	return dia_;
}

void CylBase::setLength( double v )
{
	length_ = v;
}

double CylBase::getLength() const
{
	return length_;
}

void CylBase::setNumDivs( unsigned int v )
{
	numDivs_ = v;
}

unsigned int CylBase::getNumDivs() const
{
	return numDivs_;
}

void CylBase::setIsCylinder( bool v )
{
	isCylinder_ = v;
}

bool CylBase::getIsCylinder() const
{
	return isCylinder_;
}
//////////////////////////////////////////////////////////////////
// FieldElement assignment stuff for MeshEntries
//////////////////////////////////////////////////////////////////

/**
 * The entire volume for a truncated cone is given by:
 * V = 1/3 pi.length.(r0^2 + r0.r1 + r1^2)
 * where the length is the length of the cone
 * r0 is radius at base
 * r1 is radius at top.
 * Note that this converges to volume of a cone if r0 or r1 is zero, and
 * to the volume of a cylinder if r0 == r1.
 */
double CylBase::volume( const CylBase& parent ) const
{
	if ( isCylinder_ )
			return length_ * dia_ * dia_ * PI / 4.0;
	double r0 = parent.dia_/2.0;
	double r1 = dia_/2.0;
	return length_ * ( r0*r0 + r0 *r1 + r1 * r1 ) * PI / 3.0;
}

/**
 * Returns volume of MeshEntry.
 * This isn't the best subdivision of the cylinder from the viewpoint of
 * keeping the length constants all the same for different volumes.
 * Ideally the thinner segments should have a smaller length.
 * But this is simple and so is the diffusion calculation, so leave it.
 * Easy to fine-tune later by modifying how one computes frac0 and frac1.
 */
double CylBase::voxelVolume( const CylBase& parent, unsigned int fid ) const
{
	assert( numDivs_ > fid );
	if ( isCylinder_ )
			return length_ * dia_ * dia_ * PI / ( 4.0 * numDivs_ );

 	double frac0 = ( static_cast< double >( fid ) ) /
				static_cast< double >( numDivs_ );
 	double frac1 = ( static_cast< double >( fid + 1 ) ) /
				static_cast< double >( numDivs_ );
	double r0 = 0.5 * ( parent.dia_ * ( 1.0 - frac0 ) + dia_ * frac0 );
	double r1 = 0.5 * ( parent.dia_ * ( 1.0 - frac1 ) + dia_ * frac1 );
	double s0 = length_ * frac0;
	double s1 = length_ * frac1;

	return (s1 - s0) * ( r0*r0 + r0 *r1 + r1 * r1 ) * PI / 3.0;
}

/// Virtual function to return coords of mesh Entry.
/// For Cylindrical mesh, coords are x1y1z1 x2y2z2 r0 r1 phi0 phi1
vector< double > CylBase::getCoordinates(
					const CylBase& parent, unsigned int fid ) const
{
	assert( numDivs_ > fid );
 	double frac0 = ( static_cast< double >( fid ) ) /
				static_cast< double >( numDivs_ );
 	double frac1 = ( static_cast< double >( fid + 1 ) ) /
				static_cast< double >( numDivs_ );

	double r0 = 0.5 * ( parent.dia_ * ( 1.0 - frac0 ) + dia_ * frac0 );
	// equivalent: double r0 = parent.dia_ + frac0 * ( dia_ - parent.dia_ );
	double r1 = 0.5 * ( parent.dia_ * ( 1.0 - frac1 ) + dia_ * frac1 );

	vector< double > ret( 10, 0.0 );
	ret[0] = parent.x_ + frac0 * ( x_ - parent.x_ );
	ret[1] = parent.y_ + frac0 * ( y_ - parent.y_ );
	ret[2] = parent.z_ + frac0 * ( z_ - parent.z_ );
	ret[3] = parent.x_ + frac1 * ( x_ - parent.x_ );
	ret[4] = parent.y_ + frac1 * ( y_ - parent.y_ );
	ret[5] = parent.z_ + frac1 * ( z_ - parent.z_ );
	ret[6] = r0;
	ret[7] = r1;
	ret[8] = 0;
	ret[9] = 0;

	return ret;
}

/**
 * Returns diffusion cross-section from specified index to previous.
 * For index 0, this is cross-section area of parent.
 * For index numDivs-1, it is the cross-section from the second-last to
 * the last voxel in this CylBase.
 * For index numDivs it is the area of this CylBase.
 * Thus there is no valid value for (index > numDivs), it has
 * to be computed external to the CylBase, typically by calling the
 * getDiffusionArea for the child CylBase.
 */
double CylBase::getDiffusionArea(
				const CylBase& parent, unsigned int fid ) const
{
	assert( fid < numDivs_ + 1 );
	if ( isCylinder_ )
			return PI * dia_ * dia_ / 4.0;
 	double frac0 = ( static_cast< double >( fid ) ) /
				static_cast< double >( numDivs_ );
	double r0 = 0.5 * ( parent.dia_ * ( 1.0 - frac0 ) + dia_ * frac0 );
	return PI * r0 * r0;
}

/// Return the cross section area of the middle of the specified voxel.
double CylBase::getMiddleArea(
				const CylBase& parent, unsigned int fid ) const
{
	assert( fid < numDivs_ );
	if ( isCylinder_ )
			return PI * dia_ * dia_ / 4.0;
 	double frac0 = ( 0.5 + static_cast< double >( fid ) ) /
				static_cast< double >( numDivs_ );
	double r0 = 0.5 * ( parent.dia_ * ( 1.0 - frac0 ) + dia_ * frac0 );
	return PI * r0 * r0;
}

double CylBase::getVoxelLength() const
{
	return length_ / numDivs_;
}


// Select grid size. Ideally the meshes should be comparable.
double CylBase::selectGridSize( double h, double dia1,
					double granularity ) const
{
	if ( length_ < 1e-7 && numDivs_ == 1 ) { // It is a disc, not a cylinder
		return granularity * dia_ / 2.0;
	}

	double lambda = length_ / numDivs_;
	if ( h > lambda )
		h = lambda;
	if ( h > dia_ / 2.0 )
		h = dia_ / 2.0;
	if ( h > dia1/2.0 )
		h = dia1/2.0;
	h *= granularity;
	unsigned int num = ceil( lambda / h );
	h = lambda / num;

	return h;
}

static void fillPointsOnCircle(
				const Vec& u, const Vec& v, const Vec& q,
				double h, double r, vector< double >& area,
				const CubeMesh* other
				)
{
	// fine-tune the h spacing so it is integral around circle.
	// This will cause small errors in area estimate but they will
	// be anisotropic. The alternative will have large errors toward
	// 360 degrees, but not elsewhere.
	unsigned int numAngle = floor( 2.0 * PI * r / h + 0.5 );
	assert( numAngle > 0 );
	double dtheta = 2.0 * PI / numAngle;
	double dArea = h * dtheta * r;
	// March along points on surface of circle centred at q.
	for ( unsigned int j = 0; j < numAngle; ++j ) {
		double theta = j * dtheta;
		double c = cos( theta );
		double s = sin( theta );
		double p0 = q.a0() + r * ( u.a0() * c + v.a0() * s );
		double p1 = q.a1() + r * ( u.a1() * c + v.a1() * s );
		double p2 = q.a2() + r * ( u.a2() * c + v.a2() * s );
		unsigned int index = other->spaceToIndex( p0, p1, p2 );
		if ( index != CubeMesh::EMPTY )
			area[index] += dArea;
	}
}

static void fillPointsOnDisc(
				const Vec& u, const Vec& v, const Vec& q,
				double h, double r, vector< double >& area,
				const CubeMesh* other
				)
{
	unsigned int numRadial = floor( r / h + 0.5 );
	double dRadial = r / numRadial;
	for ( unsigned int i = 0; i < numRadial; ++i ) {
		double a = ( i + 0.5 ) * dRadial;
		unsigned int numAngle = floor( 2.0 * PI * a / h + 0.5 );
		if ( i == 0 )
			numAngle = 1;
		double dtheta = 2.0 * PI / numAngle;
		double dArea = dRadial * dtheta * a;
		for ( unsigned int j = 0; j < numAngle; ++j ) {
			double theta = j * dtheta;
			double c = cos( theta );
			double s = sin( theta );
			double p0 = q.a0() + a * ( u.a0() * c + v.a0() * s );
			double p1 = q.a1() + a * ( u.a1() * c + v.a1() * s );
			double p2 = q.a2() + a * ( u.a2() * c + v.a2() * s );
			unsigned int index = other->spaceToIndex( p0, p1, p2 );
			if ( index != CubeMesh::EMPTY )
				area[index] += dArea;
		}
	}
}

void CylBase::matchCubeMeshEntries( const ChemCompt* compt,
	const CylBase& parent,
	unsigned int startIndex,
	double granularity,
	vector< VoxelJunction >& ret,
	bool useCylinderCurve, bool useCylinderCap ) const
{
	const CubeMesh* other = dynamic_cast< const CubeMesh* >( compt );
	assert( other );
	const double EPSILON = 1e-18;
	Vec a( parent.x_ - x_, parent.y_ - y_, parent.z_ - z_ );
	Vec u;
	Vec v;
	a.orthogonalAxes( u, v );

	double h = selectGridSize( other->getDx(), parent.dia_/2, granularity );
	double lambda = length_ / numDivs_;

	unsigned int num = floor( 0.1 + lambda / h );
	// March along axis of cylinder.
	// q is the location of the point along axis.
	double rSlope = ( dia_ - parent.dia_ ) * 0.5 / length_;
	for ( unsigned int i = 0; i < numDivs_; ++i ) {
		vector< double >area( other->getNumEntries(), 0.0 );
		if ( useCylinderCurve ) {
			for ( unsigned int j = 0; j < num; ++j ) {
				unsigned int m = i * num + j;
				double frac = ( m * h + h/2.0 ) / length_;
				double q0 = x_ + a.a0() * frac;
				double q1 = y_ + a.a1() * frac;
				double q2 = z_ + a.a2() * frac;
				// get radius of cylinder at this point.
				double r = dia_/2.0;
				if ( !isCylinder_ ) // Use the more complicated conic value
				r = parent.dia_/2.0 + frac * rSlope;
				fillPointsOnCircle( u, v, Vec( q0, q1, q2 ),
							h, r, area, other );
			}
		}
		if ( useCylinderCap && i == numDivs_ - 1 ) {
			fillPointsOnDisc( u, v, Vec( x_, y_, z_ ),
							h, dia_/2.0, area, other );
		}
		// Go through all cubeMesh entries and compute diffusion
		// cross-section. Assume this is through a membrane, so the
		// only factor relevant is area. Not the distance.
		for ( unsigned int k = 0; k < area.size(); ++k ) {
			if ( area[k] > EPSILON ) {
				ret.push_back( VoxelJunction( i + startIndex, k, area[k] ));
			}
		}
	}
}

// this is the function that does the actual calculation.
// Returns radial distance from self to axis formed from parent to self.
// also sends back linePos, the fraction along the line, and r, the
// radius of the parent tapering cylinder at the point of linePos.
double CylBase::nearest( double x, double y, double z,
				const CylBase& parent,
				double& linePos, double& r ) const
{
	const double EPSILON = 1e-8; // 10 nm is too small for any compt.
	// Consider a = parent and b = self, and c = x,y,z.
	// Fraction along cylinder axis ab = k
	// k = (c - a).(b - a)/((b-a).(b-a))
	//
	// Then, point p along line from parent to self is
	// p = k( self-parent) + parent.
	//
	// So distance from c to p is what we want.
	//
	// If k is
	//
	Vec a( parent.x_, parent.y_, parent.z_ );
	Vec b( x_, y_, z_ );
	Vec c( x, y, z );

	double dist = b.distance( a );
	assert( dist > EPSILON );
	double k = ( b - a ).dotProduct( c - a );
	k /= dist * dist;
	Vec pt = a.pointOnLine( b, k );

	double ret = c.distance(pt);
	linePos = k;
	// For now I disable the positioning with a tapering cylinder.
	// double rSlope = 0.5 * ( dia_ - parent.dia_ ) / numDivs_;
	double rSlope = 0;
	r = dia_/2.0 + k * numDivs_ * rSlope;
	return ret;
}

// This function returns the index.
double CylBase::nearest( double x, double y, double z,
				const CylBase& parent,
				unsigned int& index ) const
{
	double k = 0.0;
	double r;
	double ret = nearest( x, y, z, parent, k, r );
	if ( k < 0.0 ) {
		ret = -ret;
		index = 0;
	} else if ( k > 1.0 ) {
		ret = -ret;
		index = numDivs_ - 1;
	} else { // Inside length of cylinder, now is it inside radius?
		index = k * numDivs_;
		if ( index >= numDivs_ )
			index = numDivs_ - 1;
		// double ri = r0_ + (index + 0.5) * rSlope_;
		if ( ret > r * 1.01 ) // Handle roundoff
			ret = -ret;
	}
	return ret;
}

