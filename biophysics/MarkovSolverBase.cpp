/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <cfloat>
#include "../basecode/header.h"
#include "MatrixOps.h"

#include "VectorTable.h"
#include "../builtins/Interpol2D.h"
#include "MarkovRateTable.h"

#include "MarkovSolverBase.h"

SrcFinfo1< Vector >* stateOut()
{
	static SrcFinfo1< Vector > stateOut("stateOut",
		"Sends updated state to the MarkovChannel class."
		);
	return &stateOut;
}

const Cinfo* MarkovSolverBase::initCinfo()
{
	/////////////////////
	//SharedFinfos
	/////////////////////
	static DestFinfo handleVm("handleVm",
			"Handles incoming message containing voltage information.",
			new OpFunc1< MarkovSolverBase, double >(&MarkovSolverBase::handleVm)
			);

	static Finfo* channelShared[] =
	{
		&handleVm
	};

	static SharedFinfo channel("channel",
			"This message couples the MarkovSolverBase to the Compartment. The "
			"compartment needs Vm in order to look up the correct matrix "
			"exponential for computing the state.",
			channelShared, sizeof( channelShared ) / sizeof( Finfo* )
			);

	//////////////////////
	//DestFinfos
	//////////////////////

	static DestFinfo process(	"process",
			"Handles process call",
			new ProcOpFunc< MarkovSolverBase >( &MarkovSolverBase::process ) );

	static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< MarkovSolverBase >( &MarkovSolverBase::reinit ) );

	static Finfo* processShared[] =
	{
		&process, &reinit
	};

	static SharedFinfo proc( "proc",
			"This is a shared message to receive Process message from the"
			"scheduler. The first entry is a MsgDest for the Process "
			"operation. It has a single argument, ProcInfo, which "
			"holds lots of information about current time, thread, dt and"
			"so on. The second entry is a MsgDest for the Reinit "
			"operation. It also uses ProcInfo.",
		processShared, sizeof( processShared ) / sizeof( Finfo* )
	);

	static DestFinfo ligandConc("ligandConc",
			"Handles incoming message containing ligand concentration.",
			new OpFunc1< MarkovSolverBase, double >(&MarkovSolverBase::handleLigandConc)
			);

	static DestFinfo init("init",
			"Setups the table of matrix exponentials associated with the"
			" solver object.",
			new OpFunc2< MarkovSolverBase, Id, double >(&MarkovSolverBase::init)
			);

	//////////////////////
	//*ValueFinfos
	/////////////////////

	static ReadOnlyValueFinfo< MarkovSolverBase, Matrix > Q("Q",
			"Instantaneous rate matrix.",
			&MarkovSolverBase::getQ
			);

	static ReadOnlyValueFinfo< MarkovSolverBase, Vector > state("state",
			"Current state of the channel.",
			&MarkovSolverBase::getState
			);

	static ValueFinfo< MarkovSolverBase, Vector > initialstate("initialState",
			"Initial state of the channel.",
			&MarkovSolverBase::setInitialState,
			&MarkovSolverBase::getInitialState
			);

	static ValueFinfo< MarkovSolverBase, double > xmin( "xmin",
		"Minimum value for x axis of lookup table",
			&MarkovSolverBase::setXmin,
			&MarkovSolverBase::getXmin
		);
	static ValueFinfo< MarkovSolverBase, double > xmax( "xmax",
		"Maximum value for x axis of lookup table",
			&MarkovSolverBase::setXmax,
			&MarkovSolverBase::getXmax
		);
	static ValueFinfo< MarkovSolverBase, unsigned int > xdivs( "xdivs",
		"# of divisions on x axis of lookup table",
			&MarkovSolverBase::setXdivs,
			&MarkovSolverBase::getXdivs
		);
	static ReadOnlyValueFinfo< MarkovSolverBase, double > invdx( "invdx",
		"Reciprocal of increment on x axis of lookup table",
			&MarkovSolverBase::getInvDx
		);
	static ValueFinfo< MarkovSolverBase, double > ymin( "ymin",
		"Minimum value for y axis of lookup table",
			&MarkovSolverBase::setYmin,
			&MarkovSolverBase::getYmin
		);
	static ValueFinfo< MarkovSolverBase, double > ymax( "ymax",
		"Maximum value for y axis of lookup table",
			&MarkovSolverBase::setYmax,
			&MarkovSolverBase::getYmax
		);
	static ValueFinfo< MarkovSolverBase, unsigned int > ydivs( "ydivs",
		"# of divisions on y axis of lookup table",
			&MarkovSolverBase::setYdivs,
			&MarkovSolverBase::getYdivs
		);
	static ReadOnlyValueFinfo< MarkovSolverBase, double > invdy( "invdy",
		"Reciprocal of increment on y axis of lookup table",
			&MarkovSolverBase::getInvDy
		);

	static Finfo* markovSolverFinfos[] =
	{
		&channel,						//SharedFinfo
		&proc,							//SharedFinfo
		stateOut(), 				//SrcFinfo
		&ligandConc,				//DestFinfo
		&init,							//DestFinfo
		&Q,									//ReadOnlyValueFinfo
		&state,							//ReadOnlyValueFinfo
		&initialstate,			//ReadOnlyValueFinfo
		&xmin,							//ValueFinfo
		&xmax,							//ValueFinfo
		&xdivs,							//ValueFinfo
		&invdx,							//ReadOnlyValueFinfo
		&ymin,							//ValueFinfo
		&ymax,							//ValueFinfo
		&ydivs,							//ValueFinfo
		&invdy							//ReadOnlyValueFinfo
	};

	static string doc[] =
	{
		"Name", "MarkovSolverBase",
		"Author", "Vishaka Datta S, 2011, NCBS",
		"Description", "Solver for Markov Channel."
	};
	static Dinfo< MarkovSolverBase > dinfo;
	static Cinfo markovSolverBaseCinfo(
			"MarkovSolverBase",
			Neutral::initCinfo(),
			markovSolverFinfos,
			sizeof( markovSolverFinfos ) / sizeof( Finfo* ),
			&dinfo,
        	doc,
        	sizeof(doc) / sizeof(string)
		);

	return &markovSolverBaseCinfo;
}

