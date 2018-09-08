/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _VOXEL_POOLS_H
#define _VOXEL_POOLS_H

#include "OdeSystem.h"
#include "VoxelPoolsBase.h"

#ifdef USE_BOOST_ODE
#include "BoostSys.h"
#endif


class Stoich;
class ProcInfo;

/**
 * This is the class for handling reac-diff voxels used for deterministic
 * computations.
 */
class VoxelPools: public VoxelPoolsBase
{
public:
    VoxelPools();
    virtual ~VoxelPools();

    //////////////////////////////////////////////////////////////////
    void reinit( double dt );
    //////////////////////////////////////////////////////////////////
    // Solver interface functions
    //////////////////////////////////////////////////////////////////
    /**
     * setStoich: Assigns the ODE system and Stoich.
     * Stoich may be modified to create a new RateTerm vector
     * in case the volume of this VoxelPools is new.
     */
    void setStoich( Stoich* stoich, const OdeSystem* ode );

    const Stoich* getStoich( );

    /// Do the numerical integration. Advance the simulation.
    void advance( const ProcInfo* p );

    /// Set initial timestep to use by the solver.
    void setInitDt( double dt );

#ifdef USE_GSL      /* -----  not USE_BOOST_ODE  ----- */
    static int gslFunc( double t, const double* y, double *dydt, void* params);
#elif  USE_BOOST_ODE
    static void evalRates( VoxelPools* vp, const vector_type_& y, vector_type_& dydt );
#endif     /* -----  not USE_BOOST_ODE  ----- */

    //////////////////////////////////////////////////////////////////
    // Rate manipulation and calculation functions
    //////////////////////////////////////////////////////////////////
    /// Handles volume change and subsequent cascading updates.
    void setVolumeAndDependencies( double vol );

    /// Updates all the rate constants from the reference rates vector.
    void updateAllRateTerms( const vector< RateTerm* >& rates,
                             unsigned int numCoreRates	);
    /**
     * updateRateTerms updates the rate consts of a belonging to
     * the specified index on the rates vector.
     * It does recaling and assigning using values from the
     * internal rates vector.
     */
    void updateRateTerms( const vector< RateTerm* >& rates,
                          unsigned int numCoreRates, unsigned int index );

    /**
     * Core computation function. Updates the reaction velocities
     * vector yprime given the current mol 'n' vector s.
     */
    void updateRates( const double* s, double* yprime ) const;

    /**
     * updateReacVelocities computes the velocity *v* of each reaction
     * from the vector *s* of pool #s.
     * This is a utility function for programs like SteadyState that
     * need to analyze velocity.
     */
    void updateReacVelocities(
        const double* s, vector< double >& v ) const;

    /// Used for debugging.
    void print() const;
private:

#ifdef USE_GSL
    gsl_odeiv2_driver* driver_;
    gsl_odeiv2_system sys_;
#endif

    double absTol_;
    double relTol_;
    string method_;

};

#endif	// _VOXEL_POOLS_H
