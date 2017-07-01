/*******************************************************************
 * File:            Uniform.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2008-02-21 17:12:55
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

#ifndef _UNIFORM_CPP
#define _UNIFORM_CPP

#include <cassert>
#include <iostream>
#include <vector>

#include "Uniform.h"
#include "randnum.h"
#include "utility/numutil.h"
using namespace std;

Uniform::Uniform()
{
    min_ = 0.0;
    max_ = 1.0;
}
Uniform::Uniform(double min, double max)
{
    if ( min >= max )
    {
        cerr << "ERROR: specified lowerbound is greater than upper bound." << endl;
        min_ = 0.0;
        max_ = 1.0;
        return;
    }

    min_ = min;
    max_ = max;
}
double Uniform::getMean() const
{
    return (max_ + min_) / 2.0;
}
double Uniform::getVariance()const
{
    return (max_- min_) * (max_ - min_)/12.0;
}
double Uniform::getMin() const
{
    return min_;
}
double Uniform::getMax() const
{
    return max_;
}
void Uniform::setMin(double min)
{
    min_ = min;
}
void Uniform::setMax(double max)
{
    max_ = max;
}
double Uniform::getNextSample() const
{
    assert( max_ > min_ );
    return mtrand()*(max_-min_)+min_;
}

#ifdef DO_UNIT_TESTS
void doTest(double min, double max, unsigned count)
{
    Uniform rng;

    rng.setMin(min);
    rng.setMax(max);
    assert(isClose<double>(min, rng.getMin(), DBL_EPSILON));
    assert(isClose<double>(max, rng.getMax(), DBL_EPSILON));
    vector <double> seq;
    double mean = 0.0;


    for (unsigned ii = 0; ii < count; ++ii )
    {
        double sample;
        sample = rng.getNextSample();
        mean += sample;
        seq.push_back(sample);
    }
    mean /= count;
    double var = 0.0;
    for(unsigned ii = 0; ii <seq.size(); ++ii)
    {
        var += (mean - seq[ii]) * (mean - seq[ii]);
    }
    var = var / count;
    cout << "theoretical mean: " << rng.getMean() << ", sample mean: " << mean << ", theoretical var: " << rng.getVariance() << ", sample var: " << var << ", sample size: " << count << endl;

}

void testUniform()
{
    cout << "testUniform(): testing uniform rng.\n";
    doTest(-10.0, 10.0, 1000);
    doTest(1e-10, 1.1e-10, 1000);
}

#endif // DO_UNIT_TESTS
#if defined( TEST_MAIN ) && defined(DO_UNIT_TESTS)
int main(void)
{
    testUniform();
}

#endif // !defined( TEST_MAIN ) && defined(DO_UNIT_TESTS)

#endif
