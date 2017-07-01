/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _DSOLVE_H
#define _DSOLVE_H

/**
 * The Dsolve manages a large number of pools, each inhabiting a large
 * number of voxels that are shared for all the pools.
 * Each pool is represented by an array of concs, one for each voxel.
 * Each such array is kept on a single node for efficient solution.
 * The different pool arrays are assigned to different nodes for balance.
 * All pool arrays
 * We have the parent Dsolve as a global. It constructs the diffusion
 * matrix from the NeuroMesh and generates the opvecs.
 * We have the child DiffPoolVec as a local. Each one contains a
 * vector of pool 'n' in each voxel, plus the opvec for that pool. There
 * is an array of DiffPoolVecs, one for each species, and we let the
 * system put each DiffPoolVec on a suitable node for balancing.
 * Some DiffPoolVecs are for molecules that don't diffuse. These
 * simply have an empty opvec.
 */
class Dsolve: public ZombiePoolInterface
{
	public:
		Dsolve();
		~Dsolve();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////
		unsigned int getNumVarPools() const;

		void setStoich( Id id );
		Id getStoich() const;
		void setCompartment( Id id );
		// Defined in base class. Id getCompartment() const;
		void setDsolve( Id id ); /// Dummy, inherited but not used.

		void setPath( const Eref& e, string path );
		string getPath( const Eref& e ) const;

		unsigned int getNumVoxels() const;
		/// Inherited virtual.
		void setNumAllVoxels( unsigned int numVoxels );

		vector< double > getNvec( unsigned int pool ) const;
		void setNvec( unsigned int pool, vector< double > vec );

		/// LookupFied for examining cross-solver diffusion terms.
		double getDiffVol1( unsigned int voxel ) const;
		void setDiffVol1( unsigned int voxel, double vol );
		double getDiffVol2( unsigned int voxel ) const;
		void setDiffVol2( unsigned int voxel, double vol );
		double getDiffScale( unsigned int voxel ) const;
		void setDiffScale( unsigned int voxel, double scale );

		//////////////////////////////////////////////////////////////////
		// Dest Finfos
		//////////////////////////////////////////////////////////////////
		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref& e, ProcPtr p );

		/**
		 * Builds junctions between Dsolves handling NeuroMesh, SpineMesh,
		 * and PsdMesh. Must only be called from the one handling the
		 * NeuroMesh.
		 * These junctions handle diffusion between different meshes.
		 * Note that a single NeuroMesh may contact many spines which are
		 * all in a single SpineMesh. Likewise each spine has a single
		 * PSD, but there are many spines in the SpineMesh and matching
		 * psds in the PsdMesh. Finally, note that
		 * there may be many molecules which diffuse across each diffusion
		 * junction.
		 */
		void buildNeuroMeshJunctions( const Eref& e, Id spineD, Id psdD );

		/**
		 * Builds junctions between current Dsolve and another. For this
		 * to work the respective meshes must be compatible.
		 * These junctions handle diffusion between different meshes.
		 */
		void buildMeshJunctions( const Eref& e, Id other );

		/**
		 * buildMeshJunctions is the inner utility function for building
		 * the junction between any specified pair of Dsolves.
		 * Note that it builds the junction on the 'self' Dsolve.
		 */
		static void innerBuildMeshJunctions( Id self, Id other );

		/**
		 * Computes flux through a junction between diffusion solvers.
		 * Most used at junctions on spines and PSDs, but can also be used
		 * when a given diff solver is decomposed. At present the lookups
		 * on the other diffusion solver assume that the data is on the
		 * local node. Once this works well I can figure out how to do
		 * across nodes.
		 */
		void calcJunction( const DiffJunction& jn, double dt );
		//////////////////////////////////////////////////////////////////
		// Inherited virtual funcs from ZombiePoolInterface
		//////////////////////////////////////////////////////////////////
		double getNinit( const Eref& e ) const;
		void setNinit( const Eref& e, double value );
		double getN( const Eref& e ) const;
		void setN( const Eref& e, double value );
		double getDiffConst( const Eref& e ) const;
		void setDiffConst( const Eref& e, double value );
		void setMotorConst( const Eref& e, double value );

		void setNumPools( unsigned int num );
		unsigned int getNumPools() const;
		unsigned int getNumLocalVoxels() const;
		VoxelPoolsBase* pools( unsigned int i );
		double volume( unsigned int i ) const;

		void getBlock( vector< double >& values ) const;
		void setBlock( const vector< double >& values );

		// This one isn't used in Dsolve, but is defined as a dummy.
		void setupCrossSolverReacs(
					const map< Id, vector< Id > >& xr, Id otherStoich );
		void setupCrossSolverReacVols(
			const vector< vector< Id > >& subCompts,
			const vector< vector< Id > >& prdCompts );
		void filterCrossRateTerms( const vector< pair< Id, Id > >& xrt );

		// Do any updates following a volume or rate constant change.
		void updateRateTerms( unsigned int index );
		//////////////////////////////////////////////////////////////////
		// Model traversal and building functions
		//////////////////////////////////////////////////////////////////
		unsigned int convertIdToPoolIndex( const Eref& e ) const;
		unsigned int getPoolIndex( const Eref& e ) const;

		/**
		 * Fills in poolMap_ using elist of objects found when the
		 * 'setPath' function is executed. temp is returned with the
		 * list of PoolBase objects that exist on the path.
		 */
		void makePoolMapFromElist( const vector< ObjId >& elist,
						vector< Id >& temp );

		/**
		 * This key function does the work of setting up the Dsolve.
		 * Should be called after the compartment has been attached to
		 * the Dsolve, and the stoich is assigned.
		 * Called during the setStoich function.
		 */
		void build( double dt );
		void rebuildPools();

		/**
		 * Utility func for debugging: Prints N_ matrix
		 */
		void print() const;

		//////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	private:

		/// Path of pools managed by Dsolve, may include other classes too.
		string path_;

		/// Timestep used by diffusion calculations.
		double dt_;

		unsigned int numTotPools_;
		unsigned int numLocalPools_;
		unsigned int poolStartIndex_;
		unsigned int numVoxels_;

		/// Internal vector, one for each pool species managed by Dsolve.
		vector< DiffPoolVec > pools_;

		/// smallest Id value for poolMap_
		unsigned int poolMapStart_;

		/// Looks up pool# from pool Id, using poolMapStart_ as offset.
		vector< unsigned int > poolMap_;

		/**
		 * Lists all the diffusion junctions managed by this Dsolve.
		 * Each junction entry provides the info needed to do the
		 * numerical integration for flux between the Dsolves.
		 */
		vector< DiffJunction > junctions_;
};


#endif	// _DSOLVE_H
