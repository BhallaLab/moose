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
#include "../utility/utility.h"

#ifdef USE_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#endif

#include "OdeSystem.h"
#include "VoxelPoolsBase.h"
#include "VoxelPools.h"
#include "../mesh/VoxelJunction.h"
#include "ZombiePoolInterface.h"

#include "RateTerm.h"
#include "../basecode/SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "Stoich.h"
#include "../shell/Shell.h"

#include "../mesh/MeshEntry.h"
#include "../mesh/Boundary.h"
#include "../mesh/ChemCompt.h"
#include "Ksolve.h"

#include <chrono>
#include <algorithm>

#include <future>
#include <functional>
#include <atomic>
#include <thread>

using namespace std::chrono;

const unsigned int OFFNODE = ~0;

const Cinfo* Ksolve::initCinfo()
{
    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////
    static ValueFinfo< Ksolve, string > method (
        "method",
        "Integration method, using GSL. So far only explict. Options are:"
        "rk5: The default Runge-Kutta-Fehlberg 5th order adaptive dt method"
        "gsl: alias for the above"
        "rk4: The Runge-Kutta 4th order fixed dt method"
        "rk2: The Runge-Kutta 2,3 embedded fixed dt method"
        "rkck: The Runge-Kutta Cash-Karp (4,5) method"
        "rk8: The Runge-Kutta Prince-Dormand (8,9) method"
        "lsoda: LSODA method",
        &Ksolve::setMethod,
        &Ksolve::getMethod
    );

    static ValueFinfo< Ksolve, double > epsAbs (
        "epsAbs",
        "Absolute permissible integration error range.",
        &Ksolve::setEpsAbs,
        &Ksolve::getEpsAbs
    );

    static ValueFinfo< Ksolve, double > epsRel (
        "epsRel",
        "Relative permissible integration error range.",
        &Ksolve::setEpsRel,
        &Ksolve::getEpsRel
    );


    static ValueFinfo< Ksolve, Id > compartment(
        "compartment",
        "Compartment in which the Ksolve reaction system lives.",
        &Ksolve::setCompartment,
        &Ksolve::getCompartment
    );

    static ReadOnlyValueFinfo< Ksolve, unsigned int > numLocalVoxels(
        "numLocalVoxels",
        "Number of voxels in the core reac-diff system, on the "
        "current solver. ",
        &Ksolve::getNumLocalVoxels
    );

    static LookupValueFinfo< Ksolve, unsigned int, vector< double > > nVec(
        "nVec",
        "vector of pool counts. Index specifies which voxel.",
        &Ksolve::setNvec,
        &Ksolve::getNvec
    );

    static ValueFinfo< Ksolve, unsigned int > numAllVoxels(
        "numAllVoxels",
        "Number of voxels in the entire reac-diff system, "
        "including proxy voxels to represent abutting compartments.",
        &Ksolve::setNumAllVoxels,
        &Ksolve::getNumAllVoxels
    );

    static ValueFinfo< Ksolve, unsigned int > numThreads (
        "numThreads",
        "Number of threads to use",
        &Ksolve::setNumThreads,
        &Ksolve::getNumThreads
    );

    static ValueFinfo< Ksolve, unsigned int > numPools(
        "numPools",
        "Number of molecular pools in the entire reac-diff system, "
        "including variable, function and buffered.",
        &Ksolve::setNumPools,
        &Ksolve::getNumPools
    );

    static ReadOnlyValueFinfo< Ksolve, double > estimatedDt(
        "estimatedDt",
        "Estimated timestep for reac system based on Euler error",
        &Ksolve::getEstimatedDt
    );

    static ReadOnlyValueFinfo< Ksolve, Id > stoich(
        "stoich",
        "Id for stoichiometry object tied to this Ksolve",
        &Ksolve::getStoich
    );


    // DestFinfo definitions
    static DestFinfo process( "process",
            "Handles process call from Clock",
            new ProcOpFunc< Ksolve >( &Ksolve::process )
            );

    static DestFinfo reinit( "reinit",
            "Handles reinit call from Clock",
            new ProcOpFunc< Ksolve >( &Ksolve::reinit )
            );

    static DestFinfo initProc( "initProc",
            "Handles initProc call from Clock",
            new ProcOpFunc< Ksolve >( &Ksolve::initProc )
            );

    static DestFinfo initReinit( "initReinit",
            "Handles initReinit call from Clock",
            new ProcOpFunc< Ksolve >( &Ksolve::initReinit )
            );

    static DestFinfo voxelVol( "voxelVol",
            "Handles updates to all voxels. Comes from parent "
            "ChemCompt object.",
            new OpFunc1< Ksolve, vector< double > >(
                &Ksolve::updateVoxelVol )
            );

    ///////////////////////////////////////////////////////
    // Shared definitions
    ///////////////////////////////////////////////////////
    static Finfo* procShared[] =
    {
        &process, &reinit
    };

    static SharedFinfo proc( "proc",
            "Shared message for process and reinit. These are used for "
            "all regular Ksolve calculations including interfacing with "
            "the diffusion calculations by a Dsolve.",
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

    static Finfo* ksolveFinfos[] =
    {
        &method,                         // Value
        &epsAbs,                         // Value
        &epsRel ,                        // Value
        &numThreads,                     // Value
        &compartment,                    // Value
        &numLocalVoxels,                 // ReadOnlyValue
        &nVec,                           // LookupValue
        &numAllVoxels,                   // ReadOnlyValue
        &numPools,                       // Value
        &estimatedDt,                    // ReadOnlyValue
        &stoich,                         // ReadOnlyValue
        &voxelVol,                       // DestFinfo
        &proc,                           // SharedFinfo
        &init,                           // SharedFinfo
    };

    static Dinfo< Ksolve > dinfo;
    static  Cinfo ksolveCinfo(
        "Ksolve",
        Neutral::initCinfo(),
        ksolveFinfos,
        sizeof(ksolveFinfos)/sizeof(Finfo *),
        &dinfo
    );

    return &ksolveCinfo;
}

static const Cinfo* ksolveCinfo = Ksolve::initCinfo();

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////

Ksolve::Ksolve()
    :
    method_( "rk5" ),
    epsAbs_( 1e-7 ),
    epsRel_( 1e-7 ),
    numThreads_( 1 ),
    pools_( 1 ),
    startVoxel_( 0 ),
    dsolve_(),
    dsolvePtr_( nullptr )
{
    numThreads_ = moose::getEnvInt("MOOSE_NUM_THREADS", 1);

}

Ksolve::~Ksolve()
{
    ;
}

//////////////////////////////////////////////////////////////
// Field Access functions
//////////////////////////////////////////////////////////////

string Ksolve::getMethod() const
{
    return method_;
}

void Ksolve::setMethod( string method )
{
    std::transform(method.begin(), method.end(), method.begin(), ::tolower);
    // If user is trying to set ksolve method after ksolve has been initialized,
    // show a warning.
    if( isBuilt_ )
    {
        moose::showWarn(
            "You are trying to set Ksolve::method after moose::Stoich has been "
            " initialized. This will be ignored. Please do before ksolve is assigned to "
            " moose::Stoich."
            );
        return;
    }

#if USE_GSL
    if ( method == "rk5" || method == "gsl" )
    {
        method_ = "rk5";
    }
    else if ( method == "rk4"  || method == "rk2" ||
              method == "rk8" || method == "rkck" || method == "lsoda" )
    {
        method_ = method;
    }
    else
    {
        cout << "Warning: Ksolve::setMethod: '" << method <<
             "' is not known, using default rk5\n";
        method_ = "rk5";
    }
#elif USE_BOOST_ODE
    // TODO: Check for boost related methods.
    method_ = method;
#endif
}

double Ksolve::getEpsAbs() const
{
    return epsAbs_;
}

void Ksolve::setEpsAbs( double epsAbs )
{
    if ( epsAbs < 0 )
        epsAbs_ = 1.0e-4;
    else
        epsAbs_ = epsAbs;
}


double Ksolve::getEpsRel() const
{
    return epsRel_;
}

void Ksolve::setEpsRel( double epsRel )
{
    if ( epsRel < 0 )
    {
        epsRel_ = 1.0e-6;
    }
    else
    {
        epsRel_ = epsRel;
    }
}

void Ksolve::setNumThreads( unsigned int x )
{
    numThreads_ = x;
}

unsigned int Ksolve::getNumThreads(  ) const
{
    return numThreads_;
}

Id Ksolve::getStoich() const
{
    return stoich_;
}

#ifdef USE_GSL
void innerSetMethod( OdeSystem& ode, const string& method )
{
    ode.method = method;
    if ( method == "rk5" )
    {
        ode.gslStep = gsl_odeiv2_step_rkf45;
    }
    else if ( method == "rk4" )
    {
        ode.gslStep = gsl_odeiv2_step_rk4;
    }
    else if ( method == "rk2" )
    {
        ode.gslStep = gsl_odeiv2_step_rk2;
    }
    else if ( method == "rkck" )
    {
        ode.gslStep = gsl_odeiv2_step_rkck;
    }
    else if ( method == "rk8" )
    {
        ode.gslStep = gsl_odeiv2_step_rk8pd;
    }
    else
    {
        ode.gslStep = gsl_odeiv2_step_rkf45;
    }
}
#endif

void Ksolve::setStoich( Id stoich )
{
    assert( stoich.element()->cinfo()->isA( "Stoich" ) );
    stoich_ = stoich;
    stoichPtr_ = reinterpret_cast< Stoich* >( stoich.eref().data() );

    if ( !isBuilt_ )
    {
        OdeSystem ode;
        ode.epsAbs = epsAbs_;
        ode.epsRel = epsRel_;
        // ode.initStepSize = getEstimatedDt();
        ode.initStepSize = 0.01; // This will be overridden at reinit.
        ode.method = method_;

#ifdef USE_GSL
        ode.gslSys.dimension = stoichPtr_->getNumAllPools();
        if ( ode.gslSys.dimension == 0 )
        {
            stoichPtr_ = 0;
            return; // No pools, so don't bother.
        }
        innerSetMethod( ode, method_ );
        ode.gslSys.function = &VoxelPools::gslFunc;
        ode.gslSys.jacobian = 0;
        innerSetMethod( ode, method_ );
        unsigned int numVoxels = pools_.size();
        for ( unsigned int i = 0 ; i < numVoxels; ++i )
        {
            ode.gslSys.params = &pools_[i];
            pools_[i].setStoich( stoichPtr_, &ode );
            // pools_[i].setIntDt( ode.initStepSize ); // We're setting it up anyway
        }
#elif USE_BOOST_ODE
        ode.dimension = stoichPtr_->getNumAllPools();
        if ( ode.dimension == 0 )
            return; // No pools, so don't bother.
        unsigned int numVoxels = pools_.size();
        for ( unsigned int i = 0 ; i < numVoxels; ++i )
        {
            pools_[i].setStoich( stoichPtr_, &ode );
        }
#endif
        isBuilt_ = true;
    }

}

Id Ksolve::getDsolve() const
{
    return dsolve_;
}

void Ksolve::setDsolve( Id dsolve )
{
    if ( dsolve == Id () )
    {
        dsolvePtr_ = nullptr;
        dsolve_ = Id();
    }
    else if ( dsolve.element()->cinfo()->isA( "Dsolve" ) )
    {
        dsolve_ = dsolve;
        dsolvePtr_ = reinterpret_cast<ZombiePoolInterface*>(dsolve.eref().data());
    }
    else
    {
        cout << "Warning: Ksolve::setDsolve: Object '" << dsolve.path() 
            << "' should be class Dsolve, is: " 
            << dsolve.element()->cinfo()->name() << endl;
    }
}

unsigned int Ksolve::getNumLocalVoxels() const
{
    return pools_.size();
}

unsigned int Ksolve::getNumAllVoxels() const
{
    return pools_.size(); // Need to redo.
}

// If we're going to do this, should be done before the zombification.
void Ksolve::setNumAllVoxels( unsigned int numVoxels )
{
    if ( numVoxels == 0 )
    {
        return;
    }
    pools_.resize( numVoxels );
}

vector< double > Ksolve::getNvec( unsigned int voxel) const
{
    static vector< double > dummy;
    if ( voxel < pools_.size() )
    {
        return const_cast< VoxelPools* >( &( pools_[ voxel ] ) )->Svec();
    }
    return dummy;
}

void Ksolve::setNvec( unsigned int voxel, vector< double > nVec )
{
    if ( voxel < pools_.size() )
    {
        if ( nVec.size() != pools_[voxel].size() )
        {
            cout << "Warning: Ksolve::setNvec: size mismatch ( " <<
                 nVec.size() << ", " << pools_[voxel].size() << ")\n";
            return;
        }
        double* s = pools_[voxel].varS();
        for ( unsigned int i = 0; i < nVec.size(); ++i )
            s[i] = nVec[i];
    }
}


double Ksolve::getEstimatedDt() const
{
    static const double EPSILON = 1e-15;
    vector< double > s( stoichPtr_->getNumAllPools(), 1.0 );
    vector< double > v( stoichPtr_->getNumRates(), 0.0 );
    double maxVel = 0.0;
    if ( pools_.size() > 0.0 )
    {
        pools_[0].updateReacVelocities( &s[0], v );
        for ( vector< double >::iterator
                i = v.begin(); i != v.end(); ++i )
            if ( maxVel < *i )
                maxVel = *i;
    }
    if ( maxVel < EPSILON )
        return 0.1; // Based on typical sig pathway reac rates.
    // Heuristic: the largest velocity times dt should be 10% of mol conc.
    return 0.1 / maxVel;
}

//////////////////////////////////////////////////////////////
// Process operations.
//////////////////////////////////////////////////////////////
void Ksolve::process( const Eref& e, ProcPtr p )
{
    if ( isBuilt_ == false )
        return;

    //t0_ = high_resolution_clock::now();

    // First, handle incoming diffusion values, update S with those.
    if ( dsolvePtr_ )
    {
        vector< double > dvalues( 4 );
        dvalues[0] = 0;
        dvalues[1] = getNumLocalVoxels();
        dvalues[2] = 0;
        dvalues[3] = stoichPtr_->getNumVarPools();

        dsolvePtr_->getBlock( dvalues );
        // Second, set the prev_ value in DiffPoolVec
        dsolvePtr_->setPrev();
        setBlock( dvalues );
    }

    if( 1 == numThreads_ || 1 == pools_.size() )
    {
        if( numThreads_ > 1 )
        {
            cerr << "Warn: Not enough voxels for multithreading. " 
                << "Reverting to serial mode. " << endl;
            numThreads_ = 1;
        }

        for ( unsigned int i = 0; i < pools_.size(); i++ )
            pools_[i].advance( p );
    }
    else
    {
        std::vector<std::future<size_t>> vecFutures;

        // lambdas is faster than std::bind
        for (auto interval : intervals_)
        {
            vecFutures.push_back( 
                    std::async( std::launch::async
                        , &Ksolve::advance_chunk
                        , this
                        , interval.first
                        , interval.second, p 
                        )
                    );
        }
        size_t tot = 0;
        for (auto &v : vecFutures )
            tot += v.get();
        assert(tot == pools_.size());
    }

    // Assemble and send the integrated values off for the Dsolve.
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
    }

    //t1_ = high_resolution_clock::now();
    //moose::addSolverProf( "Ksolve", duration_cast<duration<double>> (t1_ - t0_ ).count(), 1 );
}

