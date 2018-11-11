/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
**           copyright (C) 2003-2004 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <fstream>
#include "../basecode/header.h"
#include "../shell/Shell.h"

#define DO_CSPACE_DEBUG 0

#include "ReadCspace.h"

const double ReadCspace::SCALE = 1.0;
const double ReadCspace::DEFAULT_CONC = 1.0;
const double ReadCspace::DEFAULT_RATE = 0.1;
const double ReadCspace::DEFAULT_KM = 1.0;
const bool ReadCspace::USE_PIPE = 1;

ReadCspace::ReadCspace()
	:
		base_(),
		compt_(),
		mesh_(),
		fout_( &cout )
{;}

void ReadCspace::printHeader()
{
	reaclist_.resize( 0 );
	mollist_.resize( 0 );
}

void ReadCspace::printFooter()
{
	string separator = ( USE_PIPE ) ? "|" : "" ;
	// We do this in all cases, regardless of the doOrdering flag.
	sort( mollist_.begin(), mollist_.end() );
	sort( reaclist_.begin(), reaclist_.end() );
	unsigned int i;
	*fout_ << separator;
	for ( i = 0; i < reaclist_.size(); i++ )
		*fout_ << reaclist_[ i ].name() << separator;

	for ( i = 0; i < mollist_.size(); i++ )
		*fout_ << " " << mollist_[i].conc();
	for ( i = 0; i < reaclist_.size(); i++ )
		*fout_ << " " << reaclist_[i].r1() << " " << reaclist_[i].r2();
	*fout_ << "\n";
}

void ReadCspace::printMol( Id id, double conc, double concinit, double vol)
{

	// Skip explicit enzyme complexes.
	ObjId parent = Neutral::parent( id.eref() );
	if ( parent.element()->cinfo()->isA( "Enzyme" ) &&
		id.element()->getName() == ( parent.element()->getName() + "_cplx" )
	)
		return;

	CspaceMolInfo m( id.element()->getName()[ 0 ], concinit );
	mollist_.push_back( m );
	// Need to look up concs in a final step so that the sorted order
	// is maintained.
}

void ReadCspace::printReac( Id id, double kf, double kb)
{
	CspaceReacInfo r( id.element()->getName(), kf, kb );
	reaclist_.push_back( r );
}

void ReadCspace::printEnz( Id id, Id cplx, double k1, double k2, double k3)
{
	CspaceReacInfo r( id.element()->getName(), k3, (k3 + k2) / k1 );
	reaclist_.push_back( r );
}

// Model string always includes topology, after that the parameters
// are filled in according to how many there are.
Id ReadCspace::readModelString( const string& model,
	const string& modelname, Id pa, const string& solverClass )
{
	// Defined in ReadKkit.cpp
	extern Id makeStandardElements( Id pa, const string& modelname );
	unsigned long pos = model.find_first_of( "|" );
	if ( pos == string::npos ) {
		cerr << "ReadCspace::readModelString: Error: model undefined in\n";
		cerr << model << "\n";
		return Id();
	}
	mol_.resize( 0 );
	molseq_.resize( 0 );
	reac_.resize( 0 );
	molparms_.resize( 0 );
	parms_.resize( 0 );
	// Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	base_ = makeStandardElements( pa, modelname );
	assert( base_ != Id() );
	string modelpath = base_.path();
	compt_ = Id( modelpath + "/kinetics");
	assert( compt_ != Id() );
	Field< double >::set( compt_, "volume", 1e-18 );
	// SetGet2< double, unsigned int >::set( compt_, "buildDefaultMesh",     1e-18, 1 );
	string temp = model.substr( pos + 1 );
	pos = temp.find_first_of( " 	\n" );

	for (unsigned long i = 0 ; i < temp.length() && i < pos; i += 5 ) {
		build( temp.c_str() + i );
		if ( temp[ i + 4 ] != '|' )
			break;
	}

	parms_.insert( parms_.begin(), molparms_.begin(), molparms_.end() );

	pos = model.find_last_of( "|" ) + 1;
	double val = 0;
	unsigned int i = 0;
	while ( pos < model.length() ) {
		if ( model[ pos ] == ' ' ) {
			val = atof( model.c_str() + pos + 1 );
			assert( i < parms_.size() );
			parms_[ i++ ] = val;
		}
		pos++;
	}

	deployParameters();

	// SetGet1< string >::set( base_, "build", solverClass );
	return base_;
}

