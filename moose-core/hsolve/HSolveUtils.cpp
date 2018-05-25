/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "HSolveUtils.h"

void HSolveUtils::initialize( Id object )
{
	//~ ProcInfoBase p;
	//~ SetConn c( object(), 0 );
	//~
	//~ if ( object()->className() == "Compartment" )
		//~ moose::Compartment::reinitFunc( &c, &p );
	//~ else if ( object()->className() == "HHChannel" )
		//~ HHChannel::reinitFunc( &c, &p );
	//~ else if ( object()->className() == "CaConc" )
		//~ CaConc::reinitFunc( &c, &p );
}

int HSolveUtils::adjacent( Id compartment, Id exclude, vector< Id >& ret )
{
	int size = ret.size();
	adjacent( compartment, ret );
	ret.erase(
		remove( ret.begin(), ret.end(), exclude ),
		ret.end()
	);
	return ret.size() - size;
}

int HSolveUtils::adjacent( Id compartment, vector< Id >& ret )
{
	int size = 0;
	size += targets( compartment, "axial", ret, "Compartment" );
	size += targets( compartment, "raxial", ret, "Compartment" );
	size += targets( compartment, "distalOut", ret, "SymCompartment" );
	size += targets( compartment, "proximalOut", ret, "SymCompartment" );
	size += targets( compartment, "cylinderOut", ret, "SymCompartment" );
	return size;
}

int HSolveUtils::children( Id compartment, vector< Id >& ret )
{
	int size = targets( compartment, "axial", ret, "Compartment" );
	size += targets( compartment, "distalOut", ret, "SymCompartment" );
	size += targets( compartment, "cylinderOut", ret, "SymCompartment" );
	return size;
}

/**
 * Gives all channels (hhchannels, synchans, any other) attached to a given
 * compartment.
 */
int HSolveUtils::channels( Id compartment, vector< Id >& ret )
{
	return targets( compartment, "channel", ret );
}

int HSolveUtils::hhchannels( Id compartment, vector< Id >& ret )
{
	// Request for elements of type "HHChannel" only since
	// channel messages can lead to synchans as well.
	return targets( compartment, "channel", ret, "HHChannel" );
}

/**
 * The 'getOriginals' flag requests Id:s of the prototype gates from which
 * copies were created, instead of Id:s of the copied gates. Default is true.
 */
int HSolveUtils::gates(
	Id channel,
	vector< Id >& ret,
	bool getOriginals )
{
//        dump("HSolveUtils::gates() is not tested with new hsolve api", "FIXME");
	unsigned int oldSize = ret.size();

	static string gateName[] = {
		string( "gateX[0]" ),
		string( "gateY[0]" ),
		string( "gateZ[0]" )
	};

	static string powerField[] = {
		string( "Xpower" ),
		string( "Ypower" ),
		string( "Zpower" )
	};

        unsigned int nGates = 3; // Number of possible gates
        for ( unsigned int i = 0; i < nGates; i++ ) {
            double power  = Field< double >::get ( channel, powerField[i] );

            if ( power > 0.0 ) {
                // string gatePath = moose::joinPath(channel.path(), gateName[i]);
                string gatePath = moose::fixPath( channel.path() ) +
						"/" +  gateName[i];
                Id gate( gatePath );

                string gPath = moose::fixPath(gate.path());
                errorSS.str("");
                errorSS << "Got " << gatePath << " expected " << gPath;
                SIMPLE_ASSERT_MSG(gPath == gatePath, errorSS.str().c_str());

                if ( getOriginals ) {
                    HHGate* g = reinterpret_cast< HHGate* >( gate.eref().data() );
                    gate = g->originalGateId();
                }

                ret.push_back( gate );
            }
        }

        return ret.size() - oldSize;
}

int HSolveUtils::spikegens( Id compartment, vector< Id >& ret )
{
	return targets( compartment, "VmOut", ret, "SpikeGen" );
}

int HSolveUtils::synchans( Id compartment, vector< Id >& ret )
{
	// "channel" msgs lead to SynChans as well HHChannels, so request
	// explicitly for former.
	return targets( compartment, "channel", ret, "SynChan" );
}

int HSolveUtils::leakageChannels( Id compartment, vector< Id >& ret )
{
	return targets( compartment, "channel", ret, "Leakage" );
}