static const Cinfo* markovSolverBaseCinfo = MarkovSolverBase::initCinfo();

MarkovSolverBase::MarkovSolverBase() : Q_(0), expMats1d_(0), expMat_(0),
	expMats2d_(0), xMin_(DBL_MAX), xMax_(DBL_MIN), xDivs_(0u),
	yMin_(DBL_MAX), yMax_(DBL_MIN), yDivs_(0u), size_(0u), Vm_(0),
 	ligandConc_(0), dt_(0)
{
	;
}

MarkovSolverBase::~MarkovSolverBase()
{
	if ( Q_ )
		delete Q_;

	if ( !expMats1d_.empty() )
	{
		while ( !expMats1d_.empty() )
		{
			delete expMats1d_.back();
			expMats1d_.pop_back();
		}
	}

	if ( !expMats2d_.empty() )
	{
		unsigned int n = expMats2d_.size();
		for( unsigned int i = 0; i < n; ++i )
		{
			for ( unsigned int j = 0; j < expMats2d_[i].size(); ++j )
				delete expMats2d_[i][j];
		}
	}

	if ( expMat_ != 0 )
		delete expMat_;
}

////////////////////////////////////
//Set-Get functions
///////////////////////////////////
Matrix MarkovSolverBase::getQ() const
{
	return *Q_;
}

Vector MarkovSolverBase::getState() const
{
	return state_;
}

Vector MarkovSolverBase::getInitialState() const
{
	return initialState_;
}

void MarkovSolverBase::setInitialState( Vector state )
{
	initialState_ = state;
	state_ = initialState_;
}

void MarkovSolverBase::setXmin( double xMin )
{
	xMin_ = xMin;
}

double MarkovSolverBase::getXmin() const
{
	return xMin_;
}

void MarkovSolverBase::setXmax( double xMax )
{
	xMax_ = xMax;
}

double MarkovSolverBase::getXmax() const
{
	return xMax_;
}

void MarkovSolverBase::setXdivs( unsigned int xDivs )
{
	xDivs_ = xDivs;
}

unsigned int MarkovSolverBase::getXdivs( ) const {
	return xDivs_;
}

double MarkovSolverBase::getInvDx() const {
	return invDx_;
}

void MarkovSolverBase::setYmin( double yMin )
{
	yMin_ = yMin;
}

double MarkovSolverBase::getYmin() const
{
	return yMin_;
}

