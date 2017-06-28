/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#ifndef WIN32
	#include <sys/time.h>
#else
	#include <time.h>
#endif
#include <math.h>
#include <queue>
#ifdef WIN32
#include "../external/xgetopt/XGetopt.h"
#else
#include <unistd.h> // for getopt
#endif
#include "../scheduling/Clock.h"
#include "DiagonalMsg.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "../mpi/PostMaster.h"
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "../shell/Shell.h"
#ifdef MACOSX
#include <sys/sysctl.h>
#endif // MACOSX

#ifdef DO_UNIT_TESTS
extern void testSync();
extern void testAsync();
extern void testSyncArray( unsigned int size, unsigned int method );
extern void testShell();
extern void testScheduling();
extern void testSchedulingProcess();
extern void testBuiltins();
extern void testBuiltinsProcess();

extern void testMpiScheduling();
extern void testMpiBuiltins();
extern void testMpiShell();
extern void testMsg();
extern void testMpiMsg();
// extern void testKinetics();
// extern void testKineticSolvers();
// extern void	testKineticSolversProcess();
extern void testBiophysics();
extern void testBiophysicsProcess();
// extern void testHSolve();
// extern void testKineticsProcess();
// extern void testGeom();
// extern void testMesh();
// extern void testSimManager();
// extern void testSigNeur();
// extern void testSigNeurProcess();

extern unsigned int initMsgManagers();
extern void destroyMsgManagers();
// void regressionTests();
#endif
extern void speedTestMultiNodeIntFireNetwork(
	unsigned int size, unsigned int runsteps );

#ifdef USE_SMOLDYN
	extern void testSmoldyn();
#endif
// bool benchmarkTests( int argc, char** argv );

extern void mooseBenchmarks( unsigned int option );

//////////////////////////////////////////////////////////////////
// System-dependent function here
//////////////////////////////////////////////////////////////////

unsigned int getNumCores()
{
	unsigned int numCPU = 0;
#ifdef WIN_32
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	numCPU = sysinfo.dwNumberOfProcessors;
#endif

#ifdef LINUX
	numCPU = sysconf( _SC_NPROCESSORS_ONLN );
#endif

#ifdef MACOSX
	int mib[4];
	size_t len = sizeof(numCPU);

	/* set the mib for hw.ncpu */
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

	/* get the number of CPUs from the system */
	sysctl(mib, 2, &numCPU, &len, NULL, 0);

	if( numCPU < 1 )
	{
		mib[1] = HW_NCPU;
		sysctl( mib, 2, &numCPU, &len, NULL, 0 );
	}
#endif
	if ( numCPU < 1 )
	{
#ifndef QUIET_MODE
		cout << "No CPU information available. Assuming single core." << endl;
#else
#endif
		numCPU = 1;
	}
	return numCPU;
}

bool quitFlag = 0;
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void checkChildren( Id parent, const string& info )
{
	vector< Id > ret;
	Neutral::children( parent.eref(), ret );
	cout << info << " checkChildren of " <<
			parent.element()->getName() << ": " <<
		ret.size() << " children\n";
	for ( vector< Id >::iterator i = ret.begin(); i != ret.end(); ++i )
	{
		cout << i->element()->getName() << endl;
	}
}

