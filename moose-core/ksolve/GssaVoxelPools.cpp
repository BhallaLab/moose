/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/global.h"
#include "../randnum/RNG.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "../basecode/SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"
#include "GssaSystem.h"
#include "GssaVoxelPools.h"

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


// Class definitions
GssaVoxelPools::GssaVoxelPools(): VoxelPoolsBase(), t_( 0.0 ), atot_( 0.0 )
{;}

GssaVoxelPools::~GssaVoxelPools()
{
    for ( size_t i = 0; i < rates_.size(); ++i )
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
    // useClockedUpdate flag, and we use the lower block here instead.
    /*
    const vector< unsigned int >& deps = g->dependentMathExpn[ rindex ];
    for( vector< unsigned int >::const_iterator
    		i = deps.begin(); i != deps.end(); ++i ) {
    		g->stoich->funcs( *i )->evalPool( varS(), time );
    }
    */
    /*
    unsigned int numFuncs = g->stoich->getNumFuncs();
    for( unsigned int i = 0; i < numFuncs; ++i )
    {
        g->stoich->funcs( i )->evalPool( varS(), time );
    }
    */
    // This function is equivalent to the loop above.
    g->stoich->updateFuncs( varS(), time );
}

void GssaVoxelPools::updateDependentRates(
    const vector< unsigned int >& deps, const Stoich* stoich )
{
    for ( auto i = deps.cbegin(); i != deps.end(); ++i )
    {
        atot_ -= fabs( v_[ *i ] );
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
    for ( auto i = v_.cbegin(); i != v_.end(); ++i )
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
    g->stoich->updateFuncs( varS(), t_ );
    updateReacVelocities( g, S(), v_ );
    atot_ = 0;
    for ( auto i = v_.cbegin(); i != v_.cend(); ++i )
        atot_ += fabs(*i);

    atot_ *= SAFETY_FACTOR;

    // Check if the system is in a stuck state. If so, terminate.
    if ( atot_ <= 0.0 )
        return false;
    return true;
}

/**
 * Recalculates the time for the next event. Used when we have a clocked
 * update of rates due to timed functions. In such cases the propensities
 * may change invisibly, so we need to update time estimates
 */
void GssaVoxelPools::recalcTime( const GssaSystem* g, double currTime )
{
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
            g->stoich->updateFuncs( varS(), t_ );
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
                g->stoich->updateFuncs( varS(), t_ );
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

        double sign = std::copysign( 1, v_[rindex] );

        g->transposeN.fireReac( rindex, Svec(), sign );
        numFire_[rindex]++;

        double r = rng_.uniform();
        while ( r <= 0.0 )
            r = rng_.uniform();

        t_ -= ( 1.0 / atot_ ) * log( r );
        g->stoich->updateFuncs( varS(), t_ );
        updateDependentRates( g->dependency[ rindex ], g->stoich );
    }
}

void GssaVoxelPools::reinit( const GssaSystem* g )
{
    rng_.setSeed( moose::getGlobalSeed() );
    VoxelPoolsBase::reinit(); // Assigns S = Sinit;
    unsigned int numVarPools = g->stoich->getNumVarPools();
    g->stoich->updateFuncs( varS(), 0 );

    double* n = varS();

    double error = 0.0;
    double _prev = 0.0;
    if( g->useRandInit )
    {
        map<double, vector<Eref>> groupByVal;
        for ( unsigned int i = 0; i < numVarPools; ++i )
        {
            _prev = n[i];
            n[i] = approximateWithInteger(_prev, rng_);
            error += (_prev - n[i]);
        }

        // Show warning to user if extra molecules in the system after
        // converting flots to integer is larger 1%.
        if( std::abs(error) >= 1.0 )
        {
            MOOSE_WARN( "Extra " << error
                        << " molecules in system after converting fractional to integer."
                      );
        }
    }
    else  // Just round to the nearest int.
    {
        for ( unsigned int i = 0; i < numVarPools; ++i )
            n[i] = std::round(n[i]);
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
                            getXreacScaleProducts(index - numCoreRates)
                        );
    else
        rates_[index] = rates[index]->copyWithVolScaling(getVolume(), 1.0, 1.0);
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
    return v;
}

void GssaVoxelPools::setStoich( const Stoich* stoichPtr )
{
    stoichPtr_ = stoichPtr;
}

// Handle volume updates. Inherited virtual func.
void GssaVoxelPools::setVolumeAndDependencies( double vol )
{
    VoxelPoolsBase::setVolumeAndDependencies( vol );
    updateAllRateTerms(stoichPtr_->getRateTerms(), stoichPtr_->getNumCoreRates());
}

// Handle cross compartment reactions
void GssaVoxelPools::xferIn( XferInfo& xf, unsigned int voxelIndex, const GssaSystem* g )
{
    unsigned int offset = voxelIndex * xf.xferPoolIdx.size();
    auto i = xf.values.cbegin() + offset;
    auto j = xf.lastValues.cbegin() + offset;
    auto m = xf.subzero.begin() + offset;
    double* s = varS();

    for ( auto k = xf.xferPoolIdx.cbegin(); k != xf.xferPoolIdx.end(); ++k )
    {
        double& x = s[*k];
        double dx = *i++ - *j++;
        // x += approximateWithInteger_debug(__FUNCTION__, dx, rng_);
        x += approximateWithInteger(dx, rng_);

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
        m++;
    }
    // If S has changed, then I should update rates of all affected reacs.
    // Go through stoich matrix to find affected reacs for each mol
    // Store in list, since some may be hit more than once in this func.
    // When done, go through and update all affected ones.

    // Does this fix the problem of negative concs?
    refreshAtot( g );
}
