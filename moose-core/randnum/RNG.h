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
#include <cassert>

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

class RNG
{
    public:
        RNG ();
        ~RNG();

        void setRandomSeed( );

        /* ====================  ACCESSORS     ======================================= */
        double getSeed( void );

        void setSeed( const unsigned long seed );

        double uniform( const double a, const double b);

        double uniform( void );


    private:
        /* ====================  DATA MEMBERS  ======================================= */
        double res_;
        double seed_;

        moose::MOOSE_RNG_DEFAULT_ENGINE rng_;
        moose::MOOSE_UNIFORM_DISTRIBUTION<double> dist_;

}; /* -----  end of template class RNG  ----- */

}                                               /* namespace moose ends  */

#endif   /* ----- #ifndef __RNG_INC  ----- */
