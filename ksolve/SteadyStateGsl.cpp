/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This program works out a steady-state value for a reaction system.
 * It uses GSL heavily, and isn't even compiled if the flag isn't set.
 * It finds the ss value closest to the initial conditions.
 *
 * If you want to find multiple stable states, it is best to do this
 * in Python as it gives a lot of flexibility in working out how to
 * find steady states.
 * Likewise, if you want to carry out a dose-response calculation.
 */

#include "header.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"
#include "../randnum/randnum.h"

#ifdef USE_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_odeiv2.h>
#endif

#include "VoxelPoolsBase.h"
#include "OdeSystem.h"
#include "VoxelPools.h"
#include "SteadyStateGsl.h"

int ss_func( const gsl_vector* x, void* params, gsl_vector* f );
#ifdef USE_GSL
int myGaussianDecomp( gsl_matrix* U );
#endif

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

#ifdef USE_GSL
	gsl_matrix* Nr;
	gsl_matrix* gamma;
#endif
};

const Cinfo* SteadyState::initCinfo()
{
	/**
	 * This picks up the entire Stoich data structure
	static Finfo* gslShared[] =
	{
		new SrcFinfo( "reinitSrc", Ftype0::global() ),
		new DestFinfo( "assignStoich",
			Ftype1< void* >::global(),
			RFCAST( &SteadyState::assignStoichFunc )
			),
		new DestFinfo( "setMolN",
			Ftype2< double, unsigned int >::global(),
			RFCAST( &SteadyState::setMolN )
			),
		new SrcFinfo( "requestYsrc", Ftype0::global() ),
		new DestFinfo( "assignY",
			Ftype1< double* >::global(),
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

	static Finfo * steadyStateFinfos[] = {
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
		"This class uses the GSL multidimensional root finder algorithms "
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
#ifdef USE_GSL
		LU_( 0 ),
		Nr_( 0 ),
		gamma_( 0 ),
#endif
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
	;
}

SteadyState::~SteadyState()
{
#ifdef USE_GSL
	if ( LU_ != 0 )
		gsl_matrix_free( LU_ );
	if ( Nr_ != 0 )
		gsl_matrix_free( Nr_ );
	if ( gamma_ != 0 )
		gsl_matrix_free( gamma_ );
#endif
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

Id SteadyState::getStoich() const {
	return stoich_;
}

void SteadyState::setStoich( Id value ) {
	if ( !value.element()->cinfo()->isA( "Stoich" ) ) {
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

bool SteadyState::badStoichiometry() const {
	return badStoichiometry_;
}

bool SteadyState::isInitialized() const {
	return isInitialized_;
}

unsigned int SteadyState::getNiter() const {
	return nIter_;
}

string SteadyState::getStatus() const {
	return status_;
}

unsigned int SteadyState::getMaxIter() const {
	return maxIter_;
}

void SteadyState::setMaxIter( unsigned int value ) {
	maxIter_ = value;
}

unsigned int SteadyState::getRank() const {
	return rank_;
}

unsigned int SteadyState::getNumVarPools() const {
	return numVarPools_;
}

unsigned int SteadyState::getStateType() const {
	return stateType_;
}

unsigned int SteadyState::getNnegEigenvalues() const {
	return nNegEigenvalues_;
}

unsigned int SteadyState::getNposEigenvalues() const {
	return nPosEigenvalues_;
}

unsigned int SteadyState::getSolutionStatus() const {
	return solutionStatus_;
}

void SteadyState::setConvergenceCriterion( double value ) {
	if ( value > 1e-10 )
		convergenceCriterion_ = value;
	else
		cout << "Warning: Convergence criterion " << value <<
		" too small. Old value " <<
		convergenceCriterion_ << " retained\n";
}

double SteadyState::getConvergenceCriterion() const {
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
	if ( i < total_.size() ) {
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

///////////////////////////////////////////////////
// GSL interface stuff
///////////////////////////////////////////////////

#ifdef USE_GSL
void print_gsl_mat( gsl_matrix* m, const char* name )
{
    size_t i, j;
    printf( "%s[%lu, %lu] = \n", name, m->size1, m->size2 );
    for (i = 0; i < m->size1; i++) {
        for (j = 0; j < m->size2; j++) {
            double x = gsl_matrix_get (m, i, j );
            if ( fabs( x ) < 1e-9 ) x = 0;
            printf( "%6g", x );
        }

        printf( "\n");
    }
}
#endif

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
#ifdef USE_GSL
	print_gsl_mat( gamma_, "gamma" );
	print_gsl_mat( Nr_, "Nr" );
	print_gsl_mat( LU_, "LU" );
#endif
}

void SteadyState::setupSSmatrix()
{
#ifdef USE_GSL
	if ( numVarPools_ == 0 || nReacs_ == 0 )
		return;

	int nTot = numVarPools_ + nReacs_;
	gsl_matrix* N = gsl_matrix_calloc (numVarPools_, nReacs_);
	if ( LU_ ) { // Clear out old one.
		gsl_matrix_free( LU_ );
	}
	LU_ = gsl_matrix_calloc (numVarPools_, nTot);
	vector< int > entry = Field< vector< int > >::get(
					stoich_, "matrixEntry" );
	vector< unsigned int > colIndex = Field< vector< unsigned int > >::get(
					stoich_, "columnIndex" );
	vector< unsigned int > rowStart = Field< vector< unsigned int > >::get(
					stoich_, "rowStart" );

	// cout << endl << endl;
	for ( unsigned int i = 0; i < numVarPools_; ++i ) {
		gsl_matrix_set (LU_, i, i + nReacs_, 1 );
		unsigned int k = rowStart[i];
		// cout << endl << i << ":	";
		for ( unsigned int j = 0; j < nReacs_; ++j ) {
			double x = 0;
			if ( j == colIndex[k] && k < rowStart[i+1] ) {
				x = entry[k++];
			}
			// cout << "	" << x;
			gsl_matrix_set (N, i, j, x);
			gsl_matrix_set (LU_, i, j, x );
		}
	}
	cout << endl << endl;

	rank_ = myGaussianDecomp( LU_ );

	unsigned int nConsv = numVarPools_ - rank_;
	if ( nConsv == 0 ) {
		cout << "SteadyState::setupSSmatrix(): Number of conserved species == 0. Aborting\n";
		return;
	}

	if ( Nr_ ) { // Clear out old one.
		gsl_matrix_free( Nr_ );
	}
	Nr_ = gsl_matrix_calloc ( rank_, nReacs_ );
	// Fill up Nr.
	for ( unsigned int i = 0; i < rank_; i++)
		for ( unsigned int j = i; j < nReacs_; j++)
			gsl_matrix_set (Nr_, i, j, gsl_matrix_get( LU_, i, j ) );

	if ( gamma_ ) { // Clear out old one.
		gsl_matrix_free( gamma_ );
	}
	gamma_ = gsl_matrix_calloc (nConsv, numVarPools_ );

	// Fill up gamma
	for ( unsigned int i = rank_; i < numVarPools_; ++i )
		for ( unsigned int j = 0; j < numVarPools_; ++j )
			gsl_matrix_set( gamma_, i - rank_, j,
				gsl_matrix_get( LU_, i, j + nReacs_ ) );

	// Fill up boundary condition values
	total_.resize( nConsv );
	total_.assign( nConsv, 0.0 );

	/*
	cout << "S = (";
	for ( unsigned int j = 0; j < numVarPools_; ++j )
		cout << s_->S()[ j ] << ", ";
	cout << "), Sinit = ( ";
	for ( unsigned int j = 0; j < numVarPools_; ++j )
		cout << s_->Sinit()[ j ] << ", ";
	cout << ")\n";
	*/
	Id ksolve = Field< Id >::get( stoich_, "ksolve" );
	vector< double > nVec =
			LookupField< unsigned int, vector< double > >::get(
			ksolve,"nVec", 0 );

	if ( nVec.size() >= numVarPools_ ) {
		for ( unsigned int i = 0; i < nConsv; ++i )
			for ( unsigned int j = 0; j < numVarPools_; ++j )
				total_[i] += gsl_matrix_get( gamma_, i, j ) * nVec[ j ];
		isSetup_ = 1;
	} else {
		cout << "Error: SteadyState::setupSSmatrix(): unable to get"
				"pool numbers from ksolve.\n";
		isSetup_ = 0;
	}

	gsl_matrix_free( N );
#endif
}

