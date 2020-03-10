/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_POOL_INTERFACE_H
#define _ZOMBIE_POOL_INTERFACE_H

/**
 * This pure virtual base class is for solvers that want to talk to
 * the zombie pool.
 * The Eref specifies both the pool identity and the voxel number within
 * the pool.
 */
class ZombiePoolInterface
{
public:
    ZombiePoolInterface();

    /// Set initial # of molecules in given pool and voxel. Bdry cond.
    virtual void setNinit( const Eref& e, double val ) = 0;
    /// get initial # of molecules in given pool and voxel. Bdry cond.
    virtual double getNinit( const Eref& e ) const = 0;

    /// Set # of molecules in given pool and voxel. Varies with time.
    virtual void setN( const Eref& e, double val ) = 0;
    /// Get # of molecules in given pool and voxel. Varies with time.
    virtual double getN( const Eref& e ) const = 0;

    /// Diffusion constant: Only one per pool, voxel number is ignored.
    virtual void setDiffConst( const Eref& e, double val ) = 0;
    /// Diffusion constant: Only one per pool, voxel number is ignored.
    virtual double getDiffConst( const Eref& e ) const = 0;

    /// Motor constant: Only one per pool, voxel number is ignored.
    /// Used only in Dsolves, so here I put in a dummy.
    virtual void setMotorConst( const Eref& e, double val )
    {;}

    /// Specifies number of pools (species) handled by system.
    virtual void setNumPools( unsigned int num ) = 0;
    /// gets number of pools (species) handled by system.
    virtual unsigned int getNumPools() const = 0;

    /// Specifies number of var pools and total pools including buffered .
    virtual void setNumVarTotPools( unsigned int var, unsigned int tot )=0;

    /// Assign number of voxels (size of pools_ vector )
    virtual void setNumAllVoxels( unsigned int numVoxels ) = 0;
    /// Number of voxels here. pools_.size() == getNumLocalVoxels
    virtual unsigned int getNumLocalVoxels() const = 0;
    /// Return a pointer to the specified VoxelPool.
    virtual VoxelPoolsBase* pools( unsigned int i ) = 0;

    /// Return volume of voxel i.
    virtual double volume( unsigned int i ) const = 0;

    /**
     * Gets block of data. The first 4 entries are passed in
     * on the 'values' vector: the start voxel, numVoxels,
     * start pool#, numPools.
     * These are followed by numVoxels * numPools of data values
     * which are filled in by the function.
     * We assert that the entire requested block is present in
     * this ZombiePoolInterface.
     * The block is organized as an array of arrays of voxels;
     * values[pool#][voxel#]
     *
     * Note that numVoxels and numPools are the number in the current
     * block, not the upper limit of the block. So
     * values.size() == 4 + numPools * numVoxels.
     */
    virtual void getBlock( vector< double >& values ) const = 0;

    /**
     * Sets block of data. The first 4 entries
     * on the 'values' vector are the start voxel, numVoxels,
     * start pool#, numPools. These are
     * followed by numVoxels * numPools of data values.
     */
    virtual void setBlock( const vector< double >& values ) = 0;

    /**
     * Informs the ZPI about the stoich, used during subsequent
     * computations.
     * Called to wrap up the model building. The Stoich
     * does this call after it has set up its own path.
     */
    virtual void setStoich( Id stoich ) = 0;

    /// Assigns the diffusion solver. Used by the reac solvers
    virtual void setDsolve( Id dsolve ) = 0;

    /// Assigns compartment.
    virtual void setCompartment( Id compartment );
    Id getCompartment() const;

    /// Used for telling Dsolver to handle all ops across Junctions
    virtual void updateJunctions( double dt );

    /// Used to tell Dsolver to assign 'prev' values.
    virtual void setPrev();
    /**
     * Informs the solver that the rate terms or volumes have changed
     * and that the parameters must be updated.
     * The index specifies which rateTerm to change, and if it is
     * ~0U it means update all of them.
     */
    virtual void updateRateTerms( unsigned int index = ~0U ) = 0;

    /// Return pool index, using Stoich ptr to do lookup.
    virtual unsigned int getPoolIndex( const Eref& er ) const = 0;

    //////////////////////////////////////////////////////////////
protected:
    /**
     * Stoich is the class that sets up the reaction system and
     * manages the stoichiometry matrix
     */
    Id stoich_;

    /// Id of Chem compartment used to figure out volumes of voxels.
    Id compartment_;

    /// Flag: True when solver setup has been completed.
    bool isBuilt_;
};

#endif    // _ZOMBIE_POOL_INTERFACE_H
