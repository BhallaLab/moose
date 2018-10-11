/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "../basecode/header.h"
#include "VectorTable.h"

#define INIT_XMIN 0
#define INIT_XMAX 0
#define INIT_XDIV 0

using namespace std;

const Cinfo* VectorTable::initCinfo()
{
	//Field information.
	static ValueFinfo< VectorTable, unsigned int > xdivs("xdivs",
			"Number of divisions.",
			&VectorTable::setDiv,
			&VectorTable::getDiv
			);

	static ValueFinfo< VectorTable, double > xmin("xmin",
			"Minimum value in table.",
			&VectorTable::setMin,
			&VectorTable::getMin
			);

	static ValueFinfo< VectorTable, double > xmax("xmax",
			"Maximum value in table.",
			&VectorTable::setMax,
			&VectorTable::getMax
			);

	static ReadOnlyValueFinfo< VectorTable, double > invdx("invdx",
			"Maximum value in table.",
			&VectorTable::getInvDx
			);

	static ValueFinfo< VectorTable, vector< double > > table("table",
			"The lookup table.",
			&VectorTable::setTable,
			&VectorTable::getTable
			);

	static ReadOnlyLookupValueFinfo< VectorTable, double, double >
			lookupvalue(
			"lookupvalue",
			"Lookup function that performs interpolation to return a value.",
			&VectorTable::lookupByValue
			);

	static ReadOnlyLookupValueFinfo< VectorTable, unsigned int, double >
			lookupindex(
			"lookupindex",
			"Lookup function that returns value by index.",
			&VectorTable::lookupByIndex
			);

	static Finfo* vectorTableFinfos[] =
	{
		&xdivs,
		&xmin,
		&xmax,
		&invdx,
		&table,
		&lookupvalue,
		&lookupindex
	};

	static string doc[] =
	{
		"Name", "VectorTable",
		"Author", "Vishaka Datta S, 2011, NCBS",
		"Description", "This is a minimal 1D equivalent of the Interpol2D class. "
		"Provides simple functions for getting and setting up the table, along "
		"with a lookup function. This class is to be used while supplying lookup "
		"tables to the MarkovChannel class, in cases where the transition rate "
		"varies with either membrane voltage or ligand concentration."
	};

	static Dinfo< VectorTable > dinfo;
	static Cinfo VectorTableCinfo(
		"VectorTable",
		Neutral::initCinfo(),
		vectorTableFinfos,
		sizeof( vectorTableFinfos )/sizeof( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &VectorTableCinfo;
}

static const Cinfo* vectorTableCinfo = VectorTable::initCinfo();

VectorTable::VectorTable() : xDivs_(INIT_XDIV), xMin_(INIT_XMIN), xMax_(INIT_XMAX),
														 invDx_(-1), table_(0)
{ ; }

//Implementation identical to that of HHGate::lookupTable.
double VectorTable::lookupByValue( double x ) const
{
	if ( table_.size() == 1 )
		return table_[0];

	if ( x < xMin_ || doubleEq( x, xMin_ ) )
		return table_[0];
	if ( x > xMax_ || doubleEq( x, xMax_ ) )
		return table_.back();

	unsigned int index = static_cast< unsigned int>( ( x - xMin_ ) * invDx_ );
	double frac = ( x - xMin_ - index / invDx_ ) * invDx_;
	return table_[ index ] * ( 1 - frac ) + table_[ index + 1 ] * frac;
}

double VectorTable::lookupByIndex( unsigned int index ) const
{
    if ( tableIsEmpty() )
        return 0;

    /*  NOTE: Why commented out?
     *  index is unsigned int, can't be less than zero.
    */
#if 0
    //Applying similar wrapping as is done in lookupByValue.
    if ( index < 0 )
        index = 0;
#endif

    if ( index >= table_.size() )
        index = table_.size() - 1;

    return table_[index];
}

vector< double > VectorTable::getTable() const
{
	if ( table_.size() == 0 )
	{
		cerr << "VectorTable::getTable : Warning : Table is empty\n";
	}

	return table_;
}

//Function to set up the lookup table.
//All parameters except xMin_ and xMax_ can be set based on the table that is
//passed in.
void VectorTable::setTable( vector< double > table )
{
	if ( table.size() > 1 && xMin_ == xMax_ )
	{
		cerr << "VectorTable::setTable : Error : xmin and xmax cannot be the same when there are more than "
			"two entries in the table!\n";
		return;
	}

	if ( table.empty() )
	{
		cerr << "VectorTable::setTable : Error : Cannot set with empty table!\n";
		return;
	}

	table_ = table;
	xDivs_ = table.size() - 1;

	//This is in case the lookup table has only one entry, in which case,
	//the transition rate being considered is assumed to be constant.
	if ( table.size() > 1 )
		invDx_ = xDivs_ / ( xMax_ - xMin_ );
	else
		invDx_ = 0;
}

unsigned int VectorTable::getDiv() const
{
	return xDivs_;
}

void VectorTable::setDiv( unsigned int xDivs  )
{
	xDivs_ = xDivs;
}

double VectorTable::getMin() const
{
	return xMin_;
}

void VectorTable::setMin( double xMin )
{
	xMin_ = xMin;
}

double VectorTable::getMax() const
{
	return xMax_;
}

void VectorTable::setMax( double xMax )
{
	xMax_ = xMax;
}

double VectorTable::getInvDx() const
{
	return invDx_;
}

bool VectorTable::tableIsEmpty() const
{
	return table_.empty();
}

istream& operator>>( istream& in, VectorTable& vecTable )
{
	in >> vecTable.xDivs_;
	in >> vecTable.xMin_;
	in >> vecTable.xMax_;
	in >> vecTable.invDx_;

	for ( unsigned int i = 0; i < vecTable.table_.size(); ++i )
		in >> vecTable.table_[i];

	return in;
}

#ifdef DO_UNIT_TESTS
void testVectorTable()
{
	VectorTable unInitedTable;

	vector< double > data;

	double arr[11] = {0.0, 0.23, 0.41, 0.46, 0.42, 0.52, 0.49, 0.43, 0.38, 0.43, 0.44};

	data.reserve( 11 );
	//Filling up user table.
	for ( int i = 0; i < 11; i++ )
		data.push_back( arr[i] );

	unInitedTable.setMin( -0.5 );
	unInitedTable.setMax( 0.5 );
	unInitedTable.setTable( data );

	assert( doubleEq(unInitedTable.getInvDx(), 10 ) );

	assert( doubleEq(unInitedTable.lookupByValue( 0.0 ), 0.52 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.1 ), 0.49 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.15 ), 0.46 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.2 ), 0.43 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.25 ), 0.405 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.3 ), 0.38 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.35 ), 0.405 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.4 ), 0.43 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.45 ), 0.435 ) );
	assert( doubleEq(unInitedTable.lookupByValue( 0.5 ), 0.44 ) );

	cout << "." << flush;
}
#endif