static double op( double x )
{
	return x * x;
}

static double invop( double x )
{
	if ( x > 0.0 )
		return sqrt( x );
	return 0.0;
}


/**
 * This does the iteration, using the specified method.
 * First try gsl_multiroot_fsolver_hybrids
 * If that doesn't work try gsl_multiroot_fsolver_dnewton
 * Returns the gsl status.
 */
#ifdef USE_GSL
int iterate( const gsl_multiroot_fsolver_type* st, struct reac_info *ri,
	int maxIter )
{
	int status = 0;
	gsl_vector* x = gsl_vector_calloc( ri->num_mols );
	gsl_multiroot_fsolver *solver =
		gsl_multiroot_fsolver_alloc( st, ri->num_mols );
	gsl_multiroot_function func = {&ss_func, ri->num_mols, ri};

	// This gives the starting point for finding the solution
	for ( unsigned int i = 0; i < ri->num_mols; ++i )
		gsl_vector_set( x, i, invop( ri->nVec[i] ) );

	gsl_multiroot_fsolver_set( solver, &func, x );

	ri->nIter = 0;
	do {
		ri->nIter++;
		status = gsl_multiroot_fsolver_iterate( solver );
		if (status ) break;
		status = gsl_multiroot_test_residual(
			solver->f, ri->convergenceCriterion);
	} while (status == GSL_CONTINUE && ri->nIter < maxIter );

	gsl_multiroot_fsolver_free( solver );
	gsl_vector_free( x );
	return status;
}
#endif

