/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifdef ENABLE_CPP11
#include <memory>
#endif

#include "header.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"
#include "GssaSystem.h"
#include "GssaVoxelPools.h"
#include "../randnum/RNG.h"
#include "../basecode/global.h"

/**
 * The SAFETY_FACTOR Protects against the total propensity exceeding
 * the cumulative
 * sum of propensities, atot. We do a lot of adding and subtracting of
 * dependency terms from atot. Roundoff error will eventually cause
 * this to drift from the true sum. To guarantee that we never lose
 * the propensity of the last reaction, this safety factor scales the
 * first calculation of atot to be slightly larger. Periodically this
 * will cause the reaction picking step to exceed the last reaction
 * index. This is safe, we just pick another random number.
 * This will happen rather infrequently.
 * That is also a good time to update the cumulative sum.
 * A double should have >15 digits, so cumulative error will be much
 * smaller than this.
 */
const double SAFETY_FACTOR = 1.0 + 1.0e-9;

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////

GssaVoxelPools::GssaVoxelPools() :
     VoxelPoolsBase(), t_( 0.0 ), atot_( 0.0 )
{ ; }

GssaVoxelPools::~GssaVoxelPools()
{
    for ( unsigned int i = 0; i < rates_.size(); ++i )
        delete( rates_[i] );
}

//////////////////////////////////////////////////////////////
// Solver ops
//////////////////////////////////////////////////////////////

void GssaVoxelPools::updateDependentMathExpn(
    const GssaSystem* g, unsigned int rindex, double time )
{
    // The issue is that if the expression depends on t, we really need
    // to update it every timestep. But then a cascading set of reacs
    // should also be updated.
    // The lower commented block has all funcs updated every time step,
    // but this too
    // doesn't update the cascading reacs. So this is now handled by the
    // useClockedUpdate flag, and we use the upper block here instead.
    /*
    const vector< unsigned int >& deps = g->dependentMathExpn[ rindex ];
    for( vector< unsigned int >::const_iterator
    		i = deps.begin(); i != deps.end(); ++i ) {
    		g->stoich->funcs( *i )->evalPool( varS(), time );
    }
    */
    unsigned int numFuncs = g->stoich->getNumFuncs();
    for( unsigned int i = 0; i < numFuncs; ++i )
    {
        g->stoich->funcs( i )->evalPool( varS(), time );
    }
}

void GssaVoxelPools::updateDependentRates(
    const vector< unsigned int >& deps, const Stoich* stoich )
{
    for ( vector< unsigned int >::const_iterator
            i = deps.begin(); i != deps.end(); ++i )
    {
        atot_ -= fabs( v_[ *i ] );
        // atot_ += ( v[ *i ] = ( *rates_[ *i ] )( S() );
        atot_ += fabs( v_[ *i ] = getReacVelocity( *i, S() ) );
    }
}

unsigned int GssaVoxelPools::pickReac()
{
    double r = rng_.uniform( ) * atot_;
    double sum = 0.0;

    // This is an inefficient way to do it. Can easily get to
    // log time or thereabouts by doing one or two levels of
    // subsidiary tables. Too many levels causes slow-down because
    // of overhead in managing the tree.
    // Slepoy, Thompson and Plimpton 2008
    // report a linear time version.
    for ( vector< double >::const_iterator
            i = v_.begin(); i != v_.end(); ++i )
    {
        if ( r < ( sum += fabs( *i ) ) )
        {
            // double vel = fabs( getReacVelocity( i - v_.begin(), S() ) );
            // assert( vel >= 0 );
            return static_cast< unsigned int >( i - v_.begin() );
        }
    }
    return v_.size();
}

void GssaVoxelPools::setNumReac( unsigned int n )
{
    v_.clear();
    v_.resize( n, 0.0 );
    numFire_.resize( n, 0 );
}

/**
 * Cleans out all reac rates and recalculates atot. Needed whenever a
 * mol conc changes, or if there is a roundoff error.
 * Returns true if OK, returns false if it is in a stuck state and atot<=0
 */
bool GssaVoxelPools::refreshAtot( const GssaSystem* g )
{
    updateReacVelocities( g, S(), v_ );
    atot_ = 0;
    for ( vector< double >::const_iterator
            i = v_.begin(); i != v_.end(); ++i )
        atot_ += fabs(*i);
    atot_ *= SAFETY_FACTOR;
    // Check if the system is in a stuck state. If so, terminate.
    if ( atot_ <= 0.0 )
    {
        return false;
    }
    return true;
}

/**
 * Recalculates the time for the next event. Used when we have a clocked
 * update of rates due to timed functions. In such cases the propensities
 * may change invisibly, so we need to update time estimates
 */
