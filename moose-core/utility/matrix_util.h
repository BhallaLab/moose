/*
 * =====================================================================================
 *
 *       Filename:  matrix_util.h
 *
 *    Description:  Utility function for working with blas matrices.
 *
 *        Version:  1.0
 *        Created:  05/10/2016 05:26:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 *
 * =====================================================================================
 */

#ifndef  matrix_util_INC
#define  matrix_util_INC

#ifdef USE_BOOST_ODE

#include <boost/numeric/ublas/matrix.hpp>
#include "boost/numeric/bindings/lapack/lapack.hpp"
#include "boost/numeric/bindings/lapack/geev.hpp"
typedef double value_type;
using namespace boost::numeric;

/**
 * @brief Swap row r1 and r2.
 */
void swapRows( ublas::matrix< value_type >& mat, unsigned int r1, unsigned int r2);

int reorderRows( ublas::matrix< value_type >& U, int start, int leftCol );

void eliminateRowsBelow( ublas::matrix< value_type >& U, int start, int leftCol );

unsigned int rankUsingBoost( ublas::matrix<value_type>& U );

#endif
#endif   /* ----- #ifndef matrix_util_INC  ----- */
