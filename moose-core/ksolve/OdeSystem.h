/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ODE_SYSTEM_H
#define _ODE_SYSTEM_H

#if  USE_BOOST_ODE
#include "BoostSys.h"
#elif USE_GSL
#include <gsl/gsl_odeiv2.h>
#endif     /* -----  not USE_BOOST_ODE  ----- */

class OdeSystem {
    public:
        OdeSystem()
            : method( "rk5" ),
            initStepSize( 0.001 ),
            epsAbs( 1e-6 ),
            epsRel( 1e-6 )
    {;}

        std::string method;

        double initStepSize;
        double epsAbs; // Absolute error
        double epsRel; // Relative error

#ifdef USE_GSL
        // GSL stuff
        gsl_odeiv2_system gslSys;
        const gsl_odeiv2_step_type* gslStep;
#endif

#if  USE_BOOST_ODE
        size_t dimension;
#endif     /* -----  USE_BOOST_ODE  ----- */
};

#endif // _ODE_SYSTEM_H
