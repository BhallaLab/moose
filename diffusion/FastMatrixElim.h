/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

class FastMatrixElim: public SparseMatrix< double >
{
public:
    FastMatrixElim();
    FastMatrixElim( unsigned int nrows, unsigned int ncolumns );
    FastMatrixElim( const SparseMatrix< double >& orig );

    void makeTestMatrix( const double* test, unsigned int numCompts );

    /**
    * Recursively scans the current matrix, to build tree of parent
    * voxels using the contents of the sparase matrix to indicate
    * connectivity. Emits warning noises and returns an empty vector if
    * the entire tree cannot be traversed, or
    * if the current matrix is not tree-like.
    * Assumes row 0 is root row. User should always call with
    * parentRow == 0;
    * Doesn't work yet.
    bool buildTree( unsigned int parentRow,
    		vector< unsigned int >& parentVoxel ) const;
    */

    /**
     * Reduces the forward elimination phase into a series of operations
     * defined by the fops vector.
     */
    void buildForwardElim( vector< unsigned int >& diag,
                           vector< Triplet< double > >& fops );
    /**
     * Reduces the backward substitution phase into a series of
     * operations defined by the bops vector, and by the list of
     * values on the diagonal.
     */
    void buildBackwardSub( vector< unsigned int >& diag,
                           vector< Triplet< double > >& bops, vector< double >& diagVal );
    /////////////////////////////////////////////////////////////
    // Here we do stuff to set up the Hines ordering of the matrix.
    /////////////////////////////////////////////////////////////
    /**
     * Takes the tree specification in the form of a list of parent
     * entries for each tree node, and reorders the matrix into the
     * twig-first sequence required for fast elimination.
     * Returns true if it succeeded in doing this; many matrices will
     * not reorder correctly.
     */
    bool hinesReorder( const vector< unsigned int >& parentVoxel,
                       vector< unsigned int >& lookupOldRowsFromNew	);

    /**
     * static function. Reorders the ops and diagVal vectors so as to
     * restore the original indexing of the input vectors.
     */
    static void opsReorder(
        const vector< unsigned int >& lookupOldRowsFromNew,
        vector< Triplet< double > >& ops,
        vector< double >& diagVal );

    /**
     * Reorders rows of the matrix according to the vector
     * lookupOldRowFromNew. The vector tells the function which old
     * row to put in the ith row of the new matrix. Since the
     * matrix has matching column entries, those get shuffled too.
     */
    void shuffleRows(
        const vector< unsigned int >& lookupOldRowFromNew );

    /**
     * Checks that the matrix shape (but not necessarily values)
     * are symmetric, returns true if symmetric.
     */
    bool checkSymmetricShape() const;

    bool operator==( const FastMatrixElim& other ) const;
    bool isSymmetric() const;

    /**
     * This function incorporates molecule-specific diffusion and
     * motor transport terms into the matrix.
     */
    void setDiffusionAndTransport(
        const vector< unsigned int >& parentVoxel,
        double diffConst, double motorConst, double dt );

    /**
     * This function makes the matrix for computing diffusion and
     * transport equations.
     */
    bool buildForDiffusion(
        const vector< unsigned int >& parentVoxel,
        const vector< double >& volume,
        const vector< double >& area,
        const vector< double >& length,
        double diffConst, double motorConst, double dt );

    /**
     * Does the actual computation of the matrix inversion, which is
     * equivalent to advancing one timestem in Backward Euler.
     * Static function here to keep namespaces clean.
     */
    static void advance( vector< double >& y,
                         const vector< Triplet< double > >& ops, //has both fops and bops
                         const vector< double >& diagVal );
};

void sortByColumn(
    vector< unsigned int >& col, vector< double >& entry );

// Todo: Maintain an internal vector of the mapping between rows so that
// the output vector can be updated in the right order, and input values
// can be mapped if matrix reassignment happens.
// The input to the parent class should just be a matrix with diameter
// and connectivity info, and then the system spawns out the ops
// vector depending on diffusion constant.
