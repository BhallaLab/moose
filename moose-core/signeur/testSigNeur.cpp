/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifdef DO_UNIT_TESTS

#include "../basecode/header.h"
#include "Adaptor.h"
#include "../shell/Shell.h"
#include "../shell/Wildcard.h"

void testAdaptor()
{
	Adaptor foo;
	foo.setInputOffset( 1 );
	foo.setOutputOffset( 2 );
	foo.setScale( 10 );

	for ( unsigned int i = 0; i < 10; ++i )
		foo.input( i );


	assert( doubleEq( foo.getOutput(), 0.0 ) );
	foo.innerProcess();

	assert( doubleEq( foo.getOutput(), ( -1.0 + 4.5) * 10.0 + 2.0 ) );

	// shell->doDelete( nid );
	cout << "." << flush;
}

/// Tests the multi-input RequestField message.
void testAdaptorRequestField()
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId().data() );
	Id model = shell->doCreate( "Neutral", Id(), "model", 1 );
	Id adaptor = shell->doCreate( "Adaptor", model, "adaptor", 1 );
	Id onepool = shell->doCreate( "Pool", model, "onepool", 1 );
	Id twopool = shell->doCreate( "Pool", model, "twopool", 2 );
	Id tenpool = shell->doCreate( "Pool", model, "tenpool", 10 );
	ObjId mid = shell->doAddMsg( "Single", adaptor, "requestOut",
					onepool, "getNInit" );
	assert( !mid.bad() );
	mid = shell->doAddMsg( "Single", adaptor, "requestOut",
					ObjId( twopool, 0 ), "getNInit" );
	assert( !mid.bad() );
	mid = shell->doAddMsg( "Single", adaptor, "requestOut",
					ObjId( twopool, 1 ), "getNInit" );
	assert( !mid.bad() );
	mid = shell->doAddMsg( "OneToAll", adaptor, "requestOut",
					tenpool, "getNInit" );
	assert( !mid.bad() );
	Field< double >::set( onepool, "nInit", 1.0 );
	Field< double >::set( ObjId( twopool, 0 ), "nInit", 20.0 );
	Field< double >::set( ObjId( twopool, 1 ), "nInit", 30.0 );
	for ( unsigned int i = 0; i < 10; ++i )
		Field< double >::
				set( ObjId( tenpool, i ), "nInit", (i + 1 ) * 100 );
	ProcInfo p;
	SetGet1< ProcPtr >::set( adaptor, "reinit", &p );
	double ret = Field< double >::get( adaptor, "outputValue" );
	assert( doubleEq( ret, ( 1.0 + 50.0 + 5500.0 )/13.0 ) );

	const Finfo *f = adaptor.element()->cinfo()->findFinfo( "requestOut" );
	const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( f );
	assert( sf );
	vector< ObjId > temp;
	temp = adaptor.element()->getMsgTargets( 0, sf );
	assert ( temp.size() == 13 );
	assert( temp[0] == ObjId( onepool, 0 ) );
	assert( temp[1] == ObjId( twopool, 0 ) );
	assert( temp[2] == ObjId( twopool, 1 ) );
	for ( int i = 0; i < 10; ++i )
		assert( temp[i + 3] == ObjId( tenpool, i ) );

	shell->doDelete( model );
	cout << "." << flush;
}


//////////////////////////////////////////////////////////////////////
// Test of multiscale model setup. Model structure is as follows:
//
// Elec:
// Single elec compt soma with HH channels. 5 spines on it. GluR on spines.
// 		Ca pool in spine head fed by GluR.
// Chem:
// 		PSD: GluR pool.
//    	Head: Ca pool. GluR pool. 'Enz' to take to PSD.
//    		Reac balances with PSD.
// 		Dend: Ca pool binds to 'kinase'. Kinase phosph K chan.
// 			Dephosph by reac.
//			Ca is pumped out into a buffered non-reactive pool
//
// 		Diffusion:
// 			Ca from spine head goes to dend
//
// Adaptors:
// 		Electrical Ca in spine head -> Ca pool in spine head
// 		Chem GluR in PSD -> Electrical GluR Gbar
// 		Chem K in soma -> Electrical K Gbar.
//
//////////////////////////////////////////////////////////////////////

#if 0
// Defined in testBiophysics.cpp
extern Id makeSquid();
// Defined in testMesh.cpp
extern Id makeSpine( Id compt, Id cell, unsigned int index, double frac,
			double len, double dia, double theta );

Id makeSpineWithReceptor( Id compt, Id cell, unsigned int index, double frac )
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	double spineLength = 5.0e-6;
	double spineDia = 4.0e-6;
	Id spineCompt = makeSpine( compt, cell, index, frac,
					spineLength, spineDia, 0.0 );

	Id gluR = shell->doCreate( "SynChan", spineCompt, "gluR", 1 );
	Field< double >::set( gluR, "tau1", 1e-3 );
	Field< double >::set( gluR, "tau2", 1e-3 );
	Field< double >::set( gluR, "Gbar", 1e-5 );
	Field< double >::set( gluR, "Ek", 10.0e-3 );
	ObjId mid = shell->doAddMsg( "Single", ObjId( spineCompt ), "channel",
					ObjId( gluR ), "channel" );
	assert( ! mid.bad() );


	Id caPool = shell->doCreate( "CaConc", spineCompt, "ca", 1 );
	Field< double >::set( caPool, "CaBasal", 1e-4 ); // millimolar
	Field< double >::set( caPool, "tau", 0.01 ); // seconds
	double B = 1.0 / (
		FaradayConst *  spineLength * spineDia * spineDia * 0.25 * PI );
	B = B / 20.0;
	Field< double >::set( caPool, "B", B ); // Convert from current to conc
	mid = shell->doAddMsg( "Single", ObjId( gluR ), "IkOut",
					ObjId( caPool ), "current" );
	assert( ! mid.bad() );

	return spineCompt;
}

