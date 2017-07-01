/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CUBE_MESH_H
#define _CUBE_MESH_H

/**
 * The CubeMesh represents a chemically identified compartment shaped
 * like a cuboid. This is not really an effective geometry for most
 * neurons because it would have to be rather finely subdivided to fit
 * a typical dendrite or soma volume, but it is general.
 */
class CubeMesh: public MeshCompt
{
	public:
		CubeMesh();
		~CubeMesh();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void setX0( double v );
		double getX0() const;
		void setY0( double v );
		double getY0() const;
		void setZ0( double v );
		double getZ0() const;

		void setX1( double v );
		double getX1() const;
		void setY1( double v );
		double getY1() const;
		void setZ1( double v );
		double getZ1() const;

		void setDx( double v );
		double getDx() const;
		void setDy( double v );
		double getDy() const;
		void setDz( double v );
		double getDz() const;

		void setNx( unsigned int v );
		unsigned int getNx() const;
		void setNy( unsigned int v );
		unsigned int getNy() const;
		void setNz( unsigned int v );
		unsigned int getNz() const;

		void innerSetCoords( const vector< double >& v );
		void setCoords( const Eref& e, vector< double > v );
		vector< double > getCoords( const Eref& e ) const;

		void setMeshToSpace( vector< unsigned int > v );
		vector< unsigned int > getMeshToSpace() const;

		void setSpaceToMesh( vector< unsigned int > v );
		vector< unsigned int > getSpaceToMesh() const;

		void setSurface( vector< unsigned int > v );
		vector< unsigned int > getSurface() const;

		unsigned int innerGetDimensions() const;

		void setIsToroid( bool v );
		bool getIsToroid() const;

		void setPreserveNumEntries( bool v );
		bool getPreserveNumEntries() const;

		void setAlwaysDiffuse( bool v );
		bool getAlwaysDiffuse() const;

		//////////////////////////////////////////////////////////////////
		// FieldElement assignment stuff for MeshEntries
		//////////////////////////////////////////////////////////////////

		/// Virtual function to return MeshType of specified entry.
		unsigned int getMeshType( unsigned int fid ) const;
		/// Virtual function to return dimensions of specified entry.
		unsigned int getMeshDimensions( unsigned int fid ) const;
		/// Virtual function to return volume of mesh Entry.
		double getMeshEntryVolume( unsigned int fid ) const;
		/// Virtual function to return coords of mesh Entry.
		vector< double > getCoordinates( unsigned int fid ) const;
		/// Virtual function to return diffusion X-section area
		vector< double > getDiffusionArea( unsigned int fid ) const;
		/// Virtual function to return scale factor for diffusion. 1 here.
		vector< double > getDiffusionScaling( unsigned int fid ) const;

		//////////////////////////////////////////////////////////////////

		/**
		 * Utility function to return volume of any voxel including those
		 * diffusively coupled and aubtting the present volume.
		 */
		double extendedMeshEntryVolume( unsigned int fid ) const;

		/**
		 * Inherited virtual func. Returns number of MeshEntry in array
		 */
		unsigned int innerGetNumEntries() const;
		/// Inherited virtual func.
		void innerSetNumEntries( unsigned int n );

		void innerHandleRequestMeshStats(
			const Eref& e,
			const SrcFinfo2< unsigned int, vector< double > >*
				meshStatsFinfo
		);

		void innerHandleNodeInfo(
			const Eref& e,
			unsigned int numNodes, unsigned int numThreads );

		/// Virtual func to get volume of entire compartment.
		double vGetEntireVolume() const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void buildMesh( Id geom, double x, double y, double z );

		void addStoich( Id stoich );

		/// Virtual func to make a mesh with specified Volume and numEntries
		void innerBuildDefaultMesh( const Eref& e,
			double volume, unsigned int numEntries );

		/// Virtual func, assigns volume, usually to single voxel.
		bool vSetVolumeNotRates( double volume );

		//////////////////////////////////////////////////////////////////
		//  Utility func
		//////////////////////////////////////////////////////////////////
		/**
		 * Recomputes all local coordinate and meshing data following
		 * a change in any of the coord parameters
		 */
		void updateCoords();

		unsigned int neighbor( unsigned int spaceIndex,
			int dx, int dy, int dz ) const;

		void transmitChange( const Eref& e, double oldvol );

		bool isInsideCuboid( double x, double y, double z ) const;
		bool isInsideSpheroid( double x, double y, double z ) const;

		//////////////////////////////////////////////////////////////////
		//  Stuff for junctions
		//////////////////////////////////////////////////////////////////

		/**
		 * Key virtual function for generating a map between facing
		 * surfaces on a CubeMesh and another ChemCompt
		 */
		void matchMeshEntries( const ChemCompt* other,
			vector< VoxelJunction >& ret ) const;

		/**
		 * Specialization for cube-to-cube mesh matching. Return vector is
		 * of pairs of meshIndices (not spatialIndices).
		 */
		void matchCubeMeshEntries( const CubeMesh* other,
			vector< VoxelJunction >& ret ) const;

		// Some ugly stuff here for cyl meshes.
		void matchCylMeshEntries( const ChemCompt* other,
			vector< VoxelJunction >& ret ) const;

		/**
		 * Dumbed-down version that does mesh matching based only on index.
		 * Useful for non-spatial matching and single-voxel systems.
		 */
		void matchAllEntries( const CubeMesh* other,
			vector< VoxelJunction >& ret ) const;


		/*
		/// Utility function for special case in matchMeshEntries.
		void matchSameSpacing( const CubeMesh* other,
			vector< pair< unsigned int, unsigned int > >& ret ) const;
			*/

		/// Utility function for returning # of dimensions in mesh
		unsigned int numDims() const;

