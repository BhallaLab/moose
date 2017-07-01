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
        int rank;
        int num_reacs;
        size_t num_mols;
        int nIter;
        double convergenceCriterion;
        double* T;
        VoxelPools* pool;
        vector< double > nVec;
        ublas::matrix< value_type > Nr;
        ublas::matrix< value_type > gamma;
};


/* Matrix inversion routine.
   Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
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
        dx_ = sqrt( numeric_limits<double>::epsilon() );
    }

    vector_type compute_at(const vector_type& x)
    {
        vector_type result( size_ );
        system(x, result);
        return result;
    }

    int apply( )
    {
        return system(x_, f_);
    }

    int compute_jacobians( const vector_type& x, bool compute_inverse = true )
    {
        for( size_t i = 0; i < size_; i++)
            for( size_t j = 0; j < size_; j++)
            {
                vector_type temp = x;
                temp[j] += dx_;
                J_(i, j) = (compute_at(temp)[i] - compute_at(x)[i]) / dx_;
            }

        // is_jacobian_valid_ = true;
        // Keep the inverted J_ ready
        //if(is_jacobian_valid_ and compute_inverse )
        if( compute_inverse )
            inverse( J_, invJ_ );

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
        apply();

        compute_jacobians( init );
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
        int num_consv = ri.num_mols - ri.rank;
        for ( size_t i = 0; i < ri.num_mols; ++i )
        {
            double temp = x[i] * x[i] ;

#if 0
            // if overflow
            if ( std::isnan( temp ) or std::isinf( temp ) )
            {
                cerr << "Failed: ";
                for( auto v : ri.nVec ) cerr << v << ", ";
                cerr << endl;
                return -1;
            }
#endif
            ri.nVec[i] = temp;
        }

        vector< double > vels;
        ri.pool->updateReacVelocities( &ri.nVec[0], vels );

        assert( vels.size() == static_cast< unsigned int >( ri.num_reacs ) );

        // y = Nr . v
        // Note that Nr is row-echelon: diagonal and above.
        for ( int i = 0; i < ri.rank; ++i )
        {
            double temp = 0;
            for ( int j = i; j < ri.num_reacs; ++j )
                temp += ri.Nr(i, j ) * vels[j];
            f[i] = temp ;
        }

        // dT = gamma.S - T
        for ( int i = 0; i < num_consv; ++i )
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
    bool find_roots_gnewton( double tolerance = 1e-7 , size_t max_iter = 50)
    {
        //tolerance = sqrt( numeric_limits<double>::epsilon() );
        double norm2OfDiff = 1.0;
        size_t iter = 0;
        int status = apply();

        while( ublas::norm_2(f_) >= tolerance )
        {
            iter += 1;
            compute_jacobians( x_, true );
            vector_type correction = ublas::prod( invJ_, f_ );
            x_ -=  correction;

            // If could not compute the value of system successfully.
            status = apply();
            if( 0 != status )
                return false;

            if( iter >= max_iter )
                break;

        }

        ri.nIter = iter;

        if( iter >= max_iter )
            return false;

        return true;
    }

    /**
     * @brief Compute the slope of function in given dimension.
     *
     * @param which_dimen The index of dimension.
     *
     * @return  Slope.
     */
    value_type slope( unsigned int which_dimen )
    {
        vector_type x = x_;
        x[which_dimen] += dx_;
        // x1 and x2 holds the f_ of system at x_ and x (which is x +
        // some step)
        system( x_, x1 );
        system( x, x2 );
        return ublas::norm_2( (x2 - x1)/dx_ );
    }


    /**
     * @brief Makes the first guess. After this call the Newton method.
     */
    void correction_step(  )
    {
        // Get the jacobian at current point. Notice that in this method, we
        // don't have to compute inverse of jacobian

        vector_type direction( size_ );

        // Now take the largest step possible such that the value of system at
        // (x_ - step ) is lower than the value of system as x_.
        vector_type nextState( size_ );

        apply();

        unsigned int i = 0;

        double factor = 1e-2;
        while( true )
        {
            i += 1;
            compute_jacobians( x_, false );
            // Make a move in either side of direction. In whichever direction
            // the function decreases.
            direction = ublas::prod( J_, f_ );
            nextState = x_ - factor * direction;
            if( ublas::norm_2( compute_at( nextState ) ) >= ublas::norm_2(compute_at(x_)))
                factor = factor / 2.0;
            else
            {
                cerr << "Correction term applied ";
                x_ = nextState;
                apply();
                break;
            }

            if ( i > 20 )
                break;
        }
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
