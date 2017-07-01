/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <math.h>
#include <vector>
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iomanip>

using namespace std;
#include "../basecode/SparseMatrix.h"
#include "../basecode/doubleEq.h"
#include "FastMatrixElim.h"

/*
const unsigned int SM_MAX_ROWS = 200000;
const unsigned int SM_MAX_COLUMNS = 200000;
const unsigned int SM_RESERVE = 8;
*/

FastMatrixElim::FastMatrixElim()
	: SparseMatrix< double >()
{;}

FastMatrixElim::FastMatrixElim( unsigned int nrows, unsigned int ncolumns )
	: SparseMatrix< double >( nrows, ncolumns )
{;}

FastMatrixElim::FastMatrixElim( const SparseMatrix< double >& orig )
	: SparseMatrix< double >( orig )
{;}

void sortByColumn(
			vector< unsigned int >& col, vector< double >& entry );
void testSorting();

//
//	static unsigned int parents[] = { 1,6,3,6,5,8,7,8,9,10,-1};
//	unsigned int numKids[] = {0,1,0,1,0,2,

static const unsigned int EMPTY_VOXEL(-1);

bool FastMatrixElim::checkSymmetricShape() const
{
	FastMatrixElim temp = *this;
	temp.transpose();
	return (
		nrows_ == temp.nrows_ &&
		ncolumns_ == temp.ncolumns_ &&
		N_.size() == temp.N_.size() &&
		rowStart_ == temp.rowStart_ &&
		colIndex_ == temp.colIndex_
	);
}

bool FastMatrixElim::operator==( const FastMatrixElim& other ) const
{
	if (
		nrows_ == other.nrows_ &&
		ncolumns_ == other.ncolumns_ &&
		N_.size() == other.N_.size() &&
		rowStart_ == other.rowStart_ &&
		colIndex_ == other.colIndex_ ) {
		for ( unsigned int i = 0; i < N_.size(); ++i )
			if ( !doubleEq( N_[i], other.N_[i] ) )
				return false;
		return true;
	}
	return false;
}

bool FastMatrixElim::isSymmetric() const
{
	FastMatrixElim temp = *this;
	temp.transpose();
	return ( temp == *this );
}

/**
 * Scans the current matrix starting from index 0, to build tree of parent
 * voxels using the contents of the sparase matrix to indicate
 * connectivity. Emits warning noises and returns an empty vector if
 * the entire tree cannot be traversed, or
 * if the current matrix is not tree-like.
 * This still doesn't work. Fails because it cannot detect branches in
 * the tree, where sibling compartments would have cross talk.
 * So we still rely on getParentVoxel function.
bool FastMatrixElim::buildTree( unsigned int parentRow,
				vector< unsigned int >& parentVoxel ) const
{
	assert( nRows() == nColumns() );
	if ( parentRow == 0 ) {
		parentVoxel.assign( nRows, EMPTY_VOXEL );
		if ( !checkSymmetricShape() )
			return false; // shape of matrix should be symmetric.
	}
	const double* entry;
	const unsigned int* colIndex;
	unsigned int numEntries = getRow( parentRow, &entry, &colIndex );
	for ( unsigned int j = 0; j < numEntries; ++j ) {
		if ( colIndex[j] <= parentRow ) continue; // ignore lower diagonal
		if ( parentVoxel[ colIndex[j] ] == EMPTY_VOXEL ) {
			parentVoxel[ colIndex[j] ] = parentRow;
		} else {
			if ( parentVoxel[ colIndex[j] ] == parentVoxel[ parentRow ] )
				continue; // OK, these are sibling compartments
			return false; // Error: the matrix isn't a clean tree.
		}
	}
	for ( unsigned int j = 0; j < numEntries; ++j ) {
		if ( colIndex[j] <= parentRow ) continue;
		if !buildTree( colIndex[j], parentVoxel )
			return false; // Propagate error from child row.
	}
	if ( parentRow > 0 )
		return true;
	// Now done with all child branches, test if whole matrix is in tree.
	for ( unsigned int j = 1; j < nRows(); ++j )
		if ( parentVoxel[j] == EMPTY_VOXEL )
			return false; // One or more rows is disconnected from tree

	return true; // All is well, have yourself a nice parentVoxel tree.
}
 */

