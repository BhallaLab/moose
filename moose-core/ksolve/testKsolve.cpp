/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "../basecode/header.h"
#include "../shell/Shell.h"

#include "RateTerm.h"
#include "FuncTerm.h"
#include "../basecode/SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "../builtins/MooseParser.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"
#include "../mesh/VoxelJunction.h"

#include "../builtins/MooseParser.h"
#include "../utility/testing_macros.hpp"

/**
 * Tab controlled by table
 * A + Tab <===> B
 * A + B -sumtot--> tot1
 * 2B <===> C
 *
 * C ---e1Pool ---> D
 * D ---e2Pool ----> E
 *
 * All these are created on /kinetics, a cube compartment of vol 1e-18 m^3
 *
 */
Id makeReacTest()
{
    double simDt = 0.1;
    double plotDt = 0.1;
    Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );

    Id pools[10];
    unsigned int i = 0;
    // Make the objects.
    Id kin = s->doCreate( "CubeMesh", Id(), "kinetics", 1 );
    Id tab = s->doCreate( "StimulusTable", kin, "tab", 1 );
    Id T = pools[i++] = s->doCreate( "BufPool", kin, "T", 1 );
    Id A = pools[i++] = s->doCreate( "Pool", kin, "A", 1 );
    Id B = pools[i++] = s->doCreate( "Pool", kin, "B", 1 );
    Id C = pools[i++] = s->doCreate( "Pool", kin, "C", 1 );
    Id D = pools[i++] = s->doCreate( "Pool", kin, "D", 1 );
    Id E = pools[i++] = s->doCreate( "Pool", kin, "E", 1 );
    Id tot1 = pools[i++] = s->doCreate( "BufPool", kin, "tot1", 1 );
    Id sum = s->doCreate( "Function", tot1, "func", 1 ); // Silly that it has to have this name.
    Id sumInput( sum.value() + 1 );
    Id e1Pool = s->doCreate( "Pool", kin, "e1Pool", 1 );
    Id e2Pool = s->doCreate( "Pool", kin, "e2Pool", 1 );
    Id e1 = s->doCreate( "Enz", e1Pool, "e1", 1 );
    Id cplx = s->doCreate( "Pool", e1, "cplx", 1 );
    Id e2 = s->doCreate( "MMenz", e2Pool, "e2", 1 );
    Id r1 = s->doCreate( "Reac", kin, "r1", 1 );
    Id r2 = s->doCreate( "Reac", kin, "r2", 1 );
    Id plots = s->doCreate( "Table2", kin, "plots", 7 );

    // Connect them up
    s->doAddMsg( "Single", tab, "output", T, "setN" );

    s->doAddMsg( "Single", r1, "sub", T, "reac" );
    s->doAddMsg( "Single", r1, "sub", A, "reac" );
    s->doAddMsg( "Single", r1, "prd", B, "reac" );

    Field< unsigned int >::set( sum, "numVars", 2 );
    s->doAddMsg( "Single", A, "nOut", ObjId( sumInput, 0, 0 ), "input" );
    s->doAddMsg( "Single", B, "nOut", ObjId( sumInput, 0, 1 ), "input" );
    s->doAddMsg( "Single", sum, "valueOut", tot1, "setN" );

    s->doAddMsg( "Single", r2, "sub", B, "reac" );
    s->doAddMsg( "Single", r2, "sub", B, "reac" );
    s->doAddMsg( "Single", r2, "prd", C, "reac" );

    s->doAddMsg( "Single", e1, "sub", C, "reac" );
    s->doAddMsg( "Single", e1, "enz", e1Pool, "reac" );
    s->doAddMsg( "Single", e1, "cplx", cplx, "reac" );
    s->doAddMsg( "Single", e1, "prd", D, "reac" );

    s->doAddMsg( "Single", e2, "sub", D, "reac" );
    s->doAddMsg( "Single", e2Pool, "nOut", e2, "enzDest" );
    s->doAddMsg( "Single", e2, "prd", E, "reac" );

    // Set parameters.
    Field< double >::set( A, "concInit", 2 );
    Field< double >::set( e1Pool, "concInit", 1 );
    Field< double >::set( e2Pool, "concInit", 1 );
    Field< string >::set( sum, "expr", "x0+x1" );
    Field< double >::set( r1, "Kf", 0.2 );
    Field< double >::set( r1, "Kb", 0.1 );
    Field< double >::set( r2, "Kf", 0.1 );
    Field< double >::set( r2, "Kb", 0.0 );
    Field< double >::set( e1, "Km", 5 );
    Field< double >::set( e1, "kcat", 1 );
    Field< double >::set( e1, "ratio", 4 );
    Field< double >::set( e2, "Km", 5 );
    Field< double >::set( e2, "kcat", 1 );
    vector< double > stim( 100, 0.0 );
    double vol = Field< double >::get( kin, "volume" );

    for(unsigned int i = 0; i < 100; ++i)
        stim[i] = vol*NA*(1.0+sin(i*2.0*PI/100.0));

    Field< vector< double > >::set( tab, "vector", stim );
    Field< double >::set( tab, "stepSize", 0.0 );
    Field< double >::set( tab, "stopTime", 10.0 );
    Field< double >::set( tab, "loopTime", 10.0 );
    Field< bool >::set( tab, "doLoop", true );

    // Connect outputs
    for ( unsigned int i = 0; i < 7; ++i )
        s->doAddMsg( "Single", ObjId( plots,i),
                     "requestOut", pools[i], "getConc" );

    // Schedule it.
    for ( unsigned int i = 11; i < 18; ++i )
        s->doSetClock( i, simDt );
    s->doSetClock( 18, plotDt );
    /*
    s->doUseClock( "/kinetics/##[ISA=Reac],/kinetics/##[ISA=EnzBase],/kinetics/##[ISA=SumFunc]",
    				"process", 4 );
    s->doUseClock( "/kinetics/##[ISA=PoolBase]", "process", 5 );
    s->doUseClock( "/kinetics/##[ISA=StimulusTable]",
    				"process", 4 );
    s->doUseClock( "/kinetics/##[ISA=Table]", "process", 8 );
    s->doSetClock( 4, simDt );
    s->doSetClock( 5, simDt );
    s->doSetClock( 8, plotDt );
    */
    return kin;
}

