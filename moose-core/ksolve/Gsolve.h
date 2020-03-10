/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GSOLVE_H
#define _GSOLVE_H

#include "../randnum/RNG.h"

class Stoich;

class Gsolve: public ZombiePoolInterface
{
public:
    Gsolve();
    ~Gsolve();

    // Assignment operator required for c++11
    Gsolve& operator=(const Gsolve& );

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff
    //////////////////////////////////////////////////////////////////
    Id getStoich() const;
    void setStoich( Id stoich ); /// Inherited from ZombiePoolInterface.
    Id getCompartment() const;
    void setCompartment( Id compt );

    unsigned int getNumLocalVoxels() const;
    unsigned int getNumAllVoxels() const;

    /**
     * Assigns the number of voxels used in the entire reac-diff
     * system. Note that fewer than this may be used on any given node.
     */
    void setNumAllVoxels( unsigned int num );

    /**
     * Assigns number of different pools (chemical species) present in
     * each voxel.
     */
    void setNumPools( unsigned int num ); /// Inherited.
    unsigned int getNumPools() const; /// Inherited.
    void setNumVarTotPools( unsigned int var, unsigned int tot );//Inherited
    VoxelPoolsBase* pools( unsigned int i ); /// Inherited.
    double volume( unsigned int i ) const;

    /// Returns the vector of pool Num at the specified voxel.
    vector< double > getNvec( unsigned int voxel) const;
    void setNvec( unsigned int voxel, vector< double > vec );
    //////////////////////////////////////////////////////////////////
    // Dest Finfos
    //////////////////////////////////////////////////////////////////
    void process( const Eref& e, ProcPtr p );
    void reinit( const Eref& e, ProcPtr p );
    void initProc( const Eref& e, ProcPtr p );
    void initReinit( const Eref& e, ProcPtr p );

    /**
     * Handles request to change volumes of voxels in this Ksolve, and
     * all cascading effects of this. At this point it won't handle
     * change in size of voxel array.
     */
    void updateVoxelVol( vector< double > vols );

    //////////////////////////////////////////////////////////////////
    // Solver setup functions
    //////////////////////////////////////////////////////////////////
    void rebuildGssaSystem();
    void fillMmEnzDep();
    void fillPoolFuncDep();
    void fillIncrementFuncDep();
    void insertMathDepReacs(unsigned int mathDepIndex, unsigned int firedReac);
    void makeReacDepsUnique();

    //////////////////////////////////////////////////////////////////
    // Solver interface functions
    //////////////////////////////////////////////////////////////////
    unsigned int getPoolIndex( const Eref& e ) const;
    unsigned int getVoxelIndex( const Eref& e ) const;
    vector< unsigned int > getNumFire( unsigned int voxel) const;

    /**
     * Inherited. Needed for reac-diff calculations so the Gsolve can
     * orchestrate the data transfer between the itself and the
     * diffusion solver.
     */
    void setDsolve( Id dsolve );

    //////////////////////////////////////////////////////////////////
    // ZombiePoolInterface inherited functions
    //////////////////////////////////////////////////////////////////

    void setN( const Eref& e, double v );
    double getN( const Eref& e ) const;
    void setNinit( const Eref& e, double v );
    double getNinit( const Eref& e ) const;
    void setDiffConst( const Eref& e, double v );
    double getDiffConst( const Eref& e ) const;

    void getBlock( vector< double >& values ) const;
    void setBlock( const vector< double >& values );

    /**
     * Rescale specified voxel rate term following rate constant change
     * or volume change. If index == ~0U then does all terms.
     */
    void updateRateTerms( unsigned int index );

    // Function for multithreading.
    size_t advance_chunk( const size_t begin, const size_t end, ProcPtr p );
    size_t recalcTimeChunk( const size_t begin, const size_t end, ProcPtr p);

    //////////////////////////////////////////////////////////////////
    /// Flag: returns true if randomized round to integers is done.
    bool getRandInit() const;
    /// Flag: set true if randomized round to integers is to be done.
    void setRandInit( bool val );

    /// Flag: returns true if randomized round to integers is done.
    bool getClockedUpdate() const;
    /// Flag: set true if randomized round to integers is to be done.
    void setClockedUpdate( bool val );

    unsigned int getNumThreads( ) const;
    void setNumThreads( unsigned int x );

    //////////////////////////////////////////////////////////////////
    static const Cinfo* initCinfo();
private:

    /**
     * @brief Number of threads to use when parallel version of Gsolve is
     * used.
     */
    size_t numThreads_;
    size_t grainSize_;

    GssaSystem sys_;

    /**
     * Each VoxelPools entry handles all the pools in a single voxel.
     * Each entry knows how to update itself in order to complete
     * the kinetic calculations for that voxel. The ksolver does
     * multinode management by indexing only the subset of entries
     * present on this node.
     */
    vector< GssaVoxelPools > pools_;

    /// First voxel indexed on the current node.
    unsigned int startVoxel_;

    /// Utility ptr used to help Pool Id lookups by the Ksolve.
    Stoich* stoichPtr_;

    /**
     * Id of diffusion solver, needed for coordinating numerics.
     */
    Id dsolve_;

    /// Pointer to diffusion solver
    ZombiePoolInterface* dsolvePtr_;

    /// Flag: True if atot should be updated every clock tick
    bool useClockedUpdate_;

    // private rng.
    moose::RNG rng_;
};

#endif	// _GSOLVE_H
