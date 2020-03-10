/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <functional>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>
#include "../basecode/SparseMatrix.h"
#include "../utility/numutil.h"
#include "KinSparseMatrix.h"

using namespace std;

/**
 * Returns the dot product of the specified row with the
 * vector v. v corresponds to the vector of reaction rates.
 * v must have nColumns entries.
 */
double KinSparseMatrix::computeRowRate(
    unsigned int row, const vector< double >& v
) const
{
    assert( nColumns() == 0 || row < nRows() );
    assert( v.size() == nColumns() );
    const int* entry = 0;
    const unsigned int* colIndex = 0;
    unsigned int numEntries = getRow( row, &entry, &colIndex );
    const int* end = entry + numEntries;

    double ret = 0.0;
    for ( const int* i = entry; i != end; ++i )
    {
        ret += *i * v[ *colIndex++ ];
        assert(! std::isnan(ret));
    }
    //assert ( !std::isnan(ret) );
    return ret;
}


/**
 * Has to operate on transposed matrix
 * row argument refers to reac# in this transformed situation.
 * Fills up 'deps' with reac#s that depend on the row argument.
 * Does NOT ensure that list is unique.
 */
void KinSparseMatrix::getGillespieDependence(
    unsigned int row, vector< unsigned int >& deps
) const
{
    deps.resize( 0 );
    // vector< unsigned int > deps;
    for ( unsigned int i = 0; i < nrows_; ++i )
    {
        // i is index for reac # here. Note that matrix is transposed.
        unsigned int j = rowStart_[ row ];
        unsigned int jend = rowStart_[ row + 1 ];
        unsigned int k = rowStart_[ i ];
        unsigned int kend = rowStart_[ i + 1 ];

        while ( j < jend && k < kend )
        {
            if ( colIndex_[ j ] == colIndex_[ k ] )
            {
                /* Pre 28 Nov 2015. Why below zero? Shouldn't it be any?
                if ( N_[ k ] < 0 ) {
                	deps.push_back( i );
                }
                */
                assert( round( N_[k] ) != 0 );
                deps.push_back( i );
                ++j;
                ++k;
            }
            else if ( colIndex_[ j ] < colIndex_[ k ] )
            {
                ++j;
            }
            else if ( colIndex_[ j ] > colIndex_[ k ] )
            {
                ++k;
            }
            else
            {
                assert( 0 );
            }
        }
    }
}

/**
 * This too operates on the transposed matrix, because we need to get all
 * the molecules for a given reac: a column in the original N matrix.
 * Direction [-1,+1] specifies whether the reaction is forward or backward.
 */
void KinSparseMatrix::fireReac( unsigned int reacIndex, vector< double >& S, double direction )
const
{
    assert( ncolumns_ == S.size() && reacIndex < nrows_ );
    unsigned int rowBeginIndex = rowStart_[ reacIndex ];
    // vector< int >::const_iterator rowEnd = N_.begin() + rowStart_[ reacIndex + 1];
    vector< int >::const_iterator rowBegin =
        N_.begin() + rowBeginIndex;
    vector< int >::const_iterator rowEnd =
        N_.begin() + rowTruncated_[ reacIndex ];
    vector< unsigned int >::const_iterator molIndex =
        colIndex_.begin() + rowBeginIndex;

    for ( vector< int >::const_iterator i = rowBegin; i != rowEnd; ++i )
    {
        double& x = S[ *molIndex++ ];
        x += *i * direction;
        x *= (x > 0 );
        // assert( S[ *molIndex ] + *i * direction >= 0.0 );
        // S[ *molIndex++ ] += *i * direction;
    }
}

/**
 * This function generates a new internal list of rowEnds, such that
 * they are all less than the maxColumnIndex.
 * It is used because in fireReac we don't want to update all the
 * molecules, only those that are variable.
 */
void KinSparseMatrix::truncateRow( unsigned int maxColumnIndex )
{
    rowTruncated_.resize( nrows_, 0 );
    if ( colIndex_.size() == 0 )
        return;
    for ( unsigned int i = 0; i < nrows_; ++i )
    {
        unsigned int endCol = rowStart_[ i ];
        for ( unsigned int j = rowStart_[ i ];
                j < rowStart_[ i + 1 ]; ++j )
        {
            if ( colIndex_[ j ] < maxColumnIndex )
            {
                endCol = j + 1;
            }
            else
            {
                break;
            }
        }
        rowTruncated_[ i ] = endCol;
    }
}


void makeVecUnique( vector< unsigned int >& v )
{
    vector< unsigned int >::iterator pos = unique( v.begin(), v.end() );
    v.resize( pos - v.begin() );
}


#ifdef DO_UNIT_TESTS
#include "../basecode/header.h"