/**
 * Reorders rows and columns to put the matrix in the form suitable for
 * rapid single-pass inversion. Returns 0 on failure, but at this
 * point I don't have a proper test for this.
 */
bool FastMatrixElim::hinesReorder(
		const vector< unsigned int >& parentVoxel,
		vector< unsigned int >& lookupOldRowFromNew
   	)
{
	// First we fill in the vector that specifies the old row number
	// assigned to each row of the reordered matrix.
	assert( parentVoxel.size() == nrows_ );
	lookupOldRowFromNew.clear();
	vector< unsigned int > numKids( nrows_, 0 );
	vector< bool > rowPending( nrows_, true );
	unsigned int numDone = 0;
	for ( unsigned int i = 0; i < nrows_; ++i ) {
		if ( parentVoxel[i] != EMPTY_VOXEL )
			numKids[ parentVoxel[i] ]++;
	}
	while ( numDone < nrows_ ) {
		for ( unsigned int i = 0; i < nrows_; ++i ) {
			if ( rowPending[i] && numKids[i] == 0 ) {
				lookupOldRowFromNew.push_back( i );
				rowPending[i] = false;
				numDone++;
				unsigned int pa = parentVoxel[i];
				// root parent is EMPTY_VOXEL
				while ( pa != EMPTY_VOXEL && numKids[pa] == 1 ) {
					assert( rowPending[pa] );
					rowPending[pa] = false;
					numDone++;
					lookupOldRowFromNew.push_back( pa );
					pa = parentVoxel[pa];
				}
				if ( pa != EMPTY_VOXEL ) {
					assert( numKids[pa] > 0 );
					numKids[pa]--;
				}
			}
		}
	}

	/*
	cout << setprecision(4);
	cout << "oldRowFromNew= {" ;
	for ( unsigned int i = 0; i < nrows_; ++i )
		cout << lookupOldRowFromNew[i] << ", ";
	cout << "}\n";
	*/
	// Then we fill in the reordered matrix. Note we need to reorder
	// columns too.
	shuffleRows( lookupOldRowFromNew );
	return true;
}


// Fill in the reordered matrix. Note we need to reorder columns too.
void FastMatrixElim::shuffleRows(
				const vector< unsigned int >& lookupOldRowFromNew )
{
	vector< unsigned int > lookupNewRowFromOld( nrows_ );
	for ( unsigned int i = 0; i < nrows_; ++i )
		lookupNewRowFromOld[ lookupOldRowFromNew[i] ] = i;

	FastMatrixElim temp = *this;
	clear();
	setSize( temp.nrows_, temp.nrows_ );
	for ( unsigned int i = 0; i < lookupOldRowFromNew.size(); ++i ) {
		vector< unsigned int > c;
		vector< double > e;
		unsigned int num = temp.getRow( lookupOldRowFromNew[i], e, c );
		vector< unsigned int > newc( num );
		vector< double > newe( num );
		for ( unsigned int j = 0; j < num; ++j ) {
			newc[j] = lookupNewRowFromOld[ c[j] ];
			newe[j] = e[j];
		}
		// Now we need to sort the new row entries in increasing col order.
		/*
		sortByColumn( newc, newe );
		addRow( i, newe, newc );
		*/
		sortByColumn( newc, e );
		addRow( i, e, newc );
	}
}

