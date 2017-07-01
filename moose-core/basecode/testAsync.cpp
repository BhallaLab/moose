/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "global.h"

#include <stdio.h>
#include <iomanip>
#include "../shell/Neutral.h"
#include "../builtins/Arith.h"
#include "Dinfo.h"
#include <queue>
#include "../biophysics/IntFire.h"
#include "../synapse/Synapse.h"
#include "../synapse/SynEvent.h"
#include "../synapse/SynHandlerBase.h"
#include "../synapse/SimpleSynHandler.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "SingleMsg.h"
#include "OneToOneMsg.h"
#include "../scheduling/Clock.h"

#include "../shell/Shell.h"
#include "../mpi/PostMaster.h"

#include "randnum/RNG.h"

int _seed_ = 0;

moose::RNG<double> rng_;

void _mtseed_( unsigned int seed )
{
    _seed_ = seed;
    rng_.setSeed( _seed_ );
}

double _mtrand_( )
{
    return rng_.uniform( );
}

void showFields()
{
	const Cinfo* nc = Neutral::initCinfo();
	Id i1 = Id::nextId();
	Element* ret = new GlobalDataElement( i1, nc, "test1", 1 );
	assert( ret );
	// i1.eref().element()->showFields();
	cout << "." << flush;

	delete i1.element();
}

void testSendMsg()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;

	const DestFinfo* df = dynamic_cast< const DestFinfo* >(
		ac->findFinfo( "setOutputValue" ) );
	assert( df != 0 );
	FuncId fid = df->getFid();

	Id i1 = Id::nextId();
	Id i2 = Id::nextId();
	Element* ret = new GlobalDataElement( i1, ac, "test1", size );
	// bool ret = nc->create( i1, "test1", size );
	assert( ret );
	// ret = nc->create( i2, "test2", size );
	ret = new GlobalDataElement( i2, ac, "test2", size );
	assert( ret );

	Eref e1 = i1.eref();
	Eref e2 = i2.eref();

	Msg* m = new OneToOneMsg( e1, e2, 0 );
	vector< vector< Eref > > ver;
	m->targets( ver );
	assert( ver.size() == size );
	assert( ver[0].size() == 1 );
	assert( ver[0][0].element() == e2.element() );
	assert( ver[0][0].dataIndex() == e2.dataIndex() );
	assert( ver[55].size() == 1 );
	assert( ver[55][0].element() == e2.element() );
	assert( ver[55][0].dataIndex() == 55 );

	SrcFinfo1<double> s( "test", "" );
	s.setBindIndex( 0 );
	e1.element()->addMsgAndFunc( m->mid(), fid, s.getBindIndex() );
	// e1.element()->digestMessages();
	const vector< MsgDigest >& md = e1.element()->msgDigest( 0 );
	assert( md.size() == 1 );
	assert( md[0].targets.size() == 1 );
	assert( md[0].targets[0].element() == e2.element() );
	assert( md[0].targets[0].dataIndex() == e2.dataIndex() );

	for ( unsigned int i = 0; i < size; ++i ) {
		double x = i + i * i;
		s.send( Eref( e1.element(), i ), x );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		double temp = i + i * i;
		double val = reinterpret_cast< Arith* >(e2.element()->data( i ) )->getOutput();
		assert( doubleEq( val, temp ) );
	}
	cout << "." << flush;

	delete i1.element();
	delete i2.element();
}

// This used to use parent/child msg, but that has other implications
// as it causes deletion of elements.
void testCreateMsg()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;
	Id i1 = Id::nextId();
	Id i2 = Id::nextId();
	Element* temp = new GlobalDataElement( i1, ac, "test1", size );
	// bool ret = nc->create( i1, "test1", size );
	assert( temp );
	temp = new GlobalDataElement( i2, ac, "test2", size );
	assert( temp );

	Eref e1 = i1.eref();
	Eref e2 = i2.eref();

	OneToOneMsg *m = new OneToOneMsg( e1, e2, 0 );
	assert( m );
	const Finfo* f1 = ac->findFinfo( "output" );
	assert( f1 );
	const Finfo* f2 = ac->findFinfo( "arg1" );
	assert( f2 );
	bool ret = f1->addMsg( f2, m->mid(), e1.element() );

	assert( ret );
	// e1.element()->digestMessages();

	for ( unsigned int i = 0; i < size; ++i ) {
		const SrcFinfo1< double >* sf = dynamic_cast< const SrcFinfo1< double >* >( f1 );
		assert( sf != 0 );
		sf->send( Eref( e1.element(), i ), double( i ) );
		double val = reinterpret_cast< Arith* >(e2.element()->data( i ) )->getArg1();
		assert( doubleEq( val, i ) );
	}

	/*
	for ( unsigned int i = 0; i < size; ++i )
		cout << i << "	" << reinterpret_cast< Neutral* >(e2.element()->data( i ))->getName() << endl;

*/
	cout << "." << flush;
	delete i1.element();
	delete i2.element();
}

void testSetGet()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;

	string arg;
	Id i2 = Id::nextId();
	Element* ret = new GlobalDataElement( i2, ac, "test2", size );
	assert( ret );
	ProcInfo p;

	for ( unsigned int i = 0; i < size; ++i ) {
		ObjId oid( i2, i );
		double x = i * 3.14;
		bool ret = Field< double >::set( oid, "outputValue", x );
		assert( ret );
		double val = reinterpret_cast< Arith* >(oid.data())->getOutput();
		assert( doubleEq( val, x ) );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		ObjId oid( i2, i );
		double x = i * 3.14;
		double ret = Field< double >::get( oid, "outputValue" );
		ProcInfo p;
		assert( doubleEq( ret, x ) );
	}

	cout << "." << flush;
	delete i2.element();
}

void testStrSet()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;

	string arg;
	Id i2 = Id::nextId();
	Element* ret = new GlobalDataElement( i2, ac, "test2", size );
	assert( ret );
	ProcInfo p;

	Shell::adopt( Id(), i2, 0 );

	assert( ret->getName() == "test2" );
	bool ok = SetGet::strSet( ObjId( i2, 0 ), "name", "NewImprovedTest" );
	assert( ok );
	assert( ret->getName() == "NewImprovedTest" );

	for ( unsigned int i = 0; i < size; ++i ) {
		double x = sqrt((double) i );
		// Eref dest( e2.element(), i );
		ObjId dest( i2, i );
		stringstream ss;
		ss << setw( 10 ) << x;
		ok = SetGet::strSet( dest, "outputValue", ss.str() );
		assert( ok );
		// SetGet1< double >::set( dest, "setOutputValue", x );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		double temp = sqrt((double) i );
		double val = reinterpret_cast< Arith* >(
						Eref( i2.element(), i ).data() )->getOutput();
		assert( fabs( val - temp ) < 1e-5 );
		// DoubleEq won't work here because string is truncated.
	}

	cout << "." << flush;

	delete i2.element();
}

void testGet()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;
	string arg;
	Id i2 = Id::nextId();

	Element* ret = new GlobalDataElement( i2, ac, "test2", size );
	assert( ret );
	ProcInfo p;

	ObjId oid( i2, 0 );

	string val = Field< string >::get( oid, "name" );
	assert( val == "test2" );
	ret->setName( "HupTwoThree" );
	val = Field< string >::get( oid, "name" );
	assert( val == "HupTwoThree" );

	for ( unsigned int i = 0; i < size; ++i ) {
		double temp = i * 3;
		reinterpret_cast< Arith* >(oid.element()->data( i ))->setOutput( temp );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		// Eref dest( e2.element(), i );
		ObjId dest( i2, i );

		double val = Field< double >::get( dest, "outputValue" );
		double temp = i * 3;
		assert( doubleEq( val, temp ) );
	}

	cout << "." << flush;
	delete i2.element();
}

void testStrGet()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;
	string arg;
	Id i2 = Id::nextId();

	Element* ret = new GlobalDataElement( i2, ac, "test2", size );
	assert( ret );
	ProcInfo p;

	ObjId oid( i2, 0 );

	string val;
	bool ok = SetGet::strGet( oid, "name", val );
	assert( ok );
	assert( val == "test2" );
	ret->setName( "HupTwoThree" );
	ok = SetGet::strGet( oid, "name", val );
	assert( ok );
	assert( val == "HupTwoThree" );

	for ( unsigned int i = 0; i < size; ++i ) {
		double temp = i * 3;
		reinterpret_cast< Arith* >( ObjId( i2, i ).data() )->setOutput( temp );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		// Eref dest( e2.element(), i );
		ObjId dest( i2, i );
		ok = SetGet::strGet( dest, "outputValue", val );
		assert( ok );
		double conv = atof( val.c_str() );
		double temp = i * 3;
		assert( fabs( conv - temp ) < 1e-5 );
		// DoubleEq won't work here because string is truncated.
	}

	cout << "." << flush;
	delete i2.element();
}


