/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MESH_COMPT_H
#define _MESH_COMPT_H

/**
 * The MeshCompt represents the base class for all compartments which
 * are set up as a meshes. These include CubeMesh, CylMesh and NeuroMesh.
 * The MeshCompt handles the stencil that is the core of all the meshes.
 * The stencil specifies which compartments diffuse into each other, and by
 * how much.
 */
class MeshCompt: public ChemCompt
{
	public:
		MeshCompt();
		~MeshCompt();

		//////////////////////////////////////////////////////////////////
		/**
		 * Utility function to return volume of any voxel including those
		 * diffusively coupled and aubtting the present volume.
		 */
		double extendedMeshEntryVolume( unsigned int fid ) const;

		/// Inherited virtual function to clear the vector of MeshEntryVolume
		void clearExtendedMeshEntryVolume();

		/// virtual func implemented here.
		void innerResetStencil();

		/// Derived function to return SparseMatrix-style row info for
		/// specified mesh entry.
		unsigned int getStencilRow( unsigned int meshIndex,
				const double** entry, const unsigned int** colIndex ) const;

		/// Returns entire sparse matrix of mesh. Used by diffusion solver.
		const SparseMatrix< double >& getStencil() const;

		/// Looks up stencil to return vector of indices of coupled voxels.
		vector< unsigned int > getNeighbors( unsigned int fid ) const;

		/**
		 * Looks up stencil to return vector of diffusion coupling to
		 * neighbor voxels.
		 */
		vector< double > innerGetStencilRate( unsigned int row ) const;

		void addRow( unsigned int index, const vector< double >& entry,
			const vector< unsigned int >& colIndex );

		void setStencilSize( unsigned int numRows, unsigned int numCols );

		//////////////////////////////////////////////////////////////////

		/// Add boundary voxels to stencil for cross-solver junctions
		void extendStencil(
				const ChemCompt* other, const vector< VoxelJunction >& vj );

		virtual vector< unsigned int > getParentVoxel() const = 0;
		virtual const vector< double >& getVoxelArea() const = 0;
		virtual const vector< double >& getVoxelLength() const = 0;

	private:

		/// Handles the core stencil for own vol
		SparseMatrix< double > coreStencil_;

		/// Handles stencil for core + abutting voxels
		SparseMatrix< double > m_;

		/**
		 * vector of meshEntryVolumes for abutting surfaces,
		 * needed to compute
		 * diffusion rates across junctions.
		 * Indexed from zero.
		 */
		vector< double > extendedMeshEntryVolume_;
};

// Helper class for setting up and sorting rows of matrix entries.
class Ecol {
	public:
		Ecol( double e, unsigned int col )
			: e_( e ), col_( col )
		{;}

		Ecol()
			: e_( 0 ), col_( 0 )
		{;}

		bool operator<( const Ecol& other ) const
		{
			return col_ < other.col_;
		}

		double e_;
		double col_;
};

#endif	// _MESH_COMPT_H
