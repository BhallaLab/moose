/***
 *       Filename:  Definitions.h
 *
 *    Description:  Various definitions.
 *
 *        Version:  0.0.1
 *        Created:  2018-08-04

 *       Revision:  none
 *
 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *
 *        License:  GNU GPL3
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <random>

namespace moose {

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  MOOSE's random device. Use it from <random>
 */
/* ----------------------------------------------------------------------------*/
typedef std::random_device MOOSE_RANDOM_DEVICE;

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Global random number generator engine. Everywhere we use this
 * engine.
 */
/* ----------------------------------------------------------------------------*/
typedef std::mersenne_twister_engine< std::uint_fast32_t, 32, 624, 397, 31
            , 0x9908b0df, 11
            , 0xffffffff, 7
            , 0x9d2c5680, 15
            , 0xefc60000, 18, 1812433253
        > MOOSE_RNG_DEFAULT_ENGINE;

}

#endif /* end of include guard: DEFINITIONS_H */
