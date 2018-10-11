/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "VoxelJunction.h"
#include "ChemCompt.h"
#include "../utility/Vec.h"
#include "CylBase.h"
#include "SpineEntry.h"

/**
 * Helper class for the SpineMesh. Defines the geometry of the spine
 * and provides the innter function for mapping to CubeMeshes.
 */

SpineEntry::SpineEntry( Id shaft, Id head, unsigned int parent )
		:
				parent_( parent ),
				shaftId_( shaft ),
				headId_( head )
{
	double dia = Field< double >::get( shaft, "diameter" );
	double length = Field< double >::get( shaft, "length" );
	double x0 = Field< double >::get( shaft, "x0" );
	double y0 = Field< double >::get( shaft, "y0" );
	double z0 = Field< double >::get( shaft, "z0" );
	double x1 = Field< double >::get( shaft, "x" );
	double y1 = Field< double >::get( shaft, "y" );
	double z1 = Field< double >::get( shaft, "z" );
	double x2 = Field< double >::get( head, "x" );
	double y2 = Field< double >::get( head, "y" );
	double z2 = Field< double >::get( head, "z" );
	double hdia = Field< double >::get( head, "diameter" );
	double hlength = Field< double >::get( head, "length" );

	// cout << "SpineEntry: " << parent_ << " " << x0 << " " << x1 << " " << x2 << " : " << y0 << " " << y1<< " " << y2 << " : " << z0 << " " << z1 << " " << z2 << " " << hdia << endl;

	root_.setX( x0 );
	root_.setY( y0 );
	root_.setZ( z0 );
	shaft_.setX( x1 );
	shaft_.setY( y1 );
	shaft_.setZ( z1 );
	head_.setX( x2 );
	head_.setY( y2 );
	head_.setZ( z2 );

	root_.setDia( dia );
	root_.setLength( length );
	root_.setNumDivs( 0 );
	root_.setIsCylinder( true );
	shaft_.setDia( dia );
	shaft_.setLength( length );
	shaft_.setNumDivs( 1 );
	shaft_.setIsCylinder( true );
	head_.setDia( hdia );
	head_.setLength( hlength );
	head_.setNumDivs( 1 );
	head_.setIsCylinder( true );
}


SpineEntry::SpineEntry()
		:
				parent_( 0 ),
				shaftId_( Id() ),
				headId_( Id() )
{
	const double defaultLength = 1e-6;
	root_.setX( 0 );
	root_.setY( 0 );
	root_.setZ( 0 );
	shaft_.setX( defaultLength );
	shaft_.setY( 0 );
	shaft_.setZ( 0 );
	head_.setX( defaultLength * 2 );
	head_.setY( 0 );
	head_.setZ( 0 );



	root_.setDia( defaultLength );
	root_.setLength( defaultLength );
	root_.setNumDivs( 0 );
	root_.setIsCylinder( true );
	shaft_.setDia( defaultLength );
	shaft_.setLength( defaultLength );
	shaft_.setNumDivs( 1 );
	shaft_.setIsCylinder( true );
	head_.setDia( defaultLength );
	head_.setLength( defaultLength );
	head_.setNumDivs( 1 );
	head_.setIsCylinder( true );
}

unsigned int SpineEntry::parent() const
{
		return parent_;
}

Id SpineEntry::shaftId() const
{
		return shaftId_;
}

Id SpineEntry::headId() const
{
		return headId_;
}

void SpineEntry::setParent( unsigned int parent )
{
	parent_ = parent;
}

void SpineEntry::mid( double& x, double& y, double& z ) const
{
	x = ( shaft_.getX() + head_.getX() ) / 2.0;
	y = ( shaft_.getY() + head_.getY() ) / 2.0;
	z = ( shaft_.getZ() + head_.getZ() ) / 2.0;
}