		/// Converts the integer meshIndex to spatial coords.
		void indexToSpace( unsigned int index,
						double& x, double& y, double& z ) const;

		/// Converts the 3-D coords to an index. EMPTY if out of range.
		unsigned int spaceToIndex( double x, double y, double z ) const;

		/**
		 * Virtual function to return the distance and index of nearest
		 * meshEntry. Places entry at centre of voxel.
		 */
		double nearest( double x, double y, double z, unsigned int& index )
			   	const;

		/// Return 0 if spacing same, -1 if self smaller, +1 if self bigger
		int compareMeshSpacing( const CubeMesh* other ) const;

		/// Defines a cuboid volume of intersection between self and other.
		void defineIntersection( const CubeMesh* other,
			double& xmin, double &xmax,
			double& ymin, double &ymax,
			double& zmin, double &zmax ) const;

		/// Fills surface_ vector with spatial meshIndices for a rectangle
		void fillTwoDimSurface();

		/// Fills surface_ vector with spatial meshIndices for a cuboid,
		/// that is, puts the surfaces of the cuboid in the vector.
		void fillThreeDimSurface();

		/// Utility and test function to read surface.
		const vector< unsigned int >& surface() const;
		//////////////////////////////////////////////////////////////////
		//  Stuff for diffusion
		//////////////////////////////////////////////////////////////////

		/// Inherited virtual, do nothing for now.
		vector< unsigned int > getParentVoxel() const;
		const vector< double >& vGetVoxelVolume() const;
		const vector< double >& vGetVoxelMidpoint() const;
		const vector< double >& getVoxelArea() const;
		const vector< double >& getVoxelLength() const;

		/**
		 * Sets up the stencil that defines how to combine neighboring
		 * mesh elements to set up the diffusion du/dt term, using the
		 * method of lines.
		 * This is a very general function. It uses the information in the
		 * m2s_ and s2m_ vectors to work out the adjacency matrix. So
		 * we could use an arbitrary 3-D image to define the diffusive
		 * volume and boundaries using m2s_ and s2m_. We could also use
		 * geometric shapes through the fillSpaceToMeshLookup() function,
		 * which is currently a dummy and just does a cuboid.
		 */
		void buildStencil();
		void fillSpaceToMeshLookup();

		/**
		 * Updates the m2s_ vector after s2m_ has been changed,
		 * and rebuilds the Stencil too. Any earlier junction information
		 * is lost.
		 */
		void deriveM2sFromS2m();

		/**
		 * Updates the s2m_ vector after m2s_ has been changed,
		 * and rebuilds the Stencil too. Any earlier junction information
		 * is lost.
		 */
		void deriveS2mFromM2s();

		void assignVoxels(
				vector< pair< unsigned int, unsigned int > >& intersect,
				double xmin, double xmax,
				double ymin, double ymax,
				double zmin, double zmax
		   	   ) const;

		/// Assigns diffusion scaling info for the voxel junctions.
		void setDiffScale( const CubeMesh* other,
			vector< VoxelJunction >& ret ) const;
		/// Assigns volume info for the voxel junctions.
		void setJunctionVol( const CubeMesh* other,
			vector< VoxelJunction >& ret ) const;

		void updateM2s();
		void updateS2m();
		//////////////////////////////////////////////////////////////////
		static const unsigned int EMPTY;
		static const unsigned int SURFACE;
		static const unsigned int ABUTX;
		static const unsigned int ABUTY;
		static const unsigned int ABUTZ;
		static const unsigned int MULTI;

		static const Cinfo* initCinfo();

	private:
		bool isToroid_; ///Flag: Should the ends loop around mathemagically?
		bool preserveNumEntries_; ///Flag: Should dx change or nx, with vol?
		bool alwaysDiffuse_; ///Flag: should all voxels diffuse to any tgt?

		double x0_; /// coords
		double y0_; /// coords
		double z0_; /// coords

		double x1_; /// coords
		double y1_; /// coords
		double z1_; /// coords

		double dx_; /// Cuboid edge
		double dy_; /// Cuboid edge
		double dz_; /// Cuboid edge

		unsigned int nx_; /// # of entries in x in surround volume
		unsigned int ny_; /// # of entries in y in surround volume
		unsigned int nz_; /// # of entries in z in surround volume

		/**
		 * For spherical mesh, coords are xyz r0 r1 theta0 theta1 phi0 phi1
		 * For Cylindrical mesh, coords are x1y1z1 x2y2z2 r0 r1 phi0 phi1
		 * For cuboid mesh, coords are x1y1z1 x2y2z2
		 * For tetrahedral mesh, coords are x1y1z1 x2y2z2 x3y3z3 x4y4z4
		 * Later worry about planar meshes. Those would be surfaces
		 */

		/**
		 * Mesh to Space lookup. Indexed by linear mesh index, from 0 to
		 * number of actual mesh entries (occupied cuboids). Returns
		 * spatial index, from 0 to nx * ny * nz - 1.
		 * Needed whenever the cuboid mesh is not filling the entire volume
		 * of the cube, that is, in most cases.
		 */
		vector< unsigned int > m2s_;

		/**
		 * Space to Mesh lookup. Indexed by spatial index, from
		 * 0 to nx * ny * nz - 1. Specifically, point x y z is indexed as
		 * ( z * ny + y ) * nx + x. Returns mesh index to look up molecules
		 * etc in the specific volume. In case the spatial location is
		 * outside the included volume of the mesh, returns ~0.
		 */
		vector< unsigned int > s2m_;

		/**
		 * Vector of spatial meshIndices comprising surface of volume in
		 * CubeMesh.
		 */
		vector< unsigned int > surface_;
};

#endif	// _CUBE_MESH_H