void MarkovSolverBase::setYmax( double yMax )
{
	yMax_ = yMax;
}

double MarkovSolverBase::getYmax() const
{
	return yMax_;
}

void MarkovSolverBase::setYdivs( unsigned int yDivs )
{
	yDivs_ = yDivs;
}

unsigned int MarkovSolverBase::getYdivs( ) const
{
	return yDivs_;
}

double MarkovSolverBase::getInvDy() const
{
	return invDy_;
}

Vector* MarkovSolverBase::bilinearInterpolate( ) const
{
    bool isEndOfX = false;
    bool isEndOfY = false;

    unsigned int xIndex =
        static_cast< unsigned int >( ( Vm_ - xMin_ ) * invDx_ );
    unsigned int yIndex =
        static_cast< unsigned int >( ( ligandConc_ - yMin_ ) * invDy_ );
    double xv = (Vm_ - xMin_) * invDx_;
    double yv = (ligandConc_ - yMin_) * invDy_;

    double xF = xv - xIndex;
    double yF = yv - yIndex;
    double xFyF = xF * yF;

    ( xIndex == xDivs_ ) ? isEndOfX = true : isEndOfX = false;
    ( yIndex == yDivs_ ) ? isEndOfY = true : isEndOfY = false;

    vector< vector< Matrix* > >::const_iterator iExpQ0 =
        expMats2d_.begin() + xIndex;
    vector< Matrix* >::const_iterator iExpQ00 = iExpQ0->begin() + yIndex;
    vector< Matrix* >::const_iterator iExpQ10;

    Matrix* expQ00 = *iExpQ00;
    Matrix* expQ01;
    Matrix* expQ10;
    Matrix* expQ11;

#if ENABLE_CPP1
    // Intialization to supress compiler warning.
    Vector *state00 = nullptr;
    Vector *state01 = nullptr;
    Vector *state10 = nullptr;
    Vector *state11 = nullptr;
    Vector *result = nullptr;
#else
    Vector *state00 = NULL, *state01 = NULL
        , *state10 = NULL, *state11 = NULL
        , *result = NULL
        ;
#endif

    state00 = vecMatMul( &state_, expQ00 );
    if ( isEndOfX )
    {
        if ( isEndOfY )
            return state00;
        else
        {
            expQ01 = *(iExpQ00 + 1);
            state01 = vecMatMul( &state_, expQ01 );
            result =  vecVecScalAdd( state00, state01,
                    (1 - yF), yF );
        }
    }
    else
    {
        iExpQ10 = ( iExpQ0 + 1 )->begin() + yIndex;
        expQ10 = *iExpQ10;
        state10 = vecMatMul( &state_, expQ10 );

        if ( isEndOfY )
        {
            result =  vecVecScalAdd( state00, state10, ( 1 - xF ), xF );
        }
        else
        {
            expQ01 = *( iExpQ00 + 1 );
            expQ11 = *( iExpQ10 + 1 );

            state01 = vecMatMul( &state_, expQ01 );
            state11 = vecMatMul( &state_, expQ11 );

            Vector *temp1, *temp2;

            temp1 = vecVecScalAdd( state00, state10,
                    ( 1 - xF - yF + xFyF ),
                    ( xF - xFyF )
                    );

            temp2 = vecVecScalAdd( state01, state11, ( yF - xFyF ), xFyF );

            result = vecVecScalAdd( temp1, temp2, 1.0, 1.0 );

            delete temp1;
            delete temp2;
        }
    }

    if ( state00 )
        delete state00;
    if ( state01 )
        delete state01;
    if ( state10 )
        delete state10;
    if ( state11 )
        delete state11;

    return result;
}

Vector* MarkovSolverBase::linearInterpolate() const
{
	double x;

	if ( rateTable_->areAllRatesVoltageDep() )
		x = Vm_;
	else
		x = ligandConc_;

	if ( x < xMin_ )
		return vecMatMul( &state_, expMats1d_[0] );
	else if ( x > xMax_ )
		return vecMatMul( &state_, expMats1d_.back() );

	unsigned int xIndex = static_cast< unsigned int >( ( x - xMin_) * invDx_ );

	double xv = ( x - xMin_ ) * invDx_;
	double xF = xv - xIndex;

	vector< Matrix* >::const_iterator iExpQ =
																					expMats1d_.begin() + xIndex;

	Vector *state0, *state1, *result;

	state0 = vecMatMul( &state_, *iExpQ );
	state1 = vecMatMul( &state_, *( iExpQ + 1 ) );

	result = vecVecScalAdd( state0, state1, 1 - xF, xF );

	delete state0;
	delete state1;

	return result;
}

