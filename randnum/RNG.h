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

#include <limits>
#include <iostream>
#include <random>

#include "Definitions.h"
#include "Distributions.h"

using namespace std;

namespace moose
{

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
            MOOSE_RANDOM_DEVICE rd_;
            setSeed( rd_() );
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
        void setSeed( const unsigned long seed )
        {
            seed_ = seed;
            if( seed == 0 )
            {
                MOOSE_RANDOM_DEVICE rd_;
                seed_ = rd_();
            }
            rng_.seed( seed_ );
        }

        /**
         * @brief Generate a uniformly distributed random number between a and b.
         *
         * @param a Lower limit (inclusive)
         * @param b Upper limit (inclusive).
         */
        T uniform( const T a, const T b)
        {
            return ( b - a ) * dist_( rng_ ) + a;
        }

        /**
         * @brief Return a uniformly distributed random number between 0 and 1
         * (exclusive).
         *
         * @return randum number.
         */
        T uniform( void )
        {
            return dist_( rng_ );
        }


    private:
        /* ====================  DATA MEMBERS  ======================================= */
        T res_;
        T seed_;

        moose::MOOSE_RNG_DEFAULT_ENGINE rng_;
        moose::MOOSE_UNIFORM_DISTRIBUTION<double> dist_;

}; /* -----  end of template class RNG  ----- */

}                                               /* namespace moose ends  */

#endif   /* ----- #ifndef __RNG_INC  ----- */
