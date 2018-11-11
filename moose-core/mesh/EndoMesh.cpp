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
#include "../basecode/ElementValueFinfo.h"
#include "../utility/Vec.h"
#include "Boundary.h"
#include "MeshEntry.h"
// #include "Stencil.h"
#include "ChemCompt.h"
#include "MeshCompt.h"
#include "CubeMesh.h"
#include "CylBase.h"
#include "NeuroNode.h"
// #include "NeuroStencil.h"
#include "NeuroMesh.h"
#include "EndoMesh.h"
#include "../utility/numutil.h"

static CubeMesh defaultParent;

const Cinfo* EndoMesh::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ElementValueFinfo< EndoMesh, double > rPower(
			"rPower",
			"Used in rEndo = rScale * pow(surroundVol, rPower)."
			"Used to obtain radius of each endo voxel from matching "
			"surround voxel. Defaults to 1/3",
			&EndoMesh::setRpower,
			&EndoMesh::getRpower
		);
		static ElementValueFinfo< EndoMesh, double > rScale(
			"rScale",
			"Used in rEndo = rScale * pow(surroundVol, rPower)."
			"Used to obtain radius of each endo voxel from matching "
			"surround voxel. Defaults to 0.5",
			&EndoMesh::setRscale,
			&EndoMesh::getRscale
		);
		static ElementValueFinfo< EndoMesh, double > aPower(
			"aPower",
			"Used in rEndo = aScale * pow(surroundVol, aPower)."
			"Used to obtain area of each endo voxel from matching "
			"surround voxel. Defaults to 1/2",
			&EndoMesh::setApower,
			&EndoMesh::getApower
		);
		static ElementValueFinfo< EndoMesh, double > aScale(
			"aScale",
			"Used in rEndo = aScale * pow(surroundVol, aPower)."
			"Used to obtain area of each endo voxel from matching "
			"surround voxel. Defaults to 0.25",
			&EndoMesh::setAscale,
			&EndoMesh::getAscale
		);
		static ElementValueFinfo< EndoMesh, double > vPower(
			"vPower",
			"Used in rEndo = vScale * pow(surroundVol, vPower)."
			"Used to obtain volume of each endo voxel from matching "
			"surround voxel. Defaults to 1.",
			&EndoMesh::setVpower,
			&EndoMesh::getVpower
		);
		static ElementValueFinfo< EndoMesh, double > vScale(
			"vScale",
			"Used in rEndo = vScale * pow(surroundVol, vPower)."
			"Used to obtain volume of each endo voxel from matching "
			"surround voxel. Defaults to 0.125",
			&EndoMesh::setVscale,
			&EndoMesh::getVscale
		);

		static ElementValueFinfo< EndoMesh, ObjId > surround(
			"surround",
			"ObjId of compartment surrounding current EndoMesh",
			&EndoMesh::setSurround,
			&EndoMesh::getSurround
		);

		static ElementValueFinfo< EndoMesh, bool > doAxialDiffusion(
			"doAxialDiffusion",
			"Flag to determine if endoMesh should undertake axial "
			"diffusion. Defaults to 'false'. "
			"Should only be used within NeuroMesh and CylMesh. "
			"Must be assigned before Dsolver is built.",
			&EndoMesh::setDoAxialDiffusion,
			&EndoMesh::getDoAxialDiffusion
		);


		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		// Field Elements
		//////////////////////////////////////////////////////////////

	static Finfo* endoMeshFinfos[] = {
		&rPower,			// Value
		&rScale,			// Value
		&aPower,			// Value
		&aScale,			// Value
		&vPower,			// Value
		&vScale,			// Value
		&surround,			// Value
		&doAxialDiffusion,	// Value
	};

	static Dinfo< EndoMesh > dinfo;
	static Cinfo endoMeshCinfo (
		"EndoMesh",
		ChemCompt::initCinfo(),
		endoMeshFinfos,
		sizeof( endoMeshFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &endoMeshCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* endoMeshCinfo = EndoMesh::initCinfo();

//////////////////////////////////////////////////////////////////
// Class stuff.
//////////////////////////////////////////////////////////////////
EndoMesh::EndoMesh()
	:
		parent_( &defaultParent ),
		rPower_( 1.0 / 3.0 ),
		rScale_( 0.5 ),
		aPower_( 0.5 ),
		aScale_( 0.25 ),
		vPower_( 1.0 ),
		vScale_( 0.125 ),
		doAxialDiffusion_( false )
{
	;
}

EndoMesh::~EndoMesh()
{
	;
}

//////////////////////////////////////////////////////////////////
// Field assignment stuff
//////////////////////////////////////////////////////////////////
//
void EndoMesh::setRpower( const Eref& e, double v )
{
	rPower_ = v;
}

double EndoMesh::getRpower( const Eref& e ) const
{
	return rPower_;
}

void EndoMesh::setRscale( const Eref& e, double v )
{
	rScale_ = v;
}

double EndoMesh::getRscale( const Eref& e ) const
{
	return rScale_;
}

void EndoMesh::setApower( const Eref& e, double v )
{
	aPower_ = v;
}

double EndoMesh::getApower( const Eref& e ) const
{
	return aPower_;
}

void EndoMesh::setAscale( const Eref& e, double v )
{
	aScale_ = v;
}

double EndoMesh::getAscale( const Eref& e ) const
{
	return aScale_;
}

void EndoMesh::setVpower( const Eref& e, double v )
{
	vPower_ = v;
}

double EndoMesh::getVpower( const Eref& e ) const
{
	return vPower_;
}

void EndoMesh::setVscale( const Eref& e, double v )
{
	vScale_ = v;
}

double EndoMesh::getVscale( const Eref& e ) const
{
	return vScale_;
}

void EndoMesh::setSurround( const Eref& e, ObjId v )
{
	if ( !v.element()->cinfo()->isA( "ChemCompt" ) ) {
		cout << "Warning: 'surround' may only be set to an object of class 'ChemCompt'\n";
		cout << v.path() << " is of class " << v.element()->cinfo()->name() << endl;
		return;
	}
	surround_ = v;
	parent_ = reinterpret_cast< const MeshCompt* >( v.data() );
}

ObjId EndoMesh::getSurround( const Eref& e ) const
{
	return surround_;
}

void EndoMesh::setDoAxialDiffusion( const Eref& e, bool v )
{
	doAxialDiffusion_ = v;
}

bool EndoMesh::getDoAxialDiffusion( const Eref& e ) const
{
	return doAxialDiffusion_;
}

//////////////////////////////////////////////////////////////////
// FieldElement assignment stuff for MeshEntries
//////////////////////////////////////////////////////////////////

/// Virtual function to return MeshType of specified entry.
unsigned int EndoMesh::getMeshType( unsigned int fid ) const
{
	return ENDO;
}

/// Virtual function to return dimensions of specified entry.
unsigned int EndoMesh::getMeshDimensions( unsigned int fid ) const
{
	return 3;
}

/// Virtual function to return # of spatial dimensions of mesh
unsigned int EndoMesh::innerGetDimensions() const
{
	return 3;
}
/// Virtual function to return volume of mesh Entry.
double EndoMesh::getMeshEntryVolume( unsigned int fid ) const
{
	double vpa = parent_->getMeshEntryVolume( fid );
	return vScale_ * pow( vpa, vPower_ );
}

/// Virtual function to return coords of mesh Entry.
/// For Endo mesh, coords are just middle of parent.
vector< double > EndoMesh::getCoordinates( unsigned int fid ) const
{
	vector< double > temp = parent_->getCoordinates( fid );
	vector< double > ret;
	if ( temp.size() > 6 ) {
		ret.resize( 4 );
		ret[0] = 0.5 * (temp[0] + temp[3] );
		ret[1] = 0.5 * (temp[1] + temp[4] );
		ret[2] = 0.5 * (temp[2] + temp[5] );
		ret[3] = 0;
	}
	return ret;
}

/// Virtual function to return diffusion X-section area for each neighbor
/// This applies if we have endo mesh voxels diffusing with each other 
vector< double > EndoMesh::getDiffusionArea( unsigned int fid ) const
{
	return vector< double >( parent_->getNumEntries(), 0.0 );
}

/// Virtual function to return scale factor for diffusion. 1 here.
/// This applies if we have endo mesh voxels diffusing with each other 
vector< double > EndoMesh::getDiffusionScaling( unsigned int fid ) const
{
	return vector< double >( parent_->getNumEntries(), 0.0 );
}

/// Virtual function to return volume of mesh Entry, including
/// for diffusively coupled voxels from other solvers.
double EndoMesh::extendedMeshEntryVolume( unsigned int fid ) const
{
	double vpa = parent_->extendedMeshEntryVolume( fid );
	return vScale_ * pow( vpa, vPower_ );
}

//////////////////////////////////////////////////////////////////
// Dest funcs
//////////////////////////////////////////////////////////////////

/// More inherited virtual funcs: request comes in for mesh stats
void EndoMesh::innerHandleRequestMeshStats( const Eref& e,
		const SrcFinfo2< unsigned int, vector< double > >* meshStatsFinfo
	)
{
	vector< double > ret( vGetEntireVolume() / parent_->getNumEntries(), 1);
	meshStatsFinfo->send( e, 1, ret );
}

void EndoMesh::innerHandleNodeInfo(
			const Eref& e,
			unsigned int numNodes, unsigned int numThreads )
{
}
//////////////////////////////////////////////////////////////////

/**
 * Inherited virtual func. Returns number of MeshEntry in array
 */
unsigned int EndoMesh::innerGetNumEntries() const
{
	return parent_->innerGetNumEntries();
}

/**
 * Inherited virtual func. Dummy, because the EndoMesh just does what its
 * parent does.
 */
void EndoMesh::innerSetNumEntries( unsigned int n )
{
}


void EndoMesh::innerBuildDefaultMesh( const Eref& e,
	double volume, unsigned int numEntries )
{
}

/**
 * This means the sibling voxel immediately adjacent to it, not the
 * voxel surrounding it. Should we wish to do diffusion we would need
 * to use the parent voxel of the surround voxel. Otherewise use
 * EMTPY_VOXEL == -1U.
 */
vector< unsigned int > EndoMesh::getParentVoxel() const
{
	if ( doAxialDiffusion_ )
		return parent_->getParentVoxel();

	vector< unsigned int > ret( parent_->innerGetNumEntries(), -1U );
	return ret;
}

const vector< double >& EndoMesh::vGetVoxelVolume() const
{
	static vector< double > ret;
   	ret = parent_->vGetVoxelVolume();
	for ( auto i = ret.begin(); i != ret.end(); ++i )
		*i = vScale_ * pow( *i, vPower_ );
	return ret;
}

const vector< double >& EndoMesh::vGetVoxelMidpoint() const
{
	return parent_->vGetVoxelMidpoint();
}

const vector< double >& EndoMesh::getVoxelArea() const
{
	static vector< double > ret;
	ret = parent_->vGetVoxelVolume();
	for ( auto i = ret.begin(); i != ret.end(); ++i )
		*i = aScale_ * pow( *i, aPower_ );
	return ret;
}

const vector< double >& EndoMesh::getVoxelLength() const
{
	static vector< double > ret;
	ret = parent_->vGetVoxelVolume();
	for ( auto i = ret.begin(); i != ret.end(); ++i )
		*i = vScale_ * pow( *i, vPower_ );
	return ret;
}

double EndoMesh::vGetEntireVolume() const
{
	double vol = 0.0;
	auto vec = vGetVoxelVolume();
	for ( auto i = vec.begin(); i != vec.end(); ++i )
		vol += *i;
	return vol;
}

bool EndoMesh::vSetVolumeNotRates( double volume )
{
	return true; // maybe should be false? Unsure
}

//////////////////////////////////////////////////////////////////
// Utility function for junctions
//////////////////////////////////////////////////////////////////

void EndoMesh::matchMeshEntries( const ChemCompt* other,
	   vector< VoxelJunction >& ret ) const
{
	/**
	 * Consider concentric EndoMeshes in an outer cyl/sphere of radius r_0.
	 * Radii, from out to in, are r_0, r_1, r_2.... r_innermost.
	 * Consider that we are on EndoMesh n.
	 * The diffusion length in each case is 1/2(r_n-1 - r_n+1).
	 * For the innermost we use r_n+1 = 0
	 * The diffusion XA is the area of the EndoMesh.
	 */
	vector< double > vol = other->vGetVoxelVolume();
	const MeshCompt* mc = static_cast< const MeshCompt* >( other );
	vector< double > len = mc->getVoxelLength();
	assert( vol.size() == len.size() );
	ret.resize( vol.size() );
	vector< double > myVol = vGetVoxelVolume();
	assert( myVol.size() == vol.size() );
	vector< double > myArea = getVoxelArea();
	assert( myArea.size() == vol.size() );

	for ( unsigned int i = 0; i < vol.size(); ++i ) {
		double rSurround = sqrt( vol[i] / (len[i] * PI ) );
		ret[i].first = ret[i].second = i;
		ret[i].firstVol = myVol[i];
		ret[i].secondVol = vol[i];

		// For now we don't consider internal EndoMeshes.
		ret[i].diffScale = 2.0 * myArea[i] / rSurround;
	}
}

// This function returns the index.
double EndoMesh::nearest( double x, double y, double z,
				unsigned int& index ) const
{
	return parent_->nearest( x, y, z, index );
}

// This function returns the index.
void EndoMesh::indexToSpace( unsigned int index,
				double &x, double &y, double &z ) const
{
	parent_->indexToSpace( index, x, y, z );
}
