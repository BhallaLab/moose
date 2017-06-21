/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/


#include "header.h"
#include "../shell/Shell.h"
#include "../randnum/randnum.h"
#include "CompartmentBase.h"
#include "../utility/testing_macros.hpp"

#include "Compartment.h"
/*
#include "HHGate.h"
#include "ChanBase.h"
#include "HHChannel.h"
*/
extern void testCompartment(); // Defined in Compartment.cpp
extern void testCompartmentProcess(); // Defined in Compartment.cpp
extern void testMarkovRateTable(); //Defined in MarkovRateTable.cpp
extern void testVectorTable();	//Defined in VectorTable.cpp

/*
extern void testSpikeGen(); // Defined in SpikeGen.cpp
extern void testCaConc(); // Defined in CaConc.cpp
extern void testNernst(); // Defined in Nernst.cpp
extern void testMarkovSolverBase();	//Defined in MarkovSolverBase.cpp
extern void testMarkovSolver();		//Defined in MarkovSolver.cpp
*/

#ifdef DO_UNIT_TESTS

// Use a larger value of runsteps when benchmarking
void testIntFireNetwork( unsigned int runsteps = 5 )
{
    static const double thresh = 0.8;
    static const double Vmax = 1.0;
    static const double refractoryPeriod = 0.4;
    static const double weightMax = 0.02;
    static const double timestep = 0.2;
    static const double delayMax = 4;
    static const double delayMin = 0;
    static const double connectionProbability = 0.1;
    static const unsigned int NUM_TOT_SYN = 104576;
    unsigned int size = 1024;
    string arg;
    Eref sheller( Id().eref() );
    Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

    Id fire = shell->doCreate( "IntFire", Id(), "network", size );
    assert( fire.element()->getName() == "network" );

    Id i2 = shell->doCreate( "SimpleSynHandler", fire, "syns", size );
    assert( i2.element()->getName() == "syns" );

    Id synId( i2.value() + 1 );
    Element* syn = synId.element();
    assert( syn->getName() == "synapse" );

    DataId di( 1 ); // DataId( data, field )
    Eref syne( syn, di );

    ObjId mid = shell->doAddMsg( "Sparse", fire, "spikeOut",
                                 ObjId( synId, 0 ), "addSpike" );

    SetGet2< double, long >::set( mid, "setRandomConnectivity",
                                  connectionProbability, 5489UL );

    mid = shell->doAddMsg( "OneToOne", i2, "activationOut",
                           fire, "activation" );
    assert( !mid.bad() );

    unsigned int nd = syn->totNumLocalField();
    if ( Shell::numNodes() == 1 )
        assert( nd == NUM_TOT_SYN );
    else if ( Shell::numNodes() == 2 )
        assert( nd == 52446 );
    else if ( Shell::numNodes() == 3 )
        //assert( nd == 34969 );
        assert( nd == 35087 );
    else if ( Shell::numNodes() == 4 )
        assert( nd == 26381 );

    //////////////////////////////////////////////////////////////////
    // Checking access to message info through SparseMsg on many nodes.
    //////////////////////////////////////////////////////////////////
    vector< ObjId > tgts;
    vector< string > funcs;
    ObjId oi( fire, 123 );
    tgts = LookupField< string, vector< ObjId > >::
           get( oi, "msgDests", "spikeOut" );
    funcs = LookupField< string, vector< string > >::
            get( oi, "msgDestFunctions", "spikeOut" );
    assert( tgts.size() == funcs.size() );
    /*
    assert( tgts.size() == 116  );
    assert( tgts[0] == ObjId( synId, 20, 11 ) );
    assert( tgts[1] == ObjId( synId, 27, 15 ) );
    assert( tgts[2] == ObjId( synId, 57, 14 ) );
    assert( tgts[90] == ObjId( synId, 788, 15 ) );
    assert( tgts[91] == ObjId( synId, 792, 12 ) );
    assert( tgts[92] == ObjId( synId, 801, 17 ) );
    */
    for ( unsigned int i = 0; i < funcs.size(); ++i )
        assert( funcs[i] == "addSpike" );

    //////////////////////////////////////////////////////////////////
    // Here we have an interesting problem. The mtRand might be called
    // by multiple threads if the above Set call is not complete.

    vector< double > origVm( size, 0.0 );
    for ( unsigned int i = 0; i < size; ++i )
        origVm[i] = mtrand() * Vmax;

    double origVm100 = origVm[100];
    double origVm900 = origVm[900];

    vector< double > temp;
    temp.clear();
    temp.resize( size, thresh );
    bool ret = Field< double >::setVec( fire, "thresh", temp );
    assert( ret );
    temp.clear();
    temp.resize( size, refractoryPeriod );
    ret = Field< double >::setVec( fire, "refractoryPeriod", temp );
    assert( ret );

    // cout << Shell::myNode() << ": fieldSize = " << fieldSize << endl;
    vector< unsigned int > numSynVec;
    Field< unsigned int >::getVec( i2, "numSynapses", numSynVec );
    assert ( numSynVec.size() == size );
    unsigned int numTotSyn = 0;
    for ( unsigned int i = 0; i < size; ++i )
        numTotSyn += numSynVec[i];
    assert( numTotSyn == NUM_TOT_SYN );

    vector< vector< double > > weight( size );
    for ( unsigned int i = 0; i < size; ++i )
    {
        weight[i].resize( numSynVec[i], 0.0 );
        vector< double > delay( numSynVec[i], 0.0 );
        for ( unsigned int j = 0; j < numSynVec[i]; ++j )
        {
            weight[i][ j ] = mtrand() * weightMax;
            delay[ j ] = delayMin + mtrand() * ( delayMax - delayMin );
        }
        ret = Field< double >::
              setVec( ObjId( synId, i ), "weight", weight[i] );
        assert( ret );
        ret = Field< double >::setVec( ObjId( synId, i ), "delay", delay );
        assert( ret );
    }

    for ( unsigned int i = 0; i < size; ++i )
    {
        vector< double > retVec(0);
        Field< double >::getVec( ObjId( synId, i ), "weight", retVec );
        assert( retVec.size() == numSynVec[i] );
        for ( unsigned int j = 0; j < numSynVec[i]; ++j )
        {
            ASSERT_DOUBLE_EQ("", retVec[j], weight[i][j] );
        }
    }

    // We have to have the SynHandlers called before the network of
    // IntFires since the 'activation' message must be delivered within
    // the same timestep.
    shell->doUseClock("/network/syns", "process", 0 );
    shell->doUseClock("/network", "process", 1 );
    shell->doSetClock( 0, timestep );
    shell->doSetClock( 1, timestep );
    shell->doSetClock( 9, timestep );
    shell->doReinit();
    ret = Field< double >::setVec( fire, "Vm", origVm );
    assert( ret );

    double retVm100 = Field< double >::get( ObjId( fire, 100 ), "Vm" );
    double retVm900 = Field< double >::get( ObjId( fire, 900 ), "Vm" );
    assert( fabs( retVm100 - origVm100 ) < 1e-6 );
    assert( fabs( retVm900 - origVm900 ) < 1e-6 );

    shell->doStart( static_cast< double >( timestep * runsteps) + 0.0 );
    if ( runsteps == 5 )   // default for unit tests, others are benchmarks
    {
        retVm100 = Field< double >::get( ObjId( fire, 100 ), "Vm" );
        double retVm101 = Field< double >::get( ObjId( fire, 101 ), "Vm" );
        double retVm102 = Field< double >::get( ObjId( fire, 102 ), "Vm" );
        double retVm99 = Field< double >::get( ObjId( fire, 99 ), "Vm" );
        retVm900 = Field< double >::get( ObjId( fire, 900 ), "Vm" );
        double retVm901 = Field< double >::get( ObjId( fire, 901 ), "Vm" );
        double retVm902 = Field< double >::get( ObjId( fire, 902 ), "Vm" );

        /*
        ASSERT_DOUBLE_EQ("", retVm100, 0.00734036 );
        ASSERT_DOUBLE_EQ("", retVm101, 0.246818 );
        ASSERT_DOUBLE_EQ("", retVm102, 0.200087 );
        ASSERT_DOUBLE_EQ("", retVm99, 0.0095779083 );
        ASSERT_DOUBLE_EQ("", retVm900, 0.1150573482 );
        ASSERT_DOUBLE_EQ("", retVm901, 0.289321534 );
        ASSERT_DOUBLE_EQ("", retVm902, 0.01011172486 );
        ASSERT_DOUBLE_EQ("", retVm100, 0.008593194687366486 );
        ASSERT_DOUBLE_EQ("", retVm101, 0.24931678857743744 );
        ASSERT_DOUBLE_EQ("", retVm102, 0.19668269662484533 );
        ASSERT_DOUBLE_EQ("", retVm99, 0.00701607616202429 );
        ASSERT_DOUBLE_EQ("", retVm900, 0.12097053045094018 );
        ASSERT_DOUBLE_EQ("", retVm901, 0.2902593120492995 );
        ASSERT_DOUBLE_EQ("", retVm902, 0.00237157280699805 );
        ASSERT_DOUBLE_EQ("", retVm100, 0.015766608829826119 );
        ASSERT_DOUBLE_EQ("", retVm101, 0.24405557875013356 );
        ASSERT_DOUBLE_EQ("", retVm102, 0.20878261213859917 );
        ASSERT_DOUBLE_EQ("", retVm99, 0.0081746848675747306 );
        ASSERT_DOUBLE_EQ("", retVm900, 0.12525297735741736 );
        ASSERT_DOUBLE_EQ("", retVm901, 0.28303358631241327 );
        ASSERT_DOUBLE_EQ("", retVm902, 0.0096374021108587178 );
        */
        ASSERT_DOUBLE_EQ("", retVm100, 0.069517018453329804 );
        ASSERT_DOUBLE_EQ("", retVm101, 0.32823493598699577 );
        ASSERT_DOUBLE_EQ("", retVm102, 0.35036493874475361 );
        ASSERT_DOUBLE_EQ("", retVm99,  0.04087358817787364 );
        ASSERT_DOUBLE_EQ("", retVm900, 0.26414663635984065 );
        ASSERT_DOUBLE_EQ("", retVm901, 0.39864519810259352 );
        ASSERT_DOUBLE_EQ("", retVm902, 0.04818717439429359 );

    }
    /*
    cout << "testIntFireNetwork: Vm100 = " << retVm100 << ", " <<
    		retVm101 << ", " << retVm102 << ", " << retVm99 <<
    		", " << Vm100 << endl;
    cout << "Vm900 = " << retVm900 << ", "<< retVm901 << ", " <<
    		retVm902 << ", " << Vm900 << endl;
    		*/

    cout << "." << flush;
    shell->doDelete( fire );
}


