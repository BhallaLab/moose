/*
 * =====================================================================================
 *
 *       Filename:  BoostSystem.cpp
 *
 *    Description:  Ode system described boost library.
 *
 *        Created:  04/11/2016 10:58:34 AM
 *       Compiler:  g++
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 *
 * =====================================================================================
 */

#include "../utility/boost_ode.h"
#include <iostream>
#include "VoxelPools.h"

#ifdef  USE_BOOST_ODE

BoostSys::BoostSys( ) : vp( NULL )
{ ; }

BoostSys::~BoostSys()
{ ;  }


void BoostSys::operator()( const vector_type_ y
        , vector_type_& dydt, const double t )
{
    assert( vp );
    VoxelPools::evalRates( y, dydt, t, vp );
}

#else      /* -----  not USE_BOOST_ODE  ----- */

#endif     /* -----  not USE_BOOST_ODE  ----- */
