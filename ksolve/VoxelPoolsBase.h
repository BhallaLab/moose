/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _VOXEL_POOLS_BASE_H
#define _VOXEL_POOLS_BASE_H

#include <map>
#include <vector>

using namespace std;

class RateTerm;
class Stoich;
class Id;

/**
 * This is the base class for voxels used in reac-diffusion systems.
 * Each voxel manages all the molecular pools that live in it. This
 * is the S_ and Sinit_ vectors.
 * Additionally, the last set of entries in S_ and Sinit_ refer to the
 * proxy pools that participate in cross-node reactions, but are really
 * located on other compartments.
 */

class VoxelPoolsBase
{
public:
    VoxelPoolsBase();
    virtual ~VoxelPoolsBase();

    //////////////////////////////////////////////////////////////////
    // Compute access operations.
    //////////////////////////////////////////////////////////////////
    /// Allocate # of pools
    void resizeArrays( unsigned int totNumPools );

    /**
     * Returns # of pools
     */
    unsigned int size() const;

    /**
     * Returns the array of doubles of current mol #s at the specified
     * mesh index
     */
    const double* S() const;

    /// Returns a handle to the mol # vector.
    vector< double >& Svec();

    /**
     * Returns the array of doubles of current mol #s at the specified
     * mesh index. Dangerous, allows one to modify the values.
     */
    double* varS();

    /**
     * Returns the array of doubles of initial mol #s at the specified
     * mesh index
     */
    const double* Sinit() const;

    /**
     * Returns the array of doubles of initial mol #s at the specified
     * mesh index, as a writable array.
     */
    double* varSinit();

    /**
     * Assigns S = Sinit and sets up initial dt.
     */
    void reinit();

    /// Just assigns the volume without any cascading to other values.
    void setVolume( double vol );
    /// Return the volume of the voxel.
    double getVolume() const;

    /**
     * Assign the volume, and handle the cascading effects by scaling
     * all the dependent values of nInit and rates if applicable.
     */
    virtual void setVolumeAndDependencies( double vol );

    //////////////////////////////////////////////////////////////////
    // Field assignment functions
    //////////////////////////////////////////////////////////////////

    void setN( unsigned int i, double v );
    double getN( unsigned int ) const;
    void setNinit( unsigned int, double v );
    double getNinit( unsigned int ) const;
    void setDiffConst( unsigned int, double v );
    double getDiffConst( unsigned int ) const;
    //////////////////////////////////////////////////////////////////
    // Functions to set up rates vector
    //////////////////////////////////////////////////////////////////
    /**
     * Reassign entire rate vector.
     */
    virtual void updateAllRateTerms( const vector< RateTerm* >& rates,
                                     unsigned int numCoreRates ) = 0;

    /**
     * Update specified index on rate terms. The entire rates vector is
     * passed in for the source values, the index specifies which
     * entry on the local rate vector is to be updated.
     */
    virtual void updateRateTerms( const vector< RateTerm* >& rates,
                                  unsigned int numCoreRates, unsigned int index ) = 0;

    /**
     * Changes cross rate terms to zero if there is no junction
     */
    void filterCrossRateTerms( const vector< Id >& offSolverReacs, const vector< pair< Id, Id > >& offSolverReacCompts );

    //////////////////////////////////////////////////////////////////
    // Functions to handle cross-compartment reactions.
    //////////////////////////////////////////////////////////////////
    /**
     * Digests incoming data values for cross-compt reactions.
     * Sums the changes in the values onto the specified pools.
     */
    void xferIn( const vector< unsigned int >& poolIndex,
                 const vector< double >& values,
                 const vector< double >& lastValues,
                 unsigned int voxelIndex );

    /**
     * Used during initialization: Takes only the proxy pool values
     * from the incoming transfer data, and assigns it to the proxy
     * pools on current solver
     */
    void xferInOnlyProxies(
        const vector< unsigned int >& poolIndex,
        const vector< double >& values,
        unsigned int numProxyPools,
        unsigned int voxelIndex	);

    /// Assembles data values for sending out for x-compt reacs.
    void xferOut( unsigned int voxelIndex,
                  vector< double >& values,
                  const vector< unsigned int >& poolIndex );
    /// Adds the index of a voxel to which proxy data should go.
    void addProxyVoxy( unsigned int comptIndex,
                       Id comptId, unsigned int voxel );
    void addProxyTransferIndex( unsigned int comptIndex,
                                unsigned int transferIndex );

