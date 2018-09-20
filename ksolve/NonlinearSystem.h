/*
 * =====================================================================================
 *
 *    Description:  Compute root of a multi-dimensional system using boost
 *    libraries.
 *
 *        Version:  1.0
 *        Created:  04/13/2016 11:31:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 *
 * =====================================================================================
 */

#ifndef  NonlinearSystem_INC
#define  NonlinearSystem_INC

#include <iostream>
#include <sstream>
#include <functional>
#include <cerrno>
#include <iomanip>
#include <limits>
#include <algorithm>

// Boost ublas library of matrix algebra.
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "VoxelPools.h"

using namespace std;
using namespace boost::numeric;

typedef double value_type;
typedef ublas::vector<value_type> vector_type;
typedef ublas::matrix<value_type> matrix_type;


class ReacInfo
{
    public:
        size_t rank;
        size_t num_reacs;
        size_t num_mols;
        size_t nIter;
        double convergenceCriterion;
        double* T;
        VoxelPools* pool;
        vector< double > nVec;
        ublas::matrix< value_type > Nr;
        ublas::matrix< value_type > gamma;
};


/* Matrix inversion routine.
 * Uses lu_factorize and lu_substitute in uBLAS to invert a matrix 
 */

template<class T>
bool inverse(const ublas::matrix<T>& input, ublas::matrix<T>& inverse)
{
    using namespace boost::numeric::ublas;
    typedef permutation_matrix<std::size_t> pmatrix;
    // create a working copy of the input
    matrix<T> A(input);
    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());

    // perform LU-factorization
    int res = lu_factorize(A,pm);

    if( res != 0 ) return false;

    // create identity matrix of "inverse"
    inverse.assign(ublas::identity_matrix<T>(A.size1()));

    // backsubstitute to get the inverse
    lu_substitute(A, pm, inverse);

    return true;
}

// A sysmte of non-linear equations. Store the values in result.
class NonlinearSystem
{

public:

    NonlinearSystem( size_t systemSize ) : size_( systemSize )
    {
        f_.resize( size_, 0);
        slopes_.resize( size_, 0);
        x_.resize( size_, 0 );

        J_.resize( size_, size_, 0);
        invJ_.resize( size_, size_, 0);

        x2.resize( size_, 0);
        x1.resize( size_, 0);

        ri.nVec.resize( size_ );

        // Find machine epsilon to decide on dx. Machine eps is typically 2.22045e-16 on 64 bit machines/intel. 
        // Square root of this value is a very good value. The values matches with GSL solver nicely.
        // Making this value too small or too big gonna cause error in blas methods especially when 
        // computing inverse.  
        // Gnu-GSL uses GSL_SQRT_DBL_EPSILON  which is 1.4901161193847656e-08
        // dx_ = 1.4901161193847656e-08;
        dx_ = std::sqrt( numeric_limits<double>::epsilon() );
    }

    bool compute_at(const vector_type& x, vector_type& result)
    {
        return system(x, result);
    }

    int apply( )
    {
        return system(x_, f_);
    }

    int compute_jacobians( const vector_type& x, bool compute_inverse = false )
    {
        for( size_t i = 0; i < size_; i++)
        {
            // This trick I leart by looking at GSL implmentation.
            double dx = dx_ * std::fabs(x[i]);
            if( dx == 0 )
                dx = dx_;

            for( size_t j = 0; j < size_; j++)
            {
                vector_type temp = x;
                temp[j] += dx;
                vector_type res1, res2;
                auto r1 = compute_at(temp, res1);
                auto r2 = compute_at(x, res2);

                if( 0 == r1 && 0 == r2 )
                {
                    J_(i, j) = (res1[i] - res2[i]) / dx;
                    if( std::isnan(J_(i,j)) || std::isinf(J_(i,j)) )
                    {
                        /* Try increasing dx */
                        J_.clear();
                        return -1;
                    }
                }
                else
                {
                    J_.clear();
                    return -1;
                }
            }
        }

        if( compute_inverse )
        {
            try {
                inverse( J_, invJ_ );
            } catch ( exception & e ) {
                J_.clear();
                invJ_.clear();
                return -1;
            }
        }
        return 0;
    }

    template<typename T>
    void initialize( const T& x )
    {
        vector_type init;
        init.resize(size_, 0);

        for( size_t i = 0; i < size_; i++)
            init[i] = x[i];

        x_ = init;
        if( 0 == apply() )
        {
            if( 0 != compute_jacobians( init ) )
            {
                return;
            }
        }
        else
            return;
    }

    string to_string( )
    {
        stringstream ss;

        ss << "=======================================================";
        ss << endl << setw(25) << "State of system: " ;
        ss << " Argument: " << x_ << " Value : " << f_;
        ss << endl << setw(25) << "Jacobian: " << J_;
        ss << endl << setw(25) << "Inverse Jacobian: " << invJ_;
        ss << endl;
        return ss.str();
    }

    int system( const vector_type& x, vector_type& f )
    {
        size_t num_consv = ri.num_mols - ri.rank;
        for ( size_t i = 0; i < ri.num_mols; ++i )
        {
            double temp = x[i] * x[i] ;

            // if overflow
            if ( std::isnan( temp ) || std::isinf( temp ) )
                return -1;
            else
                ri.nVec[i] = temp;
        }

        vector< double > vels;
        ri.pool->updateReacVelocities( &ri.nVec[0], vels );

        assert( vels.size() == static_cast< unsigned int >( ri.num_reacs ) );

        // y = Nr . v
        // Note that Nr is row-echelon: diagonal and above.
        f.resize( ri.rank + num_consv);
        for ( size_t i = 0; i < ri.rank; ++i )
        {
            double temp = 0;
            for ( size_t j = i; j < ri.num_reacs; ++j )
                temp += ri.Nr(i, j ) * vels[j];
            f[i] = temp ;
        }

        // dT = gamma.S - T
        for ( size_t i = 0; i < num_consv; ++i )
        {
            double dT = - ri.T[i];
            for ( size_t  j = 0; j < ri.num_mols; ++j )
                dT += ri.gamma(i, j) * x[j] * x[j];
            f[ i + ri.rank] = dT ;
        }
        return 0;
    }


    /**
     * @brief Find roots using Newton-Raphson method.
     *
     * @param tolerance 1e-7
     * @param max_iter  Maximum number of iteration allowed , default 100
     *
     * @return  If successful, return true. Check the variable `x_` at
     * which the system f_ is close to zero (within  the tolerance).
     */
    bool find_roots_gnewton( double tolerance, size_t max_iter)
    {
        double norm2OfDiff = 1.0;
        size_t iter = 0;
        if(0 != apply() )
            return false;

        while( ublas::norm_2(f_) > tolerance )
        {
            iter += 1;
            ri.nIter = iter;

            if( 0 != compute_jacobians( x_, true ) )
            {
                J_.clear();
                invJ_.clear();
                return false;
            }

            vector_type correction = ublas::prod( invJ_, f_ );
            x_ -=  correction;

            // If could not compute the value of system successfully.
            if( 0 != apply() )
            {
                x_.clear();
                return false;
            }

            if( iter >= max_iter )
                break;

        }
        return true;
    }

public:
    const size_t size_;

    double dx_;

    vector_type f_;
    vector_type x_;
    vector_type slopes_;
    matrix_type J_;
    matrix_type invJ_;

    bool is_jacobian_valid_;
    bool is_f_positive_;

    // These vector keeps the temporary state computation.
    vector_type x2, x1;

    ReacInfo ri;
};

#endif   /* ----- #ifndef NonlinearSystem_INC  ----- */
