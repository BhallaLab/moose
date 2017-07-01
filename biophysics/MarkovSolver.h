/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
** Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _MARKOVSOLVER_H
#define _MARKOVSOLVER_H

/////////////////////////////////////////////////////////////
//Class : MarkovSolver
//Author : Vishaka Datta S, 2011, NCBS
//Description : Candidate algorithm for solving the system of equations
//associated with the Markov model of multistate ion channels.
//
//This implementation computes the matrix exponential using the scaling and
//squaring approach described in
//"The Scaling and Squaring Method for the Matrix Exponential Revisited", by
//Nicholas J Higham, 2005, Society for Industrial and Applied Mathematics,
//26(4), pp. 1179-1193.
//
/////////////////////////////////////////////////////////////

///////////////////////////////
//SrcFinfos
///////////////////////////////

class MarkovSolver : public MarkovSolverBase {
	public :
	MarkovSolver();

	~MarkovSolver();

	Matrix* computeMatrixExponential();

	//Scaling-and-squaring related function.
	Matrix* computePadeApproximant( Matrix*, unsigned int );

	static const Cinfo* initCinfo();
	///////////////////////////
	//MsgDest functions.
	//////////////////////////
	void reinit( const Eref&, ProcPtr );
	void process( const Eref&, ProcPtr );

	/////////////
	//Unit test
	////////////
	#ifdef DO_UNIT_TESTS
	friend void testMarkovSolver();
	#endif

	private :
};
//End of class definition.

//Matrix exponential related constants.
//Coefficients of Pade approximants for degrees 3,5,7,9,13.
static double b13[14] =
			{64764752532480000.0, 32382376266240000.0, 7771770303897600.0,
			  1187353796428800.0,   129060195264000.0,   10559470521600.0,
						670442572800.0,       33522128640.0,       1323241920.0,
								40840800.0,            960960.0,  16380.0,  182.0,  1.0};

static double b9[10] =
			{17643225600.0, 8821612800.0, 2075673600.0, 302702400.0,
				30270240.0, 2162160.0, 110880.0, 3960.0, 90.0, 1 };

static double b7[8] =
			{17297280, 8648640, 1995840, 277200, 25200, 1512, 56, 1};

static double b5[6] = {30240, 15120, 3360, 420, 30, 1};

static double b3[4] = {120, 60, 12, 1};

static double thetaM[5] = {1.495585217958292e-2, 2.539398330063230e-1,
	9.504178996162932e-1, 2.097847961257068e0, 5.371920351148152e0};

static unsigned int mCandidates[5] = {3, 5, 7, 9, 13};

#endif
