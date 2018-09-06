/***
 *       Filename:  SteadyStateBoost.cpp
 *
 *    Description:  Steady state solver. Copy of ../ksolve/SteadyStateGsl.cpp
 *    but it uses boost solver.
 *
 *        Created:  2016-05-10
 *
 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *
 * This program works out a steady-state value for a reaction system.
 * It uses boost-ublas and lapack heavily.
 *
 * It finds the ss value closest to the initial conditions.
 *
 * If you want to find multiple stable states, it is best to do this
 * in Python as it gives a lot of flexibility in working out how to
 * find steady states.
 *
 * Likewise, if you want to carry out a dose-response calculation.
 */


#include "header.h"
#include "global.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"
#include "../randnum/RNG.h"

/* Root finding algorithm is implemented here */
#include "NonlinearSystem.h"

/*
 * Bindings to lapack. These headers are not part of standard boost
 * distribution. They are available with moose distribution. See 'external'
 * folder.
 */
#include "boost/numeric/bindings/lapack/lapack.hpp"
#include "boost/numeric/bindings/lapack/geev.hpp"

#include "VoxelPoolsBase.h"
#include "OdeSystem.h"
#include "VoxelPools.h"
#include "SteadyStateBoost.h"

using namespace boost::numeric::bindings;
using namespace boost::numeric;

void ss_func( const vector_type& x, void* params, vector_type& f );
unsigned int rankUsingBoost( ublas::matrix<double>& U );

// Limit below which small numbers are treated as zero.
const double SteadyState::EPSILON = 1e-9;

// This fraction of molecules is used as an increment in computing the
// Jacobian
const double SteadyState::DELTA = 1e-6;
/**
 * This is used by the multidimensional root finder
 */
struct reac_info
{

    int rank;
    int num_reacs;
    size_t num_mols;
    int nIter;
    double convergenceCriterion;

    double* T;
    VoxelPools* pool;
    vector< double > nVec;

    ublas::matrix< double >* Nr;
    ublas::matrix< double >* gamma;
};

