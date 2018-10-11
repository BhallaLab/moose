/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "testScheduling.h"
#include "Clock.h"

#include "../basecode/SparseMatrix.h"
#include "../msg/SparseMsg.h"
#include "../msg/SingleMsg.h"
#include "../builtins/Arith.h"
#include "../shell/Shell.h"


//////////////////////////////////////////////////////////////////////
// Setting up a class for testing scheduling.
//////////////////////////////////////////////////////////////////////

int TestSched::globalIndex_ = 0;
bool TestSched::isInitPending_ = true;

static DestFinfo processFinfo( "process",
	"handles process call",
	new EpFunc1< TestSched, ProcPtr >( &TestSched::process )
);

const Cinfo* TestSched::initCinfo()
{

	static Finfo* testSchedFinfos[] = {
		&processFinfo
	};

	static Dinfo< TestSched > dinfo;
	static Cinfo testSchedCinfo (
		"testSched",
		0,
		testSchedFinfos,
		sizeof ( testSchedFinfos ) / sizeof( Finfo* ),
		&dinfo
	);

	return &testSchedCinfo;
}

static const Cinfo* testSchedCinfo = TestSched::initCinfo();

void TestSched::process( const Eref& e, ProcPtr p )
{
	static const int timings[] = { 1, 2, 2, 2, 3, 3, 4, 4, 4,
		5, 5, 5, 6, 6, 6, 6, 7, 8, 8, 8, 9, 9, 10, 10, 10, 10, 10,
		11, 12, 12, 12, 12, 13, 14, 14, 14, 15, 15, 15, 15,
		16, 16, 16, 17, 18, 18, 18, 18, 19, 20, 20, 20, 20, 20,
		21, 21, 22, 22, 22, 23, 24, 24, 24, 24, 25, 25, 25 };
	// unsigned int max = sizeof( timings ) / sizeof( int );
	// cout << Shell::myNode() << " : timing[ " << index_ << "  ] = " << timings[ index_ ] << ", time = " << p->currTime << endl;
	if ( static_cast< int >( p->currTime ) != timings[ index_ ] ) {
		cout << Shell::myNode() << ":testSchedElement::process: index= " << index_ << ", currTime = " << p->currTime << endl;
	}

	assert( static_cast< int >( p->currTime ) == timings[ index_ ] );
	++index_;

	// Check that everything remains in sync
	assert( ( globalIndex_ - index_ )*( globalIndex_ - index_ ) <= 1 );
	globalIndex_ = index_;
}

//////////////////////////////////////////////////////////////////////

/**
 * Check that clock scheduling works.
 */
