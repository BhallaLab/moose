/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2016 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ROLLING_MATRIX_H
#define _ROLLING_MATRIX_H

// Temporary, just to get going.
typedef vector< double > SparseVector;

class RollingMatrix {
	public:
		// Specify empty matrix.
		RollingMatrix();
		~RollingMatrix();
		RollingMatrix& operator=( const RollingMatrix& other );

		// Specify size of matrix. Allocations may happen later.
		void resize( unsigned int numRows, unsigned int numColumns );

		// Return specified entry.
		double get( unsigned int row, unsigned int column ) const;

		// Sum contents of input into entry at specfied row, column.
		// Row index is relative to current zero.
		void sumIntoEntry( double input, unsigned int row, unsigned int column );

		// Sum contents of input into vector at specfied row.
		// Row index is relative to current zero.
		void sumIntoRow( const vector< double >& input, unsigned int row );

		// Return dot product of input with internal vector at specified
		// row, starting at specified column.
		double dotProduct( const vector< double >& input, unsigned int row,
					   	unsigned int startColumn ) const;

		// Return correlation found by summing dotProduct across all columns
		void correl( vector< double >& ret, const vector< double >& input,
						unsigned int row ) const;

		// Zero out contents of row.
		void zeroOutRow( unsigned int row );

		// Roll the matrix by one row. What was row 0 becomes row 1, etc.
		// Last row vanishes.
		void rollToNextRow(); //

	private:
		unsigned int nrows_;
		unsigned int ncolumns_;
		unsigned int currentStartRow_;

		vector< SparseVector > rows_;
};

#endif // _ROLLING_MATRIX