void testSetGetDouble()
{
	const Cinfo* ic = IntFire::initCinfo();
	unsigned int size = 100;

	string arg;
	Id i2 = Id::nextId();
	// Id i3( i2.value() + 1 );
	Element* ret = new GlobalDataElement( i2, ic, "test2", size );
	assert( ret );
	ProcInfo p;

	for ( unsigned int i = 0; i < size; ++i ) {
		// Eref e2( i2(), i );
		ObjId oid( i2, i );
		double temp = i;
		bool ret = Field< double >::set( oid, "Vm", temp );
		assert( ret );
		assert(
			doubleEq ( reinterpret_cast< IntFire* >(oid.data())->getVm() , temp ) );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		ObjId oid( i2, i );
		double temp = i;
		double ret = Field< double >::get( oid, "Vm" );
		assert( doubleEq( temp, ret ) );
	}

	cout << "." << flush;
	delete i2.element();
	// delete i3.element();
}

void testSetGetSynapse()
{
	const Cinfo* ssh = SimpleSynHandler::initCinfo();
	unsigned int size = 100;

	string arg;
	Id handler = Id::nextId();
	Element* temp = new GlobalDataElement( handler, ssh, "test2", size );
	assert( temp );
	vector< unsigned int > ns( size );
	vector< vector< double > > delay( size );
	for ( unsigned int i = 0; i < size; ++i ) {
		ns[i] = i;
		for ( unsigned int j = 0; j < i; ++j ) {
			double temp = i * 1000 + j;
			delay[i].push_back( temp );
		}
	}

	bool ret = Field< unsigned int >::setVec( handler, "numSynapse", ns );
	assert( ret );
	assert( temp->numData() == size );
	Id syns( handler.value() + 1 );
	for ( unsigned int i = 0; i < size; ++i ) {
		ret = Field< double >::
				setVec( ObjId( syns, i ), "delay", delay[i] );
		if ( i > 0 )
			assert( ret );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		assert( syns.element()->numField( i ) == i );
		SimpleSynHandler* s =
				reinterpret_cast< SimpleSynHandler* >( temp->data( i ) );
		assert( s->getNumSynapses() == i );
		for ( unsigned int j = 0; j < i; ++j ) {
			// ObjId oid( syns, i, j );
			ObjId oid( syns, i, j );
			double x = i * 1000 + j ;
			double d = Field< double >::get( oid, "delay" );
			double d2 = s->getSynapse( j )->getDelay();
			assert( doubleEq( d, x ) );
			assert( doubleEq( d2, x ) );
		}
	}
	delete syns.element();
	delete temp;
	cout << "." << flush;
}

void testSetGetVec()
{
	const Cinfo* sc = SimpleSynHandler::initCinfo();
	unsigned int size = 100;

	string arg;
	Id i2 = Id::nextId();
	Element* temp = new GlobalDataElement( i2, sc, "test2", size );
	assert( temp );

	vector< unsigned int > numSyn( size, 0 );
	for ( unsigned int i = 0; i < size; ++i )
		numSyn[i] = i;

	Eref e2( i2.element(), 0 );
	// Here we test setting a 1-D vector
	bool ret = Field< unsigned int >::setVec( i2, "numSynapse", numSyn );
	assert( ret );

	for ( unsigned int i = 0; i < size; ++i ) {
		SimpleSynHandler* ssh = reinterpret_cast< SimpleSynHandler* >( i2.element()->data( i ) );
		assert( ssh->getNumSynapses() == i );
	}

	vector< unsigned int > getSyn;

	Field< unsigned int >::getVec( i2, "numSynapse", getSyn );
	assert (getSyn.size() == size );
	for ( unsigned int i = 0; i < size; ++i )
		assert( getSyn[i] == i );

	Id synapse( i2.value() + 1 );
	delete synapse.element();
	delete temp;
	cout << "." << flush;
}

void testSendSpike()
{
	static const double WEIGHT = -1.0;
	static const double TAU = 1.0;
	static const double DT = 0.1;
	const Cinfo* ic = IntFire::initCinfo();
	const Cinfo* sc = Synapse::initCinfo();
	unsigned int size = 100;

	string arg;
	Id i2 = Id::nextId();
	Element* temp = new GlobalDataElement( i2, ic, "test2", size );
	assert( temp );
	Id i3 = Id::nextId();
	Element* temp2 = new GlobalDataElement( i3, sc, "syns", size );
	assert( temp2 );
	Eref e2 = i2.eref();
	for ( unsigned int i = 0; i < size; ++i ) {
		ObjId oid( i3, i );
		bool ret = Field< unsigned int >::set( oid, "numSynapses", i );
		assert( ret );
	}

	Id synId( i3.value() + 1 );
	ObjId target( synId , 1 );

	reinterpret_cast< Synapse* >(target.data())->setWeight( WEIGHT );
	reinterpret_cast< Synapse* >(target.data())->setDelay( 0.01 );
	SingleMsg *m = new SingleMsg( e2, target.eref(), 0 );
	const Finfo* f1 = ic->findFinfo( "spikeOut" );
	const Finfo* f2 = sc->findFinfo( "addSpike" );
	bool ret = f1->addMsg( f2, m->mid(), synId.element() );
	assert( ret );

	reinterpret_cast< IntFire* >(e2.data())->setVm( 1.0 );
	// ret = SetGet1< double >::set( e2, "Vm", 1.0 );
	ProcInfo p;
	p.dt = DT;
	reinterpret_cast< IntFire* >(e2.data())->process( e2, &p );
	// At this stage we have sent the spike, so e2.data::Vm should be -1e-7.
	double Vm = reinterpret_cast< IntFire* >(e2.data())->getVm();
	assert( doubleEq( Vm, -1e-7 ) );
	ObjId targetCell( i2, 1 );
	reinterpret_cast< IntFire* >(targetCell.data())->setTau( TAU );

	reinterpret_cast< IntFire* >(targetCell.data())->process( targetCell.eref(), &p );
	Vm = Field< double >::get( targetCell, "Vm" );
	assert( doubleEq( Vm , WEIGHT * ( 1.0 - DT / TAU ) ) );
	delete i2.element();
	delete i3.element();
	delete synId.element();
	cout << "." << flush;
}

void printSparseMatrix( const SparseMatrix< unsigned int >& m)
{
	unsigned int nRows = m.nRows();
	unsigned int nCols = m.nColumns();

	for ( unsigned int i = 0; i < nRows; ++i ) {
		cout << "[	";
		for ( unsigned int j = 0; j < nCols; ++j ) {
			cout << m.get( i, j ) << "	";
		}
		cout << "]\n";
	}
	const unsigned int *n;
	const unsigned int *c;
	for ( unsigned int i = 0; i < nRows; ++i ) {
		unsigned int num = m.getRow( i, &n, &c );
		for ( unsigned int j = 0; j < num; ++j )
			cout << n[j] << "	";
	}
	cout << endl;

	for ( unsigned int i = 0; i < nRows; ++i ) {
		unsigned int num = m.getRow( i, &n, &c );
		for ( unsigned int j = 0; j < num; ++j )
			cout << c[j] << "	";
	}
	cout << endl;
	cout << endl;
}

