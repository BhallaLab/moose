/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef DO_UNIT_TESTS
void testHSolve()
{;}
#else

#include <vector>
#include <map>
using namespace std;

#include "HinesMatrix.h"

extern void testHinesMatrix(); // Defined in HinesMatrix.cpp
extern void testHSolvePassive(); // Defined in HSolvePassive.cpp
extern void testHSolveUtils(); // Defined in HSolveUtils.cpp
extern void runRallpackBenchmarks();                 /* Defined in RallPacks.cpp */

void testHSolve()
{
	testHSolveUtils();
	testHinesMatrix();
	testHSolvePassive();
}

//////////////////////////////////////////////////////////////////////////////
// Helper functions called in testHinesMatrix, testHSolvePassive, etc.
//////////////////////////////////////////////////////////////////////////////

void makeFullMatrix(
	const vector< TreeNodeStruct >& tree,
	double dt,
	vector< vector< double > >& matrix )
{
	unsigned int size = tree.size();

	/*
	 * Some convenience variables
	 */
	vector< double > CmByDt;
	vector< double > Ga;
	for ( unsigned int i = 0; i < tree.size(); i++ ) {
		CmByDt.push_back( tree[ i ].Cm / ( dt / 2.0 ) );
		Ga.push_back( 2.0 / tree[ i ].Ra );
	}

	/* Each entry in 'coupled' is a list of electrically coupled compartments.
	 * These compartments could be linked at junctions, or even in linear segments
	 * of the cell.
	 */
	vector< vector< unsigned int > > coupled;
	for ( unsigned int i = 0; i < tree.size(); i++ )
		if ( tree[ i ].children.size() >= 1 ) {
			coupled.push_back( tree[ i ].children );
			coupled.back().push_back( i );
		}

	matrix.clear();
	matrix.resize( size );
	for ( unsigned int i = 0; i < size; ++i )
		matrix[ i ].resize( size );

	// Setting diagonal elements
	for ( unsigned int i = 0; i < size; i++ )
		matrix[ i ][ i ] = CmByDt[ i ] + 1.0 / tree[ i ].Rm;

	double gi;
	vector< vector< unsigned int > >::iterator group;
	vector< unsigned int >::iterator ic;
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
		double gsum = 0.0;

		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];

		for ( ic = group->begin(); ic != group->end(); ++ic ) {
			gi = Ga[ *ic ];

			matrix[ *ic ][ *ic ] += gi * ( 1.0 - gi / gsum );
		}
	}

	// Setting off-diagonal elements
	double gij;
	vector< unsigned int >::iterator jc;
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
		double gsum = 0.0;

		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];

		for ( ic = group->begin(); ic != group->end() - 1; ++ic ) {
			for ( jc = ic + 1; jc != group->end(); ++jc ) {
				gij = Ga[ *ic ] * Ga[ *jc ] / gsum;

				matrix[ *ic ][ *jc ] = -gij;
				matrix[ *jc ][ *ic ] = -gij;
			}
		}
	}
}

#endif
