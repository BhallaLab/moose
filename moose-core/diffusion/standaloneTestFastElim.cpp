// To compile:
// g++  standaloneTestFastElim.cpp -lgsl -lgslcblas


#include <vector>
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iomanip>
// #include <gsl/gsl_linalg.h>
#include "/usr/include/gsl/gsl_linalg.h"
using namespace std;
#include "../basecode/SparseMatrix.h"

const unsigned int SM_MAX_ROWS = 200000;
const unsigned int SM_MAX_COLUMNS = 200000;
const unsigned int SM_RESERVE = 8;

void sortByColumn(
    vector< unsigned int >& col, vector< double >& entry );
void testSorting();

class Unroll
{
public:
    Unroll( double diag, double off, unsigned int i, unsigned int j )
        :
        diagVal( diag ),
        offDiagVal( off ),
        row( i ),
        col( j )
    {;}
    double diagVal;
    double offDiagVal;
    unsigned int row; // On which the diagonal is located
    unsigned int col; // Col on which the offDiagVal is located.
};

class FastElim: public SparseMatrix< double >
{
public:
    void makeTestMatrix( const double* test, unsigned int numCompts );
    /*
    void rowElim( unsigned int row1, unsigned int row2,
    				vector< double >& rhs );
    				*/
    void buildForwardElim( vector< unsigned int >& diag,
                           vector< Triplet< double > >& fops );
    void buildBackwardSub( vector< unsigned int >& diag,
                           vector< Triplet< double > >& bops, vector< double >& diagVal );
    /////////////////////////////////////////////////////////////
    // Here we do stuff to set up the Hines ordering of the matrix.
    /////////////////////////////////////////////////////////////
    bool hinesReorder( const vector< unsigned int >& parentVoxel );
    const double* allEntries() const;
    void shuffleRows(
        const vector< unsigned int >& lookupOldRowFromNew );
    /*
    bool hinesReorder();
    void extractTwig( unsigned int i,
    				vector< unsigned int >& rowReorder,
    				vector< bool >& extracted );
    void findClosedEnds(
    				vector< unsigned int >& rowReorder,
    				vector< bool >& extracted );
    void extractClosedEnds( unsigned int i,
    				vector< unsigned int >& rowReorder,
    				vector< bool >& extracted );
    				*/
};

const double* FastElim::allEntries() const
{
    return &N_[0];
}

//
//	static unsigned int parents[] = { 1,6,3,6,5,8,7,8,9,10,-1};
//	unsigned int numKids[] = {0,1,0,1,0,2,

/**
 * Reorders rows and columns to put the matrix in the form suitable for
 * rapid single-pass inversion. Returns 0 on failure.
 */
bool FastElim::hinesReorder( const vector< unsigned int >& parentVoxel )
{
    // First we fill in the vector that specifies the old row number
    // assigned to each row of the reordered matrix.
    assert( parentVoxel.size() == nrows_ );
    vector< unsigned int > numKids( nrows_, 0 );
    vector< unsigned int > lookupOldRowFromNew;
    vector< bool > rowPending( nrows_, true );
    unsigned int numDone = 0;
    for ( unsigned int i = 0; i < nrows_; ++i )
    {
        if ( parentVoxel[i] != -1 )
            numKids[ parentVoxel[i] ]++;
    }
    while ( numDone < nrows_ )
    {
        for ( unsigned int i = 0; i < nrows_; ++i )
        {
            if ( rowPending[i] && numKids[i] == 0 )
            {
                lookupOldRowFromNew.push_back( i );
                rowPending[i] = false;
                numDone++;
                unsigned int pa = parentVoxel[i];
                // Unsure what the root parent is. Assume it is -1
                while ( pa != -1 && numKids[pa] == 1 )
                {
                    assert( rowPending[pa] );
                    rowPending[pa] = false;
                    numDone++;
                    lookupOldRowFromNew.push_back( pa );
                    pa = parentVoxel[pa];
                }
                if ( pa != -1 )
                {
                    assert( numKids[pa] > 0 );
                    numKids[pa]--;
                }
            }
        }
    }

    cout << setprecision(4);
    cout << "oldRowFromNew= {" ;
    for ( int i = 0; i < nrows_; ++i )
        cout << lookupOldRowFromNew[i] << ", ";
    cout << "}\n";
    // Then we fill in the reordered matrix. Note we need to reorder
    // columns too.
    shuffleRows( lookupOldRowFromNew );
    return true;
}

