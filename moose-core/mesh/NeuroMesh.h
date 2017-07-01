/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _NEURO_MESH_H
#define _NEURO_MESH_H

/**
 * The NeuroMesh represents sections of a neuron whose spatial attributes
 * are obtained from a neuronal model.
 * Like the CylMesh, this is pseudo-1 dimension: Only the
 * axial dimension is considered for diffusion and subdivisions. Branching
 * is also handled.
 *
 *
 * Dendritic spines typically contain different reaction systems from the
 * dendrite, but each spine has the same reactions. So they deserve
 * their own mesh: SpineMesh.
 * The idea is the the SpineMesh has just the spine head compartment,
 * which duplicate the same reactions, but does not diffuse to other
 * spine heads.
 * Instead it has an effective diffusion constant to the parent
 * dendrite compartment, obtained by treating the spine neck as a
 * diffusion barrier with zero volume.
 */
class NeuroMesh: public MeshCompt
{
	public:
		NeuroMesh();
		NeuroMesh( const NeuroMesh& other );
		~NeuroMesh();
		NeuroMesh& operator=( const NeuroMesh& other );
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
		 * This overloaded function sets up a presumed contiguous set of
 		 * compartments, complains if they are not contiguous due to the
		 * check in NeuroNode::traverse.
 		 *
		 * The 'path' argument specifies a wildcard list of compartments,
		 * which can be also a comma-separated explicit list. Does not
		 * have to be in any particular order.
 		 */
		void setSubTreePath( const Eref& e, string path	);
		string getSubTreePath( const Eref& e ) const;

		/**
		 * The SubTree is a set of compartments to model.
		 * The first entry is the root of the tree, closest to the soma.
		 * The system does handle non-contiguous sets.
		 */
		void setSubTree( const Eref& e, vector< ObjId > compartments );
		vector< ObjId > getSubTree( const Eref& e ) const;

		/**
		 * Transmits the information about spines to SpineMesh and
		 * PsdMesh.
		 */
		void transmitSpineInfo( const Eref& e );


		/**
		 * Flag. True if NeuroMesh should configure a separate SpineMesh.
		 * The process is that both the NeuroMesh and SpineMesh should have
		 * been created, and a spineList message sent from the NeuroMesh
		 * to the SpineMesh. This may cascade down to PsdMesh.
		 */
		void setSeparateSpines( bool v );
		bool getSeparateSpines() const;

		unsigned int getNumSegments() const;
		unsigned int getNumDiffCompts() const;

		void setDiffLength( double v );
		double getDiffLength() const;

		void setGeometryPolicy( string v );
		string getGeometryPolicy() const;

		unsigned int innerGetDimensions() const;

		vector< Id > getElecComptList() const;
		vector< Id > getElecComptMap() const;
		vector< unsigned int > getStartVoxelInCompt() const;
		vector< unsigned int > getEndVoxelInCompt() const;
		vector< int > getSpineVoxelOnDendVoxel() const;
		vector< unsigned int > getDendVoxelsOnCompartment( ObjId compt ) const;
		vector< unsigned int > getSpineVoxelsOnCompartment( ObjId compt ) const;

		vector< unsigned int > getParentVoxel() const;
		const vector< double >& vGetVoxelVolume() const;
		const vector< double >& vGetVoxelMidpoint() const;
		const vector< double >& getVoxelArea() const;
		const vector< double >& getVoxelLength() const;

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
		/// Vol of all mesh Entries including abutting diff-coupled voxels
		double extendedMeshEntryVolume( unsigned int fid ) const;

		//////////////////////////////////////////////////////////////////
		/**
		 * Inherited virtual func. Returns number of MeshEntry in array
		 */
		unsigned int innerGetNumEntries() const;
		/// Inherited virtual func.
		void innerSetNumEntries( unsigned int n );

		/**
		 * Inherited virtual func. Returns volume of soma and whole
		 * dendritic tree of neuron, excluding spines. Any axonal
		 * compartments are also included.
		 */
		double vGetEntireVolume() const;

		/**
		 * Inherited virtual func.
		 * In the NeuroMesh this carries out a rescaling on all dimensions.
		 * The length and diameter of each compartment are scaled by
		 * the same factor = volscale^(1/3)
		 * The rescaling carries through to the spines and PSDs, which
		 * are also updated. They are not permitted to
		 * change their own volumes.
		 */
		bool vSetVolumeNotRates( double volume );

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		/// Virtual func to make a mesh with specified size and numEntries
		void innerBuildDefaultMesh( const Eref& e,
			double size, unsigned int numEntries );

