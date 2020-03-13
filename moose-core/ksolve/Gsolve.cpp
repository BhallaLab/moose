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

#include "../mesh/VoxelJunction.h"
#include "../utility/print_function.hpp"
#include "../utility/utility.h"

#include "VoxelPoolsBase.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"

#include "RateTerm.h"
#include "FuncTerm.h"
#include "FuncRateTerm.h"
#include "../basecode/SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "GssaSystem.h"
#include "Stoich.h"
#include "GssaVoxelPools.h"
#include "Gsolve.h"

#include <chrono>
#include <algorithm>

#ifdef USE_BOOST_ASYNC
#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#endif

#include <future>
#include <atomic>
#include <thread>
#include <functional>


#define SIMPLE_ROUNDING 0

// When set use std::async rather than std::thread. This is slightly faster
// (roughly 3% with gcc7).
#define USING_ASYNC 1
#if USING_ASYNC
#define THREAD_LAUNCH_POLICY std::launch::async
#endif

const unsigned int OFFNODE = ~0;

const Cinfo* Gsolve::initCinfo()
{
    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////

    static ValueFinfo< Gsolve, Id > stoich (
        "stoich",
        "Stoichiometry object for handling this reaction system.",
        &Gsolve::setStoich,
        &Gsolve::getStoich
    );

    static ValueFinfo< Gsolve, Id > compartment (
        "compartment",
        "Compartment that contains this reaction system.",
        &Gsolve::setCompartment,
        &Gsolve::getCompartment
    );

    static ReadOnlyValueFinfo< Gsolve, unsigned int > numLocalVoxels(
        "numLocalVoxels",
        "Number of voxels in the core reac-diff system, on the "
        "current solver. ",
        &Gsolve::getNumLocalVoxels
    );

    static LookupValueFinfo<Gsolve, unsigned int, vector< double > > nVec(
        "nVec",
        "vector of pool counts",
        &Gsolve::setNvec,
        &Gsolve::getNvec
    );

    static ValueFinfo< Gsolve, unsigned int > numAllVoxels(
        "numAllVoxels",
        "Number of voxels in the entire reac-diff system, "
        "including proxy voxels to represent abutting compartments.",
        &Gsolve::setNumAllVoxels,
        &Gsolve::getNumAllVoxels
    );

    static ValueFinfo< Gsolve, unsigned int > numPools(
        "numPools",
        "Number of molecular pools in the entire reac-diff system, "
        "including variable, function and buffered.",
        &Gsolve::setNumPools,
        &Gsolve::getNumPools
    );

    static ValueFinfo< Gsolve, unsigned int > numThreads(
        "numThreads",
        "Number of threads to use in GSolve",
        &Gsolve::setNumThreads,
        &Gsolve::getNumThreads
    );

    static ValueFinfo< Gsolve, bool > useRandInit(
        "useRandInit",
        "Flag: True when using probabilistic (random) rounding.\n "
        "Default: True.\n "
        "When initializing the mol# from floating-point Sinit values, "
        "we have two options. One is to look at each Sinit, and round "
        "to the nearest integer. The other is to look at each Sinit, "
        "and probabilistically round up or down depending on the  "
        "value. For example, if we had a Sinit value of 1.49,  "
        "this would always be rounded to 1.0 if the flag is false, "
        "and would be rounded to 1.0 and 2.0 in the ratio 51:49 if "
        "the flag is true. ",
        &Gsolve::setRandInit,
        &Gsolve::getRandInit
    );

    static ValueFinfo< Gsolve, bool > useClockedUpdate(
        "useClockedUpdate",
        "Flag: True to cause all reaction propensities to be updated "
        "on every clock tick.\n"
        "Default: False.\n"
        "This flag should be set when the reaction system "
        "includes a function with a dependency on time or on external "
        "events. It has a significant speed penalty so the flag "
        "should not be set unless there are such functions. ",
        &Gsolve::setClockedUpdate,
        &Gsolve::getClockedUpdate
    );
    static ReadOnlyLookupValueFinfo<
    Gsolve, unsigned int, vector< unsigned int > > numFire(
        "numFire",
        "Vector of the number of times each reaction has fired."
        "Indexed by the voxel number."
        "Zeroed out at reinit.",
        &Gsolve::getNumFire
    );

    // DestFinfo definitions
    static DestFinfo process( "process",
            "Handles process call",
            new ProcOpFunc< Gsolve >( &Gsolve::process ) );
    static DestFinfo reinit( "reinit",
            "Handles reinit call",
            new ProcOpFunc< Gsolve >( &Gsolve::reinit ) );

    static DestFinfo voxelVol( "voxelVol",
            "Handles updates to all voxels. Comes from parent "
            "ChemCompt object.",
            new OpFunc1< Gsolve, vector< double > >(
                &Gsolve::updateVoxelVol )
            );

    static DestFinfo initProc( "initProc",
            "Handles initProc call from Clock",
            new ProcOpFunc< Gsolve >( &Gsolve::initProc ) );
    static DestFinfo initReinit( "initReinit",
            "Handles initReinit call from Clock",
            new ProcOpFunc< Gsolve >( &Gsolve::initReinit ) );

    // Shared definitions
    static Finfo* procShared[] =
    {
        &process, &reinit
    };

    static SharedFinfo proc( "proc",
            "Shared message for process and reinit",
            procShared, sizeof( procShared ) / sizeof( const Finfo* )
            );

    static Finfo* initShared[] =
    {
        &initProc, &initReinit
    };
    static SharedFinfo init( "init",
            "Shared message for initProc and initReinit. This is used"
            " when the system has cross-compartment reactions. ",
            initShared, sizeof( initShared ) / sizeof( const Finfo* )
            );

    ///////////////////////////////////////////////////////

    static Finfo* gsolveFinfos[] =
    {
        &stoich,           // Value
        &numLocalVoxels,   // ReadOnlyValue
        &nVec,             // LookupValue
        &numAllVoxels,     // ReadOnlyValue
        &numPools,         // Value
        &numThreads,       // Value
        &voxelVol,         // DestFinfo
        &proc,             // SharedFinfo
        &init,             // SharedFinfo
        // Here we put new fields that were not there in the Ksolve.
        &useRandInit,      // Value
        &useClockedUpdate, // Value
        &numFire,          // ReadOnlyLookupValue
    };

    static Dinfo< Gsolve > dinfo;

    static  Cinfo gsolveCinfo( "Gsolve",
            Neutral::initCinfo(),
            gsolveFinfos,
            sizeof(gsolveFinfos)/sizeof(Finfo *),
            &dinfo
            );

    return &gsolveCinfo;
}

