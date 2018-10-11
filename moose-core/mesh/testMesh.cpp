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
#include "../shell/Shell.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "ChemCompt.h"
#include "MeshCompt.h"
#include "CubeMesh.h"
#include "CylBase.h"
#include "NeuroNode.h"
#include "../basecode/SparseMatrix.h"
#include "NeuroMesh.h"
#include "../utility/Vec.h"
#include "CylMesh.h"
#include "SpineEntry.h"
#include "SpineMesh.h"
#include "PsdMesh.h"

/**
 * This tests how volume changes in a mesh propagate to all
 * child pools, reacs, and enzymes.
 */
void testVolScaling()
{
	extern Id makeReacTest();
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );

	Id kin = makeReacTest();
	Id subCompt =  s->doCreate( "CubeMesh", kin, "subCompt", 1 );
	Field< double >::set( subCompt, "volume", 1e-16 );
	Id SP = s->doCreate( "Pool", subCompt, "SP", 1 );
	Field< double >::set( SP, "concInit", 2 );

	vector< double > n;
	n.push_back( Field< double >::get( ObjId( "/kinetics/A" ), "nInit" ) );
	n.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool" ), "nInit" ) );
	n.push_back( Field< double >::get( ObjId( "/kinetics/r1" ), "numKf" ));
	n.push_back( Field< double >::get( ObjId( "/kinetics/r1" ), "numKb" ));
	n.push_back( Field< double >::get( ObjId( "/kinetics/r2" ), "numKf" ));
	n.push_back( Field< double >::get( ObjId( "/kinetics/r2" ), "numKb" ));
	n.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool/e1" ), "k1" ) );
	n.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool/e1" ), "k2" ) );
	n.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool/e1" ), "k3" ) );
	n.push_back( Field< double >::get( ObjId( "/kinetics/e2Pool/e2" ), "Km" ) );
	n.push_back( Field< double >::get( ObjId( "/kinetics/e2Pool/e2" ), "kcat" ) );
	n.push_back( Field< double >::get( SP, "nInit" ) );

	double vol = Field< double >::get( kin, "volume" );

	vol *= 10;
	Field< double >::set( kin, "volume", vol );

	vector< double > m;
	m.push_back( Field< double >::get( ObjId( "/kinetics/A" ), "nInit" ) );
	m.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool" ), "nInit" ) );
	m.push_back( Field< double >::get( ObjId( "/kinetics/r1" ), "numKf" ));
	m.push_back( Field< double >::get( ObjId( "/kinetics/r1" ), "numKb" ));
	m.push_back( Field< double >::get( ObjId( "/kinetics/r2" ), "numKf" ));
	m.push_back( Field< double >::get( ObjId( "/kinetics/r2" ), "numKb" ));
	m.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool/e1" ), "k1" ) );
	m.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool/e1" ), "k2" ) );
	m.push_back( Field< double >::get( ObjId( "/kinetics/e1Pool/e1" ), "k3" ) );
	m.push_back( Field< double >::get( ObjId( "/kinetics/e2Pool/e2" ), "Km" ) );
	m.push_back( Field< double >::get( ObjId( "/kinetics/e2Pool/e2" ), "kcat" ) );
	m.push_back( Field< double >::get( SP, "nInit" ) );

	assert( doubleEq( n[0] * 10, m[0] ) );	// A nInit
	assert( doubleEq( n[1] * 10, m[1] ) ); 	// e1Pool
	assert( doubleEq( n[2] / 10, m[2] ) );	// r1 numKf
	assert( doubleEq( n[3], m[3] ) );	// r1 numKb
	assert( doubleEq( n[4] / 10, m[4] ) );	// r2 numKf
	assert( doubleEq( n[5], m[5] ) );	// r2 numKb
	assert( doubleEq( n[6] / 10, m[6] ) );	// e1 k1
	assert( doubleEq( n[7], m[7] ) );	// e1 k2
	assert( doubleEq( n[8], m[8] ) );	// e1 k3
	assert( doubleEq( n[9], m[9] ) );	// e2 Km
	assert( doubleEq( n[10], m[10] ) );	// e2 kcat
	assert( doubleEq( n[11], m[11] ) );	// SP nInit

	s->doDelete( kin );
	cout << "." << flush;
}

#if 0
/**
 * Low-level test for Cylbase, no MOOSE calls
 */
void testCylBase()
{
	// CylBase( x, y, z, dia, len, numDivs );
	CylBase  a( 0, 0, 0, 1,   1, 1 );
	CylBase  b( 1, 2, 3, 1,   2, 10 );

	assert( doubleEq( b.volume( a ), PI * 0.25 * 2 ) );
	assert( a.getNumDivs() == 1 );
	assert( b.getNumDivs() == 10 );

	for ( unsigned int i = 0; i < 10; ++i ) {
		assert( doubleEq( b.voxelVolume( a, i ), PI * 0.25 * 2 * 0.1 ) );
		vector< double > coords = b.getCoordinates( a, i );
		double x = i;
		assert( doubleEq( coords[0], x / 10.0 ) );
		assert( doubleEq( coords[1], x * 2.0 / 10.0 ) );
		assert( doubleEq( coords[2], x * 3.0  / 10.0 ) );
		assert( doubleEq( coords[3], (1.0 + x) / 10.0 ) );
		assert( doubleEq( coords[4], (1.0 + x) * 2.0 / 10.0 ) );
		assert( doubleEq( coords[5], (1.0 + x) * 3.0  / 10.0 ) );
		assert( doubleEq( coords[6], 0.5 ) );
		assert( doubleEq( coords[7], 0.5 ) );
		assert( doubleEq( coords[8], 0.0 ) );
		assert( doubleEq( coords[9], 0.0 ) );

		assert( doubleEq( b.getDiffusionArea( a, i ), PI * 0.25 ) );
	}
	b.setDia( 2.0 );
	assert( doubleEq( b.getDia(), 2.0 ) );
	assert( doubleEq( b.volume( a ), PI * (2*(0.5*0.5+0.5+1)/3.0) ) );
	for ( unsigned int i = 0; i < 10; ++i ) {
		double x = static_cast< double >( i ) / 10.0;
		double r0 = 0.5 * ( a.getDia() * ( 1.0 - x ) + b.getDia() * x );
		x = x + 0.1;
		double r1 = 0.5 * ( a.getDia() * ( 1.0 - x ) + b.getDia() * x );
		// len = 2, and we have 10 segments of it.
		double vv = 0.2 * ( r0 * r0 + r0 * r1 + r1 * r1 ) * PI / 3.0;
		assert( doubleEq( b.voxelVolume( a, i ), vv ) );
		assert( doubleEq( b.getDiffusionArea( a, i ), PI * r0 * r0 ) );
	}

	cout << "." << flush;
}

/**
 * Low-level test for NeuroNode, no MOOSE calls
 */
void testNeuroNode()
{
	CylBase  a( 0, 0, 0, 1,   1, 1 );
	CylBase  b( 1, 2, 3, 2,   2, 10 );
	CylBase  dcb( 1, 2, 3, 2,   2, 0 );
	vector< unsigned int > twoKids( 2, 2 );
	twoKids[0] = 2;
	twoKids[1] = 4;
	vector< unsigned int > oneKid( 1, 2 );
	vector< unsigned int > noKids( 0 );
	NeuroNode na( a, 0, twoKids, 0, Id(), true );
	NeuroNode ndummy( dcb, 0, oneKid, 1, Id(), false );
	NeuroNode nb( b, 1, noKids, 1, Id(), false );

	assert( na.parent() == 0 );
	assert( na.startFid() == 0 );
	assert( na.elecCompt() == Id() );
	assert( na.isDummyNode() == false );
	assert( na.isSphere() == true );
	assert( na.isStartNode() == true );
	assert( na.children().size() == 2 );
	assert( na.children()[0] == 2 );
	assert( na.children()[1] == 4 );
	assert( doubleEq( na.volume( a ), PI * 0.25 ) );

	assert( ndummy.parent() == 0 );
	assert( ndummy.startFid() == 1 );
	assert( ndummy.elecCompt() == Id() );
	assert( ndummy.isDummyNode() == true );
	assert( ndummy.isSphere() == false );
	assert( ndummy.isStartNode() == false );
	assert( ndummy.children().size() == 1 );
	assert( ndummy.children()[0] == 2 );

	assert( nb.parent() == 1 );
	assert( nb.startFid() == 1 );
	assert( nb.elecCompt() == Id() );
	assert( nb.isDummyNode() == false );
	assert( nb.isSphere() == false );
	assert( nb.isStartNode() == false );
	assert( nb.children().size() == 0 );
	assert( doubleEq( nb.volume( a ), PI * (2*(0.5*0.5+0.5+1)/3.0) ) );

	cout << "." << flush;
}

void zebraConcPattern( vector< vector< double > >& S )
{
	// Set up an alternating pattern of zero/nonzero voxels to do test
	// calculations
	assert( S.size() == 44 );
	for ( unsigned int i = 0; i < S.size(); ++i )
		S[i][0] = 0;

	S[0][0] = 100; // soma
	S[2][0] = 10; // basal dend
	S[4][0] = 1; // basal
	S[6][0] = 10; // basal
	S[8][0] = 1; // basal
	S[10][0] = 10; // basal
	S[12][0] = 1; // basal
	S[14][0] = 10; // basal
	S[16][0] = 1; // basal
	S[18][0] = 10; // basal
	S[20][0] = 1; // basal tip

	S[22][0] = 10; // Apical left dend second compt.
	S[24][0] = 1; // Apical left dend
	S[26][0] = 10; // Apical left dend
	S[28][0] = 1; // Apical left dend
	S[30][0] = 10; // Apical left dend last compt

	// Tip compts are 31 and 32:
	S[32][0] = 1; // Apical middle Tip compartment.

	S[34][0] = 10; // Apical right dend second compt.
	S[36][0] = 1; // Apical right dend
	S[38][0] = 10; // Apical right dend
	S[40][0] = 1; // Apical right dend
	S[42][0] = 10; // Apical right dend last compt

	S[43][0] = 1; // Apical right tip.
}

/// Set up uniform concentrations in each voxel. Only the end voxels
/// should see a nonzero flux.
void uniformConcPattern( vector< vector< double > >& S,
				vector< double >& vs )
{
	// Set up uniform concs throughout. This means to scale each # by
	// voxel size.
	assert( S.size() == 44 );
	for ( unsigned int i = 0; i < S.size(); ++i )
		S[i][0] = 10 * vs[i];
}