Id init( int argc, char** argv, bool& doUnitTests, bool& doRegressionTests,
	  unsigned int& benchmark )
{
	unsigned int numCores = getNumCores();
	int numNodes = 1;
	int myNode = 0;
	bool isInfinite = 0;
	int opt;
	benchmark = 0; // Default, means don't do any benchmarks.
	Cinfo::rebuildOpIndex();
#ifdef USE_MPI
	/*
	// OpenMPI does not use argc or argv.
	// unsigned int temp_argc = 1;
	int provided;
	MPI_Init_thread( &argc, &argv, MPI_THREAD_SERIALIZED, &provided );
	*/
	MPI_Init( &argc, &argv );

	MPI_Comm_size( MPI_COMM_WORLD, &numNodes );
	MPI_Comm_rank( MPI_COMM_WORLD, &myNode );
	/*
	if ( provided < MPI_THREAD_SERIALIZED && myNode == 0 ) {
		cout << "Warning: This MPI implementation does not like multithreading: " << provided << "\n";
	}
	*/
	// myNode = MPI::COMM_WORLD.Get_rank();
#endif
	/**
	 * Here we allow the user to override the automatic identification
	 * of processor configuration
	 */
	while ( ( opt = getopt( argc, argv, "hiqurn:b:B:" ) ) != -1 ) {
		switch ( opt ) {
			case 'i' : // infinite loop, used for multinode debugging, to give gdb something to attach to.
				isInfinite = 1;
				break;
			case 'n': // Multiple nodes
			  numNodes = (unsigned int)atoi( optarg );
				break;
			case 'b': // Benchmark:
				benchmark = atoi( optarg );
				break;
			case 'B': // Benchmark plus dump data: handle later.
				break;
			case 'u': // Do unit tests, pass back.
				doUnitTests = 1;
				break;
			case 'r': // Do regression tests: pass back
				doRegressionTests = 1;
				break;
			case 'q': // quit immediately after completion.
				quitFlag = 1;
				break;
			case 'h': // help
			default:
				cout << "Usage: moose -help -infiniteLoop -unit_tests -regression_tests -quit -n numNodes -benchmark [ksolve intFire hhNet msg_<msgType>_<size>]\n";

				exit( 1 );
		}
	}
	if ( myNode == 0 )
        {
#ifndef QUIET_MODE
		cout << "on node " << myNode << ", numNodes = "
                    << numNodes << ", numCores = " << numCores << endl;
#endif
        }

	Id shellId;
	Element* shelle =
		new GlobalDataElement( shellId, Shell::initCinfo(), "root", 1 );

	Id clockId = Id::nextId();
	assert( clockId.value() == 1 );
	Id classMasterId = Id::nextId();
	Id postMasterId = Id::nextId();

	Shell* s = reinterpret_cast< Shell* >( shellId.eref().data() );
	s->setShellElement( shelle );
	s->setHardware( numCores, numNodes, myNode );
	s->loadBalance();

	/// Sets up the Elements that represent each class of Msg.
	unsigned int numMsg = Msg::initMsgManagers();

	new GlobalDataElement( clockId, Clock::initCinfo(), "clock", 1 );
	new GlobalDataElement( classMasterId, Neutral::initCinfo(), "classes", 1);
	new GlobalDataElement( postMasterId, PostMaster::initCinfo(), "postmaster", 1 );

	assert ( shellId == Id() );
	assert( clockId == Id( 1 ) );
	assert( classMasterId == Id( 2 ) );
	assert( postMasterId == Id( 3 ) );



	// s->connectMasterMsg();

	Shell::adopt( shellId, clockId, numMsg++ );
	Shell::adopt( shellId, classMasterId, numMsg++ );
	Shell::adopt( shellId, postMasterId, numMsg++ );

	assert( numMsg == 10 ); // Must be the same on all nodes.

	Cinfo::makeCinfoElements( classMasterId );


	// This will be initialized within the Process loop, and better there
	// as it flags attempts to call the Reduce operations before ProcessLoop
	// Qinfo::clearReduceQ( numCores ); // Initialize the ReduceQ entry.


	// SetGet::setShell();
	// Msg* m = new OneToOneMsg( shelle, shelle );
	// assert ( m != 0 );

	while ( isInfinite ) // busy loop for debugging under gdb and MPI.
		;

	return shellId;
}

/**
 * These tests are meant to run on individual nodes, and should
 * not invoke MPI calls. They should not be run when MPI is running.
 * These tests do not use the threaded/MPI event loop and are the most
 * basic of the set.
 */
void nonMpiTests( Shell* s )
{
#ifdef DO_UNIT_TESTS
	if ( Shell::myNode() == 0 ) {
		unsigned int numNodes = s->numNodes();
		unsigned int numCores = s->numCores();
		if ( numCores > 0 )
		s->setHardware( 1, 1, 0 );
		testAsync();
		testMsg();
		testShell();
		testScheduling();
		testBuiltins();
		// testKinetics();
		// testKineticSolvers();
		testBiophysics();
		// testHSolve();
		// testGeom();
		// testMesh();
		// testSigNeur();
#ifdef USE_SMOLDYN
		// testSmoldyn();
#endif
		s->setHardware( numCores, numNodes, 0 );
	}
#endif
}

/**
 * These tests involve the threaded/MPI process loop and are the next
 * level of tests.
 */
void processTests( Shell* s )
{
#ifdef DO_UNIT_TESTS
	testSchedulingProcess();
	testBuiltinsProcess();
	// testKineticsProcess();
	testBiophysicsProcess();
	// testKineticSolversProcess();
	// testSimManager();
	// testSigNeurProcess();
#endif
}

/**
 * These are tests that are MPI safe. They should also run
 * properly on single nodes.
 */
void mpiTests()
{
#ifdef DO_UNIT_TESTS
		testMpiMsg();
		cout << "." << flush;
		testMpiShell();
		cout << "." << flush;
		testMpiBuiltins();
		cout << "." << flush;
		testMpiScheduling();
		cout << "." << flush;
#endif
}
