/*
 * =====================================================================================
 *
 *       Filename:  matrix_util.cpp
 *
 *    Description:  Some matrix utility function. Used when boost library is
 *    used.
 *
 *        Version:  1.0
 *        Created:  05/10/2016 05:25:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 *
 * =====================================================================================
 */

#ifdef  USE_BOOST

#include "matrix_util.h"

#define EPSILON 1e-9                            /* Treat number below it as 0 */

/*-----------------------------------------------------------------------------
 *  These functions computes rank of a matrix.
 *-----------------------------------------------------------------------------*/

/**
 * @brief Swap row r1 and r2.
 *
 * @param mat Matrix input
 * @param r1 index of row 1
 * @param r2 index of row 2
 */

void swapRows( ublas::matrix< value_type >& mat, unsigned int r1, unsigned int r2)
{
    ublas::vector<value_type> temp( mat.size2() );
    for (size_t i = 0; i < mat.size2(); i++)
    {
        temp[i] = mat(r1, i );
        mat(r1, i ) = mat(r2, i );
    }

    for (size_t i = 0; i < mat.size2(); i++)
        mat(r2, i) = temp[i];
}


int reorderRows( ublas::matrix< value_type >& U, int start, int leftCol )
{
    int leftMostRow = start;
    int numReacs = U.size2() - U.size1();
    int newLeftCol = numReacs;
    for ( size_t i = start; i < U.size1(); ++i )
    {
        for ( int j = leftCol; j < numReacs; ++j )
        {
            if ( fabs( U(i,j )) > EPSILON )
            {
                if ( j < newLeftCol )
                {
                    newLeftCol = j;
                    leftMostRow = i;
                }
                break;
            }
        }
    }

    if ( leftMostRow != start )   // swap them.
        swapRows( U, start, leftMostRow );

    return newLeftCol;
}

void eliminateRowsBelow( ublas::matrix< value_type >& U, int start, int leftCol )
{
    int numMols = U.size1();
    double pivot = U( start, leftCol );
    assert( fabs( pivot ) > EPSILON );
    for ( int i = start + 1; i < numMols; ++i )
    {
        double factor = U(i, leftCol);
        if( fabs ( factor ) > EPSILON )
        {
            factor = factor / pivot;
            for ( size_t j = leftCol + 1; j < U.size2(); ++j )
            {
                double x = U(i,j);
                double y = U( start, j );
                x -= y * factor;
                if ( fabs( x ) < EPSILON )
                    x = 0.0;
                U( i, j ) = x;
            }
        }
        U(i, leftCol) = 0.0;
    }
}

unsigned int rankUsingBoost( ublas::matrix<value_type>& U )
{
    int numMols = U.size1();
    int numReacs = U.size2() - numMols;
    int i;
    // Start out with a nonzero entry at 0,0
    int leftCol = reorderRows( U, 0, 0 );

    for ( i = 0; i < numMols - 1; ++i )
    {
        eliminateRowsBelow( U, i, leftCol );
        leftCol = reorderRows( U, i + 1, leftCol );
        if ( leftCol == numReacs )
            break;
    }
    return i + 1;
}

#endif     /* -----  not USE_BOOST  ----- */
