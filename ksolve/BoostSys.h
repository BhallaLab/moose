#ifndef BOOSTSYSTEM_H
#define BOOSTSYSTEM_H

#ifdef USE_BOOST_ODE

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

#endif // USE_BOOST_ODE

#endif /* end of include guard: BOOSTSYSTEM_H */

