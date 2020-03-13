/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GSSA_VOXEL_POOLS_BASE_H
#define _GSSA_VOXEL_POOLS_BASE_H

#include "../randnum/RNG.h"

class Stoich;

class GssaVoxelPools: public VoxelPoolsBase
{
public:
    GssaVoxelPools();

    virtual ~GssaVoxelPools();

    // Solver interface functions
    void advance( const ProcInfo* p );

    void recalcTime( const GssaSystem* g, double currTime );

    void updateDependentMathExpn( const GssaSystem* g, unsigned int rindex, double time );

    void updateDependentRates( const vector< unsigned int >& deps, const Stoich* stoich );

    unsigned int pickReac();

    void setNumReac( unsigned int n );

    void advance( const ProcInfo* p, const GssaSystem* g );

    vector< unsigned int > numFire() const;

    /**
    * Cleans out all reac rates and recalculates atot. Needed whenever a
    * mol conc changes, or if there is a roundoff error. Returns true
    * if OK, returns false if it is in a stuck state and atot<=0
    */
    bool refreshAtot( const GssaSystem* g );

    /**
     * Builds the gssa system as needed.
     */
    void reinit( const GssaSystem* g );

    void updateAllRateTerms( const vector< RateTerm* >& rates,
            unsigned int numCoreRates	);
    void updateRateTerms( const vector< RateTerm* >& rates,
            unsigned int numCoreRates, unsigned int index );

    double getReacVelocity( unsigned int r, const double* s ) const;
    void updateReacVelocities( const GssaSystem* g,
            const double* s, vector< double >& v ) const;

    /**
     * Assign the volume, and handle the cascading effects by scaling
     * all the dependent values of nInit and rates if applicable.
     */
    void setVolumeAndDependencies( double vol );

    /**
     * Digests incoming data values for cross-compt reactions.
     * Sums the changes in the values onto the specified pools.
     */
    void xferIn( XferInfo& xf,
            unsigned int voxelIndex, const GssaSystem* g );

    void setStoich( const Stoich* stoichPtr );

private:
    /// Time at which next event will occur.
    double t_;

    /**
     * Total propensity of all the reactions in the system
     */
    double atot_;

    /**
     * State vector of reaction velocities. Only a subset are
     * recalculated on each step.
     */
    vector< double > v_;
    // Possibly we should put independent RNGS, so save one here.

    // Count how many times each reaction has fired.
    vector< unsigned int > numFire_;

    /**
     * @brief RNG.
     */
    moose::RNG rng_;
};

#endif	// _GSSA_VOXEL_POOLS_H