void testSparseMatrix()
{
	static unsigned int preN[] = { 1, 2, 3, 4, 5, 6, 7 };
	static unsigned int postN[] = { 1, 3, 4, 5, 6, 2, 7 };
	static unsigned int preColIndex[] = { 0, 4, 0, 1, 2, 3, 4 };
	static unsigned int postColIndex[] = { 0, 1, 1, 1, 2, 0, 2 };

	static unsigned int dropN[] = { 1, 6, 2, 7 };
	static unsigned int dropColIndex[] = { 0, 1, 0, 1 };

	SparseMatrix< unsigned int > m( 3, 5 );
	unsigned int nRows = m.nRows();
	unsigned int nCols = m.nColumns();

	m.set( 0, 0, 1 );
	m.set( 0, 4, 2 );
	m.set( 1, 0, 3 );
	m.set( 1, 1, 4 );
	m.set( 1, 2, 5 );
	m.set( 2, 3, 6 );
	m.set( 2, 4, 7 );

	const unsigned int *n;
	const unsigned int *c;
	unsigned int k = 0;
	for ( unsigned int i = 0; i < nRows; ++i ) {
		unsigned int num = m.getRow( i, &n, &c );
		for ( unsigned int j = 0; j < num; ++j ) {
			assert( n[j] == preN[ k ] );
			assert( c[j] == preColIndex[ k ] );
			k++;
		}
	}
	assert( k == 7 );

	// printSparseMatrix( m );

	m.transpose();
	assert( m.nRows() == nCols );
	assert( m.nColumns() == nRows );

	k = 0;
	for ( unsigned int i = 0; i < nCols; ++i ) {
		unsigned int num = m.getRow( i, &n, &c );
		for ( unsigned int j = 0; j < num; ++j ) {
			assert( n[j] == postN[ k ] );
			assert( c[j] == postColIndex[ k ] );
			k++;
		}
	}
	assert( k == 7 );

	// Drop column 1.
	vector< unsigned int > keepCols( 2 );
	keepCols[0] = 0;
	keepCols[1] = 2;
	// cout << endl; m.print();
	m.reorderColumns( keepCols );
	// cout << endl; m.print();
	assert( m.nRows() == nCols );
	assert( m.nColumns() == 2 );

	k = 0;
	for ( unsigned int i = 0; i < nCols; ++i ) {
		unsigned int num = m.getRow( i, &n, &c );
		for ( unsigned int j = 0; j < num; ++j ) {
			assert( n[j] == dropN[ k ] );
			assert( c[j] == dropColIndex[ k ] );
			k++;
		}
	}
	assert( k == 4 );

	cout << "." << flush;
}

void testSparseMatrix2()
{
	// Here zeroes mean no entry, not an entry of zero.
	// Rows 0 to 4 are totally empty
	static unsigned int row5[] = { 1, 0, 2, 0, 0, 0, 0, 0, 0, 0 };
	static unsigned int row6[] = { 0, 0, 3, 4, 0, 0, 0, 0, 0, 0 };
	static unsigned int row7[] = { 0, 0, 0, 0, 5, 0, 0, 0, 0, 6 };
	static unsigned int row8[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	static unsigned int row9[] = { 0, 0, 7, 0, 0, 0, 0, 8, 0, 0 };

	vector< vector < unsigned int > > m( 10 );
	for ( unsigned int i = 0; i < 10; ++i )
		m[i].resize( 10, 0 );

	for ( unsigned int i = 0; i < 10; ++i ) m[5][i] = row5[i];
	for ( unsigned int i = 0; i < 10; ++i ) m[6][i] = row6[i];
	for ( unsigned int i = 0; i < 10; ++i ) m[7][i] = row7[i];
	for ( unsigned int i = 0; i < 10; ++i ) m[8][i] = row8[i];
	for ( unsigned int i = 0; i < 10; ++i ) m[9][i] = row9[i];

	SparseMatrix< unsigned int > n( 10, 10 );
	for ( unsigned int i = 0; i < 10; ++i )
		for ( unsigned int j = 0; j < 10; ++j )
			if ( m[i][j] != 0 )
				n.set( i, j, m[i][j] );

	n.transpose();
	for ( unsigned int i = 0; i < 10; ++i )
		for ( unsigned int j = 0; j < 10; ++j )
			assert (n.get( j, i ) ==  m[i][j] );
	n.transpose();
	for ( unsigned int i = 0; i < 10; ++i )
		for ( unsigned int j = 0; j < 10; ++j )
			assert (n.get( i, j ) ==  m[i][j] );

	///////////////////////////////////////////////////////////////
	// Drop columns 2 and 7.
	///////////////////////////////////////////////////////////////
	static unsigned int init[] = {0, 1, 3, 4, 5, 6, 8, 9};
	vector< unsigned int > keepCols(
					init, init + sizeof( init ) / sizeof( unsigned int ) );
	n.reorderColumns( keepCols );
	for ( unsigned int i = 0; i < 10; ++i ) {
		for ( unsigned int j = 0; j < 8; ++j ) {
			unsigned int k = keepCols[j];
			assert (n.get( i, j ) ==  m[i][k] );
		}
	}
	n.clear();
	n.setSize( 1, 100 );
	for ( unsigned int i = 0; i < 100; ++i )
		n.set( 0, i, 10 * i );
	n.transpose();
	for ( unsigned int i = 0; i < 100; ++i )
		assert( n.get( i, 0 ) == 10 * i );
	n.transpose();
	for ( unsigned int i = 0; i < 100; ++i )
		assert( n.get( 0, i ) == 10 * i );

	/*
	n.printInternal();
	cout << "before transpose\n";
	n.print();
	n.transpose();
	cout << "after transpose\n";
	n.print();
	n.transpose();
	cout << "after transpose back\n";
	n.print();
	*/

	cout << "." << flush;
}

void testSparseMatrixReorder()
{
	SparseMatrix< int > n( 2, 1 );
	n.set( 0, 0, -1 );
	n.set( 1, 0, 1 );
	vector< unsigned int > colOrder( 1, 0 ); // Keep the original as is
	n.reorderColumns( colOrder ); // This case failed in an earlier version
	assert( n.get( 0, 0 ) == -1 );
	assert( n.get( 1, 0 ) == 1 );

	unsigned int nrows = 4;
	unsigned int ncolumns = 5;

	//////////////////////////////////////////////////////////////
	// Test a reordering
	//////////////////////////////////////////////////////////////
	n.setSize( nrows, ncolumns );
	for ( unsigned int i = 0; i < nrows; ++i ) {
		for ( unsigned int j = 0; j < ncolumns; ++j ) {
			int x = i * 10 + j;
			n.set( i, j, x );
		}
	}
	colOrder.resize( ncolumns );
	colOrder[0] = 3;
	colOrder[1] = 2;
	colOrder[2] = 0;
	colOrder[3] = 4;
	colOrder[4] = 1;
	n.reorderColumns( colOrder );
	assert( n.nRows() == nrows );
	assert( n.nColumns() == ncolumns );
	for ( unsigned int i = 0; i < nrows; ++i ) {
		for ( unsigned int j = 0; j < ncolumns; ++j ) {
			int x = i * 10 + colOrder[j];
			assert( n.get( i, j ) == x );
		}
	}

	//////////////////////////////////////////////////////////////
	// Test reordering + eliminating some columns
	//////////////////////////////////////////////////////////////
	// Put back in original config
	for ( unsigned int i = 0; i < nrows; ++i ) {
		for ( unsigned int j = 0; j < ncolumns; ++j ) {
			unsigned int x = i * 10 + j;
			n.set( i, j, x );
		}
	}
	colOrder.resize( 2 );
	colOrder[0] = 3;
	colOrder[1] = 2;
	n.reorderColumns( colOrder );
	assert( n.nRows() == nrows );
	assert( n.nColumns() == 2 );
	for ( unsigned int i = 0; i < nrows; ++i ) {
		assert( n.get( i, 0 ) == static_cast< int >( i * 10 + 3 ) );
		assert( n.get( i, 1 ) == static_cast< int >( i * 10 + 2 ) );
	}
	cout << "." << flush;
}

void testSparseMatrixFill()
{
	SparseMatrix< int > n;
	unsigned int nrow = 5;
	unsigned int ncol = 7;
	vector< unsigned int > row;
	vector< unsigned int > col;
	vector< int > val;
	unsigned int num = 0;
	for ( unsigned int i = 0; i < nrow; ++i ) {
		for ( unsigned int j = 0; j < ncol; ++j ) {
			if ( j == 0 || i + j == 6 || ( j - i) == 2 ) {
				row.push_back( i );
				col.push_back( j );
				val.push_back( 100 + i * 10 + j );
				++num;
			}
		}
	}
	n.tripletFill( row, col, val );
	// n.print();
	assert( n.nRows() == nrow );
	assert( n.nColumns() == ncol );
	assert( n.nEntries() == num );
	for ( unsigned int i = 0; i < nrow; ++i ) {
		for ( unsigned int j = 0; j < ncol; ++j ) {
			int val = n.get( i, j );
			if ( j == 0 || i + j == 6 || ( j - i) == 2 )
				assert( static_cast< unsigned int >( val ) == 100 + i * 10 + j );
			else
				assert( val == 0 );
		}
	}
	cout << "." << flush;
}

void printGrid( Element* e, const string& field, double min, double max )
{
	static string icon = " .oO@";
	unsigned int yside = sqrt( double ( e->numData() ) );
	unsigned int xside = e->numData() / yside;
	if ( e->numData() % yside > 0 )
		xside++;

	for ( unsigned int i = 0; i < e->numData(); ++i ) {
		if ( ( i % xside ) == 0 )
			cout << endl;
		Eref er( e, i );
		ObjId oid( e->id(), i );
		double Vm = Field< double >::get( oid, field );
		int shape = 5.0 * ( Vm - min ) / ( max - min );
		if ( shape > 4 )
			shape = 4;
		if ( shape < 0 )
			shape = 0;
		cout << icon[ shape ];
	}
	cout << endl;
}

void testSparseMsg()
{
	// static const unsigned int NUMSYN = 104576;
	static const double thresh = 0.2;
	static const double Vmax = 1.0;
	static const double refractoryPeriod = 0.4;
	static const double weightMax = 0.02;
	static const double delayMax = 4;
	static const double timestep = 0.2;
	static const double connectionProbability = 0.1;
	static const unsigned int runsteps = 5;
	const Cinfo* ic = IntFire::initCinfo();
	const Cinfo* sshc = SimpleSynHandler::initCinfo();
	const Cinfo* sc = Synapse::initCinfo();
	const Finfo* procFinfo = sshc->findFinfo( "process" );
	assert( procFinfo );
	const DestFinfo* df = dynamic_cast< const DestFinfo* >( procFinfo );
	assert( df );
	// const Cinfo* sc = Synapse::initCinfo();
	unsigned int size = 1024;

	string arg;

	_mtseed_( 5489UL ); // The default value, but better to be explicit.

	Id sshid = Id::nextId();
	Element* t2 = new GlobalDataElement( sshid, sshc, "test2", size );
	assert( t2 );
	Id syns( sshid.value() + 1 );
	Id cells = Id::nextId();
	Element* t3 = new GlobalDataElement( cells, ic, "intFire", size );
	assert( t3 );

	SparseMsg* sm = new SparseMsg( t3, syns.element(), 0 );
	assert( sm );
	const Finfo* f1 = ic->findFinfo( "spikeOut" );
	const Finfo* f2 = sc->findFinfo( "addSpike" );
	assert( f1 && f2 );
	f1->addMsg( f2, sm->mid(), t3 );
	sm->randomConnect( connectionProbability );

	vector< double > temp( size, 0.0 );
	for ( unsigned int i = 0; i < size; ++i )
		temp[i] = _mtrand_() * Vmax;

	bool ret = Field< double >::setVec( cells, "Vm", temp );
	assert( ret );
	temp.clear();
	temp.resize( size, thresh );
	ret = Field< double >::setVec( cells, "thresh", temp );
	assert( ret );
	temp.clear();
	temp.resize( size, refractoryPeriod );
	ret = Field< double >::setVec( cells, "refractoryPeriod", temp );
	assert( ret );

	unsigned int fieldSize = 5000;
	vector< double > weight( size * fieldSize, 0.0 );
	vector< double > delay( size * fieldSize, 0.0 );
	for ( unsigned int i = 0; i < size; ++i ) {
		ObjId id( sshid, i );
		unsigned int numSyn =
				Field< unsigned int >::get( id, "numSynapse" );
		unsigned int k = i * fieldSize;
		for ( unsigned int j = 0; j < numSyn; ++j ) {
			weight[ k + j ] = _mtrand_() * weightMax;
			delay[ k + j ] = _mtrand_() * delayMax;
		}
	}
	ret = Field< double >::setVec( syns, "weight", weight );
	assert( ret );
	ret = Field< double >::setVec( syns, "delay", delay );
	assert( ret );

	// printGrid( cells(), "Vm", 0, thresh );

	ProcInfo p;
	p.dt = timestep;
	for ( unsigned int i = 0; i < runsteps; ++i ) {
		p.currTime += p.dt;
		SetGet1< ProcInfo* >::setRepeat( sshid, "process", &p );
		SetGet1< ProcInfo* >::setRepeat( cells, "process", &p );
		// cells()->process( &p, df->getFid() );
	}

	delete t2;
	delete t3;
	cout << "." << flush;
}

void test2ArgSetVec()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;

	string arg;
	Id i2 = Id::nextId();
	Element* ret = new GlobalDataElement( i2, ac, "test2", size );
	assert( ret );

	vector< double > arg1( size );
	vector< double > arg2( size );
	for ( unsigned int i = 0; i < size; ++i ) {
		arg1[i] = i;
		arg2[i] = 100 * ( 100 - i );
	}

	SetGet2< double, double >::setVec( i2, "arg1x2", arg1, arg2 );

	for ( unsigned int i = 0; i < size; ++i ) {
		ObjId oid( i2, i );
		double x = i * 100 * ( 100 - i );
		double val = reinterpret_cast< Arith* >(oid.data())->getOutput();
		assert( doubleEq( val, x ) );
	}
	cout << "." << flush;
	delete i2.element();
}


