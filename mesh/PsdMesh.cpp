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
#include "PsdMesh.h"
#include "SpineEntry.h"
#include "SpineMesh.h"
#include "../utility/numutil.h"
const Cinfo* PsdMesh::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< PsdMesh, double > thickness(
			"thickness",
			"An assumed thickness for PSD. The volume is computed as the"
			"PSD area passed in to each PSD, times this value."
			"defaults to 50 nanometres. For reference, membranes are 5 nm.",
			&PsdMesh::setThickness,
			&PsdMesh::getThickness
		);

		static ReadOnlyValueFinfo< PsdMesh, vector< unsigned int > >
			neuronVoxel
		(
		 	"neuronVoxel",
			"Vector of indices of voxels on parent NeuroMesh, from which "
			"the respective spines emerge.",
			&PsdMesh::getNeuronVoxel
		);

		static ReadOnlyValueFinfo< PsdMesh, vector< Id > > elecComptMap(
			"elecComptMap",
			"Vector of Ids of electrical compartments that map to each "
			"voxel. This is necessary because the order of the IDs may "
			"differ from the ordering of the voxels. Note that there "
			"is always just one voxel per PSD. ",
			&PsdMesh::getElecComptMap
		);
		static ReadOnlyValueFinfo< PsdMesh, vector< Id > > elecComptList(
			"elecComptList",
			"Vector of Ids of all electrical compartments in this "
			"PsdMesh. Ordering is as per the tree structure built in "
			"the NeuroMesh, and may differ from Id order. Ordering "
			"matches that used for startVoxelInCompt and endVoxelInCompt",
			&PsdMesh::getElecComptMap
		);
		static ReadOnlyValueFinfo< PsdMesh, vector< unsigned int > > startVoxelInCompt(
			"startVoxelInCompt",
			"Index of first voxel that maps to each electrical "
			"compartment. This is a trivial function in the PsdMesh, as"
			"we have a single voxel per spine. So just a vector of "
			"its own indices.",
			&PsdMesh::getStartVoxelInCompt
		);
		static ReadOnlyValueFinfo< PsdMesh, vector< unsigned int > > endVoxelInCompt(
			"endVoxelInCompt",
			"Index of end voxel that maps to each electrical "
			"compartment. Since there is just one voxel per electrical "
			"compartment in the spine, this is just a vector of index+1",
			&PsdMesh::getEndVoxelInCompt
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////

		static DestFinfo psdList( "psdList",
			"Specifies the geometry of the spine,"
			"and the associated parent voxel"
			"Arguments: "
			"disk params vector with 8 entries per psd, "
			"vector of Ids of electrical compts mapped to voxels, "
			"parent voxel index ",
			new EpFunc3< PsdMesh,
				vector< double >,
				vector< Id >,
		   		vector< unsigned int > >(
				&PsdMesh::handlePsdList )
		);

		//////////////////////////////////////////////////////////////
		// Field Elements
		//////////////////////////////////////////////////////////////

	static Finfo* psdMeshFinfos[] = {
		&thickness,			// ValueFinfo
		&neuronVoxel,		// ReadOnlyValueFinfo
		&elecComptMap,		// ReadOnlyValueFinfo
		&elecComptList,		// ReadOnlyValueFinfo
		&startVoxelInCompt,		// ReadOnlyValueFinfo
		&endVoxelInCompt,		// ReadOnlyValueFinfo
		&psdList,			// DestFinfo
	};

	static Dinfo< PsdMesh > dinfo;
	static Cinfo psdMeshCinfo (
		"PsdMesh",
		ChemCompt::initCinfo(),
		psdMeshFinfos,
		sizeof( psdMeshFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &psdMeshCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* psdMeshCinfo = PsdMesh::initCinfo();

//////////////////////////////////////////////////////////////////
// Class stuff.
//////////////////////////////////////////////////////////////////
PsdMesh::PsdMesh()
	:
		thickness_( 50.0e-9 ),
		psd_( 1 ),
		pa_( 1 ),
		parentDist_( 1, 1e-6 ),
		parent_( 1, 0 ),
		surfaceGranularity_( 0.1 ),
		vs_(1, 5.0e-21 ),
		area_(1, 1.0e-13 ),
		length_(1, 50.0e-9 )
{
	const double defaultLength = 1e-6;
	psd_[0].setDia( defaultLength );
	psd_[0].setLength( thickness_ );
	psd_[0].setNumDivs( 1 );
	psd_[0].setIsCylinder( true );
}

PsdMesh::PsdMesh( const PsdMesh& other )
	:
		psd_( other.psd_ ),
		surfaceGranularity_( other.surfaceGranularity_ )
{;}

PsdMesh::~PsdMesh()
{
	;
}

//////////////////////////////////////////////////////////////////
// Field assignment stuff
//////////////////////////////////////////////////////////////////
double PsdMesh::getThickness() const
{
	return thickness_;
}
void PsdMesh::setThickness( double v )
{
	thickness_ = v;
}

/**
 * Returns index of voxel on NeuroMesh to which this spine is connected.
 * This isn't such a great function, as it only works if the spine is
 * sitting directly on the NeuroMesh. Otherwise it returns the vector
 * of voxels of parent spine heads, which isn't what the name indicates.
 */
vector< unsigned int > PsdMesh::getNeuronVoxel() const
{
	cout << "Warning: PsdMesh::getNeuronVoxel. Currently not working\n";
	return parent_;
}

vector< Id > PsdMesh::getElecComptMap() const
{
	return elecCompt_;
}

vector< unsigned int > PsdMesh::getStartVoxelInCompt() const
{
	vector< unsigned int > ret( elecCompt_.size() );
	for ( unsigned int i = 0; i < ret.size(); ++i )
		ret[i] = i;
	return ret;
}

vector< unsigned int > PsdMesh::getEndVoxelInCompt() const
{
	vector< unsigned int > ret( elecCompt_.size() );
	for ( unsigned int i = 0; i < ret.size(); ++i )
		ret[i] = i+1;
	return ret;
}

/**
 * This assumes that lambda is the quantity to preserve, over numEntries.
 * So when the compartment changes volume, numEntries changes too.
 * Assumes that the soma node is at index 0.
 */
void PsdMesh::updateCoords()
{
	buildStencil();
}

unsigned int PsdMesh::innerGetDimensions() const
{
	return 2;
}

// Here we set up the psds.
void PsdMesh::handlePsdList(
		const Eref& e,
		vector< double > diskCoords, //ctr(xyz), dir(xyz), dia, diffDist
		// The elecCompt refers to the spine head elec compartment.
		vector< Id > elecCompt,
		// The parentVoxel is just a vector where parentVoxel[i] == i
		// Thus the parent voxel is the spine head with the same index
		vector< unsigned int > parentVoxel )
{
		double oldVol = getMeshEntryVolume( 0 );
		assert( diskCoords.size() == 8 * parentVoxel.size() );
		psd_.resize( parentVoxel.size() );
		pa_.resize( parentVoxel.size() );
		vs_.resize( parentVoxel.size() );
		area_.resize( parentVoxel.size() );
		length_.resize( parentVoxel.size() );
		elecCompt_ = elecCompt;

		psd_.clear();
		pa_.clear();
		parentDist_.clear();
		parent_.clear();
		vector< double >::const_iterator x = diskCoords.begin();
		for ( unsigned int i = 0; i < parentVoxel.size(); ++i ) {
			double p = *x;
			double q = *(x+1);
			double r = *(x+2);

			psd_.push_back( CylBase( p, q, r, 1, 0, 1 ));
			x += 3;
			pa_.push_back( CylBase( p - *x, q - *(x+1), r - *(x+2), 1, 0, 1 ));
			x += 3;
			psd_.back().setDia( *x++ );
			psd_.back().setIsCylinder( true );
				// This is an entirely nominal
				// length, so that the effective vol != 0.
			psd_.back().setLength( thickness_ );

			parentDist_.push_back( *x++ );
			vs_[i] = psd_.back().volume( psd_.back() );
			area_[i] = psd_.back().getDiffusionArea( psd_.back(), 0 );
			length_[i] = parentDist_.back();
		}
		parent_ = parentVoxel;

		updateCoords();

		Id meshEntry( e.id().value() + 1 );

		vector< unsigned int > localIndices( psd_.size() );
		vector< double > vols( psd_.size() );
		for ( unsigned int i = 0; i < psd_.size(); ++i ) {
			localIndices[i] = i;
			vols[i] = thickness_ * psd_[i].getDiffusionArea( pa_[i], 0 );
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
unsigned int PsdMesh::getMeshType( unsigned int fid ) const
{
	assert( fid < psd_.size() );
	return DISK;
}

/// Virtual function to return dimensions of specified entry.
unsigned int PsdMesh::getMeshDimensions( unsigned int fid ) const
{
	return 2;
}

/// Virtual function to return volume of mesh Entry.
double PsdMesh::getMeshEntryVolume( unsigned int fid ) const
{
	if ( psd_.size() == 0 ) // Default for meshes before init.
		return 1.0;
	assert( fid < psd_.size() );
	return thickness_ * psd_[ fid ].getDiffusionArea( pa_[fid], 0 );
}

/// Virtual function to assign volume of mesh Entry. Thickness not touched.
void PsdMesh::setMeshEntryVolume( unsigned int fid, double volume )
{
	if ( psd_.size() == 0 ) // Default for meshes before init.
		return;
	assert( fid < psd_.size() );
	vs_[fid] = volume;
	area_[fid] = volume / thickness_;
	double dia = 2.0 * sqrt( area_[fid] / PI );
	psd_[ fid ].setDia( dia );
}

/// Virtual function to return coords of mesh Entry.
/// For PsdMesh, coords are x1y1z1 x2y2z2 dia
vector< double > PsdMesh::getCoordinates( unsigned int fid ) const
{
	vector< double > ret;
	ret.push_back( psd_[fid].getX() );
	ret.push_back( psd_[fid].getY() );
	ret.push_back( psd_[fid].getZ() );
	ret.push_back( psd_[fid].getX() - pa_[fid].getX() );
	ret.push_back( psd_[fid].getY() - pa_[fid].getY() );
	ret.push_back( psd_[fid].getZ() - pa_[fid].getZ() );
	ret.push_back( psd_[fid].getDia() );
	return ret;
}

/// Virtual function to return diffusion X-section area for each neighbor
vector< double > PsdMesh::getDiffusionArea( unsigned int fid ) const
{
	vector< double > ret;
	assert( fid < psd_.size() );
	ret.push_back( psd_[ fid ].getDiffusionArea( pa_[fid], 0 ) );

	return ret;
}

/// Virtual function to return scale factor for diffusion.
/// I think all dendite tips need to return just one entry of 1.
//  Regular points return vector( 2, 1.0 );
vector< double > PsdMesh::getDiffusionScaling( unsigned int fid ) const
{
	return vector< double >( 2, 1.0 );
}

/// Virtual function to return volume of mesh Entry, including
/// for diffusively coupled voxels from other solvers.
double PsdMesh::extendedMeshEntryVolume( unsigned int fid ) const
{
	if ( fid < psd_.size() ) {
		return getMeshEntryVolume( fid );
	} else {
		return MeshCompt::extendedMeshEntryVolume( fid - psd_.size() );
	}
}



//////////////////////////////////////////////////////////////////
// Dest funcsl
//////////////////////////////////////////////////////////////////

/// More inherited virtual funcs: request comes in for mesh stats
/// Not clear what this does.
void PsdMesh::innerHandleRequestMeshStats( const Eref& e,
		const SrcFinfo2< unsigned int, vector< double > >* meshStatsFinfo
	)
{
		;
}

void PsdMesh::innerHandleNodeInfo(
			const Eref& e,
			unsigned int numNodes, unsigned int numThreads )
{
}
//////////////////////////////////////////////////////////////////

unsigned int PsdMesh::parent( unsigned int i ) const
{
	if ( i < parent_.size() )
		return parent_[i];
	cout << "Error: PsdMesh::parent: Index " << i << " out of range: " <<
				parent_.size() << "\n";
	return 0;
}

/**
 * Inherited virtual func. Returns number of MeshEntry in array
 */
unsigned int PsdMesh::innerGetNumEntries() const
{
	return psd_.size();
}

/**
 * Inherited virtual func. Assigns number of MeshEntries.
 * Doesn't do anything, we have to set psd # from geometry.
 */
void PsdMesh::innerSetNumEntries( unsigned int n )
{
}


/**
 * Not permitted
 */
void PsdMesh::innerBuildDefaultMesh( const Eref& e,
	double volume, unsigned int numEntries )
{
	cout << "Warning: PsdMesh::innerBuildDefaultMesh: attempt to build a default psd: not permitted\n";
}

//////////////////////////////////////////////////////////////////
// Utility function to set up Stencil for diffusion
//////////////////////////////////////////////////////////////////
void PsdMesh::buildStencil()
{
	setStencilSize( psd_.size(), psd_.size() );
	innerResetStencil();
}

//////////////////////////////////////////////////////////////////
// Utility function for junctions
//////////////////////////////////////////////////////////////////

void PsdMesh::matchMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	const SpineMesh* sm = dynamic_cast< const SpineMesh* >( other );
	if ( sm ) {
		matchSpineMeshEntries( other, ret );
		return;
	}
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
	cout << "Warning: PsdMesh::matchMeshEntries: unknown class\n";
}

void PsdMesh::indexToSpace( unsigned int index,
			double& x, double& y, double& z ) const
{
	if ( index >= innerGetNumEntries() )
		return;
	x = psd_[index].getX();
	y = psd_[index].getY();
	z = psd_[index].getZ();
}

double PsdMesh::nearest( double x, double y, double z,
				unsigned int& index ) const
{
	double best = 1e12;
	index = 0;
	for( unsigned int i = 0; i < psd_.size(); ++i ) {
		Vec a( psd_[i].getX(), psd_[i].getY(), psd_[i].getZ() );
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

void PsdMesh::matchSpineMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	const SpineMesh* sm = dynamic_cast< const SpineMesh* >( other );
	assert( sm );
	for ( unsigned int i = 0; i < psd_.size(); ++i ) {
		double xda = psd_[i].getDiffusionArea( pa_[i], 0 ) / parentDist_[i];
		ret.push_back( VoxelJunction( i, parent_[i], xda ) );
		ret.back().firstVol = getMeshEntryVolume( i );
		ret.back().secondVol = sm->getMeshEntryVolume( parent_[i] );
	}
}

// This only makes sense if the PSD is directly on the NeuroMesh, that is,
// no spine shaft or head. In this case the parent_ vector must refer to
// the parent compartment on the NeuroMesh.
void PsdMesh::matchNeuroMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	const NeuroMesh* nm = dynamic_cast< const NeuroMesh* >( other );
	assert( nm );
	for ( unsigned int i = 0; i < psd_.size(); ++i ) {
		double xda = psd_[i].getDiffusionArea( pa_[i], 0) / parentDist_[i];
		ret.push_back( VoxelJunction( i, parent_[i], xda ) );
	}
}

void PsdMesh::matchCubeMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	for( unsigned int i = 0; i < psd_.size(); ++i ) {
		const CylBase& cb = psd_[i];
		cb.matchCubeMeshEntries( other, pa_[i],
		i, surfaceGranularity_, ret, false, true );
	}
}


////////////////////////////////////////////////////////////////////////
// Inherited Virtual funcs for getting voxel info.
////////////////////////////////////////////////////////////////////////

/**
 * This function returns the diffusively connected parent voxel within
 * the current (psd) mesh. Since each spine is treated as an independed
 * voxel, there is no such voxel, so we return -1U for each psd.
 * Note that there is a separate function that returns the parentVoxel
 * referred to the NeuroMesh that this spine/psd sits on.
 */
vector< unsigned int > PsdMesh::getParentVoxel() const
{
	vector< unsigned int > ret( parent_.size(), -1U );
	return ret;
	// return parent_;
}

const vector< double >& PsdMesh::vGetVoxelVolume() const
{
	return vs_;
}

/*
* The order of coords sent in is
		 * 	centre xyz
		 * 	direction xyz
		 * 	dia,
		 * 	diffusion distance to middle of spine Head.
*/
const vector< double >& PsdMesh::vGetVoxelMidpoint() const
{
	static vector< double > midpoint;
	midpoint.resize( psd_.size() * 3 );
	vector< double >::iterator k = midpoint.begin();
	for ( unsigned int i = 0; i < psd_.size(); ++i ) {
		vector< double > coords =
				psd_[i].getCoordinates( pa_[i], 0 );
		*k = ( coords[0] + coords[3] ) / 2.0;
		*(k + psd_.size() ) = ( coords[1] + coords[4] ) / 2.0;
		*(k + 2 * psd_.size() ) = ( coords[2] + coords[5] ) / 2.0;
		k++;
		// cout << i << " " << coords[0] << " " << coords[3]<< " " << coords[1]<< " " << coords[4]<< " " << coords[2]<< " " << coords[5] << " " << coords[6] << endl;
	}
	return midpoint;
}

const vector< double >& PsdMesh::getVoxelArea() const
{
	return area_;
}

const vector< double >& PsdMesh::getVoxelLength() const
{
	return length_;
}

double PsdMesh::vGetEntireVolume() const
{
	double ret = 0.0;
	for ( vector< double >::const_iterator i =
					vs_.begin(); i != vs_.end(); ++i )
		ret += *i;
	return ret;
}

bool PsdMesh::vSetVolumeNotRates( double volume )
{
	double volscale = volume / vGetEntireVolume();
	double linscale = pow( volscale, 1.0/3.0 );
	assert( vs_.size() == psd_.size() );
	assert( area_.size() == psd_.size() );
	assert( length_.size() == psd_.size() );
	thickness_ *= linscale;
	for ( unsigned int i = 0; i < psd_.size(); ++i ) {
		psd_[i].setLength( psd_[i].getLength() * linscale );
		psd_[i].setDia( psd_[i].getDia() * linscale );
		vs_[i] *= volscale;
		area_[i] *= linscale * linscale;
		length_[i] *= linscale;
	}
	return true;
}

