/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
** Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _MARKOVSOLVERBASE_H
#define _MARKOVSOLVERBASE_H

/////////////////////////////////////////////////////////////
//Class : MarkovSolverBase
//Author : Vishaka Datta S, 2011, NCBS
//Description : Base class for all current Markov solver(s) (and future ones,
//hopefully).
//
//After the setup of the MarkovRateTable class, where the user has entered
//the lookup tables for the various transition rates, we have enough
//information to compute all the exponential matrices that correspond to the
//solution of the kinetic equation at each time step.
//
//Before the channel simulation is run, the setup of the MarkovSolver requires
//that a table of matrix exponentials be computed and stored. In general,
//this would require a 2D lookup table where each exponential is index by
//([L],V) where [L] = ligand concentration and V = membrane voltage.
//In the case all rates are either ligand or voltage dependent, not both, a 1D
//lookup table of exponentials suffices.
//
//The above computations are achieved by going through the lookup tables
//of the MarkovRateTable class. In a general case, the number of divisions
//i.e. step sizes in each lookup table will be different. We choose the smallest
//such step size, and assume that rates with bigger step sizes stay constant
//over this time interval. By iterating over the whole range, we setup the
//exponential table.
//
//The MarkovSolverBase class handles all the bookkeeping for the table of matrix
//exponentials. It also handles all the above-mentioned interactions with the
//remaining MOOSE classes.
//
//Any MarkovSolver class that derives from this one only need implement
//a ComputeMatrixExponential() function, which handles the actual computation
//of a the matrix exponential given the Q_ matrix.
//
/////////////////////////////////////////////////////////////

///////////////////////////////
//SrcFinfos
///////////////////////////////

class MarkovSolverBase {
	public :
	MarkovSolverBase();

	virtual ~MarkovSolverBase();

	////////////////////////
	//Set-get stuff.
	///////////////////////
	Matrix getQ() const ;
	Vector getState() const;
	Vector getInitialState() const;
	void setInitialState( Vector );

	//Lookup table related stuff. Have stuck to the same naming
	//used in the Interpol2D class for simplicity.
	void setXmin( double );
	double getXmin() const;
	void setXmax( double );
	double getXmax() const;
	void setXdivs( unsigned int );
	unsigned int getXdivs() const;
	double getInvDx() const;

	void setYmin( double );
	double getYmin() const;
	void setYmax( double );
	double getYmax() const;
	void setYdivs( unsigned int );
	unsigned int getYdivs() const;
	double getInvDy() const;

	/////////////////////////
	//Lookup table related stuff.
	////////////////////////

	//Fills up lookup table of matrix exponentials.
	void innerFillupTable( vector< unsigned int >, string,
											   unsigned int, unsigned int );
	void fillupTable();

	//This returns the pointer to the exponential of the Q matrix.
	virtual Matrix* computeMatrixExponential();

	//State space interpolation routines.
	Vector* bilinearInterpolate() const;
	Vector* linearInterpolate() const;

	//Computes the updated state of the system. Is called from the process
	//function.
	void computeState();

	///////////////////////////
	//MsgDest functions.
	//////////////////////////
	void reinit( const Eref&, ProcPtr );
	void process( const Eref&, ProcPtr );

	//Handles information about Vm from the compartment.
	void handleVm( double );

	//Handles concentration information.
	void handleLigandConc( double );

	//Takes the Id of a MarkovRateTable object to initialize the table of matrix
	//exponentials.
	void init( Id, double );

	static const Cinfo* initCinfo();

	/////////////////
	//Unit test
	////////////////
	#ifdef DO_UNIT_TESTS
	friend void testMarkovSolverBase();
	#endif

	protected :
	//The instantaneous rate matrix.
	Matrix *Q_;

	#ifdef DO_UNIT_TESTS
	//Allows us to set Vm_ and ligandConc_ for the state space interpolation
	//unit test in the MarkovSolver class.
	void setVm( double );
	void setLigandConc( double );
	#endif

	private :
	//////////////
	//Helper functions.
	/////////////

	//Sets the values of xMin, xMax, xDivs, yMin, yMax, yDivs.
	void setLookupParams();

	//////////////
	//Lookup table related stuff.
	/////////////
	/*
	* Exponentials of all rate matrices that are generated over the
	* duration of the simulation. The idea is that when copies of the channel
	* are made, they will all refer this table to get the appropriate
	* exponential matrix.
	*
	* The exponential matrices are computed over a range of voltage levels
	* and/or ligand concentrations and stored in the appropriate lookup table.
	*
	* Depending on whether
	* 1) All rates are constant,
	* 2) Rates vary with only 1 parameter i.e. ligand/votage,
	* 3) Some rates are 2D i.e. vary with two parameters,
	* we store the table of exponentials in the appropriate pointers below.
	*
	* If a system contains both 2D and 1D rates, then, only the 2D pointer
	* is used.
	*/
	//Used for storing exponentials when at least one of the rates are 1D and
	//none are 2D.
	vector< Matrix* > expMats1d_;

	Matrix* expMat_;

	//Used for storing exponentials when at least one of the rates are 2D.
	vector< vector< Matrix* > > expMats2d_;

	double xMin_;
	double xMax_;
	double invDx_;
	unsigned int xDivs_;
	double yMin_;
	double yMax_;
	double invDy_;
	unsigned int yDivs_;

	////////////
	//Miscallenous stuff
	///////////

	//Pointer to the MarkovRateTable object. Necessary to glean information
	//about the properties of all the transition rates.
	MarkovRateTable* rateTable_;

	//Instantaneous state of the system.
	Vector state_;

	//Initial state of the system.
	Vector initialState_;

	//Stands for a lot of things. The dimension of the Q matrix, the number of
	//states in the rate table, etc which all happen to be the same.
	unsigned int size_;

	//Membrane voltage.
	double Vm_;
	//Ligand concentration.
	double ligandConc_;

	//Time step in simulation. The state at t = (t0 + dt) is given by
	//exp( A * dt ) * [state at t = t0 ].
	double dt_;
};
//End of class definition.
#endif
