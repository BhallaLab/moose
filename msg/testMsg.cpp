/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../builtins/Arith.h"

#include "../shell/Shell.h"

void testAssortedMsg()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	ObjId pa = shell->doCreate( "Neutral", ObjId(), "pa", 1 );
	unsigned int numData = 5;


	///////////////////////////////////////////////////////////
	// Set up the objects.
	///////////////////////////////////////////////////////////
	Id a1 = shell->doCreate( "Arith", pa, "a1", numData );
	Id a2 = shell->doCreate( "Arith", pa, "a2", numData );

	Id b1 = shell->doCreate( "Arith", pa, "b1", numData );
	Id b2 = shell->doCreate( "Arith", pa, "b2", numData );

	Id c1 = shell->doCreate( "Arith", pa, "c1", numData );
	Id c2 = shell->doCreate( "Arith", pa, "c2", numData );

	Id d1 = shell->doCreate( "Arith", pa, "d1", numData );
	Id d2 = shell->doCreate( "Arith", pa, "d2", numData );

	Id e1 = shell->doCreate( "Arith", pa, "e1", numData );
	Id e2 = shell->doCreate( "Arith", pa, "e2", numData );

	///////////////////////////////////////////////////////////
	// Set up initial conditions
	///////////////////////////////////////////////////////////
	bool ret = 0;
	vector< double > init; // 12345
	for ( unsigned int i = 1; i < 6; ++i )
		init.push_back( i );
	ret = SetGet1< double >::setVec( a1, "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( b1, "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( c1, "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( d1, "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( e1, "arg1", init ); // 12345
	assert( ret );

	///////////////////////////////////////////////////////////
	// Set up messaging
	///////////////////////////////////////////////////////////
	// Should give 04000
	ObjId m1 = shell->doAddMsg( "Single",
		ObjId( a1, 3 ), "output", ObjId( a2, 1 ), "arg1" );
	assert( !m1.bad() );

	// Should give 33333
	ObjId m2 = shell->doAddMsg( "OneToAll",
		ObjId( b1, 2 ), "output", ObjId( b2, 0 ), "arg1" );
	assert( !m2.bad() );

	// Should give 12345
	ObjId m3 = shell->doAddMsg( "OneToOne",
		ObjId( c1, 0 ), "output", ObjId( c2, 0 ), "arg1" );
	assert( !m3.bad() );

	// Should give 01234
	ObjId m4 = shell->doAddMsg( "Diagonal",
		ObjId( d1, 0 ), "output", ObjId( d2, 0 ), "arg1" );
	assert( !m4.bad() );

	// Should give 54321
	ObjId m5 = shell->doAddMsg( "Sparse",
		ObjId( e1, 0 ), "output", ObjId( e2, 0 ), "arg1" );
	assert( !m5.bad() );

	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5, "setEntry", 0, 4, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5, "setEntry", 1, 3, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5, "setEntry", 2, 2, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5, "setEntry", 3, 1, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5, "setEntry", 4, 0, 0 );
	assert( ret );

	assert( ret );

	///////////////////////////////////////////////////////////
	// Test traversal
	///////////////////////////////////////////////////////////
	// Single
	ObjId f = Msg::getMsg( m1 )->findOtherEnd( ObjId( a1, 3 ) );
	assert( f == ObjId( a2, 1 ) );

	f = Msg::getMsg( m1 )->findOtherEnd( ObjId( a2, 1 ) );
	assert( f == ObjId( a1, 3 ) );

	f = Msg::getMsg( m1 )->findOtherEnd( ObjId( a1, 0 ) );
	assert( f.bad() );

	f = Msg::getMsg( m1 )->findOtherEnd( ObjId( a2, 0 ) );
	assert( f.bad() );

	f = Msg::getMsg( m1 )->findOtherEnd( ObjId( b2, 1 ) );
	assert( f.bad() );

	// OneToAll
	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( b1, 2 ) );
	assert( f == ObjId( b2, 0 ) );

	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( b2, 0 ) );
	assert( f == ObjId( b1, 2 ) );
	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( b2, 1 ) );
	assert( f == ObjId( b1, 2 ) );
	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( b2, 2 ) );
	assert( f == ObjId( b1, 2 ) );
	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( b2, 3 ) );
	assert( f == ObjId( b1, 2 ) );
	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( b2, 4 ) );
	assert( f == ObjId( b1, 2 ) );

	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( b1, 0 ) );
	assert( f.bad() );

	f = Msg::getMsg( m2 )->findOtherEnd( ObjId( a2, 1 ) );
	assert( f.bad() );

	// OneToOne
	for ( unsigned int i = 0; i < 5; ++i ) {
		f = Msg::getMsg( m3 )->findOtherEnd( ObjId( c1, i ) );
		assert( f == ObjId( c2, i ) );
		f = Msg::getMsg( m3 )->findOtherEnd( ObjId( c2, i ) );
		assert( f == ObjId( c1, i ) );
	}
	f = Msg::getMsg( m3 )->findOtherEnd( ObjId( a2, 1 ) );
	assert( f.bad() );

	// Diagonal
	for ( unsigned int i = 0; i < 4; ++i ) {
		f = Msg::getMsg( m4 )->findOtherEnd( ObjId( d1, i ) );
		assert( f == ObjId( d2, i + 1 ) );
		f = Msg::getMsg( m4 )->findOtherEnd( ObjId( d2, i + 1 ) );
		assert( f == ObjId( d1, i ) );
	}
	f = Msg::getMsg( m4 )->findOtherEnd( ObjId( d1, 4 ) );
	assert( f.bad() );
	f = Msg::getMsg( m4 )->findOtherEnd( ObjId( d2, 0 ) );
	assert( f.bad() );

	f = Msg::getMsg( m4 )->findOtherEnd( ObjId( a2, 1 ) );
	assert( f.bad() );

	// Sparse
	for ( unsigned int i = 0; i < 5; ++i ) {
		f = Msg::getMsg( m5 )->findOtherEnd( ObjId( e1, i ) );
		assert( f == ObjId( e2, 4 - i ) );
		f = Msg::getMsg( m5 )->findOtherEnd( ObjId( e2, i ) );
		assert( f == ObjId( e1, 4 - i ) );
	}

	f = Msg::getMsg( m5 )->findOtherEnd( ObjId( a2, 1 ) );
	assert( f.bad() );

	cout << "." << flush;

	///////////////////////////////////////////////////////////
	// Check lookup by funcId.
	///////////////////////////////////////////////////////////
	const Finfo* aFinfo = Arith::initCinfo()->findFinfo( "arg1" );
	FuncId afid = dynamic_cast< const DestFinfo* >( aFinfo )->getFid();

	ObjId m = a2.element()->findCaller( afid );
	assert ( m == m1 );
	m = b2.element()->findCaller( afid );
	assert ( m == m2 );
	m = c2.element()->findCaller( afid );
	assert ( m == m3 );
	m = d2.element()->findCaller( afid );
	assert ( m == m4 );
	m = e2.element()->findCaller( afid );
	assert ( m == m5 );

	///////////////////////////////////////////////////////////
	// Clean up.
	///////////////////////////////////////////////////////////
	shell->doDelete( pa );

	cout << "." << flush;
}

