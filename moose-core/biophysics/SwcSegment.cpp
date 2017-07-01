/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2015 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <math.h>
using namespace std;
#include "../utility/Vec.h"
#include "SwcSegment.h"

const short SwcSegment::UNDEF = 0;
const short SwcSegment::SOMA = 1;
const short SwcSegment::AXON = 2;
const short SwcSegment::DEND = 3;
const short SwcSegment::APICAL = 4;
const short SwcSegment::FORK = 5; // Assumed to be on regular dend
const short SwcSegment::END = 6; // Assumed to be on regular dend
const short SwcSegment::CUSTOM = 7;
		// Here are a few more
const short SwcSegment::BadSegment = 8;
const short SwcSegment::AXON_FORK = 10;
const short SwcSegment::AXON_END = 11;
const short SwcSegment::APICAL_FORK = 12;
const short SwcSegment::APICAL_END = 13;

const string SwcSegment::typeName[] = {
	"undef", "soma", "axon", "dend", "apical", "dend_f", "dend_e",
	"custom", "bad", "undef",
	"axon_f", "axon_e", "apical_f", "apical_e" };

SwcSegment::SwcSegment( const string& line )
		:
			geometricalDistanceFromSoma_( 0.0 ),
			electrotonicDistanceFromSoma_( 0.0 )
{
	vector< string > args;
	stringstream ss( line );
	string temp;
	while (ss >> temp ) {
		args.push_back( temp );
	}
	if ( args.size() == 7 ) {
		myIndex_ = atoi( args[0].c_str() );
		type_ = atoi( args[1].c_str() );
		double x = atof( args[2].c_str() );
		double y = atof( args[3].c_str() );
		double z = atof( args[4].c_str() );
		v_ = Vec( x, y, z );
		radius_ = atof( args[5].c_str() );
		int pa = atoi( args[6].c_str() );
		if ( pa > 0 )
			parent_ = pa;
		else
			parent_ = ~0U;
	} else {
		type_ = BadSegment;
	}
}

SwcSegment::SwcSegment( int i,  short type,
				double x, double y, double z,
				double r, int parent )
				:
						myIndex_( i ),
						type_( type ),
						v_( x, y, z ),
						radius_( r ),
						length_( 0.0 ),
						L_( 0.0 ),
						geometricalDistanceFromSoma_( 0.0 ),
						electrotonicDistanceFromSoma_( 0.0 )
{
	if ( parent >= 0 )
		parent_ = parent;
	else
		parent_ = ~0U;
}

void SwcSegment::figureOutType()
{
	if ( type_ == SOMA ) // already defined as soma
		return;
	if ( type_ == DEND ) {
		if ( kids_.size() > 1 )
			type_ = FORK; // Dend fork point
		else if ( kids_.size() == 0 )
			type_ = END; // end point
	} else if ( type_ == APICAL ) {
		if ( kids_.size() > 1 )
			type_ = APICAL_FORK; // apical Dend fork point
		else if ( kids_.size() == 0 )
			type_ = APICAL_END; // apical end point
	} else if ( type_ == AXON ) {
		if ( kids_.size() > 1 )
			type_ = AXON_FORK; // apical Dend fork point
		else if ( kids_.size() == 0 )
			type_ = AXON_END; // apical end point
	}
}

//////////////////////////////////////////////////////////////////////

SwcBranch::SwcBranch( int i,  const SwcSegment& start, double len, double L,
				const vector< int >& cable )
				: SwcSegment( start ),
				r0( start.radius() ),
				r1( start.radius() ),
				geomLength( len ),
				electroLength( L )
{
	myIndex_ = i;
	parent_ = 0;
	kids_.resize( 0 );
	segs_.resize( cable.size() );
	// Put the contents of cable into segs, in reverse order.
	vector< int >::const_reverse_iterator j = cable.rbegin();
	vector< int >::iterator k = segs_.begin();
	for ( k = segs_.begin(); k != segs_.end(); ++k )
		*k = *j++;
}

void SwcBranch::printDiagnostics() const
{
	cout << myIndex() << ":  " << segs_[0] << " -> " << segs_.back() <<
			" = " << segs_.size() <<
			" :	pa = " << parent() << " ,	length=( " <<
			geomLength << ", " << electroLength << " )\n";
}