/**
 * Low-level test for NeuroStencil, no MOOSE calls
 * This outlines the policy for how nodes and dummies are
 * organized.
 * The set of nodes represents a neuron like this:
 *
 *  \/ /  1
 *   \/   2
 *    |   3
 *    |   4
 * The soma is at the top of the vertical section. Diameter = 10 microns.
 * Each little segment is a NeuroNode. Assume 10 microns each.
 * Level 1 has only 1 voxel in each segment. 1 micron at tip
 * Level 2 has 10 voxels in each segment. 1 micron at tip,
 *   2 microns at soma surface
 * The soma is between levels 2 and 3 and is a single voxel.
 *  10 microns diameter.
 * Level 3 has 10 voxels. 3 micron at soma, 2 microns at tip
 * Level 4 has 10 voxels. 1 micron at tip.
 * We have a total of 8 full nodes, plus 3 dummy nodes for
 * the segments at 2 and 3, since they plug into a sphere.
	Order is depth first:
			0: soma
			1: basal dummy
			2: basal segment at level 3
			3: basal segment at level 4
			4: left dummy
			5: left segment at level 2, apicalL1
			6: left segment at level 1, apicalL2
			7: other (middle) segment at level 1;  apicalM
			8: right dummy
			9: right segment at level 2, apicalR1
			10: right segment at level 1, apicalR2
 */
unsigned int buildNode( vector< NeuroNode >& nodes, unsigned int parent,
		double x, double y, double z, double dia,
		unsigned int numDivs, bool isDummy, unsigned int startFid )
{
	x *= 1e-6;
	y *= 1e-6;
	z *= 1e-6;
	dia *= 1e-6;
	CylBase cb( x, y, z, dia, dia/2, numDivs );
	vector< unsigned int > kids( 0 );
	if ( nodes.size() == 0 ) { // make soma
		NeuroNode nn( cb, parent, kids, startFid, Id(), true );
		nodes.push_back( nn );
	} else if ( isDummy ) { // make dummy
		NeuroNode nn( cb, parent, kids, startFid, Id(), false );
		nodes.push_back( nn );
	} else {
		NeuroNode nn( cb, parent, kids, startFid, Id(), false );
		nodes.push_back( nn );
	}
	NeuroNode& paNode = nodes[ parent ];
	nodes.back().calculateLength( paNode );
	return startFid + numDivs;
}

void connectChildNodes( vector< NeuroNode >& nodes )
{
	assert( nodes.size() == 11 );
	for ( unsigned int i = 1; i < nodes.size(); ++i ) {
		assert( nodes[i].parent() < nodes.size() );
		NeuroNode& parent = nodes[ nodes[i].parent() ];
		parent.addChild( i );
	}
	////////////////////////////////////////////////////////////////////
	// Check children
	assert( nodes[0].children().size() == 3 );
	assert( nodes[0].children()[0] == 1 );
	assert( nodes[0].children()[1] == 4 );
	assert( nodes[0].children()[2] == 8 );
	assert( nodes[1].children().size() == 1 );
	assert( nodes[1].children()[0] == 2 );
	assert( nodes[2].children().size() == 1 );
	assert( nodes[2].children()[0] == 3 );
	assert( nodes[3].children().size() == 0 );
	assert( nodes[4].children().size() == 1 );
	assert( nodes[4].children()[0] == 5 );
	assert( nodes[5].children().size() == 2 );
	assert( nodes[5].children()[0] == 6 );
	assert( nodes[5].children()[1] == 7 );
	assert( nodes[6].children().size() == 0 );
	assert( nodes[7].children().size() == 0 );
	assert( nodes[8].children().size() == 1 );
	assert( nodes[8].children()[0] == 9 );
	assert( nodes[9].children().size() == 1 );
	assert( nodes[9].children()[0] == 10 );
	assert( nodes[10].children().size() == 0 );
}


/**
 * Low-level tests for the CylMesh object: No MOOSE calls involved.
 */
void testCylMesh()
{
	CylMesh cm;
	assert( cm.getMeshType( 0 ) == CYL );
	assert( cm.getMeshDimensions( 0 ) == 3 );
	assert( cm.getDimensions() == 3 );

	vector< double > coords( 9 );
	coords[0] = 1; // X0
	coords[1] = 2; // Y0
	coords[2] = 3; // Z0

	coords[3] = 3; // X1
	coords[4] = 5; // Y1
	coords[5] = 7; // Z1

	coords[6] = 1; // R0
	coords[7] = 2; // R1

	coords[8] = 1; // DiffLength

	cm.innerSetCoords( coords );

	assert( doubleEq( cm.getX0(), 1 ) );
	assert( doubleEq( cm.getY0(), 2 ) );
	assert( doubleEq( cm.getZ0(), 3 ) );
	assert( doubleEq( cm.getR0(), 1 ) );

	assert( doubleEq( cm.getX1(), 3 ) );
	assert( doubleEq( cm.getY1(), 5 ) );
	assert( doubleEq( cm.getZ1(), 7 ) );
	assert( doubleEq( cm.getR1(), 2 ) );

	assert( doubleEq( cm.getDiffLength(), sqrt( 29.0 ) / 5.0 ) );

	cm.setX0( 2 );
	cm.setY0( 3 );
	cm.setZ0( 4 );
	cm.setR0( 2 );

	cm.setX1( 4 );
	cm.setY1( 6 );
	cm.setZ1( 8 );
	cm.setR1( 3 );

	cm.setDiffLength( 2.0 );

	vector< double > temp = cm.getCoords( Id().eref(), 0 );
	assert( temp.size() == 9 );
	// Can't check on the last coord as it is DiffLength, it changes.
	for ( unsigned int i = 0; i < temp.size() - 1; ++i )
		assert( doubleEq( temp[i], coords[i] + 1 ) );

	double totLen = sqrt( 29.0 );
	assert( doubleEq( cm.getTotLength() , totLen ) );

	cm.setDiffLength( 1.0 );
	assert( cm.getNumEntries() == 5 );
	assert( doubleEq( cm.getDiffLength(), totLen / 5 ) );

	///////////////////////////////////////////////////////////////
	assert( doubleEq( cm.getMeshEntryVolume( 2 ), 2.5 * 2.5 * PI * totLen / 5 ) );

	///////////////////////////////////////////////////////////////
	// LenSlope/totLen = 0.016 =
	// 	1/numEntries * (r1-r0)/numEntries * 2/(r0+r1) = 1/25 * 1 * 2/5
	// Here are the fractional positions
	// part0 = 1/5 - 0.032: end= 0.2 - 0.032
	// part1 = 1/5 - 0.016: end = 0.4 - 0.048
	// part2 = 1/5			: end = 0.6 - 0.048
	// part3 = 1/5 + 0.016	: end = 0.8 - 0.032
	// part4 = 1/5 + 0.032	: end = 1

	coords = cm.getCoordinates( 2 );
	assert( coords.size() == 10 );
	assert( doubleEq( coords[0], 2 + (0.4 - 0.048) * 2 ) );
	assert( doubleEq( coords[1], 3 + (0.4 - 0.048) * 3 ) );
	assert( doubleEq( coords[2], 4 + (0.4 - 0.048) * 4 ) );

	assert( doubleEq( coords[3], 2 + (0.6 - 0.048) * 2 ) );
	assert( doubleEq( coords[4], 3 + (0.6 - 0.048) * 3 ) );
	assert( doubleEq( coords[5], 4 + (0.6 - 0.048) * 4 ) );

	assert( doubleEq( coords[6], 2.4 ) );
	assert( doubleEq( coords[7], 2.6 ) );

	///////////////////////////////////////////////////////////////
	vector< unsigned int > neighbors = cm.getNeighbors( 2 );
	assert( neighbors.size() == 2 );
	assert( neighbors[0] == 1 );
	assert( neighbors[1] == 3 );

	///////////////////////////////////////////////////////////////
	coords = cm.getDiffusionArea( 2 );
	assert( coords.size() == 2 );
	assert( doubleEq( coords[0], 2.4 * 2.4 * PI ) );
	assert( doubleEq( coords[1], 2.6 * 2.6 * PI ) );

	///////////////////////////////////////////////////////////////
	coords = cm.getCoords( Id().eref(), 0 );
	assert( coords.size() == 9 );

	double x, y, z;
	double ne = cm.getNumEntries();
	double ux = ( coords[3] - coords[0] ) / ne;
	double uy = ( coords[4] - coords[1] ) / ne;
	double uz = ( coords[5] - coords[2] ) / ne;
	cm.indexToSpace( 0, x, y, z );
	assert( doubleEq( x, coords[0] + 0.5 * ux ) );
	assert( doubleEq( y, coords[1] + 0.5 * uy ) );
	assert( doubleEq( z, coords[2] + 0.5 * uz ) );
	cm.indexToSpace( 4, x, y, z );
	assert( doubleEq( x, coords[3] - 0.5 * ux ) );
	assert( doubleEq( y, coords[4] - 0.5 * uy ) );
	assert( doubleEq( z, coords[5] - 0.5 * uz ) );

	///////////////////////////////////////////////////////////////

	unsigned int index;
	double dist = cm.nearest( x, y, z, index );
	assert( doubleEq( dist, 0.0 ) );
	assert( index == cm.innerGetNumEntries() - 1 );
	x += 1; // Should be within R1
	dist = cm.nearest( x, y, z, index );
	assert( dist > 0.0 && dist < 1.0 );
	assert( index == cm.innerGetNumEntries() - 1 );

	x += 5; // Should now be outside R1
	dist = cm.nearest( x, y, z, index );
	assert( dist < 0.0 );
	assert( index == cm.innerGetNumEntries() - 1 );

	///////////////////////////////////////////////////////////////
	dist = cm.selectGridVolume( 10.0 );
	assert( doubleEq( dist, 0.1 * cm.getDiffLength() ) );
	dist = cm.selectGridVolume( 0.1 );
	assert( dist <= 0.01 );

	///////////////////////////////////////////////////////////////
	// Test CylMesh::fillPointsOnCircle
	CubeMesh cube;
	cube.setPreserveNumEntries( 0 );
	coords[0] = 0; // X0
	coords[1] = 0; // Y0
	coords[2] = 0; // Z0

	coords[3] = 100; // X1
	coords[4] = 100; // Y1
	coords[5] = 100; // Z1
	coords[6] = 100; // DX
	coords[7] = 100; // DY
	coords[8] = 100; // DZ
	cube.innerSetCoords( coords );
	vector< VoxelJunction > ret;
	cm.matchCubeMeshEntries( &cube, ret );
	assert( ret.size() == cm.innerGetNumEntries() );
	for ( unsigned int i = 0; i < ret.size(); ++i ) {
		assert( ret[i].first == i );
		assert( ret[i].second == 0 ); // Only one cube entry
		double r = cm.getR0() +
				( cm.getR1() - cm.getR0() ) *
				cm.getDiffLength() * ( 0.5 + i ) / cm.getTotLength();
		double a = r * cm.getDiffLength() * 2 * PI;
		//assert( doubleApprox( ret[i].diffScale, a ) );
		// cout << i << ". mesh: " << ret[i].diffScale << ", calc: " << a << endl;
		assert( fabs( ret[i].diffScale - a ) < 0.5 );
	}

	///////////////////////////////////////////////////////////////
	// We're going to set up a new cylinder to abut the old one. The
	// coords of x1 of the new cylinder are the same x0 of the old one, but
	// the coords of the other end change.
	coords = cm.getCoords( Id().eref(), 0 );
	coords[3] = coords[0];
	coords[4] = coords[1];
	coords[5] = coords[2];
	coords[2] -= 10; // x and y stay put, it is a vertical cylinder.
	coords[6] = 1; // R0
	coords[7] = 1; // R1
	coords[8] = 1; // DiffLength
	CylMesh cm2;
	cm2.innerSetCoords( coords );
	assert( doubleEq( cm2.getDiffLength(), 1 ) );
	assert( cm2.getNumEntries() == 10 );
	vector< VoxelJunction > vj;
	cm.matchMeshEntries( &cm2, vj );
	assert( vj.size() == 1 );
	assert( vj[0].first == 0 );
	assert( vj[0].second == cm2.getNumEntries() - 1 );
	double xda = 2.0 * 1 * 1 * PI / ( cm.getDiffLength() + cm2.getDiffLength() );
	assert( doubleEq( vj[0].diffScale, xda ) );

	cout << "." << flush;
}


