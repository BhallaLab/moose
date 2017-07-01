/*******************************************************************
 * File:            numutil.h
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray dot subhasis at gmail dot com
 * Created:         2007-11-02 11:47:21
 ********************************************************************/
/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Messaging Object Oriented Simulation Environment,
 ** also known as GENESIS 3 base code.
 **           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
 ** It is made available under the terms of the
 ** GNU General Public License version 2
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _NUMUTIL_H
#define _NUMUTIL_H

#include <cmath>
#include <cfloat>
#include <limits>

const int WORD_LENGTH = 32; // number of bits in a word - check for portability
const double LN2 = 0.69314718055994528622676;
const unsigned long LN2BYTES = 0xB1721814;
const double NATURAL_E = 2.718281828459045;

//extern const double getMachineEpsilon();
//extern const double EPSILON;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E  2.7182818284590452353
#endif


/**
 * Functions for floating point comparisons
 */
    template<class T>
bool isNaN( T value )
{
    return value != value;
}

    template< typename T >
bool isInfinity( T value )
{
    return value == std::numeric_limits< T >::infinity();
}

/**
 * Check 2 floating-point numbers for "equality".
 * Algorithm (from Knuth) 'a' and 'b' are close if:
 *      | ( a - b ) / a | < e AND | ( a - b ) / b | < e
 * where 'e' is a small number.
 *
 * In this function, 'e' is computed as:
 * 	    e = tolerance * machine-epsilon
 */
    template< class T >
bool isClose( T a, T b, T tolerance )
{
    T epsilon = std::numeric_limits< T >::epsilon();

    if ( a == b )
        return true;

    if ( a == 0 || b == 0 )
        return ( fabs( a - b ) < tolerance * epsilon );

    return (
            fabs( ( a - b ) / a ) < tolerance * epsilon
            &&
            fabs( ( a - b ) / b ) < tolerance * epsilon
           );
}

bool almostEqual(float x, float y, float epsilon = FLT_EPSILON);
bool almostEqual(double x, double y, double epsilon = DBL_EPSILON);
bool almostEqual(long double x, long double y, long double epsilon = LDBL_EPSILON);

// round, isinf and isnan are not defined in VC++ or Borland C++
#if defined(__TURBOC__) || defined(__BORLANDC__) || defined(_MSC_VER)
#define isinf(param) !_finite(param)
#define isnan(param) _isnan(param)
#define round(param) floor(param+0.5)
#endif
#endif