const Cinfo* SteadyState::initCinfo()
{
    /**
     * This picks up the entire Stoich data structure
    static Finfo* gslShared[] =
    {
    	new SrcFinfo( "reinitSrc", Ftype0() ),
    	new DestFinfo( "assignStoich",
    		Ftype1< void* >(),
    		RFCAST( &SteadyState::assignStoichFunc )
    		),
    	new DestFinfo( "setMolN",
    		Ftype2< double, unsigned int >(),
    		RFCAST( &SteadyState::setMolN )
    		),
    	new SrcFinfo( "requestYsrc", Ftype0() ),
    	new DestFinfo( "assignY",
    		Ftype1< double* >(),
    		RFCAST( &SteadyState::assignY )
    		),
    };
     */

    /**
     * These are the fields of the SteadyState class
     */
    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////
    static ValueFinfo< SteadyState, Id > stoich(
        "stoich",
        "Specify the Id of the stoichiometry system to use",
        &SteadyState::setStoich,
        &SteadyState::getStoich
    );
    static ReadOnlyValueFinfo< SteadyState, bool > badStoichiometry(
        "badStoichiometry",
        "Bool: True if there is a problem with the stoichiometry",
        &SteadyState::badStoichiometry
    );
    static ReadOnlyValueFinfo< SteadyState, bool > isInitialized(
        "isInitialized",
        "True if the model has been initialized successfully",
        &SteadyState::isInitialized
    );
    static ReadOnlyValueFinfo< SteadyState, unsigned int > nIter(
        "nIter",
        "Number of iterations done by steady state solver",
        &SteadyState::getNiter
    );
    static ReadOnlyValueFinfo< SteadyState, string > status(
        "status",
        "Status of solver",
        &SteadyState::getStatus
    );
    static ValueFinfo< SteadyState, unsigned int > maxIter(
        "maxIter",
        "Max permissible number of iterations to try before giving up",
        &SteadyState::setMaxIter,
        &SteadyState::getMaxIter
    );
    static ValueFinfo< SteadyState, double> convergenceCriterion(
        "convergenceCriterion",
        "Fractional accuracy required to accept convergence",
        &SteadyState::setConvergenceCriterion,
        &SteadyState::getConvergenceCriterion
    );
    static ReadOnlyValueFinfo< SteadyState, unsigned int > numVarPools(
        "numVarPools",
        "Number of variable molecules in reaction system.",
        &SteadyState::getNumVarPools
    );
    static ReadOnlyValueFinfo< SteadyState, unsigned int > rank(
        "rank",
        "Number of independent molecules in reaction system",
        &SteadyState::getRank
    );
    static ReadOnlyValueFinfo< SteadyState, unsigned int > stateType(
        "stateType",
        "0: stable; 1: unstable; 2: saddle; 3: osc?; 4: one near-zero eigenvalue; 5: other",
        &SteadyState::getStateType
    );
    static ReadOnlyValueFinfo< SteadyState, unsigned int >
    nNegEigenvalues (
        "nNegEigenvalues",
        "Number of negative eigenvalues: indicates type of solution",
        &SteadyState::getNnegEigenvalues
    );
    static ReadOnlyValueFinfo< SteadyState, unsigned int >
    nPosEigenvalues(
        "nPosEigenvalues",
        "Number of positive eigenvalues: indicates type of solution",
        &SteadyState::getNposEigenvalues
    );
    static ReadOnlyValueFinfo< SteadyState, unsigned int > solutionStatus(
        "solutionStatus",
        "0: Good; 1: Failed to find steady states; "
        "2: Failed to find eigenvalues",
        &SteadyState::getSolutionStatus
    );
    static LookupValueFinfo< SteadyState, unsigned int, double > total(
        "total",
        "Totals table for conservation laws. The exact mapping of"
        "this to various sums of molecules is given by the "
        "conservation matrix, and is currently a bit opaque."
        "The value of 'total' is set to initial conditions when"
        "the 'SteadyState::settle' function is called."
        "Assigning values to the total is a special operation:"
        "it rescales the concentrations of all the affected"
        "molecules so that they are at the specified total."
        "This happens the next time 'settle' is called.",
        &SteadyState::setTotal,
        &SteadyState::getTotal
    );
    static ReadOnlyLookupValueFinfo<
    SteadyState, unsigned int, double > eigenvalues(
        "eigenvalues",
        "Eigenvalues computed for steady state",
        &SteadyState::getEigenvalue
    );
    ///////////////////////////////////////////////////////
    // MsgDest definitions
    ///////////////////////////////////////////////////////
    static DestFinfo setupMatrix( "setupMatrix",
                                  "This function initializes and rebuilds the matrices used "
                                  "in the calculation.",
                                  new OpFunc0< SteadyState >(&SteadyState::setupMatrix)
                                );

    static DestFinfo settle( "settle",
                             "Finds the nearest steady state to the current initial "
                             "conditions. This function rebuilds the entire calculation "
                             "only if the object has not yet been initialized.",
                             new OpFunc0< SteadyState >( &SteadyState::settleFunc )
                           );
    static DestFinfo resettle( "resettle",
                               "Finds the nearest steady state to the current initial "
                               "conditions. This function rebuilds the entire calculation ",
                               new OpFunc0< SteadyState >( &SteadyState::resettleFunc )
                             );
    static DestFinfo showMatrices( "showMatrices",
                                   "Utility function to show the matrices derived for the calculations on the reaction system. Shows the Nr, gamma, and total matrices",
                                   new OpFunc0< SteadyState >( &SteadyState::showMatrices )
                                 );
    static DestFinfo randomInit( "randomInit",
                                 "Generate random initial conditions consistent with the mass"
                                 "conservation rules. Typically invoked in order to scan"
                                 "states",
                                 new EpFunc0< SteadyState >(
                                     &SteadyState::randomizeInitialCondition )
                               );
    ///////////////////////////////////////////////////////
    // Shared definitions
    ///////////////////////////////////////////////////////

    static Finfo * steadyStateFinfos[] =
    {
        &stoich,				// Value
        &badStoichiometry,		// ReadOnlyValue
        &isInitialized,			// ReadOnlyValue
        &nIter,					// ReadOnlyValue
        &status,				// ReadOnlyValue
        &maxIter,				// Value
        &convergenceCriterion,	// ReadOnlyValue
        &numVarPools,			// ReadOnlyValue
        &rank,					// ReadOnlyValue
        &stateType,				// ReadOnlyValue
        &nNegEigenvalues,		// ReadOnlyValue
        &nPosEigenvalues,		// ReadOnlyValue
        &solutionStatus,		// ReadOnlyValue
        &total,					// LookupValue
        &eigenvalues,			// ReadOnlyLookupValue
        &setupMatrix,			// DestFinfo
        &settle,				// DestFinfo
        &resettle,				// DestFinfo
        &showMatrices,			// DestFinfo
        &randomInit,			// DestFinfo


    };

    static string doc[] =
    {
        "Name", "SteadyState",
        "Author", "Upinder S. Bhalla, 2009, updated 2014, NCBS",
        "Description", "SteadyState: works out a steady-state value for "
        "a reaction system. "
        "This class uses the multidimensional root finder algorithms "
        "to find the fixed points closest to the "
        "current molecular concentrations. "
        "When it finds the fixed points, it figures out eigenvalues of "
        "the solution, as a way to help classify the fixed points. "
        "Note that the method finds unstable as well as stable fixed "
        "points.\n "
        "The SteadyState class also provides a utility function "
        "*randomInit()*	to "
        "randomly initialize the concentrations, within the constraints "
        "of stoichiometry. This is useful if you are trying to find "
        "the major fixed points of the system. Note that this is "
        "probabilistic. If a fixed point is in a very narrow range of "
        "state space the probability of finding it is small and you "
        "will have to run many iterations with different initial "
        "conditions to find it.\n "
        "The numerical calculations used by the SteadyState solver are "
        "prone to failing on individual calculations. All is not lost, "
        "because the system reports the solutionStatus. "
        "It is recommended that you test this field after every "
        "calculation, so you can simply ignore "
        "cases where it failed and try again with different starting "
        "conditions.\n "
        "Another rule of thumb is that the SteadyState object is more "
        "likely to succeed in finding solutions from a new starting point "
        "if you numerically integrate the chemical system for a short "
        "time (typically under 1 second) before asking it to find the "
        "fixed point. "
    };

    static Dinfo< SteadyState > dinfo;
    static Cinfo steadyStateCinfo(
        "SteadyState",
        Neutral::initCinfo(),
        steadyStateFinfos,
        sizeof( steadyStateFinfos )/sizeof(Finfo *),
        &dinfo,
        doc, sizeof( doc ) / sizeof( string )
    );

    return &steadyStateCinfo;
}