static const Cinfo* gsolveCinfo = Gsolve::initCinfo();

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////

Gsolve::Gsolve() :
    numThreads_ ( 1 ),
    pools_( 1 ),
    startVoxel_( 0 ),
    dsolve_(),
    dsolvePtr_(nullptr),
    useClockedUpdate_( false )
{
    // Initialize with global seed.
    rng_.setSeed(moose::getGlobalSeed());
    numThreads_ = moose::getEnvInt("MOOSE_NUM_THREADS", 1);
}

Gsolve& Gsolve::operator=(const Gsolve& )
{
    return *this;
}

Gsolve::~Gsolve()
{
    ;
}

//////////////////////////////////////////////////////////////
// Field Access functions
//////////////////////////////////////////////////////////////

Id Gsolve::getStoich() const
{
    return stoich_;
}

void Gsolve::setCompartment( Id compt )
{
    if ( ( compt.element()->cinfo()->isA( "ChemCompt" ) ) )
    {
        compartment_ = compt;
        vector< double > vols = Field< vector< double > >::get( compt, "voxelVolume" );
        if ( vols.size() > 0 )
        {
            pools_.resize( vols.size() );
            for ( unsigned int i = 0; i < vols.size(); ++i )
            {
                pools_[i].setVolume( vols[i] );
            }
        }
    }
}

Id Gsolve::getCompartment() const
{
    return compartment_;
}

void Gsolve::setStoich( Id stoich )
{
    // This call is done _before_ setting the path on stoich
    assert( stoich.element()->cinfo()->isA( "Stoich" ) );
    stoich_ = stoich;
    stoichPtr_ = reinterpret_cast< Stoich* >( stoich.eref().data() );
    if ( stoichPtr_->getNumAllPools() == 0 )
    {
        stoichPtr_ = 0;
        return;
    }
    sys_.stoich = stoichPtr_;
    sys_.isReady = false;
    for ( unsigned int i = 0; i < pools_.size(); ++i )
        pools_[i].setStoich( stoichPtr_ );
}

unsigned int Gsolve::getNumLocalVoxels() const
{
    return pools_.size();
}

unsigned int Gsolve::getNumAllVoxels() const
{
    return pools_.size(); // Need to redo.
}

// If we're going to do this, should be done before the zombification.
void Gsolve::setNumAllVoxels( unsigned int numVoxels )
{
    if ( numVoxels == 0 )
    {
        return;
    }
    pools_.resize( numVoxels );
    sys_.isReady = false;
}