/**
 * mid-level tests for the CylMesh object, using MOOSE calls.
 */
void testMidLevelCylMesh()
{
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );

	Id cylId = s->doCreate( "CylMesh", Id(), "cyl", 1 );
	Id meshId( cylId.value() + 1 );

	vector< double > coords( 9 );
	coords[0] = 1; // X0
	coords[1] = 2; // Y0
	coords[2] = 3; // Z0

	coords[3] = 3; // X1
	coords[4] = 5; // Y1
	coords[5] = 7; // Z1

	coords[6] = 1; // R0
	coords[7] = 2; // R1

	coords[8] = 1; // DiffLength

	bool ret = Field< vector< double > >::set( cylId, "coords", coords );
	assert( ret );

	assert( doubleEq( Field< double >::get( cylId, "x0" ), 1 ) );
	assert( doubleEq( Field< double >::get( cylId, "y0" ), 2 ) );
	assert( doubleEq( Field< double >::get( cylId, "z0" ), 3 ) );
	assert( doubleEq( Field< double >::get( cylId, "x1" ), 3 ) );
	assert( doubleEq( Field< double >::get( cylId, "y1" ), 5 ) );
	assert( doubleEq( Field< double >::get( cylId, "z1" ), 7 ) );
	assert( doubleEq( Field< double >::get( cylId, "r0" ), 1 ) );
	assert( doubleEq( Field< double >::get( cylId, "r1" ), 2 ) );

	ret = Field< double >::set( cylId, "diffLength", 1 );
	assert( ret );
	meshId.element()->syncFieldDim();

	assert( meshId()->dataHandler()->localEntries() == 5 );

	unsigned int n = Field< unsigned int >::get( cylId, "num_mesh" );
	assert( n == 5 );

	ObjId oid( meshId, DataId( 2 ) );

	double totLen = sqrt( 29.0 );
	assert( doubleEq( Field< double >::get( oid, "volume" ),
		1.5 * 1.5 * PI * totLen / 5 ) );

	vector< unsigned int > neighbors =
		Field< vector< unsigned int > >::get( oid, "neighbors" );
	assert( neighbors.size() == 2 );
	assert( neighbors[0] = 1 );
	assert( neighbors[1] = 3 );

	s->doDelete( cylId );

	cout << "." << flush;
}
#endif

/**
 * Low-level tests for the CubeMesh object: No MOOSE calls involved.
 */
void testCubeMesh()
{
	CubeMesh cm;
	cm.setPreserveNumEntries( 0 );
	assert( cm.getMeshType( 0 ) == CUBOID );
	assert( cm.getMeshDimensions( 0 ) == 3 );
	assert( cm.getDimensions() == 3 );

	vector< double > coords( 9 );
	coords[0] = 0; // X0
	coords[1] = 0; // Y0
	coords[2] = 0; // Z0

	coords[3] = 2; // X1
	coords[4] = 4; // Y1
	coords[5] = 8; // Z1

	coords[6] = 1; // DX
	coords[7] = 1; // DY
	coords[8] = 1; // DZ

	cm.innerSetCoords( coords );

	vector< unsigned int > neighbors = cm.getNeighbors( 0 );
	assert( neighbors.size() == 3 );
	assert( neighbors[0] = 1 );
	assert( neighbors[0] = 2 );
	assert( neighbors[0] = 8 );

	assert( cm.innerGetNumEntries() == 64 );
	assert( doubleEq( cm.getX0(), 0 ) );
	assert( doubleEq( cm.getY0(), 0 ) );
	assert( doubleEq( cm.getZ0(), 0 ) );

	assert( doubleEq( cm.getX1(), 2 ) );
	assert( doubleEq( cm.getY1(), 4 ) );
	assert( doubleEq( cm.getZ1(), 8 ) );

	assert( doubleEq( cm.getDx(), 1 ) );
	assert( doubleEq( cm.getDy(), 1 ) );
	assert( doubleEq( cm.getDz(), 1 ) );

	assert( cm.getNx() == 2 );
	assert( cm.getNy() == 4 );
	assert( cm.getNz() == 8 );

	cm.setX0( 1 );
	cm.setY0( 2 );
	cm.setZ0( 4 );

	cm.setX1( 5 );
	cm.setY1( 6 );
	cm.setZ1( 8 );

	vector< double > temp = cm.getCoords( Id().eref() );
	assert( temp.size() == 9 );
	assert( doubleEq( temp[0], 1 ) );
	assert( doubleEq( temp[1], 2 ) );
	assert( doubleEq( temp[2], 4 ) );
	assert( doubleEq( temp[3], 5 ) );
	assert( doubleEq( temp[4], 6 ) );
	assert( doubleEq( temp[5], 8 ) );
	assert( doubleEq( temp[6], 1 ) );
	assert( doubleEq( temp[7], 1 ) );
	assert( doubleEq( temp[8], 1 ) );
	assert( cm.innerGetNumEntries() == 64 );
	assert( cm.getNx() == 4 );
	assert( cm.getNy() == 4 );
	assert( cm.getNz() == 4 );

	neighbors = cm.getNeighbors( 0 );
	assert( neighbors.size() == 3 );
	assert( neighbors[0] == 1 );
	assert( neighbors[1] == 4 );
	assert( neighbors[2] = 16 );

	neighbors = cm.getNeighbors( 63 );
	assert( neighbors.size() == 3 );
	assert( neighbors[0] == 47 );
	assert( neighbors[1] == 59 );
	assert( neighbors[2] == 62 );

	neighbors = cm.getNeighbors( 2 );
	assert( neighbors.size() == 4 );
	assert( neighbors[0] == 1 );
	assert( neighbors[1] == 3 );
	assert( neighbors[2] == 6 );
	assert( neighbors[3] == 18 );

	neighbors = cm.getNeighbors( 6 );
	assert( neighbors.size() == 5 );
	assert( neighbors[0] == 2 );
	assert( neighbors[1] == 5 );
	assert( neighbors[2] == 7 );
	assert( neighbors[3] == 10 );
	assert( neighbors[4] == 22 );

	neighbors = cm.getNeighbors( 22 );
	assert( neighbors.size() == 6 );
	assert( neighbors[0] == 6 );
	assert( neighbors[1] == 18 );
	assert( neighbors[2] == 21 );
	assert( neighbors[3] == 23 );
	assert( neighbors[4] == 26 );
	assert( neighbors[5] == 38 );

	cm.setPreserveNumEntries( 1 );
	assert( cm.getNx() == 4 );
	assert( cm.getNy() == 4 );
	assert( cm.getNz() == 4 );
	assert( doubleEq( cm.getDx(), 1.0 ) );
	assert( doubleEq( cm.getDy(), 1.0 ) );
	assert( doubleEq( cm.getDz(), 1.0 ) );

	cm.setX0( 0 );
	cm.setY0( 0 );
	cm.setZ0( 0 );
	// x1 is 5, y1 is 6 and z1 is 8

	assert( doubleEq( cm.getDx(), 1.25 ) );
	assert( doubleEq( cm.getDy(), 1.5 ) );
	assert( doubleEq( cm.getDz(), 2.0 ) );

	cout << "." << flush;
}

void testCubeMeshExtendStencil()
{
	CubeMesh cm0;
	cm0.setPreserveNumEntries( 0 );
	assert( cm0.getMeshType( 0 ) == CUBOID );
	assert( cm0.getMeshDimensions( 0 ) == 3 );
	assert( cm0.getDimensions() == 3 );
	CubeMesh cm1 = cm0;

	vector< double > coords( 9 );
	coords[0] = 0; // X0
	coords[1] = 0; // Y0
	coords[2] = 0; // Z0

	coords[3] = 2; // X1
	coords[4] = 4; // Y1
	coords[5] = 8; // Z1

	coords[6] = 1; // DX
	coords[7] = 1; // DY
	coords[8] = 1; // DZ

	cm0.innerSetCoords( coords );
	coords[2] = 8; // 2x4 face abuts.
	coords[5] = 16;
	cm1.innerSetCoords( coords );

	const double* entry;
	const unsigned int* colIndex;
	unsigned int num = cm0.getStencilRow( 0, &entry, &colIndex );
	assert( num == 3 );
	assert( colIndex[0] == 1 );
	assert( colIndex[1] == 2 );
	assert( colIndex[2] == 8 );

	num = cm0.getStencilRow( 56, &entry, &colIndex );
	assert( num == 3 );
	assert( colIndex[0] == 48 );
	assert( colIndex[1] == 57 );
	assert( colIndex[2] == 58 );

	vector< VoxelJunction > vj;
	for ( unsigned int i = 0; i < 8; ++i ) {
		vj.push_back( VoxelJunction( 56 + i, i ) );
	}
	cm0.extendStencil( &cm1, vj );

	num = cm0.getStencilRow( 56, &entry, &colIndex );
	assert( num == 4 );
	assert( colIndex[0] == 48 );
	assert( colIndex[1] == 57 );
	assert( colIndex[2] == 58 );
	assert( colIndex[3] == 64 );

	for ( unsigned int i = 0; i < 8; ++i ) {
		num = cm0.getStencilRow( 64 + i, &entry, &colIndex );
		assert( num == 1 );
		assert( colIndex[0] == 56 + i );
	}

	cout << "." << flush;
}

/**
 * Tests scaling of volume and # of entries upon mesh resize
 */