void testClock()
{
	const double runtime = 20.0;
	Id clock(1);

	bool ret = Field< double >::set( clock, "baseDt", 1.0);
	assert( ret );
	ret = LookupField< unsigned int, double >::set( clock, "tickDt", 0, 2.0);
	assert( ret );
	ret = LookupField< unsigned int, double >::set( clock, "tickDt", 3, 1.0);
	assert( ret );
	ret = LookupField< unsigned int, double >::set( clock, "tickDt", 1, 2.0);
	assert( ret );
	ret = LookupField< unsigned int, double >::set( clock, "tickDt", 2, 5.0);
	assert( ret );
	ret = LookupField< unsigned int, double >::set( clock, "tickDt", 4, 3.0);
	assert( ret );
	ret = LookupField< unsigned int, double >::set( clock, "tickDt", 7, 5.0);
	assert( ret );

	/*
	Id tabid = Id::nextId();
	Element* tse = new Element( tsid, testSchedCinfo, "tse", 1 );

	Eref ts( tse, 0 );

	FuncId f( processFinfo.getFid() );
	const Finfo* proc0 = clock.element()->cinfo()->findFinfo( "process0" );
	assert( proc0 );
	const SrcFinfo* sproc0 = dynamic_cast< const SrcFinfo* >( proc0 );
	assert( sproc0 );
	unsigned int b0 = sproc0->getBindIndex();
	SingleMsg *m0 = new SingleMsg( er0.eref(), ts );
	er0.element()->addMsgAndFunc( m0->mid(), f, er0.dataId.value()*2 + b0);
	SingleMsg *m1 = new SingleMsg( er1.eref(), ts );
	er1.element()->addMsgAndFunc( m1->mid(), f, er1.dataId.value()*2 + b0);
	SingleMsg *m2 = new SingleMsg( er2.eref(), ts );
	er2.element()->addMsgAndFunc( m2->mid(), f, er2.dataId.value()*2 + b0);
	SingleMsg *m3 = new SingleMsg( er3.eref(), ts );
	er3.element()->addMsgAndFunc( m3->mid(), f, er3.dataId.value()*2 + b0);
	SingleMsg *m4 = new SingleMsg( er4.eref(), ts );
	er4.element()->addMsgAndFunc( m4->mid(), f, er4.dataId.value()*2 + b0);
	SingleMsg *m5 = new SingleMsg( er5.eref(), ts );
	er5.element()->addMsgAndFunc( m5->mid(), f, er5.dataId.value()*2 + b0);



	assert( cdata->tickPtr_[0].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er3.data() ) );
	assert( cdata->tickPtr_[1].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er2.data() ) );
	assert( cdata->tickPtr_[1].mgr()->ticks_[1] == reinterpret_cast< const Tick* >( er1.data() ) );
	assert( cdata->tickPtr_[2].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er4.data() ) );
	assert( cdata->tickPtr_[3].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er0.data() ) );
	assert( cdata->tickPtr_[3].mgr()->ticks_[1] == reinterpret_cast< const Tick* >( er5.data() ) );
	*/

	Eref clocker = clock.eref();
	Clock* cdata = reinterpret_cast< Clock* >( clocker.data() );
	assert ( cdata->ticks_.size() == Clock::numTicks );
	cdata->buildTicks( clocker );
	assert( cdata->activeTicks_.size() == 0 ); // No messages

	// Now put in the scheduling tester and messages.
	Id test = Id::nextId();
	Element* teste = new GlobalDataElement( test, testSchedCinfo, "test", 1 );
	assert( teste );
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );

	shell->doAddMsg( "oneToAll", clock, "process0", test, "process" );
	shell->doAddMsg( "oneToAll", clock, "process1", test, "process" );
	shell->doAddMsg( "oneToAll", clock, "process2", test, "process" );
	shell->doAddMsg( "oneToAll", clock, "process3", test, "process" );
	shell->doAddMsg( "oneToAll", clock, "process4", test, "process" );
	shell->doAddMsg( "oneToAll", clock, "process7", test, "process" );
	// clock.element()->digestMessages();
	cdata->handleReinit( clocker );
	assert( cdata->activeTicks_.size() == 6 ); // No messages
	assert( cdata->activeTicks_[0] == 2 );
	assert( cdata->activeTicks_[1] == 2 );
	assert( cdata->activeTicks_[2] == 5 );
	assert( cdata->activeTicks_[3] == 1 );
	assert( cdata->activeTicks_[4] == 3 );
	assert( cdata->activeTicks_[5] == 5 );
	cdata->handleStart( clocker, runtime, false );
	assert( doubleEq( cdata->getCurrentTime(), runtime ) );
	test.destroy();
	for ( unsigned int i = 0; i < Clock::numTicks; ++i )
		cdata->ticks_[i] = 0;
	cdata->buildTicks( clocker );
	cout << "." << flush;
}

void testClockMessaging()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// Clock* cdata = reinterpret_cast< Clock* >( Id(1).eref().data() );
	Id syns = shell->doCreate( "SimpleSynHandler", Id(), "syns0", 10 );
	Id fire = shell->doCreate( "IntFire", Id(), "fire0", 10 );

	shell->doDelete( syns );
	shell->doDelete( fire );
	cout << "." << flush;
}

void testScheduling()
{
	testClockMessaging();
	testClock();
}

void testSchedulingProcess()
{
}

void testMpiScheduling()
{
}