void ReadCspace::makePlots( double plotdt )
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< Id > children;
	Neutral::children( compt_.eref(), children );
	string basepath = base_.path();
	Id graphs( basepath + "/graphs" );
	assert( graphs != Id () );
	for ( unsigned int i = 0; i < children.size(); ++i ) {
		const Cinfo* kidCinfo = children[i].element()->cinfo();
		if ( kidCinfo->isA( "PoolBase" ) ) {
			string plotname = "plot" + children[i].element()->getName();
    		Id tab = shell->doCreate( "Table2", graphs, plotname, 1 );
			assert( tab != Id() );
			// cout << "ReadCspace made plot " << plotname << endl;
			ObjId mid = shell->doAddMsg( "Single",
				tab, "requestOut", children[i], "getConc" );
			assert( mid != ObjId() );
		}
	}
	/* Clocks are now assigned automatically
    shell->doSetClock( 8, plotdt );

    string plotpath = basepath + "/graphs/##[TYPE=Table2]";
    shell->doUseClock( plotpath, "process", 8 );
	*/
}


/////////////////////////////////////////////////////////////////////
//	From reacdef.cpp in CSPACE:
//             if (line == "A <==> B") type = 'A';
//        else if (line == "2A <==> B") type = 'B';
//        else if (line == "A --A--> B") type = 'C';
//        else if (line == "A --B--> B") type = 'D';
//        else if (line == "A <==> B + C") type = 'E';
//        else if (line == "2A <==> B + C") type = 'F';
//        else if (line == "2A + B <==> C") type = 'G';
//        else if (line == "2A + B <==> 2C") type = 'H';
//        else if (line == "4A + B <==> C") type = 'I';
//        else if (line == "A --B--> C") type = 'J';
//        else if (line == "A --A--> B + C") type = 'K';
//        else if (line == "A --B--> B + C") type = 'L';
/////////////////////////////////////////////////////////////////////

void ReadCspace::expandEnzyme(
	const char* name, int e, int s, int p, int p2 )
{
	static Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );

	Id enzMolId = mol_[ name[e] - 'a' ];

	Id enzId = shell->doCreate( "Enz", enzMolId, name, 1 );
	assert( enzId != Id() );
	string cplxName = name;
	cplxName += "_cplx";
	Id cplxId = shell->doCreate( "Pool", enzId, cplxName, 1 );
	assert( cplxId != Id() );
	ObjId ret = shell->doAddMsg( "OneToOne",
		enzId, "cplx", cplxId, "reac" );

	ret = shell->doAddMsg( "OneToOne",
		enzMolId, "reac", enzId, "enz" );

	ret = shell->doAddMsg( "OneToOne",
		mol_[ name[ s ] - 'a' ], "reac", enzId, "sub" );

	ret = shell->doAddMsg( "OneToOne",
		mol_[ name[ p ] - 'a' ], "reac", enzId, "prd" );

	if ( p2 != 0 )
		ret = shell->doAddMsg( "OneToOne",
			mol_[ name[ p2 ] - 'a' ], "reac", enzId, "prd" );

	assert( ret != ObjId() );

	reac_.push_back( enzId );
	parms_.push_back( DEFAULT_RATE );
	parms_.push_back( DEFAULT_KM );
}

void ReadCspace::expandReaction( const char* name, int nm1 )
{
	static Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );

	if ( name[0] == 'C' || name[0] == 'D' || name[0] >= 'J' ) // enzymes
		return;
	int i;

	Id reacId = s->doCreate( "Reac", compt_, name, 1 );

	// A is always a substrate
	for (i = 0; i < nm1; i++ ) {
		s->doAddMsg( "OneToOne", reacId, "sub", mol_[ name[1] - 'a' ], "reac" );
	}

	if ( name[0] < 'G' ) { // B is a product
		s->doAddMsg( "OneToOne", reacId, "prd", mol_[ name[2] - 'a' ], "reac" );
	} else { // B is a substrate
		s->doAddMsg( "OneToOne", reacId, "sub", mol_[ name[2] - 'a' ], "reac" );
	}

	if ( name[0] > 'D' ) { // C is a product
		s->doAddMsg( "OneToOne", reacId, "prd", mol_[ name[3] - 'a' ], "reac" );
	}

	if ( name[0] == 'H' ) { // C is a dual product
		s->doAddMsg( "OneToOne", reacId, "prd", mol_[ name[3] - 'a' ], "reac" );
	}
	reac_.push_back( reacId );
	parms_.push_back( DEFAULT_RATE );
	parms_.push_back( DEFAULT_RATE );
}