void GssaVoxelPools::recalcTime( const GssaSystem* g, double currTime )
{
    updateDependentMathExpn( g, 0, currTime );
    refreshAtot( g );
    assert( t_ > currTime );
    t_ = currTime;
    double r = rng_.uniform( );
    while( r == 0.0 )
        r = rng_.uniform( );
    t_ -= ( 1.0 / atot_ ) * log( r );
}

void GssaVoxelPools::advance( const ProcInfo* p, const GssaSystem* g )
{
    double nextt = p->currTime;
    while ( t_ < nextt )
    {
        if ( atot_ <= 0.0 )   // reac system is stuck, will not advance.
        {
            t_ = nextt;
            return;
        }
        unsigned int rindex = pickReac();
        assert( g->stoich->getNumRates() == v_.size() );
        if ( rindex >= g->stoich->getNumRates() )
        {
            // probably cumulative roundoff error here.
            // Recalculate atot to avoid, and redo.
            if ( !refreshAtot( g ) )   // Stuck state.
            {
                t_ = nextt;
                return;
            }
            // We had a roundoff error, fixed it, but now need to be sure
            // we only fire a reaction where this is permissible.
            for ( unsigned int i = v_.size(); i > 0; --i )
            {
                if ( fabs( v_[i-1] ) > 0.0 )
                {
                    rindex = i - 1;
                    break;
                }
            }
            assert( rindex < v_.size() );
        }

        double sign = double(v_[rindex] >= 0) - double(0 > v_[rindex] );
        g->transposeN.fireReac( rindex, Svec(), sign );
		numFire_[rindex]++;

        double r = rng_.uniform();
        while ( r <= 0.0 )
        {
            r = rng_.uniform();
        }
        t_ -= ( 1.0 / atot_ ) * log( r );
        // g->stoich->updateFuncs( varS(), t_ ); // Handled next line.
        updateDependentMathExpn( g, rindex, t_ );
        updateDependentRates( g->dependency[ rindex ], g->stoich );
    }
}

void GssaVoxelPools::reinit( const GssaSystem* g )
{
    rng_.setSeed( moose::__rng_seed__ );
    VoxelPoolsBase::reinit(); // Assigns S = Sinit;
    unsigned int numVarPools = g->stoich->getNumVarPools();
    g->stoich->updateFuncs( varS(), 0 );

    double* n = varS();

    if ( g->useRandInit )
    {
        // round up or down probabilistically depending on fractional
        // num molecules.
        for ( unsigned int i = 0; i < numVarPools; ++i )
        {
            double base = floor( n[i] );
            assert( base >= 0.0 );
            double frac = n[i] - base;
            if ( rng_.uniform() > frac )
                n[i] = base;
            else
                n[i] = base + 1.0;
        }
    }
    else     // Just round to the nearest int.
    {
        for ( unsigned int i = 0; i < numVarPools; ++i )
        {
            n[i] = round( n[i] );
        }
    }
    t_ = 0.0;
    refreshAtot( g );
	numFire_.assign( v_.size(), 0 );
}

vector< unsigned int > GssaVoxelPools::numFire() const
{
	return numFire_;
}

/////////////////////////////////////////////////////////////////////////
// Rate computation functions
/////////////////////////////////////////////////////////////////////////

void GssaVoxelPools::updateAllRateTerms( const vector< RateTerm* >& rates,
        unsigned int numCoreRates )
{
    for ( unsigned int i = 0; i < rates_.size(); ++i )
        delete( rates_[i] );
    rates_.resize( rates.size() );

    for ( unsigned int i = 0; i < numCoreRates; ++i )
        rates_[i] = rates[i]->copyWithVolScaling( getVolume(), 1, 1 );
    for ( unsigned int i = numCoreRates; i < rates.size(); ++i )
        rates_[i] = rates[i]->copyWithVolScaling( getVolume(),
                    getXreacScaleSubstrates(i - numCoreRates),
                    getXreacScaleProducts(i - numCoreRates ) );
}

void GssaVoxelPools::updateRateTerms( const vector< RateTerm* >& rates,
                                      unsigned int numCoreRates, unsigned int index 	)
{
    // During setup or expansion of the reac system, this might be called
    // before the local rates_ term is assigned. If so, ignore.
    if ( index >= rates_.size() )
        return;
    delete( rates_[index] );
    if ( index >= numCoreRates )
        rates_[index] = rates[index]->copyWithVolScaling(
                            getVolume(),
                            getXreacScaleSubstrates(index - numCoreRates),
                            getXreacScaleProducts(index - numCoreRates ) );
    else
        rates_[index] = rates[index]->copyWithVolScaling(
                            getVolume(), 1.0, 1.0 );
}
/**
 * updateReacVelocities computes the velocity *v* of each reaction.
 * This is a utility function for programs like SteadyState that need
 * to analyze velocity.
 */