    /**
     * Assigns the Id and volume of the specified x-reac coupled
     * compartment
    void setComptVolume( unsigned int comptIndex, Id id, double vol );
     */

    /// True when this voxel has data to be transferred.
    bool hasXfer( unsigned int comptIndex ) const;

    /**
     * True when there is a junction. i1 must match, and if i2 is
     * non-zero then it too must match.
     */
    bool isVoxelJunctionPresent( Id i1, Id i2 ) const;
    //////////////////////////////////////////////////////////////////
    // Functions for cross-compartment rate scaling.
    //////////////////////////////////////////////////////////////////
    /**
     * Initialize/reset the vector xReacScaling_ for scale factors
     * for each cross reaction in this voxel.
     */
    void resetXreacScale( unsigned int size );

    /**
     * Multiply in scale factor for different cross-reac volumes,
     * to be applied to specified RateTerm in the rates_ vector.
     * Applies to forward reaction terms
     */
    void forwardReacVolumeFactor( unsigned int i, double volume );

    /**
     * Multiply in scale factor for different cross-reac volumes,
     * to be applied to specified RateTerm in the rates_ vector.
     * Applies to backward reaction terms
     */
    void backwardReacVolumeFactor( unsigned int i, double volume );

    /**
     * Return scale factor for specified entry on rate terms.
     * Index is # of cross-compartment rate term.
     */
    double getXreacScaleSubstrates( unsigned int i ) const;

    /**
     * Return scale factor for specified entry on rate terms.
     * Index is # of cross-compartment rate term.
     */
    double getXreacScaleProducts( unsigned int i ) const;

    void scaleVolsBufsRates( double ratio, const Stoich* stoichPtr );

    /// Debugging utility
    void print() const;

protected:
    const Stoich* stoichPtr_;
    vector< RateTerm* > rates_;

private:
    /**
     * S_ is the array of molecules. Stored as n, number of molecules
     * per mesh entry.
     * The poolIndex specifies which molecular species pool to use.
     *
     * The first numVarPools_ in the poolIndex are state variables and
     * are integrated using the ODE solver or the GSSA solver.
     * The next numBufPools_ are fixed but can be changed by the script.
     * The next numFuncPools_ are computed using arbitrary functions of
     * any of the molecule levels, and the time.
     * The functions evaluate _before_ the solver.
     * The functions should not cascade as there is no guarantee of
     * execution order.
     */
    vector< double > S_;

    /**
     * Sinit_ specifies initial conditions at t = 0. Whenever the reac
     * system is rebuilt or reinited, all S_ values become set to Sinit.
     * Also used for buffered molecules as the fixed values of these
     * molecules.
     */
    vector< double > Sinit_;

    /**
     * The number of pools (at the end of S and Sinit) that are here
     * just as proxies for some off-compartment pools that participate
     * in cross-compartment reactions.
    unsigned int numProxyPools_;
     */

    /**
     * proxyPoolVoxels_[comptIndex][#]
     * Used to build the transfer vector.
     * Stores the voxels of proxy pools in specified compartment,
     */
    vector< vector< unsigned int > > proxyPoolVoxels_;
    /**
     * proxyTransferIndex_[comptIndex][#]
     * Stores the index in the transfer vector for proxy pools that
     * live on this voxel. The poolIndex and # of pools is known from
     * the xferPoolIdx_ vector stored on the Ksolve.
     */
    vector< vector< unsigned int > > proxyTransferIndex_;

    /**
     * Looks up comptIndex from compt. Actually just a set would do.
     */
    map< Id, unsigned int > proxyComptMap_;

    /** This is what I would need to fully identify proxy compts and
     * voxels for each proxy reac.
    vector< Id, Id, unsigned int, unsigned int>  proxyComptVoxel
     */

    /**
     * Volume of voxel.
     */
    double volume_;

    /**
     * xReacScaleSubstrates_[crossRateTermIndex]
     * Product of substrateVol/voxelVol for each of the substrates,
     * for forward reacs.
     * Applied to R1 of the RateTerm. Used only for cross reactions.
     */
    vector< double > xReacScaleSubstrates_;

    /**
     * xReacScaleProducts_[crossRateTermIndex]
     * Product of productVol/voxelVol for each of the products,
     * for reverse reacs.
     * Applied to R2 of the RateTerm. Used only for cross reactions.
     */
    vector< double > xReacScaleProducts_;
};

#endif	// _VOXEL_POOLS_BASE_H