static const double EREST = -0.07;



#if 0
void testHHGateCreation()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    vector< int > dims( 1, 1 );
    Id nid = shell->doCreate( "Neutral", Id(), "n", dims );
    Id comptId = shell->doCreate( "Compartment", nid, "compt", dims );
    Id chanId = shell->doCreate( "HHChannel", nid, "Na", dims );
    MsgId mid = shell->doAddMsg( "Single", ObjId( comptId ), "channel",
                                 ObjId( chanId ), "channel" );
    assert( mid != Msg::bad );

    // Check gate construction and removal when powers are assigned
    vector< Id > kids;
    HHChannel* chan = reinterpret_cast< HHChannel* >(chanId.eref().data());
    assert( chan->xGate_ == 0 );
    assert( chan->yGate_ == 0 );
    assert( chan->zGate_ == 0 );
    kids = Field< vector< Id > >::get( chanId, "children" );
    assert( kids.size() == 3 );
    assert( kids[0] == Id( chanId.value() + 1 ) );
    assert( kids[1] == Id( chanId.value() + 2 ) );
    assert( kids[2] == Id( chanId.value() + 3 ) );
    assert( kids[0]()->dataHandler()->localEntries() == 0 );
    assert( kids[1]()->dataHandler()->localEntries() == 0 );
    assert( kids[2]()->dataHandler()->localEntries() == 0 );

    Field< double >::set( chanId, "Xpower", 1 );
    assert( chan->xGate_ != 0 );
    assert( chan->yGate_ == 0 );
    assert( chan->zGate_ == 0 );
    kids = Field< vector< Id > >::get( chanId, "children" );
    assert( kids.size() == 3 );
    assert( kids[0]()->dataHandler()->localEntries() == 1 );
    assert( kids[1]()->dataHandler()->localEntries() == 0 );
    assert( kids[2]()->dataHandler()->localEntries() == 0 );
    // Read the size of the gateIds.

    Field< double >::set( chanId, "Xpower", 2 );
    assert( chan->xGate_ != 0 );
    assert( chan->yGate_ == 0 );
    assert( chan->zGate_ == 0 );
    assert( kids[0]()->dataHandler()->localEntries() == 1 );
    assert( kids[1]()->dataHandler()->localEntries() == 0 );
    assert( kids[2]()->dataHandler()->localEntries() == 0 );

    Field< double >::set( chanId, "Xpower", 0 );
    assert( chan->xGate_ == 0 );
    assert( chan->yGate_ == 0 );
    assert( chan->zGate_ == 0 );
    kids = Field< vector< Id > >::get( chanId, "children" );
    assert( kids.size() == 3 );
    // Even though gate was deleted, its Id is intact.
    assert( kids[0] == Id( chanId.value() + 1 ) );
    assert( kids[0]()->dataHandler()->localEntries() == 0 );
    assert( kids[1]()->dataHandler()->localEntries() == 0 );
    assert( kids[2]()->dataHandler()->localEntries() == 0 );

    Field< double >::set( chanId, "Xpower", 2 );
    assert( chan->xGate_ != 0 );
    assert( chan->yGate_ == 0 );
    assert( chan->zGate_ == 0 );
    kids = Field< vector< Id > >::get( chanId, "children" );
    assert( kids.size() == 3 );
    // New gate was created but has orig Id
    assert( kids[0] == Id( chanId.value() + 1 ) );
    assert( kids[0]()->dataHandler()->localEntries() == 1 );
    assert( kids[1]()->dataHandler()->localEntries() == 0 );
    assert( kids[2]()->dataHandler()->localEntries() == 0 );

    Field< double >::set( chanId, "Ypower", 3 );
    assert( chan->xGate_ != 0 );
    assert( chan->yGate_ != 0 );
    assert( chan->zGate_ == 0 );
    kids = Field< vector< Id > >::get( chanId, "children" );
    assert( kids.size() == 3 );
    assert( kids[0]()->dataHandler()->localEntries() == 1 );
    assert( kids[1]()->dataHandler()->localEntries() == 1 );
    assert( kids[2]()->dataHandler()->localEntries() == 0 );

    Field< double >::set( chanId, "Zpower", 1 );
    assert( chan->xGate_ != 0 );
    assert( chan->yGate_ != 0 );
    assert( chan->zGate_ != 0 );
    kids = Field< vector< Id > >::get( chanId, "children" );
    assert( kids.size() == 3 );
    assert( kids[0] == Id( chanId.value() + 1 ) );
    assert( kids[1] == Id( chanId.value() + 2 ) );
    assert( kids[2] == Id( chanId.value() + 3 ) );
    assert( kids[0]()->dataHandler()->localEntries() == 1 );
    assert( kids[1]()->dataHandler()->localEntries() == 1 );
    assert( kids[2]()->dataHandler()->localEntries() == 1 );

    shell->doDelete( nid );
    cout << "." << flush;
}


// AP measured in millivolts wrt EREST, at a sample interval of
// 100 usec
static double actionPotl[] = { 0,
                               1.2315, 2.39872, 3.51917, 4.61015, 5.69088, 6.78432, 7.91934, 9.13413,
                               10.482, 12.0417, 13.9374, 16.3785, 19.7462, 24.7909, 33.0981, 47.967,
                               73.3833, 98.7377, 105.652, 104.663, 101.815, 97.9996, 93.5261, 88.6248,
                               83.4831, 78.2458, 73.0175, 67.8684, 62.8405, 57.9549, 53.217, 48.6206,
                               44.1488, 39.772, 35.4416, 31.0812, 26.5764, 21.7708, 16.4853, 10.6048,
                               4.30989, -1.60635, -5.965, -8.34834, -9.3682, -9.72711,
                               -9.81085, -9.78371, -9.71023, -9.61556, -9.50965, -9.39655,
                               -9.27795, -9.15458, -9.02674, -8.89458, -8.75814, -8.61746,
                               -8.47254, -8.32341, -8.17008, -8.01258, -7.85093, -7.68517,
                               -7.51537, -7.34157, -7.16384, -6.98227, -6.79695, -6.60796,
                               -6.41542, -6.21944, -6.02016, -5.81769, -5.61219, -5.40381,
                               -5.19269, -4.979, -4.76291, -4.54459, -4.32422, -4.10197,
                               -3.87804, -3.65259, -3.42582, -3.19791, -2.96904, -2.7394,
                               -2.50915, -2.27848, -2.04755, -1.81652, -1.58556, -1.3548,
                               -1.12439, -0.894457, -0.665128, -0.436511, -0.208708, 0.0181928,
                             };

// y(x) = (A + B * x) / (C + exp((x + D) / F))
// So: A = 0.1e6*(EREST+0.025), B = -0.1e6, C = -1.0, D = -(EREST+0.025)
// F = -0.01

double Na_m_A( double v )
{
    if ( fabs( EREST + 0.025 - v ) < 1e-6 )
        v += 1e-6;
    return  0.1e6 * ( EREST + 0.025 - v ) / ( exp ( ( EREST + 0.025 - v )/ 0.01 ) - 1.0 );
}

// A = 4.0e3, B = 0, C = 0.0, D = -EREST, F = 0.018
double Na_m_B( double v )
{
    return 4.0e3 * exp ( ( EREST - v ) / 0.018 );
}

// A = 70, B = 0, C = 0, D = -EREST, F = 0.02
double Na_h_A( double v )
{
    return 70.0 * exp ( ( EREST - v ) / 0.020 );
}

// A = 1e3, B = 0, C = 1.0, D = -(EREST + 0.03 ), F = -0.01
double Na_h_B( double v )
{
    return 1.0e3 / ( exp ( ( 0.030 + EREST - v )/ 0.01 )  + 1.0 );
}

// A = 1e4 * (0.01 + EREST), B = -1e4, C = -1.0, D = -(EREST + 0.01 ), F = -0.01
double K_n_A( double v )
{
    if ( fabs( EREST + 0.025 - v ) < 1e-6 )
        v += 1e-6;

    return ( 1.0e4 * ( 0.01 + EREST - v ) ) / ( exp ( ( 0.01 + EREST         - v ) / 0.01 ) - 1.0 );
}

