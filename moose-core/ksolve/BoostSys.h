#ifndef BOOSTSYSTEM_H
#define BOOSTSYSTEM_H

#ifdef USE_BOOST

#include <vector>
#include <string>

#include <boost/numeric/odeint.hpp>

typedef double value_type_;
typedef std::vector<value_type_> vector_type_;

typedef boost::numeric::odeint::runge_kutta4< vector_type_ > rk4_stepper_type_;
typedef boost::numeric::odeint::runge_kutta_dopri5< vector_type_ > rk_dopri_stepper_type_;
typedef boost::numeric::odeint::modified_midpoint< vector_type_ > rk_midpoint_stepper_type_;

/*-----------------------------------------------------------------------------
 *  This stepper type found to be most suitable for adaptive solver. The gsl
 *  implementation has runge_kutta_fehlberg78 solver.
 *-----------------------------------------------------------------------------*/
typedef boost::numeric::odeint::runge_kutta_cash_karp54< vector_type_ > rk_karp_stepper_type_;
typedef boost::numeric::odeint::runge_kutta_fehlberg78< vector_type_ > rk_felhberg_stepper_type_;


class VoxelPools;

/*
 * =====================================================================================
 *        Class:  BoostSys
 *  Description:  The ode system of ksolve. It uses boost library to solve it.
 *  It is intended to be gsl replacement.
 * =====================================================================================
 */
class BoostSys
{
    public:
        BoostSys( );
        ~BoostSys();

        /* Operator is called by boost ode-solver */
        void operator()( const vector_type_ y , vector_type_& dydt, const double t );

        /* Pointer to the arbitrary parameters of the system */
        VoxelPools* vp;
        void* params;

        double epsAbs;
        double epsRel;
        std::string method;
};

#endif // USE_BOOST

#endif /* end of include guard: BOOSTSYSTEM_H */