		void innerHandleRequestMeshStats(
			const Eref& e,
			const SrcFinfo2< unsigned int, vector< double > >*
				meshStatsFinfo
		);

		void innerHandleNodeInfo(
			const Eref& e,
			unsigned int numNodes, unsigned int numThreads );

		// void transmitChange( const Eref& e, double oldVol );

		/**
		 * Helper function for buildStencil, calculates diffusion term
		 * adx for rate between current compartment curr, and parent.
		 * By product: also passes back parent compartment index.
		 */
		double getAdx( unsigned int curr, unsigned int& parentFid ) const;

		/// Utility function to set up Stencil for diffusion in NeuroMesh
		void buildStencil();

		//////////////////////////////////////////////////////////////////
		// inherited virtual funcs for Boundary
		//////////////////////////////////////////////////////////////////

		void matchMeshEntries( const ChemCompt* other,
			vector< VoxelJunction > & ret ) const;

		void matchCubeMeshEntries( const ChemCompt* other,
			vector< VoxelJunction > & ret ) const;

		void matchNeuroMeshEntries( const ChemCompt* other,
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


		//////////////////////////////////////////////////////////////////
		// Utility functions for building tree.
		//////////////////////////////////////////////////////////////////

		/**
		 * Puts in a dummy node between parent and self. Used to
		 * set up the correct size of proximal compartments.
		 */
		void insertSingleDummy( unsigned int parent, unsigned int self,
			   double x, double y, double z	);

		/**
		 * Puts in all the required dummy nodes for the tree.
		 */
		void insertDummyNodes();

		/// This shuffles the nodes_ vector to put soma node at the start
		Id putSomaAtStart( Id origSoma, unsigned int maxDiaIndex );

		/**
		 * buildNodeTree: This connects up parent and child nodes
		 * and if needed inserts dummy nodes to build up the model tree.
		 */
		void buildNodeTree( const map< Id, unsigned int >& comptMap );

		/**
		 * Returns true if it finds a compartment name that looks like
		 * it ought to be on a spine. It filters out the names
		 * "neck", "shaft", "spine" and "head".
		 * The latter two are classified into the head_ vector.
		 * The first two are classified into the shaft_ vector.
		 */
		bool filterSpines( Id compt );
		/**
		 * converts the parents_ vector from identifying the parent
		 * NeuroNode to identifying the parent voxel, for each shaft entry.
 		 */
		void updateShaftParents();

		//////////////////////////////////////////////////////////////////
		// Utility functions for testing
		// const Stencil* getStencil() const;
		const vector< NeuroNode >& getNodes() const;

		//////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();

	private:
		/**
		 * Array of geometry specifiers for each segment of the neuronal
		 * model. Includes information equivalent to
		 * the CylMesh, as well as information to tie the node back to the
		 * original 'compartment' from the neuronal model.
		 */
		vector< NeuroNode > nodes_;

		/**
		 * Path of sub-tree used for the chemical model. This is undefined
		 * if the user assigns the subtree as a vector of objects, which
		 * is the case when using rdesigneur.
		 */
		string subTreePath_;

		/**
		 * nodeIndex_[fid_for_MeshEntry].
		 * Looks up index of NeuroNode from the fid of each MeshEntry.
		 * In other words, node# = nodeIndex_[ voxel# ].
		 * This is needed because there are typically many voxels in each
		 * electrical compartment of the neuron. The nodes_ map closely to
		 * the electrical compartments.
		 */
		vector< unsigned int > nodeIndex_;

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

		// double size_; /// Total Volume
		double diffLength_;	/// Max permitted length constant for diffusion

		/**
		 * Flag. True if NeuroMesh should configure a separate SpineMesh.
		 * The process is that both the NeuroMesh and SpineMesh should have
		 * been created, and a spineList message sent from the NeuroMesh
		 * to the SpineMesh.
		 */
		bool separateSpines_;

		string geometryPolicy_;

		/**
		 * Decides how finely to subdivide diffLength_ or radius or cubic
		 * mesh side when computing surfacearea of intersections with
		 * CubeMesh. Defaults to 0.1.
		 */
		double surfaceGranularity_;

		/**
		 * The shaft vector and the matching head vector track the dendritic
		 * spines. The parent is the voxel to which the spine  is attached.
		 */
		vector< Id > shaft_; /// Id of shaft compartment.
		vector< Id > head_;	/// Id of head compartment
		vector< unsigned int > parent_; /// Index of parent voxel of spines
		/**
		 * Index of parent voxel of each voxel. The root voxel has a
		 * parent of -1.
		 */
		vector< unsigned int > parentVoxel_;
};


#endif	// _NEURO_MESH_H