void SpineEntry::matchCubeMeshEntries( const ChemCompt* compt,
	unsigned int myIndex,
	double granularity, vector< VoxelJunction >& ret )
{
		// First flag is for curve, second is for cap of cylinder.
	head_.matchCubeMeshEntries( compt, shaft_, myIndex,
			granularity, ret, true, true );
}

double SpineEntry::volume() const
{
	return head_.volume( shaft_ );
}

void SpineEntry::matchCubeMeshEntriesToHead( const ChemCompt* compt,
	unsigned int myIndex,
	double granularity, vector< VoxelJunction >& ret ) const
{
	head_.matchCubeMeshEntries( compt, shaft_, myIndex,
			granularity, ret, true, false );
}

void SpineEntry::matchCubeMeshEntriesToPSD( const ChemCompt* compt,
				unsigned int myIndex,
				double granularity, vector< VoxelJunction >& ret ) const
{
	// First flag is for curve, second is for cap of cylinder.
	head_.matchCubeMeshEntries( compt, shaft_, myIndex,
			granularity, ret, false, true );
}

		/**
		 * Find the matching NeuroMesh entry index to the
		 * root of the shaft of this spine. Also compute the area and
		 * diffusion length of the shaft.
		 */
unsigned int SpineEntry::matchNeuroMeshEntriesToShaft(
				const ChemCompt* compt, unsigned int myIndex,
  		double& area, double& length ) const
{
		return 0;
}

double SpineEntry::rootArea() const
{
	return root_.getDia() * root_.getDia() * PI * 0.25;
}

double SpineEntry::diffusionLength() const
{
	// I've simplified this because a) it doesn't make much sense to add
	// a diffusion length with a different axial size and b) it makes the
	// resizing hugely complicated.
	// return shaft_.getLength() + 0.5 * head_.getLength();
	return shaft_.getLength();
}

vector< double > SpineEntry::psdCoords() const
{
	vector< double > ret( 8, 0.0 );
	double m0, m1, m2;
	mid( m0, m1, m2 );
	ret[0] = m0;
	ret[1] = m1;
	ret[2] = m2;
	ret[3] = head_.getX() - m0;
	ret[4] = head_.getY() - m1;
	ret[5] = head_.getZ() - m2;
	/*
	ret[0] = head_.getX();
	ret[1] = head_.getY();
	ret[2] = head_.getZ();
	double m0, m1, m2;
	mid( m0, m1, m2 );
	ret[3] = ret[0] - m0;
	ret[4] = ret[1] - m1;
	ret[5] = ret[2] - m2;
	*/
	ret[6] = head_.getDia();
	ret[7] = sqrt( ret[3] * ret[3] + ret[4] * ret[4] + ret[5] * ret[5] );

	return ret;
}


void SpineEntry::setVolume( double volume )
{
	double volscale = volume / head_.volume( shaft_ );
	double linscale = pow( volscale, 1.0 / 3.0 );
	head_.setLength( head_.getLength() * linscale );
	head_.setDia( head_.getDia() * linscale );
	double dx = head_.getX() - shaft_.getX();
	double dy = head_.getY() - shaft_.getY();
	double dz = head_.getZ() - shaft_.getZ();
	head_.setX( dx * linscale + shaft_.getX() );
	head_.setY( dy * linscale + shaft_.getY() );
	head_.setZ( dz * linscale + shaft_.getZ() );
}

void SpineEntry::positionShaftBase( double x, double y, double z )
{
	double dx = x - root_.getX();
	double dy = y - root_.getY();
	double dz = y - root_.getZ();
	root_.setX( x );
	root_.setY( y );
	root_.setZ( z );
	shaft_.setX( dx + shaft_.getX() );
	shaft_.setY( dy + shaft_.getY() );
	shaft_.setZ( dz + shaft_.getZ() );
	head_.setX( dx + head_.getX() );
	head_.setY( dy + head_.getY() );
	head_.setZ( dz + head_.getZ() );
}