// A = 0.125e3, B = 0, C = 0, D = -EREST ), F = 0.08
double K_n_B( double v )
{
    return 0.125e3 * exp ( (EREST - v ) / 0.08 );
}

void testHHGateLookup()
{
    Id shellId = Id();
    HHGate gate( shellId, Id( 1 ) );
    Eref er = Id(1).eref();
    Qinfo q;
    gate.setMin( er, &q, -2.0 );
    gate.setMax( er, &q, 2.0 );
    gate.setDivs( er, &q, 1 );
    assert( gate.A_.size() == 2 );
    assert( gate.B_.size() == 2 );
    assert( gate.getDivs( er, &q ) == 1 );
    ASSERT_DOUBLE_EQ("", gate.invDx_, 0.25 );
    gate.A_[0] = 0;
    gate.A_[1] = 4;
    gate.lookupByInterpolation_ = 0;
    ASSERT_DOUBLE_EQ("", gate.lookupA( -3 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -2 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -1.5 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -1 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -0.5 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 0 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 1 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 2 ), 4 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 3 ), 4 );
    gate.lookupByInterpolation_ = 1;
    ASSERT_DOUBLE_EQ("", gate.lookupA( -3 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -2 ), 0 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -1.5 ), 0.5 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -1 ), 1 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( -0.5 ), 1.5 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 0 ), 2 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 1 ), 3 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 2 ), 4 );
    ASSERT_DOUBLE_EQ("", gate.lookupA( 3 ), 4 );

    gate.B_[0] = -1;
    gate.B_[1] = 1;
    double x = 0;
    double y = 0;
    gate.lookupBoth( -3 , &x, &y );
    ASSERT_DOUBLE_EQ("", x, 0 );
    ASSERT_DOUBLE_EQ("", y, -1 );
    gate.lookupBoth( -2 , &x, &y );
    ASSERT_DOUBLE_EQ("", x, 0 );
    ASSERT_DOUBLE_EQ("", y, -1 );
    gate.lookupBoth( -0.5, &x, &y );
    ASSERT_DOUBLE_EQ("", x, 1.5 );
    ASSERT_DOUBLE_EQ("", y, -0.25 );
    gate.lookupBoth( 0, &x, &y );
    ASSERT_DOUBLE_EQ("", x, 2 );
    ASSERT_DOUBLE_EQ("", y, 0 );
    gate.lookupBoth( 1.5, &x, &y );
    ASSERT_DOUBLE_EQ("", x, 3.5 );
    ASSERT_DOUBLE_EQ("", y, 0.75 );
    gate.lookupBoth( 100000, &x, &y );
    ASSERT_DOUBLE_EQ("", x, 4 );
    ASSERT_DOUBLE_EQ("", y, 1 );

    cout << "." << flush;
}

void testHHGateSetup()
{
    Id shellId = Id();
    HHGate gate( shellId, Id( 1 ) );
    Eref er = Id(1).eref();
    Qinfo q;

    vector< double > parms;
    // Try out m-gate of NA.
// For the alpha:
// A = 0.1e6*(EREST*0.025), B = -0.1e6, C= -1, D= -(EREST+0.025), F = -0.01
// For the beta: A = 4.0e3, B = 0, C = 0.0, D = -EREST, F = 0.018
// xdivs = 100, xmin = -0.1, xmax = 0.05
    unsigned int xdivs = 100;
    double xmin = -0.1;
    double xmax = 0.05;
    parms.push_back( 0.1e6 * ( EREST + 0.025 ) );	// A alpha
    parms.push_back( -0.1e6 );				// B alpha
    parms.push_back( -1 );					// C alpha
    parms.push_back( -(EREST + 0.025 ) );	// D alpha
    parms.push_back( -0.01 );				// F alpha

    parms.push_back( 4e3 );		// A beta
    parms.push_back( 0 );		// B beta
    parms.push_back( 0 );		// C beta
    parms.push_back( -EREST );	// D beta
    parms.push_back( 0.018 );	// F beta

    parms.push_back( xdivs );
    parms.push_back( xmin );
    parms.push_back( xmax );

    gate.setupAlpha( er, &q, parms );
    assert( gate.A_.size() == xdivs + 1 );
    assert( gate.B_.size() == xdivs + 1 );

    double x = xmin;
    double dx = (xmax - xmin) / xdivs;
    for ( unsigned int i = 0; i <= xdivs; ++i )
    {
        double ma = Na_m_A( x );
        double mb = Na_m_B( x );
        ASSERT_DOUBLE_EQ("", gate.A_[i], ma );
        ASSERT_DOUBLE_EQ("", gate.B_[i], ma + mb );
        x += dx;
    }

    cout << "." << flush;
}

////////////////////////////////////////////////////////////////
// Check construction and result of HH squid simulation
////////////////////////////////////////////////////////////////

/// Returns Id of parent neutral for squid model
Id makeSquid()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    vector< int > dims( 1, 1 );
    Id nid = shell->doCreate( "Neutral", Id(), "n", dims );
    Id comptId = shell->doCreate( "Compartment", nid, "compt", dims );
    Id naId = shell->doCreate( "HHChannel", comptId, "Na", dims );
    MsgId mid = shell->doAddMsg( "Single", ObjId( comptId ), "channel",
                                 ObjId( naId ), "channel" );
    assert( mid != Msg::bad );
    Id kId = shell->doCreate( "HHChannel", comptId, "K", dims );
    mid = shell->doAddMsg( "Single", ObjId( comptId ), "channel",
                           ObjId( kId ), "channel" );
    assert( mid != Msg::bad );

    //////////////////////////////////////////////////////////////////////
    // set up compartment properties
    //////////////////////////////////////////////////////////////////////

    Field< double >::set( comptId, "Cm", 0.007854e-6 );
    Field< double >::set( comptId, "Ra", 7639.44e3 ); // does it matter?
    Field< double >::set( comptId, "Rm", 424.4e3 );
    Field< double >::set( comptId, "Em", EREST + 0.010613 );
    Field< double >::set( comptId, "inject", 0.1e-6 );
    Field< double >::set( comptId, "initVm", EREST );

    //////////////////////////////////////////////////////////////////////
    // set up Na channel properties
    //////////////////////////////////////////////////////////////////////
    Field< double >::set( naId, "Gbar", 0.94248e-3 );
    Field< double >::set( naId, "Ek", EREST + 0.115 );
    Field< double >::set( naId, "Xpower", 3.0 );
    Field< double >::set( naId, "Ypower", 1.0 );

    //////////////////////////////////////////////////////////////////////
    // set up K channel properties
    //////////////////////////////////////////////////////////////////////
    Field< double >::set( kId, "Gbar", 0.282743e-3 );
    Field< double >::set( kId, "Ek", EREST - 0.012 );
    Field< double >::set( kId, "Xpower", 4.0 );

    //////////////////////////////////////////////////////////////////////
    // set up m-gate of Na.
    //////////////////////////////////////////////////////////////////////
    vector< Id > kids; // These are the HHGates.
    kids = Field< vector< Id > >::get( naId, "children" );
    assert( kids.size() == 3 );
    vector< double > parms;
// For the alpha:
// A = 0.1e6*(EREST*0.025), B = -0.1e6, C= -1, D= -(EREST+0.025), F = -0.01
// For the beta: A = 4.0e3, B = 0, C = 0.0, D = -EREST, F = 0.018
// xdivs = 100, xmin = -0.1, xmax = 0.05
    unsigned int xdivs = 150;
    double xmin = -0.1;
    double xmax = 0.05;
    parms.push_back( 0.1e6 * ( EREST + 0.025 ) );	// A alpha
    parms.push_back( -0.1e6 );				// B alpha
    parms.push_back( -1 );					// C alpha
    parms.push_back( -(EREST + 0.025 ) );	// D alpha
    parms.push_back( -0.01 );				// F alpha

    parms.push_back( 4e3 );		// A beta
    parms.push_back( 0 );		// B beta
    parms.push_back( 0 );		// C beta
    parms.push_back( -EREST );	// D beta
    parms.push_back( 0.018 );	// F beta

    parms.push_back( xdivs );
    parms.push_back( xmin );
    parms.push_back( xmax );

    SetGet1< vector< double > >::set( kids[0], "setupAlpha", parms );
    Field< bool >::set( kids[0], "useInterpolation", 1 );

    //////////////////////////////////////////////////////////////////////
    // set up h-gate of NA.
    //////////////////////////////////////////////////////////////////////
    // Alpha rates: A = 70, B = 0, C = 0, D = -EREST, F = 0.02
    // Beta rates: A = 1e3, B = 0, C = 1.0, D = -(EREST + 0.03 ), F = -0.01
    parms.resize( 0 );
    parms.push_back( 70 );
    parms.push_back( 0 );
    parms.push_back( 0 );
    parms.push_back( -EREST );
    parms.push_back( 0.02 );

    parms.push_back( 1e3 );		// A beta
    parms.push_back( 0 );		// B beta
    parms.push_back( 1 );		// C beta
    parms.push_back( -( EREST + 0.03 ) );	// D beta
    parms.push_back( -0.01 );	// F beta

    parms.push_back( xdivs );
    parms.push_back( xmin );
    parms.push_back( xmax );

    SetGet1< vector< double > >::set( kids[1], "setupAlpha", parms );
    Field< bool >::set( kids[1], "useInterpolation", 1 );

    // Check parameters
    vector< double > A = Field< vector< double > >::get( kids[1], "tableA");
    vector< double > B = Field< vector< double > >::get( kids[1], "tableB");
    double x = xmin;
    double dx = (xmax - xmin) / xdivs;
    for ( unsigned int i = 0; i <= xdivs; ++i )
    {
        double ha = Na_h_A( x );
        double hb = Na_h_B( x );
        ASSERT_DOUBLE_EQ("", A[i], ha );
        ASSERT_DOUBLE_EQ("", B[i], ha + hb );
        x += dx;
    }

    //////////////////////////////////////////////////////////////////////
    // set up n-gate of K.
    //////////////////////////////////////////////////////////////////////
    // Alpha rates: A = 1e4 * (0.01 + EREST), B = -1e4, C = -1.0,
    //	D = -(EREST + 0.01 ), F = -0.01
    // Beta rates: 	A = 0.125e3, B = 0, C = 0, D = -EREST ), F = 0.08
    kids = Field< vector< Id > >::get( kId, "children" );
    parms.resize( 0 );
    parms.push_back(  1e4 * (0.01 + EREST) );
    parms.push_back( -1e4 );
    parms.push_back( -1.0 );
    parms.push_back( -( EREST + 0.01 ) );
    parms.push_back( -0.01 );

    parms.push_back( 0.125e3 );		// A beta
    parms.push_back( 0 );		// B beta
    parms.push_back( 0 );		// C beta
    parms.push_back( -EREST );	// D beta
    parms.push_back( 0.08 );	// F beta

    parms.push_back( xdivs );
    parms.push_back( xmin );
    parms.push_back( xmax );

    SetGet1< vector< double > >::set( kids[0], "setupAlpha", parms );
    Field< bool >::set( kids[0], "useInterpolation", 1 );

    // Check parameters
    A = Field< vector< double > >::get( kids[0], "tableA" );
    B = Field< vector< double > >::get( kids[0], "tableB" );
    x = xmin;
    for ( unsigned int i = 0; i <= xdivs; ++i )
    {
        double na = K_n_A( x );
        double nb = K_n_B( x );
        if ( i != 40 && i != 55 )
        {
            // Annoying near-misses due to different ways of handling
            // singularity. I think the method in the HHGate is better.
            ASSERT_DOUBLE_EQ("", A[i], na );
            ASSERT_DOUBLE_EQ("", B[i], na + nb );
        }
        x += dx;
    }
    return nid;
}

