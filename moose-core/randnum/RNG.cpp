/***
 *    Description:  Random number generator.
 *
 *        Created:  2019-05-26

 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *        License:  MIT License
 */

#include "RNG.h"

namespace moose {

RNG::RNG ()                                  /* constructor      */
{
    // Setup a random seed if possible.
    setRandomSeed( );
}

RNG::~RNG ()                                 /* destructor       */
{ ; }

void RNG::setRandomSeed( )
{
    MOOSE_RANDOM_DEVICE rd_;
    setSeed( rd_() );
}

double RNG::getSeed( void )
{
    return seed_;
}

/**
 * @brief If seed if 0 then set seed to a random number else set seed to
 * the given number.
 *
 * @param seed
 */
void RNG::setSeed( const unsigned long seed )
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
double RNG::uniform( const double a, const double b)
{
    return ( b - a ) * dist_( rng_ ) + a;
}

/**
 * @brief Return a uniformly distributed random number between 0 and 1
 * (exclusive).
 *
 * @return randum number.
 */
double RNG::uniform( void )
{
    return dist_( rng_ );
}

}