static const Cinfo* steadyStateCinfo = SteadyState::initCinfo();

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

SteadyState::SteadyState()
    :
    nIter_( 0 ),
    maxIter_( 100 ),
    badStoichiometry_( 0 ),
    status_( "OK" ),
    isInitialized_( 0 ),
    isSetup_( 0 ),
    convergenceCriterion_( 1e-7 ),
    stoich_(),
    numVarPools_( 0 ),
    nReacs_( 0 ),
    rank_( 0 ),
    reassignTotal_( 0 ),
    nNegEigenvalues_( 0 ),
    nPosEigenvalues_( 0 ),
    stateType_( 0 ),
    solutionStatus_( 0 ),
    numFailed_( 0 )
{
}

SteadyState::~SteadyState()
{

}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

Id SteadyState::getStoich() const
{
    return stoich_;
}

void SteadyState::setStoich( Id value )
{
    if ( !value.element()->cinfo()->isA( "Stoich" ) )
    {
        cout << "Error: SteadyState::setStoich: Must be of Stoich class\n";
        return;
    }

    stoich_ = value;
    Stoich* stoichPtr = reinterpret_cast< Stoich* >( value.eref().data());
    numVarPools_ = Field< unsigned int >::get( stoich_, "numVarPools" );
    nReacs_ = Field< unsigned int >::get( stoich_, "numRates" );
    setupSSmatrix();
    double vol = LookupField< unsigned int, double >::get(
                     stoichPtr->getCompartment(), "oneVoxelVolume", 0 );
    pool_.setVolume( vol );
    pool_.setStoich( stoichPtr, 0 );
    pool_.updateAllRateTerms( stoichPtr->getRateTerms(),
                              stoichPtr->getNumCoreRates() );
    isInitialized_ = 1;
}

