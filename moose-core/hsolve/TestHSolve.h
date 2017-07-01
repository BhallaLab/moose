/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _TEST_HSOLVE_H
#define _TEST_HSOLVE_H

void makeFullMatrix(
	const vector< TreeNodeStruct >& tree,
	double dt,
	vector< vector< double > >& matrix );

template< class T >
void permute(
	vector< T >& g,
	const vector< unsigned int >& permutation )
{
	assert( g.size() == permutation.size() );

	vector< T > copy( g.size() );

	for ( unsigned int i = 0; i < g.size(); i++ )
		copy[ permutation[ i ] ] = g[ i ];

	for ( unsigned int i = 0; i < g.size(); i++ )
		g[ i ] = copy[ i ];
}

#endif // _TEST_HSOLVE_H