class TestId {
	public :
		void setId( Id id ) {
			id_ = id;
		}
		Id getId() const {
			return id_;
		}
		static const Cinfo* initCinfo();
	private :
		Id id_ ;
};
// Here we test setRepeat using an Id field. This test is added
// because of a memory leak problem that cropped up much later.
const Cinfo* TestId::initCinfo()
{
		static ValueFinfo< TestId, Id > id(
			"id",
			"test",
			&TestId::setId,
			&TestId::getId
		);
		static Finfo* testIdFinfos[] = {&id};
		static Cinfo testIdCinfo(
			"TestIdRepeatAssignment",
			Neutral::initCinfo(),
			testIdFinfos,
			sizeof( testIdFinfos )/ sizeof( Finfo* ),
			new Dinfo< TestId >()
		);
		return &testIdCinfo;
}

void testSetRepeat()
{
	const Cinfo* sc = SimpleSynHandler::initCinfo();
	unsigned int size = 100;

	string arg;
	Id cell = Id::nextId();
	Element* temp = new GlobalDataElement( cell, sc, "cell", size );
	assert( temp );
	vector< unsigned int > numSyn( size, 0 );
	for ( unsigned int i = 0; i < size; ++i )
		numSyn[i] = i;

	// Here we test setting a 1-D vector
	bool ret = Field< unsigned int >::setVec( cell, "numSynapse", numSyn);
	assert( ret );

	Id synapse( cell.value() + 1 );
	// Here we test setting a 2-D array with different dims on each axis.
	for ( unsigned int i = 0; i < size; ++i ) {
		ret = Field< double >::
				setRepeat( ObjId( synapse, i ), "delay", 123.0 );
		assert( ret );
	}
	for ( unsigned int i = 0; i < size; ++i ) {
		vector< double > delay;
		Field< double >::getVec( ObjId( synapse, i ), "delay", delay );
		assert( delay.size() == i );
		for ( unsigned int j = 0; j < i; ++j ) {
			assert( doubleEq( delay[j], 123.0 ) );
		}
	}

	delete synapse.element();
	delete temp;
	cout << "." << flush;
}

/**
 * This sets up a reciprocal shared Msg in which the incoming value gets
 * appended onto the corresponding value of the target. Also, as soon
 * as any of the s1 or s2 are received, the target sends out an s0 call.
 * All this is tallied for validating the unit test.
 */

static SrcFinfo0 s0( "s0", "");
class Test
{
	public:
		Test()
			: numAcks_( 0 )
		{;}

		void process( const Eref& e, ProcPtr p )
		{;}

		void handleS0() {
			numAcks_++;
		}

		void handleS1( const Eref& e, string s ) {
			s_ = s + s_;
			s0.send( e );
		}

		void handleS2( const Eref& e, int i1, int i2 ) {
			i1_ += 10 * i1;
			i2_ += 10 * i2;
			s0.send( e );
		}

		static Finfo* sharedVec[ 6 ];

		static const Cinfo* initCinfo()
		{
			static SharedFinfo shared( "shared", "",
				sharedVec, sizeof( sharedVec ) / sizeof( const Finfo * ) );
			static Finfo * testFinfos[] = {
				&shared,
			};

			static Dinfo< Test > dinfo;
			static Cinfo testCinfo(
				"Test",
				0,
				testFinfos,
				sizeof( testFinfos ) / sizeof( Finfo* ),
				&dinfo
			);

			return &testCinfo;
		}

		string s_;
		int i1_;
		int i2_;
		int numAcks_;
};

Finfo* Test::sharedVec[6];