void testReMesh()
{
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	Id base = s->doCreate( "Neutral", Id(), "base", 1 );

	Id cube = s->doCreate( "CubeMesh", base, "cube", 1 );
	bool ret = SetGet2< double, unsigned int >::set(
		cube, "buildDefaultMesh", 1.0, 1 );
	assert( ret );
	unsigned int vol = Field< double >::get( cube, "volume" );
	assert( doubleEq( vol, 1.0 ) );
	Id pool = s->doCreate( "Pool", cube, "pool", 1 );
	Id mesh( "/base/cube/mesh" );
	assert( mesh != Id() );

/////////////////////////////////////////////////////////////////
	// 1 millimolar in 1 m^3 is 1 mole per liter.
	double linsize = Field< double >::get( pool, "volume" );
	assert( doubleEq( linsize, 1.0 ) );

	ret = Field< double >::set( pool, "conc", 1 );
	assert( ret );
	double n = Field< double >::get( pool, "n" );
	assert( doubleEq( n, NA ) );

	ret = SetGet2< double, unsigned int >::set(
		cube, "buildDefaultMesh", 1.0e-3, 1 );
	Field< double >::set( pool, "conc", 1 );
	n = Field< double >::get( pool, "n" );
	assert( doubleEq( n, NA / 1000.0 ) );

/////////////////////////////////////////////////////////////////
	// Next we do the remeshing.
	double x = 1.234;
	Field< double >::set( pool, "concInit", x );
	ret = SetGet2< double, unsigned int >::set(
		cube, "buildDefaultMesh", 1, 8 );
	// This is nasty, needs the change to propagate through messages.
	linsize = Field< double >::get( pool, "volume" );
	assert( doubleEq( linsize, 0.125 ) );

	n = Field< double >::get( ObjId( pool, 0 ), "concInit" );
	assert( doubleEq( n, x ) );
	n = Field< double >::get( ObjId( pool, 7 ), "concInit" );
	assert( doubleEq( n, x ) );
	n = Field< double >::get( ObjId( pool, 0 ), "nInit" );
	assert( doubleEq( n, x * NA / 8.0 ) );
	n = Field< double >::get( ObjId( pool, 7 ), "nInit" );
	assert( doubleEq( n, x * NA / 8.0 ) );
	n = Field< double >::get( ObjId( pool, 0 ), "conc" );
	assert( doubleEq( n, x ) );
	n = Field< double >::get( ObjId( pool, 7 ), "conc" );
	assert( doubleEq( n, x ) );

/////////////////////////////////////////////////////////////////
	s->doDelete( base );
	cout << "." << flush;
}

/**
 * theta in degrees
 * len and dia in metres as usual
 */
Id makeCompt( Id parentCompt, Id parentObj,
		string name, double len, double dia, double theta )
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	Id ret = shell->doCreate( "Compartment", parentObj, name, 1 );
	double pax = 0;
	double pay = 0;
	if ( parentCompt != Id() ) {
		pax = Field< double >::get( parentCompt, "x" );
		pay = Field< double >::get( parentCompt, "y" );
		shell->doAddMsg( "Single", parentCompt, "raxial", ret, "axial" );
	}
	Field< double >::set( ret, "x0", pax );
	Field< double >::set( ret, "y0", pay );
	Field< double >::set( ret, "z0", 0.0 );
	double x = pax + len * cos( theta * PI / 180.0 );
	double y = pay + len * sin( theta * PI / 180.0 );
	Field< double >::set( ret, "x", x );
	Field< double >::set( ret, "y", y );
	Field< double >::set( ret, "z", 0.0 );
	Field< double >::set( ret, "diameter", dia );
	Field< double >::set( ret, "length", len );

	return ret;
}

#if 0
// Assumes parent dend is along x axis.
Id makeSpine( Id parentCompt, Id parentObj, unsigned int index,
		double frac, double len, double dia, double theta )
{
	const double RA = 1;
	const double RM = 1;
	const double CM = 0.01;
	assert( parentCompt != Id() );
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	double pax0 = Field< double >::get( parentCompt, "x0" );
	double pay0 = Field< double >::get( parentCompt, "y0" );
	double paz0 = Field< double >::get( parentCompt, "z0" );
	double pax1 = Field< double >::get( parentCompt, "x" );
	double pay1 = Field< double >::get( parentCompt, "y" );
	double paz1 = Field< double >::get( parentCompt, "z" );

	stringstream ss;
	ss << "shaft" << index;
	string sname = ss.str();
	stringstream ss2;
	ss2 << "head" << index;
	string hname = ss2.str();

	Id shaft = shell->doCreate( "Compartment", parentObj, sname, 1 );
	ObjId mid =
		shell->doAddMsg( "Single", parentCompt, "raxial", shaft, "axial" );
	assert( !mid.bad() );
	double x = pax0 + frac * ( pax1 - pax0 );
	double y = pay0 + frac * ( pay1 - pay0 );
	double z = paz0 + frac * ( paz1 - paz0 );
	Field< double >::set( shaft, "x0", x );
	Field< double >::set( shaft, "y0", y );
	Field< double >::set( shaft, "z0", z );
	double sy = y + len * cos( theta * PI / 180.0 );
	double sz = z + len * sin( theta * PI / 180.0 );
	Field< double >::set( shaft, "x", x );
	Field< double >::set( shaft, "y", sy );
	Field< double >::set( shaft, "z", sz );
	Field< double >::set( shaft, "diameter", dia/10.0 );
	Field< double >::set( shaft, "length", len );
	double xa = PI * dia * dia / 400.0;
	double circumference = PI * dia / 10.0;
	Field< double >::set( shaft, "Ra", RA * len / xa );
	Field< double >::set( shaft, "Rm", RM / ( len * circumference ) );
	Field< double >::set( shaft, "Cm", CM * len * circumference );

	Id head = shell->doCreate( "Compartment", parentObj, hname, 1 );
	mid = shell->doAddMsg( "Single", shaft, "raxial", head, "axial" );
	assert( !mid.bad() );
	Field< double >::set( head, "x0", x );
	Field< double >::set( head, "y0", sy );
	Field< double >::set( head, "z0", sz );
	double hy = sy + len * cos( theta * PI / 180.0 );
	double hz = sz + len * sin( theta * PI / 180.0 );
	Field< double >::set( head, "x", x );
	Field< double >::set( head, "y", hy );
	Field< double >::set( head, "z", hz );
	Field< double >::set( head, "diameter", dia );
	Field< double >::set( head, "length", len );
	xa = PI * dia * dia / 4.0;
	circumference = PI * dia;
	Field< double >::set( head, "Ra", RA * len / xa );
	Field< double >::set( head, "Rm", RM / ( len * circumference ) );
	Field< double >::set( head, "Cm", CM * len * circumference );

	return head;
}

/**
 * soma---->d1---->d1a---->d11---->d111
 *  |               |       +----->d112
 *  |               |
 *  |               +----->d12---->d121
 *  |                       +----->d122
 *  |
 *  +------>d2----->d2a---->d21---->d211
 *                  |        +----->d212
 *                  |
 *                  +------>d22---->d221
 *                           +----->d222
 *
 */
pair< unsigned int, unsigned int > buildBranchingCell(
				Id cell, double len, double dia )
{
	Id soma = makeCompt( Id(), cell, "soma", dia, dia, 90 );
	dia /= sqrt( 2.0 );
	Id d1 = makeCompt( soma, cell, "d1", len , dia, 0 );
	Id d2 = makeCompt( soma, cell, "d2", len , dia, 180 );
	Id d1a = makeCompt( d1, cell, "d1a", len , dia, 0 );
	Id d2a = makeCompt( d2, cell, "d2a", len , dia, 180 );
	dia /= sqrt( 2.0 );
	Id d11 = makeCompt( d1a, cell, "d11", len , dia, -45 );
	Id d12 = makeCompt( d1a, cell, "d12", len , dia, 45 );
	Id d21 = makeCompt( d2a, cell, "d21", len , dia, 45 );
	Id d22 = makeCompt( d2a, cell, "d22", len , dia, -45 );
	dia /= sqrt( 2.0 );
	Id d111 = makeCompt( d11, cell, "d111", len , dia, -90 );
	Id d112 = makeCompt( d11, cell, "d112", len , dia, 0 );
	Id d121 = makeCompt( d12, cell, "d121", len , dia, 0 );
	Id d122 = makeCompt( d12, cell, "d122", len , dia, 90 );
	Id d211 = makeCompt( d21, cell, "d211", len , dia, 90 );
	Id d212 = makeCompt( d21, cell, "d212", len , dia, 180 );
	Id d221 = makeCompt( d22, cell, "d221", len , dia, 180 );
	Id d222 = makeCompt( d22, cell, "d222", len , dia, -90 );

	return pair< unsigned int, unsigned int >( 17, 161 );
}

	// y = initConc * dx * (0.5 / sqrt( PI * DiffConst * runtime ) ) *
	//        exp( -x * x / ( 4 * DiffConst * runtime ) )
double diffusionFunction( double D, double dx, double x, double t )
{
	return
		dx * (0.5 / sqrt( PI * D * t ) ) * exp( -x * x / ( 4 * D * t ) );
}

void testNeuroMeshLinear()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// Build a linear cylindrical cell, no tapering.
	Id cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
	unsigned int numCompts = 500;
	double dia = 1e-6; // metres
	double diffLength = 0.2e-6; // metres
	double len = diffLength * numCompts;
	double D = 1e-12;
	double totNum = 1e6;

	Id soma = makeCompt( Id(), cell, "soma", len, dia, 0 );

	// Scan it with neuroMesh and check outcome.
	Id nm = shell->doCreate( "NeuroMesh", Id(), "neuromesh", 1 );
	Field< double >::set( nm, "diffLength", diffLength );
	Field< string >::set( nm, "geometryPolicy", "cylinder" );
	Field< Id >::set( nm, "cell", cell );
	unsigned int ns = Field< unsigned int >::get( nm, "numSegments" );
	assert( ns == 1 );
	unsigned int ndc = Field< unsigned int >::get( nm, "numDiffCompts" );
	assert( ndc == numCompts );
	const vector< NeuroNode >& nodes =
			reinterpret_cast< NeuroMesh* >( nm.eref().data() )->
			getNodes();
	assert( nodes.size() == 2 ); // Self plus dummy parent.
	assert( nodes[0].children().size() == 0 );

	// Insert a molecule at first subdivision of soma. I use a dummy
	// matrix S rather than the one in the system.
	// Field< double >::set( ObjId( soma, 0 ), "nInit", 1.0e6 );
	vector< double > molNum( 1, 0 ); // We only have one pool
	// S[meshIndex][poolIndex]
	vector< vector< double > > S( ndc, molNum );
	S[0][0] = totNum;
	vector< double > diffConst( 1, D );
	vector< double > temp( 1, 0.0 );
	vector< vector< double > > flux( ndc, temp );

	MeshCompt* mc = reinterpret_cast< MeshCompt* >( nm.eref().data() );
	assert( mc->getNumEntries() == numCompts );
	const double adx = dia * dia * PI * 0.25 / diffLength;
	for ( unsigned int i = 0; i < numCompts; ++i ) {
		const double* entry;
		const unsigned int* colIndex;
		unsigned int numAbut =  mc->getStencilRow( i, &entry, &colIndex );
		if ( i == 0 ) {
			assert( numAbut == 1 );
			assert( doubleEq( entry[0], adx ) );
			assert( colIndex[0] == 1 );
		} else if ( i == numCompts - 1 ) {
			assert( numAbut == 1 );
			assert( doubleEq( entry[0], adx ) );
			assert( colIndex[0] == numCompts - 2  );
		} else {
			assert( numAbut == 2 );
			assert( doubleEq( entry[0], adx ) );
			assert( colIndex[0] == i - 1 );
			assert( doubleEq( entry[1], adx ) );
			assert( colIndex[1] == i + 1 );
		}
	}

	///////////////////////////////////////////////////////////////////
	// Test the 'nearest' function
	unsigned int index;
	double near = mc->nearest( diffLength * 27.5, diffLength / 10.0, 0, index );
	assert( index == 27 );
	assert( doubleEq( near, diffLength / 10.0 ) );

	near = mc->nearest( -10, 0, 0, index );
	assert( index == 0 );
	assert( doubleEq( near, -1.0 ) );

	// Test indexToSpace
	double x, y, z;
	mc->indexToSpace( 27, x, y, z );
	assert( doubleEq( x, diffLength * 27.5 ) );
	assert( doubleEq( y, 0.0 ) );
	assert( doubleEq( z, 0.0 ) );

	///////////////////////////////////////////////////////////////////
	shell->doDelete( cell );
	shell->doDelete( nm );
	cout << "." << flush;
}