int HSolveUtils::caTarget( Id channel, vector< Id >& ret )
{
	return targets( channel, "IkOut", ret, "CaConc" );
}

int HSolveUtils::caDepend( Id channel, vector< Id >& ret )
{
	return targets( channel, "concen", ret, "CaConc" );
}

/*
 * Functions for accessing gates' lookup tables.
 */

//~ /**
 //~ * Finds the xmin and xmax for the lookup tables (A and B) belonging to a gate.
 //~ *
 //~ * 'min' will be the smaller of the 2 mins.
 //~ * 'max' will be the greater of the 2 maxs.
 //~ */
//~ int HSolveUtils::domain(
	//~ Id gate,
	//~ double& min,
	//~ double& max )
//~ {
	//~ Id A;
	//~ Id B;
	//~
	//~ bool success;
	//~ success = lookupGet< Id, string >( gate(), "lookupChild", A, "A" );
	//~ if ( ! success ) {
		//~ cerr << "Error: Interpol A not found as child of " << gate()->name();
		//~ return 0;
	//~ }
	//~
	//~ success = lookupGet< Id, string >( gate(), "lookupChild", B, "B" );
	//~ if ( ! success ) {
		//~ cerr << "Error: Interpol B not found as child of " << gate()->name();
		//~ return 0;
	//~ }
	//~
	//~ double Amin, Amax;
	//~ double Bmin, Bmax;
	//~ get< double >( A(), "xmin", Amin );
	//~ get< double >( A(), "xmax", Amax );
	//~ get< double >( B(), "xmin", Bmin );
	//~ get< double >( B(), "xmax", Bmax );
	//~
	//~ min = Amin < Bmin ? Amin : Bmin;
	//~ max = Amax > Bmax ? Amax : Bmax;
	//~
	//~ return 1;
//~ }

unsigned int HSolveUtils::Grid::size()
{
	return divs_ + 1;
}

double HSolveUtils::Grid::entry( unsigned int i )
{
	assert( i <= divs_ + 1 );

	return ( min_ + dx_ * i );
}

void HSolveUtils::rates(
	Id gateId,
	HSolveUtils::Grid grid,
	vector< double >& A,
	vector< double >& B )
{
    // dump("HSolveUtils::rates() has not been tested yet.", "WARN");
    double min = Field< double >::get( gateId, "min" );
    double max = Field< double >::get( gateId, "max" );
    unsigned int divs = Field< unsigned int >::get( gateId, "divs" );

    if ( grid == Grid( min, max, divs ) ) {
        A = Field< vector< double > >::get( gateId, "tableA" );
        B = Field< vector< double > >::get( gateId, "tableB" );
        return;
    }

    A.resize( grid.size() );
    B.resize( grid.size() );

    /*
     * Getting Id of original (prototype) gate, so that we can set fields on
     * it. Copied gates are read-only.
     */
    HHGate* gate = reinterpret_cast< HHGate* >( gateId.eref().data() );
    gateId = gate->originalGateId();

    /*
     * Setting interpolation flag on. Will set back to its original value once
     * we're done.
     */
    bool useInterpolation = Field< bool >::get( gateId, "useInterpolation");
    gate->setUseInterpolation( gateId.eref(), true );

    unsigned int igrid;
    double* ia = &A[ 0 ];
    double* ib = &B[ 0 ];
    for ( igrid = 0; igrid < grid.size(); ++igrid ) {
        gate->lookupBoth( grid.entry( igrid ), ia, ib );

        ++ia, ++ib;
    }

    // Setting interpolation flag back to its original value.
    //~ HSolveUtils::set< HHGate, bool >
    //~ ( gateId, "useInterpolation", useInterpolation );
    gate->setUseInterpolation( gateId.eref(), useInterpolation );
}

//~ int HSolveUtils::modes( Id gate, int& AMode, int& BMode )
//~ {
	//~ Id A;
	//~ Id B;
	//~
	//~ bool success;
	//~ success = lookupGet< Id, string >( gate(), "lookupChild", A, "A" );
	//~ if ( ! success ) {
		//~ cerr << "Error: Interpol A not found as child of " << gate()->name();
		//~ return 0;
	//~ }
	//~
	//~ success = lookupGet< Id, string >( gate(), "lookupChild", B, "B" );
	//~ if ( ! success ) {
		//~ cerr << "Error: Interpol B not found as child of " << gate()->name();
		//~ return 0;
	//~ }
	//~
	//~ get< int >( A(), "mode", AMode );
	//~ get< int >( B(), "mode", BMode );
	//~ return 1;
