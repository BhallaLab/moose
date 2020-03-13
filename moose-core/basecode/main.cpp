/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SparseMatrix.h"

#include <time.h>
#include <math.h>
#include <queue>
#include <unistd.h> // for getopt
#include "../scheduling/Clock.h"
#include "../msg/DiagonalMsg.h"
#include "../msg/SparseMsg.h"
#include "../mpi/PostMaster.h"
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "../shell/Shell.h"
#ifdef MACOSX
#include <sys/sysctl.h>
#endif // MACOSX

#ifdef DO_UNIT_TESTS
#include <iomanip>
int testIndex = 0;
#define MOOSE_TEST(name,  f ) \
    cout << "[TEST " << setw(2) << testIndex << "] " << name; \
    testIndex += 1; \
    f; \
    cout << std::right << "  [DONE]" << endl; \

extern void testAsync();
extern void testSyncArray( unsigned int size, unsigned int method );
extern void testShell();
extern void testScheduling();
extern void testSchedulingProcess();
extern void testBuiltins();
extern void testSynapse();
extern void testBuiltinsProcess();

extern void testMpiScheduling();
extern void testMpiBuiltins();
extern void testMpiShell();
extern void testMsg();
extern void testMpiMsg();
// extern void testKinetics();
extern void testKsolve();
extern void testKsolveProcess();
extern void testBiophysics();
extern void testBiophysicsProcess();
extern void testDiffusion();
extern void testHSolve();
// extern void testKineticsProcess();
// extern void testGeom();
extern void testMesh();
// extern void testSimManager();
extern void testSigNeur();
extern void testSigNeurProcess();

extern unsigned int initMsgManagers();
extern void destroyMsgManagers();
// void regressionTests();
#endif   // DO_UNIT_TESTS

extern void speedTestMultiNodeIntFireNetwork(unsigned int size, unsigned int runsteps);

#ifdef USE_SMOLDYN
extern void testSmoldyn();
#endif
// bool benchmarkTests( int argc, char** argv );

//////////////////////////////////////////////////////////////////
// System-dependent function here
//////////////////////////////////////////////////////////////////

unsigned int getNumCores()
{
    unsigned int numCPU = 0;

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
        cout << "No CPU information available. Assuming single core." << endl;
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


Id init(int argc, char** argv, bool& doUnitTests)
{
    unsigned int numCores = getNumCores();
    int numNodes = 1;
    int myNode = 0;
    bool isInfinite = 0;
    int opt;
    Cinfo::rebuildOpIndex();

#ifdef USE_MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &numNodes );
    MPI_Comm_rank( MPI_COMM_WORLD, &myNode );
#endif

    /**
     * Here we allow the user to override the automatic identification
     * of processor configuration
     */
    while ( ( opt = getopt( argc, argv, "hiqun:b:B:" ) ) != -1 )
    {
        switch ( opt )
        {
        case 'i' : // infinite loop, used for multinode debugging, to give gdb something to attach to.
            isInfinite = 1;
            break;
        case 'n': // Multiple nodes
            numNodes = (unsigned int)atoi( optarg );
        break;
        case 'B': // Benchmark plus dump data: handle later.
            break;
        case 'u': // Do unit tests, pass back.
            doUnitTests = true;
            break;
        case 'q': // quit immediately after completion.
            quitFlag = 1;
            break;
        case 'h': // help
        default:
            cout << "Usage: moose -help -infiniteLoop -unit_tests -quit -n numNodes\n";

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

    Shell::adopt( shellId, clockId, numMsg++ );
    Shell::adopt( shellId, classMasterId, numMsg++ );
    Shell::adopt( shellId, postMasterId, numMsg++ );

    assert( numMsg == 10 ); // Must be the same on all nodes.

    Cinfo::makeCinfoElements( classMasterId );

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
    if ( Shell::myNode() == 0 )
    {
        unsigned int numNodes = s->numNodes();
        unsigned int numCores = s->numCores();
        if ( numCores > 0 )
            s->setHardware( 1, 1, 0 );
        MOOSE_TEST("testAsync", testAsync());
        MOOSE_TEST("testMsg", testMsg());
        MOOSE_TEST("testShell", testShell());
        MOOSE_TEST("testScheduling", testScheduling());
        MOOSE_TEST("testBuiltints", testBuiltins());
        //MOOSE_TEST("testKinetics", testKinetics());
        MOOSE_TEST("testKsolve", testKsolve());
        //MOOSE_TEST("testKsolveProcess", testKsolveProcess());
        MOOSE_TEST("testBiophysics", testBiophysics());
        MOOSE_TEST("testDiffusion", testDiffusion());
        MOOSE_TEST("testHsolve", testHSolve());
        //MOOSE_TEST("testGeom", testGeom());
        MOOSE_TEST("testMesh", testMesh());
        MOOSE_TEST("testSynapse", testSynapse());
        MOOSE_TEST( "testSigneur", testSigNeur());
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
    MOOSE_TEST( "testSchedulingProcess", testSchedulingProcess());
    MOOSE_TEST( "testBuiltinsProcess", testBuiltinsProcess());
    MOOSE_TEST( "testBiophysicsProcess", testBiophysicsProcess());
    MOOSE_TEST( "testSigNeurProcess", testSigNeurProcess());
#endif
}

/**
 * These are tests that are MPI safe. They should also run
 * properly on single nodes.
 */
void mpiTests()
{
#ifdef DO_UNIT_TESTS
    MOOSE_TEST( "testMpiMsg", testMpiMsg());
    MOOSE_TEST( "testMpiShell", testMpiShell());
    MOOSE_TEST( "testMpiBuiltins", testMpiBuiltins());
    MOOSE_TEST( "testMpiScheduling", testMpiScheduling());
#endif
}
#if ! defined(PYMOOSE) && ! defined(MOOSE_LIB)
int main( int argc, char** argv )
{
    bool doUnitTests = 0;
    bool doRegressionTests = 0;
    unsigned int benchmark = 0;
    // This reorders the OpFunc to Fid mapping to ensure it is node and
    // compiler independent.
    Id shellId = init(argc, argv, doUnitTests);
    // Note that the main loop remains the parser loop, though it may
    // spawn a lot of other stuff.
    Element* shelle = shellId.element();
    Shell* s = reinterpret_cast< Shell* >( shelle->data( 0 ) );
    if ( doUnitTests )
        nonMpiTests( s ); // These tests do not need the process loop.

    if ( Shell::myNode() == 0 )
    {
        if ( Shell::numNodes() > 1 )
        {
            // Use the last clock for the postmaster, so that it is called
            // after everything else has been processed and all messages
            // are ready to send out.
            s->doUseClock( "/postmaster", "process", 9 );
            s->doSetClock( 9, 1.0 ); // Use a sensible default.
        }
#ifdef DO_UNIT_TESTS
        if ( doUnitTests )
        {
            mpiTests();
            processTests( s );
        }
#endif
        // Here we set off a little event loop to poll user input.
        // It deals with the doQuit call too.
        if(! quitFlag)
            Shell::launchParser();
    }
    else
    {
        PostMaster* p = reinterpret_cast< PostMaster* >( ObjId( 3 ).data());
        while ( Shell::keepLooping() )
        {
            p->clearPending();
        }
    }
    Msg::clearAllMsgs();
    Id::clearAllElements();
#ifdef USE_MPI
    MPI_Finalize();
#endif
    return 0;
}
#endif