void Ksolve::advance_pool( const size_t i, ProcPtr p )
{
    pools_[i].advance(p);
}

size_t Ksolve::advance_chunk( const size_t begin, const size_t end, ProcPtr p )
{
    size_t tot = 0;
    for (size_t i = begin; i < std::min(end, pools_.size()); i++)
    {
        pools_[i].advance( p );
        tot += 1;
    }
    return tot;
}


void Ksolve::reinit( const Eref& e, ProcPtr p )
{
    if ( !stoichPtr_ )
        return;

    if ( isBuilt_ )
    {
        for ( unsigned int i = 0 ; i < pools_.size(); ++i )
            pools_[i].reinit( p->dt );
    }
    else
    {
        cout << "Warning:Ksolve::reinit: Reaction system not initialized\n";
        return;
    }

    if(numThreads_ > pools_.size())
        numThreads_ = pools_.size();

    if(numThreads_ > 1)
        cout << "Info: Multi-threaded Ksolve (" << numThreads_ << " threads)."
            << endl;

    // Recompute the partition of interval.
    intervals_.clear();
    moose::splitIntervalInNParts(pools_.size(), numThreads_, intervals_);
}

//////////////////////////////////////////////////////////////
// init operations.
//////////////////////////////////////////////////////////////
void Ksolve::initProc( const Eref& e, ProcPtr p )
{
}