void sortByColumn( vector< unsigned int >& col, vector< double >& entry )
{
	unsigned int num = col.size();
	assert( num == entry.size() );
	// Stupid bubble sort, as we only have up to 5 entries and need to
	// sort both the col and reorder the entries by the same sequence.
	for ( unsigned int i = 0; i < num; ++i ) {
		for ( unsigned int j = 1; j < num; ++j ) {
			if ( col[j] < col[j-1] ) {
				unsigned int temp = col[j];
				col[j] = col[j-1];
				col[j-1] = temp;
				double v = entry[j];
				entry[j] = entry[j-1];
				entry[j-1] = v;
			}
		}
	}
}


void FastMatrixElim::makeTestMatrix(
				const double* test, unsigned int numCompts )
{
	setSize( numCompts, numCompts );
	vector< double > row( numCompts, ~0 );
	for ( unsigned int i = 0; i < numCompts; ++i ) {
		for ( unsigned int j = 0; j < numCompts; ++j ) {
			unsigned int k = i * numCompts + j;
			if ( test[k] < 0.1 ) {
			} else {
				N_.push_back( test[k] );
				colIndex_.push_back( j );
			}
		}
		rowStart_[i + 1] = N_.size();
	}
}

/*
I need an outer function to fill the vector of ops for forward elim.
Then I need another outer function to fill another set of ops for
back-substitution.
*/

/**
 * Builds the vector of forward ops: ratio, i, j
 * RHS[i] = RHS[i] - RHS[j] * ratio
 * This vec tells the routine which rows below have to be eliminated.
 * This includes the rows if any in the tridiagonal band and also
 * rows, if any, on branches.
 */
void FastMatrixElim::buildForwardElim( vector< unsigned int >& diag,
	vector< Triplet< double > >& fops )
{
	vector< vector< unsigned int > > rowsToElim( nrows_ );
	diag.clear();
	for ( unsigned int i = 0; i < nrows_; ++i ) {
		unsigned int rs = rowStart_[i];
		unsigned int re = rowStart_[i+1];
		for ( unsigned int j = rs; j < re; ++j ) {
			unsigned int k = colIndex_[j];
			if ( k == i ) {
				diag.push_back(j);
			} else if ( k > i ) {
				rowsToElim[ i ].push_back( k );
			}
		}
	}
	assert( diag.size() == nrows_ );
	for ( unsigned int i = 0; i < nrows_; ++i ) {
		double d = N_[diag[i]];
		unsigned int diagend = rowStart_[ i + 1 ];
		assert( diag[i] < diagend );
		vector< unsigned int >& elim = rowsToElim[i];
		for ( unsigned int j = 0; j < elim.size(); ++j ) {
			unsigned int erow = elim[j];
			if ( erow == i ) continue;
			unsigned int rs = rowStart_[ erow ];
			unsigned int re = rowStart_[ erow+1 ];
			// assert( colIndex_[rs] == i );
			double ratio = get( erow, i ) / d;
			// double ratio = N_[rs]/N_[diag[i]];
			for ( unsigned int k = diag[i]+1; k < diagend; ++k ) {
				unsigned int col = colIndex_[k];
				// findElimEntry, subtract it out.
				for ( unsigned int q = rs; q < re; ++q ) {
					if ( colIndex_[q] == col ) {
						N_[q] -= N_[k] * ratio;
					}
				}
			}
			fops.push_back( Triplet< double >( ratio, i, erow) );
		}
	}
	/*
	for ( unsigned int i = 0; i < rowsToElim.size(); ++i ) {
		cout << i << " :		";
		for ( unsigned int j = 0; j < rowsToElim[i].size(); ++j ) {
			cout << rowsToElim[i][j] << "	";
		}
		cout << endl;
	}
	for ( unsigned int i = 0; i < fops.size(); ++i ) {
		cout << "fops[" << i << "]=		" << fops[i].b_ << "	" << fops[i].c_ <<
				"	" << fops[i].a_ << endl;
	}
	*/
}