void testHHChannel()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    vector< int > dims( 1, 1 );

    Id nid = makeSquid();
    Id comptId( "/n/compt" );

    Id tabId = shell->doCreate( "Table", nid, "tab", dims );
    MsgId mid = shell->doAddMsg( "Single", ObjId( tabId, 0 ),
                                 "requestOut", ObjId( comptId, 0 ), "get_Vm" );
    assert( mid != Msg::bad );
    //////////////////////////////////////////////////////////////////////
    // Schedule, Reset, and run.
    //////////////////////////////////////////////////////////////////////

    shell->doSetClock( 0, 1.0e-5 );
    shell->doSetClock( 1, 1.0e-5 );
    shell->doSetClock( 2, 1.0e-5 );
    shell->doSetClock( 3, 1.0e-4 );

    shell->doUseClock( "/n/compt", "init", 0 );
    shell->doUseClock( "/n/compt", "process", 1 );
    // shell->doUseClock( "/n/compt/##", "process", 2 );
    shell->doUseClock( "/n/compt/Na,/n/compt/K", "process", 2 );
    shell->doUseClock( "/n/tab", "process", 3 );

    shell->doReinit();
    shell->doReinit();
    shell->doStart( 0.01 );

    //////////////////////////////////////////////////////////////////////
    // Check output
    //////////////////////////////////////////////////////////////////////
    vector< double > vec = Field< vector< double > >::get( tabId, "vector" );
    // assert( vec.size() == 101 );
    double delta = 0;
    for ( unsigned int i = 0; i < 100; ++i )
    {
        double ref = EREST + actionPotl[i] * 0.001;
        delta += (vec[i] - ref) * (vec[i] - ref);
    }
    assert( sqrt( delta/100 ) < 0.001 );

    ////////////////////////////////////////////////////////////////
    // Clear it all up
    ////////////////////////////////////////////////////////////////
    shell->doDelete( nid );
    cout << "." << flush;
}

#endif //#if 0
/////////////////////////////////////
// Markov Channel unit tests.
////////////////////////////////////

//Sample current obtained from channel in Chapter 20, Sakmann & Neher, Pg. 603.
//The current is sampled at intervals of 10 usec.
static double sampleCurrent[] = {0.0, // This is to handle the value sent by ChanBase on reinit
                                 0.0000000e+00, 3.0005743e-26, 1.2004594e-25, 2.7015505e-25, 4.8036751e-25, 7.5071776e-25,
                                 1.0812402e-24, 1.4719693e-24, 1.9229394e-24, 2.4341850e-24, 3.0057404e-24, 3.6376401e-24,
                                 4.3299183e-24, 5.0826095e-24, 5.8957481e-24, 6.7693684e-24, 7.7035046e-24, 8.6981913e-24,
                                 9.7534627e-24, 1.0869353e-23, 1.2045897e-23, 1.3283128e-23, 1.4581082e-23, 1.5939791e-23,
                                 1.7359292e-23, 1.8839616e-23, 2.0380801e-23, 2.1982878e-23, 2.3645883e-23, 2.5369850e-23,
                                 2.7154813e-23, 2.9000806e-23, 3.0907863e-23, 3.2876020e-23, 3.4905309e-23, 3.6995766e-23,
                                 3.9147423e-23, 4.1360317e-23, 4.3634480e-23, 4.5969946e-23, 4.8366751e-23, 5.0824928e-23,
                                 5.3344511e-23, 5.5925535e-23, 5.8568033e-23, 6.1272040e-23, 6.4037589e-23, 6.6864716e-23,
                                 6.9753453e-23, 7.2703835e-23, 7.5715897e-23, 7.8789672e-23, 8.1925194e-23, 8.5122497e-23,
                                 8.8381616e-23, 9.1702584e-23, 9.5085435e-23, 9.8530204e-23, 1.0203692e-22, 1.0560563e-22,
                                 1.0923636e-22, 1.1292913e-22, 1.1668400e-22, 1.2050099e-22, 1.2438013e-22, 1.2832146e-22,
                                 1.3232502e-22, 1.3639083e-22, 1.4051894e-22, 1.4470937e-22, 1.4896215e-22, 1.5327733e-22,
                                 1.5765494e-22, 1.6209501e-22, 1.6659757e-22, 1.7116267e-22, 1.7579032e-22, 1.8048057e-22,
                                 1.8523345e-22, 1.9004900e-22, 1.9492724e-22, 1.9986821e-22, 2.0487195e-22, 2.0993849e-22,
                                 2.1506786e-22, 2.2026010e-22, 2.2551524e-22, 2.3083331e-22, 2.3621436e-22, 2.4165840e-22,
                                 2.4716548e-22, 2.5273563e-22, 2.5836888e-22, 2.6406527e-22, 2.6982483e-22, 2.7564760e-22,
                                 2.8153360e-22, 2.8748287e-22, 2.9349545e-22, 2.9957137e-22, 3.0571067e-22
                                };