void testSharedMsg()
{
	static SrcFinfo1< string > s1( "s1", "" );
	static SrcFinfo2< int, int > s2( "s2", "" );
	static DestFinfo d0( "d0", "",
		new OpFunc0< Test >( & Test::handleS0 ) );
	static DestFinfo d1( "d1", "",
		new EpFunc1< Test, string >( &Test::handleS1 ) );
	static DestFinfo d2( "d2", "",
		new EpFunc2< Test, int, int >( &Test::handleS2 ) );

	Test::sharedVec[0] = &s0;
	Test::sharedVec[1] = &d0;
	Test::sharedVec[2] = &s1;
	Test::sharedVec[3] = &d1;
	Test::sharedVec[4] = &s2;
	Test::sharedVec[5] = &d2;

	Id t1 = Id::nextId();
	Id t2 = Id::nextId();
	// bool ret = Test::initCinfo()->create( t1, "test1", 1 );

	Element* temp = new GlobalDataElement( t1, Test::initCinfo(), "test1", 1 );
	assert( temp );
	temp = new GlobalDataElement( t2, Test::initCinfo(), "test2", 1 );
	// ret = Test::initCinfo()->create( t2, "test2", 1 );
	assert( temp );

	// Assign initial values
	Test* tdata1 = reinterpret_cast< Test* >( t1.eref().data() );
	tdata1->s_ = "tdata1";
	tdata1->i1_ = 1;
	tdata1->i2_ = 2;

	Test* tdata2 = reinterpret_cast< Test* >( t2.eref().data() );
	tdata2->s_ = "TDATA2";
	tdata2->i1_ = 5;
	tdata2->i2_ = 6;

	// Set up message. The actual routine is in Shell.cpp, but here we
	// do it independently.

	const Finfo* shareFinfo = Test::initCinfo()->findFinfo( "shared" );
	assert( shareFinfo != 0 );
	Msg* m = new OneToOneMsg( t1.eref(), t2.eref(), 0 );
	assert( m != 0 );
	bool ret = shareFinfo->addMsg( shareFinfo, m->mid(), t1.element() );
	assert( ret );

	// t1.element()->digestMessages();
	// t2.element()->digestMessages();
	// Display stuff. Need to figure out how to unit test this.
	// t1()->showMsg();
	// t2()->showMsg();

	// Send messages
	ProcInfo p;
	string arg1 = " hello ";
	s1.send( t1.eref(), arg1 );
	s2.send( t1.eref(), 100, 200 );

	string arg2 = " goodbye ";
	s1.send( t2.eref(), arg2 );
	s2.send( t2.eref(), 500, 600 );

	/*
	cout << "data1: s=" << tdata1->s_ <<
		", i1=" << tdata1->i1_ << ", i2=" << tdata1->i2_ <<
		", numAcks=" << tdata1->numAcks_ << endl;
	cout << "data2: s=" << tdata2->s_ <<
		", i1=" << tdata2->i1_ << ", i2=" << tdata2->i2_ <<
		", numAcks=" << tdata2->numAcks_ << endl;
	*/
	// Check results

	assert( tdata1->s_ == " goodbye tdata1" );
	assert( tdata2->s_ == " hello TDATA2" );
	assert( tdata1->i1_ == 5001  );
	assert( tdata1->i2_ == 6002  );
	assert( tdata2->i1_ == 1005  );
	assert( tdata2->i2_ == 2006  );
	assert( tdata1->numAcks_ == 2  );
	assert( tdata2->numAcks_ == 2  );

	t1.destroy();
	t2.destroy();

	cout << "." << flush;
}

void testConvVector()
{
	vector< unsigned int > intVec;
	for ( unsigned int i = 0; i < 5; ++i )
		intVec.push_back( i * i );

	double buf[500];
	double* tempBuf = buf;

	Conv< vector< unsigned int > > intConv;
	assert( intConv.size( intVec ) == 1 + intVec.size() );
	intConv.val2buf( intVec, &tempBuf );
	assert( tempBuf == buf + 6 );
	assert( buf[0] == intVec.size() );
	assert( static_cast< unsigned int >( buf[1] ) == intVec[0] );
	assert( static_cast< unsigned int >( buf[2] ) == intVec[1] );
	assert( static_cast< unsigned int >( buf[3] ) == intVec[2] );
	assert( static_cast< unsigned int >( buf[4] ) == intVec[3] );
	assert( static_cast< unsigned int >( buf[5] ) == intVec[4] );

	tempBuf = buf;
	const vector< unsigned int >& testIntVec = intConv.buf2val( &tempBuf );

	assert( intVec.size() == testIntVec.size() );
	for ( unsigned int i = 0; i < intVec.size(); ++i ) {
		assert( intVec[ i ] == testIntVec[i] );
	}

	vector< string > strVec;
	strVec.push_back( "one" );
	strVec.push_back( "two" );
	strVec.push_back( "three and more and more and more" );
	strVec.push_back( "four and yet more" );

	tempBuf = buf;
	Conv< vector< string > >::val2buf( strVec, &tempBuf );
	unsigned int sz = Conv< vector< string > >::size( strVec );
	assert( sz == 1 + 2 + ( strVec[2].length() + 8) /8 + ( strVec[3].length() + 8 )/8 );
	assert( buf[0] == 4 );
	assert( strcmp( reinterpret_cast< char* >( buf + 1 ), "one" ) == 0 );

	tempBuf = buf;
	const vector< string >& tgtStr =
			Conv< vector< string > >::buf2val( &tempBuf );
	assert( tgtStr.size() == 4 );
	for ( unsigned int i = 0; i < 4; ++i )
		assert( tgtStr[i] == strVec[i] );

	cout << "." << flush;
}

void testConvVectorOfVectors()
{
	short *row0 = 0;
	short row1[] = { 1 };
	short row2[] = { 2, 3 };
	short row3[] = { 4, 5, 6 };
	short row4[] = { 7, 8, 9, 10 };
	short row5[] = { 11, 12, 13, 14, 15 };

	vector< vector < short > > vec( 6 );
	vec[0].insert( vec[0].end(), row0, row0 + 0 );
	vec[1].insert( vec[1].end(), row1, row1 + 1 );
	vec[2].insert( vec[2].end(), row2, row2 + 2 );
	vec[3].insert( vec[3].end(), row3, row3 + 3 );
	vec[4].insert( vec[4].end(), row4, row4 + 4 );
	vec[5].insert( vec[5].end(), row5, row5 + 5 );

	double expected[] = {
		6,  // Number of sub-vectors
   		0,		// No entries on first sub-vec
		1,		1,
		2,		2,3,
		3,		4,5,6,
		4,		7,8,9,10,
		5,		11,12,13,14,15
	};

	double origBuf[500];
	double* buf = origBuf;

	Conv< vector< vector< short > > > conv;

	assert( conv.size( vec ) == 1 + 6 + 0 + 1 + 2 + 3 + 4 + 5 ); // 21
	conv.val2buf( vec, &buf );
	assert( buf == 22 + origBuf );
	for ( unsigned int i = 0; i < 22; ++i )
		assert( doubleEq( origBuf[i], expected[i] ) );

	double* buf2 = origBuf;
	const vector< vector< short > >& rc = conv.buf2val( &buf2 );

	assert( rc.size() == 6 );
	for ( unsigned int i = 0; i < 6; ++i ) {
		assert( rc[i].size() == i );
		for ( unsigned int j = 0; j < i; ++j )
			assert( rc[i][j] == vec[i][j] );
	}

	cout << "." << flush;
}