vector< double > Gsolve::getNvec( unsigned int voxel) const
{
    static vector< double > dummy;
    if ( voxel < pools_.size() )
    {
        return const_cast< GssaVoxelPools* >( &( pools_[ voxel ]) )->Svec();
    }
    return dummy;
}

void Gsolve::setNvec( unsigned int voxel, vector< double > nVec )
{
    if ( voxel < pools_.size() )
    {
        if ( nVec.size() != pools_[voxel].size() )
        {
            cout << "Warning: Gsolve::setNvec: size mismatch ( " <<
                 nVec.size() << ", " << pools_[voxel].size() << ")\n";
            return;
        }
        double* s = pools_[voxel].varS();
        for ( unsigned int i = 0; i < nVec.size(); ++i )
        {
            s[i] = std::round( nVec[i] );
            if ( s[i] < 0.0 )
                s[i] = 0.0;
        }
        if ( sys_.isReady )
            pools_[voxel].refreshAtot( &sys_ );
    }
}

vector< unsigned int > Gsolve::getNumFire( unsigned int voxel) const
{
    static vector< unsigned int > dummy;
    if ( voxel < pools_.size() )
    {
        return const_cast< GssaVoxelPools* >( &( pools_[ voxel ]) )->numFire();
    }
    return dummy;
}


bool Gsolve::getRandInit() const
{
    return sys_.useRandInit;
}

void Gsolve::setRandInit( bool val )
{
    sys_.useRandInit = val;
}

bool Gsolve::getClockedUpdate() const
{
    return useClockedUpdate_;
}

void Gsolve::setClockedUpdate( bool val )
{
    useClockedUpdate_ = val;
}


//////////////////////////////////////////////////////////////
// Process operations.
//////////////////////////////////////////////////////////////
void Gsolve::process( const Eref& e, ProcPtr p )
{
    // cout << stoichPtr_ << "    dsolve = " <<    dsolvePtr_ << endl;
    if ( !stoichPtr_ )
        return;

    // First, handle incoming diffusion values. Note potential for
    // issues with roundoff if diffusion is not integral.
    if ( dsolvePtr_ )
    {
        vector< double > dvalues( 4 );
        dvalues[0] = 0;
        dvalues[1] = getNumLocalVoxels();
        dvalues[2] = 0;
        dvalues[3] = stoichPtr_->getNumVarPools();
        dsolvePtr_->getBlock( dvalues );
        dsolvePtr_->setPrev();

        // Here we need to convert to integers, just in case. Normally
        // one would use a stochastic (integral) diffusion method with
        // the GSSA, but in mixed models it may be more complicated.
        vector< double >::iterator i = dvalues.begin() + 4;
        for ( ; i != dvalues.end(); ++i )
        {
#if 0
            *i = std::round( *i );
#else
            // *i = approximateWithInteger_debug(__FUNCTION__, *i, rng_);
            *i = approximateWithInteger(*i, rng_);
#endif
        }
        setBlock( dvalues );
    }

    if ( dsolvePtr_ )
    {
        for ( auto i = pools_.begin(); i != pools_.end(); ++i )
            i->refreshAtot( &sys_ );
    }

    if( 1 == numThreads_ || 1 == pools_.size())
    {
        if( numThreads_ > 1 )
        {
            cerr << "Warn: Not enough voxel. Reverting back to serial mode. " << endl;
            numThreads_ = 1;
        }

        for ( size_t i = 0; i < pools_.size(); i++ )
            pools_[i].advance( p, &sys_ );
    }
    else
    {
        /*-----------------------------------------------------------------------------
         *  Somewhat complicated computation to compute the number of threads. 1
         *  thread per (at least) voxel pool is ideal situation.
         *-----------------------------------------------------------------------------*/
#if USING_ASYNC
        vector<std::future<size_t>> vecFutures;
        for (size_t i = 0; i < numThreads_; i++) 
            vecFutures.push_back( 
                std::async( THREAD_LAUNCH_POLICY
                    , [this, i, p](){ 
                        return this->advance_chunk(i*this->grainSize_, (i+1)*this->grainSize_, p); 
                    })
                );
        // Block in same order
        size_t tot = 0;
        for (auto& fut : vecFutures) tot += fut.get();
        // We have processed all the pools.
        assert( tot >= pools_.size() );
#else
        vector<std::thread> vecThreads;
        for (size_t i = 0; i < numThreads_; i++)
        {
            // Use lambda. It is roughly 10% faster than std::bind and does not
            // involve copying data.
            vecThreads.push_back( 
                std::thread( 
                    [this, i, p](){ this->advance_chunk(i*this->grainSize_, (i+1)*this->grainSize_, p); }
                    )
                );
        }

        for( auto &v : vecThreads )
            v.join();
#endif

    }

    if ( useClockedUpdate_ )   // Check if a clocked stim is to be updated
    {
        if(numThreads_ == 1)
        {
            for ( auto &v : pools_ )
                v.recalcTime( &sys_, p->currTime );
        }
        else
        {
#if USING_ASYNC
        vector<std::future<size_t>> vecFutures;
        for (size_t i = 0; i < numThreads_; i++) 
            vecFutures.push_back( 
                std::async( THREAD_LAUNCH_POLICY
                    , [this, i, p](){ 
                        return this->recalcTimeChunk(i*this->grainSize_, (i+1)*this->grainSize_, p); 
                    })
                );
        // Block in same order
        size_t tot = 0;
        for (auto& fut : vecFutures) tot += fut.get();
        assert( tot >= pools_.size() ); // We have processed all the pools.
#else
            vector<std::thread> vecThreads;

            for (size_t i = 0; i < numThreads_; i++)
            {
                // Use lambda. It is roughly 10% faster than std::bind and does not
                // involve copying data.
                vecThreads.push_back( 
                        std::thread( 
                            [this, i, p](){ 
                                this->recalcTimeChunk(i*this->grainSize_, (i+1)*this->grainSize_, p); 
                            }
                        )
                    );
            }
            for( auto &v : vecThreads )
                v.join();
#endif

        }
    }

    // Finally, assemble and send the integrated values off for the Dsolve.
    if ( dsolvePtr_ )
    {
        vector< double > kvalues( 4 );
        kvalues[0] = 0;
        kvalues[1] = getNumLocalVoxels();
        kvalues[2] = 0;
        kvalues[3] = stoichPtr_->getNumVarPools();
        getBlock( kvalues );
        dsolvePtr_->setBlock( kvalues );

        // Now use the values in the Dsolve to update junction fluxes
        // for diffusion, channels, and xreacs
        dsolvePtr_->updateJunctions( p->dt );
        // Here the Gsolve may need to do something to convert to integers
    }
}