void testSetupReac()
{
    Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
    Id kin = makeReacTest();
    s->doReinit();
    s->doStart( 20.0 );
    Id plots( "/kinetics/plots" );
    /*
    for ( unsigned int i = 0; i < 7; ++i ) {
    	stringstream ss;
    	ss << "plot." << i;
    	SetGet2< string, string >::set( ObjId( plots, i ), "xplot",
    					"tsr.plot", ss.str() );
    }
    */
    s->doDelete( kin );
    cout << "." << flush;
}

void testBuildStoich()
{
    // In the updated version, we have reordered all pools and reacs by
    // Id number, modulo varPools then bufPOols.
    // Matrix looks like:
    // Reac Name	R1	R2	e1a	e1b	e2
    // MolName
    // D			-1	0	0	0	0
    // A			-1	0	0	0	0
    // B			+1	-2	0	0	0
    // C			0	+1	-1	0	0
    // enz1			0	0	-1	+1	0
    // e1cplx		0	0	+1	-1	0
    // E			0	0	0	+1	-1
    // F			0	0	0	0	+1
    // enz2			0	0	0	0	0
    // tot1			0	0	0	0	0
    //
    // This has been shuffled to:
    // A			-1	0	0	0	0
    // B			+1	-2	0	0	0
    // C			0	+1	-1	0	0
    // E			0	0	0	+1	-1
    // F			0	0	0	0	+1
    // enz1			0	0	-1	+1	0
    // enz2			0	0	0	0	0
    // e1cplx		0	0	+1	-1	0
    // D			-1	0	0	0	0
    // tot1			0	0	0	0	0
    //
    // (This is the output of the print command on the sparse matrix.)
    //
    Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
    Id kin = makeReacTest();
    Id ksolve = s->doCreate( "Ksolve", kin, "ksolve", 1 );
    Id stoich = s->doCreate( "Stoich", ksolve, "stoich", 1 );
    Field< Id >::set( stoich, "compartment", kin );
    Field< Id >::set( stoich, "ksolve", ksolve );

    // Used to get at the stoich matrix from gdb.
#ifndef NDEBUG
    Stoich* stoichPtr = reinterpret_cast< Stoich* >( stoich.eref().data() );
#endif

    Field< string >::set( stoich, "path", "/kinetics/##" );

    unsigned int n = Field< unsigned int >::get( stoich, "numAllPools" );
    assert( n == 10 );
    unsigned int r = Field< unsigned int >::get( stoich, "numRates" );
    assert( r == 5 ); // One each for reacs and MMenz, two for Enz.

    vector< int > entries = Field< vector< int > >::get( stoich,
                            "matrixEntry" );
    vector< unsigned int > colIndex = Field< vector< unsigned int > >::get(
                                          stoich, "columnIndex" );
    vector< unsigned int > rowStart = Field< vector< unsigned int > >::get(
                                          stoich, "rowStart" );

    assert( rowStart.size() == n + 1 );
    assert( entries.size() == colIndex.size() );
    assert( entries.size() == 13 );
    assert( entries[0] == -1 );
    assert( entries[1] == 1 );
    assert( entries[2] == -2 );
    assert( entries[3] == 1 );
    assert( entries[4] == -1 );
    assert( entries[5] == 1 );
    assert( entries[6] == -1 );
    assert( entries[7] == 1 );
    assert( entries[8] == -1 );
    assert( entries[9] == 1 );
    assert( entries[10] == 1 );
    assert( entries[11] == -1 );
    assert( entries[12] == -1 );

    s->doDelete( kin );
    cout << "." << flush;
}