void testMarkovGslSolver()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    unsigned size = 1;

    Id nid = shell->doCreate( "Neutral", Id(), "n", size );
    Id comptId = shell->doCreate( "Compartment", nid, "compt", size );
    Id rateTabId = shell->doCreate( "MarkovRateTable", comptId, "rateTab", size );
    Id mChanId = shell->doCreate( "MarkovChannel", comptId, "mChan", size );
    Id gslSolverId = shell->doCreate( "MarkovGslSolver", comptId, "gslSolver", size );

    Id tabId = shell->doCreate( "Table", nid, "tab", size );

    ObjId mid = shell->doAddMsg( "Single", ObjId( comptId ), "channel",
                                 ObjId( mChanId ), "channel" );
    assert( !mid.bad() );

    mid = shell->doAddMsg( "Single", ObjId( comptId ), "channel",
                           ObjId( rateTabId ), "channel" );
    assert( !mid.bad() );
    mid = shell->doAddMsg( "Single", ObjId( gslSolverId ), "stateOut",
                           ObjId( mChanId ), "handleState" );
    assert( !mid.bad() );

    mid = shell->doAddMsg("Single", ObjId( rateTabId ), "instratesOut",
                          ObjId( gslSolverId ), "handleQ" );

    mid = shell->doAddMsg( "Single", ObjId( tabId, 0 ), "requestOut",
                           ObjId( mChanId, 0 ), "getIk" );
    assert( !mid.bad() );

    //////////////////////////////////////////////////////////////////////
    // set up compartment properties
    //////////////////////////////////////////////////////////////////////

    Field< double >::set( comptId, "Cm", 0.007854e-6 );
    Field< double >::set( comptId, "Ra", 7639.44e3 ); // does it matter?
    Field< double >::set( comptId, "Rm", 424.4e3 );
    Field< double >::set( comptId, "Em", -0.1 );
    Field< double >::set( comptId, "inject", 0 );
    Field< double >::set( comptId, "initVm", -0.1 );

    /////////////////////////////////
    //
    //Setup of Markov Channel.
    //This is a simple 5-state channel model taken from Chapter 20, "Single-Channel
    //Recording", Sakmann & Neher.
    //All the transition rates are constant.
    //
    ////////////////////////////////

    //Setting number of states, number of open states.
    Field< unsigned int >::set( mChanId, "numStates", 5 );
    Field< unsigned int >::set( mChanId, "numOpenStates", 2 );

    //Setting initial state of system.
    vector< double > initState;

    initState.push_back( 0.0 );
    initState.push_back( 0.0 );
    initState.push_back( 0.0 );
    initState.push_back( 0.0 );
    initState.push_back( 1.0 );

    Field< vector< double > >::set( mChanId, "initialState", initState );

    vector< string > stateLabels;

    stateLabels.push_back( "O1" );
    stateLabels.push_back( "O2" );
    stateLabels.push_back( "C1" );
    stateLabels.push_back( "C2" );
    stateLabels.push_back( "C3" );

    Field< vector< string > >::set( mChanId, "labels", stateLabels );

    vector< double > gBars;

    gBars.push_back( 40e-12 );
    gBars.push_back( 50e-12 );

    Field< vector< double > >::set( mChanId, "gbar", gBars );

    //Setting up rate tables.
    SetGet1< unsigned int >::set( rateTabId, "init", 5 );

    //Filling in values into one parameter rate table. Note that all rates here
    //are constant.
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 1, 2, 0.05 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 1, 4, 3 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 2, 1, 0.00066667 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 2, 3, 0.5 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 3, 2, 15 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 3, 4, 4 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 4, 1, 0.015 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 4, 3, 0.05 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 4, 5, 2.0 );
    SetGet3< unsigned int, unsigned int, double >::
    set( rateTabId, "setconst", 5, 4, 0.01 );

    //Setting initial state of the solver. Once this is set, the solver object
    //will send out messages containing the updated state to the channel object.
    SetGet1< vector< double > >::set( gslSolverId, "init", initState );

    shell->doSetClock( 0, 1.0e-5 );
    shell->doSetClock( 1, 1.0e-5 );
    shell->doSetClock( 2, 1.0e-5 );
    shell->doSetClock( 3, 1.0e-5 );
    shell->doSetClock( 4, 1.0e-5 );

    //Voltage is clamped to -100 mV in the example. Hence, we skip running the
    //process function.
    shell->doUseClock( "/n/compt", "init", 0 );
    shell->doUseClock( "/n/compt", "process", 1 );
    shell->doUseClock( "/n/compt/rateTab", "process", 1 );
    shell->doUseClock( "/n/compt/gslSolver", "process", 1 );
    shell->doUseClock( "/n/compt/mChan,/n/tab", "process", 2 );

    shell->doReinit( );
    shell->doReinit( );
    shell->doStart( 1.0e-3 );

    vector< double > vec = Field< vector< double > >::get( tabId, "vector" );

    for ( unsigned i = 0; i < 101; ++i )
    {
        if (!doubleEq( sampleCurrent[i] * 1e25, vec[i] * 1e25 ))
        {
            cout << "testMarkovGslSolver: sample=" << sampleCurrent[i]*1e25 << " calculated=" << vec[i]*1e25 << endl;
        }
        ASSERT_DOUBLE_EQ("", sampleCurrent[i] * 1e25, vec[i] * 1e25 );
    }
    //Currents involved here are incredibly small. Scaling them up is necessary
    //for the doubleEq function to do its job.

    shell->doDelete( nid );
    cout << "." << flush;
}