Id buildSigNeurElec( vector< Id >& spines )
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );
	double comptLength = 100e-6;
	double comptDia = 4e-6;
	unsigned int numSpines = 5;

	Id nid = makeSquid();
	Id compt( "/n/compt" );
	Field< double >::set( compt, "inject", 0.0 );
	Field< double >::set( compt, "x0", 0 );
	Field< double >::set( compt, "y0", 0 );
	Field< double >::set( compt, "z0", 0 );
	Field< double >::set( compt, "x", comptLength );
	Field< double >::set( compt, "y", 0 );
	Field< double >::set( compt, "z", 0 );
	Field< double >::set( compt, "length", comptLength );
	Field< double >::set( compt, "diameter", comptDia );

	// Make a SpikeGen as a synaptic input to the spines.
	Id synInput = shell->doCreate( "SpikeGen", nid, "synInput", dims.size() );
	Field< double >::set( synInput, "refractT", 47e-3 );
	Field< double >::set( synInput, "threshold", -1.0 );
	Field< bool >::set( synInput, "edgeTriggered", false );
	SetGet1< double >::set( synInput, "Vm", 0.0 );

	spines.resize( numSpines );

	for ( unsigned int i = 0; i < numSpines; ++i ) {
		double frac = ( 0.5 + i ) / numSpines;
		spines[i] = makeSpineWithReceptor( compt, nid, i, frac );
		stringstream ss;
		ss << "/n/head" << i << "/gluR";
		string name = ss.str();
		Id gluR(  name );
		assert( gluR != Id() );
		Field< unsigned int >::set( gluR, "num_synapse", 1 );
		Id syn( name + "/synapse" );
		assert( syn != Id() );
		ObjId mid = shell->doAddMsg( "Single", ObjId( synInput ), "event",
					ObjId( syn ), "addSpike" );
		assert( ! mid.bad() );
		Field< double >::set( syn, "weight", 0.2 );
		Field< double >::set( syn, "delay", i * 1.0e-4 );
	}

	return nid;
}


