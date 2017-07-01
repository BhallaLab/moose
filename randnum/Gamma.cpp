/*******************************************************************
 * File:            Gamma.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-05 18:33:01
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

#ifndef _GAMMA_CPP
#define _GAMMA_CPP
#include <cmath>
#include "utility/numutil.h"
#include "Gamma.h"
#include "Exponential.h"
#include "randnum.h"
Gamma::Gamma(double alpha, double theta):alpha_(alpha), theta_(theta)
{
    if (( alpha < 0 ) || (theta < 0 ))
    {
        cerr << "ERROR: setting parameter of Gamma distribution to negative. Setting both to 1." << endl;
        alpha_ = 1;
        theta_ = 1;
    }
}
double Gamma::getAlpha()
{
    return alpha_;
}

double Gamma::getTheta()
{
    return theta_;
}

double Gamma::getMean() const
{
    return alpha_*theta_;
}

double Gamma::getVariance() const
{
    return alpha_*theta_*theta_;
}

double Gamma::getNextSample() const
{
    double result;

    if ( alpha_ <= 1 )
    {
        result = gammaSmall();
    }
    else
    {
        result = gammaLarge();
    }
    if ( !isClose< double >(theta_, 1.0, DBL_EPSILON))
    {
        result *= theta_;
    }

    return result;
}

// See Algorithm A in TAOCP by Knuth, Vol 2 ,Section 3.4.1
double Gamma::gammaLarge() const// alpha > 1
{
    double result = 0.0;
    // a1. generate candidate
    double yValue;
    double uniformU;
    double uniformV;
    double check;
    double tmp;

    while (true)
    {
        uniformU = mtrand();
        yValue = tan(M_PI*uniformU);
        tmp = sqrt(2*alpha_ - 1)*yValue;

        result = tmp + alpha_ - 1;
        if (result > 0)
        {
            uniformV = mtrand();
            check = ( 1 + yValue*yValue )*exp((alpha_ - 1.0)*log(result/(alpha_ - 1.0)) - tmp);
            if (uniformV < check )
            {
                return result;
            }
        }
    }
    return result;    // silence the compiler
}


// See: TAOCP by Knuth, Vol 2, 3.4.1 Exercise 16
double Gamma::gammaSmall() const // 0 < alpha < 1
{
    static Exponential expGen(1.0);

    // G1. initialize
    static double p = NATURAL_E/(alpha_+NATURAL_E);
    static double pByE = 1.0/(alpha_+NATURAL_E);
    double uniformU;
    double expSample;
    double xValue = 0.0;
    double qValue;

    while ( true )
    {
        // G2. generate G deviate
        uniformU = mtrand();
        expSample = expGen.getNextSample();
        while (expSample == 0 )
        {
            expSample = expGen.getNextSample();
        }

        if ( uniformU < p )
        {
            xValue = exp(-expSample/alpha_);
            if ( uniformU < pByE )
            {
                return xValue;
            }
            qValue = p*exp(-xValue);
        }
        else
        {
            xValue = 1 + expSample;
            qValue = p + (1.0-p)*pow(xValue, alpha_ - 1.0);
        }

        // G3. reject?
        if ( uniformU < qValue )
        {
            return xValue;
        }
    }
    return xValue; // silence the compiler
}


#endif
