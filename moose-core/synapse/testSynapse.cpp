/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifdef DO_UNIT_TESTS

#include <queue>
#include "header.h"
#include "Synapse.h"
#include "SynEvent.h"
#include "SynHandlerBase.h"
#include "SimpleSynHandler.h"
#include "RollingMatrix.h"
#include "SeqSynHandler.h"
#include "../shell/Shell.h"
#include "../randnum/randnum.h"

void testRollingMatrix()
{
	int nr = 5;
	int ncol = 10;
	RollingMatrix rm;
	rm.resize( 5, 10 );

	for ( int i = 0; i < nr; ++i ) {
		rm.sumIntoEntry( i + 1, i, i );
	}
	for ( int i = 0; i < nr; ++i ) {
		for ( int j = 0; j < ncol; ++j ) {
			assert( rm.get( i, j ) == ( i == j ) * (i+1) );
		}
	}
	cout << "." << flush;

	// Old row0 becomes row1 and so on. Old row4 (now 0) should be cleared.
	rm.rollToNextRow();
	for ( int i = 0; i < nr; ++i ) {
		for ( int j = 0; j < ncol; ++j ) {
			// cout << rm.get( i, j );
			assert( rm.get( i, j ) == ( i == j+1 ) * i );
		}
		// Here are the entries in the rm.rows_ matrix
		// 000000000
		// 100000000
		// 020000000
		// 003000000
		// 000400000
	}
	cout << "." << flush;

	vector< double > input( 10, 0.0 );
	for ( int i = 0; i < nr; ++i )
			input[i] = i + 1;

	assert( doubleEq( rm.dotProduct( input, 0, 0 ), 0.0 ) );
	assert( doubleEq( rm.dotProduct( input, 1, 0 ), 1.0 ) );
	assert( doubleEq( rm.dotProduct( input, 2, 0 ), 4.0 ) );
	assert( doubleEq( rm.dotProduct( input, 3, 0 ), 9.0 ) );
	assert( doubleEq( rm.dotProduct( input, 4, 0 ), 16.0 ) );
	assert( doubleEq( rm.dotProduct( input, 4, 1 ), 12.0 ) );
	assert( doubleEq( rm.dotProduct( input, 4, 2 ), 8.0 ) );
	assert( doubleEq( rm.dotProduct( input, 4, 3 ), 4.0 ) );
	assert( doubleEq( rm.dotProduct( input, 4, 4 ), 0.0 ) );

	rm.sumIntoRow( input, 0 );	// input == [1234500000]
	vector< double > corr;
	rm.correl( corr, input, 4 );	// rm[4] == [00040000]
	assert( doubleEq( corr[0], 16.0 ) );
	assert( doubleEq( corr[1], 12.0 ) );
	assert( doubleEq( corr[2], 8.0 ) );
	assert( doubleEq( corr[3], 4.0 ) );
	assert( doubleEq( corr[4], 0.0 ) );

	corr.assign( corr.size(), 0 );
	rm.correl( corr, input, 0 );	// rm[0] == [1234500000]
	assert( doubleEq( corr[0], 55.0 ) );
	assert( doubleEq( corr[1], 40.0 ) );
	assert( doubleEq( corr[2], 26.0 ) );
	assert( doubleEq( corr[3], 14.0 ) );
	assert( doubleEq( corr[4], 5.0 ) );

	cout << "." << flush;
}

void testSeqSynapse()
{
	int numSyn = 10;
	int kernelWidth = 5;
	SeqSynHandler ssh;
	ssh.vSetNumSynapses( numSyn );
	// for ( int i = 0; i < numSyn; ++i )
		// ssh.addSynapse();
	assert( static_cast< int >( ssh.vGetNumSynapses() ) == numSyn );
	ssh.setSeqDt( 1.0 );
	ssh.setHistoryTime( 5.0 );
	ssh.setKernelWidth( kernelWidth );
	ssh.setKernelEquation( "(x == t)*5 + ((x+1)==t || (x-1)==t) * 2 - 1" );

	vector< double > ret = ssh.getKernel();
	assert( ret.size() == static_cast< unsigned int > (5 * kernelWidth ) );
	vector< double >::iterator k = ret.begin();
	for ( int t = 0; t < 5; ++t ) {
		for ( int x = 0; x < kernelWidth; ++x ) {
			double val = (x == t)*5 + ((x+1)==t || (x-1)==t) * 2 - 1;
			assert( doubleEq( *k++, val ) );
		}
	}

	cout << "." << flush;

	ssh.setResponseScale( 1.0 );
	for ( int i = 0; i < numSyn; ++i ) {
		ssh.addSpike( i, 0.0, 1.0 );
	}
	ssh.setWeightScale( 1.0 );
	ProcInfo p;

	Eref sheller( Id().eref() );
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	Id sid = shell->doCreate( "SeqSynHandler", Id(), "sid", 1 );
	assert( sid.element()->getName() == "sid" );
	ssh.vProcess( sid.eref(), &p );

	// Here we correlate the vector [1,1,1,1,1,1,1,1,1,1,1] with
	// the kernel [4,1,-1,-1,-1]
	// Other lines are zeros.
	// Should really make the kernel mapping symmetrical.
	assert( doubleEq( ssh.getSeqActivation(), 28.0 ) );
	vector< double > wts = ssh.getWeightScaleVec();
	for ( int i = 0; i < numSyn-4; ++i )
		assert( doubleEq( wts[i], 2.0 ) );
	assert( doubleEq( wts[6], 3 ) ); // Edge effects. Last -1 vanishes.
	assert( doubleEq( wts[7], 4 ) ); // Edge effects.
	assert( doubleEq( wts[8], 5 ) ); // Edge effects.
	assert( doubleEq( wts[9], 4 ) ); // Edge effects.

	cout << "." << flush;
	shell->doDelete( sid );
}

#endif // DO_UNIT_TESTS

// This tests stuff without using the messaging.
void testSynapse()
{
#ifdef DO_UNIT_TESTS
	testRollingMatrix();
	testSeqSynapse();
#endif // DO_UNIT_TESTS
}

// This is applicable to tests that use the messaging and scheduling.
void testSynapseProcess()
{
}