void testMsgField()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 10;

	const DestFinfo* df = dynamic_cast< const DestFinfo* >(
		ac->findFinfo( "setOutputValue" ) );
	assert( df != 0 );
	FuncId fid = df->getFid();

	Id i1 = Id::nextId();
	Id i2 = Id::nextId();
	Element* ret = new GlobalDataElement( i1, ac, "test1", size );
	assert( ret );
	ret = new GlobalDataElement( i2, ac, "test2", size );
	assert( ret );

	Eref e1 = i1.eref();

	Msg* m = new SingleMsg( Eref( i1.element(), 5 ), Eref( i2.element(), 3 ), 0 );
	ProcInfo p;

	assert( m->mid().element()->getName() == "singleMsg" );

	SingleMsg* sm = reinterpret_cast< SingleMsg* >( m->mid().data() );
	assert( sm );
	assert ( sm == m );
	assert( sm->getI1() == 5 );
	assert( sm->getI2() == 3 );

	SrcFinfo1<double> s( "test", "" );
	s.setBindIndex( 0 );
	e1.element()->addMsgAndFunc( m->mid(), fid, s.getBindIndex() );

	for ( unsigned int i = 0; i < size; ++i ) {
		double x = i * 42;
		s.send( Eref( e1.element(), i ), x );
	}

	// Check that regular msgs go through.
	Eref tgt3( i2.element(), 3 );
	Eref tgt8( i2.element(), 8 );
	double val = reinterpret_cast< Arith* >( tgt3.data() )->getOutput();
	assert( doubleEq( val, 5 * 42 ) );
	val = reinterpret_cast< Arith* >( tgt8.data() )->getOutput();
	assert( doubleEq( val, 0 ) );

	// Now change I1 and I2, rerun, and check.
	sm->setI1( 9 );
	sm->setI2( 8 );
	for ( unsigned int i = 0; i < size; ++i ) {
		double x = i * 1000;
		s.send( Eref( e1.element(), i ), x );
	}
	val = reinterpret_cast< Arith* >( tgt3.data() )->getOutput();
	assert( doubleEq( val, 5 * 42 ) );
	val = reinterpret_cast< Arith* >( tgt8.data() )->getOutput();
	assert( doubleEq( val, 9000 ) );

	cout << "." << flush;

	delete i1.element();
	delete i2.element();
}

void testSetGetExtField()
{
	const Cinfo* nc = Neutral::initCinfo();
	const Cinfo* rc = Arith::initCinfo();
	unsigned int size = 100;

	string arg;
	Id i1 = Id::nextId();
	Id i2( i1.value() + 1 );
	Id i3( i2.value() + 1 );
	Id i4( i3.value() + 1 );
	Element* e1 = new GlobalDataElement( i1, nc, "test", size );
	assert( e1 );
	Shell::adopt( Id(), i1, 0 );
	Element* e2 = new GlobalDataElement( i2, rc, "x", size );
	assert( e2 );
	Shell::adopt( i1, i2, 0 );
	Element* e3 = new GlobalDataElement( i3, rc, "y", size );
	assert( e3 );
	Shell::adopt( i1, i3, 0 );
	Element* e4 = new GlobalDataElement( i4, rc, "z", size );
	assert( e4 );
	Shell::adopt( i1, i4, 0 );
	bool ret;

	vector< double > vec;
	for ( unsigned int i = 0; i < size; ++i ) {
		ObjId a( i1, i );
		ObjId b( i1, size - i - 1);
		// Eref a( e1, i );
		// Eref b( e1, size - i - 1 );
		double temp = i;
		ret = Field< double >::set( a, "x", temp );
		assert( ret );
		double temp2  = temp * temp;
		ret = Field< double >::set( b, "y", temp2 );
		assert( ret );
		vec.push_back( temp2 - temp );
	}

	ret = Field< double >::setVec( i1, "z", vec );
	assert( ret );

	for ( unsigned int i = 0; i < size; ++i ) {
		/*
		Eref a( e2, i );
		Eref b( e3, size - i - 1 );
		Eref c( e4, i );
		*/
		ObjId a( i2, i );
		ObjId b( i3, size - i - 1 );
		ObjId c( i4, i );
		double temp = i;
		double temp2  = temp * temp;

		double v = reinterpret_cast< Arith* >(a.data() )->getOutput();
		assert( doubleEq( v, temp ) );

		v = reinterpret_cast< Arith* >(b.data() )->getOutput();
		assert( doubleEq( v, temp2 ) );

		v = reinterpret_cast< Arith* >( c.data() )->getOutput();
		assert( doubleEq( v, temp2 - temp ) );
	}

	for ( unsigned int i = 0; i < size; ++i ) {
		// Eref a( e1, i );
		// Eref b( e1, size - i - 1 );
		ObjId a( i1, i );
		ObjId b( i1, size - i - 1 );

		double temp = i;
		double temp2  = temp * temp;
		double ret = Field< double >::get( a, "x" );
		assert( doubleEq( temp, ret ) );

		ret = Field< double >::get( b, "y" );
		assert( doubleEq( temp2, ret ) );

		ret = Field< double >::get( a, "z" );
		assert( doubleEq( temp2 - temp, ret ) );
		// cout << i << "	" << ret << "	temp2 = " << temp2 << endl;
	}

	cout << "." << flush;

	/*
	* This works, but I want to avoid calling the Shell specific ops here
	*
	* Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	* s->doDelete( i1 );
	*/
	i4.destroy();
	i3.destroy();
	i2.destroy();
	i1.destroy();
}

void testLookupSetGet()
{
	const Cinfo* ac = Arith::initCinfo();
	unsigned int size = 100;
	string arg;
	Id i2 = Id::nextId();

	Element* elm = new GlobalDataElement( i2, ac, "test2", size );
	assert( elm );
	ObjId obj( i2, 0 );

	Arith* arith = reinterpret_cast< Arith* >(obj.data() );
	for ( unsigned int i = 0; i < 4; ++i )
		arith->setIdentifiedArg( i, 0 );
	for ( unsigned int i = 0; i < 4; ++i )
		assert( doubleEq( 0, arith->getIdentifiedArg( i ) ) );

	LookupField< unsigned int, double >::set( obj, "anyValue", 0, 100 );
	LookupField< unsigned int, double >::set( obj, "anyValue", 1, 101 );
	LookupField< unsigned int, double >::set( obj, "anyValue", 2, 102 );
	LookupField< unsigned int, double >::set( obj, "anyValue", 3, 103 );

	assert( doubleEq( arith->getOutput(), 100 ) );
	assert( doubleEq( arith->getArg1(), 101 ) );
	assert( doubleEq( arith->getIdentifiedArg( 2 ), 102 ) );
	assert( doubleEq( arith->getIdentifiedArg( 3 ), 103 ) );

	for ( unsigned int i = 0; i < 4; ++i )
		arith->setIdentifiedArg( i, 17 * i + 3 );

	double ret = LookupField< unsigned int, double >::get(
		obj, "anyValue", 0 );
	assert( doubleEq( ret, 3 ) );

	ret = LookupField< unsigned int, double >::get( obj, "anyValue", 1 );
	assert( doubleEq( ret, 20 ) );

	ret = LookupField< unsigned int, double >::get( obj, "anyValue", 2 );
	assert( doubleEq( ret, 37 ) );

	ret = LookupField< unsigned int, double >::get( obj, "anyValue", 3 );
	assert( doubleEq( ret, 54 ) );

	cout << "." << flush;
	i2.destroy();
}

void testIsA()
{
	const Cinfo* n = Neutral::initCinfo();
	const Cinfo* a = Arith::initCinfo();
	assert( a->isA( "Arith" ) );
	assert( a->isA( "Neutral" ) );
	assert( !a->isA( "Fish" ) );
	assert( !a->isA( "Synapse" ) );
	assert( !n->isA( "Arith" ) );
	assert( n->isA( "Neutral" ) );
	cout << "." << flush;
}

void testFinfoFields()
{
	const FinfoWrapper vmFinfo = IntFire::initCinfo()->findFinfo( "Vm" );
	const FinfoWrapper synFinfo = SimpleSynHandler::initCinfo()->findFinfo( "synapse" );
	const FinfoWrapper procFinfo = IntFire::initCinfo()->findFinfo( "proc" );
	const FinfoWrapper processFinfo = IntFire::initCinfo()->findFinfo( "process" );
	const FinfoWrapper reinitFinfo = IntFire::initCinfo()->findFinfo( "reinit" );
	const FinfoWrapper spikeFinfo = IntFire::initCinfo()->findFinfo( "spikeOut" );
	const FinfoWrapper classNameFinfo = Neutral::initCinfo()->findFinfo( "className" );

	assert( vmFinfo.getName() == "Vm" );
	assert( vmFinfo.docs() == "Membrane potential" );
	assert( vmFinfo.src().size() == 0 );
	assert( vmFinfo.dest().size() == 2 );
	assert( vmFinfo.dest()[0] == "setVm" );
	assert( vmFinfo.dest()[1] == "getVm" );
	assert( vmFinfo.type() == "double" );

	assert( synFinfo.getName() == "synapse" );
	assert( synFinfo.docs() == "Sets up field Elements for synapse" );
	assert( synFinfo.src().size() == 0 );
	assert( synFinfo.dest().size() == 0 );
	// cout <<  synFinfo->type() << endl;
	assert( synFinfo.type() == typeid(Synapse).name() );

	assert( procFinfo.getName() == "proc" );
	assert( procFinfo.docs() == "Shared message for process and reinit" );
	assert( procFinfo.src().size() == 0 );
	assert( procFinfo.dest().size() == 2 );
	assert( procFinfo.dest()[0] == "process" );
	assert( procFinfo.dest()[1] == "reinit" );
	 // cout << "proc " << procFinfo.type() << endl;
	assert( procFinfo.type() == "void" );

	assert( processFinfo.getName() == "process" );
	assert( processFinfo.docs() == "Handles process call" );
	assert( processFinfo.src().size() == 0 );
	assert( processFinfo.dest().size() == 0 );
	// cout << "process " << processFinfo.type() << endl;
	assert( processFinfo.type() == "const ProcInfo*" );

	assert( reinitFinfo.getName() == "reinit" );
	assert( reinitFinfo.docs() == "Handles reinit call" );
	assert( reinitFinfo.src().size() == 0 );
	assert( reinitFinfo.dest().size() == 0 );
	// cout << "reinit " << reinitFinfo.type() << endl;
	assert( reinitFinfo.type() == "const ProcInfo*" );

	assert( spikeFinfo.getName() == "spikeOut" );
	assert( spikeFinfo.docs() == "Sends out spike events. The argument is the timestamp of the spike. " );
	assert( spikeFinfo.src().size() == 0 );
	assert( spikeFinfo.dest().size() == 0 );
	// cout << spikeFinfo->type() << endl;
	assert( spikeFinfo.type() == "double" );

	assert( classNameFinfo.getName() == "className" );
	assert( classNameFinfo.type() == "string" );

	cout << "." << flush;
}

