/*******************************************************************
 * File:            numutil.h
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray dot subhasis at gmail dot com
 * Created:         2007-11-02 11:47:21
 ********************************************************************/

#ifndef _NUMUTIL_H
#define _NUMUTIL_H

#include <cmath>
#include <cfloat>
#include <limits>
#include "../randnum/RNG.h"
#include "../basecode/global.h"


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
double approximateWithInteger(const double x, moose::RNG& rng);
double approximateWithInteger(const double x);
double approximateWithInteger_debug(const char* name, const double x, moose::RNG& rng);

#endif