/**
 * Operations to be done on the RHS for the back sub are generated and
 * put into the bops (backward ops) vector.
 * col > row here, row is the entry being operated on, and col is given by
 * rowsToSub.
 * offDiagVal is the value on the off-diagonal at row,col.
 * diagVal is the value on the diagonal at [row][row].
 * RHS[row] = ( RHS[row] - offDiagVal * RHS[col] ) / diagVal
 */
void FastMatrixElim::buildBackwardSub( vector< unsigned int >& diag,
	vector< Triplet< double > >& bops, vector< double >& diagVal )
{
	// This vec tells the routine which rows below have to be back-subbed.
	// This includes the rows if any in the tridiagonal band and also
	// rows, if any, on branches.
	vector< vector< unsigned int > > rowsToSub( nrows_ );

	for ( unsigned int i = 0; i < nrows_; ++i ) {
		unsigned int d = diag[i] + 1;
		unsigned int re = rowStart_[i+1];
		for ( unsigned int j = d; j < re; ++j ) {
			unsigned int k = colIndex_[j];
			// At this point the row to sub is at (i, k). We need to go down
			// to the (k,k) diagonal to sub it out.
			rowsToSub[ k ].push_back( i );
		}
	}
	/*
	for ( unsigned int i = 0; i < rowsToSub.size(); ++i ) {
		cout << i << " :		";
		for ( unsigned int j = 0; j < rowsToSub[i].size(); ++j ) {
			cout << rowsToSub[i][j] << "	";
		}
		cout << endl;
	}
	*/

	diagVal.resize( 0 );
	// Fill in the diagonal terms. Here we do all entries.
	for ( unsigned int i = 0; i != nrows_ ; ++i ) {
		diagVal.push_back( 1.0 / N_[diag[i]] );
	}

	// Fill in the back-sub operations. Note we don't need to check zero.
	for ( unsigned int i = nrows_-1; i != 0 ; --i ) {
		for ( int j = rowsToSub[i].size() - 1; j != -1; --j ) {
			unsigned int k = rowsToSub[i][j];
			double val = get( k, i ); //k is the row to go, i is the diag.
			bops.push_back( Triplet< double >( val * diagVal[i], i, k ) );
		}
	}

	/*
	for ( unsigned int i = 0; i < bops.size(); ++i ) {
		cout << i << ":		" << bops[i].a_ << "	" <<
				bops[i].b_ << "	" <<  // diagonal index
				bops[i].c_ << "	" <<  // off-diagonal index
				1.0 / diagVal[bops[i].b_] << // diagonal value.
				endl;
	}
	*/
}

/**
 * Put in diff and transport terms and also fill in the diagonal
 * The terms are:
 * n-1:    dt*(D+M)*adx(-)
 * n:   1 -dt*[ D*adx(-) +D*adx(+) + M*adx(+) ]
 * n+1:    dt*D*adx(+)
 * Note that there will be only one parent term but possibly many
 * distal terms.
 */
