/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/SparseMatrix.h"
#include "../basecode/ElementValueFinfo.h"
#include "../utility/Vec.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "ChemCompt.h"
#include "MeshCompt.h"
#include "CubeMesh.h"
#include "CylBase.h"
#include "NeuroNode.h"
#include "NeuroMesh.h"
#include "SpineEntry.h"
#include "SpineMesh.h"
#include "PsdMesh.h"
#include "../utility/numutil.h"

/*
static SrcFinfo3< Id, vector< double >, vector< unsigned int > >*
	psdListOut()
{
	static SrcFinfo3< Id, vector< double >, vector< unsigned int > >
   		psdListOut(
		"psdListOut",
		"Tells PsdMesh to build a mesh. "
		"Arguments: Coordinates of each psd, "
		"index of matching parent voxels for each spine"
		"The coordinates each have 8 entries:"
		"xyz of centre of psd, xyz of vector perpendicular to psd, "
		"psd diameter, "
		" diffusion distance from parent compartment to PSD"
	);
	return &psdListOut;
}
*/

const Cinfo* SpineMesh::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ReadOnlyValueFinfo< SpineMesh, vector< unsigned int > >
			parentVoxel
		(
		 	"parentVoxel",
			"Vector of indices of proximal voxels within this mesh."
			"Spines are at present modeled with just one compartment,"
			"so each entry in this vector is always set to EMPTY == -1U",
			&SpineMesh::getParentVoxel
		);

		static ReadOnlyValueFinfo< SpineMesh, vector< unsigned int > >
			neuronVoxel
		(
		 	"neuronVoxel",
			"Vector of indices of voxels on parent NeuroMesh, from which "
			"the respective spines emerge.",
			&SpineMesh::getNeuronVoxel
		);

		static ReadOnlyValueFinfo< SpineMesh, vector< Id > > elecComptMap(
			"elecComptMap",
			"Vector of Ids of electrical compartments that map to each "
			"voxel. This is necessary because the order of the IDs may "
			"differ from the ordering of the voxels. Note that there "
			"is always just one voxel per spine head. ",
			&SpineMesh::getElecComptMap
		);
		static ReadOnlyValueFinfo< SpineMesh, vector< Id > > elecComptList(
			"elecComptList",
			"Vector of Ids of all electrical compartments in this "
			"SpineMesh. Ordering is as per the tree structure built in "
			"the NeuroMesh, and may differ from Id order. Ordering "
			"matches that used for startVoxelInCompt and endVoxelInCompt",
			&SpineMesh::getElecComptMap
		);
		static ReadOnlyValueFinfo< SpineMesh, vector< unsigned int > > startVoxelInCompt(
			"startVoxelInCompt",
			"Index of first voxel that maps to each electrical "
			"compartment. This is a trivial function in the SpineMesh, as"
			"we have a single voxel per spine. So just a vector of "
			"its own indices.",
			&SpineMesh::getStartVoxelInCompt
		);
		static ReadOnlyValueFinfo< SpineMesh, vector< unsigned int > > endVoxelInCompt(
			"endVoxelInCompt",
			"Index of end voxel that maps to each electrical "
			"compartment. Since there is just one voxel per electrical "
			"compartment in the spine, this is just a vector of index+1",
			&SpineMesh::getEndVoxelInCompt
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////

		static DestFinfo spineList( "spineList",
			"Specifies the list of electrical compartments for the spine,"
			"and the associated parent voxel"
			"Arguments: shaft compartments, "
			"head compartments, parent voxel index ",
			new EpFunc3< SpineMesh, vector< Id >, vector< Id >,
		   	vector< unsigned int > >(
				&SpineMesh::handleSpineList )
		);

		//////////////////////////////////////////////////////////////
		// Field Elements
		//////////////////////////////////////////////////////////////

	static Finfo* spineMeshFinfos[] = {
		&parentVoxel,		// ReadOnlyValueFinfo
		&neuronVoxel,		// ReadOnlyValueFinfo
		&elecComptMap,		// ReadOnlyValueFinfo
		&elecComptList,		// ReadOnlyValueFinfo
		&startVoxelInCompt,		// ReadOnlyValueFinfo
		&endVoxelInCompt,		// ReadOnlyValueFinfo
		&spineList,			// DestFinfo
		// psdListOut(),		// SrcFinfo
	};

	static Dinfo< SpineMesh > dinfo;
	static Cinfo spineMeshCinfo (
		"SpineMesh",
		ChemCompt::initCinfo(),
		spineMeshFinfos,
		sizeof( spineMeshFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &spineMeshCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* spineMeshCinfo = SpineMesh::initCinfo();

//////////////////////////////////////////////////////////////////
// Class stuff.
//////////////////////////////////////////////////////////////////
SpineMesh::SpineMesh()
	:
		spines_( 1 ),
		surfaceGranularity_( 0.1 ),
		vs_( 1, 1.0e-18 ),
		area_( 1, 1.0e-12 ),
		length_( 1, 1.0e-6 )
{;}

SpineMesh::SpineMesh( const SpineMesh& other )
	:
		spines_( other.spines_ ),
		surfaceGranularity_( other.surfaceGranularity_ )
{;}

SpineMesh::~SpineMesh()
{
	;
}

//////////////////////////////////////////////////////////////////
// Field assignment stuff
//////////////////////////////////////////////////////////////////

/**
 * This function returns the diffusively connected parent voxel within
 * the current (spine) mesh. Since each spine is treated as an independed
 * voxel, there is no such voxel, so we return -1U for each spine.
 * Note that there is a separate function that returns the parentVoxel
 * referred to the NeuroMesh that this spine sits on.
 */
vector< unsigned int > SpineMesh::getParentVoxel() const
{
	vector< unsigned int > ret( spines_.size(), -1U );
	// for ( unsigned int i = 0; i < spines_.size(); ++i )
	// 	ret[i] = spines_[i].parent(); // Wrong, returns voxel on NeuroMesh
	return ret;
}

/**
 * Returns index of voxel on NeuroMesh to which this spine is connected.
 */
vector< unsigned int > SpineMesh::getNeuronVoxel() const
{
	vector< unsigned int > ret( spines_.size(), -1U );
	for ( unsigned int i = 0; i < spines_.size(); ++i )
		ret[i] = spines_[i].parent();
	return ret;
}

vector< Id > SpineMesh::getElecComptMap() const
{
	vector< Id > ret( spines_.size() );
	for ( unsigned int i = 0; i < spines_.size(); ++i )
		ret[i] = spines_[i].headId();
	return ret;
}

vector< unsigned int > SpineMesh::getStartVoxelInCompt() const
{
	vector< unsigned int > ret( spines_.size() );
	for ( unsigned int i = 0; i < ret.size(); ++i )
		ret[i] = i;
	return ret;
}

vector< unsigned int > SpineMesh::getEndVoxelInCompt() const
{
	vector< unsigned int > ret( spines_.size() );
	for ( unsigned int i = 0; i < ret.size(); ++i )
		ret[i] = i+1;
	return ret;
}

//////////////////////////////////////////////////////////////////////

/**
 * This assumes that lambda is the quantity to preserve, over numEntries.
 * So when the compartment changes volume, numEntries changes too.
 * Assumes that the soma node is at index 0.
 */
void SpineMesh::updateCoords()
{
	buildStencil();
}

unsigned int SpineMesh::innerGetDimensions() const
{
	return 3;
}

// Here we set up the spines. We don't permit heads without shafts.
void SpineMesh::handleSpineList(
		const Eref& e, vector< Id > shaft, vector< Id > head,
		vector< unsigned int > parentVoxel )
{
		double oldVol = getMeshEntryVolume( 0 );
		assert( head.size() == parentVoxel.size() );
		assert( head.size() == shaft.size() );
		spines_.resize( head.size() );
		vs_.resize( head.size() );
		area_.resize( head.size() );
		length_.resize( head.size() );

		vector< double > ret;
		vector< double > psdCoords;
		vector< unsigned int > index( head.size(), 0 );
		for ( unsigned int i = 0; i < head.size(); ++i ) {
			spines_[i] = SpineEntry( shaft[i], head[i], parentVoxel[i] );
			// cout << i << "	" << head[i] << ", pa = " << parentVoxel[i] << endl;
			// ret = spines_[i].psdCoords();
			// assert( ret.size() == 8 );
			// psdCoords.insert( psdCoords.end(), ret.begin(), ret.end() );
			// index[i] = i;
			vs_[i] = spines_[i].volume();
			area_[i] = spines_[i].rootArea();
			length_[i] = spines_[i].diffusionLength();
		}

		updateCoords();
		Id meshEntry( e.id().value() + 1 );

		vector< unsigned int > localIndices( head.size() );
		vector< double > vols( head.size() );
		for ( unsigned int i = 0; i < head.size(); ++i ) {
			localIndices[i] = i;
			vols[i] = spines_[i].volume();
		}
		vector< vector< unsigned int > > outgoingEntries;
		vector< vector< unsigned int > > incomingEntries;
		// meshSplit()->send( e, oldVol, vols, localIndices, outgoingEntries, incomingEntries );
		lookupEntry( 0 )->triggerRemesh( meshEntry.eref(),
						oldVol, 0, localIndices, vols );
}

//////////////////////////////////////////////////////////////////
// FieldElement assignment stuff for MeshEntries
//////////////////////////////////////////////////////////////////

/// Virtual function to return MeshType of specified entry.
unsigned int SpineMesh::getMeshType( unsigned int fid ) const
{
	assert( fid < spines_.size() );
	return CYL;
}

/// Virtual function to return dimensions of specified entry.
unsigned int SpineMesh::getMeshDimensions( unsigned int fid ) const
{
	return 3;
}

/// Virtual function to return volume of mesh Entry.
double SpineMesh::getMeshEntryVolume( unsigned int fid ) const
{
	if ( spines_.size() == 0 )
		return 1.0;
	assert( fid < spines_.size() );
	return spines_[ fid % spines_.size() ].volume();
}
/// Virtual function to assign volume of mesh Entry.
void SpineMesh::setMeshEntryVolume( unsigned int fid, double volume )
{
	if ( spines_.size() == 0 )
		return;
	assert( fid < spines_.size() );
	spines_[ fid % spines_.size() ].setVolume( volume );
}

/// Virtual function to return coords of mesh Entry.
/// For SpineMesh, coords are x1y1z1 x2y2z2 x3y3z3 r0 r1
vector< double > SpineMesh::getCoordinates( unsigned int fid ) const
{
	vector< double > ret;
	return ret;
}

/// Virtual function to return diffusion X-section area for each neighbor
vector< double > SpineMesh::getDiffusionArea( unsigned int fid ) const
{
	vector< double > ret;
	return ret;
}

/// Virtual function to return scale factor for diffusion.
/// I think all dendite tips need to return just one entry of 1.
//  Regular points return vector( 2, 1.0 );
vector< double > SpineMesh::getDiffusionScaling( unsigned int fid ) const
{
	return vector< double >( 2, 1.0 );
}

/// Virtual function to return volume of mesh Entry, including
/// for diffusively coupled voxels from other solvers.
double SpineMesh::extendedMeshEntryVolume( unsigned int fid ) const
{
	if ( fid < spines_.size() ) {
		return getMeshEntryVolume( fid );
	} else {
		return MeshCompt::extendedMeshEntryVolume( fid - spines_.size() );
	}
}



//////////////////////////////////////////////////////////////////
// Dest funcsl
//////////////////////////////////////////////////////////////////

/*
/// More inherited virtual funcs: request comes in for mesh stats
/// Not clear what this does.
void SpineMesh::innerHandleRequestMeshStats( const Eref& e,
		const SrcFinfo2< unsigned int, vector< double > >* meshStatsFinfo
	)
{
		;
}
*/

void SpineMesh::innerHandleNodeInfo(
			const Eref& e,
			unsigned int numNodes, unsigned int numThreads )
{


}
//////////////////////////////////////////////////////////////////
// Inherited virtual funcs
//////////////////////////////////////////////////////////////////

const vector< double >& SpineMesh::vGetVoxelVolume() const
{
	return vs_;
}

const vector< double >& SpineMesh::vGetVoxelMidpoint() const
{
	static vector< double > midpoint;
	midpoint.resize( spines_.size() * 3 );
	for ( unsigned int i = 0; i < spines_.size(); ++i ) {
		spines_[i].mid( midpoint[i],
						midpoint[i + spines_.size() ],
						midpoint[i + 2 * spines_.size() ]
		);
	}
	return midpoint;
}

const vector< double >& SpineMesh::getVoxelArea() const
{
	return area_;
}

const vector< double >& SpineMesh::getVoxelLength() const
{
	return length_;
}

double SpineMesh::vGetEntireVolume() const
{
	double ret = 0.0;
	for ( vector< double >::const_iterator i =
					vs_.begin(); i != vs_.end(); ++i )
		ret += *i;
	return ret;
}

bool SpineMesh::vSetVolumeNotRates( double volume )
{
	double volscale = volume / vGetEntireVolume();
	double linscale = pow( volscale, 1.0/3.0 );
	assert( vs_.size() == spines_.size() );
	assert( area_.size() == spines_.size() );
	assert( length_.size() == spines_.size() );
	for ( unsigned int i = 0; i < spines_.size(); ++i ) {
		spines_[i].setVolume( volume );
		vs_[i] *= volscale;
		area_[i] *= linscale * linscale;
		length_[i] *= linscale;
	}
	return true;
}

//////////////////////////////////////////////////////////////////

/**
 * Inherited virtual func. Returns number of MeshEntry in array
 */
unsigned int SpineMesh::innerGetNumEntries() const
{
	return spines_.size();
}

/**
 * Inherited virtual func. Assigns number of MeshEntries.
 * Doesn't do anything, we have to set spine # from geometry.
 */
void SpineMesh::innerSetNumEntries( unsigned int n )
{
}


/**
 * Not allowed.
 */
void SpineMesh::innerBuildDefaultMesh( const Eref& e,
	double volume, unsigned int numEntries )
{
	cout << "Warning: SpineMesh::innerBuildDefaultMesh: attempt to build a default spine: not permitted\n";
}

//////////////////////////////////////////////////////////////////
const vector< SpineEntry >& SpineMesh::spines() const
{
		return spines_;
}

//////////////////////////////////////////////////////////////////
// Utility function to set up Stencil for diffusion
//////////////////////////////////////////////////////////////////
void SpineMesh::buildStencil()
{
// stencil_[0] = new NeuroStencil( nodes_, nodeIndex_, vs_, area_);
	setStencilSize( spines_.size(), spines_.size() );
	innerResetStencil();
}

//////////////////////////////////////////////////////////////////
// Utility function for junctions
//////////////////////////////////////////////////////////////////

void SpineMesh::matchMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	const CubeMesh* cm = dynamic_cast< const CubeMesh* >( other );
	if ( cm ) {
		matchCubeMeshEntries( other, ret );
		return;
	}
	const NeuroMesh* nm = dynamic_cast< const NeuroMesh* >( other );
	if ( nm ) {
		matchNeuroMeshEntries( other, ret );
		return;
	}
	const PsdMesh* pm = dynamic_cast< const PsdMesh* >( other );
	if ( pm ) {
		pm->matchSpineMeshEntries( this, ret );
		flipRet( ret );
		return;
	}
	cout << "Warning: SpineMesh::matchMeshEntries: unknown class\n";
}

void SpineMesh::indexToSpace( unsigned int index,
			double& x, double& y, double& z ) const
{
	if ( index >= innerGetNumEntries() )
		return;
	spines_[ index ].mid( x, y, z );
}

double SpineMesh::nearest( double x, double y, double z,
				unsigned int& index ) const
{
	double best = 1e12;
	index = 0;
	for( unsigned int i = 0; i < spines_.size(); ++i ) {
		const SpineEntry& se = spines_[i];
		double a0, a1, a2;
		se.mid( a0, a1, a2 );
		Vec a( a0, a1, a2 );
		Vec b( x, y, z );
		double d = a.distance( b );
		if ( best > d ) {
			best = d;
			index = i;
		}
	}
	if ( best == 1e12 )
		return -1;
	return best;
}

void SpineMesh::matchSpineMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
}

void SpineMesh::matchNeuroMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	const NeuroMesh* nm = dynamic_cast< const NeuroMesh* >( other );
	assert( nm );
	for ( unsigned int i = 0; i < spines_.size(); ++i ) {
		double xda = spines_[i].rootArea() / spines_[i].diffusionLength();
		ret.push_back( VoxelJunction( i, spines_[i].parent(), xda ) );
		ret.back().firstVol = spines_[i].volume();
		ret.back().secondVol =
				nm->getMeshEntryVolume( spines_[i].parent() );
	}
}

void SpineMesh::matchCubeMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	for( unsigned int i = 0; i < spines_.size(); ++i ) {
		const SpineEntry& se = spines_[i];
		se.matchCubeMeshEntriesToHead( other, i, surfaceGranularity_, ret );
	}
}