//~ }

///////////////////////////////////////////////////
// Utility functions
///////////////////////////////////////////////////

int HSolveUtils::targets(
	Id object,
	string msg,
	vector< Id >& target,
	string filter,   // Default: ""
	bool include )   // Default: true
{
	vector< string > filter_v;

	if ( filter != "" )
		filter_v.push_back( filter );

	return targets( object, msg, target, filter_v, include );
}

/**
 * Appends to 'target' any destination objects of messages of the
 * 	specified name found on the object.
 *	The filter restricts the returns to those objects of the specified class
 *	include is a flag, when false it flips the returns to objects _not_ of
 *	the specified class.
 */
int HSolveUtils::targets(
	Id object,
	string msg,
	vector< Id >& target,
	const vector< string >& filter,    // This does not have a default value,
	                                   // to avoid ambiguity between the two
	                                   // 'targets()' functions when the last 2
	                                   // arguments are skipped.
	bool include )                     // Default: true
{
	unsigned int oldSize = target.size();

	vector< Id > all;
	Element* e = object.element();
	const Finfo* f = e->cinfo()->findFinfo( msg );
	if ( !f ) // Might not find SymCompartment Finfos if it is a Compartment
		return 0;
	e->getNeighbors( all, f );

	vector< Id >::iterator ia;
	if ( filter.empty() ) {
		target.insert( target.end(), all.begin(), all.end() );
	} else {
		for ( ia = all.begin(); ia != all.end(); ++ia ) {
			string className = (*ia).element()->cinfo()->name();
			bool hit =
				find(
					filter.begin(),
					filter.end(),
					className
				) != filter.end();

			if ( ( hit && include ) || ( !hit && !include ) )
				target.push_back( *ia );
		}
	}

	return target.size() - oldSize;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS

#include "HinesMatrix.h"
#include "../shell/Shell.h"
void testHSolveUtils( )
{
        //TEST_BEGIN;
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	bool success;

	Id n = shell->doCreate( "Neutral", Id(), "n", 1 );

	/**
	 *  First we test the functions which return the compartments linked to a
	 *  given compartment: adjacent(), and children().
	 *
	 *  A small tree is created for this:
	 *
	 *               c0
	 *                L c1
	 *                   L c2
	 *                   L c3
	 *                   L c4
	 *                   L c5
	 *
	 *  (c0 is the parent of c1. c1 is the parent of c2, c3, c4, c5.)
	 */
	Id c[ 6 ];
	c[ 0 ] = shell->doCreate( "Compartment", n, "c0", 1 );
	c[ 1 ] = shell->doCreate( "Compartment", n, "c1", 1 );
	c[ 2 ] = shell->doCreate( "Compartment", n, "c2", 1 );
	c[ 3 ] = shell->doCreate( "Compartment", n, "c3", 1 );
	c[ 4 ] = shell->doCreate( "Compartment", n, "c4", 1 );
	c[ 5 ] = shell->doCreate( "Compartment", n, "c5", 1 );

	ObjId mid;
	mid = shell->doAddMsg( "Single", c[ 0 ], "axial", c[ 1 ], "raxial" );
	ASSERT( ! mid.bad(), "Linking compartments" );
	mid = shell->doAddMsg( "Single", c[ 1 ], "axial", c[ 2 ], "raxial" );
	ASSERT( ! mid.bad(), "Linking compartments" );
	mid = shell->doAddMsg( "Single", c[ 1 ], "axial", c[ 3 ], "raxial" );
	ASSERT( ! mid.bad(), "Linking compartments" );
	mid = shell->doAddMsg( "Single", c[ 1 ], "axial", c[ 4 ], "raxial" );
	ASSERT( ! mid.bad(), "Linking compartments" );
	mid = shell->doAddMsg( "Single", c[ 1 ], "axial", c[ 5 ], "raxial" );
	ASSERT( ! mid.bad(), "Linking compartments" );

	vector< Id > found;
	unsigned int nFound;

	/*
	 * Testing version 1 of HSolveUtils::adjacent.
	 * It finds all neighbors of given compartment.
	 */
	// Neighbors of c0
	nFound = HSolveUtils::adjacent( c[ 0 ], found );
	ASSERT( nFound == found.size(), "Finding adjacent compartments" );
	// c1 is adjacent
	ASSERT( nFound == 1, "Finding adjacent compartments" );
	ASSERT( found[ 0 ] == c[ 1 ], "Finding adjacent compartments" );

	// Neighbors of c1
	found.clear();
	nFound = HSolveUtils::adjacent( c[ 1 ], found );
	ASSERT( nFound == 5, "Finding adjacent compartments" );
	// c0 is adjacent
	success =
		find( found.begin(), found.end(), c[ 0 ] ) != found.end();
	ASSERT( success, "Finding adjacent compartments" );
	// c2 - c5 are adjacent
	for ( int i = 2; i < 6; i++ ) {
		success =
			find( found.begin(), found.end(), c[ i ] ) != found.end();
		ASSERT( success, "Finding adjacent compartments" );
	}

	// Neighbors of c2
	found.clear();
	nFound = HSolveUtils::adjacent( c[ 2 ], found );
	// c1 is adjacent
	ASSERT( nFound == 1, "Finding adjacent compartments" );
	ASSERT( found[ 0 ] == c[ 1 ], "Finding adjacent compartments" );

	/*
	 * Testing version 2 of HSolveUtils::adjacent.
	 * It finds all but one neighbors of given compartment.
	 * The the second argument to 'adjacent' is the one that is excluded.
	 */
	// Neighbors of c1 (excluding c0)
	found.clear();
	nFound = HSolveUtils::adjacent( c[ 1 ], c[ 0 ], found );
	ASSERT( nFound == 4, "Finding adjacent compartments" );
	// c2 - c5 are adjacent
	for ( int i = 2; i < 6; i++ ) {
		success =
			find( found.begin(), found.end(), c[ i ] ) != found.end();
		ASSERT( success, "Finding adjacent compartments" );
	}

	// Neighbors of c1 (excluding c2)
	found.clear();
	nFound = HSolveUtils::adjacent( c[ 1 ], c[ 2 ], found );
	ASSERT( nFound == 4, "Finding adjacent compartments" );
	// c0 is adjacent
	success =
		find( found.begin(), found.end(), c[ 0 ] ) != found.end();
	ASSERT( success, "Finding adjacent compartments" );
	// c3 - c5 are adjacent
	for ( int i = 3; i < 6; i++ ) {
		success =
			find( found.begin(), found.end(), c[ i ] ) != found.end();
		ASSERT( success, "Finding adjacent compartments" );
	}

	// Neighbors of c2 (excluding c1)
	found.clear();
	nFound = HSolveUtils::adjacent( c[ 2 ], c[ 1 ], found );
	// None adjacent, if c1 is excluded
	ASSERT( nFound == 0, "Finding adjacent compartments" );

	// Neighbors of c2 (excluding c3)
	found.clear();
	nFound = HSolveUtils::adjacent( c[ 2 ], c[ 3 ], found );
	// c1 is adjacent, while c3 is not even connected
	ASSERT( nFound == 1, "Finding adjacent compartments" );
	ASSERT( found[ 0 ] == c[ 1 ], "Finding adjacent compartments" );

	/*
	 * Testing HSolveUtils::children.
	 * It finds all compartments which are dests for the "axial" message.
	 */
	// Children of c0
	found.clear();
	nFound = HSolveUtils::children( c[ 0 ], found );
	ASSERT( nFound == 1, "Finding child compartments" );
	// c1 is a child
	ASSERT( found[ 0 ] == c[ 1 ], "Finding child compartments" );

	// Children of c1
	found.clear();
	nFound = HSolveUtils::children( c[ 1 ], found );
	ASSERT( nFound == 4, "Finding child compartments" );
	// c2 - c5 are c1's children
	for ( int i = 2; i < 6; i++ ) {
		success =
			find( found.begin(), found.end(), c[ i ] ) != found.end();
		ASSERT( success, "Finding child compartments" );
	}

	// Children of c2
	found.clear();
	nFound = HSolveUtils::children( c[ 2 ], found );
	// c2 has no children
	ASSERT( nFound == 0, "Finding child compartments" );

	// Clean up
	shell->doDelete( n );
        cout << "." << flush;
        //  TEST_END;
}

#endif // DO_UNIT_TESTS