void SteadyState::classifyState( const double* T )
{
#ifdef USE_GSL
	// unsigned int nConsv = numVarPools_ - rank_;
	gsl_matrix* J = gsl_matrix_calloc ( numVarPools_, numVarPools_ );
	// double* yprime = new double[ numVarPools_ ];
	// vector< double > yprime( numVarPools_, 0.0 );
	// Generate an approximation to the Jacobean by generating small
	// increments to each of the molecules in the steady state, one
	// at a time, and putting the resultant rate vector into a column
	// of the J matrix.
	// This needs a bit of heuristic to decide what is a 'small' increment.
	// Use the CoInits for this. Stoichiometry shouldn't matter too much.
	// I used the totals from consv rules earlier, but that can have
	// negative values.
	double tot = 0.0;
	Stoich* s = reinterpret_cast< Stoich* >( stoich_.eref().data() );
	vector< double > nVec = LookupField< unsigned int, vector< double > >::get(
		s->getKsolve(), "nVec", 0 );
	for ( unsigned int i = 0; i < numVarPools_; ++i ) {
		tot += nVec[i];
	}
	tot *= DELTA;

	vector< double > yprime( nVec.size(), 0.0 );
	// Fill up Jacobian
	for ( unsigned int i = 0; i < numVarPools_; ++i ) {
		double orig = nVec[i];
		if ( isNaN( orig ) ) {
			cout << "Warning: SteadyState::classifyState: orig=nan\n";
			solutionStatus_ = 2; // Steady state OK, eig failed
			gsl_matrix_free ( J );
			return;
		}
		if ( isNaN( tot ) ) {
			cout << "Warning: SteadyState::classifyState: tot=nan\n";
			solutionStatus_ = 2; // Steady state OK, eig failed
			gsl_matrix_free ( J );
			return;
		}
		nVec[i] = orig + tot;

		pool_.updateRates( &nVec[0], &yprime[0] );
		nVec[i] = orig;

		// Assign the rates for each mol.
		for ( unsigned int j = 0; j < numVarPools_; ++j ) {
			gsl_matrix_set( J, i, j, yprime[j] );
		}
	}

	// Jacobian is now ready. Find eigenvalues.
	gsl_vector_complex* vec = gsl_vector_complex_alloc( numVarPools_ );
	gsl_eigen_nonsymm_workspace* workspace =
		gsl_eigen_nonsymm_alloc( numVarPools_ );
	int status = gsl_eigen_nonsymm( J, vec, workspace );
	eigenvalues_.clear();
	eigenvalues_.resize( numVarPools_, 0.0 );
	if ( status != GSL_SUCCESS ) {
		cout << "Warning: SteadyState::classifyState failed to find eigenvalues. Status = " <<
			status << endl;
		solutionStatus_ = 2; // Steady state OK, eig classification failed
	} else { // Eigenvalues are ready. Classify state.
		nNegEigenvalues_ = 0;
		nPosEigenvalues_ = 0;
		for ( unsigned int i = 0; i < numVarPools_; ++i ) {
			gsl_complex z = gsl_vector_complex_get( vec, i );
			double r = GSL_REAL( z );
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

	gsl_vector_complex_free( vec );
	gsl_matrix_free ( J );
	gsl_eigen_nonsymm_free( workspace );
#endif
}

static bool isSolutionPositive( const vector< double >& x )
{
	for ( vector< double >::const_iterator
					i = x.begin(); i != x.end(); ++i ) {
		if ( *i < 0.0 ) {
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
#ifdef USE_GSL
	gsl_set_error_handler_off();

	if ( !isInitialized_ ) {
		cout << "Error: SteadyState object has not been initialized. No calculations done\n";
		return;
	}
	if ( forceSetup || isSetup_ == 0 ) {
		setupSSmatrix();
	}

	// Setting up matrices and vectors for the calculation.
	unsigned int nConsv = numVarPools_ - rank_;
	double * T = (double *) calloc( nConsv, sizeof( double ) );

	unsigned int i, j;


	Id ksolve = Field< Id >::get( stoich_, "ksolve" );
	struct reac_info ri;
	ri.rank = rank_;
	ri.num_reacs = nReacs_;
	ri.num_mols = numVarPools_;
	ri.T = T;
	ri.Nr = Nr_;
	ri.gamma = gamma_;
	ri.pool = &pool_;
	ri.nVec =
			LookupField< unsigned int, vector< double > >::get(
			ksolve,"nVec", 0 );
	ri.convergenceCriterion = convergenceCriterion_;

	// Fill up boundary condition values
	if ( reassignTotal_ ) { // The user has defined new conservation values.
		for ( i = 0; i < nConsv; ++i )
			T[i] = total_[i];
		reassignTotal_ = 0;
	} else {
		for ( i = 0; i < nConsv; ++i )
			for ( j = 0; j < numVarPools_; ++j )
				T[i] += gsl_matrix_get( gamma_, i, j ) * ri.nVec[ j ];
		total_.assign( T, T + nConsv );
	}

	vector< double > repair( numVarPools_, 0.0 );
	for ( unsigned int j = 0; j < numVarPools_; ++j )
		repair[j] = ri.nVec[j];

	int status = iterate( gsl_multiroot_fsolver_hybrids, &ri, maxIter_ );
	if ( status ) // It failed. Fall back with the Newton method
		status = iterate( gsl_multiroot_fsolver_dnewton, &ri, maxIter_ );
	status_ = string( gsl_strerror( status ) );
	nIter_ = ri.nIter;
	if ( status == GSL_SUCCESS && isSolutionPositive( ri.nVec ) ) {
		solutionStatus_ = 0; // Good solution
		LookupField< unsigned int, vector< double > >::set(
			ksolve,"nVec", 0, ri.nVec );
		classifyState( T );
	} else {
		cout << "Warning: SteadyState iteration failed, status = " <<
			status_ << ", nIter = " << nIter_ << endl;
		// Repair the mess
		for ( unsigned int j = 0; j < numVarPools_; ++j )
			ri.nVec[j] = repair[j];
		solutionStatus_ = 1; // Steady state failed.
		LookupField< unsigned int, vector< double > >::set(
			ksolve,"nVec", 0, ri.nVec );
	}

	// Clean up.
	free( T );
#endif
}

// Long section here of functions using GSL
#ifdef USE_GSL
int ss_func( const gsl_vector* x, void* params, gsl_vector* f )
{
	struct reac_info* ri = (struct reac_info *)params;
	// Stoich* s = reinterpret_cast< Stoich* >( ri->stoich.eref().data() );
	int num_consv = ri->num_mols - ri->rank;

	for ( unsigned int i = 0; i < ri->num_mols; ++i ) {
		double temp = op( gsl_vector_get( x, i ) );
		if ( isNaN( temp ) || isInfinity( temp ) ) {
			return GSL_ERANGE;
		} else {
			ri->nVec[i] = temp;
		}
	}
	vector< double > vels;
	ri->pool->updateReacVelocities( &ri->nVec[0], vels );
	assert( vels.size() == static_cast< unsigned int >( ri->num_reacs ) );

	// y = Nr . v
	// Note that Nr is row-echelon: diagonal and above.
	for ( int i = 0; i < ri->rank; ++i ) {
		double temp = 0;
		for ( int j = i; j < ri->num_reacs; ++j )
			temp += gsl_matrix_get( ri->Nr, i, j ) * vels[j];
		gsl_vector_set( f, i, temp );
	}

	// dT = gamma.S - T
	for ( int i = 0; i < num_consv; ++i ) {
		double dT = - ri->T[i];
		for ( unsigned int j = 0; j < ri->num_mols; ++j )
			dT += gsl_matrix_get( ri->gamma, i, j) *
				op( gsl_vector_get( x, j ) );

		gsl_vector_set( f, i + ri->rank, dT );
	}

	return GSL_SUCCESS;
}

/**
 * eliminateRowsBelow:
 * Eliminates leftmost entry of all rows below 'start'.
 * Returns the row index of the row which has the leftmost nonzero
 * entry. If this sticks out beyond numReacs, then the elimination is
 * complete, and it returns a zero to indicate this.
 * In leftCol it returns the column # of this leftmost nonzero entry.
 * Zeroes out anything below EPSILON.
 */
void eliminateRowsBelow( gsl_matrix* U, int start, int leftCol )
{
	int numMols = U->size1;
	double pivot = gsl_matrix_get( U, start, leftCol );
	assert( fabs( pivot ) > SteadyState::EPSILON );
	for ( int i = start + 1; i < numMols; ++i ) {
		double factor = gsl_matrix_get( U, i, leftCol );
		if ( fabs ( factor ) > SteadyState::EPSILON ) {
			factor = factor / pivot;
			for ( size_t j = leftCol + 1; j < U->size2; ++j ) {
				double x = gsl_matrix_get( U, i, j );
				double y = gsl_matrix_get( U, start, j );
				x -= y * factor;
				if ( fabs( x ) < SteadyState::EPSILON )
					x = 0.0;
				gsl_matrix_set( U, i, j, x  );
			}
		}
		gsl_matrix_set( U, i, leftCol, 0.0 ); // Cleaning up.
	}
}

/**
 * reorderRows:
 * Finds the leftmost row beginning from start, ignoring everything to the
 * left of leftCol. Puts this row at 'start', swapping with the original.
 * Assumes that the matrix is set up as [N I].
 * Returns the new value of leftCol
 * If there are no appropriate rows, returns numReacs.
 */
int reorderRows( gsl_matrix* U, int start, int leftCol )
{
	int leftMostRow = start;
	int numReacs = U->size2 - U->size1;
	int newLeftCol = numReacs;
	for ( size_t i = start; i < U->size1; ++i ) {
		for ( int j = leftCol; j < numReacs; ++j ) {
			if ( fabs( gsl_matrix_get( U, i, j ) ) > SteadyState::EPSILON ){
				if ( j < newLeftCol ) {
					newLeftCol = j;
					leftMostRow = i;
				}
				break;
			}
		}
	}
	if ( leftMostRow != start ) { // swap them.
		gsl_matrix_swap_rows( U, start, leftMostRow );
	}
	return newLeftCol;
}

/**
 * Does a simple gaussian decomposition. Assumes U has nice clean numbers
 * so I can apply a generous EPSILON to zero things out.
 * Assumes that the U matrix is the N matrix padded out by an identity
 * matrix on the right.
 * Returns rank.
 */
int myGaussianDecomp( gsl_matrix* U )
{
	int numMols = U->size1;
	int numReacs = U->size2 - numMols;
	int i;
	// Start out with a nonzero entry at 0,0
	int leftCol = reorderRows( U, 0, 0 );

	for ( i = 0; i < numMols - 1; ++i ) {
		eliminateRowsBelow( U, i, leftCol );
		leftCol = reorderRows( U, i + 1, leftCol );
		if ( leftCol == numReacs )
			break;
	}
	return i + 1;
}

//////////////////////////////////////////////////////////////////
// Utility functions for doing scans for steady states
//////////////////////////////////////////////////////////////////

void recalcTotal( vector< double >& tot, gsl_matrix* g, const double* S )
{
	assert( g->size1 == tot.size() );
	for ( unsigned int i = 0; i < g->size1; ++i ) {
		double t = 0.0;
		for ( unsigned int j = 0; j < g->size2; ++j )
			t += gsl_matrix_get( g, i, j ) * S[j];
		tot[ i ] = t;
	}
}
#endif // end of long section of functions using GSL

static bool checkAboveZero( const vector< double >& y )
{
	for ( vector< double >::const_iterator
			i = y.begin(); i != y.end(); ++i ) {
		if ( *i < 0.0 )
			return false;
	}
	return true;
}

/**
 * Generates a new set of values for the S vector that is a) random
 * and b) obeys the conservation rules.
 */
void SteadyState::randomizeInitialCondition( const Eref& me )
{
#ifdef USE_GSL
	Id ksolve = Field< Id >::get( stoich_, "ksolve" );
	vector< double > nVec =
			LookupField< unsigned int, vector< double > >::get(
			ksolve,"nVec", 0 );
	int numConsv = total_.size();
	recalcTotal( total_, gamma_, &nVec[0] );
	// The reorderRows function likes to have an I matrix at the end of
	// numVarPools_, so we provide space for it, although only its first
	// column is used for the total vector.
	gsl_matrix* U = gsl_matrix_calloc ( numConsv, numVarPools_ + numConsv );
	for ( int i = 0; i < numConsv; ++i ) {
		for ( unsigned int j = 0; j < numVarPools_; ++j ) {
			gsl_matrix_set( U, i, j, gsl_matrix_get( gamma_, i, j ) );
		}
		gsl_matrix_set( U, i, numVarPools_, total_[i] );
	}
	// Do the forward elimination
	int rank = myGaussianDecomp( U );
	assert( rank = numConsv );

	vector< double > eliminatedTotal( numConsv, 0.0 );
	for ( int i = 0; i < numConsv; ++i ) {
		eliminatedTotal[i] = gsl_matrix_get( U, i, numVarPools_ );
	}

	// Put Find a vector Y that fits the consv rules.
	vector< double > y( numVarPools_, 0.0 );
	do {
		fitConservationRules( U, eliminatedTotal, y );
	} while ( !checkAboveZero( y ) );

	// Sanity check. Try the new vector with the old gamma and tots
	for ( int i = 0; i < numConsv; ++i ) {
		double tot = 0.0;
		for ( unsigned int j = 0; j < numVarPools_; ++j ) {
			tot += y[j] * gsl_matrix_get( gamma_, i, j );
		}
		assert( fabs( tot - total_[i] ) / tot < EPSILON );
	}

	// Put the new values into S.
	// cout << endl;
	for ( unsigned int j = 0; j < numVarPools_; ++j ) {
		nVec[j] = y[j];
		// cout << y[j] << " ";
	}
	LookupField< unsigned int, vector< double > >::set(
			ksolve,"nVec", 0, nVec );
#endif
}

/**
 * This does the actual work of generating random numbers and
 * making sure they fit.
 */
#ifdef USE_GSL
void SteadyState::fitConservationRules(
	gsl_matrix* U, const vector< double >& eliminatedTotal,
		vector< double >&y
	)
{
	int numConsv = total_.size();
	int lastJ = numVarPools_;
	for ( int i = numConsv - 1; i >= 0; --i ) {
		for ( unsigned int j = 0; j < numVarPools_; ++j ) {
			double g = gsl_matrix_get( U, i, j );
			if ( fabs( g ) > EPSILON ) {
				// double ytot = calcTot( g, i, j, lastJ );
				double ytot = 0.0;
				for ( int k = j; k < lastJ; ++k ) {
					y[k] = mtrand();
					ytot += y[k] * gsl_matrix_get( U, i, k );
				}
				assert( fabs( ytot ) > EPSILON );
				double lastYtot = 0.0;
				for ( unsigned int k = lastJ; k < numVarPools_; ++k ) {
					lastYtot += y[k] * gsl_matrix_get( U, i, k );
				}
				double scale = ( eliminatedTotal[i] - lastYtot ) / ytot;
				for ( int k = j; k < lastJ; ++k ) {
					y[k] *= scale;
				}
				lastJ = j;
				break;
			}
		}
	}
}

#endif