void testCinfoFields()
{
	assert( IntFire::initCinfo()->getDocs() == "" );
	assert( SimpleSynHandler::initCinfo()->getBaseClass() == "SynHandlerBase" );

	// We have a little bit of a hack here to cast away
	// constness, due to the way the FieldElementFinfos
	// are set up.
	Cinfo *neutralCinfo = const_cast< Cinfo* >( Neutral::initCinfo() );
	assert( neutralCinfo->getNumSrcFinfo() == 1 );

	Cinfo *cinfo = const_cast< Cinfo* >( IntFire::initCinfo() );
	unsigned int nsf = neutralCinfo->getNumSrcFinfo();
	assert( nsf == 1 );
	assert( cinfo->getNumSrcFinfo() == 1 + nsf );
	assert( cinfo->getSrcFinfo( 0 + nsf ) == cinfo->findFinfo( "spikeOut" ) );

	unsigned int ndf = neutralCinfo->getNumDestFinfo();
	assert( ndf == 29 );
	unsigned int sdf = IntFire::initCinfo()->getNumDestFinfo();
	assert( sdf == 40 );

	/*
	assert( cinfo->getDestFinfo( 0+ndf )->name() == "setNumSynapses" );
	assert( cinfo->getDestFinfo( 1+ndf )->name() == "getNumSynapses" );
	assert( cinfo->getDestFinfo( 2+ndf )->name() == "setNumSynapse" );
	assert( cinfo->getDestFinfo( 3+ndf )->name() == "getNumSynapse" );
	*/

	assert( cinfo->getDestFinfo( 0+ndf ) == cinfo->findFinfo( "setVm" ) );
	assert( cinfo->getDestFinfo( 1+ndf ) == cinfo->findFinfo( "getVm" ) );
	assert( cinfo->getDestFinfo( 2+ndf ) == cinfo->findFinfo( "setTau" ) );
	assert( cinfo->getDestFinfo( 3+ndf ) == cinfo->findFinfo( "getTau" ) );

	assert( cinfo->getDestFinfo( 4+ndf ) == cinfo->findFinfo( "setThresh" ) );
	assert( cinfo->getDestFinfo( 5+ndf ) == cinfo->findFinfo( "getThresh" ) );
	assert( cinfo->getDestFinfo( 6+ndf ) == cinfo->findFinfo( "setRefractoryPeriod" ) );
	assert( cinfo->getDestFinfo( 7+ndf ) == cinfo->findFinfo( "getRefractoryPeriod" ) );
	assert( cinfo->getDestFinfo( 8+ndf ) == cinfo->findFinfo( "activation" ) );
	assert( cinfo->getDestFinfo( 9+ndf ) == cinfo->findFinfo( "process" ) );
	assert( cinfo->getDestFinfo( 10+ndf ) == cinfo->findFinfo( "reinit" ) );


	unsigned int nvf = neutralCinfo->getNumValueFinfo();
	assert( nvf == 19 );
	assert( cinfo->getNumValueFinfo() == 4 + nvf );
	assert( cinfo->getValueFinfo( 0 + nvf ) == cinfo->findFinfo( "Vm" ) );
	assert( cinfo->getValueFinfo( 1 + nvf ) == cinfo->findFinfo( "tau" ) );
	assert( cinfo->getValueFinfo( 2 + nvf ) == cinfo->findFinfo( "thresh" ) );
	assert( cinfo->getValueFinfo( 3 + nvf ) == cinfo->findFinfo( "refractoryPeriod" ) );

	unsigned int nlf = neutralCinfo->getNumLookupFinfo();
	assert( nlf == 4 ); // Neutral inserts a lookup field for neighbors
	assert( cinfo->getNumLookupFinfo() == 0 + nlf );
	assert( cinfo->getLookupFinfo( 0 + nlf )->name() == "dummy");

	unsigned int nshf = neutralCinfo->getNumSharedFinfo();
	assert( nshf == 0 );
	assert( cinfo->getNumSharedFinfo() == 1 + nshf );
	assert( cinfo->getSharedFinfo( 0 + nshf ) == cinfo->findFinfo( "proc" ) );

	cout << "." << flush;
}

void testCinfoElements()
{
	Id intFireCinfoId( "/classes/IntFire" );
	// const Cinfo *neutralCinfo = Neutral::initCinfo();
	// unsigned int nvf = neutralCinfo->getNumValueFinfo();
	// unsigned int nsf = neutralCinfo->getNumSrcFinfo();
	// unsigned int ndf = neutralCinfo->getNumDestFinfo();
	//unsigned int sdf = SynHandler::initCinfo()->getNumDestFinfo();

	assert( intFireCinfoId != Id() );
	assert( Field< string >::get( intFireCinfoId, "name" ) == "IntFire" );
	assert( Field< string >::get( intFireCinfoId, "baseClass" ) ==
					"Neutral" );
	Id intFireValueFinfoId( "/classes/IntFire/valueFinfo" );
	unsigned int n = Field< unsigned int >::get(
		intFireValueFinfoId, "numData" );
	assert( n == 4 );
	Id intFireSrcFinfoId( "/classes/IntFire/srcFinfo" );
	assert( intFireSrcFinfoId != Id() );
	n = Field< unsigned int >::get( intFireSrcFinfoId, "numData" );
	assert( n == 1 );
	Id intFireDestFinfoId( "/classes/IntFire/destFinfo" );
	assert( intFireDestFinfoId != Id() );
	n = Field< unsigned int >::get( intFireDestFinfoId, "numData" );
	assert( n == 11 );

	ObjId temp( intFireSrcFinfoId, 0 );
	string foo = Field< string >::get( temp, "fieldName" );
	assert( foo == "spikeOut" );

	foo = Field< string >::get( temp, "type" );
	assert( foo == "double" );

	n = Field< unsigned int >::get( intFireDestFinfoId, "numField" );
	assert( n == 1 );

	temp = ObjId( intFireDestFinfoId, 7 );
	string str = Field< string >::get( temp, "fieldName" );
	assert( str == "getRefractoryPeriod");
	temp = ObjId( intFireDestFinfoId, 10 );
	str = Field< string >::get( temp, "fieldName" );
	assert( str == "reinit" );
	cout << "." << flush;
}

