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
#include "VoxelJunction.h"
#include "Boundary.h"
// #include "Stencil.h"
#include "MeshEntry.h"
#include "ChemCompt.h"
#include "MeshCompt.h"

static const unsigned int EMPTY = ~0;

//////////////////////////////////////////////////////////////////
// Class stuff.
//////////////////////////////////////////////////////////////////
MeshCompt::MeshCompt()
{
	;
}

MeshCompt::~MeshCompt()
{
	;
}

/// Virtual function to return volume of mesh Entry, including
// for diffusively coupled voxels from other solvers.
double MeshCompt::extendedMeshEntryVolume( unsigned int index ) const
{
	assert( index < extendedMeshEntryVolume_.size() );
	return extendedMeshEntryVolume_[ index ];
}

/// Inherited virtual function to clear the vector of MeshEntryVolume
void MeshCompt::clearExtendedMeshEntryVolume()
{
	extendedMeshEntryVolume_.clear();
}

/// virtual func implemented here.
void MeshCompt::innerResetStencil()
{
	m_ = coreStencil_;
}

unsigned int MeshCompt::getStencilRow( unsigned int meshIndex,
			const double** entry, const unsigned int** colIndex ) const
{
		return m_.getRow( meshIndex, entry, colIndex );
}

const SparseMatrix< double >& MeshCompt::getStencil() const
{
		return coreStencil_;
}


//////////////////////////////////////////////////////////////////

/// Virtual function to return info on Entries connected to this one
vector< unsigned int > MeshCompt::getNeighbors( unsigned int row ) const
{
	const double* entry;
	const unsigned int *colIndex;

	unsigned int n = m_.getRow( row, &entry, &colIndex );

	vector< unsigned int > ret;
	ret.insert( ret.end(), colIndex, colIndex + n );

	return ret;
}

//////////////////////////////////////////////////////////////////
// Inherited virtual funcs for field access
//////////////////////////////////////////////////////////////////
vector< double > MeshCompt::innerGetStencilRate( unsigned int row ) const
{
	const double* entry;
	const unsigned int *colIndex;

	unsigned int n = m_.getRow( row, &entry, &colIndex );

	vector< double > ret;
	ret.insert( ret.end(), entry, entry + n );

	return ret;
}

void MeshCompt::addRow( unsigned int index,
	const vector< double >& entry,
	const vector< unsigned int >& colIndex
	)
{
	coreStencil_.addRow( index, entry, colIndex );
}

void MeshCompt::setStencilSize( unsigned int numRows, unsigned int numCols )
{
	coreStencil_.clear();
	coreStencil_.setSize( numRows, numCols );
}


/**
 * extendStencil adds voxels to the current stencil m_, to build up a
 * monolithic stencil that also handles the entries just past all the
 * boundaries.
 * This function may be called many times to deal with the addition of
 * multiple junctions. Before the first of these calls, the m_ matrix
 * should be set to the coreStencil_.
 */
void MeshCompt::extendStencil(
	const ChemCompt* other, const vector< VoxelJunction >& vj )
{
	// Maps from remote meshIndex (in vj) to local index of proxy voxel.
	map< unsigned int, unsigned int > meshMap;
	map< unsigned int, unsigned int >::iterator mmi;

	// Maps from local index of proxy voxel back to remote meshIndex.
	vector< unsigned int > meshBackMap;


	unsigned int coreSize = coreStencil_.nRows();
	unsigned int oldSize = m_.nRows();
	unsigned int newSize = oldSize;

	/// Organizes vj by voxel, that is, by row.
	vector< vector< VoxelJunction > > vvj( coreSize );

	for ( vector< VoxelJunction >::const_iterator
					i = vj.begin(); i != vj.end(); ++i ) {
		mmi = meshMap.find( i->second );
		if ( mmi == meshMap.end() ) {
			assert( i->first < coreSize );
			meshBackMap.push_back( i->second );
			meshMap[i->second] = newSize++;
			vvj[i->first].push_back( *i );
		}
	}
	vector< vector< VoxelJunction > > vvjCol( newSize );
	SparseMatrix< double > oldM = m_;
	m_.clear();
	m_.setSize( newSize, newSize );
	for ( unsigned int i = 0; i < newSize; ++i ) {
		vector< VoxelJunction > temp;
		if ( i < oldSize ) { // Copy over old matrix.
			const double* entry;
			const unsigned int* colIndex;
			unsigned int num = oldM.getRow( i, &entry, &colIndex );
			temp.resize( num );
			for ( unsigned int j = 0; j < num; ++j ) {
				temp[j].first = colIndex[j];
				temp[j].diffScale = entry[j];
			}
		}
		if ( i < coreSize ) { // Set up diffusion into proxy voxels.
			for ( vector< VoxelJunction >::const_iterator
				j = vvj[i].begin(); j != vvj[i].end(); ++j )
			{
				unsigned int row = j->first;
				assert( row == i );
				unsigned int col = meshMap[j->second];
				assert( col >= oldSize );
				temp.push_back(
						VoxelJunction( col, EMPTY, j->diffScale ) );
				vvjCol[col].push_back(
						VoxelJunction( row, EMPTY, j->diffScale ) );
			}
		}
		if ( i >= oldSize ) { // Set up diffusion from proxy to old voxels
			for ( vector< VoxelJunction >::const_iterator
				j = vvjCol[i].begin(); j != vvjCol[i].end(); ++j )
			{
				temp.push_back( *j );
			}
		}
		// Now we've filled in all the VoxelJunctions for the new row.
		sort( temp.begin(), temp.end() );
		vector< double > e( temp.size() );
		vector< unsigned int > c( temp.size() );
		for ( unsigned int j = 0; j < temp.size(); ++j ) {
			e[j] = temp[j].diffScale;
			c[j] = temp[j].first;
		}
		m_.addRow( i, e, c );
	}

	// Fill in the volumes of the external mesh entries
	for ( vector< unsigned int>::const_iterator
			i = meshBackMap.begin(); i != meshBackMap.end(); ++i ) {
		extendedMeshEntryVolume_.push_back( other->getMeshEntryVolume( *i ) );
	}
}

//////////////////////////////////////////////////////////////////