size_t Gsolve::recalcTimeChunk( const size_t begin, const size_t end, ProcPtr p)
{
    assert( begin >= std::min(pools_.size(), end));

    size_t tot = 0;
    for (size_t i = begin; i < std::min(pools_.size(), end); i++)  {
        tot += 1;
        pools_[i].recalcTime( &sys_, p->currTime );
    }
    return tot;
}

size_t Gsolve::advance_chunk( const size_t begin, const size_t end, ProcPtr p )
{
    assert( begin <= std::min(end, pools_.size()) );
    size_t tot = 0;
    for (size_t i = begin; i < std::min(end, pools_.size() ); i++)
    {
        pools_[i].advance( p, &sys_ );
        tot += 1;
    }
    return tot;
}

void Gsolve::reinit( const Eref& e, ProcPtr p )
{
    if ( !stoichPtr_ )
        return;

    if ( !sys_.isReady )
        rebuildGssaSystem();

    // First reinit concs.
    for (auto i = pools_.begin(); i != pools_.end(); ++i )
        i->reinit( &sys_ );

    // Second, update the atots.
    for ( auto i = pools_.begin(); i != pools_.end(); ++i )
        i->refreshAtot( &sys_ );


    // LoadBalancing. Recompute the optimal number of threads.
    size_t nvPools = pools_.size( );
    grainSize_ = (size_t) std::ceil((double)nvPools / (double)numThreads_);
    assert( grainSize_ * numThreads_ >= nvPools);
    numThreads_ = (size_t) std::ceil((double)nvPools / (double)grainSize_);
    MOOSE_DEBUG( "Grain size is " << grainSize_ << ". Num threads " << numThreads_);

    if(1 < numThreads_)
        cout << "Info: Setting up threaded gsolve with " << getNumThreads( )
             << " threads. " << endl;

}

//////////////////////////////////////////////////////////////
// init operations.
//////////////////////////////////////////////////////////////
void Gsolve::initProc( const Eref& e, ProcPtr p )
{;}

void Gsolve::initReinit( const Eref& e, ProcPtr p )
{
    if ( !stoichPtr_ )
        return;

    for( size_t i = 0 ; i < pools_.size(); ++i )
        pools_[i].reinit( &sys_ );
}
//////////////////////////////////////////////////////////////
// Solver setup
//////////////////////////////////////////////////////////////