// Fill in the reordered matrix. Note we need to reorder columns too.
void FastElim::shuffleRows(
    const vector< unsigned int >& lookupOldRowFromNew )
{
    vector< unsigned int > lookupNewRowFromOld( nrows_ );
    for ( unsigned int i = 0; i < nrows_; ++i )
        lookupNewRowFromOld[ lookupOldRowFromNew[i] ] = i;

    FastElim temp = *this;
    clear();
    setSize( temp.nrows_, temp.nrows_ );
    for ( unsigned int i = 0; i < lookupOldRowFromNew.size(); ++i )
    {
        vector< unsigned int > c;
        vector< double > e;
        unsigned int num = temp.getRow( lookupOldRowFromNew[i], e, c );
        vector< unsigned int > newc( num );
        vector< double > newe( num );
        for ( unsigned int j = 0; j < num; ++j )
        {
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
    for ( unsigned int i = 0; i < num; ++i )
    {
        for ( unsigned int j = 1; j < num; ++j )
        {
            if ( col[j] < col[j-1] )
            {
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

/*
bool FastElim::hinesReorder()
{
	vector< unsigned int > rowReorder;
	vector< bool > extracted( nrows_, false );
	for ( unsigned int i = 0; i < nrows_; ++i ) {
		if ( rowStart_[i+1] - rowStart_[i] == 2 )
			extractTwig( i, rowReorder, extracted );
	}
	// List of rows that now have all sub-branches extracted
	vector< unsigned int > closedEnds;
	do {
		closedEnds.resize( 0 );
		findClosedEnds( closedEnds, extracted );
		for ( vector< unsigned int >::iterator
					i = closedEnds.begin(); i != closedEnds.end(); ++i ) {
			extractClosedEnds( *i, rowReorder, extracted );
		}
	} while ( closedEnds.size() > 0 );
}
*/

/**
 * Finds the 'twigs' of the matrix: Only one end connected.
void FastElim::extractTwig( unsigned int i,
						vector< unsigned int >& rowReorder,
						vector< bool >& extracted )
{
		;
}

void FastElim::findClosedEnds(
						vector< unsigned int >& rowReorder,
						vector< bool >& extracted )
{
		;
}

void FastElim::extractClosedEnds( unsigned int i,
						vector< unsigned int >& rowReorder,
						vector< bool >& extracted )
{
		;
}
 */

/*
void FastElim::rowElim( unsigned int row1, unsigned int row2,
						vector< double >& rhs )
{
	unsigned int rs1 = rowStart_[row1];
	unsigned int rs2 = rowStart_[row2];
	unsigned int n1 = rowStart_[row1+1] - rs1;
	if ( n1 < 2 ) return;
	assert( colIndex_[rs2] == row1 );
	double diag1 = N_[diagIndex_[row1]];
	double temp = N_[rs2];
	double r = temp/diag1;

	const double* p1 = &(N_[ diagIndex_[row1] + 1 ] );
	double* p2 = N_ +
	for ( unsigned int i =
	N_[rs2+1]

	double* v1 = N_ + rs1;
	double* v2 = N_ + rs2;


			v2' - v1'*v2/v1

}
*/

void FastElim::makeTestMatrix( const double* test, unsigned int numCompts )
{
    setSize( numCompts, numCompts );
    vector< double > row( numCompts, ~0 );
    unsigned int i = 1;
    for ( unsigned int i = 0; i < numCompts; ++i )
    {
        for ( unsigned int j = 0; j < numCompts; ++j )
        {
            unsigned int k = i * numCompts + j;
            if ( test[k] < 0.1 )
            {
            }
            else
            {
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
void FastElim::buildForwardElim( vector< unsigned int >& diag,
                                 vector< Triplet< double > >& fops )
{
    vector< vector< unsigned int > > rowsToElim( nrows_ );
    diag.clear();
    for ( unsigned int i = 0; i < nrows_; ++i )
    {
        unsigned int rs = rowStart_[i];
        unsigned int re = rowStart_[i+1];
        for ( unsigned int j = rs; j < re; ++j )
        {
            unsigned int k = colIndex_[j];
            if ( k == i )
            {
                diag.push_back(j);
            }
            else if ( k > i )
            {
                rowsToElim[ i ].push_back( k );
            }
        }
    }
    for ( unsigned int i = 0; i < nrows_; ++i )
    {
        double d = N_[diag[i]];
        unsigned int diagend = rowStart_[ i + 1 ];
        assert( diag[i] < diagend );
        vector< unsigned int >& elim = rowsToElim[i];
        for ( unsigned int j = 0; j < elim.size(); ++j )
        {
            unsigned int erow = elim[j];
            if ( erow == i ) continue;
            unsigned int rs = rowStart_[ erow ];
            unsigned int re = rowStart_[ erow+1 ];
            // assert( colIndex_[rs] == i );
            double ratio = get( erow, i ) / d;
            // double ratio = N_[rs]/N_[diag[i]];
            for ( unsigned int k = diag[i]+1; k < diagend; ++k )
            {
                unsigned int col = colIndex_[k];
                // findElimEntry, subtract it out.
                for ( unsigned int q = rs; q < re; ++q )
                {
                    if ( colIndex_[q] == col )
                    {
                        N_[q] -= N_[k] * ratio;
                    }
                }
            }
            fops.push_back( Triplet< double >( ratio, i, erow) );
        }
    }
    for ( unsigned int i = 0; i < rowsToElim.size(); ++i )
    {
        cout << i << " :		";
        for ( unsigned int j = 0; j < rowsToElim[i].size(); ++j )
        {
            cout << rowsToElim[i][j] << "	";
        }
        cout << endl;
    }
    for ( unsigned int i = 0; i < fops.size(); ++i )
    {
        cout << "fops[" << i << "]=		" << fops[i].b_ << "	" << fops[i].c_ <<
             "	" << fops[i].a_ << endl;
    }
    /*
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
void FastElim::buildBackwardSub( vector< unsigned int >& diag,
                                 vector< Triplet< double > >& bops, vector< double >& diagVal )
{
    // This vec tells the routine which rows below have to be back-subbed.
    // This includes the rows if any in the tridiagonal band and also
    // rows, if any, on branches.
    vector< vector< unsigned int > > rowsToSub( nrows_ );

    for ( unsigned int i = 0; i < nrows_; ++i )
    {
        unsigned int d = diag[i] + 1;
        unsigned int re = rowStart_[i+1];
        for ( unsigned int j = d; j < re; ++j )
        {
            unsigned int k = colIndex_[j];
            // At this point the row to sub is at (i, k). We need to go down
            // to the (k,k) diagonal to sub it out.
            rowsToSub[ k ].push_back( i );
        }
    }
    for ( unsigned int i = 0; i < rowsToSub.size(); ++i )
    {
        cout << i << " :		";
        for ( unsigned int j = 0; j < rowsToSub[i].size(); ++j )
        {
            cout << rowsToSub[i][j] << "	";
        }
        cout << endl;
    }

    diagVal.resize( 0 );
    // Fill in the diagonal terms. Here we do all entries.
    for ( unsigned int i = 0; i != nrows_ ; ++i )
    {
        diagVal.push_back( 1.0 / N_[diag[i]] );
    }

    // Fill in the back-sub operations. Note we don't need to check zero.
    for ( unsigned int i = nrows_-1; i != 0 ; --i )
    {
        for ( unsigned int j = rowsToSub[i].size() - 1; j != -1; --j )
        {
            unsigned int k = rowsToSub[i][j];
            double val = get( k, i ); //k is the row to go, i is the diag.
            bops.push_back( Triplet< double >( val * diagVal[i], i, k ) );
        }
    }

    for ( unsigned int i = 0; i < bops.size(); ++i )
    {
        cout << i << ":		" << bops[i].a_ << "	" <<
             bops[i].b_ << "	" <<  // diagonal index
             bops[i].c_ << "	" <<  // off-diagonal index
             1.0 / diagVal[bops[i].b_] << // diagonal value.
             endl;
    }
}

void advance( vector< double >& y,
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

double checkAns(
    const double* m, unsigned int numCompts,
    const double* ans, const double* rhs )
{
    vector< double > check( numCompts, 0.0 );
    for ( unsigned int i = 0; i < numCompts; ++i )
    {
        for ( unsigned int j = 0; j < numCompts; ++j )
            check[i] += m[i*numCompts + j] * ans[j];
    }
    double ret = 0.0;
    for ( unsigned int i = 0; i < numCompts; ++i )
        ret += (check[i] - rhs[i]) * (check[i] - rhs[i] );
    return ret;
}



int main()
{


    /*
    2    11
     1  4
       3    10
        9  5
         6
         7
         8

            1 2 3 4 5 6 7 8 9 10 11
    1       x x x x
    2       x x
    3       x   x x         x
    4       x   x x              x
    5               x x     x x
    6               x x x   x
    7                 x x x
    8                   x x
    9           x   x x     x
    10              x         x
    11            x              x
    	static double test[] = {
    		1,  2,  3,  4,  0,  0,  0,  0,  0,  0,  0,
    		5,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    		7,  0,  8,  9,  0,  0,  0,  0, 10,  0,  0,
    		11, 0, 12, 13,  0,  0,  0,  0,  0,  0, 14,
    		0,  0,  0,  0, 15, 16,  0,  0, 17, 18,  0,
    		0,  0,  0,  0, 19, 20, 21,  0, 22,  0,  0,
    		0,  0,  0,  0,  0, 23, 24, 25,  0,  0,  0,
    		0,  0,  0,  0,  0,  0, 26, 27,  0,  0,  0,
    		0,  0, 28,  0, 29, 30,  0,  0, 31,  0,  0,
    		0,  0,  0,  0, 32,  0,  0,  0,  0, 33,  0,
    		0,  0,  0, 34,  0,  0,  0,  0,  0,  0, 35,
    	};
    	const unsigned int numCompts = 11;
    //	static unsigned int parents[] = { 3,1,9,3,6,7,8,-1,6,5,4 };
    	static unsigned int parents[] = { 2,0,8,2,5,6,7,-1,5,4,3 };
    */

    /*
    1   3
     2 4
       7   5
        8 6
         9
         10
         11

            1 2 3 4 5 6 7 8 9 10 11
    1       x x
    2       x x         x
    3           x x
    4           x x     x
    5               x x
    6               x x     x
    7         x   x     x x
    8                   x x x
    9                 x   x x x
    10                      x x  x
    11                        x  x
    	static double test[] = {
    		1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    		3,  4,  0,  0,  0,  0,  5,  0,  0,  0,  0,
    		0,  0,  6,  7,  0,  0,  0,  0,  0,  0,  0,
    		0,  0,  8,  9,  0,  0, 10,  0,  0,  0,  0,
    		0,  0,  0,  0, 11, 12,  0,  0,  0,  0,  0,
    		0,  0,  0,  0, 13, 14,  0,  0, 15,  0,  0,
    		0, 16,  0, 17,  0,  0, 18, 19,  0,  0,  0,
    		0,  0,  0,  0,  0,  0, 20, 21, 22,  0,  0,
    		0,  0,  0,  0,  0, 23,  0, 24, 25, 26,  0,
    		0,  0,  0,  0,  0,  0,  0,  0, 27, 28, 29,
    		0,  0,  0,  0,  0,  0,  0,  0,  0, 30, 31,
    	};
    	const unsigned int numCompts = 11;
    	static unsigned int parents[] = { 1,6,3,6,5,8,7,8,9,10,-1};
    */

    /*
    Linear cable, 12 segments.
    */

    static double test[] =
    {
        1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        3,  4,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  6,  7,  8,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  9, 10, 11,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0, 12, 13, 14,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0, 15, 16, 17,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, 18, 19, 20,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0, 21, 22, 23,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0, 24, 25, 26,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0, 27, 28, 29,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0, 30, 31, 32,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 33, 34,
    };
    const unsigned int numCompts = 12;
    static int parents[] = { 1,2,3,4,5,6,7,8,9,10,11,-1};

    /*
    static double test[] = {
    1,  2,
    3,  4
    };
    const unsigned int numCompts = 2;
    static double test[] = {
    1, 2, 0, 0,
    3, 4, 5, 0,
    0, 6, 7, 8,
    0, 0, 9, 10
    };
    const unsigned int numCompts = 4;
    static double test[] = {
    1,  2,  0,  0,  0,  0,
    3,  4,  5,  0,  0,  0,
    0,  6,  7,  8,  0,  0,
    0,  0,  9, 10, 11,  0,
    0,  0,  0, 12, 13, 14,
    0,  0,  0,  0, 15, 16,
    };
    const unsigned int numCompts = 6;
    static double test[] = {
    1,  2,  0,  0,  0,  0,
    3,  4,  0,  0,  1,  0,
    0,  0,  7,  8,  0,  0,
    0,  0,  9, 10, 11,  0,
    0,  1,  0, 12, 13, 14,
    0,  0,  0,  0, 15, 16,
    };
    const unsigned int numCompts = 6;
    */
    // testSorting(); // seems to work fine.
    FastElim fe;
    vector< Triplet< double > > fops;
    vector< Unroll > bops;
    fe.makeTestMatrix( test, numCompts );
    fe.print();
    cout << endl << endl;
    vector< unsigned int > parentVoxel;
    parentVoxel.insert( parentVoxel.begin(), &parents[0], &parents[numCompts] );
    fe.hinesReorder( parentVoxel );
    /*
    */
    /*
    vector< unsigned int > shuf;
    for ( unsigned int i = 0; i < numCompts; ++i )
    	shuf.push_back( i );
    shuf[0] = 1;
    shuf[1] = 0;
    fe.shuffleRows( shuf );
    */
    fe.print();
    cout << endl << endl;
    FastElim foo = fe;

    vector< unsigned int > diag;
    vector< double > diagVal;
    fe.buildForwardElim( diag, fops );
    fe.print();
    fe.buildBackwardSub( diag, fops, diagVal );
    vector< double > y( numCompts, 1.0 );
    vector< double > ones( numCompts, 1.0 );
    advance( y, fops, diagVal );
    for ( int i = 0; i < numCompts; ++i )
        cout << "y" << i << "]=	" << y[i] << endl;

    // Here we verify the answer

    vector< double > alle;
    for( unsigned int i = 0; i < numCompts; ++i )
    {
        for( unsigned int j = 0; j < numCompts; ++j )
        {
            alle.push_back( foo.get( i, j ) );
        }
    }
    cout << "myCode: " <<
         checkAns( &alle[0], numCompts, &y[0], &ones[0] ) << endl;



    /////////////////////////////////////////////////////////////////////
    // Here we do the gsl test.
    vector< double > temp( &test[0], &test[numCompts*numCompts] );
    gsl_matrix_view m = gsl_matrix_view_array( &temp[0], numCompts, numCompts );

    vector< double > z( numCompts, 1.0 );
    gsl_vector_view b = gsl_vector_view_array( &z[0], numCompts );
    gsl_vector* x = gsl_vector_alloc( numCompts );
    int s;
    gsl_permutation* p = gsl_permutation_alloc( numCompts );
    gsl_linalg_LU_decomp( &m.matrix, p, &s );
    gsl_linalg_LU_solve( &m.matrix, p, &b.vector, x);
    vector< double > gslAns( numCompts );
    for ( int i = 0; i < numCompts; ++i )
    {
        gslAns[i] = gsl_vector_get( x, i );
        cout << "x[" << i << "]=	" << gslAns[i] << endl;
    }
    /*
    */
    cout << "GSL: " << checkAns( test, numCompts, &gslAns[0], &ones[0] ) << endl;
    gsl_vector_free( x );
    return 0;
}

void testSorting()
{
    static unsigned int k[] = {20,40,60,80,100,10,30,50,70,90};
    static double d[] = {1,2,3,4,5,6,7,8,9,10};
    vector< unsigned int > col;
    col.insert( col.begin(), k, k+10);
    vector< double > entry;
    entry.insert( entry.begin(), d, d+10);
    sortByColumn( col, entry );
    cout << "testing sorting\n";
    for ( int i = 0; i < col.size(); ++i )
    {
        cout << "d[" << i << "]=	" << k[i] <<
             ", col[" << i << "]= " <<	col[i] << ", e=" << entry[i] << endl;
    }
    cout << endl;
}