//Computes the updated state of the system. Is called from the process function.
//This performs state space interpolation to calculate the state of the
//channel.
//When a value of Vm_ and ligandConc_ is provided, we find the 4 matrix
//exponentials that are closest to these values, and then calculate the
//states at each of these 4 points. We then interpolate between these
//states to calculate the final one.
//In case all rates are 1D, then, the above-mentioned interpolation is
//only one-dimensional in nature.
void MarkovSolverBase::computeState( )
{
	Vector* newState;
	bool useBilinear = false;
    // useLinear = false;

	if ( rateTable_->areAnyRates2d() ||
			( rateTable_->areAllRates1d() &&
 			  rateTable_->areAnyRatesVoltageDep() &&
			  rateTable_->areAnyRatesLigandDep()
			)  )
	{
		useBilinear = true;
	}
	/*
	else if ( rateTable_->areAllRatesVoltageDep() ||
						rateTable_->areAllRatesLigandDep() )
	{
		useLinear = true;
	}
	*/

	//Heavily borrows from the Interpol2D::interpolate function.
	if ( useBilinear )
		newState = bilinearInterpolate();
	else
		newState = linearInterpolate();

	state_ = *newState;

	delete newState;
}

void MarkovSolverBase::innerFillupTable(
																		 vector< unsigned int > rateIndices,
																		 string rateType,
																		 unsigned int xIndex,
																		 unsigned int yIndex )
{
	unsigned int n = rateIndices.size(), i, j;

	for ( unsigned int k = 0; k < n; ++k )
	{
		i = ( ( rateIndices[k] / 10 ) % 10 ) - 1;
		j = ( rateIndices[k] % 10 ) - 1;

		(*Q_)[i][i] += (*Q_)[i][j];

		if ( rateType.compare("2D") == 0 )
			(*Q_)[i][j] = rateTable_->lookup2dIndex( i, j, xIndex, yIndex );
		else if ( rateType.compare("1D") == 0 )
			(*Q_)[i][j] = rateTable_->lookup1dIndex( i, j, xIndex );
		else if ( rateType.compare("constant") == 0 )
			(*Q_)[i][j] = rateTable_->lookup1dValue( i, j, 1.0 );

		(*Q_)[i][j] *= dt_;

		(*Q_)[i][i] -= (*Q_)[i][j];
	}
}

void MarkovSolverBase::fillupTable()
{
	double dx = (xMax_ - xMin_) / xDivs_;
	double dy = (yMax_ - yMin_) / yDivs_;

	vector< unsigned int > listOf1dRates = rateTable_->getListOf1dRates();
	vector< unsigned int > listOf2dRates = rateTable_->getListOf2dRates();
	vector< unsigned int > listOfConstantRates =
												 rateTable_->getListOfConstantRates();

	//Set constant rates in the Q matrix, if any.
	innerFillupTable( listOfConstantRates, "constant",
										0.0, 0.0 );

	//xIndex loops through all voltages, yIndex loops through all
	//ligand concentrations.
	if ( rateTable_->areAnyRates2d() ||
			( rateTable_->areAllRates1d() &&
 			  rateTable_->areAnyRatesVoltageDep() &&
			  rateTable_->areAnyRatesLigandDep()
			)  )
	{
		double voltage = xMin_, ligandConc = yMin_;

		for ( unsigned int xIndex = 0; xIndex < xDivs_ + 1; ++xIndex )
		{
			ligandConc = yMin_;
			for( unsigned int yIndex = 0; yIndex < yDivs_ + 1; ++yIndex )
			{
				innerFillupTable( listOf2dRates, "2D", xIndex, yIndex );

				//This is a very klutzy way of updating 1D rates as the same
				//lookup is done multiple times. But this all occurs at setup,
				//and lookups arent that slow either. This way is also easier
				//to maintain.
				innerFillupTable( listOf1dRates, "1D", xIndex, yIndex );

				expMats2d_[xIndex][yIndex] = computeMatrixExponential();
				ligandConc += dy;
			}
			voltage += dx;
		}
	}
	else if ( rateTable_->areAllRatesLigandDep() )
	{
		double x = xMin_;
		vector< unsigned int > listOfLigandRates =
													rateTable_->getListOfLigandRates();

		for ( unsigned int xIndex = 0; xIndex < xDivs_ + 1; ++xIndex )
		{
			innerFillupTable( listOfLigandRates, "1D", xIndex, 0 );
			expMats1d_[xIndex] = computeMatrixExponential();
			x += dx;
		}
	}
	else if ( rateTable_->areAllRatesVoltageDep() )
	{
		double x = xMin_;
		vector< unsigned int > listOfVoltageRates =
												 rateTable_->getListOfVoltageRates();

		for ( unsigned int xIndex = 0; xIndex < xDivs_ + 1; ++xIndex )
		{
			innerFillupTable( listOfVoltageRates, "1D", xIndex, 0 );
			expMats1d_[xIndex] = computeMatrixExponential();
			x += dx;
		}
	}
	else if ( rateTable_->areAllRatesConstant() )
	{
		expMat_ = computeMatrixExponential();
		return;
	}
}