bool SteadyState::badStoichiometry() const
{
    return badStoichiometry_;
}

bool SteadyState::isInitialized() const
{
    return isInitialized_;
}

unsigned int SteadyState::getNiter() const
{
    return nIter_;
}

string SteadyState::getStatus() const
{
    return status_;
}

unsigned int SteadyState::getMaxIter() const
{
    return maxIter_;
}

void SteadyState::setMaxIter( unsigned int value )
{
    maxIter_ = value;
}

unsigned int SteadyState::getRank() const
{
    return rank_;
}

unsigned int SteadyState::getNumVarPools() const
{
    return numVarPools_;
}

unsigned int SteadyState::getStateType() const
{
    return stateType_;
}

unsigned int SteadyState::getNnegEigenvalues() const
{
    return nNegEigenvalues_;
}

unsigned int SteadyState::getNposEigenvalues() const
{
    return nPosEigenvalues_;
}

unsigned int SteadyState::getSolutionStatus() const
{
    return solutionStatus_;
}

void SteadyState::setConvergenceCriterion( double value )
{
    if ( value > 1e-10 )
        convergenceCriterion_ = value;
    else
        cout << "Warning: Convergence criterion " << value <<
             " too small. Old value " <<
             convergenceCriterion_ << " retained\n";
}

double SteadyState::getConvergenceCriterion() const
{
    return convergenceCriterion_;
}

double SteadyState::getTotal( const unsigned int i ) const
{
    if ( i < total_.size() )
        return total_[i];
    cout << "Warning: SteadyState::getTotal: index " << i <<
         " out of range " << total_.size() << endl;
    return 0.0;
}

void SteadyState::setTotal( const unsigned int i, double val )
{
    if ( i < total_.size() )
    {
        total_[i] = val;
        reassignTotal_ = 1;
        return;
    }
    cout << "Warning: SteadyState::setTotal: index " << i <<
         " out of range " << total_.size() << endl;
}