void FastMatrixElim::setDiffusionAndTransport(
			const vector< unsigned int >& parentVoxel,
			double diffConst, double motorConst,
		   	double dt )
{
	FastMatrixElim m;
	m.nrows_ = m.ncolumns_ = nrows_;
	m.rowStart_.resize( nrows_ + 1 );
	m.rowStart_[0] = 0;
	for ( unsigned int i = 1; i <= nrows_; ++i ) {
		// Insert an entry for diagonal in each.
		m.rowStart_[i] = rowStart_[i] + i;
	}
	for ( unsigned int i = 0; i < nrows_; ++i ) {
		double proximalTerms = 0.0;
		double distalTerms = 0.0;
		double term = 0.0;
		bool pendingDiagonal = true;
		unsigned int diagonalIndex = EMPTY_VOXEL;
		for ( unsigned int j = rowStart_[i]; j < rowStart_[i+1]; ++j ) {
			// Treat transport as something either to or from soma.
			// The motor direction is based on this.
			// Assume no transport between sibling compartments.
			if ( parentVoxel[colIndex_[j]] == i ) {
				term = N_[j] * dt * ( diffConst + motorConst );
				proximalTerms += N_[j];
			} else {
				term = N_[j] * dt * diffConst;
				distalTerms += N_[j];
			}
			if ( colIndex_[j] < i ) {
				m.colIndex_.push_back( colIndex_[j] );
				m.N_.push_back( term );
			} else if ( colIndex_[j] == i ) {
				assert( 0 );
			} else {
				if ( pendingDiagonal ) {
					pendingDiagonal = false;
					diagonalIndex = m.N_.size();
					m.colIndex_.push_back( i ); // diagonal
					m.N_.push_back( 0 ); // placeholder
				}
				m.colIndex_.push_back( colIndex_[j] );
				m.N_.push_back( term );
			}
		}
		if ( pendingDiagonal ) {
			diagonalIndex = m.N_.size();
			m.colIndex_.push_back( i ); // diagonal
			m.N_.push_back( 0 ); // placeholder
		}
		assert( diagonalIndex != EMPTY_VOXEL );
		m.N_[diagonalIndex] = 1.0 - dt * (
				diffConst * ( proximalTerms + distalTerms ) +
				motorConst * distalTerms
			);
	}
	*this = m;
}

//////////////////////////////////////////////////////////////////////////
// All the rest was setup. This function does the work, in a tight
// inner loop called many times. Doesn't even depend on matrix contents,
// but the data layout is built by the matrix so it is set as a
// static function of it.
//////////////////////////////////////////////////////////////////////////
// Static function.
void FastMatrixElim::advance( vector< double >& y,
		const vector< Triplet< double > >& ops, // has both fops and bops.
		const vector< double >& diagVal )
{
	for ( vector< Triplet< double > >::const_iterator
				i = ops.begin(); i != ops.end(); ++i )
		y[i->c_] -= y[i->b_] * i->a_;

	assert( y.size() == diagVal.size() );
	vector< double >::iterator iy = y.begin();
	for ( vector< double >::const_iterator
				i = diagVal.begin(); i != diagVal.end(); ++i )
		*iy++ *= *i;
}

/**
 * static function. Reorders the ops and diagVal vectors so as to restore
 * the original indexing of the input vectors.
 */
void FastMatrixElim::opsReorder(
		const vector< unsigned int >& lookupOldRowsFromNew,
		vector< Triplet< double > >& ops,
		vector< double >& diagVal )
{
	vector< double > oldDiag = diagVal;

	for ( unsigned int i = 0; i < ops.size(); ++i ) {
		ops[i].b_ = lookupOldRowsFromNew[ ops[i].b_ ];
		ops[i].c_ = lookupOldRowsFromNew[ ops[i].c_ ];
	}

	for ( unsigned int i = 0; i < diagVal.size(); ++i ) {
		diagVal[ lookupOldRowsFromNew[i] ] = oldDiag[i];
	}
}


// Build up colIndices for each row.
void buildColIndex( unsigned int nrows,
			const vector< unsigned int >& parentVoxel,
			vector< vector< unsigned int > >& colIndex
	)
{
	colIndex.clear();
	colIndex.resize( nrows );
	for ( unsigned int i = 0; i < nrows; ++i ) {
		if ( parentVoxel[i] != EMPTY_VOXEL ) {
			colIndex[i].push_back( parentVoxel[i] ); // parent
			colIndex[ parentVoxel[i] ].push_back( i ); // children
		}
		colIndex[i].push_back( i ); // diagonal: self
	}
	for ( unsigned int i = 0; i < nrows; ++i ) {
		vector< unsigned int >& c = colIndex[i];
		sort( c.begin(), c.end() );
		// Should check that there are no duplicates.
		for ( unsigned int j = 1; j < c.size(); ++j ) {
			assert( c[j -1 ] != c[j] );
		}
	}
}