// Reported as a bug by Subha 22 Feb 2012.
void testMsgElementListing()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	unsigned int numData = 1;
	Id pa = shell->doCreate( "Neutral", Id(), "pa", numData );
	numData = 5;


	///////////////////////////////////////////////////////////
	// Set up the objects.
	///////////////////////////////////////////////////////////
	Id a1 = shell->doCreate( "Arith", pa, "a1", numData );
	Id a2 = shell->doCreate( "Arith", pa, "a2", numData );

	Id b1 = shell->doCreate( "Arith", pa, "b1", numData );
	Id b2 = shell->doCreate( "Arith", pa, "b2", numData );

	Id c1 = shell->doCreate( "Arith", pa, "c1", numData );
	Id c2 = shell->doCreate( "Arith", pa, "c2", numData );

	Id d1 = shell->doCreate( "Arith", pa, "d1", numData );
	Id d2 = shell->doCreate( "Arith", pa, "d2", numData );

	Id e1 = shell->doCreate( "Arith", pa, "e1", numData );
	Id e2 = shell->doCreate( "Arith", pa, "e2", numData );

	///////////////////////////////////////////////////////////
	// Set up messaging
	///////////////////////////////////////////////////////////
	ObjId m1 = shell->doAddMsg( "Single",
		ObjId( a1, 3 ), "output", ObjId( a2, 1 ), "arg1" );
	assert( !m1.bad() );
	ObjId m2 = shell->doAddMsg( "OneToAll",
		ObjId( b1, 2 ), "output", ObjId( b2, 0 ), "arg1" );
	assert( !m2.bad() );
	ObjId m3 = shell->doAddMsg( "OneToOne",
		ObjId( c1, 0 ), "output", ObjId( c2, 0 ), "arg1" );
	assert( !m3.bad() );
	ObjId m4 = shell->doAddMsg( "Diagonal",
		ObjId( d1, 0 ), "output", ObjId( d2, 0 ), "arg1" );
	assert( !m4.bad() );
	ObjId m5 = shell->doAddMsg( "Sparse",
		ObjId( e1, 0 ), "output", ObjId( e2, 0 ), "arg1" );
	assert( !m5.bad() );

	///////////////////////////////////////////////////////////
	// List messages
	///////////////////////////////////////////////////////////
	Id manager( "/Msgs" );
	assert( manager != Id() );
	vector< Id > children =
		Field< vector< Id > >::get( manager, "children" );
	assert( children.size() == 5 );
	assert( children[0].element()->getName() == "singleMsg" );
	assert( children[1].element()->getName() == "oneToOneMsg" );
	assert( children[2].element()->getName() == "oneToAllMsg" );
	assert( children[3].element()->getName() == "diagonalMsg" );
	assert( children[4].element()->getName() == "sparseMsg" );

	/*
	// A remarkably large number of some message classes, including 645
	// OneToAll which are used by parent-child messages. I thought they
	// were cleaned out as the tests proceed.
	for ( unsigned int i = 0; i < children.size(); ++i ) {
		cout << "\nlocalEntries[" << i << "] = " <<
			children[i].element()->dataHandler()->localEntries() << endl;
	}
	*/
	/*
	string path = children[0].path();
	cout << "\nlocalEntries = " <<
		children[0].element()->dataHandler()->localEntries() << endl;
	assert( path == "/Msgs/singleMsg[0]" );
	*/
	assert( children[0].path() == "/Msgs[0]/singleMsg" );
	assert( children[1].path() == "/Msgs[0]/oneToOneMsg" );
	assert( children[2].path() == "/Msgs[0]/oneToAllMsg" );
	assert( children[3].path() == "/Msgs[0]/diagonalMsg" );
	assert( children[4].path() == "/Msgs[0]/sparseMsg" );


	///////////////////////////////////////////////////////////
	// Next: check that the child messages have the appropriate number
	// and indices of entries.
	///////////////////////////////////////////////////////////

	shell->doDelete( pa );
	cout << "." << flush;
}