void testMsgSrcDestFields()
{
	//////////////////////////////////////////////////////////////
	// Setup
	//////////////////////////////////////////////////////////////
	/* This is initialized in testSharedMsg()
	static SrcFinfo1< string > s1( "s1", "" );
	static SrcFinfo2< int, int > s2( "s2", "" );
	static DestFinfo d0( "d0", "",
		new OpFunc0< Test >( & Test::handleS0 ) );
	static DestFinfo d1( "d1", "",
		new EpFunc1< Test, string >( &Test::handleS1 ) );
	static DestFinfo d2( "d2", "",
		new EpFunc2< Test, int, int >( &Test::handleS2 ) );

	Test::sharedVec[0] = &s0;
	Test::sharedVec[1] = &d0;
	Test::sharedVec[2] = &s1;
	Test::sharedVec[3] = &d1;
	Test::sharedVec[4] = &s2;
	Test::sharedVec[5] = &d2;
	*/

	Id t1 = Id::nextId();
	Id t2 = Id::nextId();
	// bool ret = Test::initCinfo()->create( t1, "test1", 1 );
	Element* e1 = new GlobalDataElement( t1, Test::initCinfo(), "test1" );
	assert( e1 );
	assert( e1 == t1.element() );
	Element* e2 = new GlobalDataElement( t2, Test::initCinfo(), "test2", 1 );
	// ret = Test::initCinfo()->create( t2, "test2", 1 );
	assert( e2 );
	assert( e2 == t2.element() );

	// Set up message. The actual routine is in Shell.cpp, but here we
	// do it independently.
	const Finfo* shareFinfo = Test::initCinfo()->findFinfo( "shared" );
	assert( shareFinfo != 0 );
	Msg* m = new OneToOneMsg( t1.eref(), t2.eref(), 0 );
	assert( m != 0 );
	bool ret = shareFinfo->addMsg( shareFinfo, m->mid(), t1.element() );
	assert( ret );

	//////////////////////////////////////////////////////////////
	// Test Element::getFieldsOfOutgoingMsg
	//////////////////////////////////////////////////////////////
	vector< pair< BindIndex, FuncId > > pairs;
	e1->getFieldsOfOutgoingMsg( m->mid(), pairs );
	assert( pairs.size() == 3 );
	assert( pairs[0].first == dynamic_cast< SrcFinfo* >(Test::sharedVec[0])->getBindIndex() );
	assert( pairs[0].second == dynamic_cast< DestFinfo* >(Test::sharedVec[1])->getFid() );

	assert( pairs[1].first == dynamic_cast< SrcFinfo* >(Test::sharedVec[2])->getBindIndex() );
	assert( pairs[1].second == dynamic_cast< DestFinfo* >(Test::sharedVec[3])->getFid() );

	assert( pairs[2].first == dynamic_cast< SrcFinfo* >(Test::sharedVec[4])->getBindIndex() );
	assert( pairs[2].second == dynamic_cast< DestFinfo* >(Test::sharedVec[5])->getFid() );

	e2->getFieldsOfOutgoingMsg( m->mid(), pairs );
	assert( pairs.size() == 3 );

	//////////////////////////////////////////////////////////////
	// Test Cinfo::srcFinfoName
	//////////////////////////////////////////////////////////////
	assert( Test::initCinfo()->srcFinfoName( pairs[0].first ) == "s0" );
	assert( Test::initCinfo()->srcFinfoName( pairs[1].first ) == "s1" );
	assert( Test::initCinfo()->srcFinfoName( pairs[2].first ) == "s2" );

	//////////////////////////////////////////////////////////////
	// Test Cinfo::destFinfoName
	//////////////////////////////////////////////////////////////
	assert( Test::initCinfo()->destFinfoName( pairs[0].second ) == "d0" );
	assert( Test::initCinfo()->destFinfoName( pairs[1].second ) == "d1" );
	assert( Test::initCinfo()->destFinfoName( pairs[2].second ) == "d2" );
	//////////////////////////////////////////////////////////////
	// Test Msg::getSrcFieldsOnE1 and family
	//////////////////////////////////////////////////////////////
	vector< string > fieldNames;
	fieldNames = m->getSrcFieldsOnE1();
	assert( fieldNames.size() == 3 );
	assert( fieldNames[0] == "s0" );
	assert( fieldNames[1] == "s1" );
	assert( fieldNames[2] == "s2" );

	fieldNames = m->getDestFieldsOnE2();
	assert( fieldNames.size() == 3 );
	assert( fieldNames[0] == "d0" );
	assert( fieldNames[1] == "d1" );
	assert( fieldNames[2] == "d2" );

	fieldNames = m->getSrcFieldsOnE2();
	assert( fieldNames.size() == 3 );
	assert( fieldNames[0] == "s0" );
	assert( fieldNames[1] == "s1" );
	assert( fieldNames[2] == "s2" );

	fieldNames = m->getDestFieldsOnE1();
	assert( fieldNames.size() == 3 );
	assert( fieldNames[0] == "d0" );
	assert( fieldNames[1] == "d1" );
	assert( fieldNames[2] == "d2" );

	//////////////////////////////////////////////////////////////
	// getMsgTargetAndFunctions
	//////////////////////////////////////////////////////////////
	vector< ObjId > tgt;
	vector< string > func;
	unsigned int numTgt = e1->getMsgTargetAndFunctions( 0,
					dynamic_cast< SrcFinfo* >(Test::sharedVec[0] ),
					tgt, func );
	assert( numTgt == tgt.size() );
	assert( tgt.size() == 1 );
	assert( tgt[0] == ObjId( t2, 0 ) );
	assert( func[0] == "d0" );

	// Note that the srcFinfo #2 is in sharedVec[4]
	numTgt = e2->getMsgTargetAndFunctions( 0,
					dynamic_cast< SrcFinfo* >(Test::sharedVec[4] ),
					tgt, func );
	assert( numTgt == tgt.size() );
	assert( tgt.size() == 1 );
	assert( tgt[0] == ObjId( t1, 0 ) );
	assert( func[0] == "d2" );

	//////////////////////////////////////////////////////////////
	// getMsgSourceAndSender
	//////////////////////////////////////////////////////////////
	vector< ObjId > source;
	vector< string > sender;
	FuncId fid =
		static_cast< const DestFinfo* >( Test::sharedVec[5] )->getFid();
	unsigned int numSrc = t2.element()->getMsgSourceAndSender(
					fid, source, sender );
	assert( numSrc == 1 );
	assert( source.size() == 1 );
	assert( source[0] == tgt[0] );
	assert( sender[0] == Test::sharedVec[4]->name() );
	cout << "." << flush;

	//////////////////////////////////////////////////////////////
	// Clean up.
	//////////////////////////////////////////////////////////////
	t1.destroy();
	t2.destroy();
	cout << "." << flush;
}

void testHopFunc()
{
	extern const double* checkHopFuncTestBuffer();

	HopIndex hop2( 1234, MooseTestHop );
	HopFunc2< string, double > two( hop2 );

	two.op( Id(3).eref(), "two", 2468.0 );
	const double* buf = checkHopFuncTestBuffer();
	const TgtInfo* tgt = reinterpret_cast< const TgtInfo* >( buf );
	assert( tgt->bindIndex() == 1234 );
	assert( tgt->dataSize() == 2 );
	const char* c = reinterpret_cast< const char* >(
					buf + TgtInfo::headerSize );
	assert( strcmp( c, "two" ) == 0 );
	assert( doubleEq( buf[TgtInfo::headerSize + 1], 2468.0 ) );

	HopIndex hop3( 36912, MooseTestHop );
	HopFunc3< string, int, vector< double > > three( hop3 );
	vector< double > temp( 3 );
	temp[0] = 11222;
	temp[1] = 24332;
	temp[2] = 234232342;
	three.op( Id(3).eref(), "three", 3333, temp );

	assert( tgt->bindIndex() == 36912 );
	assert( tgt->dataSize() == 6 );
	c = reinterpret_cast< const char* >( buf + TgtInfo::headerSize );
	assert( strcmp( c, "three" ) == 0 );
	int i = TgtInfo::headerSize + 1;
	assert( doubleEq( buf[i++], 3333.0 ) );
	assert( doubleEq( buf[i++], 3.0 ) ); // Size of array.
	assert( doubleEq( buf[i++], temp[0] ) );
	assert( doubleEq( buf[i++], temp[1] ) );
	assert( doubleEq( buf[i++], temp[2] ) );

	cout << "." << flush;
}

void testAsync( )
{
	showFields();
#ifdef DO_UNIT_TESTS
	testSendMsg();
	testCreateMsg();
	testSetGet();
	testSetGetDouble();
	testSetGetSynapse();
	testSetGetVec();
	test2ArgSetVec();
	testSetRepeat();
	testStrSet();
	testStrGet();
	testLookupSetGet();
//	testSendSpike();
	testSparseMatrix();
	testSparseMatrix2();
	testSparseMatrixReorder();
	testSparseMatrixFill();
	testSparseMsg();
	testSharedMsg();
	testConvVector();
	testConvVectorOfVectors();
	testMsgField();
	// testSetGetExtField(); Unsure if we're keeping ext fields.
	testIsA();
	testFinfoFields();
	testCinfoFields();
	testCinfoElements();
	testMsgSrcDestFields();
	testHopFunc();
#endif
}