void Gsolve::rebuildGssaSystem()
{
    stoichPtr_->convertRatesToStochasticForm();
    sys_.transposeN = stoichPtr_->getStoichiometryMatrix();
    sys_.transposeN.transpose();
    sys_.transposeN.truncateRow( stoichPtr_->getNumVarPools() + stoichPtr_->getNumProxyPools() );
    vector< vector< unsigned int > > & dep = sys_.dependency;
    dep.resize( stoichPtr_->getNumRates() );
    for ( unsigned int i = 0; i < stoichPtr_->getNumRates(); ++i )
    {
        sys_.transposeN.getGillespieDependence( i, dep[i] );
    }
    fillMmEnzDep();
    fillPoolFuncDep();
    fillIncrementFuncDep();
    makeReacDepsUnique();
    for ( vector< GssaVoxelPools >::iterator
            i = pools_.begin(); i != pools_.end(); ++i )
    {
        i->setNumReac( stoichPtr_->getNumRates() );
        i->updateAllRateTerms( stoichPtr_->getRateTerms(),
                               stoichPtr_->getNumCoreRates() );
    }
    sys_.isReady = true;
}

/**
 * Fill in dependency list for all MMEnzs on reactions.
 * The dependencies of MMenz products are already in the system,
 * so here we just need to add cases where any reaction product
 * is the Enz of an MMEnz.
 */
void Gsolve::fillMmEnzDep()
{
    unsigned int numRates = stoichPtr_->getNumRates();
    vector< unsigned int > indices;

    // Make a map to look up enzyme RateTerm using
    // the key of the enzyme molecule.
    map< unsigned int, unsigned int > enzMolMap;
    for ( unsigned int i = 0; i < numRates; ++i )
    {
        const MMEnzymeBase* mme = dynamic_cast< const MMEnzymeBase* >(
                                      stoichPtr_->rates( i ) );
        if ( mme )
        {
            vector< unsigned int > reactants;
            mme->getReactants( reactants );
            if ( reactants.size() > 1 )
                enzMolMap[ reactants.front() ] = i; // front is enzyme.
        }
    }

    // Use the map to fill in deps.
    for ( unsigned int i = 0; i < numRates; ++i )
    {
        // Extract the row of all molecules that depend on the reac.
        const int* entry;
        const unsigned int* colIndex;

        unsigned int numInRow =
            sys_.transposeN.getRow( i, &entry, &colIndex );
        for( unsigned int j = 0; j < numInRow; ++j )
        {
            map< unsigned int, unsigned int >::iterator pos =
                enzMolMap.find( colIndex[j] );
            if ( pos != enzMolMap.end() )
                sys_.dependency[i].push_back( pos->second );
        }
    }
}

/**
 * Here we fill in the dependencies involving poolFuncs. These are
 * the functions that evaluate an expression and assign directly to the
 * # of a target molecule.
 * There are two dependencies:
 * 1. When a reaction fires, all the Functions that depend on the reactants
 * must update their target molecule. This is in sys_.dependentMathExpn[].
 * 2. All the reactions that depend on the updated target molecules must
 * now recompute their propensity. This, along with other reac dependencies,
 * is in sys_.dependency[].
 */
void Gsolve::fillPoolFuncDep()
{
    // create map of funcs that depend on specified molecule.
    vector< vector< unsigned int > > funcMap(
        stoichPtr_->getNumAllPools() );
    unsigned int numFuncs = stoichPtr_->getNumFuncs();
    for ( unsigned int i = 0; i < numFuncs; ++i )
    {
        const FuncTerm *f = stoichPtr_->funcs( i );
        vector< unsigned int > molIndex = f->getReactantIndex();
        for ( unsigned int j = 0; j < molIndex.size(); ++j )
            funcMap[ molIndex[j] ].push_back( i );
    }
    // The output of each func is a mol indexed as
    // numVarMols + numBufMols + i
    unsigned int numRates = stoichPtr_->getNumRates();
    sys_.dependentMathExpn.resize( numRates );
    vector< unsigned int > indices;
    for ( unsigned int i = 0; i < numRates; ++i )
    {
        vector< unsigned int >& dep = sys_.dependentMathExpn[ i ];
        dep.resize( 0 );
        // Extract the row of all molecules that depend on the reac.
        const int* entry;
        const unsigned int* colIndex;
        unsigned int numInRow =
            sys_.transposeN.getRow( i, &entry, &colIndex );
        for ( unsigned int j = 0; j < numInRow; ++j )
        {
            unsigned int molIndex = colIndex[j];
            vector< unsigned int >& funcs = funcMap[ molIndex ];
            dep.insert( dep.end(), funcs.begin(), funcs.end() );
            for ( unsigned int k = 0; k < funcs.size(); ++k )
            {
                // unsigned int outputMol = funcs[k] + funcOffset;
                unsigned int outputMol = stoichPtr_->funcs( funcs[k] )->getTarget();
                // Insert reac deps here. Columns are reactions.
                vector< int > e; // Entries: we don't need.
                vector< unsigned int > c; // Column index: the reactions.
                stoichPtr_->getStoichiometryMatrix().
                getRow( outputMol, e, c );
                // Each of the reacs (col entries) depend on this func.
                vector< unsigned int > rdep = sys_.dependency[i];
                rdep.insert( rdep.end(), c.begin(), c.end() );
            }
        }
    }
}