void ReadCspace::build( const char* name )
{
	makeMolecule( name[1] );
	makeMolecule( name[2] );
	makeMolecule( name[3] );
	char tname[6];
	strncpy( tname, name, 4 );
	tname[4] = '\0';

	switch ( tname[0] ) {
		case 'A':
		case 'E':
			expandReaction( tname, 1 );
			break;
		case 'B':
		case 'F':
		case 'G':
		case 'H':
			expandReaction( tname, 2 );
			break;
		case 'I':
			expandReaction( tname, 4 );
			break;
		case 'C':
			expandEnzyme( tname, 1, 1, 2 );
			break;
		case 'D':
			expandEnzyme( tname, 2, 1, 2 );
			break;
		case 'J':
			expandEnzyme( tname, 2, 1, 3 );
			break;
		case 'K':
			expandEnzyme( tname, 1, 1, 2, 3 );
			break;
		case 'L':
			expandEnzyme( tname, 2, 1, 2, 3 );
			break;
		default:
			break;
	}
}

void ReadCspace::makeMolecule( char name )
{
	static Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );

	if ( name == 'X' ) // silently ignore it, as it is a legal state
		return;
	if ( name < 'a' || name > 'z' ) {
		cerr << "ReadCspace::makeMolecule Error: name '" << name <<
			"' out of range 'a' to 'z'\n";
		return;
	}

	unsigned int index = 1 + name - 'a';

	// Put in molecule if it is a new one.
	if ( find( molseq_.begin(), molseq_.end(), index - 1 ) ==
					molseq_.end() )
			molseq_.push_back( index - 1 );

	for ( unsigned int i = mol_.size(); i < index; i++ ) {
		string molname("");
		molname += 'a' + i;
		/*
		stringstream ss( "a" );
		ss << i ;
		string molname = ss.str();
		*/
		Id temp = s->doCreate( "Pool", compt_, molname, 1 );
		mol_.push_back( temp );
		molparms_.push_back( DEFAULT_CONC );
	}
}

void ReadCspace::deployParameters( )
{
	// static Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	unsigned long i, j;
	if ( parms_.size() != mol_.size() + 2 * reac_.size() ) {
		cerr << "ReadCspace::deployParameters: Error: # of parms mismatch\n";
		return;
	}
	for ( i = 0; i < mol_.size(); i++ ) {
		// SetField(mol_[ i ], "volscale", volscale );
		// SetField(mol_[ molseq_[i] ], "ninit", parms_[ i ] );

		// Parameters are in micromolar, but the conc units are millimolar.
		Field< double >::set( mol_[i], "concInit", parms_[i] *  1e-3 );
	}
	for ( j = 0; j < reac_.size(); j++ ) {
		if ( reac_[ j ].element()->cinfo()->isA( "Reac" ) ) {
			Field< double >::set( reac_[j], "Kf", parms_[i++] );
			Field< double >::set( reac_[j], "Kb", parms_[i++] );
		} else {
			Field< double >::set( reac_[j], "k3", parms_[i] );
			Field< double >::set( reac_[j], "k2", 4.0 * parms_[i++] );
			// Again, note that conc units in MOOSE are millimolar, so we
			// need to convert from the CSPACE micromolar units.
			Field< double >::set( reac_[j], "Km", parms_[i++] * 1e-3 );
			vector< Id > cplx( 0 );
			Neutral::children( reac_[j].eref(), cplx );
			assert( cplx.size() == 1 );
		}
	}
}