void Ksolve::initReinit( const Eref& e, ProcPtr p )
{
    for ( unsigned int i = 0 ; i < pools_.size(); ++i )
        pools_[i].reinit( p->dt );
}

/**
 * updateRateTerms obtains the latest parameters for the rates_ vector,
 * and has each of the pools update its parameters including rescaling
 * for volumes.
 */
void Ksolve::updateRateTerms( unsigned int index )
{
    if ( index == ~0U )
    {
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


//////////////////////////////////////////////////////////////
// Solver ops
//////////////////////////////////////////////////////////////

unsigned int Ksolve::getPoolIndex( const Eref& e ) const
{
    return stoichPtr_->convertIdToPoolIndex( e.id() );
}

unsigned int Ksolve::getVoxelIndex( const Eref& e ) const
{
    unsigned int ret = e.dataIndex();
    if ( ret < startVoxel_  || ret >= startVoxel_ + pools_.size() )
        return OFFNODE;
    return ret - startVoxel_;
}

//////////////////////////////////////////////////////////////
// Zombie Pool Access functions
//////////////////////////////////////////////////////////////

void Ksolve::setN( const Eref& e, double v )
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
        pools_[vox].setN( getPoolIndex( e ), v );
}

double Ksolve::getN( const Eref& e ) const
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
        return pools_[vox].getN( getPoolIndex( e ) );
    return 0.0;
}