/**
 * Here we fill in the dependencies involving incrementFuncs. These are
 * the functions that evaluate an expression that specifies rate of change
 * of # of a target molecule.
 * There are two dependencies:
 * 1. When a reaction fires, all the incrementFuncs that depend on the
 * reactants must update their rates. This is added to sys_.dependency[]
 * which is the usual handler for reac dependencies. Note that the inputs
 * to the incrementFuncs are NOT present in the stoichiometry matrix, so
 * this has to be done as a separate step.
 * 2. When the incrementFunc fires, then downstream reacs must update
 * their rates. This is handled by default with the regular dependency
 * calculations, since incrementFuncs are treated like regular reactions.
 */
void Gsolve::fillIncrementFuncDep()
{
    // create map of funcs that depend on specified molecule.
    vector< vector< unsigned int > > funcMap( stoichPtr_->getNumAllPools() );
    const vector< RateTerm* >& rates = stoichPtr_->getRateTerms();
    vector< FuncRate* > incrementRates;
    vector< unsigned int > incrementRateIndex;
    const vector< RateTerm* >::const_iterator q;
    for ( unsigned int i = 0; i < rates.size(); ++i )
    {
        FuncRate *term =
            dynamic_cast< FuncRate* >( rates[i] );
        if (term)
        {
            incrementRates.push_back( term );
            incrementRateIndex.push_back( i );
        }
    }

    for ( unsigned int k = 0; k < incrementRates.size(); ++k )
    {
        const vector< unsigned int >& molIndex =
            incrementRates[k]->getFuncArgIndex();
        for ( unsigned int j = 0; j < molIndex.size(); ++j )
            funcMap[ molIndex[j] ].push_back( incrementRateIndex[k] );
    }

    unsigned int numRates = stoichPtr_->getNumRates();
    sys_.dependentMathExpn.resize( numRates );
    vector< unsigned int > indices;
    for ( unsigned int i = 0; i < numRates; ++i )
    {
        // Algorithm:
        // 1.Go through stoich matrix finding all the poolIndices affected
        // by each Rate Term.
        // 2.Use funcMap to look up FuncRateTerms affected by these indices
        // 3. Add the rateTerm->FuncRateTerm mapping to the dependencies.

        const int* entry;
        const unsigned int* colIndex;
        unsigned int numInRow =
            sys_.transposeN.getRow( i, &entry, &colIndex );
        // 1.Go through stoich matrix finding all the poolIndices affected
        // by each Rate Term.
        for ( unsigned int j = 0; j < numInRow; ++j )
        {
            unsigned int molIndex = colIndex[j]; // Affected poolIndex

            // 2.Use funcMap to look up FuncRateTerms affected by these indices
            vector< unsigned int >& funcs = funcMap[ molIndex ];
            // 3. Add the rateTerm->FuncRateTerm mapping to the dependencies.
            vector< unsigned int >& rdep = sys_.dependency[i];
            rdep.insert( rdep.end(), funcs.begin(), funcs.end() );
        }
    }
}

/*
void Gsolve::fillMathDep()
{
    // create map of funcs that depend on specified molecule.
    vector< vector< unsigned int > > funcMap(
            stoichPtr_->getNumAllPools() );
    unsigned int numFuncs = stoichPtr_->getNumFuncs();
    for ( unsigned int i = 0; i < numFuncs; ++i ) {
        const FuncTerm *f = stoichPtr_->funcs( i );
        vector< unsigned int > molIndex = f->getReactantIndex();
        for ( unsigned int j = 0; j < molIndex.size(); ++j )
            funcMap[ molIndex[j] ].push_back( i );
    }
    // The output of each func is a mol indexed as
    // numVarMols + numBufMols + i
    unsigned int funcOffset =
            stoichPtr_->getNumVarPools() + stoichPtr_->getNumProxyPools() + stoichPtr_->getNumBufPools();
    unsigned int numRates = stoichPtr_->getNumRates();
    sys_.dependentMathExpn.resize( numRates );
    vector< unsigned int > indices;
    for ( unsigned int i = 0; i < numRates; ++i ) {
        vector< unsigned int >& dep = sys_.dependentMathExpn[ i ];
        dep.resize( 0 );
        // Extract the row of all molecules that depend on the reac.
        const int* entry;
        const unsigned int* colIndex;
        unsigned int numInRow =
                sys_.transposeN.getRow( i, &entry, &colIndex );
        for ( unsigned int j = 0; j < numInRow; ++j ) {
            unsigned int molIndex = colIndex[j];
            vector< unsigned int >& funcs = funcMap[ molIndex ];
            dep.insert( dep.end(), funcs.begin(), funcs.end() );
            for ( unsigned int k = 0; k < funcs.size(); ++k ) {
                unsigned int outputMol = funcs[k] + funcOffset;
                // Insert reac deps here. Columns are reactions.
                vector< int > e; // Entries: we don't need.
                vector< unsigned int > c; // Column index: the reactions.
                stoichPtr_->getStoichiometryMatrix().
                        getRow( outputMol, e, c );
                // Each of the reacs (col entries) depend on this func.
                vector< unsigned int > rdep = sys_.dependency[i];
                rdep.insert( rdep.end(), c.begin(), c.end() );
            }
        }
    }
}
*/