void ReadCspace::testReadModel( )
{
	const double CONCSCALE = 1e-3;
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	// cout << "Testing ReadCspace::readModelString()\n";
	Id modelId = readModelString( "|Habc|Lbca|", "mod1", Id(), "Neutral" );
	assert( mol_.size() == 3 );
	assert( reac_.size() == 2 );

	shell->doDelete( modelId );

	modelId = readModelString( "|AabX|BbcX|CcdX|DdeX|Eefg|Ffgh|Gghi|Hhij|Iijk|Jjkl|Kklm|Llmn| 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0 11.0 12.0 13.0 14.0 101 102 201 202 301 302 401 402 501 502 601 602 701 702 801 802 901 902 1001 1002 1101 1102 1201 1202",
		"model", Id(), "Neutral" );
	assert( mol_.size() == 14 );
	assert( reac_.size() == 12 );
	double concInit;
	int i;
	// cout << "\nTesting ReadCspace:: conc assignment\n";
	for ( i = 0; i < 14; i++ ) {
		string path( base_.path() + "/kinetics/" );
		path += 'a' + i;
		/*
		stringstream ss( "/kinetics/" );
		// const char* molname = ss.str();
		ss << 'a' + i;
		Id temp( ss.str() );
		*/
		Id temp( path );
		concInit = Field< double >::get( temp, "concInit" );

		// The 0.001 is for converting to millimolar, which is the internal
		// unit used in MOOSE.
		assert( doubleEq( concInit, 0.001 * ( i + 1 ) ) );
	}

	// cout << "\nTesting ReadCspace:: reac construction\n";
	assert( reac_[ 0 ].path() == "/model/kinetics/AabX" );
	assert( reac_[ 1 ].path() == "/model/kinetics/BbcX" );
	assert( reac_[ 2 ].path() == "/model/kinetics/c/CcdX" );
	assert( reac_[ 3 ].path() == "/model/kinetics/e/DdeX" );
	assert( reac_[ 4 ].path() == "/model/kinetics/Eefg" );
	assert( reac_[ 5 ].path() == "/model/kinetics/Ffgh" );
	assert( reac_[ 6 ].path() == "/model/kinetics/Gghi" );
	assert( reac_[ 7 ].path() == "/model/kinetics/Hhij" );
	assert( reac_[ 8 ].path() == "/model/kinetics/Iijk" );
	assert( reac_[ 9 ].path() == "/model/kinetics/k/Jjkl" );
	assert( reac_[ 10 ].path() == "/model/kinetics/k/Kklm" );
	assert( reac_[ 11 ].path() == "/model/kinetics/m/Llmn" );

	// cout << "\nTesting ReadCspace:: reac rate assignment\n";
	Id tempA( "/model/kinetics/AabX" );
	double r1 = Field< double >::get( tempA, "Kf");
	double r2 = Field< double >::get( tempA, "Kb");
	assert( doubleEq( r1, 101 ) && doubleEq( r2, 102 ) );

	Id tempB( "/model/kinetics/BbcX" );
	r1 = Field< double >::get( tempB, "Kf");
	r2 = Field< double >::get( tempB, "Kb");
	assert( doubleEq( r1, 201 ) && doubleEq( r2, 202 ) );

	Id tempC( "/model/kinetics/c/CcdX" );
	r1 = Field< double >::get( tempC, "k3");
	r2 = Field< double >::get( tempC, "Km");
	assert( doubleEq( r1, 301 ) && doubleEq( r2, 302 * CONCSCALE ) );

	Id tempD( "/model/kinetics/e/DdeX" );
	r1 = Field< double >::get( tempD, "k3");
	r2 = Field< double >::get( tempD, "Km");
	assert( doubleEq( r1, 401 ) && doubleEq( r2, 402 * CONCSCALE ) );

	for ( i = 4; i < 9; i++ ) {
		/*
		stringstream ss( "/kinetics/A" );
		ss << i << 'a' + i << 'b' + i << 'c' + i;
		// sprintf( ename, "/kinetics/%c%c%c%c", 'A' + i, 'a' + i, 'b' + i, 'c' + i );
		Id temp( ss.str() );
		*/

		string path( "/model/kinetics/" );
		path += 'A' + i;
		path += 'a' + i;
		path += 'b' + i;
		path += 'c' + i;
		Id temp( path );
		r1 = Field< double >::get( temp, "Kf");
		r2 = Field< double >::get( temp, "Kb");
		assert( doubleEq( r1, i* 100 + 101 ) &&
			doubleEq( r2, i * 100 + 102 ) );
	}

	Id tempJ( "/model/kinetics/k/Jjkl" );
	r1 = Field< double >::get( tempJ, "k3");
	r2 = Field< double >::get( tempJ, "Km");
	assert( doubleEq( r1, 1001 ) && doubleEq( r2, 1002 * CONCSCALE ) );

	Id tempK( "/model/kinetics/k/Kklm" );
	r1 = Field< double >::get( tempK, "k3");
	r2 = Field< double >::get( tempK, "Km");
	assert( doubleEq( r1, 1101 ) && doubleEq( r2, 1102 * CONCSCALE ) );

	Id tempL( "/model/kinetics/m/Llmn" );
	r1 = Field< double >::get( tempL, "k3");
	r2 = Field< double >::get( tempL, "Km");
	assert( doubleEq( r1, 1201 ) && doubleEq( r2, 1202 * CONCSCALE ) );
	shell->doDelete( modelId );
}
