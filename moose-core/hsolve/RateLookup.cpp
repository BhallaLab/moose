/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <vector>
using namespace std;

#include "RateLookup.h"

LookupTable::LookupTable(
	double min, double max, unsigned int nDivs, unsigned int nSpecies )
{
	min_ = min;
	max_ = max;
	// Number of points is 1 more than number of divisions.
	// Then add one more since we may interpolate at the last point in the table.
	nPts_ = nDivs + 1 + 1;
	dx_ = ( max - min ) / nDivs;
	// Every row has 2 entries for each type of gate
	nColumns_ = 2 * nSpecies;

	//~ interpolate_.resize( nSpecies );
	table_.resize( nPts_ * nColumns_ );
}

void LookupTable::addColumns(
	int species,
	const vector< double >& C1,
	const vector< double >& C2 )
	//~ const vector< double >& C2,
	//~ bool interpolate )
{
	vector< double >::const_iterator ic1 = C1.begin();
	vector< double >::const_iterator ic2 = C2.begin();
	vector< double >::iterator iTable = table_.begin() + 2 * species;
	// Loop until last but one point
	for ( unsigned int igrid = 0; igrid < nPts_ - 1 ; ++igrid ) {
		*( iTable )     = *ic1;
		*( iTable + 1 ) = *ic2;

		iTable += nColumns_;
		++ic1, ++ic2;
	}
	// Then duplicate the last point
	*( iTable )     = C1.back();
	*( iTable + 1 ) = C2.back();

	//~ interpolate_[ species ] = interpolate;
}

void LookupTable::column( unsigned int species, LookupColumn& column )
{
	column.column = 2 * species;
	//~ column.interpolate = interpolate_[ species ];
}

void LookupTable::row( double x, LookupRow& row )
{
	if ( x < min_ )
		x = min_;
	else if ( x > max_ )
		x = max_;

	double div = ( x - min_ ) / dx_;
	unsigned int integer = ( unsigned int )( div );

	row.fraction = div - integer;
	row.row = &( table_.front() ) + integer * nColumns_;
}

void LookupTable::lookup(
	const LookupColumn& column,
	const LookupRow& row,
	double& C1,
	double& C2 )
{
	double a, b;
	double *ap, *bp;

	ap = row.row + column.column;

	//~ if ( ! column.interpolate ) {
		//~ C1 = *ap;
		//~ C2 = *( ap + 1 );
		//~
		//~ return;
	//~ }

	bp = ap + nColumns_;

	a = *ap;
	b = *bp;
	C1 = a + ( b - a ) * row.fraction;

	a = *( ap + 1 );
	b = *( bp + 1 );
	C2 = a + ( b - a ) * row.fraction;
}