void testNeuroMeshBranching()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// Build a cell
	Id cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
	double len = 10e-6; // metres
	double dia = 1e-6; // metres
	double diffLength = 1e-6; // metres
	double D = 4e-12; // Diff const, m^2/s
	double totNum = 1e6; // Molecules
	double maxt = 10.0;
	double dt = 0.001;

	pair< unsigned int, unsigned int > ret =
			buildBranchingCell( cell, len, dia );

	// Scan it with neuroMesh and check outcome.
	Id nm = shell->doCreate( "NeuroMesh", Id(), "neuromesh", 1 );
	Field< double >::set( nm, "diffLength", diffLength );
	Field< string >::set( nm, "geometryPolicy", "cylinder" );
	Field< Id >::set( nm, "cell", cell );
	unsigned int ns = Field< unsigned int >::get( nm, "numSegments" );
	assert( ns == ret.first );
	unsigned int ndc = Field< unsigned int >::get( nm, "numDiffCompts" );
	assert( ndc == ret.second );
	NeuroMesh* neuro = reinterpret_cast< NeuroMesh* >( nm.eref().data() );
	const vector< NeuroNode >& nodes = neuro-> getNodes();
	assert( nodes.size() == ns + 15 ); // 15 dummy nodes.
	// We have depth-first ordering in the nodes vector.
    /**
     * soma---->d1---->d1a---->d11---->d111
     *  |               |       +----->d112
     *  |               |
     *  |               +----->d12---->d121
     *  |                       +----->d122
     *  |
     *  +------>d2----->d2a---->d21---->d211
     *                  |        +----->d212
     *                  |
     *                  +------>d22---->d221
     *                           +----->d222
     *
     */
	assert( nodes[0].children().size() == 2 );	// soma
	assert( nodes[1].children().size() == 1 );	// d1
	assert( nodes[2].children().size() == 2 );	// d1a
	assert( nodes[3].children().size() == 2 );	// d11
	assert( nodes[4].children().size() == 0 );	// d111
	assert( nodes[5].children().size() == 0 );	// d112
	assert( nodes[6].children().size() == 2 );	// d12
	assert( nodes[7].children().size() == 0 );	// d121
	assert( nodes[8].children().size() == 0 );	// d122

	assert( nodes[9].children().size() == 1 );	// d2
	assert( nodes[10].children().size() == 2 );	// d2a
	assert( nodes[11].children().size() == 2 );	// d21
	assert( nodes[12].children().size() == 0 );	// d211
	assert( nodes[13].children().size() == 0 );	// d212
	assert( nodes[14].children().size() == 2 );	// d22
	assert( nodes[15].children().size() == 0 );	// d221
	assert( nodes[16].children().size() == 0 );	// d222

	// Insert a molecule at soma
	vector< double > molNum( 1, 0 ); // We only have one pool
	// S[meshIndex][poolIndex]
	vector< double > S( ndc, 0.0 );
	S[0] = totNum;
	vector< double > flux( ndc, 0.0 );
	vector< double > vol( ndc, 0.0 );
	for ( unsigned int i = 0; i < ndc; ++i )
		vol[i] = neuro->getMeshEntryVolume( i );

	assert( doubleEq( vol[0], dia * dia * 0.25 * PI * diffLength ) );
	assert( doubleEq( vol[1], dia * dia * 0.125 * PI * diffLength ) );
	// Watch diffusion using stencil and direct calls to the flux
	// calculations rather than going through the ksolve.
	for ( double t = 0; t < maxt; t += dt ) {
		flux.assign( ndc, 0.0 );

		for ( unsigned int i = 0; i < ndc; ++i ) {
			const double* entry;
			const unsigned int* colIndex;
			unsigned int numEntries =
					neuro->getStencilRow( i, &entry, &colIndex);
			for ( unsigned int j = 0; j < numEntries; ++j ) {
				unsigned int k = colIndex[j];
				double delta = ( S[k]/vol[k] - S[i]/vol[i] ) * entry[j];
				flux[ k ] -= delta;
				// flux[ i ] += delta; // Am I double counting?
			}
		}
		for ( unsigned int i = 0; i < ndc; ++i )
			S[i] += flux[i] * D * dt;
	}
	double tot = 0.0;
	for ( unsigned int i = 0; i < ndc; ++i ) {
			tot += S[i];
	}

	// Compare with analytic solution.
	assert( doubleEq( tot, totNum ) );
	double x = 1.5 * diffLength;
	for ( unsigned int i = 1; i < 11; ++i ) { // First segment of tree.
		double y = totNum * diffusionFunction( D, diffLength, x, maxt );
		// cout << "S[" << i << "] = " << S[i] << "	" << y << endl;
		assert( doubleApprox( y, S[i] ) );
		x += diffLength;
	}
	tot = 0;
	for ( double x = diffLength / 2.0 ; x < 10 * len; x += diffLength ) {
			// the 2x is needed because we add up only half of the 2-sided
			// diffusion distribution.
		double y = 2 * totNum * diffusionFunction( D, diffLength, x, maxt);
		// cout << floor( x / diffLength ) << ": " << y << endl;
		tot += y;
	}
	assert( doubleEq( tot, totNum ) );

	///////////////////////////////////////////////////////////////////
	// Test the 'nearest' function
	MeshCompt* mc = reinterpret_cast< MeshCompt* >( nm.eref().data() );
	assert( mc->getNumEntries() == ndc );
	unsigned int index;
	double y, z;
	mc->indexToSpace( 60, x, y, z );

	double near = mc->nearest( x, y, z + diffLength/10.0, index );
	assert( index == 60 );
	assert( doubleEq( near, diffLength / 10.0 ) );

	near = mc->nearest( -10, 0, 0, index );
	assert( index == 0 );
	assert( doubleEq( near, -1.0 ) );

	vector< NeuroNode > nn = dynamic_cast< NeuroMesh* >( mc )->getNodes();
	assert( nn.size() == 32 ); // 17 plus 15 dummies
	/*
	for ( unsigned int i = 0; i < nn.size(); ++i ) {
		if ( !nn[i].isDummyNode() ) {
			cout << i << "	(" <<
				nn[i].getX() << ", " << nn[i].getY() << ", " <<
				nn[i].getZ() << ") on " <<
				nn[i].elecCompt().element()->getName() <<
				"\n";
		}
	}
	*/

	mc->indexToSpace( 72, x, y, z );
	near = mc->nearest( x, y, z, index );
	assert( index == 72 );
	assert( doubleEq( near, 0 ) );

	for( unsigned int i = 0; i < ndc; ++i ) {
		mc->indexToSpace( i, x, y, z );
		near = mc->nearest( x, y, z + diffLength/10.0, index );
		assert( index == i );
		assert( near == diffLength / 10.0 );
	}

	///////////////////////////////////////////////////////////////////
	// Test the area calculation

	CubeMesh cube;
	cube.setPreserveNumEntries( 0 );
	vector< double > coords( 9, 0.0 );
	coords[0] = -10; // X0
	coords[1] = -10; // Y0
	coords[2] = -10; // Z0

	coords[3] = 10; // X1
	coords[4] = 10; // Y1
	coords[5] = 10; // Z1
	coords[6] = 20; // DX
	coords[7] = 20; // DY
	coords[8] = 20; // DZ
	cube.innerSetCoords( coords );
	vector< VoxelJunction > vj;
	mc->matchMeshEntries( &cube, vj );
	assert( vj.size() == mc->innerGetNumEntries() );
	double area = 0.0;
	for ( unsigned int i = 0; i < vj.size(); ++i ) {
		assert( vj[i].first == i );
		assert( vj[i].second == 0 ); // Only one cube entry
		area += vj[i].diffScale;
		/*
		double r = cm.getR0() +
				( cm.getR1() - cm.getR0() ) *
				cm.getDiffLength() * ( 0.5 + i ) / cm.getTotLength();
		double a = r * cm.getDiffLength() * 2 * PI;
		//assert( doubleApprox( vj[i].diffScale, a ) );
		// cout << i << ". mesh: " << vj[i].diffScale << ", calc: " << a << endl;
		assert( fabs( vj[i].diffScale - a ) < 0.5 );
		*/
	}
	double a2 = dia * dia * PI +
			4 * len * dia * PI / sqrt( 2.0 ) +
			4 * len * dia * PI / 2.0 +
			8 * len * dia * PI / (2.0 * sqrt( 2.0 ) );

	// Scaling is needed otherwise doubleApprox treats both as zero.
	assert( doubleApprox( area * 1e8, a2 * 1e8 ) );


	///////////////////////////////////////////////////////////////////

	shell->doDelete( cell );
	shell->doDelete( nm );
	cout << "." << flush;
}
#endif

// Assorted definitions from CubeMesh.cpp
static const unsigned int EMPTY = ~0;
static const unsigned int SURFACE = ~1;
static const unsigned int ABUT = ~2;
static const unsigned int MULTI = ~3;
typedef pair< unsigned int, unsigned int > PII;
extern void setIntersectVoxel(
		vector< PII >& intersect,
		unsigned int ix, unsigned int iy, unsigned int iz,
		unsigned int nx, unsigned int ny, unsigned int nz,
		unsigned int meshIndex );

extern void checkAbut(
		const vector< PII >& intersect,
		unsigned int ix, unsigned int iy, unsigned int iz,
		unsigned int nx, unsigned int ny, unsigned int nz,
		unsigned int meshIndex,
		vector< VoxelJunction >& ret );

