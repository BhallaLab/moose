/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _STEADYSTATE_H
#define _STEADYSTATE_H
class SteadyState
{
#ifdef DO_UNIT_TESTS
	friend void testSteadyState();
#endif
	public:
		SteadyState();
		~SteadyState();

		///////////////////////////////////////////////////
		// Field function definitions
		///////////////////////////////////////////////////
		Id getStoich() const ;
		void setStoich( Id s );
		bool badStoichiometry() const;
		bool isInitialized() const;
		unsigned int getRank() const;
		unsigned int getNumVarPools() const;
		unsigned int getNiter() const;
		unsigned int getMaxIter() const;
		void setMaxIter( unsigned int value );
		string getStatus() const;
		double getConvergenceCriterion() const;
		void setConvergenceCriterion( double value );
		double getTotal( const unsigned int i ) const;
		void setTotal( const unsigned int i, double val );
		double getEigenvalue( const unsigned int i ) const;
		void setEigenvalue( double val, const unsigned int i );
		unsigned int getStateType() const;
		unsigned int getNnegEigenvalues() const;
		unsigned int getNposEigenvalues() const;
		unsigned int getSolutionStatus() const;

		///////////////////////////////////////////////////
		// Msg Dest function definitions
		///////////////////////////////////////////////////
		void setupMatrix();
		void settleFunc();
		void resettleFunc();
		void settle( bool forceSetup );
		void showMatricesFunc();
		void showMatrices();
		void randomizeInitialCondition( const Eref& e);
		static void assignY( double* S );
		// static void randomInitFunc();
		// void randomInit();
		////////////////////////////////////////////////////
		// Utility functions for randomInit
		////////////////////////////////////////////////////
		/*
		int isLastConsvMol( int i );
		void recalcRemainingTotal(
			vector< double >& y, vector< double >& tot );
		*/
#ifdef USE_GSL
		void fitConservationRules(
			gsl_matrix* U,
			const vector< double >& eliminatedTotal,
			vector< double >&yi
		);
#endif

		////////////////////////////////////////////////////
		// funcs to handle externally imposed changes in mol N
		////////////////////////////////////////////////////
		static void setMolN( double y, unsigned int i );
		// static void assignStoichFunc( void* stoich );
		// void assignStoichFuncLocal( void* stoich );
		void classifyState( const double* T );
		static const double EPSILON;
		static const double DELTA;
		//////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();

	private:
		void setupSSmatrix();

		///////////////////////////////////////////////////
		// Internal fields.
		///////////////////////////////////////////////////
		unsigned int nIter_;
		unsigned int maxIter_;
		bool badStoichiometry_;
		string status_;
		bool isInitialized_;
		bool isSetup_;
		double convergenceCriterion_;

#ifdef USE_GSL
		gsl_matrix* LU_;
		gsl_matrix* Nr_;
		gsl_matrix* gamma_;
#endif
		Id stoich_;
		unsigned int numVarPools_;
		unsigned int nReacs_;
		unsigned int rank_;

		vector< double > total_;
		bool reassignTotal_;
		unsigned int nNegEigenvalues_;
		unsigned int nPosEigenvalues_;
		vector< double > eigenvalues_;
		unsigned int stateType_;
		unsigned int solutionStatus_;
		unsigned int numFailed_;
		VoxelPools pool_;
};

extern const Cinfo* initSteadyStateCinfo();
#endif // _STEADYSTATE_H
