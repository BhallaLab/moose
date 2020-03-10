/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _KIN_SPARSE_MATRIX_H
#define _KIN_SPARSE_MATRIX_H

class KinSparseMatrix: public SparseMatrix< int >
{
public:

    /**
     * Returns all non-zero column indices, for the specified row.
      * This gives reac #s in orig matrix, and molecule #s in the
     * transposed matrix
     * Not needed. The getRow function does all this, more efficiently.
    int getRowIndices(
    	unsigned int row, vector< unsigned int >& indices );
     */

    /**
     * Returns the dot product of the specified row with the
     * vector v. v corresponds to the vector of reaction rates.
     * v must have nColumns entries.
     */
    double computeRowRate(
        unsigned int row, const vector< double >& v
    ) const;


    /**
     * Does a special self-product of the specified row. Output
     * is the set of nonzero indices in the product
     * abs( Rij ) * neg( Rjk ) for the specified index i, where
     * neg( val ) is true only if val < 0.
     */
    void getGillespieDependence(
        unsigned int row, vector< unsigned int >& cols
    ) const;

    /**
     * Fires a stochastic reaction: It undergoes a single transition
     * This operation updates the mol concs due to the reacn.
     * Direction is +1 or -1, specifies direction of reaction
     */
    void fireReac( unsigned int reacIndex, vector< double >& S,
                   double direction ) const;

    /**
    * This function generates a new internal list of rowEnds, such
    * that they are all less than the maxColumnIndex.
    * It is used because in fireReac we don't want to update all the
    * molecules, only those that are variable.
    */
    void truncateRow( unsigned int maxColumnIndex );

private:
    /**
     * End colIndex for rows (molecules in the transposed matrix)
     * so that only variable molecules are below the colIndex.
     */
    vector< unsigned int > rowTruncated_;
};

#endif	// _KIN_SPARSE_MATRIX_H