void Ksolve::setNinit( const Eref& e, double v )
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
        pools_[vox].setNinit( getPoolIndex( e ), v );
}

double Ksolve::getNinit( const Eref& e ) const
{
    unsigned int vox = getVoxelIndex( e );
    if ( vox != OFFNODE )
        return pools_[vox].getNinit( getPoolIndex( e ) );
    return 0.0;
}

void Ksolve::setDiffConst( const Eref& e, double v )
{
    ; // Do nothing.
}

double Ksolve::getDiffConst( const Eref& e ) const
{
    return 0;
}

void Ksolve::setNumPools( unsigned int numPoolSpecies )
{
    unsigned int numVoxels = pools_.size();
    for ( unsigned int i = 0 ; i < numVoxels; ++i )
    {
        pools_[i].resizeArrays( numPoolSpecies );
    }
}

void Ksolve::setNumVarTotPools( unsigned int var, unsigned int tot )
{
    unsigned int numVoxels = pools_.size();
    for ( unsigned int i = 0 ; i < numVoxels; ++i )
    {
        pools_[i].resizeArrays( tot );
    }
}

unsigned int Ksolve::getNumPools() const
{
    if ( pools_.size() > 0 )
        return pools_[0].size();
    return 0;
}

VoxelPoolsBase* Ksolve::pools( unsigned int i )
{
    if ( pools_.size() > i )
        return &pools_[i];
    return 0;
}

