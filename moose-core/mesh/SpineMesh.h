/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SPINE_MESH_H
#define _SPINE_MESH_H

/**
 * The SpineMesh sets up the diffusion geometries for dendritic spines.
 * It is filled by a message from a NeuroMesh that contains information
 * about the matching voxel on the NeuroMesh, and the compartment Ids for
 * the spine shaft and head.
 * The SpineMesh can further generate a PSD mesh with the info for the
 * PSD geometries.
 * This assumes that the each spine is a single voxel: single diffusive
 * compartment, well-stirred. The shaft is treated as zero volume
 * diffusion barrier to the dendrite.
 * The PSD is a separate compt with its own diffusion coupling to the
 * spine head.
 */
class SpineMesh: public MeshCompt
{
	public:
		SpineMesh();
		SpineMesh( const SpineMesh& other );
		~SpineMesh();
//		SpineMesh& operator=( const SpineMesh& other );
		//////////////////////////////////////////////////////////////////
		//  Utility func
		//////////////////////////////////////////////////////////////////
		/**
		 * Recomputes all local coordinate and meshing data following
		 * a change in any of the coord parameters
		 */

		void updateCoords();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////
		/**
		 * This function returns the diffusively connected parent voxel
		 * within the current (spine) mesh. Since each spine is treated
		 * as an independed voxel, there is no such voxel, so we return
		 * -1U for each spine. Note that there is a separate function
		 * that returns the parentVoxel referred to the NeuroMesh that
		 * this spine sits on.
		 */
		vector< unsigned int > getParentVoxel() const;
		/**
 		 * Returns index of voxel on NeuroMesh to which this spine is
		 * connected.
		 */
		vector< unsigned int > getNeuronVoxel() const;

		/**
		 * Returns vector of Ids of electrical compts that map to
		 * respective voxels in SpineMesh
		 */
		vector< Id > getElecComptMap() const;

		/// Returns index of first voxel mapping to elec compt
		vector< unsigned int > getStartVoxelInCompt() const;
		/// Returns index of end voxel mapping to elec compt, just first+1
		vector< unsigned int > getEndVoxelInCompt() const;

		//////////////////////////////////////////////////////////////////
		// FieldElement assignment stuff for MeshEntries
		//////////////////////////////////////////////////////////////////

		/// Virtual function to return MeshType of specified entry.
		unsigned int getMeshType( unsigned int fid ) const;
		/// Virtual function to return dimensions of specified entry.
		unsigned int getMeshDimensions( unsigned int fid ) const;
		/// Virtual function to return volume of mesh Entry.
		double getMeshEntryVolume( unsigned int fid ) const;
		/// Virtual function to assign volume of mesh Entry.
		void setMeshEntryVolume( unsigned int fid, double volume );
		/// Virtual function to return coords of mesh Entry.
		vector< double > getCoordinates( unsigned int fid ) const;
		/// Virtual function to return diffusion X-section area
		vector< double > getDiffusionArea( unsigned int fid ) const;
		/// Virtual function to return scale factor for diffusion. 1 here.
		vector< double > getDiffusionScaling( unsigned int fid ) const;
		/// Vol of all mesh Entries including abutting diff-coupled voxels
		double extendedMeshEntryVolume( unsigned int fid ) const;

		//////////////////////////////////////////////////////////////////
		/**
		 * Inherited virtual func. Returns number of MeshEntry in array
		 */
		unsigned int innerGetNumEntries() const;
		/// Inherited virtual func.
		void innerSetNumEntries( unsigned int n );

		/// Returns # of dimensions, always 3 here. Inherited pure virt func
		unsigned int innerGetDimensions() const;

		/// Inherited virtual func.
		double vGetEntireVolume() const;

		/// Inherited virtual func
		bool vSetVolumeNotRates( double volume );

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		/// Virtual func to make a mesh with specified Volume and numEntries
		void innerBuildDefaultMesh( const Eref& e,
			double volume, unsigned int numEntries );

		/*
		void innerHandleRequestMeshStats(
			const Eref& e,
			const SrcFinfo2< unsigned int, vector< double > >*
				meshStatsFinfo
		);
			*/

		void innerHandleNodeInfo(
			const Eref& e,
			unsigned int numNodes, unsigned int numThreads );

		void handleSpineList(
			const Eref& e,
			vector< Id > shaft, vector< Id > head,
			vector< unsigned int > parentVoxel );

		void transmitChange( const Eref& e );

		void buildStencil();

		//////////////////////////////////////////////////////////////////
		// inherited virtual funcs for Boundary
		//////////////////////////////////////////////////////////////////

		void matchMeshEntries( const ChemCompt* other,
			vector< VoxelJunction > & ret ) const;

		void matchNeuroMeshEntries( const ChemCompt* other,
			vector< VoxelJunction > & ret ) const;

		void matchCubeMeshEntries( const ChemCompt* other,
			vector< VoxelJunction > & ret ) const;

		void matchSpineMeshEntries( const ChemCompt* other,
			vector< VoxelJunction > & ret ) const;

		/**
		 * This works a little different from other subclass versions of
		 * the function. It finds the index of the
		 * mesh entry whose centre is closest to the specified coords,
		 * and returns the distance to the centre.
		 * Doesn't worry about whether this distance is inside or outside
		 * cell.
		 */
		double nearest( double x, double y, double z,
						unsigned int& index ) const;

		void indexToSpace( unsigned int index,
						double& x, double& y, double& z ) const;

		const vector< double >& vGetVoxelVolume() const;
		const vector< double >& vGetVoxelMidpoint() const;
		const vector< double >& getVoxelArea() const;
		const vector< double >& getVoxelLength() const;
		//////////////////////////////////////////////////////////////////
		// Utility function for tests
		const vector< SpineEntry >& spines() const;
		//////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();

	private:
		/**
		 * These do the actual work.
		 */
		vector< SpineEntry > spines_;

		/**
		 * Decides how finely to subdivide diffLength_ or radius or cubic
		 * mesh side when computing surfacearea of intersections with
		 * CubeMesh. Defaults to 0.1.
		 */
		double surfaceGranularity_;

		/**
		 * Volscale pre-calculations for each MeshEntry.
		 * vs = #molecules / vol
		 * where vol is expressed in m^3.
		 */
		vector< double > vs_;

		/**
		 * Mesh junction area pre-calculations for each MeshEntry.
		 * This is the cross-section area of the middle of each voxel.
		 */
		vector< double > area_;

		/// Pre-calculation of length of each MeshEntry
		vector< double > length_;
};


#endif	// _SPINE_MESH_H