void testRunKsolve()
{
    double simDt = 0.1;
    // double plotDt = 0.1;
    Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
    Id kin = makeReacTest();
    Id ksolve = s->doCreate( "Ksolve", kin, "ksolve", 1 );
    Id stoich = s->doCreate( "Stoich", ksolve, "stoich", 1 );
    Field< Id >::set( stoich, "compartment", kin );
    Field< Id >::set( stoich, "ksolve", ksolve );
    Field< string >::set( stoich, "path", "/kinetics/##" );
    s->doUseClock( "/kinetics/ksolve", "process", 4 );
    s->doSetClock( 4, simDt );

    s->doReinit();
    s->doStart( 20.0 );
    Id plots( "/kinetics/plots" );
    for ( unsigned int i = 0; i < 7; ++i )
    {
        stringstream ss;
        ss << "plot." << i;
        SetGet2< string, string >::set( ObjId( plots, i ), "xplot",
                                        "tsr2.plot", ss.str() );
    }
    s->doDelete( kin );
    cout << "." << flush;
}

void testRunKsolveWithLSODA()
{
    double simDt = 0.1;
    // double plotDt = 0.1;
    Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
    Id kin = makeReacTest();

    Id ksolve = s->doCreate( "Ksolve", kin, "ksolve", 1 );
    Field< string >::set( ksolve, "method", "lsoda" );

    Id stoich = s->doCreate( "Stoich", ksolve, "stoich", 1 );
    Field< Id >::set( stoich, "compartment", kin );
    Field< Id >::set( stoich, "ksolve", ksolve );
    Field< string >::set( stoich, "path", "/kinetics/##" );
    s->doUseClock( "/kinetics/ksolve", "process", 4 );
    s->doSetClock( 4, simDt );

    s->doReinit();
    s->doStart( 20.0 );
    Id plots( "/kinetics/plots" );
    for ( unsigned int i = 0; i < 7; ++i )
    {
        stringstream ss;
        ss << "plot." << i;
        SetGet2< string, string >::set( ObjId( plots, i )
                , "xplot", "tsr2lsoda.plot", ss.str()
                );
    }
    s->doDelete( kin );
    cout << "." << flush;
}


void testRunGsolve()
{
    double simDt = 0.1;
    // double plotDt = 0.1;
    Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
    Id kin = makeReacTest();
    double volume = 1e-21;
    Field< double >::set( kin, "volume", volume );
    Field< double >::set( ObjId( "/kinetics/A" ), "concInit", 2 );
    Field< double >::set( ObjId( "/kinetics/e1Pool" ), "concInit", 1 );
    Field< double >::set( ObjId( "/kinetics/e2Pool" ), "concInit", 1 );
    Id e1( "/kinetics/e1Pool/e1" );
    Field< double >::set( e1, "Km", 5 );
    Field< double >::set( e1, "kcat", 1 );
    vector< double > stim( 100, 0.0 );
    for ( unsigned int i = 0; i< 100; ++i )
    {
        stim[i] = volume * NA * (1.0 + sin( i * 2.0 * PI / 100.0 ) );
    }
    Field< vector< double > >::set( ObjId( "/kinetics/tab" ), "vector", stim );


    Id gsolve = s->doCreate( "Gsolve", kin, "gsolve", 1 );
    Id stoich = s->doCreate( "Stoich", gsolve, "stoich", 1 );
    Field< Id >::set( stoich, "compartment", kin );
    Field< Id >::set( stoich, "ksolve", gsolve );

    Field< string >::set( stoich, "path", "/kinetics/##" );
    s->doUseClock( "/kinetics/gsolve", "process", 4 );
    s->doSetClock( 4, simDt );

    s->doReinit();
    s->doStart( 20.0 );
    Id plots( "/kinetics/plots" );
    for ( unsigned int i = 0; i < 7; ++i )
    {
        stringstream ss;
        ss << "plot." << i;
        SetGet2< string, string >::set( ObjId( plots, i ), "xplot",
                                        "tsr3.plot", ss.str() );
    }
    s->doDelete( kin );
    cout << "." << flush;
}

void testFuncTerm()
{
    FuncTerm ft;
    double args[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // First check that it doesn't die even if we forget to set up anything.
    double ans = ft(args, 2.0);
    vector< unsigned int > mol(2, 0);
    mol[0] = 2;
    mol[1] = 0;
    ft.setReactantIndex(mol);
    
    ft.setExpr("x0+x1*t");
    ans = ft(args, 10.0);
    assert(doubleEq(ans, 13.0));

    mol[0] = 0;
    mol[1] = 9;
    ft.setReactantIndex(mol);
    ans = ft(args, 2.0);
    ASSERT_EQ(21.0, ans, "testFuncTerm");
    cout << "." << flush;
}

void testKsolve()
{
    testSetupReac();
    testBuildStoich();
    testRunKsolve();
    testRunGsolve();
    testFuncTerm();
}

void testKsolveProcess()
{
    ;
}