void testIntersectVoxel()
{
		/**
		 * Here is the geometry of the surface. * is surface, - is empty.
		 *
		 *			-***-
		 *			-*---
		 *			-***-
		 *
		 *
		 *			x***x
		 *			x*32-
		 *			x***x
		 *
		 * 	x is ABUTX
		 * 	y is ABUTY
		 * 	z is ABUTZ
		 * 	2 is 2 points
		 * 	3 is MULTI
		 *
		 */


	unsigned int nx = 5;
	unsigned int ny = 3;
	unsigned int nz = 1;
	vector< PII > intersect( nx * ny * nz, PII(
							CubeMesh::EMPTY, CubeMesh::EMPTY ) );
	unsigned int meshIndex = 0;
	setIntersectVoxel( intersect, 1, 0, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 2, 0, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 3, 0, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 1, 1, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 1, 2, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 2, 2, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 3, 2, 0, nx, ny, nz, meshIndex++ );

	assert( intersect[0].first == 0 &&
					intersect[0].second == CubeMesh::ABUTX );
	assert( intersect[1].first == 0 &&
					intersect[1].second == CubeMesh::SURFACE );
	assert( intersect[2].first == 1 &&
					intersect[2].second == CubeMesh::SURFACE );
	assert( intersect[3].first == 2 &&
					intersect[3].second == CubeMesh::SURFACE );
	assert( intersect[4].first == 2 &&
					intersect[4].second == CubeMesh::ABUTX );

	assert( intersect[5].first == 3 &&
					intersect[5].second == CubeMesh::ABUTX );
	assert( intersect[6].first == 3 &&
					intersect[6].second == CubeMesh::SURFACE );
	assert( intersect[7].first == 1 &&
					intersect[7].second == CubeMesh::MULTI );
	assert( intersect[8].first == 2 &&
					intersect[8].second == CubeMesh::MULTI );
	assert( intersect[9].first == EMPTY &&
					intersect[9].second == CubeMesh::EMPTY );

	assert( intersect[10].first == 4 &&
					intersect[10].second == CubeMesh::ABUTX );
	assert( intersect[11].first == 4 &&
					intersect[11].second == CubeMesh::SURFACE );
	assert( intersect[12].first == 5 &&
					intersect[12].second == CubeMesh::SURFACE );
	assert( intersect[13].first == 6 &&
					intersect[13].second == CubeMesh::SURFACE );
	assert( intersect[14].first == 6 &&
					intersect[14].second == CubeMesh::ABUTX );

	// Next: test out checkAbut.
	vector< VoxelJunction > ret;
	checkAbut( intersect, 0, 0, 0, nx, ny, nz, 1234, ret );
	assert( ret.size() == 1 );
	assert( ret[0].first == 0 && ret[0].second == 1234 );
	ret.clear();
	// The ones below are either SURFACE or EMPTY and should not add points
	checkAbut( intersect, 1, 0, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 2, 0, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 3, 0, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 1, 1, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 4, 1, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 1, 2, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 2, 2, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 3, 2, 0, nx, ny, nz, 1234, ret );
	assert( ret.size() == 0 );
	checkAbut( intersect, 2, 1, 0, nx, ny, nz, 9999, ret );
	assert( ret.size() == 3 );
	assert( ret[0].first == 3 && ret[0].second == 9999 );
	assert( ret[1].first == 1 && ret[1].second == 9999 );
	assert( ret[2].first == 5 && ret[1].second == 9999 );
	ret.clear();
	checkAbut( intersect, 3, 1, 0, nx, ny, nz, 8888, ret );
	assert( ret.size() == 2 );
	assert( ret[0].first == 2 && ret[0].second == 8888 );
	assert( ret[1].first == 6 && ret[1].second == 8888 );
	ret.clear();
	checkAbut( intersect, 4, 0, 0, nx, ny, nz, 7777, ret );
	checkAbut( intersect, 0, 1, 0, nx, ny, nz, 6666, ret );
	checkAbut( intersect, 0, 2, 0, nx, ny, nz, 5555, ret );
	checkAbut( intersect, 4, 2, 0, nx, ny, nz, 4444, ret );
	assert( ret.size() == 4 );
	assert( ret[0].first == 2 && ret[0].second == 7777 );
	assert( ret[1].first == 3 && ret[1].second == 6666 );
	assert( ret[2].first == 4 && ret[2].second == 5555 );
	assert( ret[3].first == 6 && ret[3].second == 4444 );

	cout << "." << flush;
}

void testCubeMeshFillTwoDimSurface()
{
	CubeMesh cm;
	vector< double > coords( 9, 0.0 );
	coords[3] = 5.0;
	coords[4] = 3.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm.setPreserveNumEntries( false );
	cm.innerSetCoords( coords );
	assert( cm.numDims() == 2 );
	const vector< unsigned int >& surface = cm.surface();
	assert( surface.size() == 15 );
	for ( unsigned int i = 0; i < 15; ++i ) {
		assert( surface[i] == i );
	}
	cout << "." << flush;
}

void testCubeMeshFillThreeDimSurface()
{
	cout << "." << flush;
}

void testCubeMeshJunctionTwoDimSurface()
{
		/**
		 * 						8	9
		 * 10	11	12	13	14	6	7
		 * 5	6	7	8	9	4	5
		 * 0	1	2	3	4	2	3
		 * 						0	1
		 *
		 * So, junction should be (4,2),(9,4),(14,6)
		 */
	CubeMesh cm1;
	vector< double > coords( 9, 0.0 );
	coords[3] = 5.0;
	coords[4] = 3.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm1.setPreserveNumEntries( false );
	cm1.innerSetCoords( coords );
	vector< unsigned int > surface = cm1.surface();
	assert( surface.size() == 15 );

	CubeMesh cm2;
	coords[0] = 5.0;
	coords[1] = -1.0;
	coords[2] = 0.0;
	coords[3] = 7.0;
	coords[4] = 4.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm2.setPreserveNumEntries( false );
	cm2.innerSetCoords( coords );
	const vector< unsigned int >& surface2 = cm2.surface();
	assert( surface2.size() == 10 );

	vector< VoxelJunction > ret;
	cm1.matchCubeMeshEntries( &cm2, ret );
	assert( ret.size() == 3 );

	assert( ret[0].first == 4 );
	assert( ret[0].second == 2 );
	assert( ret[1].first == 9 );
	assert( ret[1].second == 4 );
	assert( ret[2].first == 14 );
	assert( ret[2].second == 6 );

	/**
	 * That was too easy, since the spatial and meshIndices were
	 * identical. Now trim the geometries a bit to look like:
		 * 						6	7
		 * 10	11	12	13	-	4	5
		 * 5	6	7	8	9	2	3
		 * 0	1	2	3	4	-	1
		 * 						-	0
		 *
		 * So, junction should be (9,2) only.
		 */

	// Trimming cm1. At this point we don't assume automatic updates of
	// the m2s, s2m and surface vectors when any of them is changed.
	vector< unsigned int > m2s = cm1.getMeshToSpace();
	assert( m2s.size() == 15 );
	m2s.resize( 14 );
	cm1.setMeshToSpace( m2s );
	vector< unsigned int > s2m = cm1.getSpaceToMesh();
	assert( s2m.size() == 15 );
	s2m[14] = ~0;
	cm1.setSpaceToMesh( s2m );
	surface.resize( 4 );
	// As a shortcut, just assign the places near the junction
	// Note that the indices are spaceIndices.
	surface[0] = 3;
	surface[1] = 4;
	surface[2] = 9;
	surface[3] = 13;
	cm1.setSurface( surface );

	// Trimming cm2.
	m2s = cm2.getMeshToSpace();
	assert( m2s.size() == 10 );
	m2s.resize( 8 );
	m2s[0] = 1;
	for ( unsigned int i = 1; i < 8; ++i )
		m2s[i] = i + 2;
	cm2.setMeshToSpace( m2s );
	s2m.clear();
	s2m.resize( 10, ~0 );
	for ( unsigned int i = 0; i < 8; ++i )
		s2m[ m2s[i] ] = i;
	cm2.setSpaceToMesh( s2m );
	// As a shortcut, just assign the places near the junction
	// Note that the indices are spaceIndices.
	surface[0] = 3;
	surface[1] = 4;
	surface[2] = 6;
	surface[3] = 8;
	cm2.setSurface( surface );

	// Now test it out.
	ret.resize( 0 );
	cm1.matchCubeMeshEntries( &cm2, ret );
	assert( ret.size() == 1 );
	assert( ret[0].first == 9 );
	assert( ret[0].second == 2 );

	cout << "." << flush;
}

void testCubeMeshJunctionDiffSizeMesh()
{
		/**
		 * 						14	15
		 * 						12	13
		 * 10	11	12	13	14	10	11
		 *						8	9
		 * 5	6	7	8	9	6	7
		 *						4	5
		 * 0	1	2	3	4	2	3
		 * 						0	1
		 *
		 * So, junction should be (4,2)(4,4),(9,6),(9,8),(14,10),(14,12)
		 */
	CubeMesh cm1;
	vector< double > coords( 9, 0.0 );
	coords[3] = 5.0;
	coords[4] = 3.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm1.setPreserveNumEntries( false );
	cm1.innerSetCoords( coords );
	vector< unsigned int > surface = cm1.surface();
	assert( surface.size() == 15 );

	CubeMesh cm2;
	coords[0] = 5.0;
	coords[1] = -0.5;
	coords[2] = 0.0;
	coords[3] = 7.0;
	coords[4] = 3.5;
	coords[5] = 0.5;
	coords[6] = 1.0;
	coords[7] = 0.5;
   	coords[8] = 0.5;
	cm2.setPreserveNumEntries( false );
	cm2.innerSetCoords( coords );
	const vector< unsigned int >& surface2 = cm2.surface();
	assert( surface2.size() == 16 );

	vector< VoxelJunction > ret;
	cm1.matchCubeMeshEntries( &cm2, ret );
	assert( ret.size() == 6 );

	assert( ret[0].first == 4 );
	assert( ret[0].second == 2 );
	assert( ret[1].first == 4 );
	assert( ret[1].second == 4 );
	assert( ret[2].first == 9 );
	assert( ret[2].second == 6 );
	assert( ret[3].first == 9 );
	assert( ret[3].second == 8 );
	assert( ret[4].first == 14 );
	assert( ret[4].second == 10 );
	assert( ret[5].first == 14 );
	assert( ret[5].second == 12 );

	cout << "." << flush;
}

void testCubeMeshJunctionThreeDimSurface()
{
	cout << "." << flush;
}

/**
 * The simulated geometry is:
 *                    D
 *                    D
 *                 BBBA
 *                 CCCCC
 *  Here, A is at (0,0,0) to (10,10,10) microns.
 *  B is then (-30,0,0) to (0,10,10) microns
 *  C is (-30,-10,0) to (20,0,10) microns
 *  D is (0,10,0) to (10,30,10) microns.
 */