void testKinSparseMatrix()
{
    const double EPSILON = 1e-7;
    // This is the stoichiometry matrix for the unidirectional reacns
    // coming out of the following system:
    // a + b <===> c
    // c + d <===> e
    // a + f <===> g
    // a + e <===> 2g
    //
    // When halfreac 0 fires, it affects 0, 1, 2, 4, 6.
    // and so on.
    static int transposon[][ 8 ] =
    {
        { -1,  1,  0,  0, -1,  1, -1,  1 },
        { -1,  1,  0,  0,  0,  0,  0,  0 },
        {  1, -1, -1,  1,  0,  0,  0,  0 },
        {  0,  0, -1,  1,  0,  0,  0,  0 },
        {  0,  0,  1, -1,  0,  0, -1,  1 },
        {  0,  0,  0,  0, -1,  1,  0,  0 },
        {  0,  0,  0,  0,  1, -1,  2, -2 }
    };

    cout << "\nTesting KinSparseMatrix" << flush;
    const unsigned int NR = 4;
    const unsigned int NC = 5;

    const unsigned int NTR = 7; // for transposon
    const unsigned int NTC = 8;

    KinSparseMatrix sm;
    sm.setSize( NR, NC );

    for ( unsigned int i = 0; i < NR; i++ )
    {
        for ( unsigned int j = 0; j < NC; j++ )
        {
            sm.set( i, j, 10 * i + j );
            // cout << i << ", " << j << ", " << sm.nColumns() << endl;
            int ret = sm.get( i, j );
            assert( ret == static_cast< int >( 10 * i + j ) );
        }
    }
    // cout << sm;

    vector< double > v( 5, 1.0 );
    double dret = sm.computeRowRate( 0, v );
    assert( fabs( dret - 10.0 ) < EPSILON );
    dret = sm.computeRowRate( 1, v );
    assert( fabs( dret - 60.0 ) < EPSILON );
    dret = sm.computeRowRate( 2, v );
    assert( fabs( dret - 110.0 ) < EPSILON );
    dret = sm.computeRowRate( 3, v );
    assert( fabs( dret - 160.0 ) < EPSILON );

    ////////////////////////////////////////////////////////////////
    // Checking transposition operation
    ////////////////////////////////////////////////////////////////
    KinSparseMatrix orig;
    orig.setSize( NTR, NTC );
    for ( unsigned int i = 0; i < NTR; i++ )
        for ( unsigned int j = 0; j < NTC; j++ )
            orig.set( i, j, transposon[ i ][ j ] );

    for ( unsigned int i = 0; i < NTR; i++ )
        for ( unsigned int j = 0; j < NTC; j++ )
            assert( orig.get( i, j ) ==  transposon[ i ][ j ] );

    orig.transpose();
    for ( unsigned int i = 0; i < NTR; i++ )
        for ( unsigned int j = 0; j < NTC; j++ )
            assert( orig.get( j, i ) ==  transposon[ i ][ j ] );


    ////////////////////////////////////////////////////////////////
    // Checking generation of dependency graphs.
    ////////////////////////////////////////////////////////////////

    KinSparseMatrix trans;
    vector< unsigned int > deps;
    trans.getGillespieDependence( 0, deps );

    /*
    makeVecUnique( deps );
    ASSERT( deps.size() == 5, "Gillespie dependence" );
    ASSERT( deps[0] == 0, "Gillespie dependence" );
    ASSERT( deps[1] == 1, "Gillespie dependence" );
    ASSERT( deps[2] == 2, "Gillespie dependence" );
    ASSERT( deps[3] == 4, "Gillespie dependence" );
    ASSERT( deps[4] == 6, "Gillespie dependence" );

    trans.getGillespieDependence( 1, deps );
    makeVecUnique( deps );
    ASSERT( deps.size() == 5, "Gillespie dependence" );
    ASSERT( deps[0] == 0, "Gillespie dependence" );
    ASSERT( deps[1] == 1, "Gillespie dependence" );
    ASSERT( deps[2] == 2, "Gillespie dependence" );
    ASSERT( deps[3] == 4, "Gillespie dependence" );
    ASSERT( deps[4] == 6, "Gillespie dependence" );

    trans.getGillespieDependence( 2, deps );
    makeVecUnique( deps );
    ASSERT( deps.size() == 4, "Gillespie dependence" );
    ASSERT( deps[0] == 1, "Gillespie dependence" );
    ASSERT( deps[1] == 2, "Gillespie dependence" );
    ASSERT( deps[2] == 3, "Gillespie dependence" );
    ASSERT( deps[3] == 6, "Gillespie dependence" );

    trans.getGillespieDependence( 4, deps );
    makeVecUnique( deps );
    ASSERT( deps.size() == 5, "Gillespie dependence" );
    ASSERT( deps[0] == 0, "Gillespie dependence" );
    ASSERT( deps[1] == 4, "Gillespie dependence" );
    ASSERT( deps[2] == 5, "Gillespie dependence" );
    ASSERT( deps[3] == 6, "Gillespie dependence" );
    ASSERT( deps[4] == 7, "Gillespie dependence" );

    trans.getGillespieDependence( 6, deps );
    makeVecUnique( deps );
    ASSERT( deps.size() == 6, "Gillespie dependence" );
    ASSERT( deps[0] == 0, "Gillespie dependence" );
    ASSERT( deps[1] == 3, "Gillespie dependence" );
    ASSERT( deps[2] == 4, "Gillespie dependence" );
    ASSERT( deps[3] == 5, "Gillespie dependence" );
    ASSERT( deps[4] == 6, "Gillespie dependence" );
    ASSERT( deps[5] == 7, "Gillespie dependence" );
    */
}

#endif