Matrix* MarkovSolverBase::computeMatrixExponential()
{
	return 0;
}

///////////////
//MsgDest functions
//////////////

void MarkovSolverBase::reinit( const Eref& e, ProcPtr p )
{
	if ( initialState_.empty() )
	{
		cerr << "MarkovSolverBase::reinit : Initial state has not been "
						"set.\n";
		return;
	}
	state_ = initialState_;

	stateOut()->send( e, state_ );
}

void MarkovSolverBase::process( const Eref& e, ProcPtr p )
{
	computeState();

	stateOut()->send( e, state_ );
}

void MarkovSolverBase::handleVm( double Vm )
{
	Vm_ = Vm;
}

void MarkovSolverBase::handleLigandConc( double ligandConc )
{
	ligandConc_ = ligandConc;
}

//Sets up the exponential lookup tables based on the rate table that is passed
//in. Initializes the whole object.
void MarkovSolverBase::init( Id rateTableId, double dt )
{
	MarkovRateTable* rateTable = reinterpret_cast< MarkovRateTable* >(
            rateTableId.eref().data() );

	size_ = rateTable->getSize();
	rateTable_ = rateTable;
	setLookupParams( );

	if ( rateTable->areAnyRates2d() ||
			( rateTable->areAllRates1d() &&
 			  rateTable->areAnyRatesVoltageDep() &&
			  rateTable->areAnyRatesLigandDep()
			)  )
	{
		expMats2d_.resize( xDivs_ + 1 );
		for( unsigned int i = 0; i < xDivs_ + 1; ++i )
			expMats2d_[i].resize( yDivs_ + 1);
	}
	else if ( rateTable->areAllRatesLigandDep() ||
						rateTable->areAllRatesVoltageDep() )
	{
		expMats1d_.resize( xDivs_ + 1);
	}
	else	//All rates must be constant.
	{
		expMat_ = matAlloc( size_ );
	}

	//Initializing Q.
	Q_ = matAlloc( size_ );

	//The state at t = t0 + dt is exp( dt * Q ) * [state at t = t0].
	//Hence, we need to scale the terms of Q by dt.
	dt_ = dt;

	//Fills up the newly setup tables with exponentials.
	fillupTable( );
}