void testCubeMeshMultiJunctionTwoD()
{
	CubeMesh A;
	vector< double > coords( 9, 0.0 );
	coords[3] = 10e-6;
	coords[4] = 10e-6;
	coords[5] = 10e-6;
	coords[6] = coords[7] = coords[8] = 10e-6;
	A.setPreserveNumEntries( false );
	A.innerSetCoords( coords );
	vector< unsigned int > surface = A.surface();
	assert( surface.size() == 1 );
	assert( surface[0] == 0 );

	CubeMesh B;
	coords[0] = -30e-6;	coords[1] = 0; 		coords[2] = 0;
	coords[3] = 0; 		coords[4] = 10e-6;	coords[5] = 10e-6;
	coords[6] = 		coords[7] = 		coords[8] = 10e-6;
	B.setPreserveNumEntries( false );
	B.innerSetCoords( coords );
	surface = B.surface();
	assert( surface.size() == 3 );
	assert( surface[0] == 0 );
	assert( surface[1] == 1 );
	assert( surface[2] == 2 );

	CubeMesh D;
	coords[0] = 0;		coords[1] = 10e-6;	coords[2] = 0;
	coords[3] = 10e-6; 	coords[4] = 30e-6;	coords[5] = 10e-6;
	coords[6] = 		coords[7] = 		coords[8] = 10e-6;
	D.setPreserveNumEntries( false );
	D.innerSetCoords( coords );
	surface = D.surface();
	assert( surface.size() == 2 );
	assert( surface[0] == 0 );
	assert( surface[1] == 1 );

	CubeMesh C;
	coords[0] = -30e-6;	coords[1] = -10e-6;	coords[2] = 0;
	coords[3] = 20e-6;	coords[4] = 0;		coords[5] = 10e-6;
	coords[6] = 		coords[7] = 		coords[8] = 10e-6;
	C.setPreserveNumEntries( false );
	C.innerSetCoords( coords );
	surface = C.surface();
	assert( surface.size() == 5 );
	assert( surface[0] == 0 );
	assert( surface[1] == 1 );
	assert( surface[2] == 2 );
	assert( surface[3] == 3 );
	assert( surface[4] == 4 );


	cout << "." << flush;
}

void testVec()
{
	Vec i( 1, 0, 0 );
	Vec j( 0, 1, 0 );
	Vec k( 0, 0, 1 );

	assert( doubleEq( i.dotProduct( j ), 0.0 ) );
	assert( doubleEq( j.dotProduct( k ), 0.0 ) );
	assert( doubleEq( j.dotProduct( j ), 1.0 ) );

	assert( i.crossProduct( j ) == k );

	Vec u, v;
	i.orthogonalAxes( u, v );
	assert( u == j );
	assert( v == k );

	cout << "." << flush;
}

#if 0
void testSpineEntry()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// Build a cell
	Id cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
// Id makeCompt( Id parentCompt, Id parentObj, string name, double len, double dia, double theta )
	Id neck = makeCompt( Id(), cell, "neck", 1e-6, 1e-7, 0 );
	Id head = makeCompt( neck, cell, "head", 1e-6, 1e-6, 0 );

	SpineEntry se( neck, head, 1234 );

	assert( se.parent() == 1234 );
	assert( se.shaftId() == neck );
	assert( se.headId() == head );
	assert( doubleEq( se.volume(), 1e-18 * PI/4.0  ) );
	double x, y, z;
	se.mid( x, y, z );
	assert( doubleEq( x, 1.5e-6 ) );
	assert( doubleEq( y, 0.0 ) );
	assert( doubleEq( z, 0.0 ) );

	//////////////////////////////////////////////////
	// Test matchCubeMeshEntries.
	vector< VoxelJunction > vj;
	CubeMesh cube;
	cube.setPreserveNumEntries( 0 );
	vector< double > coords( 9, 0.0 );
	coords[0] = -10; // X0
	coords[1] = -10; // Y0
	coords[2] = -10; // Z0

	coords[3] = 10; // X1
	coords[4] = 10; // Y1
	coords[5] = 10; // Z1
	coords[6] = 20; // DX
	coords[7] = 20; // DY
	coords[8] = 20; // DZ
	cube.innerSetCoords( coords );
	se.matchCubeMeshEntriesToHead( &cube, 1234, 0.1, vj );
	assert( vj.size() == 1 );
	assert( vj[0].first == 1234 );
	assert( vj[0].second == 0 );
	assert( doubleApprox( vj[0].diffScale * 1e10, 1e-12 * PI * 1e10 ) );

	vj.clear();
	se.matchCubeMeshEntriesToPSD( &cube, 4321, 0.1, vj );
	assert( vj.size() == 1 );
	assert( vj[0].first == 4321 );
	assert( vj[0].second == 0 );
	assert( doubleApprox( vj[0].diffScale * 1e10, 1e-12 * PI * 0.25 * 1e10 ) );

	/////////////////////////////////////////////////////////
	shell->doDelete( cell );
	cout << "." << flush;
}

void testSpineAndPsdMesh()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// Build a linear cylindrical cell, no tapering.
	Id cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
	unsigned int numCompts = 500;
	double dia = 20e-6; // metres
	double diffLength = 0.5e-6; // metres
	double len = diffLength * numCompts;
	unsigned int numSpines = 10;
	Id soma = makeCompt( Id(), cell, "soma", dia, dia, 90 );
	Field< double >::set( soma, "y0", -dia );
	Field< double >::set( soma, "y", 0 );
	Id dend = makeCompt( soma, cell, "dend", len, dia, 0 );

	for ( unsigned int i = 0; i < numSpines; ++i ) {
		double frac = i / static_cast< double >( numSpines );
		makeSpine( dend, cell, i, frac, 1.0e-6, 1.0e-6, i * 30.0 );
	}

	Id nm = shell->doCreate( "NeuroMesh", Id(), "neuromesh", 1 );
	Field< bool >::set( nm, "separateSpines", true );
	Field< double >::set( nm, "diffLength", diffLength );
	Field< string >::set( nm, "geometryPolicy", "cylinder" );
	Id sm = shell->doCreate( "SpineMesh", Id(), "spinemesh", 1 );
	ObjId mid = shell->doAddMsg(
					"OneToOne", nm, "spineListOut", sm, "spineList" );
	assert( !mid.bad() );
	Id pm = shell->doCreate( "PsdMesh", Id(), "psdmesh", 1 );
	mid = shell->doAddMsg(
					"OneToOne", nm, "psdListOut", pm, "psdList" );
	assert( !mid.bad() );
	Field< Id >::set( nm, "cell", cell );
	unsigned int ns = Field< unsigned int >::get( nm, "numSegments" );
	assert( ns == 2 ); // soma and dend
	unsigned int ndc = Field< unsigned int >::get( nm, "numDiffCompts" );
	assert( ndc == numCompts + floor( dia / diffLength + 0.5 ) );

	unsigned int sdc = Field< unsigned int >::get( sm, "num_mesh" );
	assert( sdc == numSpines );

	// Should now iterate through the spines to find their 'nearest' on
	// the dendrite.
	SpineMesh* s = reinterpret_cast< SpineMesh* >( sm.eref().data() );
	for ( unsigned int i = 0; i < sdc; ++i ) {
		assert( s->spines()[i].parent() == 40 + i * numCompts/numSpines );
		assert( doubleEq( s->spines()[i].volume(), PI * 0.25e-12 ) );
		double x = i * diffLength * numCompts/numSpines;
		unsigned int index = 0;
		s->nearest( x, 0, 0, index );
		assert( index == i );
	}

	// Check coupling to Neuromesh.
	vector< VoxelJunction > ret;
	NeuroMesh* nmesh = reinterpret_cast< NeuroMesh* >( nm.eref().data() );
	s->matchMeshEntries( nmesh, ret );
	assert( ret.size() == numSpines );
	for ( unsigned int i = 0; i < numSpines; ++i ) {
		assert( ret[i].first == i );
		assert( ret[i].second == s->spines()[i].parent() );
		assert( doubleEq( ret[i].diffScale, PI * 0.25e-14 / 1.5e-6 ) );
	}

	// Check coupling to CubeMesh.
	CubeMesh cube;
	cube.setPreserveNumEntries( 0 );
	vector< double > coords( 9, 0.0 );
	coords[0] = -len/2.0 - 1.2e-5; // X0
	coords[1] = -1e-5; // Y0
	coords[2] = -1e-5; // Z0

	coords[3] = len * 2.0 - 1.2e-5; // X1
	coords[4] = 1.5e-5; // Y1
	coords[5] = 1.5e-5; // Z1
	coords[6] = 2.5e-5; // DX
	coords[7] = 2.5e-5; // DY
	coords[8] = 2.5e-5; // DZ
	cube.innerSetCoords( coords );
	ret.clear();
	s->matchMeshEntries( &cube, ret );
	assert( ret.size() == numSpines );
	for ( unsigned int i = 0; i < ret.size(); ++i ) {
		assert( ret[i].first == i );
		unsigned int cubeIndex = i + 5;
		assert( ret[i].second == cubeIndex );
		assert( doubleApprox( ret[i].diffScale * 1e10, PI * 1e-12 * 1e10 ) );
		/*
		cout << i << " cubeIndex=" << cubeIndex << ", (" <<
				ret[i].first << ", " << ret[i].second << ") : " <<
				ret[i].diffScale << "\n";
				*/
	}
	////////////////////////////////////////////////////////////
	// Check PSD mesh
	unsigned int pdc = Field< unsigned int >::get( pm, "num_mesh" );
	Id pe( pm.value() + 1 );
	assert( pdc == numSpines );
	PsdMesh* p = reinterpret_cast< PsdMesh* >( pm.eref().data() );
	for ( unsigned int i = 0; i < pdc; ++i ) {
		assert( p->parent( i ) == i );
		ObjId oi( pe, i );
		double area = Field< double >::get( oi, "volume" );
		assert( doubleEq( area, 1e-12 * 0.25 * PI ) );
		unsigned int dim = Field< unsigned int >::get( oi, "dimensions" );
		assert( dim == 2 );
		double x = i * diffLength * numCompts/numSpines;
		unsigned int index = 0;
		s->nearest( x, 0, 0, index );
		assert( index == i );
	}
	ret.clear();
	p->matchMeshEntries( s, ret );
	assert( ret.size() == numSpines );
	for ( unsigned int i = 0; i < ret.size(); ++i ) {
		assert( ret[i].first == i );
		assert( ret[i].second == i );
		assert( doubleApprox( ret[i].diffScale * 1e10,
								1e10 * 0.25 * PI * 1e-12 / 0.5e-6 ) );
	}
	ret.clear();
	p->matchMeshEntries( &cube, ret );
	assert( ret.size() == numSpines );
	for ( unsigned int i = 0; i < ret.size(); ++i ) {
		assert( ret[i].first == i );
		unsigned int cubeIndex = i + 5;
		assert( ret[i].second == cubeIndex );
		assert( doubleApprox( ret[i].diffScale * 1e10, 0.25 * PI * 1e-12 * 1e10 ) );
	}


	shell->doDelete( cell );
	shell->doDelete( nm );
	shell->doDelete( sm );
	shell->doDelete( pm );
	cout << "." << flush;
}