double Ksolve::volume( unsigned int i ) const
{
    if ( pools_.size() > i )
        return pools_[i].getVolume();
    return 0.0;
}

void Ksolve::getBlock( vector< double >& values ) const
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

void Ksolve::setBlock( const vector< double >& values )
{
    unsigned int startVoxel = values[0];
    unsigned int numVoxels = values[1];
    unsigned int startPool = values[2];
    unsigned int numPools = values[3];

    assert( startVoxel >= startVoxel_ );
    assert( numVoxels <= pools_.size() );
    assert( pools_.size() > 0 );
    assert( numPools + startPool <= pools_[0].size() );
    assert( values.size() == 4 + numVoxels * numPools );

    for ( unsigned int i = 0; i < numVoxels; ++i )
    {
        double* v = pools_[ startVoxel + i ].varS();
        for ( unsigned int j = 0; j < numPools; ++j )
        {
            v[ j + startPool ] = values[ 4 + j * numVoxels + i ];
        }
    }
}

void Ksolve::updateVoxelVol( vector< double > vols )
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

// cross-compartment reaction stuff.
// Functions for setup of cross-compartment transfer.
void Ksolve::print() const
{
    cout << "path = " << stoichPtr_->getKsolve().path() <<
         ", numPools = " << pools_.size() << "\n";
    for ( unsigned int i = 0; i < pools_.size(); ++i )
    {
        cout << "pools[" << i << "] contents = ";
        pools_[i].print();
    }
    cout << "method = " << method_ << ", stoich=" << stoich_.path() <<endl;
    cout << "dsolve = " << dsolve_.path() << endl;
    cout << "compartment = " << compartment_.path() << endl;
}

