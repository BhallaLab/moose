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
#include <boost/random/uniform_01.hpp>

#if  defined(BOOST_RANDOM_DEVICE_EXISTS)
#include <boost/random/random_device.hpp>
#endif  // BOOST_RANDOM_DEVICE_EXISTS
#else      /* -----  not USE_BOOST  ----- */
#include <ctime>
#include "randnum.h"
#endif     /* -----  not USE_BOOST  ----- */

#include <limits>
#include <iostream>

#ifdef ENABLE_CPP11
#include <random>
#endif

using namespace std;

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
            setRandomSeed( );
        }

        ~RNG ()                                 /* destructor       */
        { ; }

        void setRandomSeed( )
        {
#if defined(USE_BOOST)
#if defined(BOOST_RANDOM_DEVICE_EXISTS)
            boost::random::random_device rd;
            setSeed( rd() );
#endif
#elif defined(ENABLE_CPP11)
            std::random_device rd;
            setSeed( rd() );
#else
            mtseed( time(NULL) );
#endif     /* -----  not ENABLE_CPP11  ----- */
        }

        /* ====================  ACCESSORS     ======================================= */
        T getSeed( void )
        {
            return seed_;
        }

        /* ====================  MUTATORS      ======================================= */
        /**
         * @brief If seed if 0 then set seed to a random number else set seed to
         * the given number.
         *
         * @param seed
         */
        void setSeed( const unsigned long int seed )
        {
            seed_ = seed;
            if( seed == 0 )
            {
                setRandomSeed( );
                return;
            }

#if defined(USE_BOOST) || defined(ENABLE_CPP11)
            rng_.seed( seed_ );
#else
            mtseed( seed_ );
#endif
        }

        /**
         * @brief Generate a uniformly distributed random number between a and b.
         *
         * @param a Lower limit (inclusive)
         * @param b Upper limit (inclusive).
         */
        T uniform( const T a, const T b)
        {
#if defined(USE_BOOST) || defined(ENABLE_CPP11)
            return ( b - a ) * dist_( rng_ ) + a;
#else
            return (b-a) * mtrand() + a;
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
            return dist_( rng_ );
#else
            return mtrand();
#endif
        }


    private:
        /* ====================  DATA MEMBERS  ======================================= */
        T res_;
        T seed_;

#if USE_BOOST
        boost::random::mt19937 rng_;
        boost::random::uniform_01<T> dist_;
#elif ENABLE_CPP11
        std::mt19937 rng_;
        std::uniform_real_distribution<> dist_;
#endif     /* -----  not ENABLE_CPP11  ----- */

}; /* -----  end of template class RNG  ----- */


}                                               /* namespace moose ends  */

#endif   /* ----- #ifndef __RNG_INC  ----- */