#include "../shell/Wildcard.h"
void testNeuroNodeTree()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// Build a cell
	Id cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
	double len = 10e-6; // metres
	double dia = 1e-6; // metres

	buildBranchingCell( cell, len, dia );

	vector< NeuroNode > nodes;
	vector< Id > elist;
	wildcardFind( "/cell/##", elist );
	NeuroNode::buildTree( nodes, elist );
	assert( nodes.size() == 17 );
	unsigned int i = 0;
	// Here we want the function to generate a depth-first tree.
	assert( nodes[i++].elecCompt().element()->getName() == "soma" );
	assert( nodes[i++].elecCompt().element()->getName() == "d1" );
	assert( nodes[i++].elecCompt().element()->getName() == "d1a" );
	assert( nodes[i++].elecCompt().element()->getName() == "d11" );
	assert( nodes[i++].elecCompt().element()->getName() == "d111" );
	assert( nodes[i++].elecCompt().element()->getName() == "d112" );
	assert( nodes[i++].elecCompt().element()->getName() == "d12" );
	assert( nodes[i++].elecCompt().element()->getName() == "d121" );
	assert( nodes[i++].elecCompt().element()->getName() == "d122" );

	assert( nodes[i++].elecCompt().element()->getName() == "d2" );
	assert( nodes[i++].elecCompt().element()->getName() == "d2a" );
	assert( nodes[i++].elecCompt().element()->getName() == "d21" );
	assert( nodes[i++].elecCompt().element()->getName() == "d211" );
	assert( nodes[i++].elecCompt().element()->getName() == "d212" );
	assert( nodes[i++].elecCompt().element()->getName() == "d22" );
	assert( nodes[i++].elecCompt().element()->getName() == "d221" );
	assert( nodes[i++].elecCompt().element()->getName() == "d222" );

	/*
	Id soma = makeCompt( Id(), cell, "soma", dia, dia, 90 );
	Id dend = makeCompt( Id(), cell, "dend", len, dia, 0 );
	for ( unsigned int i = 0; i < numSpines; ++i ) {
		double frac = i / static_cast< double >( numSpines );
		makeSpine( dend, cell, i, frac, 1.0e-6, 1.0e-6, i * 30.0 );
	}
	*/
	//////////////////////////////////////////////////////////////////////
	// Now to make a totally random cell, with a complete mash of
	// messaging, having circular linkage even.
	//////////////////////////////////////////////////////////////////////
	shell->doDelete( cell );
	cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
	vector< Id > compt( 10 );
	for ( unsigned int i = 0; i < compt.size(); ++i ) {
		stringstream ss;
		ss << "compt" << i;
		string name = ss.str();
		compt[i] = shell->doCreate( "SymCompartment", cell, name, 1 );
		double dia = 50.0 - (i - 3) * (i - 3 );
		Field< double >::set( compt[i], "diameter", dia );
	}
	shell->doAddMsg( "Single", compt[0], "proximal", compt[1], "distal" );
	shell->doAddMsg( "Single", compt[1], "proximal", compt[2], "distal" );
	shell->doAddMsg( "Single", compt[2], "distal", compt[3], "proximal" );
	shell->doAddMsg( "Single", compt[3], "sibling", compt[2], "sibling" );
	shell->doAddMsg( "Single", compt[3], "sphere", compt[4], "proximalOnly" );
	shell->doAddMsg( "Single", compt[4], "distal", compt[5], "proximal" );
	shell->doAddMsg( "Single", compt[4], "distal", compt[6], "proximal" );
	shell->doAddMsg( "Single", compt[4], "axial", compt[7], "raxial" );
	shell->doAddMsg( "Single", compt[7], "cylinder", compt[8], "proximalOnly" );
	shell->doAddMsg( "Single", compt[7], "cylinder", compt[9], "proximalOnly" );
	shell->doAddMsg( "Single", compt[9], "proximal", compt[0], "distal" );

	nodes.clear();
	elist.clear();
	wildcardFind( "/cell/##", elist );
	NeuroNode::buildTree( nodes, elist );
	assert( nodes.size() == 10 );
	i = 0;
	// The fallback for the soma is the compt with biggest diameter.
	assert( nodes[i++].elecCompt().element()->getName() == "compt3" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt2" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt1" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt0" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt9" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt7" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt8" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt4" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt5" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt6" );

	// Now I change the biggest compartment to compartment 5.
	Field< double >::set( compt[5], "diameter", 51.0 );
	nodes.clear();
	elist.clear();
	wildcardFind( "/cell/##", elist );
	NeuroNode::buildTree( nodes, elist );
	assert( nodes.size() == 10 );
	i = 0;
	assert( nodes[i++].elecCompt().element()->getName() == "compt5" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt4" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt3" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt2" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt1" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt0" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt9" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt7" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt8" );
	assert( nodes[i++].elecCompt().element()->getName() == "compt6" );
	//////////////////////////////////////////////////////////////////////
	// Now to make a dendrite with lots of spines and filter them off.
	//////////////////////////////////////////////////////////////////////
	shell->doDelete( cell );
	cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
	vector< Id > dend( 10 );
	vector< Id > neck( 10 );
	vector< Id > head( 10 );
	for ( unsigned int i = 0; i < neck.size(); ++i ) {
		stringstream ss;
		ss << "dend" << i;
		string name = ss.str();
		dend[i] = shell->doCreate( "SymCompartment", cell, name, 1 );
		Field< double >::set( dend[i], "diameter", 2.0e-6 );
		stringstream ss1;
		ss1 << "neck" << i;
		name = ss1.str();
		neck[i] = shell->doCreate( "SymCompartment", cell, name, 1 );
		Field< double >::set( neck[i], "diameter", 0.2e-6 );
		stringstream ss2;
		ss2 << "head" << i;
		name = ss2.str();
		head[i] = shell->doCreate( "SymCompartment", cell, name, 1 );
		Field< double >::set( head[i], "diameter", 1.0e-6 );
	}
	for ( unsigned int i = 0; i < neck.size(); ++i ) {
	// Just to make things interesting, add the messages staggered.
		if ( i > 0 )
			shell->doAddMsg( "Single", dend[i], "proximal",
						dend[ (i-1)], "distal" );
		shell->doAddMsg( "Single", neck[i], "proximalOnly",
						dend[ (i+7) % head.size()], "cylinder" );
		shell->doAddMsg( "Single", neck[i], "distal",
						head[ (i+4) % neck.size() ], "proximal" );
	}
	nodes.clear();
	elist.clear();
	wildcardFind( "/cell/##", elist );
	NeuroNode::buildTree( nodes, elist );
	assert( nodes.size() == 30 );
	vector< NeuroNode > orig = nodes;
	vector< Id > shaftId;
	vector< Id > headId;
	vector< unsigned int > parent;
	NeuroNode::filterSpines( nodes, shaftId, headId, parent );
	assert( nodes.size() == 10 );
	/*
	cout << endl;
	for ( unsigned int i = 0; i < 10; ++i ) {
		cout << "nodes[" << i << "].pa = " << nodes[i].parent() <<
				", Id = " << nodes[i].elecCompt() << endl;
	}
	for ( unsigned int i = 0; i < 30; ++i ) {
		cout << "orig[" << i << "].pa = " << orig[i].parent() <<
				", Id = " << orig[i].elecCompt() << endl;
	}
	*/
	// Check that the correct dend is parent of each shaft, and
	// correct shaft is parent of each head.
	for ( unsigned int i = 0; i < parent.size(); ++i ) {
		unsigned int j = (shaftId[i].value() - neck[0].value())/3;
		assert( parent[i] == (j+7) % 10 );
		unsigned int k = (headId[i].value() - head[0].value())/3;
		assert( k == (j+4) % 10 );
	}

	shell->doDelete( cell );
	cout << "." << flush;
}

void testCellPortion()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// Build a linear cylindrical cell, no tapering.
	Id cell = shell->doCreate( "Neutral", Id(), "cell", 1 );
	unsigned int numCompts = 50;
	double dia = 20e-6; // metres
	double diffLength = 0.5e-6; // metres
	double len = diffLength * numCompts;
	unsigned int numSpines = 10;
	unsigned int numSpines2 = 25;
	Id soma = makeCompt( Id(), cell, "soma", dia, dia, 90 );
	Field< double >::set( soma, "y0", -dia );
	Field< double >::set( soma, "y", 0 );
	Id dend0 = makeCompt( soma, cell, "dend0", len, dia, 0 );
	Id dend1 = makeCompt( dend0, cell, "dend1", len, dia, 0 );
	Id dend2 = makeCompt( dend1, cell, "dend2", len, dia, 0 );

	for ( unsigned int i = 0; i < numSpines; ++i ) {
		double frac = i / static_cast< double >( numSpines );
		makeSpine( dend1, cell, i, frac, 1.0e-6, 1.0e-6, i * 30.0 );
	}
	for ( unsigned int i = 0; i < numSpines2; ++i ) {
		double frac = i / static_cast< double >( numSpines2 );
		makeSpine( dend2, cell, i + numSpines, frac, 1.0e-6, 1.0e-6, i * 30.0 );
	}

	Id nm = shell->doCreate( "NeuroMesh", Id(), "neuromesh", 1 );
	Field< bool >::set( nm, "separateSpines", true );
	Field< double >::set( nm, "diffLength", diffLength );
	Field< string >::set( nm, "geometryPolicy", "cylinder" );
	Id sm = shell->doCreate( "SpineMesh", Id(), "spinemesh", 1 );
	ObjId mid = shell->doAddMsg(
					"OneToOne", nm, "spineListOut", sm, "spineList" );
	assert( !mid.bad() );
	Id pm = shell->doCreate( "PsdMesh", Id(), "psdmesh", 1 );
	mid = shell->doAddMsg(
					"OneToOne", nm, "psdListOut", pm, "psdList" );
	assert( !mid.bad() );
	// SetGet2< Id, string >::set( nm, "cellPortion", cell, "/cell/dend2,/cell/shaft#,/cell/head#" );
	SetGet2< Id, string >::set( nm, "cellPortion", cell, "/cell/dend1,/cell/dend2,/cell/shaft1?,/cell/head1?,/cell/shaft3,cell/head3" );
	unsigned int ns = Field< unsigned int >::get( nm, "numSegments" );
	assert( ns == 2 ); // dend1 + dend2 only
	unsigned int ndc = Field< unsigned int >::get( nm, "numDiffCompts" );
	assert( ndc == numCompts * 2 );

	unsigned int sdc = Field< unsigned int >::get( sm, "num_mesh" );
	assert( sdc == 11 ); // I've selected only those in the teens plus #3

	shell->doDelete( cell );
	cout << "." << flush;
}
#endif

void testMesh()
{
	testVec();
	testVolScaling();
	// testCylBase();
	// testNeuroNode();
	// testNeuroNodeTree();
	// testCylMesh();
	// testMidLevelCylMesh();
	testCubeMesh();
	testCubeMeshExtendStencil();
	// testReMesh(); // Waiting to have pool subdivision propagate.
	// testNeuroMeshLinear();
	// testNeuroMeshBranching();
	// testIntersectVoxel();
	testCubeMeshFillTwoDimSurface();
	testCubeMeshFillThreeDimSurface();
	testCubeMeshJunctionTwoDimSurface();
	testCubeMeshJunctionThreeDimSurface();
	testCubeMeshJunctionDiffSizeMesh();
	testCubeMeshMultiJunctionTwoD();
	// testSpineEntry();
	// testSpineAndPsdMesh();
	// testCellPortion();
}
