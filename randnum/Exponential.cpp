/*******************************************************************
 * File:            Exponential.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-01 09:03:51
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

#ifndef _EXPONENTIAL_CPP
#define _EXPONENTIAL_CPP
#include "Exponential.h"
#include "randnum.h"
#include "utility/numutil.h"
#include <iostream>
#include <cmath>

using namespace std;

Exponential::Exponential(double mean):mean_(mean),generator_(&(Exponential::randomMinimization))
{
}

Exponential::Exponential(ExponentialGenerator method, double mean):mean_(mean)
{
    switch(method)
    {
        case LOGARITHMIC:
            generator_ = &(Exponential::logarithmic);
            break;
        default:
            generator_ = &(Exponential::randomMinimization);
            break;
    }
}

double Exponential::getMean() const
{
    return mean_;
}

double Exponential::getVariance() const
{
    return mean_*mean_;
}

double Exponential::getNextSample() const
{
    return generator_(mean_);
}


double Exponential::logarithmic(double mean)
{
    double uniform = mtrand();
    if ( uniform <= 0 )
    {
        uniform = 1.0e-6;
    }

    return - mean*log(uniform);
}

extern unsigned long genrand_int32(void);

/**
   successive entries in the series
   {Qk} = {(ln2/1! + (ln2)^2/2! + (ln2)^3/3! + ... + (ln2)^k/k!)}
   used in the random minimization algorithm.
 */
static const double q[] =
{
    1.0, // dummy for q[0]
    0.69314718055994528622676,
    0.93337368751904603580982,
    0.98887779618386761892879,
    0.99849592529149611142003,
    0.99982928110613900063441,
    0.99998331641007276449074,
    0.99999856914387685868917,
    0.99999989069255590390384,
    0.99999999247341597730099,
    0.99999999952832763217003,
    0.99999999997288147035590  //  > 0.99999999953433871269226 = 1 - 2^(-31)
};

/**
   See Knuth, Vol II Sec 3.4.1 : Algorithm S
 */

double Exponential::randomMinimization(double mean)
{
    double result;

    unsigned long uniform = genrand_int32(); // 1) generate t+1 (=32) bit uniform random binary fraction .b0..bt
    int j = 0;

    if ( uniform == 0 )
    {
        uniform = 1;
    }

    while (0x80000000 & uniform ) // 1) detect the first 0 bit
    {
        uniform = uniform << 1; // 1a) shift off leading j+1 bits, setting u = .b(j+1) .. b(t)
        ++j;
    }
    uniform = uniform << 1; // 1a)shift off leading j+1 bits, setting u = .b(j+1) .. b(t)
    double uniform_frac = uniform / 4294967296.0;

    if ( uniform_frac < LN2 ) // 2) u < ln2?
    {
        result = mean*(j*LN2 + uniform_frac); // x <- mean * ( j * ln2 + u )
    }
    else
    {
        // 3) minimize
        unsigned int k = 2;
        unsigned long v = ~0UL;
        unsigned long u;

        while ( ( uniform_frac >= q[k] ))
        {
            k++;
        }
        for ( unsigned int i = 0; i < k; ++i )
        {
            u = genrand_int32();
            if ( u < v )
            {
                v = u;
            }
        }

        result = mean*( j + v/4294967296.0 )*LN2;
    }

    return result;
}
#if 0 // test main
#include <vector>
int main(void)
{
    double mean = .25;
    double sum = 0.0;
    double sd = 0.0;
    vector <unsigned> classes;
    Exponential ex(mean);
    int MAX_SAMPLE = 100000;
    int MAX_CLASSES = 1000;


    for ( int i = 0; i < MAX_CLASSES; ++i )
    {
        classes.push_back(0);
    }

    for ( int i = 0; i < MAX_SAMPLE; ++i )
    {
        double p = ex.getNextSample();//aliasMethod();
        int index = (int)(p*MAX_CLASSES);
//        cout << index << " ] " << p << endl;

        if ( index < MAX_CLASSES){
            classes[index]++;
        }
        else
        {
            classes[MAX_CLASSES-1]++;
        }


        sum += p;
        sd += (p - mean)*(p - mean);
    }
    mean = sum/MAX_SAMPLE;
    sd = sqrt(sd/MAX_SAMPLE);
    cout << "mean = " << mean << " sd = " << sd << endl;
    for ( int i = 0; i < MAX_CLASSES; ++i )
    {
        cout << classes[i] << endl;
    }

    return 0;
}
#endif // test main


#endif