double SteadyState::getEigenvalue( const unsigned int i ) const
{
    if ( i < eigenvalues_.size() )
        return eigenvalues_[i];
    cout << "Warning: SteadyState::getEigenvalue: index " << i <<
         " out of range " << eigenvalues_.size() << endl;
    return 0.0;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

// Static func
void SteadyState::setupMatrix()
{
    setupSSmatrix();
}
void SteadyState::settleFunc()
{
    settle( 0 );
}

void SteadyState::resettleFunc()
{
    settle( 1 );
}

// Dummy function
void SteadyState::assignY( double* S )
{
}

void SteadyState::showMatrices()
{
    if ( !isInitialized_ ) {
        cout << "SteadyState::showMatrices: Sorry, the system is not yet initialized.\n";
        return;
    }
    int numConsv = numVarPools_ - rank_;
    cout << "Totals:	";
    for ( int i = 0; i < numConsv; ++i )
        cout << total_[i] << "	";
    cout << endl;
    cout << "gamma " << gamma_ << endl;
    cout << "Nr " << Nr_ << endl;
    cout << "LU " << LU_ << endl;
}

void SteadyState::setupSSmatrix()
{

    if ( numVarPools_ == 0 || nReacs_ == 0 )
        return;

    int nTot = numVarPools_ + nReacs_;

    ublas::matrix<double> N(numVarPools_, nReacs_, 0.0);

    LU_ = ublas::matrix< double >( numVarPools_, nTot, 0.0);

    vector< int > entry = Field< vector< int > >::get(
                              stoich_, "matrixEntry" );
    vector< unsigned int > colIndex = Field< vector< unsigned int > >::get(
                                          stoich_, "columnIndex" );
    vector< unsigned int > rowStart = Field< vector< unsigned int > >::get(
                                          stoich_, "rowStart" );

    for ( unsigned int i = 0; i < numVarPools_; ++i )
    {
        LU_(i, i + nReacs_) = 1;
        unsigned int k = rowStart[i];
        for ( unsigned int j = 0; j < nReacs_; ++j )
        {
            double x = 0;
            if ( j == colIndex[k] && k < rowStart[i+1] )
            {
                x = entry[k++];
            }
            N(i,j) = x;
            LU_(i,j) = x;
        }
    }

    // This function reorgranize LU_.
    rank_ = rankUsingBoost( LU_ );
    Nr_ = ublas::matrix< double >( rank_, nReacs_ );
    Nr_.assign( ublas::zero_matrix< double >( rank_, nReacs_ ) );

    unsigned int nConsv = numVarPools_ - rank_;
    if ( nConsv == 0 )
    {
        cout << "SteadyState::setupSSmatrix(): Number of conserved species == 0. Aborting\n";
        return;
    }

    // Fill up Nr.
    for ( unsigned int i = 0; i < rank_; i++)
        for ( unsigned int j = i; j < nReacs_; j++)
            Nr_(i,j) = LU_(i, j);

    gamma_ = ublas::matrix< double >( nConsv, numVarPools_, 0.0 );

    // Fill up gamma
    for ( unsigned int i = rank_; i < numVarPools_; ++i )
        for ( unsigned int j = 0; j < numVarPools_; ++j )
            gamma_(i-rank_, j) = LU_(i, j+nReacs_);

    // Fill up boundary condition values
    total_.resize( nConsv );
    total_.assign( nConsv, 0.0 );

    Id ksolve = Field< Id >::get( stoich_, "ksolve" );
    vector< double > nVec =
        LookupField< unsigned int, vector< double > >::get(
            ksolve,"nVec", 0 );

    if ( nVec.size() >= numVarPools_ )
    {
        for ( unsigned int i = 0; i < nConsv; ++i )
            for ( unsigned int j = 0; j < numVarPools_; ++j )
                total_[i] += gamma_(i,j) * nVec[ j ];
        isSetup_ = 1;
    }
    else
    {
        cout << "Error: SteadyState::setupSSmatrix(): unable to get"
             "pool numbers from ksolve.\n";
        isSetup_ = 0;
    }

}

void SteadyState::classifyState( const double* T )
{
    /* column_major trait is needed for fortran */
    ublas::matrix<double, ublas::column_major> J(numVarPools_, numVarPools_);

    double tot = 0.0;
    Stoich* s = reinterpret_cast< Stoich* >( stoich_.eref().data() );
    vector< double > nVec = LookupField< unsigned int, vector< double > >::get(
                                s->getKsolve(), "nVec", 0 );
    for ( unsigned int i = 0; i < numVarPools_; ++i )
        tot += nVec[i];

    tot *= DELTA;

    vector< double > yprime( nVec.size(), 0.0 );
    // Fill up Jacobian
    for ( unsigned int i = 0; i < numVarPools_; ++i )
    {
        double orig = nVec[i];
        if ( std::isnan( orig ) or std::isinf( orig ) )
        {
            cout << "Warning: SteadyState::classifyState: orig=nan\n";
            solutionStatus_ = 2; // Steady state OK, eig failed
            J.clear();
            return;
        }
        if ( std::isnan( tot ) or std::isinf( tot ))
        {
            cout << "Warning: SteadyState::classifyState: tot=nan\n";
            solutionStatus_ = 2; // Steady state OK, eig failed
            J.clear();
            return;
        }
        nVec[i] = orig + tot;

        pool_.updateRates( &nVec[0], &yprime[0] );
        nVec[i] = orig;

        // Assign the rates for each mol.
        for ( unsigned int j = 0; j < numVarPools_; ++j )
        {
            if( std::isnan( yprime[j] ) or std::isinf( yprime[j] ) )
            {
                cout << "Warning: Overflow/underflow. Can't continue " << endl;
                solutionStatus_ = 2;
                return;
            }
            J(i, j) = yprime[j];
        }
    }

    // Jacobian is now ready. Find eigenvalues.
    ublas::vector< std::complex< double > > eigenVec ( J.size1() );

    ublas::matrix< std::complex<double>, ublas::column_major >* vl, *vr;
    vl = NULL; vr = NULL;

    /*-----------------------------------------------------------------------------
     *  INFO: Calling lapack routine geev to compute eigen vector of matrix J.
     *
     *  Argument 3 and 4 are left- and right-eigenvectors. Since we do not need
     *  them, they are set to NULL. Argument 2 holds eigen-vector and result is
     *  copied to it (output ).
     *-----------------------------------------------------------------------------*/
    int status = lapack::geev( J, eigenVec, vl, vr, lapack::optimal_workspace() );

    eigenvalues_.clear();
    eigenvalues_.resize( numVarPools_, 0.0 );
    if ( status != 0 )
    {
        cout << "Warning: SteadyState::classifyState failed to find eigenvalues. Status = " <<
             status << endl;
        solutionStatus_ = 2; // Steady state OK, eig classification failed
    }
    else     // Eigenvalues are ready. Classify state.
    {
        nNegEigenvalues_ = 0;
        nPosEigenvalues_ = 0;
        for ( unsigned int i = 0; i < numVarPools_; ++i )
        {
            std::complex<value_type> z = eigenVec[ i ];
            double r = z.real();
            nNegEigenvalues_ += ( r < -EPSILON );
            nPosEigenvalues_ += ( r > EPSILON );
            eigenvalues_[i] = r;
            // We have a problem here because numVarPools_ usually > rank
            // This means we have several zero eigenvalues.
        }
        if ( nNegEigenvalues_ == rank_ )
            stateType_ = 0; // Stable
        else if ( nPosEigenvalues_ == rank_ ) // Never see it.
            stateType_ = 1; // Unstable
        else  if (nPosEigenvalues_ == 1)
            stateType_ = 2; // Saddle
        else if ( nPosEigenvalues_ >= 2 )
            stateType_ = 3; // putative oscillatory
        else if ( nNegEigenvalues_ == ( rank_ - 1) && nPosEigenvalues_ == 0 )
            stateType_ = 4; // one zero or unclassified eigenvalue. Messy.
        else
            stateType_ = 5; // Other
    }
}

static bool isSolutionValid( const vector< double >& x )
{
    for( size_t i = 0; i < x.size(); i++ )
    {
        double v = x[i];
        if ( std::isnan( v ) or std::isinf( v ) )
        {
            cout << "Warning: SteadyState iteration gave nan/inf concs\n";
            return false;
        }
        else if( v < 0.0 )
        {
            cout << "Warning: SteadyState iteration gave negative concs\n";
            return false;
        }
    }
    return true;
}

/**
 * The settle function computes the steady state nearest the initial
 * conditions.
 */
void SteadyState::settle( bool forceSetup )
{

    if ( !isInitialized_ )
    {
        cout << "Error: SteadyState object has not been initialized. No calculations done\n";
        return;
    }

    if ( forceSetup || isSetup_ == 0 )
        setupSSmatrix();

    // Setting up matrices and vectors for the calculation.
    unsigned int nConsv = numVarPools_ - rank_;
    double * T = (double *) calloc( nConsv, sizeof( double ) );

    // Setting up matrices and vectors for the calculation.
    Id ksolve = Field< Id >::get( stoich_, "ksolve" );

    ss = new NonlinearSystem( numVarPools_ );

    ss->ri.rank = rank_;
    ss->ri.num_reacs = nReacs_;
    ss->ri.num_mols = numVarPools_;
    ss->ri.T = T;
    ss->ri.Nr = Nr_;
    ss->ri.gamma = gamma_;
    ss->ri.pool = &pool_;
    ss->ri.nVec = LookupField< unsigned int, vector< double > >::get(
            ksolve,"nVec", 0
            );
    ss->ri.convergenceCriterion = convergenceCriterion_;

    // This gives the starting point for finding the solution.
    vector<double> init( numVarPools_ );

    // Instead of starting at sqrt( x ),
    for( size_t i = 0; i < numVarPools_; ++i )
        init[i] = max( 0.0, sqrt(ss->ri.nVec[i]) );

    ss->initialize<vector<double>>( init );

    // Fill up boundary condition values
    if ( reassignTotal_ )   // The user has defined new conservation values.
    {
        for (size_t i = 0; i < nConsv; ++i )
            T[i] = total_[i];
        reassignTotal_ = 0;
    }
    else
    {
        for ( size_t i = 0; i < nConsv; ++i )
            for ( size_t j = 0; j < numVarPools_; ++j )
                T[i] += gamma_( i, j ) * ss->ri.nVec[ j ];
        total_.assign( T, T + nConsv );
    }

    vector< double > repair( numVarPools_, 0.0 );

    for ( unsigned int j = 0; j < numVarPools_; ++j )
        repair[j] = ss->ri.nVec[j];


    int status = 1;

    // Find roots . If successful, set status to 0.
    if( ss->find_roots_gnewton( ) )
        status = 0;

    if ( status == 0 && isSolutionValid( ss->ri.nVec ) )
    {
        solutionStatus_ = 0; // Good solution

        LookupField< unsigned int, vector< double > >::set(
            ksolve, "nVec", 0, ss->ri.nVec
            );
        // Check what we set
        vector<double> t = LookupField< unsigned int, vector< double > >::get(
             ksolve,"nVec", 0
             );

        //classifyState( T );
    }
    else
    {
        cout << "Warning: SteadyState iteration failed, status = " <<
             status_ << ", nIter = " << ss->ri.nIter << endl;

        for ( unsigned int j = 0; j < numVarPools_; j++ )
            ss->ri.nVec[j] = repair[j];

        solutionStatus_ = 1; // Steady state failed.
        LookupField< unsigned int, vector< double > >::set(
            ksolve, "nVec", 0, ss->ri.nVec
            );

    }

    // Clean up.
    free( T );
}


/*-----------------------------------------------------------------------------
 *  These functions computes rank of a matrix.
 *-----------------------------------------------------------------------------*/

/**
 * @brief Swap row r1 and r2.
 *
 * @param mat Matrix input
 * @param r1 index of row 1
 * @param r2 index of row 2
 */
void swapRows( ublas::matrix< double >& mat, unsigned int r1, unsigned int r2)
{
    ublas::vector<value_type> temp( mat.size2() );
    for (size_t i = 0; i < mat.size2(); i++)
    {
        temp[i] = mat(r1, i );
        mat(r1, i ) = mat(r2, i );
    }

    for (size_t i = 0; i < mat.size2(); i++)
        mat(r2, i) = temp[i];
}


int reorderRows( ublas::matrix< double >& U, int start, int leftCol )
{
    int leftMostRow = start;
    int numReacs = U.size2() - U.size1();
    int newLeftCol = numReacs;
    for ( size_t i = start; i < U.size1(); ++i )
    {
        for ( int j = leftCol; j < numReacs; ++j )
        {
            if ( fabs( U(i,j )) > SteadyState::EPSILON )
            {
                if ( j < newLeftCol )
                {
                    newLeftCol = j;
                    leftMostRow = i;
                }
                break;
            }
        }
    }

    if ( leftMostRow != start )   // swap them.
        swapRows( U, start, leftMostRow );

    return newLeftCol;
}

void eliminateRowsBelow( ublas::matrix< double >& U, int start, int leftCol )
{
    int numMols = U.size1();
    double pivot = U( start, leftCol );
    assert( fabs( pivot ) > SteadyState::EPSILON );
    for ( int i = start + 1; i < numMols; ++i )
    {
        double factor = U(i, leftCol);
        if( fabs ( factor ) > SteadyState::EPSILON )
        {
            factor = factor / pivot;
            for ( size_t j = leftCol + 1; j < U.size2(); ++j )
            {
                double x = U(i,j);
                double y = U( start, j );
                x -= y * factor;
                if ( fabs( x ) < SteadyState::EPSILON )
                    x = 0.0;
                U( i, j ) = x;
            }
        }
        U(i, leftCol) = 0.0;
    }
}

unsigned int rankUsingBoost( ublas::matrix<double>& U )
{
    int numMols = U.size1();
    int numReacs = U.size2() - numMols;
    int i;
    // Start out with a nonzero entry at 0,0
    int leftCol = reorderRows( U, 0, 0 );

    for ( i = 0; i < numMols - 1; ++i )
    {
        eliminateRowsBelow( U, i, leftCol );
        leftCol = reorderRows( U, i + 1, leftCol );
        if ( leftCol == numReacs )
            break;
    }
    return i + 1;
}


static bool checkAboveZero( const vector< double >& y )
{
    for ( vector< double >::const_iterator
            i = y.begin(); i != y.end(); ++i )
    {
        if ( *i < 0.0 )
            return false;
    }
    return true;
}

/**
 * @brief Utility funtion to doing scans for steady states.
 *
 * @param tot
 * @param g
 * @param S
 */
void recalcTotal( vector< double >& tot, ublas::matrix<double>& g, const double* S )
{
    assert( g.size1() == tot.size() );
    for ( size_t i = 0; i < g.size1(); ++i )
    {
        double t = 0.0;
        for ( unsigned int j = 0; j < g.size2(); ++j )
            t += g( i, j ) * S[j];
        tot[ i ] = t;
    }
}

/**
 * Generates a new set of values for the S vector that is a) random
 * and b) obeys the conservation rules.
 */
void SteadyState::randomizeInitialCondition( const Eref& me )
{
    Id ksolve = Field< Id >::get( stoich_, "ksolve" );
    vector< double > nVec =
        LookupField< unsigned int, vector< double > >::get(
                ksolve,"nVec", 0 );
    int numConsv = total_.size();
    recalcTotal( total_, gamma_, &nVec[0] );
    // The reorderRows function likes to have an I matrix at the end of
    // numVarPools_, so we provide space for it, although only its first
    // column is used for the total vector.
    ublas::matrix<double> U(numConsv, numVarPools_ + numConsv, 0);

    for ( int i = 0; i < numConsv; ++i )
    {
        for ( unsigned int j = 0; j < numVarPools_; ++j )
            U(i, j) = gamma_(i, j);

        U(i, numVarPools_) = total_[i];
    }
    // Do the forward elimination
    int rank = rankUsingBoost( U );
    assert( rank = numConsv );

    vector< double > eliminatedTotal( numConsv, 0.0 );
    for ( int i = 0; i < numConsv; ++i )
        eliminatedTotal[i] = U( i, numVarPools_ );

    // Put Find a vector Y that fits the consv rules.
    vector< double > y( numVarPools_, 0.0 );
    do
    {
        fitConservationRules( U, eliminatedTotal, y );
    }
    while ( !checkAboveZero( y ) );

    // Sanity check. Try the new vector with the old gamma and tots
    for ( int i = 0; i < numConsv; ++i )
    {
        double tot = 0.0;
        for ( unsigned int j = 0; j < numVarPools_; ++j )
        {
            tot += y[j] * gamma_( i, j );
        }
        assert( fabs( tot - total_[i] ) / tot < EPSILON );
    }

    // Put the new values into S.
    // cout << endl;
    for ( unsigned int j = 0; j < numVarPools_; ++j )
    {
        nVec[j] = y[j];
        // cout << y[j] << " ";
    }
    LookupField< unsigned int, vector< double > >::set(
        ksolve,"nVec", 0, nVec );
}

/**
 * This does the actual work of generating random numbers and
 * making sure they fit.
 */
void SteadyState::fitConservationRules(
    ublas::matrix<double>& U, const vector< double >& eliminatedTotal,
    vector< double >&y
)
{
    int numConsv = total_.size();
    int lastJ = numVarPools_;
    for ( int i = numConsv - 1; i >= 0; --i )
    {
        for ( unsigned int j = 0; j < numVarPools_; ++j )
        {
            double g = U( i, j );
            if ( fabs( g ) > EPSILON )
            {
                // double ytot = calcTot( g, i, j, lastJ );
                double ytot = 0.0;
                for ( int k = j; k < lastJ; ++k )
                {
                    y[k] = moose::mtrand();
                    ytot += y[k] * U( i, k );
                }
                assert( fabs( ytot ) > EPSILON );
                double lastYtot = 0.0;
                for ( unsigned int k = lastJ; k < numVarPools_; ++k )
                {
                    lastYtot += y[k] * U( i, k );
                }
                double scale = ( eliminatedTotal[i] - lastYtot ) / ytot;
                for ( int k = j; k < lastJ; ++k )
                {
                    y[k] *= scale;
                }
                lastJ = j;
                break;
            }
        }
    }
}