////////////////
//This function sets the limits of the final lookup table of matrix
//exponentials.
//xMin_, xMax, yMin_, yMax_, xDivs_, yDivs_ are chosen such that
//the longest inverval covering all the limits of the rate lookup
//tables is chosen.
//i.e. xMin_ = min( xMin of all 1D and 2D lookup tables ),
//	   xMax_ = max( xMax of all 1D and 2D lookup tables ),
//	   yMin_ = min( yMin of all 2D lookup tables ),
//	   yMax_ = max( yMax of all 2D lookup tables ),
//	   xDivs_ = min( xDivs of all 1D and 2D lookup tables ),
//		 yDivs_ = min( yDivs of all 2D lookup tables )
//
//If all the rates are constant, then all these values remain unchanged
//from the time the MarkovSolverBase object was constructed i.e. all are zero.
///////////////
void MarkovSolverBase::setLookupParams( )
{
	if ( rateTable_->areAnyRates1d() )
	{
		vector< unsigned int > listOfLigandRates
														= rateTable_->getListOfLigandRates();
		vector< unsigned int > listOfVoltageRates
														= rateTable_->getListOfVoltageRates();

		double temp;
		double yMax = DBL_MIN, yMin = DBL_MAX;
	 	unsigned int yDivs = 0u;
		unsigned int divs, i, j;

		for( unsigned int k = 0; k < listOfLigandRates.size(); ++k )
		{
			i = ( ( listOfLigandRates[k] / 10 ) % 10 ) - 1;
			j = ( listOfLigandRates[k] % 10 ) - 1;

			temp = rateTable_->getVtChildTable( i, j )->getMin();
			if ( yMin > temp )
				yMin = temp;

			temp = rateTable_->getVtChildTable( i, j )->getMax();
			if ( yMax < temp )
				yMax = temp;

			divs = rateTable_->getVtChildTable( i, j )->getDiv();
			if ( yDivs < divs )
				yDivs = divs;
		}

		if ( ( rateTable_->areAllRatesLigandDep() &&
					 rateTable_->areAllRates1d() ) )
		{
			xMin_ = yMin;
			xMax_ = yMax;
			xDivs_ = yDivs;
			invDx_ = yDivs / ( yMax - yMin );
		}
		else
		{
			yMin_= yMin;
			yMax_ = yMax;
			yDivs_ = yDivs;
			invDy_ = yDivs / ( yMax - yMin );
		}

		for( unsigned int k = 0; k < listOfVoltageRates.size(); ++k )
		{
			i = ( ( listOfVoltageRates[k] / 10 ) % 10 ) - 1;
			j = ( listOfVoltageRates[k] % 10 ) - 1;

			temp = rateTable_->getVtChildTable( i, j )->getMin();
			if ( xMin_ > temp )
				xMin_ = temp;

			temp = rateTable_->getVtChildTable( i, j )->getMax();
			if ( xMax_ < temp )
				xMax_ = temp;

			divs = rateTable_->getVtChildTable( i, j )->getDiv();
			if ( xDivs_ < divs )
				xDivs_ = divs;
		}
	}

	if ( rateTable_->areAnyRates2d() )
	{
		vector< unsigned int > listOf2dRates = rateTable_->getListOf2dRates();
		double temp;
		unsigned int divs, i, j;

		for( unsigned int k = 0; k < listOf2dRates.size(); ++k )
		{
			i = ( ( listOf2dRates[k] / 10 ) % 10 ) - 1;
			j = ( listOf2dRates[k] % 10 ) - 1;

			temp = rateTable_->getInt2dChildTable( i, j )->getXmin();
			if ( xMin_ > temp )
				xMin_ = temp;

			temp = rateTable_->getInt2dChildTable( i, j )->getXmax();
			if ( xMax_ < temp )
				xMax_ = temp;

			temp = rateTable_->getInt2dChildTable( i, j )->getYmin();
			if ( yMin_ > temp )
				yMin_ = temp;

			temp = rateTable_->getInt2dChildTable( i, j )->getYmax();
			if ( yMax_ < temp )
				yMax_ = temp;

			divs = rateTable_->getInt2dChildTable( i, j )->getXdivs();
			if ( xDivs_ < divs )
				xDivs_ = divs;

			divs = rateTable_->getInt2dChildTable( i, j )->getYdivs();
			if ( yDivs_ < divs )
				yDivs_ = divs;
		}

		invDx_ = xDivs_ / ( xMax_ - xMin_ );
		invDy_ = yDivs_ / ( yMax_ - yMin_ );
	}
}

#ifdef DO_UNIT_TESTS
void MarkovSolverBase::setVm( double Vm )
{
	Vm_ = Vm;
}

void MarkovSolverBase::setLigandConc( double ligandConc )
{
	ligandConc_ = ligandConc;
}

void setupInterpol2D( Interpol2D* table, unsigned int xDivs, double xMin,
								double xMax, unsigned int yDivs, double yMin, double yMax )
{
	table->setXdivs( xDivs );
	table->setXmin( xMin );
	table->setXmax( xMax );
	table->setYdivs( yDivs );
	table->setYmin( yMin );
	table->setYmax( yMax );
}

