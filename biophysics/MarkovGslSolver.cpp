/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>
#include "MarkovGslSolver.h"

static SrcFinfo1< vector<double> >* stateOut()
{
	static SrcFinfo1< vector< double > > stateOut( "stateOut",
		"Sends updated state to the MarkovChannel class." );
	return &stateOut;
}

const Cinfo* MarkovGslSolver::initCinfo()
{
		///////////////////////////////////////////////////////
		// Field definitions
		///////////////////////////////////////////////////////
		static ReadOnlyValueFinfo< MarkovGslSolver, bool > isInitialized(
			"isInitialized",
			"True if the message has come in to set solver parameters.",
			&MarkovGslSolver::getIsInitialized
		);
		static ValueFinfo< MarkovGslSolver, string > method( "method",
			"Numerical method to use.",
			&MarkovGslSolver::setMethod,
			&MarkovGslSolver::getMethod
		);
		static ValueFinfo< MarkovGslSolver, double > relativeAccuracy(
			"relativeAccuracy",
			"Accuracy criterion",
			&MarkovGslSolver::setRelativeAccuracy,
			&MarkovGslSolver::getRelativeAccuracy
		);
		static ValueFinfo< MarkovGslSolver, double > absoluteAccuracy(
			"absoluteAccuracy",
			"Another accuracy criterion",
			&MarkovGslSolver::setAbsoluteAccuracy,
			&MarkovGslSolver::getAbsoluteAccuracy
		);
		static ValueFinfo< MarkovGslSolver, double > internalDt(
			"internalDt",
			"internal timestep to use.",
			&MarkovGslSolver::setInternalDt,
			&MarkovGslSolver::getInternalDt
		);

		///////////////////////////////////////////////////////
		// DestFinfo definitions
		///////////////////////////////////////////////////////
		static DestFinfo init( "init",
			"Initialize solver parameters.",
			new OpFunc1< MarkovGslSolver, vector< double > >
			( &MarkovGslSolver::init )
		);

		static DestFinfo handleQ( "handleQ",
			"Handles information regarding the instantaneous rate matrix from "
			"the MarkovRateTable class.",
			new OpFunc1< MarkovGslSolver, vector< vector< double > > >( &MarkovGslSolver::handleQ) );

		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< MarkovGslSolver >( &MarkovGslSolver::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< MarkovGslSolver >( &MarkovGslSolver::reinit ) );
		///////////////////////////////////////////////////////
		// Shared definitions
		///////////////////////////////////////////////////////
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* MarkovGslFinfos[] =
	{
		&isInitialized,			// ValueFinfo
		&method,						// ValueFinfo
		&relativeAccuracy,	// ValueFinfo
		&absoluteAccuracy,	// ValueFinfo
		&internalDt,				// ValueFinfo
		&init,							// DestFinfo
		&handleQ,						// DestFinfo
		&proc,							// SharedFinfo
		stateOut(),  				// SrcFinfo
	};

	static string doc[] =
	{
		"Name", "MarkovGslSolver",
		"Author", "Vishaka Datta S, 2011, NCBS",
		"Description", "Solver for Markov Channel."
	};

	static Dinfo< MarkovGslSolver > dinfo;
	static Cinfo MarkovGslSolverCinfo(
		"MarkovGslSolver",
		Neutral::initCinfo(),
		MarkovGslFinfos,
		sizeof(MarkovGslFinfos)/sizeof(Finfo *),
		&dinfo,
        doc,
        sizeof(doc) / sizeof(string)
	);

	return &MarkovGslSolverCinfo;
}

static const Cinfo* MarkovGslSolverCinfo = MarkovGslSolver::initCinfo();

///////////////////////////////////////////////////
// Basic class function definitions
///////////////////////////////////////////////////

MarkovGslSolver::MarkovGslSolver()
{
	isInitialized_ = 0;
	method_ = "rk5";
	gslStepType_ = gsl_odeiv_step_rkf45;
	gslStep_ = 0;
	nVars_ = 0;
	absAccuracy_ = 1.0e-8;
	relAccuracy_ = 1.0e-8;
	internalStepSize_ = 1.0e-6;
	stateGsl_ = 0;
	gslEvolve_ = NULL;
	gslControl_ = NULL;
}

MarkovGslSolver::~MarkovGslSolver()
{
	if ( gslEvolve_ )
		gsl_odeiv_evolve_free( gslEvolve_ );
	if ( gslControl_ )
		gsl_odeiv_control_free( gslControl_ );
	if ( gslStep_ )
		gsl_odeiv_step_free( gslStep_ );

	if ( stateGsl_ )
		delete[] stateGsl_;
}