////////////////
//The testMarkovGslSolver() function includes the MarkovChannel object, but
//is a rather trivial case, in that the rates are all constant.
//This test simultaneously tests the MarkovChannel, MarkovGslSolver,
//MarkovSolverBase and MarkovSolver classes.
//This test involves simulating the 4-state NMDA channel model specified
//in the following paper :
//"Voltage Dependence of NMDA-Activated Macroscopic Conductances Predicted
//by Single-Channel Kinetics", Craig E. Jahr and Charles F. Stevens, The Journal
//of Neuroscience, 1990, 10(9), pp. 3178-3182.
//It is expected that the MarkovGslSolver and the MarkovSolver objects will
//give the same answer.
//
//Note that this is different from the NMDAChan test which involves synapses.
///////////////
void testMarkovChannel()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
    unsigned size = 1;

    Id nid = shell->doCreate( "Neutral", Id(), "n", size );

    Id gslComptId = shell->doCreate( "Compartment", nid, "gslCompt", size );
    Id exptlComptId = shell->doCreate( "Compartment", nid,
                                       "exptlCompt", size );

    Id gslRateTableId = shell->doCreate( "MarkovRateTable", gslComptId,
                                         "gslRateTable", size );
    Id exptlRateTableId = shell->doCreate( "MarkovRateTable", exptlComptId,
                                           "exptlRateTable", size );

    Id mChanGslId = shell->doCreate( "MarkovChannel", gslComptId,
                                     "mChanGsl", size );
    Id mChanExptlId = shell->doCreate( "MarkovChannel", exptlComptId,
                                       "mChanExptl", size );

    Id gslSolverId = shell->doCreate( "MarkovGslSolver", gslComptId,
                                      "gslSolver", size );
    Id exptlSolverId = shell->doCreate( "MarkovSolver", exptlComptId,
                                        "exptlSolver", size );

    Id gslTableId = shell->doCreate( "Table", nid, "gslTable", size );
    Id exptlTableId = shell->doCreate( "Table", nid, "exptlTable", size );

    Id int2dTableId = shell->doCreate( "Interpol2D", nid, "int2dTable", size );
    Id vecTableId = shell->doCreate( "VectorTable", nid, "vecTable", size );

    vector< double > table1d;
    vector< vector< double > > table2d;

    ///////////////////////////
    //Setting up the messaging.
    //////////////////////////

    //Connecting Compartment and MarkovChannel objects.
    //Compartment sends Vm to MarkovChannel object. The MarkovChannel,
    //via its ChanBase base class, sends back the conductance and current through
    //it.
    ObjId mid = shell->doAddMsg( "Single", ObjId( gslComptId ), "channel",
                                 ObjId( mChanGslId ), "channel" );
    assert( !mid.bad() );

    mid = shell->doAddMsg( "Single", ObjId( exptlComptId ), "channel",
                           ObjId( mChanExptlId ), "channel" );
    assert( !mid.bad() );

    ////////
    //Connecting up the MarkovGslSolver.
    ///////

    //Connecting Compartment and MarkovRateTable.
    //The MarkovRateTable's job is to send out the instantaneous rate matrix,
    //Q, to the solver object(s).
    //In order to do so, the MarkovRateTable object needs information on
    //Vm and ligand concentration to look up the rate from the table provided
    //by the user. Hence, the need of the connection to the Compartment object.
    //However, unlike a channel object, the MarkovRateTable object does not
    //return anything to the Compartment directly, and communicates only with the
    //solvers.
    mid = shell->doAddMsg( "Single", ObjId( gslComptId ), "channel",
                           ObjId( gslRateTableId ), "channel" );
    assert( !mid.bad() );

    //Connecting the MarkovRateTable with the MarkovGslSolver object.
    //As mentioned earlier, the MarkovRateTable object sends out information
    //about Q to the MarkovGslSolver. The MarkovGslSolver then churns out
    //the state of the system for the next time step.
    mid = shell->doAddMsg("Single", ObjId( gslRateTableId ), "instratesOut",
                          ObjId( gslSolverId ), "handleQ" );

    //Connecting MarkovGslSolver with MarkovChannel.
    //The MarkovGslSolver object, upon computing the state of the channel,
    //sends this information to the MarkovChannel object. The MarkovChannel
    //object will compute the expected conductance of the channel and send
    //this information to the compartment.
    mid = shell->doAddMsg( "Single", ObjId( gslSolverId ), "stateOut",
                           ObjId( mChanGslId ), "handleState" );
    assert( !mid.bad() );

    //////////
    //Connecting up the MarkovSolver class.
    /////////

    //Connecting the MarkovSolver and Compartment.
    //The MarkovSolver derives from the MarkovSolverBase class.
    //The base class need Vm and ligand concentration information to
    //perform lookup and interpolation on the matrix exponential lookup
    //tables.
    mid = shell->doAddMsg( "Single", ObjId( exptlComptId ), "channel",
                           ObjId( exptlSolverId ), "channel" );
    assert( !mid.bad() );

    mid = shell->doAddMsg( "Single", ObjId( exptlSolverId ), "stateOut",
                           ObjId( mChanExptlId ), "handleState" );
    assert( !mid.bad() );

    /////////
    //Connecting up the Table objects to cross-check values.
    ////////

    //Get the current values from the GSL solver based channel.
    mid = shell->doAddMsg( "Single", ObjId( gslTableId ), "requestOut",
                           ObjId( mChanGslId ), "getIk" );
    assert( !mid.bad() );

    //Get the current values from the matrix exponential solver based channel.
    mid = shell->doAddMsg( "Single", ObjId( exptlTableId ), "requestOut",
                           ObjId( mChanExptlId ), "getIk" );
    assert( !mid.bad() );

    ////////////////////
    //Compartment properties. Identical to ones used in testHHChannel()
    //barring a few modifications.
    ///////////////////

    Field< double >::set( gslComptId, "Cm", 0.007854e-6 );
    Field< double >::set( gslComptId, "Ra", 7639.44e3 ); // does it matter?
    Field< double >::set( gslComptId, "Rm", 424.4e3 );
    Field< double >::set( gslComptId, "Em", EREST + 0.1 );
    Field< double >::set( gslComptId, "inject", 0 );
    Field< double >::set( gslComptId, "initVm", EREST );

    Field< double >::set( exptlComptId, "Cm", 0.007854e-6 );
    Field< double >::set( exptlComptId, "Ra", 7639.44e3 ); // does it matter?
    Field< double >::set( exptlComptId, "Rm", 424.4e3 );
    Field< double >::set( exptlComptId, "Em", EREST + 0.1 );
    Field< double >::set( exptlComptId, "inject", 0 );
    Field< double >::set( exptlComptId, "initVm", EREST );

    //////////////////
    //Setup of rate tables.
    //Refer paper mentioned at the header of the unit test for more
    //details.
    /////////////////

    //Number of states and open states.
    Field< unsigned int >::set( mChanGslId, "numStates", 4 );
    Field< unsigned int >::set( mChanExptlId, "numStates", 4 );

    Field< unsigned int >::set( mChanGslId, "numOpenStates", 1 );
    Field< unsigned int >::set( mChanExptlId, "numOpenStates", 1 );

    vector< string > stateLabels;

    //In the MarkovChannel class, the opening states are listed first.
    //This is in line with the convention followed in Chapter 20, Sakmann &
    //Neher.
    stateLabels.push_back( "O" );		//State 1.
    stateLabels.push_back( "B1" );	//State 2.
    stateLabels.push_back( "B2" );	//State 3.
    stateLabels.push_back( "C" ); 	//State 4.

    Field< vector< string > >::set( mChanGslId, "labels", stateLabels );
    Field< vector< string > >::set( mChanExptlId, "labels", stateLabels );

    //Setting up conductance value for single open state.	Value chosen
    //is quite arbitrary.
    vector< double > gBar;

    gBar.push_back( 5.431553e-9 );

    Field< vector< double > >::set( mChanGslId, "gbar", gBar );
    Field< vector< double > >::set( mChanExptlId, "gbar", gBar );

    //Initial state of the system. This is really an arbitrary choice.
    vector< double > initState;

    initState.push_back( 0.00 );
    initState.push_back( 0.20 );
    initState.push_back( 0.80 );
    initState.push_back( 0.00 );

    Field< vector< double > >::set( mChanGslId, "initialState", initState );
    Field< vector< double > >::set( mChanExptlId, "initialState", initState );

    //This initializes the GSL solver object.
    SetGet1< vector< double > >::set( gslSolverId, "init", initState );

    //Initializing MarkovRateTable object.
    double v;
    double conc;

    SetGet1< unsigned int >::set( gslRateTableId, "init", 4 );
    SetGet1< unsigned int >::set( exptlRateTableId, "init", 4 );

    //Setting up lookup tables for the different rates.
    //Please note that the rates should be in sec^(-1).

    //Transition from "O" to "B1" i.e. r12 or a1.
    Field< double >::set( vecTableId, "xmin", -0.10 );
    Field< double >::set( vecTableId, "xmax", 0.10 );
    Field< unsigned int >::set( vecTableId, "xdivs", 200 );

    v = Field< double >::get( vecTableId, "xmin" );
    for ( unsigned int i = 0; i < 201; ++i )
    {
        table1d.push_back( 1e3 * exp( -16 * v - 2.91 ) );
        v += 0.001;
    }

    Field< vector< double > >::set( vecTableId, "table", table1d );

    SetGet4< unsigned int, unsigned int, Id, unsigned int >::set(
        gslRateTableId, "set1d", 1, 2, vecTableId, 0 );
    SetGet4< unsigned int, unsigned int, Id, unsigned int >::set(
        exptlRateTableId, "set1d", 1, 2, vecTableId, 0 );

    table1d.erase( table1d.begin(), table1d.end() );

    //Transition from "B1" back to O i.e. r21 or b1
    v = Field< double >::get( vecTableId, "xmin" );
    for ( unsigned int i = 0; i < 201; ++i )
    {
        table1d.push_back( 1e3 * exp( 9 * v + 1.22 ) );
        v += 0.001;
    }

    Field< vector< double > >::set( vecTableId, "table", table1d );
    SetGet4< unsigned int, unsigned int, Id, unsigned int >::set(
        gslRateTableId, "set1d", 2, 1, vecTableId, 0 );
    SetGet4< unsigned int, unsigned int, Id, unsigned int >::set(
        exptlRateTableId, "set1d", 2, 1, vecTableId, 0 );

    table1d.erase( table1d.begin(), table1d.end() );

    //Transition from "O" to "B2" i.e. r13 or a2
    //This is actually a 2D rate. But, there is no change in Mg2+ concentration
    //that occurs. Hence, I create a 2D lookup table anyway but I manually
    //set the concentration on the rate table object anyway.

    Field< double >::set( gslRateTableId, "ligandConc", 24e-6 );
    Field< double >::set( exptlRateTableId, "ligandConc", 24e-6 );

    Field< double >::set( int2dTableId, "xmin", -0.10 );
    Field< double >::set( int2dTableId, "xmax", 0.10 );
    Field< double >::set( int2dTableId, "ymin", 0 );
    Field< double >::set( int2dTableId, "ymax", 30e-6 );
    Field< unsigned int >::set( int2dTableId, "xdivs", 200 );
    Field< unsigned int >::set( int2dTableId, "ydivs", 30 );

    table2d.resize( 201 );
    v = Field< double >::get( int2dTableId, "xmin" );
    for ( unsigned int i = 0; i < 201; ++i )
    {
        conc = Field< double >::get( int2dTableId, "ymin" );
        for ( unsigned int j = 0; j < 31; ++j )
        {
            table2d[i].push_back( 1e3 * conc * exp( -45 * v - 6.97 ) );
            conc += 1e-6;
        }
        v += 1e-3;
    }

    Field< vector< vector< double > > >::set( int2dTableId, "tableVector2D",
            table2d );

    SetGet3< unsigned int, unsigned int, Id >::set( gslRateTableId,
            "set2d", 1, 3, int2dTableId );
    SetGet3< unsigned int, unsigned int, Id >::set( exptlRateTableId,
            "set2d", 1, 3, int2dTableId );

    //There is only one 2D rate, so no point manually erasing the elements.

    //Transition from "B2" to "O" i.e. r31 or b2
    v = -0.10;
    for ( unsigned int i = 0; i < 201; ++i )
    {
        table1d.push_back( 1e3 * exp( 17 * v + 0.96 ) );
        v += 0.001;
    }

    Field< vector< double > >::set( vecTableId, "table", table1d );
    SetGet4< unsigned int, unsigned int, Id, unsigned int >::set(
        gslRateTableId, "set1d", 3, 1, vecTableId, 0 );
    SetGet4< unsigned int, unsigned int, Id, unsigned int >::set(
        exptlRateTableId, "set1d", 3, 1, vecTableId, 0 );

    table1d.erase( table1d.begin(), table1d.end() );

    //Transition from "O" to "C" i.e. r14
    SetGet3< unsigned int, unsigned int, double >::set( gslRateTableId,
            "setconst", 1, 4, 1e3 * exp( -2.847 ) );
    SetGet3< unsigned int, unsigned int, double >::set( exptlRateTableId,
            "setconst", 1, 4, 1e3 * exp( -2.847 ) );

    //Transition from "B1" to "C" i.e. r24
    SetGet3< unsigned int, unsigned int, double >::set( gslRateTableId,
            "setconst", 2, 4, 1e3 * exp( -0.693 ) );
    SetGet3< unsigned int, unsigned int, double >::set( exptlRateTableId,
            "setconst", 2, 4, 1e3 * exp( -0.693 ) );

    //Transition from "B2" to "C" i.e. r34
    SetGet3< unsigned int, unsigned int, double >::set( gslRateTableId,
            "setconst", 3, 4, 1e3* exp( -3.101 ) );
    SetGet3< unsigned int, unsigned int, double >::set( exptlRateTableId,
            "setconst", 3, 4, 1e3* exp( -3.101 ) );

    //Once the rate tables have been set up, we can initialize the
    //tables in the MarkovSolver class.
    SetGet2< Id, double >::set( exptlSolverId, "init", exptlRateTableId, 1.0e-3 );
    SetGet1< double >::set( exptlSolverId, "ligandConc", 24e-6 );
    Field< vector< double > >::set( exptlSolverId, "initialState",
                                    initState );

    Field< double >::set( gslSolverId, "relativeAccuracy", 1e-24 );
    Field< double >::set( gslSolverId, "absoluteAccuracy", 1e-24 );
    Field< double >::set( gslSolverId, "internalDt", 1e-24 );

    shell->doSetClock( 0, 1.0e-3 );
    shell->doSetClock( 1, 1.0e-3 );
    shell->doSetClock( 2, 1.0e-3 );
    shell->doSetClock( 3, 1.0e-3 );
    shell->doSetClock( 4, 1.0e-3 );
    shell->doSetClock( 5, 1.0e-3 );

    shell->doUseClock( "/n/gslCompt,/n/exptlCompt", "init", 0 );
    shell->doUseClock( "/n/gslCompt,/n/exptlCompt", "process", 1 );
    shell->doUseClock( "/n/gslCompt/gslRateTable,/n/exptlCompt/exptlRateTable",
                       "process", 2 );
    shell->doUseClock( "/n/gslCompt/gslSolver,/n/exptlCompt/exptlSolver",
                       "process", 3 );
    shell->doUseClock( "/n/gslCompt/mChanGsl,/n/gslTable","process", 4 );
    shell->doUseClock( "/n/exptlCompt/mChanExptl,/n/exptlTable", "process", 5 );

    shell->doReinit();
    // shell->doReinit();// why twice? - subha
    shell->doStart( 1.0 );

    vector< double > gslVec = Field< vector< double > >::get( gslTableId, "vector" );
    vector< double > exptlVec = Field< vector< double > >::get( exptlTableId, "vector");

    assert( gslVec.size() == 1001 );
    assert( exptlVec.size() == 1001 );

    for ( unsigned int i = 0; i < 1001; ++i )
        assert( doubleApprox( gslVec[i], exptlVec[i] ) );

    shell->doDelete( nid );
    cout << "." << flush;
}