void buildSigNeurChem( Id nid, Id neuroMesh, Id spineMesh, Id psdMesh )
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );

	///////////////////////////////////////////////////////////////////
	// Stuff in PSD
	///////////////////////////////////////////////////////////////////
	Id psdMeshEntries( "/n/psdMesh/mesh" );
	assert( psdMeshEntries != Id() );
	Id psdGluR = shell->doCreate( "Pool", psdMesh, "psdGluR", 1 );
	ObjId mid = shell->doAddMsg( "Single", psdGluR, "mesh", psdMeshEntries, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( psdGluR, "nInit", 0 );
	///////////////////////////////////////////////////////////////////
	// Stuff in spine head
	///////////////////////////////////////////////////////////////////
	Id spineMeshEntries( "/n/spineMesh/mesh" );

	Id headCa = shell->doCreate( "Pool", spineMesh, "Ca" , 1);
	mid = shell->doAddMsg( "Single", spineMeshEntries, "mesh", headCa, "mesh" );
	assert( ! mid.bad() );

	Field< double >::set( headCa, "concInit", 1e-4 );

	Id headGluR = shell->doCreate( "Pool", spineMesh, "headGluR", 1 );
	mid = shell->doAddMsg( "Single", spineMeshEntries, "mesh", headGluR, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( headGluR, "nInit", 100 );

	Id toPsd = shell->doCreate( "Pool", spineMesh, "toPsd", 1 );
	mid = shell->doAddMsg( "Single", spineMeshEntries, "mesh", toPsd, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( toPsd, "concInit", 0 );

	Id toPsdInact = shell->doCreate( "Pool", spineMesh, "toPsdInact", 1 );
	mid = shell->doAddMsg( "Single", spineMeshEntries, "mesh", toPsdInact, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( toPsdInact, "concInit", 1e-3 );

	Id turnOnPsd = shell->doCreate( "Reac", spineMesh, "turnOnPsd", 1);
	mid = shell->doAddMsg( "OneToOne", turnOnPsd, "sub", headCa, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToOne", turnOnPsd, "sub", toPsdInact, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToOne", turnOnPsd, "prd", toPsd, "reac" );
	assert( ! mid.bad() );
	Field< double >::set( turnOnPsd, "Kf", 0.1e3 );
	Field< double >::set( turnOnPsd, "Kb", 0.1 );


	Id toPsdEnz = shell->doCreate( "Enz", toPsd, "enz" , 1);
	Id toPsdEnzCplx = shell->doCreate( "Pool", toPsdEnz, "cplx", 1 );
	shell->doAddMsg( "Single", spineMeshEntries, "mesh", toPsdEnzCplx, "mesh" );
	mid = shell->doAddMsg( "OneToOne", toPsdEnz, "enz", toPsd, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToOne", toPsdEnz, "sub", headGluR, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToOne", toPsdEnz, "prd", psdGluR, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToOne", toPsdEnz, "cplx", toPsdEnzCplx, "reac" );
	assert( ! mid.bad() );
	Field< double >::set( toPsdEnz, "Km", 1.0e-3 ); 	// 1 uM
	Field< double >::set( toPsdEnz, "kcat", 1.0 );	// 1/sec.

	// Must be in same compartment as reagent.
	Id fromPsd = shell->doCreate( "Reac", psdMesh, "fromPsd", 1 );

	mid = shell->doAddMsg( "OneToOne", fromPsd, "sub", psdGluR, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToOne", fromPsd, "prd", headGluR, "reac" );
	assert( ! mid.bad() );
	Field< double >::set( fromPsd, "Kf", 0.05 );
	Field< double >::set( fromPsd, "Kb", 0.0 );

	// Id toPsdEnzCplx( toPsdEnz.value() + 1 );
	Id foo( "/n/spineMesh/toPsd/enz/cplx" );
	assert( toPsdEnzCplx != Id() );
	assert( toPsdEnzCplx == foo );

	///////////////////////////////////////////////////////////////////
	// Stuff in dendrite
	///////////////////////////////////////////////////////////////////
	Id dendMeshEntries( "/n/neuroMesh/mesh" );

	Id dendCa = shell->doCreate( "Pool", neuroMesh, "Ca", 1 );
	mid = shell->doAddMsg( "Single", dendMeshEntries, "mesh", dendCa, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( dendCa, "concInit", 1e-4 ); // 0.1 uM.
	double conc = Field< double >::get( dendCa, "concInit" );
	assert( doubleEq( conc, 1e-4 ) );

	Id bufCa = shell->doCreate( "BufPool", neuroMesh, "bufCa", 1 );
	mid = shell->doAddMsg( "Single", dendMeshEntries, "mesh", bufCa, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( bufCa, "concInit", 1e-4 ); // 0.1 uM.
	conc = Field< double >::get( bufCa, "concInit" );
	assert( doubleEq( conc, 1e-4 ) );

	Id pumpCa = shell->doCreate( "Reac", neuroMesh, "pumpCa" , 1);
	mid = shell->doAddMsg( "OneToOne", pumpCa, "sub", dendCa, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToOne", pumpCa, "prd", bufCa, "reac" );
	assert( ! mid.bad() );
	Field< double >::set( pumpCa, "Kf", 0.1 );
	Field< double >::set( pumpCa, "Kb", 0.1 );

	Id dendKinaseInact = shell->doCreate( "Pool", neuroMesh, "inact_kinase", 1 );
	mid = shell->doAddMsg( "Single", dendMeshEntries, "mesh", dendKinaseInact, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( dendKinaseInact, "concInit", 1e-3 ); // 1 uM.

	Id dendKinase = shell->doCreate( "Pool", neuroMesh, "Ca.kinase", 1 );
	mid = shell->doAddMsg( "Single", dendMeshEntries, "mesh", dendKinase, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( dendKinase, "concInit", 0.0 ); // 1 uM.


	Id dendTurnOnKinase = shell->doCreate( "Reac", neuroMesh, "turnOnKinase", 1);
	mid = shell->doAddMsg(
		"OneToOne", dendTurnOnKinase, "sub", dendCa, "reac" );
	mid = shell->doAddMsg(
		"OneToOne", dendTurnOnKinase, "sub", dendKinaseInact, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg(
		"OneToOne", dendTurnOnKinase, "prd", dendKinase, "reac" );
	assert( ! mid.bad() );
	Field< double >::set( dendTurnOnKinase, "Kf", 0.5e3 );
	Field< double >::set( dendTurnOnKinase, "Kb", 0.1 );

	Id dendKinaseEnz = shell->doCreate( "Enz", dendKinase, "enz" , 1);
	Id dendKinaseEnzCplx = shell->doCreate( "Pool", dendKinaseEnz, "cplx", 1 );
	mid = shell->doAddMsg( "Single", dendMeshEntries, "mesh", dendKinaseEnzCplx, "mesh" );
	assert( ! mid.bad() );
	Id kChan = shell->doCreate( "Pool", neuroMesh, "kChan", 1);
	mid = shell->doAddMsg( "Single", dendMeshEntries, "mesh", kChan, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( kChan, "concInit", 1e-3 ); // 1 uM.

	Id kChan_p = shell->doCreate( "Pool", neuroMesh, "kChan_p", 1);
	mid = shell->doAddMsg( "Single", dendMeshEntries, "mesh", kChan_p, "mesh" );
	assert( ! mid.bad() );
	Field< double >::set( kChan_p, "concInit", 0 ); // 0 uM.

	mid = shell->doAddMsg("OneToOne", dendKinaseEnz, "enz", dendKinase, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg("OneToOne", dendKinaseEnz, "sub", kChan, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg("OneToOne", dendKinaseEnz, "prd", kChan_p, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg("OneToOne", dendKinaseEnz, "cplx", dendKinaseEnzCplx, "reac" );
	assert( ! mid.bad() );
	Field< double >::set( dendKinaseEnz, "Km", 5e-3 ); 	// 5 uM
	Field< double >::set( dendKinaseEnz, "kcat", 0.1 );	// 0.1/sec.

	Id dendPhosphatase = shell->doCreate( "Reac", neuroMesh, "phosphatase", 1);
	mid = shell->doAddMsg("OneToOne", dendPhosphatase, "sub", kChan_p, "reac" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg("OneToOne", dendPhosphatase, "prd", kChan, "reac" );
	assert( ! mid.bad() );
	Field< double >::set( dendPhosphatase, "Kf", 0.1 );
	Field< double >::set( dendPhosphatase, "Kb", 0.0 );


	Id bar( "/n/neuroMesh/Ca.kinase/enz/cplx" );
	assert( dendKinaseEnzCplx != Id() );
	assert( dendKinaseEnzCplx == bar );
	shell->doAddMsg( "Single", spineMeshEntries, "mesh", dendKinaseEnzCplx, "mesh" );
}

void buildSigNeurNeuroMesh( Id nid, Id& neuroMesh, Id& spineMesh, Id& psdMesh )
{
	const double diffLength = 1e-6;
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );

	neuroMesh = shell->doCreate( "NeuroMesh", nid, "neuroMesh", dims.size() );
	Field< bool >::set( neuroMesh, "separateSpines", true );
	Field< double >::set( neuroMesh, "diffLength", diffLength );
	Field< string >::set( neuroMesh, "geometryPolicy", "cylinder" );
	spineMesh = shell->doCreate( "SpineMesh", nid, "spineMesh", dims.size());
	ObjId mid;
	mid = shell->doAddMsg( "OneToOne", neuroMesh, "spineListOut", spineMesh, "spineList" );
	assert( ! mid.bad() );
	psdMesh = shell->doCreate( "PsdMesh", nid, "psdMesh", dims.size() );
	mid = shell->doAddMsg( "OneToOne", neuroMesh, "psdListOut", psdMesh, "psdList" );
	assert( ! mid.bad() );
}

void makeChemInNeuroMesh()
{
	double spineLength = 5.0e-6;
	double spineDia = 4.0e-6;
	double comptLength = 100.0e-6;
	double comptDia = 4.0e-6;
	vector< Id > spines;
	Id nid = buildSigNeurElec( spines );
	Id neuroMesh, spineMesh, psdMesh;
	buildSigNeurNeuroMesh( nid, neuroMesh, spineMesh, psdMesh );
	buildSigNeurChem( nid, neuroMesh, spineMesh, psdMesh );
	///////////////////////////////////////////////////////////////////
	// Make NeuroMesh
	///////////////////////////////////////////////////////////////////
	Field< Id >::set( neuroMesh, "cell", nid );
	// shell->doReinit();
	// Qinfo::clearQ( 0 );
	// Qinfo::clearQ( 0 );
	// Deprecated. Qinfo::waitProcCycles( 4 );

	///////////////////////////////////////////////////////////////////
	// Check that stuff has been built
	///////////////////////////////////////////////////////////////////
	const unsigned int numComptsInDend = 100;
	const unsigned int numSpines = 5;
	unsigned int size;

	// 25 Apr 2013: this doesn't work, though it should. Need to clean up.
	//size = psdMeshEntries.element()->dataHandler()->totalEntries();

	Id psdMeshEntries( "/n/psdMesh/mesh" );
	size = Id( "/n/spineMesh/mesh" ).element()->numData();
	assert( size == numSpines );
	size = Id( "/n/psdMesh/mesh" ).element()->numData();
	assert( size == numSpines );

	size = Id( "/n/spineMesh/headGluR" ).element()->numData();
	// size = Field< unsigned int >::get( headGluR, "linearSize");
	assert( size == numSpines );
	double vol;
	vol = Field< double >::get( Id( "/n/spineMesh/headGluR" ), "volume" );
	assert( doubleEq( vol, spineLength * spineDia * spineDia * PI / 4.0 ) );

	size = Id( "/n/spineMesh/toPsd" ).element()->numData();
	// size = Field< unsigned int >::get( toPsdEnz, "linearSize");
	assert( size == numSpines );
	size = Id( "/n/spineMesh/fromPsd" ).element()->numData();
	// size = Field< unsigned int >::get( fromPsd, "linearSize");
	assert( size == 1 ); // It is a reac.

	size = Id( "/n/psdMesh/psdGluR" ).element()->numData();
	// size = Field< unsigned int >::get( psdGluR, "linearSize");
	assert( size == numSpines );
	vol = Field< double >::get( Id( "/n/psdMesh/psdGluR" ), "volume" );
	double thick = Field< double >::get( Id( "/n/psdMesh" ), "thickness" );
	assert( doubleEq( vol, thick * spineDia * spineDia * PI / 4.0 ) );

	size = Id( "/n/neuroMesh/Ca" ).element()->numData();
	// size = Field< unsigned int >::get( dendCa, "linearSize");
	assert( size == numComptsInDend );
	vol = Field< double >::get( Id( "/n/neuroMesh/Ca" ), "volume" );
	assert( doubleEq( vol, comptLength * comptDia * comptDia * PI / (4.0 * numComptsInDend ) ) );

	size = Id( "/n/neuroMesh/pumpCa" ).element()->numData();
	// size = Field< unsigned int >::get( pumpCa, "linearSize");
	assert( size == 1 );
	size = Id( "/n/neuroMesh/dendKinaseEnz" ).element()->numData();
	// size = Field< unsigned int >::get( dendKinaseEnz, "linearSize");
	assert( size == 1 );


	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	shell->doDelete( nid );
	cout << "." << flush;
}

Id makeChemInCubeMesh()
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );
	double dendSide = 10.8e-6;	// Matches vol cylinder of 100x4 microns.
	double spineSide = 6.8e-6;	// Matches vol of 5 spines of 5x4 microns.
	double psdSide = 8.565e-7;	// Matches 'vol' of 5 psds of 4 microns
			// diameter. Assume thickness of 0.01 micron, since area not
			// comparable otherwise.

	Id nid( "/n" );
	if ( nid == Id() )
		nid = shell->doCreate( "Neutral", Id(), "n", 1 );
	Id neuroMesh = shell->doCreate( "CubeMesh", nid, "neuroMesh", 1);
	Id spineMesh = shell->doCreate( "CubeMesh", nid, "spineMesh", 1);
	Id psdMesh = shell->doCreate( "CubeMesh", nid, "psdMesh", 1);

	vector< double > coords( 9, dendSide );
	coords[0] = 0.0;
	coords[1] = 0.0;
	coords[2] = 0.0;
	Field< vector< double > >::set( neuroMesh, "coords", coords );
	Field< bool >::set( neuroMesh, "preserveNumEntries", true );

	coords.clear();
	coords.resize( 9, spineSide );
	coords[0] = dendSide;
	coords[1] = 0.0;
	coords[2] = 0.0;
	coords[3] = spineSide + dendSide;
	Field< vector< double > >::set( spineMesh, "coords", coords );
	Field< bool >::set( spineMesh, "preserveNumEntries", true );

	coords.clear();
	coords.resize( 9, psdSide );
	coords[0] = dendSide + spineSide;
	coords[1] = 0.0;
	coords[2] = 0.0;
	coords[3] = psdSide + spineSide + dendSide;
	Field< vector< double > >::set( psdMesh, "coords", coords );
	Field< bool >::set( psdMesh, "preserveNumEntries", true );

	buildSigNeurChem( nid, neuroMesh, spineMesh, psdMesh );

	///////////////////////////////////////////////////////////////////
	// Check that stuff has been built
	///////////////////////////////////////////////////////////////////
	unsigned int size;

	Id psdMeshEntries( "/n/psdMesh/mesh" );
	size = Id( "/n/spineMesh/mesh" ).element()->numData();
	assert( size == 1 );
	size = Id( "/n/psdMesh/mesh" ).element()->numData();
	assert( size == 1 );

	size = Id( "/n/spineMesh/headGluR" ).element()->numData();
	assert( size == 1 );
	double vol;
	vol = Field< double >::get( Id( "/n/spineMesh/headGluR" ), "volume" );
	assert( doubleEq( vol, pow( spineSide, 3.0 ) ) );

	size = Id( "/n/spineMesh/toPsd" ).element()->numData();
	assert( size == 1 );
	size = Id( "/n/spineMesh/fromPsd" ).element()->numData();
	assert( size == 1 ); // It is a reac.

	size = Id( "/n/psdMesh/psdGluR" ).element()->numData();
	assert( size == 1 );
	vol = Field< double >::get( Id( "/n/psdMesh/psdGluR" ), "volume" );
	assert( doubleEq( vol, pow( psdSide, 3.0 ) ) );

	size = Id( "/n/neuroMesh/Ca" ).element()->numData();
	assert( size == 1 );
	vol = Field< double >::get( Id( "/n/neuroMesh/Ca" ), "volume" );
	assert( doubleEq( vol, pow( dendSide, 3.0 ) ) );

	size = Id( "/n/neuroMesh/pumpCa" ).element()->numData();
	assert( size == 1 );
	size = Id( "/n/neuroMesh/dendKinaseEnz" ).element()->numData();
	assert( size == 1 );
	/////////////////////////////////////////////////////////////////////
	// Sanity check
	/////////////////////////////////////////////////////////////////////
	vol = dendSide * dendSide * dendSide;
	double conc = Field< double >::get( Id( "/n/neuroMesh/Ca"), "concInit");
	double nInit = Field< double >::get( Id( "/n/neuroMesh/Ca"), "nInit");
	assert( doubleEq( conc, 1e-4 ) );
	assert( doubleEq( nInit, 1e-4 * NA * vol ) );

	conc = Field< double >::get( Id( "/n/neuroMesh/bufCa"), "concInit");
	nInit = Field< double >::get( Id( "/n/neuroMesh/bufCa"), "nInit");
	assert( doubleEq( conc, 1e-4 ) );
	assert( doubleEq( nInit, 1e-4 * NA * vol ) );

	double concK;
	double numK;
	concK = Field< double >::get( Id( "/n/neuroMesh/turnOnKinase"), "Kf" );
	assert( doubleEq( concK, 500 ) );
	numK = Field< double >::get( Id( "/n/neuroMesh/turnOnKinase"), "kf" );
	assert( doubleEq( numK, 500 / ( NA * vol ) ) );
	concK = Field< double >::get( Id( "/n/neuroMesh/Ca.kinase/enz" ), "Km");
	assert( doubleEq( concK, 0.005 ) );
	numK = Field< double >::get( Id( "/n/neuroMesh/Ca.kinase/enz" ),"kcat");
	assert( doubleEq( numK, 0.1 ) );
	numK = Field< double >::get( Id( "/n/neuroMesh/Ca.kinase/enz" ), "k1");
	assert( doubleEq( numK, ( 0.1 + 0.4 ) / ( 0.005 * NA * vol ) ) );
	numK = Field< double >::get( Id( "/n/neuroMesh/Ca.kinase/enz" ), "k2");
	assert( doubleEq( numK, 0.4 ) );
	numK = Field< double >::get( Id( "/n/neuroMesh/Ca.kinase/enz" ), "k3");
	assert( doubleEq( numK, 0.1 ) );


	vol = spineSide * spineSide * spineSide;
	concK = Field< double >::get( Id( "/n/spineMesh/turnOnPsd"), "Kf" );
	assert( doubleEq( concK, 100 ) );
	numK = Field< double >::get( Id( "/n/spineMesh/turnOnPsd"), "kf" );
	assert( doubleEq( numK, 100 / ( NA * vol ) ) );
	concK = Field< double >::get( Id( "/n/spineMesh/toPsd/enz" ), "Km");
	assert( doubleEq( concK, 0.001 ) );
	numK = Field< double >::get( Id( "/n/spineMesh/toPsd/enz" ),"kcat");
	assert( doubleEq( numK, 1 ) );
	numK = Field< double >::get( Id( "/n/spineMesh/toPsd/enz" ), "k1");
	assert( doubleEq( numK, ( 1.0 + 4.0 ) / ( 0.001 * NA * vol ) ) );
	numK = Field< double >::get( Id( "/n/spineMesh/toPsd/enz" ), "k2");
	assert( doubleEq( numK, 4 ) );
	numK = Field< double >::get( Id( "/n/spineMesh/toPsd/enz" ), "k3");
	assert( doubleEq( numK, 1 ) );
	return nid;
}

void testChemInCubeMesh()
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );
	Id nid = makeChemInCubeMesh();

	/*
	vector< Id > ids;
	simpleWildcardFind( "/n/##[ISA=PoolBase]", ids );
	for( vector< Id >::iterator i = ids.begin(); i != ids.end(); ++i ) {
		cout << i->path() <<
			" :	concInit = " << Field< double >::get( *i, "concInit" ) <<
			";	nInit = " << Field< double >::get( *i, "nInit" ) <<
			";	volume = " << Field< double >::get( *i, "volume" )
				<< endl;
	}
	ids.clear();
	simpleWildcardFind( "/n/##[ISA=ReacBase]", ids );
	for( vector< Id >::iterator i = ids.begin(); i != ids.end(); ++i ) {
		cout << i->element()->getName() <<
			":	Kf = " << Field< double >::get( *i, "Kf" ) <<
			";	Kb = " << Field< double >::get( *i, "Kb" ) <<
			":	kf = " << Field< double >::get( *i, "kf" ) <<
			";	kb = " << Field< double >::get( *i, "kb" )
				<< endl;
	}
	ids.clear();
	simpleWildcardFind( "/n/##[ISA=EnzBase]", ids );
	for( vector< Id >::iterator i = ids.begin(); i != ids.end(); ++i ) {
		cout << i->path() <<
			":	Km = " << Field< double >::get( *i, "Km" ) <<
			";	kcat = " << Field< double >::get( *i, "kcat" ) <<
			";	k1 = " << Field< double >::get( *i, "k1" ) <<
			";	k2 = " << Field< double >::get( *i, "k2" ) <<
			";	k3 = " << Field< double >::get( *i, "k3" ) <<
				endl;
	}
	*/

	/////////////////////////////////////////////////////////////////////
	// Run it
	/////////////////////////////////////////////////////////////////////
	Id tab = shell->doCreate( "Table", nid, "tab", dims.size() );
	ObjId mid = shell->doAddMsg( "Single", tab, "requestData",
		Id( "/n/psdMesh/psdGluR" ), "get_n" );
	assert( ! mid.bad() );
	Id tabCa = shell->doCreate( "Table", nid, "tabCa", dims.size() );
	mid = shell->doAddMsg( "Single", tabCa, "requestData",
		Id( "/n/spineMesh/Ca" ), "get_conc" );

	assert( ! mid.bad() );
	Id tab2 = shell->doCreate( "Table", nid, "tab2", dims.size() );
	mid = shell->doAddMsg( "Single", tab2, "requestData",
		Id( "/n/neuroMesh/kChan_p" ), "get_conc" );

	assert( ! mid.bad() );
	Id tab4 = shell->doCreate( "Table", nid, "tab4", dims.size() );
	mid = shell->doAddMsg( "Single", tab4, "requestData",
		Id( "/n/neuroMesh/Ca.kinase" ), "get_conc" );

	assert( ! mid.bad() );
	Id tab3 = shell->doCreate( "Table", nid, "tab3", dims.size() );
	mid = shell->doAddMsg( "Single", tab3, "requestData",
		Id( "/n/spineMesh/toPsd" ), "get_conc" );

	assert( ! mid.bad() );
	//////////////////////////////////////////////////////////////////////
	// Schedule, Reset, and run.
	//////////////////////////////////////////////////////////////////////

	shell->doSetClock( 5, 1.0e-2 );
	shell->doSetClock( 6, 1.0e-2 );
	shell->doSetClock( 7, 1.0 );

	shell->doUseClock( "/n/neuroMesh/##,/n/spineMesh/##,/n/psdMesh/##", "process", 5 );
	shell->doUseClock( "/n/tab#", "process", 7 );

	shell->doReinit();
	shell->doReinit();
	shell->doStart( 100.0 );

	SetGet2< string, string >::set( tabCa, "xplot", "SigNeurChem.plot", "spineCa" );
	SetGet2< string, string >::set( tab, "xplot", "SigNeurChem.plot", "psdGluR_N" );
	SetGet2< string, string >::set( tab2, "xplot", "SigNeurChem.plot", "kChan_p" );
	SetGet2< string, string >::set( tab4, "xplot", "SigNeurChem.plot", "dendKinase" );
	SetGet2< string, string >::set( tab3, "xplot", "SigNeurChem.plot", "toPsd" );

	//////////////////////////////////////////////////////////////////////
	// Build a solver, reset and run
	//////////////////////////////////////////////////////////////////////

	Id solver = shell->doCreate( "GslStoich", nid, "solver", dims.size() );
	assert( solver != Id() );
	Field< string >::set( solver, "path", "/n/##" );
	Field< string >::set( solver, "method", "rk5" );
	shell->doSetClock( 5, 1.0 );
	shell->doSetClock( 6, 1.0 );
	shell->doUseClock( "/n/solver", "process", 5 );
	shell->doReinit();
	shell->doStart( 100.0 );

	SetGet2< string, string >::set( tabCa, "xplot", "SigNeurChem.plot", "spineCa2" );
	SetGet2< string, string >::set( tab, "xplot", "SigNeurChem.plot", "psdGluR_N2" );
	SetGet2< string, string >::set( tab2, "xplot", "SigNeurChem.plot", "kChan_p2" );
	SetGet2< string, string >::set( tab3, "xplot", "SigNeurChem.plot", "toPsd2" );

	shell->doDelete( nid );
	cout << "." << flush;
}

void testSigNeurElec()
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );

	vector< Id > spines;
	Id nid = buildSigNeurElec( spines );
	Id compt( "/n/compt" );
	//////////////////////////////////////////////////////////////////////
	// Graph
	//////////////////////////////////////////////////////////////////////
	/*
	vector< Id > ret;
	simpleWildcardFind( "/n/##[ISA=Compartment]", ret );
	for ( vector< Id >::iterator i = ret.begin(); i != ret.end(); ++i ) {
		double Ra = Field< double >::get( *i, "Ra" );
		double Rm = Field< double >::get( *i, "Rm" );
		double Em = Field< double >::get( *i, "Em" );
		double Cm = Field< double >::get( *i, "Cm" );
		string name = i->element()->getName();
		cout << name << ": Ra = " << Ra << ", Rm = " << Rm <<
				", Cm = " << Cm << ", Em = " << Em << endl;
	}
	*/
	Id tab = shell->doCreate( "Table", nid, "tab", dims.size() );
	ObjId mid = shell->doAddMsg( "Single", ObjId( tab, 0 ),
		"requestData", ObjId( spines[2], 0 ), "get_Vm" );
	assert( ! mid.bad() );
	Id tab2 = shell->doCreate( "Table", nid, "tab2", dims.size() );
	mid = shell->doAddMsg( "Single", ObjId( tab2, 0 ),
		"requestData", ObjId( compt, 0 ), "get_Vm" );

	assert( ! mid.bad() );
	Id ca2( "/n/head2/ca" );
	Id tabCa = shell->doCreate( "Table", nid, "tab3", dims.size() );
	mid = shell->doAddMsg( "Single", ObjId( tabCa, 0 ),
		"requestData", ObjId( ca2, 0 ), "get_Ca" );
	assert( ! mid.bad() );
	//////////////////////////////////////////////////////////////////////
	// Schedule, Reset, and run.
	//////////////////////////////////////////////////////////////////////

	shell->doSetClock( 0, 1.0e-5 );
	shell->doSetClock( 1, 1.0e-5 );
	shell->doSetClock( 2, 1.0e-5 );
	shell->doSetClock( 3, 1.0e-4 );

	shell->doUseClock( "/n/compt,/n/shaft#,/n/head#", "init", 0 );
	shell->doUseClock( "/n/compt,/n/shaft#,/n/head#", "process", 1 );
	shell->doUseClock( "/n/synInput", "process", 1 );
	shell->doUseClock( "/n/compt/Na,/n/compt/K", "process", 2 );
	shell->doUseClock( "/n/head#/#", "process", 2 );
	shell->doUseClock( "/n/tab#", "process", 3 );

	shell->doReinit();
	shell->doReinit();
	shell->doStart( 0.1 );

	SetGet2< string, string >::set( tab, "xplot", "SigNeur.plot", "spineVm" );
	SetGet2< string, string >::set( tab2, "xplot", "SigNeur.plot", "comptVm" );
	SetGet2< string, string >::set( tabCa, "xplot", "SigNeur.plot", "headCa" );

	//////////////////////////////////////////////////////////////////////
	// Build hsolver, Schedule, Reset, and run.
	//////////////////////////////////////////////////////////////////////
	Id hsolve = shell->doCreate( "HSolve", nid, "hsolve", dims.size() );
	shell->doUseClock( "/n/hsolve", "process", 1 );
	Field< double >::set( hsolve, "dt", 2.0e-5 );
	Field< string >::set( hsolve, "target", "/n/compt" );
	shell->doSetClock( 0, 2.0e-5 );
	shell->doSetClock( 1, 2.0e-5 );
	shell->doSetClock( 2, 2.0e-5 );
	shell->doSetClock( 3, 1.0e-4 );


	shell->doReinit();
	shell->doStart( 0.1 );

	SetGet2< string, string >::set( tab, "xplot", "SigNeur.plot", "spineVm_hsolve" );
	SetGet2< string, string >::set( tab2, "xplot", "SigNeur.plot", "comptVm_hsolve" );
	SetGet2< string, string >::set( tabCa, "xplot", "SigNeur.plot", "headCa_hsolve" );

	shell->doDelete( nid );
	cout << "." << flush;
}

Id addPlot( const string& objPath, const string& field,
				const string& plotname )
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );

	Id nid( "/n" );
	assert( nid != Id() );
	Id obj( objPath );
	assert( obj != Id() );
	Id tab = shell->doCreate( "Table", nid, plotname, dims.size() );
	ObjId mid = shell->doAddMsg( "Single", tab, "requestData", obj, field );
	assert( ! mid.bad() );
	return tab;
}

Id buildAdaptorsInCubeMesh( vector< Id >& plots )
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );
	plots.clear();

	vector< Id > spines;
	Id nid = buildSigNeurElec( spines );

	cout << "Cm = " << Field< double >::get( Id( "/n/head2" ), "Cm" ) << endl;

	Id compt( "/n/compt" );
	Field< double >::set( compt, "inject", 0.0e-9 );
	Id synInput( "/n/synInput" );
	Field< double >::set( synInput, "refractT", 87e-3 );
	Id Na( "/n/compt/Na" );
	double gbar = Field< double >::get( Na, "Gbar" );
	Field< double >::set( Na, "Gbar", gbar * 1.5 );

	makeChemInCubeMesh();
	Id elecCa( "/n/head2/ca" );
	Id chemCa( "/n/spineMesh/Ca" );

	Id elecGluR( "/n/head2/gluR" );
	Id chemGluR( "/n/psdMesh/psdGluR" );

	Id elecK( "/n/compt/K" );
	Id chemK( "/n/neuroMesh/kChan" );

	///////////////////////////////////////////////////////////////////
	// Fake diffusion between spines and dend
	///////////////////////////////////////////////////////////////////
	Id dendCa( "/n/neuroMesh/Ca" );
	Id spineMesh( "/n/spineMesh" );
	Id diffReac = shell->doCreate( "Reac", spineMesh, "diff", 1 );
	ObjId mid = shell->doAddMsg( "Single", diffReac, "sub", chemCa, "reac");
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "Single", diffReac, "prd", dendCa, "reac");
	assert( ! mid.bad() );
	double dendVol = Field< double >::get( dendCa, "volume" );
	double spineVol = Field< double >::get( chemCa, "volume" );
	Field< double >::set( diffReac, "Kf", 1 ); // from spine to dend
	Field< double >::set( diffReac, "Kb", spineVol / dendVol );

	///////////////////////////////////////////////////////////////////
	// Adaptors
	///////////////////////////////////////////////////////////////////
	Id adaptCa = shell->doCreate( "Adaptor", nid, "adaptCa", dims.size() );
	mid = shell->doAddMsg( "OneToAll",
					elecCa, "concOut", adaptCa, "input" );
	assert( ! mid.bad() );
	/*
	mid = shell->doAddMsg( "OneToAll",
					adaptCa, "requestOut", elecCa, "get_Ca" );
	assert( ! mid.bad() );
	*/

	mid = shell->doAddMsg( "OneToAll",
					adaptCa, "output", chemCa, "set_conc" );
	assert( ! mid.bad() );
	Field< double >::set( adaptCa, "outputOffset", 0.0001 ); // 100 nM
	Field< double >::set( adaptCa, "scale", 0.05 ); // .06 to .003 mM

	Id adaptGluR = shell->doCreate( "Adaptor", nid, "adaptGluR", dims.size() );
	mid = shell->doAddMsg( "OneToAll",
					adaptGluR, "requestOut", chemGluR, "get_n" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToAll",
					adaptGluR, "output", elecGluR, "set_Gbar" );
	assert( ! mid.bad() );
	// max n = 100, max Gar =
	Field< double >::set( adaptGluR, "scale", 1e-4/100 ); // from n to pS

	Id adaptK = shell->doCreate( "Adaptor", nid, "adaptK", dims.size() );
	mid = shell->doAddMsg( "OneToAll",
					adaptK, "requestOut", chemK, "get_conc" );
	assert( ! mid.bad() );
	mid = shell->doAddMsg( "OneToAll",
					adaptK, "output", elecK, "set_Gbar" );
	assert( ! mid.bad() );
	Field< double >::set( adaptK, "scale", 0.3 ); // from mM to Siemens
		// The starting Gbar is 0.0002827.

	////////////////////////////////////////////////////////////////////
	// Set up the plots, both elec and chem.
	////////////////////////////////////////////////////////////////////
	// plots.push_back( addPlot( "/n/head0", "get_Vm", "spineVm0" ) );
	plots.push_back( addPlot( "/n/head1", "get_Vm", "spineVm1" ) );
	plots.push_back( addPlot( "/n/head2", "get_Vm", "spineVm2" ) );
	// plots.push_back( addPlot( "/n/head3", "get_Vm", "spineVm3" ) );
	// plots.push_back( addPlot( "/n/head4", "get_Vm", "spineVm4" ) );
	plots.push_back( addPlot( "/n/compt", "get_Vm", "dendVm" ) );
	plots.push_back( addPlot( "/n/head2/ca", "get_Ca", "spineElecCa" ) );
	// plots.push_back( addPlot( "/n/spineMesh/Ca", "get_conc", "spineChemCa") );
	// plots.push_back( addPlot( "/n/neuroMesh/Ca", "get_conc", "dendChemCa"));
	plots.push_back( addPlot( "/n/psdMesh/psdGluR", "get_n", "psdGluR_N" ));
	// plots.push_back( addPlot( "/n/head2/gluR", "get_Gbar", "elecGluR_Gbar" ) );
	plots.push_back( addPlot( "/n/head2/gluR", "get_Ik", "elecGluR_Ik2" ) );
	// plots.push_back( addPlot( "/n/head0/gluR", "get_Gk", "elecGluR_Gk0" ) );
	// plots.push_back( addPlot( "/n/head1/gluR", "get_Gk", "elecGluR_Gk1" ) );
	plots.push_back( addPlot( "/n/head2/gluR", "get_Gk", "elecGluR_Gk2" ) );
	// plots.push_back( addPlot( "/n/head3/gluR", "get_Gk", "elecGluR_Gk3" ) );
	// plots.push_back( addPlot( "/n/head4/gluR", "get_Gk", "elecGluR_Gk4" ) );
	// plots.push_back( addPlot( "/n/neuroMesh/kChan", "get_conc", "kChan_conc" ) );
	// plots.push_back( addPlot( "/n/compt/K", "get_Gbar", "kChan_Gbar" ) );
	plots.push_back( addPlot( "/n/spineMesh/toPsd", "get_conc", "toPsd") );

	////////////////////////////////////////////////////////////////////
	return nid;
}

void testAdaptorsInCubeMesh()
{
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	vector< int > dims( 1, 1 );
	vector< Id > plots;
	Id nid = buildAdaptorsInCubeMesh( plots );

	//////////////////////////////////////////////////////////////////////
	// Schedule, Reset, and run.
	//////////////////////////////////////////////////////////////////////

	shell->doSetClock( 0, 20.0e-6 );
	shell->doSetClock( 1, 20.0e-6 );
	shell->doSetClock( 2, 20.0e-6 );
	shell->doSetClock( 5, 1.0e-3 );
	shell->doSetClock( 6, 1.0e-3 );
	shell->doSetClock( 8, 1.0e-4 );

	shell->doUseClock( "/n/compt,/n/shaft#,/n/head#", "init", 0 );
	shell->doUseClock( "/n/compt,/n/shaft#,/n/head#", "process", 1 );
	shell->doUseClock( "/n/synInput", "process", 1 );
	shell->doUseClock( "/n/compt/Na,/n/compt/K", "process", 2 );
	shell->doUseClock( "/n/head#/#", "process", 2 );
	shell->doUseClock( "/n/#Mesh/##", "process", 5 );
	shell->doUseClock( "/n/adapt#", "process", 6 );
	shell->doUseClock( "/n/#[ISA=Table]", "process", 8 );

	shell->doReinit();
	shell->doReinit();
	shell->doStart( 10 );

	for ( vector< Id >::iterator i = plots.begin(); i != plots.end(); ++i )
	{
		SetGet2< string, string >::set( *i, "xplot", "adapt.plot",
						i->element()->getName() );
	}
	//////////////////////////////////////////////////////////////////////
	// Now put in a chemical solver to do the whole chem model
	//////////////////////////////////////////////////////////////////////
	Id solver = shell->doCreate( "GslStoich", nid, "solver", dims.size() );
	assert( solver != Id() );
	Field< string >::set( solver, "path", "/n/##" );
	Field< string >::set( solver, "method", "rk5" );
	shell->doSetClock( 5, 1.0e-3 );
	shell->doSetClock( 6, 1.0e-3 );
	shell->doUseClock( "/n/solver", "process", 5 );
	shell->doReinit();
	shell->doStart( 10.0 );

	for ( vector< Id >::iterator i = plots.begin(); i != plots.end(); ++i )
	{
		SetGet2< string, string >::set( *i, "xplot", "k_adapt.plot",
						i->element()->getName() );
	}
	//////////////////////////////////////////////////////////////////////
	// Build hsolver too.
	//////////////////////////////////////////////////////////////////////
	Id hsolve = shell->doCreate( "HSolve", nid, "hsolve", dims.size() );
	shell->doUseClock( "/n/hsolve", "process", 1 );
	Field< double >::set( hsolve, "dt", 20.0e-6 );
	Field< string >::set( hsolve, "target", "/n/compt" );
	shell->doReinit();
	shell->doStart( 10.0 );

	for ( vector< Id >::iterator i = plots.begin(); i != plots.end(); ++i )
	{
		SetGet2< string, string >::set( *i, "xplot", "hk_adapt.plot",
						i->element()->getName() );
	}
	//////////////////////////////////////////////////////////////////////

	shell->doDelete( nid );
	cout << "." << flush;
}
#endif

// This tests stuff without using the messaging.
void testSigNeur()
{
	testAdaptor();
}

// This is applicable to tests that use the messaging and scheduling.
void testSigNeurProcess()
{
	testAdaptorRequestField();
	// After 2 June 2013, checkin 4579, the tests in
	// testSigNeurElec
	// testChemInCubeMesh
	// testAdaptorsInCubeMesh() // Does a chem+elec model with adaptors
	// have been moved to a
	// separate Python snippet called testSigNeur.py. These tests take
	// too long to run in unit tests and anyway it does not test as much
	// as generate an output that I can compare with the expected one.
	/*
	makeChemInNeuroMesh();
	Id nid = makeChemInCubeMesh();
	Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	shell->doDelete( nid );
	cout << "." << flush;
	*/
}

#else // DO_UNIT_TEST
void testSigNeur()
{;}
void testSigNeurProcess()
{;}
#endif