void setupVectorTable( VectorTable* table, unsigned int xDivs, double xMin,
												double xMax )
{
	table->setDiv( xDivs );
	table->setMin( xMin );
	table->setMax( xMax );
}

#if 0
//Simple tests on whether the exponential lookup tables are being set
//to the correct size and the lookup function tests.
//Cannot test the interpolation routines here as there is not matrix
//exponential method in the base class.
//testMarkovSolver() defined in MarkovSolver.cpp contains this test.
void testMarkovSolverBase()
{

	const Cinfo* rateTableCinfo = MarkovRateTable::initCinfo();
	const Cinfo* interpol2dCinfo = Interpol2D::initCinfo();
	const Cinfo* vectorTableCinfo = VectorTable::initCinfo();
	const Cinfo* solverBaseCinfo = MarkovSolverBase::initCinfo();

	Id vecTableId = Id::nextId();
	Id int2dId = Id::nextId();

	vector< Id > rateTableIds;
	vector< Id > solverBaseIds;

	vector< Element* > rateTableElements;
	vector< Eref* > rateTableErefs;

	vector< Element* > solverBaseElements;
	vector< Eref* > solverBaseErefs;

	vector< MarkovRateTable* > rateTables;
	vector< MarkovSolverBase* > solverBases;

	vector< unsigned int > single;
	string str;

	unsigned int numCopies = 4;

	for ( unsigned int i = 0; i < numCopies; ++i )
	{
		rateTableIds.push_back( Id::nextId() );
		str = string("ratetable") + static_cast< char >( 65 + i );
		rateTableElements.push_back( new Element( rateTableIds[i], rateTableCinfo,
                                                          str, single, 1 ) );
		rateTableErefs.push_back( new Eref( rateTableElements[i], 0 ) );
		rateTables.push_back( reinterpret_cast< MarkovRateTable* >(
															rateTableErefs[i]->data() ) );

		solverBaseIds.push_back( Id::nextId() );
		str = string("solverbase") + static_cast< char >( 65 + i );
		solverBaseElements.push_back( new Element( solverBaseIds[i], solverBaseCinfo,
																	str, single, 1 ) );
		solverBaseErefs.push_back( new Eref( solverBaseElements[i], 0 ) );
		solverBases.push_back( reinterpret_cast< MarkovSolverBase* >(
															solverBaseErefs[i]->data() ) );
	}

	Element *eInt2d = new Element( int2dId, interpol2dCinfo, "int2d", single, 1 );
	Element *eVecTable = new Element( vecTableId, vectorTableCinfo, "vecTable",
																		single, 1 );

	Interpol2D *int2dTable;
	VectorTable* vecTable;

	Eref int2dEref( eInt2d, 0 );
	int2dTable = reinterpret_cast< Interpol2D* >( int2dEref.data() );

	Eref vecTableEref( eVecTable, 0 );
	vecTable = reinterpret_cast< VectorTable* >( vecTableEref.data() );

	//////////////////////////////
	//ratetables[0]		//Only 2D rates.
	//ratetables[1]		//1D and 2D rates.
	//ratetables[2]		//Only ligand dependent rates.
	//ratetables[3]  	//Only voltage dependent rates.
	//////////////////////////////

	////////
	//Case 1 :
	//Rates (1,2) and (2,3) are ligand and voltage dependent.
	///////
	rateTables[0]->init( 3 );

	setupInterpol2D( int2dTable, 201, -0.05, 0.10, 151, 1e-9, 50e-9 );
	rateTables[0]->setInt2dChildTable( 1, 2, int2dId );

	setupInterpol2D( int2dTable, 175, -0.02, 0.19, 151, 3e-9, 75e-9 );
	rateTables[0]->setInt2dChildTable( 2, 3, int2dId );

	solverBases[0]->init( rateTableIds[0], 0.1 );

	assert( solverBases[0]->getXdivs() == 201 );
	assert( doubleEq( solverBases[0]->getXmin(), -0.05 ) );
	assert( doubleEq( solverBases[0]->getXmax(), 0.19 ) );
	assert( solverBases[0]->getYdivs() == 151 );

	//1D and 2D rates.
	rateTables[1]->init( 5 );

	///////
	//Case 2 :
	//Rates (1,2) and (1,3) are ligand and voltage dependent.
	//Rates (2,1) and (3,1) are voltage and ligand dependent respectively.
	//////
	setupInterpol2D( int2dTable, 250, -0.10, 0.75, 101, 10e-9, 25e-8 );
	rateTables[1]->setInt2dChildTable( 1, 2, int2dId );

	setupInterpol2D( int2dTable, 275, -0.05, 0.55, 141, 5e-9, 40e-7 );
	rateTables[1]->setInt2dChildTable( 1, 3, int2dId );

	//Voltage dependent.
	setupVectorTable( vecTable, 145, -0.17, 0.73 );
	rateTables[1]->setVtChildTable( 2, 1, vecTableId, 0 );

	//Ligand dependent
	setupVectorTable( vecTable, 375, 7e-9, 75e-7 );
	rateTables[1]->setVtChildTable( 3, 1, vecTableId, 1 );

	solverBases[1]->init( rateTableIds[1], 0.1 );

	assert( solverBases[1]->getXdivs() == 275 );
	assert( solverBases[1]->getYdivs() == 375 );
	assert( doubleEq( solverBases[1]->getXmin(), -0.17 ) );
	assert( doubleEq( solverBases[1]->getXmax(), 0.75 ) );
	assert( doubleEq( solverBases[1]->getYmin(), 5e-9 ) );
	assert( doubleEq( solverBases[1]->getYmax(), 75e-7 ) );

	////////
	//Case 3 : Only ligand dependent rates.
	//Rates (1, 2), (3, 5), (2, 4), (4, 1).
	///////

	rateTables[2]->init( 5 );

	setupVectorTable( vecTable, 155, 7e-9, 50e-9 );
	rateTables[2]->setVtChildTable( 1, 2, vecTableId, 1 );

	setupVectorTable( vecTable, 190, 4e-9, 35e-9 );
	rateTables[2]->setVtChildTable( 3, 5, vecTableId, 1 );

	setupVectorTable( vecTable, 120, 7e-9, 90e-9 );
	rateTables[2]->setVtChildTable( 2, 4, vecTableId, 1 );

	setupVectorTable( vecTable, 250, 10e-9, 100e-9 );
	rateTables[2]->setVtChildTable( 4, 1, vecTableId, 1 );

	solverBases[2]->init( rateTableIds[2], 0.1 );

	assert( doubleEq( 1e-308 * solverBases[2]->getYmin(), 1e-308 * DBL_MAX ) );
	assert( doubleEq( 1e308 * solverBases[2]->getYmax(), 1e308 * DBL_MIN ) );
	assert( solverBases[2]->getYdivs() == 0u );

	assert( doubleEq( solverBases[2]->getXmin(), 4e-9 ) );
	assert( doubleEq( solverBases[2]->getXmax(), 100e-9 ) );
	assert( solverBases[2]->getXdivs() == 250 );

	///////
	//Case 4 : Only voltage dependent rates.
	//Rates (3,6), (5, 6), (1, 4).
	//////

	rateTables[3]->init( 7 );

	setupVectorTable( vecTable, 100, -0.05, 0.1 );
	rateTables[3]->setVtChildTable( 3, 6, vecTableId, 1 );

	setupVectorTable( vecTable, 190, -0.15, 0.2 );
	rateTables[3]->setVtChildTable( 5, 6, vecTableId, 1 );

	setupVectorTable( vecTable, 140, -0.2, 0.1 );
	rateTables[3]->setVtChildTable( 1, 4, vecTableId, 1 );

	solverBases[3]->init( rateTableIds[3], 0.1 );

	assert( doubleEq( 1e-308 * solverBases[3]->getYmin(), 1e-308 * DBL_MAX ) );
	assert( doubleEq( 1e308 * solverBases[3]->getYmax(), 1e308 * DBL_MIN ) );
	assert( solverBases[3]->getYdivs() == 0u );

	assert( doubleEq( solverBases[3]->getXmin(), -0.2 ) );
	assert( doubleEq( solverBases[3]->getXmax(), 0.2 ) );
	assert( solverBases[3]->getXdivs() == 190 );

	for ( unsigned int i = 0; i < numCopies; ++i )
	{
		rateTableIds[i].destroy();
		solverBaseIds[i].destroy();
		delete rateTableErefs[i];
		delete solverBaseErefs[i];
	}

	int2dId.destroy();
	vecTableId.destroy();

	cout << "." << flush;
}
#endif // #if 0
#endif
