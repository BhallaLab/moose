/*
 * =====================================================================================
 *
 *       Filename:  RNG.h
 *
 *    Description:  Random Number Generator class
 *
 *        Created:  05/09/2016 12:00:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 *
 * =====================================================================================
 */


#ifndef  __RNG_INC
#define  __RNG_INC

#ifdef  USE_BOOST
#include <boost/random.hpp>
#include <boost/random/uniform_int.hpp>

#if  BOOST_RANDOM_DEVICE_EXISTS
#include <boost/random/random_device.hpp>
#endif     /* -----  BOOST_RANDOM_DEVICE_EXISTS  ----- */

#else      /* -----  not USE_BOOST  ----- */

#ifdef  ENABLE_CPP11
#include <random>
#elif USE_GSL      /* -----  not ENABLE_CPP11 and using GSL  ----- */
#include <ctime>
#include <gsl/gsl_rng.h>
#endif     /* -----  not ENABLE_CPP11  ----- */

#endif     /* -----  not USE_BOOST  ----- */

#include <limits>
#include <iostream>

namespace moose {

/* 
 * =====================================================================================
 *        Class:  RNG
 *  Description:  Random number generator class.
 * =====================================================================================
 */

template < typename T >
class RNG
{
    public:
        RNG ()                                  /* constructor      */
        {
            // Setup a random seed if possible.
#ifdef  ENABLE_CPP11 
            std::random_device rd;
            setSeed( rd() );
#elif defined(USE_BOOST) && defined(BOOST_RANDOM_DEVICE_EXISTS)
            boost::random::random_device rd;
            setSeed( rd() );
#elif USE_GSL
            gsl_r_ = gsl_rng_alloc( gsl_rng_default );
            gsl_rng_set( gsl_r_, time(NULL) );
#else      /* -----  not ENABLE_CPP11  ----- */

#endif     /* -----  not ENABLE_CPP11  ----- */

        }

        ~RNG ()                                 /* destructor       */
        {

#if defined(USE_BOOST) || defined(ENABLE_CPP11) 
#else
            gsl_rng_free( gsl_r_ );
#endif

        }

        /* ====================  ACCESSORS     ======================================= */
        T getSeed( void )
        {
            return seed_;
        }

        /* ====================  MUTATORS      ======================================= */
        void setSeed( const T seed )
        {
#if defined(USE_BOOST) || defined(ENABLE_CPP11)
            seed_ = seed;
            rng_.seed( seed_ );
#elif USE_GSL
            gsl_rng_set(gsl_r_, seed );
#else 
            std::srand( seed_ );
#endif
        }

        /**
         * @brief Generate a uniformly distributed random number between a and b.
         *
         * @param a Lower limit (inclusive)
         * @param b Upper limit (exclusive).
         */
        T uniform( const T a, const T b)
        {
            size_t maxInt = std::numeric_limits<int>::max();
#if defined(USE_BOOST) || defined(ENABLE_CPP11)
            return ( (b - a ) * (T)dist_( rng_ ) / maxInt ) + a;
#elif USE_GSL
            return ( (b -a ) * (T)gsl_rng_get( gsl_r_ ) / gsl_rng_max( gsl_r_ ) + a );
#else
            return (b-a) * (T)rand() / RAND_MAX + a;
#endif
        }

        /**
         * @brief Return a uniformly distributed random number between 0 and 1
         * (exclusive).
         *
         * @return randum number.
         */
        T uniform( void )
        {
#if defined(USE_BOOST) || defined(ENABLE_CPP11)
            return (T)dist_( rng_ ) / std::numeric_limits<int>::max();
#elif USE_GSL
            return (T)gsl_rng_uniform( gsl_r_ );
#else
            return (T)rand( ) / RAND_MAX;
#endif
        }


    private:
        /* ====================  DATA MEMBERS  ======================================= */
        T res_;
        T seed_;

#if USE_BOOST
        boost::random::mt19937 rng_;
        boost::random::uniform_int_distribution<> dist_;
#elif ENABLE_CPP11
        std::mt19937 rng_;
        std::uniform_int_distribution<> dist_;
#else      /* -----  not ENABLE_CPP11  ----- */
        gsl_rng* gsl_r_;
#endif     /* -----  not ENABLE_CPP11  ----- */

}; /* -----  end of template class RNG  ----- */


}                                               /* namespace moose ends  */

#endif   /* ----- #ifndef __RNG_INC  ----- */