int MarkovGslSolver::evalSystem( double t, const double* state, double* f, void *params)
{
	vector< vector< double > >* Q = static_cast< vector< vector< double > >* >( params );
	unsigned int nVars = Q->size();

	//Matrix being accessed along columns, which is a very bad thing in terms of
	//cache optimality. Transposing the matrix during reinit() would be a good idea.
	for ( unsigned int i = 0; i < nVars; ++i)
	{
		f[i] = 0;
		for ( unsigned int j = 0; j < nVars; ++j)
			f[i] += state[j] * ((*Q)[j][i]);
	}

	return GSL_SUCCESS;
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

bool MarkovGslSolver::getIsInitialized() const
{
	return isInitialized_;
}

string MarkovGslSolver::getMethod() const
{
	return method_;
}

void MarkovGslSolver::setMethod( string method )
{
	method_ = method;
	gslStepType_ = 0;

	if ( method == "rk2" ) {
		gslStepType_ = gsl_odeiv_step_rk2;
	} else if ( method == "rk4" ) {
		gslStepType_ = gsl_odeiv_step_rk4;
	} else if ( method == "rk5" ) {
		gslStepType_ = gsl_odeiv_step_rkf45;
	} else if ( method == "rkck" ) {
		gslStepType_ = gsl_odeiv_step_rkck;
	} else if ( method == "rk8pd" ) {
		gslStepType_ = gsl_odeiv_step_rk8pd;
	} else if ( method == "rk2imp" ) {
		gslStepType_ = gsl_odeiv_step_rk2imp;
	} else if ( method == "rk4imp" ) {
		gslStepType_ = gsl_odeiv_step_rk4imp;
	} else if ( method == "bsimp" ) {
		gslStepType_ = gsl_odeiv_step_rk4imp;
		cout << "Warning: implicit Bulirsch-Stoer method not yet implemented: needs Jacobian\n";
	} else if ( method == "gear1" ) {
		gslStepType_ = gsl_odeiv_step_gear1;
	} else if ( method == "gear2" ) {
		gslStepType_ = gsl_odeiv_step_gear2;
	} else {
		cout << "Warning: MarkovGslSolver::innerSetMethod: method '" <<
			method << "' not known, using rk5\n";
		gslStepType_ = gsl_odeiv_step_rkf45;
	}
}

double MarkovGslSolver::getRelativeAccuracy() const
{
	return relAccuracy_;
}

void MarkovGslSolver::setRelativeAccuracy( double value )
{
	relAccuracy_ = value;
}

double MarkovGslSolver::getAbsoluteAccuracy() const
{
	return absAccuracy_;
}
void MarkovGslSolver::setAbsoluteAccuracy( double value )
{
	absAccuracy_ = value;
}

double MarkovGslSolver::getInternalDt() const
{
	return internalStepSize_;
}

void MarkovGslSolver::setInternalDt( double value )
{
	internalStepSize_ = value;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

//Handles data from MarkovChannel class.
void MarkovGslSolver::init( vector< double > initialState )
{
	nVars_ = initialState.size();

	if ( stateGsl_ == 0 )
		stateGsl_ = new double[ nVars_ ];

	state_ = initialState;
	initialState_ = initialState;

	Q_.resize( nVars_ );

	for ( unsigned int i = 0; i < nVars_; ++i )
		Q_[i].resize( nVars_, 0.0 );

	isInitialized_ = 1;

	assert( gslStepType_ != 0 );
	if ( gslStep_ )
		gsl_odeiv_step_free(gslStep_);

	gslStep_ = gsl_odeiv_step_alloc( gslStepType_, nVars_ );
	assert( gslStep_ != 0 );

	if ( !gslEvolve_ )
		gslEvolve_ = gsl_odeiv_evolve_alloc(nVars_);
	else
		gsl_odeiv_evolve_reset(gslEvolve_);

	assert(gslEvolve_ != 0);

	if ( !gslControl_ )
		gslControl_ = gsl_odeiv_control_y_new( absAccuracy_, relAccuracy_ );
	else
		gsl_odeiv_control_init(gslControl_,absAccuracy_, relAccuracy_, 1, 0);

	assert(gslControl_!= 0);

	gslSys_.function = &MarkovGslSolver::evalSystem;
	gslSys_.jacobian = 0;
	gslSys_.dimension = nVars_;
	gslSys_.params = static_cast< void * >( &Q_ );
}

//////////////////////////
//MsgDest functions.
/////////////////////////
void MarkovGslSolver::process( const Eref& e, ProcPtr info )
{
	double nextt = info->currTime + info->dt;
	double t = info->currTime;
	double sum = 0;

	for ( unsigned int i = 0; i < nVars_; ++i )
		stateGsl_[i] = state_[i];

	while ( t < nextt ) {
		int status = gsl_odeiv_evolve_apply (
			gslEvolve_, gslControl_, gslStep_, &gslSys_,
			&t, nextt,
			&internalStepSize_, stateGsl_);

		//Simple idea borrowed from Dieter Jaeger's implementation of a Markov
		//channel to deal with potential round-off error.
		sum = 0;
		for ( unsigned int i = 0; i < nVars_; i++ )
			sum += stateGsl_[i];

		for ( unsigned int i = 0; i < nVars_; i++ )
			stateGsl_[i] /= sum;

		if ( status != GSL_SUCCESS )
			break;
	}

	for ( unsigned int i = 0; i < nVars_; ++i )
		state_[i] = stateGsl_[i];

	stateOut()->send( e, state_ );
}

void MarkovGslSolver::reinit( const Eref& e, ProcPtr info )
{
	state_ = initialState_;
	if ( initialState_.empty() )
	{
		cerr << "MarkovGslSolver::reinit : "
				 "Initial state has not been set. Solver has not been initialized."
				 "Call init() before running.\n";
	}

	stateOut()->send( e, state_ );
}

void MarkovGslSolver::handleQ( vector< vector< double > > Q )
{
	Q_ = Q;
}