void GssaVoxelPools::updateReacVelocities( const GssaSystem* g,
        const double* s, vector< double >& v ) const
{
    const KinSparseMatrix& N = g->stoich->getStoichiometryMatrix();
    assert( N.nColumns() == rates_.size() );

    vector< RateTerm* >::const_iterator i;
    v.clear();
    v.resize( rates_.size(), 0.0 );
    vector< double >::iterator j = v.begin();

    for ( i = rates_.begin(); i != rates_.end(); i++)
    {
        *j++ = (**i)( s );
        assert( !std::isnan( *( j-1 ) ) );
    }
}

double GssaVoxelPools::getReacVelocity(
    unsigned int r, const double* s ) const
{
    double v = rates_[r]->operator()( s );
    // assert( v >= 0.0 );
    return v;

    // return rates_[r]->operator()( s );
}

void GssaVoxelPools::setStoich( const Stoich* stoichPtr )
{
    stoichPtr_ = stoichPtr;
}

// Handle volume updates. Inherited virtual func.
void GssaVoxelPools::setVolumeAndDependencies( double vol )
{
    VoxelPoolsBase::setVolumeAndDependencies( vol );
    stoichPtr_->setupCrossSolverReacVols();
    updateAllRateTerms( stoichPtr_->getRateTerms(),
                        stoichPtr_->getNumCoreRates() );
}

//////////////////////////////////////////////////////////////
// Handle cross compartment reactions
//////////////////////////////////////////////////////////////

/*
 * Not sure if we need it. Hold off for now.
static double integralTransfer( double propensity )
{
	double t= floor( propensity );
	if ( rng_.uniform() < propensity - t )
		return t + 1;
	return t;
}
*/

void GssaVoxelPools::xferIn( XferInfo& xf,
                             unsigned int voxelIndex, const GssaSystem* g )
{
    unsigned int offset = voxelIndex * xf.xferPoolIdx.size();
    vector< double >::const_iterator i = xf.values.begin() + offset;
    vector< double >::const_iterator j = xf.lastValues.begin() + offset;
    vector< double >::iterator m = xf.subzero.begin() + offset;
    double* s = varS();
    bool hasChanged = false;
    for ( vector< unsigned int >::const_iterator
            k = xf.xferPoolIdx.begin(); k != xf.xferPoolIdx.end(); ++k )
    {
        double& x = s[*k];
        // cout << x << "	i = " << *i << *j << "	m = " << *m << endl;
        double dx = *i++ - *j++;
        double base = floor( dx );
        if ( rng_.uniform() > dx - base )
            x += base;
        else
            x += base + 1.0;

        // x += round( *i++ - *j );
        if ( x < *m )
        {
            *m -= x;
            x = 0;
        }
        else
        {
            x -= *m;
            *m = 0;
        }
        /*
        double y = fabs( x - *j );
        // hasChanged |= ( fabs( x - *j ) < 0.1 ); // they are all integers.
        hasChanged |= ( y > 0.1 ); // they are all integers.
        */
        // j++;
        m++;
    }
    // If S has changed, then I should update rates of all affected reacs.
    // Go through stoich matrix to find affected reacs for each mol
    // Store in list, since some may be hit more than once in this func.
    // When done, go through and update all affected ones.
    /*
    if ( hasChanged ) {
    	refreshAtot( g );
    }
    */
    // Does this fix the problem of negative concs?
    refreshAtot( g );
}

void GssaVoxelPools::xferInOnlyProxies(
    const vector< unsigned int >& poolIndex,
    const vector< double >& values,
    unsigned int numProxyPools,
    unsigned int voxelIndex	)
{
    unsigned int offset = voxelIndex * poolIndex.size();
    vector< double >::const_iterator i = values.begin() + offset;
    unsigned int proxyEndIndex = stoichPtr_->getNumVarPools() +
                                 stoichPtr_->getNumProxyPools();
    for ( vector< unsigned int >::const_iterator
            k = poolIndex.begin(); k != poolIndex.end(); ++k )
    {
        // if ( *k >= size() - numProxyPools )
        if ( *k >= stoichPtr_->getNumVarPools() && *k < proxyEndIndex )
        {
            double base = floor( *i );
            if ( rng_.uniform() > *i - base )
                varSinit()[*k] = (varS()[*k] += base );
            else
                varSinit()[*k] = (varS()[*k] += base + 1.0 );
            // varSinit()[*k] = (varS()[*k] += round( *i ));
        }
        i++;
    }
}
