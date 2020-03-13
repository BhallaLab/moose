/*******************************************************************
 * File:            numutil.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray dot subhasis at gmail dot com
 * Created:         2007-11-02 11:46:40
 ********************************************************************/

#include "numutil.h"
#include <cmath>
#include "../randnum/RNG.h"

bool almostEqual(float x, float y, float epsilon)
{
    if (x == 0.0 && y == 0.0) {
        return true;
    }

    if (fabs(x) > fabs(y)) {
        return fabs((x - y) / x) < epsilon;
    } else {
        return fabs((x - y) / y) < epsilon;
    }
}

bool almostEqual(double x, double y, double epsilon)
{
    if (x == 0.0 && y == 0.0){
        return true;
    }
    if (fabs(x) > fabs(y)){
        return fabs((x - y) / x) < epsilon;
    } else {
        return fabs((x - y) / y) < epsilon;
    }
}

bool almostEqual(long double x, long double y, long double epsilon)
{
    if (x == 0.0 && y == 0.0){
        return true;
    }
    if (std::fabs(x) > std::fabs(y)){
        return std::fabs((x - y) / x) < epsilon;
    } else {
        return std::fabs((x - y) / y) < epsilon;
    }
}

double approximateWithInteger_debug(const char* name, const double x, moose::RNG& rng)
{
    static size_t n = 0;
    n += 1;
    cerr << name << ' ' << ':' << x;
    auto y = approximateWithInteger(x, rng);
    cout << ' ' << y << ", ";
    if( std::fmod(n, 4) == 0)
        cerr << endl;
    return y;
}

double approximateWithInteger(const double x, moose::RNG& rng)
{
#if 0
    return std::round(x);
#else
    assert(x >= 0.0);
    double xf = std::floor(x);
    double base = x - xf;
    if( base == 0.0)
        return x;
    if( rng.uniform() < base)
        return xf+1.0;
    return xf;
#endif
}

double approximateWithInteger(const double x)
{
    return approximateWithInteger(x, moose::rng);
}