/**
 * In all cases we set up the same amount of data transfer by the msgs, that
 * is, equivalent to a fully recurrently connected network.
 * Used in regressionTests/benchmarkTests.cpp
 */
void benchmarkMsg( unsigned int n, string msgType )
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	vector< double > init( n );
	for ( unsigned int i = 0; i < n; ++i )
		init[i] = (i + 1) * 1e6;

	Id a1 = shell->doCreate( "Arith", Id(), "a1", n );

	if ( msgType == "Single" ) {
		for ( unsigned int i = 0; i < n; ++i ) {
			for ( unsigned int j = 0; j < n; ++j ) {
				ObjId m1 = shell->doAddMsg( "Single",
					ObjId( a1, i ), "output", ObjId( a1, j ), "arg3" );
				assert( !m1.bad() );
			}
		}
	} else if ( msgType == "OneToAll" ) {
		for ( unsigned int i = 0; i < n; ++i ) {
			ObjId m1 = shell->doAddMsg( "OneToAll",
				ObjId( a1, i ), "output", ObjId( a1, 0 ), "arg3" );
			assert( !m1.bad() );
		}
	} else if ( msgType == "OneToOne" ) {
		for ( unsigned int i = 0; i < n; ++i ) { // just repeat it n times
			ObjId m1 = shell->doAddMsg( "OneToOne",
				ObjId( a1, 0 ), "output", ObjId( a1, 0 ), "arg3" );
			assert( !m1.bad() );
		}
	} else if ( msgType == "Diagonal" ) {
		for ( unsigned int i = 0; i < 2 * n; ++i ) { // Set up all offsets
			ObjId m1 = shell->doAddMsg( "Diagonal",
				ObjId( a1, 0 ), "output", ObjId( a1, 0 ), "arg3" );
			Field< int >::set( m1, "stride", n - i );
		}
	} else if ( msgType == "Sparse" ) {
		ObjId m1 = shell->doAddMsg( "Sparse",
			ObjId( a1, 0 ), "output", ObjId( a1, 0 ), "arg3" );

		SetGet2< double, long >::set( m1,
			"setRandomConnectivity", 1.0, 1234 );
	}

	shell->doUseClock( "/a1", "proc", 0 );
	for ( unsigned int i = 0; i < 10; ++i )
		shell->doSetClock( i, 0 );
	shell->doSetClock( 0, 1 );
	shell->doReinit();
	SetGet1< double >::setVec( a1, "arg1", init );
	shell->doStart( 100 );
	for ( unsigned int i = 0; i < n; ++i )
		init[i] = 0; // be sure we don't retain old info.
	init.clear();
	Field< double >::getVec( a1, "outputValue", init );
	cout << endl;
	for ( unsigned int i = 0; i < n; ++i ) {
		cout << i << " " << init[i] << "	";
		if ( i % 5 == 4 )
			cout << endl;
	}

	shell->doDelete( a1 );
}

void testMsg()
{
    testAssortedMsg();
    testMsgElementListing();
}

void testMpiMsg( )
{
    cout << "." << flush;
}