/**
 * Inserts reactions that depend on molecules modified by the
 * specified MathExpn, into the dependency list.
 * Later.
 */
void Gsolve::insertMathDepReacs( unsigned int mathDepIndex,
                                 unsigned int firedReac )
{
    /*
    unsigned int molIndex = sumTotals_[ mathDepIndex ].target( S_ );
    vector< unsigned int > reacIndices;

    // Extract the row of all reacs that depend on the target molecule
    if ( N_.getRowIndices( molIndex, reacIndices ) > 0 ) {
        vector< unsigned int >& dep = dependency_[ firedReac ];
        dep.insert( dep.end(), reacIndices.begin(), reacIndices.end() );
    }
    */
}

// Clean up dependency lists: Ensure only unique entries.
// Also a reac cannot depend on itself.
void Gsolve::makeReacDepsUnique()
{
    unsigned int numRates = stoichPtr_->getNumRates();
    for ( unsigned int i = 0; i < numRates; ++i )
    {
        vector< unsigned int >& dep = sys_.dependency[ i ];
        // Here we want to remove self-entries as well as duplicates.
        sort( dep.begin(), dep.end() );
        vector< unsigned int >::iterator k = dep.begin();

        /// STL stuff follows, with the usual weirdness.
        vector<unsigned int>::iterator pos = unique( dep.begin(), dep.end() );
        dep.resize( pos - dep.begin() );
    }
}

//////////////////////////////////////////////////////////////
// Solver ops
//////////////////////////////////////////////////////////////
unsigned int Gsolve::getPoolIndex( const Eref& e ) const
{
    return stoichPtr_->convertIdToPoolIndex( e.id() );
}

unsigned int Gsolve::getVoxelIndex( const Eref& e ) const
{
    unsigned int ret = e.dataIndex();
    if ( (ret < startVoxel_) || (ret >= startVoxel_ + pools_.size()))
        return OFFNODE;
    return ret - startVoxel_;
}

void Gsolve::setDsolve( Id dsolve )
{
    if ( dsolve == Id () )
    {
        dsolvePtr_ = 0;
        dsolve_ = Id();
    }
    else if ( dsolve.element()->cinfo()->isA( "Dsolve" ) )
    {
        dsolve_ = dsolve;
        dsolvePtr_ = reinterpret_cast<ZombiePoolInterface*>(dsolve.eref().data());
    }
    else
    {
        cout << "Warning: Gsolve::setDsolve: Object '" << dsolve.path() <<
             "' should be class Dsolve, is: " <<
             dsolve.element()->cinfo()->name() << endl;
    }
}


//////////////////////////////////////////////////////////////
// Zombie Pool Access functions
//////////////////////////////////////////////////////////////

void Gsolve::setN( const Eref& e, double v )
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
    {
        if ( e.element()->cinfo()->isA( "ZombieBufPool" ) )
        {
            // Do NOT round it here, it is folded into rate term.
            pools_[vox].setN( getPoolIndex( e ), v );
            // refresh rates because nInit controls ongoing value of n.
            if ( sys_.isReady )
                pools_[vox].refreshAtot( &sys_ );
        }
        else
        {
            pools_[vox].setN( getPoolIndex( e ), std::round( v ) );
        }
    }
}

double Gsolve::getN( const Eref& e ) const
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
        return pools_[vox].getN( getPoolIndex( e ) );
    return 0.0;
}