///////////////////////////////////////////////////
// Unit tests for SynChan
///////////////////////////////////////////////////

// #include "SpikeGen.h"

/**
 * Here we set up a SynChan recieving spike inputs from two
 * SpikeGens. The first has a delay of 1 msec, the second of 10.
 * The tau of the SynChan is 1 msec.
 * We test for generation of peak responses at the right time, that
 * is 2 and 11 msec.
 */
void testSynChan()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );

    Id nid = shell->doCreate( "Neutral", Id(), "n", 1 );

    Id synChanId = shell->doCreate( "SynChan", nid, "synChan", 1 );
    Id synHandlerId = shell->doCreate( "SimpleSynHandler", synChanId, "syns", 1 );
    Id synId( synHandlerId.value() + 1 );
    Id sgId1 = shell->doCreate( "SpikeGen", nid, "sg1", 1 );
    Id sgId2 = shell->doCreate( "SpikeGen", nid, "sg2", 1 );
    ProcInfo p;
    p.dt = 1.0e-4;
    p.currTime = 0;
    bool ret;
    assert( synId.element()->getName() == "synapse" );
    ret = Field< double >::set( synChanId, "tau1", 1.0e-3 );
    assert( ret );
    ret = Field< double >::set( synChanId, "tau2", 1.0e-3 );
    assert( ret );
    ret = Field< double >::set( synChanId, "Gbar", 1.0 );
    assert( ret );

    // This is a hack, should really inspect msgs to automatically figure
    // out how many synapses are needed.
    ret = Field< unsigned int >::set( synHandlerId, "numSynapse", 2 );
    assert( ret );

    Element* syne = synId.element();
    assert( syne->totNumLocalField() == 2 );

    ObjId mid = shell->doAddMsg( "single",
                                 ObjId( sgId1, 0 ), "spikeOut", ObjId( synId, 0, 0 ), "addSpike" );
    assert( mid != Id() );
    mid = shell->doAddMsg( "single",
                           ObjId( sgId2, 0 ), "spikeOut", ObjId( synId, 0, 1 ), "addSpike" );
    assert( mid != Id() );
    mid = shell->doAddMsg( "single",
                           synHandlerId, "activationOut", synChanId, "activation" );
    assert( mid != Id() );

    ret = Field< double >::set( sgId1, "threshold", 0.0 );
    ret = Field< double >::set( sgId1, "refractT", 1.0 );
    ret = Field< bool >::set( sgId1, "edgeTriggered", 0 );
    ret = Field< double >::set( sgId2, "threshold", 0.0 );
    ret = Field< double >::set( sgId2, "refractT", 1.0 );
    ret = Field< bool >::set( sgId2, "edgeTriggered", 0 );

    ret = Field< double >::set( ObjId( synId, 0, 0 ), "weight", 1.0 );
    assert( ret);
    ret = Field< double >::set( ObjId( synId, 0, 0 ), "delay", 0.001 );
    assert( ret);
    ret = Field< double >::set( ObjId( synId, 0, 1 ), "weight", 1.0 );
    assert( ret);
    ret = Field< double >::set( ObjId( synId, 0, 1 ), "delay", 0.01 );
    assert( ret);

    double dret;
    dret = Field< double >::get( ObjId( synId, 0, 0 ), "weight" );
    ASSERT_DOUBLE_EQ("", dret, 1.0 );
    dret = Field< double >::get( ObjId( synId, 0, 0 ), "delay" );
    ASSERT_DOUBLE_EQ("", dret, 0.001 );
    dret = Field< double >::get( ObjId( synId, 0, 1 ), "weight" );
    ASSERT_DOUBLE_EQ("", dret, 1.0 );
    dret = Field< double >::get( ObjId( synId, 0, 1 ), "delay" );
    ASSERT_DOUBLE_EQ("", dret, 0.01 );

    dret = SetGet1< double >::set( sgId1, "Vm", 2.0 );
    dret = SetGet1< double >::set( sgId2, "Vm", 2.0 );
    dret = Field< double >::get( synChanId, "Gk" );
    ASSERT_DOUBLE_EQ("", dret, 0.0 );

    /////////////////////////////////////////////////////////////////////

    shell->doSetClock( 0, 1e-4 );
    shell->doSetClock( 1, 1e-4 );
    shell->doSetClock( 2, 1e-4 );
    // shell->doUseClock( "/n/##", "process", 0 );
    // shell->doUseClock( "/n/synChan/syns,/n/sg1,/n/sg2", "process", 0 );
    //It turns out that the order of setting of the spikes (sg1, sg2)
    //does not affect the outcome. The one thing that is critical is that
    //the 'process' call for the 'syns' should be before that of the
    //synChan. This is because the 'activation' message from the syns to
    //the synChan should proceed within a given timestep otherwise the
    //apparent arrival time of the event is delayed.
    shell->doUseClock( "/n/sg1,/n/sg2", "process", 0 );
    shell->doUseClock( "/n/synChan/syns", "process", 1 );
    shell->doUseClock( "/n/synChan", "process", 2 );
    // shell->doStart( 0.001 );
    shell->doReinit();
    shell->doReinit();

    shell->doStart( 0.001 );
    dret = Field< double >::get( synChanId, "Gk" );
    assert( doubleApprox( dret, 0.0 ) );

    shell->doStart( 0.0005 );
    dret = Field< double >::get( synChanId, "Gk" );
    assert( doubleApprox( dret, 0.825 ) );

    shell->doStart( 0.0005 );
    dret = Field< double >::get( synChanId, "Gk" );
    assert( doubleApprox( dret, 1.0 ) );

    shell->doStart( 0.001 );
    dret = Field< double >::get( synChanId, "Gk" );
    assert( doubleApprox( dret, 0.736 ) );

    shell->doStart( 0.001 );
    dret = Field< double >::get( synChanId, "Gk" );
    assert( doubleApprox( dret, 0.406 ) );

    shell->doStart( 0.007 );
    dret = Field< double >::get( synChanId, "Gk" );
    // assert( doubleApprox( dret, 0.997 ) );
    assert( doubleApprox( dret, 1.002 ) );

    shell->doDelete( nid );
    cout << "." << flush;
}

#if 0

void testNMDAChan()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );

    vector< int > dims( 1, 1 );
    Id nid = shell->doCreate( "Neutral", Id(), "n", dims );

    Id synChanId = shell->doCreate( "NMDAChan", nid, "nmdaChan", dims );
    Id synId( synChanId.value() + 1 );
    Id sgId1 = shell->doCreate( "SpikeGen", nid, "sg1", dims );
    ProcInfo p;
    p.dt = 1.0e-4;
    p.currTime = 0;
    bool ret;
    assert( synId()->getName() == "synapse" );
    ret = Field< double >::set( synChanId, "tau1", 130.5e-3 );
    assert( ret );
    ret = Field< double >::set( synChanId, "tau2", 5.0e-3 );
    assert( ret );
    ret = Field< double >::set( synChanId, "Gbar", 1.0 );
    assert( ret );

    // This is a hack, should really inspect msgs to automatically figure
    // out how many synapses are needed.
    ret = Field< unsigned int >::set( synChanId, "num_synapse", 1 );
    assert( ret );

    Element* syne = synId();
    assert( syne->dataHandler()->localEntries() == 1 );
    dynamic_cast< FieldDataHandlerBase* >( syne->dataHandler() )->setNumField( synChanId.eref().data(), 1 );

    assert( syne->dataHandler()->totalEntries() == 1 );
    assert( syne->dataHandler()->numDimensions() == 1 );
    assert( syne->dataHandler()->sizeOfDim( 0 ) == 1 );

    MsgId mid = shell->doAddMsg( "single",
                                 ObjId( sgId1, DataId( 0 ) ), "spikeOut",
                                 ObjId( synId, DataId( 0 ) ), "addSpike" );
    assert( mid != Id() );

    ret = Field< double >::set( sgId1, "threshold", 0.0 );
    ret = Field< double >::set( sgId1, "refractT", 1.0 );
    ret = Field< bool >::set( sgId1, "edgeTriggered", 0 );

    ret = Field< double >::set( ObjId( synId, DataId( 0 ) ),
                                "weight", 1.0 );
    assert( ret);
    ret = Field< double >::set( ObjId( synId, DataId( 0 ) ),
                                "delay", 0.001 );
    assert( ret);

    double dret;
    dret = Field< double >::get( ObjId( synId, DataId( 0 ) ), "weight" );
    ASSERT_DOUBLE_EQ("", dret, 1.0 );
    dret = Field< double >::get( ObjId( synId, DataId( 0 ) ), "delay" );
    ASSERT_DOUBLE_EQ("", dret, 0.001 );

    dret = SetGet1< double >::set( sgId1, "Vm", 2.0 );
    dret = Field< double >::get( synChanId, "Gk" );
    ASSERT_DOUBLE_EQ("", dret, 0.0 );

    /////////////////////////////////////////////////////////////////////

    shell->doSetClock( 0, 1e-4 );
    // shell->doUseClock( "/n/##", "process", 0 );
    shell->doUseClock( "/n/synChan,/n/sg1", "process", 0 );
    // shell->doStart( 0.001 );
    shell->doReinit();
    shell->doReinit();

    shell->doStart( 0.001 );
    dret = Field< double >::get( synChanId, "Gk" );
    assert( doubleApprox( dret, 0.0 ) );

    shell->doStart( 0.0005 );
    dret = Field< double >::get( synChanId, "Gk" );
    cout << "Gk:" << dret << endl;
    assert( doubleApprox( dret, 1.0614275017053588e-07 ) );

    // shell->doStart( 0.0005 );
    // dret = Field< double >::get( synChanId, "Gk" );
    // cout << "Gk:" << dret << endl;
    // assert( doubleApprox( dret, 1.0 ) );

    // shell->doStart( 0.001 );
    // dret = Field< double >::get( synChanId, "Gk" );
    // cout << "Gk:" << dret << endl;
    // assert( doubleApprox( dret, 0.736 ) );

    // shell->doStart( 0.001 );
    // dret = Field< double >::get( synChanId, "Gk" );
    // cout << "Gk:" << dret << endl;
    // assert( doubleApprox( dret, 0.406 ) );

    // shell->doStart( 0.007 );
    // dret = Field< double >::get( synChanId, "Gk" );
    // cout << "Gk:" << dret << endl;
    // assert( doubleApprox( dret, 0.997 ) );

    shell->doDelete( nid );
    cout << "." << flush;

}
#endif