/**
 * Motor transport into branches is divided between the child branches
 * in proportion to their area. This function computes these proportions.
 */
void findAreaProportion( vector< double >& areaProportion,
		const vector< unsigned int >& parentVoxel,
			const vector< double >& area )
{
	unsigned int nrows = parentVoxel.size();
	vector< double > sumAreaOfChildren( nrows, 0.0 );
	for ( unsigned int i = 0; i < nrows; ++i ) {
		if ( parentVoxel[i] != EMPTY_VOXEL )
			sumAreaOfChildren[ parentVoxel[i] ] += area[i];
	}
	for ( unsigned int i = 0; i < nrows; ++i ) {
		if ( parentVoxel[i] != EMPTY_VOXEL )
			areaProportion[i] = area[i]/sumAreaOfChildren[ parentVoxel[i] ];
		else
			areaProportion[i] = 1.0;
	}
}

/// This function makes the diffusion matrix.
bool FastMatrixElim::buildForDiffusion(
			const vector< unsigned int >& parentVoxel,
			const vector< double >& volume,
			const vector< double >& area,
			const vector< double >& length,
			double diffConst, double motorConst, double dt )
{
	// Too slow to matter.
	if ( diffConst < 1e-18 && fabs( motorConst ) < 1e-12 )
		return false;
	assert( nrows_ == parentVoxel.size() );
	assert( nrows_ == volume.size() );
	assert( nrows_ == area.size() );
	assert( nrows_ == length.size() );
	vector< vector< unsigned int > > colIndex;

	buildColIndex( nrows_, parentVoxel, colIndex );
	vector< bool > isTwig( nrows_, true );
	for ( unsigned int i = 0; i < nrows_; ++i ) {
		if ( parentVoxel[i] != EMPTY_VOXEL )
			isTwig[ parentVoxel[i] ] = false;
	}

	vector< double > areaProportion( nrows_, 1.0 );
	findAreaProportion( areaProportion, parentVoxel, area );

	// Fill in the matrix entries for each colIndex
	for ( unsigned int i = 0; i < nrows_; ++i ) {
		vector< unsigned int >& c = colIndex[i];
		vector< double > e( c.size(), 0.0 );

		for ( unsigned int j = 0; j < c.size(); ++j ) {
			unsigned int k = c[j]; // This is the colIndex, in order.
			double vol = volume[k];
			double a = area[k];
			double len = length[k];
			if ( k == i ) { // Diagonal term
				e[j] = 0.0 ;
				// Fill in diffusion from all connected voxels.
				for ( unsigned int p = 0; p < c.size(); ++p ) {
					unsigned int q = c[p];
					if ( q != i ) { // Skip self
						e[j] -= (area[q] + a)/(length[q] + len )/vol;
					}
				}
				e[j] *= diffConst;
				// Fill in motor transport
				if ( i > 0 && motorConst < 0 ) { // Towards soma
					e[j] += motorConst / len;
				}
				if ( !isTwig[i] && motorConst > 0 ) { // Toward twigs
					e[j] -= motorConst / len;
				}
				e[j] *= -dt; // The previous lot is the rate, so scale by dt
				e[j] += 1.0; // term for self.
			} else { // Not diagonal.
				// Fill in diffusion from this entry to i.
				e[j] = diffConst *
						(area[i] + a)/(length[i] + len )/vol;

				// Fill in motor transport
				if ( k == parentVoxel[i] && motorConst > 0 ) { //toward twig
					e[j] += areaProportion[i] * motorConst / len;
				}
				if ( i == parentVoxel[k] && motorConst < 0 ) { //toward soma
					e[j] -= motorConst / length[k];
				}
				e[j] *= -dt; // Scale the whole thing by dt.
			}
		}
		// Now put it all in the sparase matrix.
		addRow( i, e, c );
	}
	return true;
}