void Gsolve::setNinit( const Eref& e, double v )
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
    {
        if ( e.element()->cinfo()->isA( "ZombieBufPool" ) )
        {
            // Do NOT round it here, it is folded into rate term.
            pools_[vox].setNinit( getPoolIndex( e ), v );
            // refresh rates because nInit controls ongoing value of n.
            if ( sys_.isReady )
                pools_[vox].refreshAtot( &sys_ );
        }
        else
        {
            // I now do the rounding at reinit time. It is better there as
            // it can give a distinct value each cycle. It is also better
            // to keep the full resolution of Ninit for volume scaling.
            // pools_[vox].setNinit( getPoolIndex( e ), round( v ) );
            pools_[vox].setNinit( getPoolIndex( e ), v );
        }
    }
}

double Gsolve::getNinit( const Eref& e ) const
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
        return pools_[vox].getNinit( getPoolIndex( e ) );
    return 0.0;
}

void Gsolve::setDiffConst( const Eref& e, double v )
{
    ; // Do nothing.
}

double Gsolve::getDiffConst( const Eref& e ) const
{
    return 0;
}

void Gsolve::setNumPools( unsigned int numPoolSpecies )
{
    sys_.isReady = false;
    unsigned int numVoxels = pools_.size();
    for ( unsigned int i = 0 ; i < numVoxels; ++i )
    {
        pools_[i].resizeArrays( numPoolSpecies );
    }
}

void Gsolve::setNumVarTotPools( unsigned int var, unsigned int tot ) {
	setNumPools( tot );
}

unsigned int Gsolve::getNumPools() const
{
    if ( pools_.size() > 0 )
        return pools_[0].size();
    return 0;
}

void Gsolve::getBlock( vector< double >& values ) const
{
    unsigned int startVoxel = values[0];
    unsigned int numVoxels = values[1];
    unsigned int startPool = values[2];
    unsigned int numPools = values[3];

    assert( startVoxel >= startVoxel_ );
    assert( numVoxels <= pools_.size() );
    assert( pools_.size() > 0 );
    assert( numPools + startPool <= pools_[0].size() );
    values.resize( 4 + numVoxels * numPools );

    for ( unsigned int i = 0; i < numVoxels; ++i )
    {
        const double* v = pools_[ startVoxel + i ].S();
        for ( unsigned int j = 0; j < numPools; ++j )
        {
            values[ 4 + j * numVoxels + i]  = v[ j + startPool ];
        }
    }
}

void Gsolve::setBlock( const vector< double >& values )
{
    unsigned int startVoxel = values[0];
    unsigned int numVoxels = values[1];
    unsigned int startPool = values[2];
    unsigned int numPools = values[3];

    assert( startVoxel >= startVoxel_ );
    assert( numVoxels <= pools_.size() );
    assert( pools_.size() > 0 );
    assert( numPools + startPool <= pools_[0].size() );

    for ( unsigned int i = 0; i < numVoxels; ++i )
    {
        double* v = pools_[ startVoxel + i ].varS();
        for ( unsigned int j = 0; j < numPools; ++j )
        {
            v[ j + startPool ] = values[ 4 + j * numVoxels + i ];
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Gsolve::updateVoxelVol( vector< double > vols )
{
    // For now we assume identical numbers of voxels. Also assume
    // identical voxel junctions. But it should not be too hard to
    // update those too.
    if ( vols.size() == pools_.size() )
    {
        for ( unsigned int i = 0; i < vols.size(); ++i )
        {
            pools_[i].setVolumeAndDependencies( vols[i] );
        }
        updateRateTerms( ~0U );
    }
}

void Gsolve::updateRateTerms( unsigned int index )
{
    if ( index == ~0U )
    {
        // unsigned int numCrossRates = stoichPtr_->getNumRates() - stoichPtr_->getNumCoreRates();
        for ( unsigned int i = 0 ; i < pools_.size(); ++i )
        {
            pools_[i].updateAllRateTerms( stoichPtr_->getRateTerms(),
                    stoichPtr_->getNumCoreRates() );
        }
    }
    else if ( index < stoichPtr_->getNumRates() )
    {
        for ( unsigned int i = 0 ; i < pools_.size(); ++i )
            pools_[i].updateRateTerms( stoichPtr_->getRateTerms(),
                    stoichPtr_->getNumCoreRates(), index );
    }
}

//////////////////////////////////////////////////////////////////////////

VoxelPoolsBase* Gsolve::pools( unsigned int i )
{
    if ( pools_.size() > i )
        return &pools_[i];
    return 0;
}

double Gsolve::volume( unsigned int i ) const
{
    if ( pools_.size() > i )
        return pools_[i].getVolume();
    return 0.0;
}

unsigned int Gsolve::getNumThreads( ) const
{
    return numThreads_;
}

void Gsolve::setNumThreads( unsigned int x )
{
    numThreads_ = x;
}