static Id addCompartment( const string& name,
                          Id neuron, Id parent,
                          double dx, double dy, double dz, double dia )
{
    static Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    double x0 = 0.0;
    double y0 = 0.0;
    double z0 = 0.0;
    Id compt = shell->doCreate( "Compartment", neuron, name, 1 );
    if ( parent != Id() )
    {
        ObjId mid = shell->doAddMsg( "single",
                                     parent, "axial", compt, "raxial" );
        assert( mid != Id() );
        x0 = Field< double >::get( parent, "x" );
        y0 = Field< double >::get( parent, "y" );
        z0 = Field< double >::get( parent, "z" );
    }
    Field< double >::set( compt, "x0", x0 );
    Field< double >::set( compt, "y0", y0 );
    Field< double >::set( compt, "z0", z0 );
    Field< double >::set( compt, "x", x0 + dx );
    Field< double >::set( compt, "y", y0 + dy );
    Field< double >::set( compt, "z", z0 + dz );
    double length = sqrt( dx*dx + dy*dy + dz*dz );
    Field< double >::set( compt, "length", length );
    Field< double >::set( compt, "diameter", dia );
    Field< double >::set( compt, "Rm", 1.0 / ( PI * length * dia ) );
    Field< double >::set( compt, "Cm", 0.01 * ( PI * length * dia ) );
    Field< double >::set( compt, "Ra", 1 * length / ( PI*0.25*dia*dia ) );
    return compt;
}

#include "../utility/Vec.h"
#include "SwcSegment.h"
#include "Spine.h"
#include "Neuron.h"
#include "../shell/Wildcard.h"
static void testNeuronBuildTree()
{
    Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );

    Id nid = shell->doCreate( "Neuron", Id(), "n", 1 );
    double somaDia = 5e-6;
    double dendDia = 2e-6;
    double branchDia = 1e-6;
    static double len[] = { 10e-6, 100e-6, 200e-6, 500e-6 };
    static double dia[] = { somaDia, dendDia, branchDia, branchDia };
    Id soma = addCompartment ( "soma", nid, Id(), 10e-6, 0, 0, somaDia );
    Id dend1 = addCompartment ( "dend1", nid, soma, 100e-6, 0, 0, dendDia);
    Id branch1 = addCompartment ( "branch1", nid, dend1, 0, 200e-6, 0, branchDia );
    Id branch2 = addCompartment ( "branch2", nid, dend1, 0, -500e-6, 0, branchDia );
    static double x[] = { 10e-6, 110e-6, 110e-6, 110e-6 };
    static double y[] = {0, 0, 200e-6, -500e-6};
    static double z[] = {0, 0, 0, 0};

    SetGet0::set( nid, "buildSegmentTree" );

    vector< double > e = Field< vector< double > >::get(
                             nid, "electrotonicDistanceFromSoma" );
    vector< double > g = Field< vector< double > >::get(
                             nid, "geometricalDistanceFromSoma" );
    vector< double > p = Field< vector< double > >::get(
                             nid, "pathDistanceFromSoma" );
    assert( e.size() == 4 );
    ASSERT_DOUBLE_EQ("", e[0], 0.0 );
    double dL = 100e-6 / sqrt( dendDia /4.0 );
    ASSERT_DOUBLE_EQ("", e[1], dL );
    double bL1 = dL + 200e-6 / sqrt( branchDia /4.0 );
    ASSERT_DOUBLE_EQ("", e[2], bL1 );
    double bL2 = dL + 500e-6 / sqrt( branchDia/4.0 );
    ASSERT_DOUBLE_EQ("", e[3], bL2 );

    ASSERT_DOUBLE_EQ("", p[0], 0.0 );
    ASSERT_DOUBLE_EQ("", p[1], 100.0e-6 );
    ASSERT_DOUBLE_EQ("", p[2], 300.0e-6 ); // 100 + 200 microns
    ASSERT_DOUBLE_EQ("", p[3], 600.0e-6 ); // 100 + 500 microns

    ASSERT_DOUBLE_EQ("", g[0], 0.0 );
    ASSERT_DOUBLE_EQ("", g[1], 100.0e-6 );
    ASSERT_DOUBLE_EQ("", g[2], sqrt(5.0) * 100.0e-6 ); // 100 + 200 microns
    ASSERT_DOUBLE_EQ("", g[3], sqrt(26.0) * 100.0e-6 ); // 100 + 500 microns

    //////////////////////////////////////////////////////////////////
    // Here we test Neuron::evalExprForElist, which uses the muParser
    // Note that the wildcard list starts with the spine which is not
    // a compartment. So the indexing of the arrays e, p and g needs care.
    unsigned int nuParserNumVal = 13;
    vector< ObjId > elist;
    wildcardFind( "/n/#[ISA=Compartment]", elist );
    Neuron* n = reinterpret_cast< Neuron* >( nid.eref().data() );
    vector< double > val;
    n->evalExprForElist( elist, "p + g + L + len + dia + H(1-L)", val );
    assert( val.size() == nuParserNumVal * elist.size() );
    double maxP = 0.0;
    double maxG = 0.0;
    double maxL = 0.0;
    for ( unsigned int i = 0; i < elist.size(); ++i )
    {
        if ( maxP < p[i] ) maxP = p[i];
        if ( maxG < g[i] ) maxG = g[i];
        if ( maxL < e[i] ) maxL = e[i];
    }
    unsigned int j = 0;
    for ( unsigned int i = 0; i < elist.size(); ++i )
    {
        if ( !elist[i].element()->cinfo()->isA( "CompartmentBase" ) )
            continue;
        assert( val[i * nuParserNumVal] ==
                p[j] + g[j] + e[j] + len[j] + dia[j] + ( 1.0 - e[j] > 0 ) );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 1], p[j] );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 2], g[j] );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 3], e[j] );
        assert( doubleEq( val[i * nuParserNumVal + 4], len[j]  ));
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 5], dia[j] );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 6], maxP );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 7], maxG );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 8], maxL );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 9], x[j] );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 10], y[j] );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 11], z[j] );
        ASSERT_DOUBLE_EQ("", val[i * nuParserNumVal + 12], 0.0 );
        j++;
    }
    //////////////////////////////////////////////////////////////////
    // Here we test Neuron::makeSpacingDistrib, which uses the muParser
    // n->evalExprForElist( elist, "H(p-50e-6)*5e-6", val );
    n->evalExprForElist( elist, "H(p-100e-6)*5e-6", val );
    vector< unsigned int > seglistIndex;
    vector< unsigned int > elistIndex;
    vector< double > pos;
    vector< string > line; // empty, just use the default spacingDistrib=0
    n->makeSpacingDistrib( elist, val, seglistIndex, elistIndex, pos, line );
    // Can't do this now, it is not determinisitic.
    // assert( pos.size() == ((800 - 100)/5) );
    // ASSERT_DOUBLE_EQ("", pos[0], 2.5e-6 );
    // ASSERT_DOUBLE_EQ("", pos.back(), 500e-6 - 7.5e-6 );
    assert( seglistIndex[0] == 2 );
    assert( seglistIndex.back() == 3 );
    assert( elistIndex[0] == 2 );
    assert( elistIndex.back() == 3 );

    shell->doDelete( nid );
}


// This tests stuff without using the messaging.
void testBiophysics()
{
    testCompartment();
    testVectorTable();
    testNeuronBuildTree();
#if 0
    testMarkovSolverBase();
    testMarkovSolver();
    testHHGateCreation();
    testHHGateLookup();
    testHHGateSetup();
    testSpikeGen();
    testCaConc();
    testNernst();
#endif
}

// This is applicable to tests that use the messaging and scheduling.
void testBiophysicsProcess()
{
    // testSynChan();
    testIntFireNetwork();
    testCompartmentProcess();
    // testMarkovGslSolver();
    // testMarkovChannel();
#if 0
    testHHChannel();
#endif
}

#else // ifdef DO_UNIT_TESTS
void testBiophysics()
{
    ;
}
void testBiophysicsProcess()
{
    ;
}
void testIntFireNetwork( unsigned int unsteps = 5 )
{
    ;
}
#endif
