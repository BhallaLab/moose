/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <math.h>
#include <vector>
#include <algorithm>
#include <cassert>
#include "../basecode/header.h"
#include "RateTerm.h"
using namespace std;

const double RateTerm::EPSILON = 1.0e-6;

StochNOrder::StochNOrder( double k, vector< unsigned int > v )
    : NOrder( k, v )
{
    // Here we sort the y vector so that if there are repeated
    // substrates, they are put consecutively. This lets us use
    // the algorithm below to deal with repeats.
    sort( v_.begin(), v_.end() );
}

double StochNOrder::operator() ( const double* S ) const
{
    double ret = k_;
    vector< unsigned int >::const_iterator i;
    unsigned int lasty = 0;
    double y = 0.0;
    for ( i = v_.begin(); i != v_.end(); i++)
    {
        assert( !std::isnan( S[ *i ] ) );
        if ( lasty == *i )
            y -= 1.0;
        else
            y = S[ *i ];
        ret *= y;
        lasty = *i;
    }
    return ret;
}
