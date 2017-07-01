/*******************************************************************
 * File:            Binomial.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-10-28 13:44:46
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

#ifndef _BINOMIAL_CPP
#define _BINOMIAL_CPP
#include <cmath>
#include "randnum.h"
#include "utility/numutil.h"
#include "Binomial.h"
#include <vector>
#include <iostream>
#include <climits>
using namespace std;

// First 10 entries in lookup table
const double fc[] = {
    0.08106146679532726,
    0.04134069595540929,
    0.02767792568499834,
    0.02079067210376509,
    0.01664469118982119,
    0.01387612882307075,
    0.01189670994589177,
    0.01041126526197209,
    0.009255462182712733,
    0.008330563433362871
};


/// Insert first 100 elements in lookup table
const vector <double> initializeLookupTable()
{
    static vector <double> table;

    for ( int i = 0; i < 10; ++i )
    {
        table.push_back(fc[i]);
    }
    for ( int i = 10; i < 100; ++i )
    {
        double denom = 1.0/(i+1);
        double value = (0.083333333333333333 -  (0.002777777777777778 -  0.0007936508*denom*denom)*denom*denom)*denom;
        table.push_back(value);
    }
    return table;
}
static const vector <double> lookupTable = initializeLookupTable();



/**
   retrieve fc value from table or calculate it depending on the value of k
*/
inline double getFc(unsigned int k)
{
    if (lookupTable.size() <= k )
    {
        double denom = 1.0/(k+1);
        return (0.083333333333333333 -  (0.002777777777777778 -  0.0007936508*denom*denom)*denom*denom)*denom;
    }
    else
    {
        return lookupTable[k];
    }
}

/**
   Binomial distribution generator with parameters n and p.  p is the
   probability of the favoured outcome, n is the number of trials.
 */
Binomial::Binomial( long n, double p):n_(n), p_(p)
{

    if (( p < 0 ) || ( p > 1 ))
    {
        cerr << "ERROR: p must be in [0,1] range." << endl;
        p = 0.0;
        return;
    }
    if ( n < 1 )
    {
        cerr << "ERROR: n must be >= 1" << endl;
        return;
    }


    double tmpMean;
    double tmp;
    isInverted_ = false;

//     tmpMean = n*((p < 0.5)? p : (1-p));

//     if ((tmpMean > 10.0))
//     {
    // the above can be simplified as: ( saves one floating point comparison, aesthetically pleasing :D )
    if ( n > 20 )
    {
        if( p < 0.5 )
        {
            p_ = p;
        }
        else
        {
            p_ = 1.0 - p;
            isInverted_ = true;
        }
        tmpMean = n*p_;

        tmp = sqrt(tmpMean*(1.0 - p_));
        paramC_ = tmpMean + 0.5;
        paramB_ = 1.15 + 2.53*tmp;
        paramA_ = -0.0873 + 0.0248*paramB_ + 0.01*p_;
        paramAlpha_ = (2.83 + 5.1/paramB_)*tmp;
        paramUr_ = 0.43;
        paramVr_ = 0.92 - 4.2/paramB_;
        paramUrVr_ = 0.86*paramVr_;
        paramM_ = floor(tmpMean+p_);
        paramR_ = floor(p_/(1-p_));
        paramNr_ = (n+1)*paramR_;
        paramNpq_ = tmpMean*(1-p_);
    }
    mean_ = n_*p_;
}

long Binomial::getN() const
{
    return n_;
}

double Binomial::getP() const
{
    if ( isInverted_)
    {
        return 1 - p_;
    }
    else
    {
        return p_;
    }
}

double Binomial::getMean() const
{
    if (isInverted_)
    {
        return (n_ - mean_);
    }else{
        return mean_;
    }
}

double Binomial::getVariance() const
{
    static double variance = sqrt(n_*p_*(1.0-p_));
    return variance;
}

/**
   returns the next random number in this distribution as the ratio of
   the number of positive outcomes and the total number of trials.
   This is the most naive implementation. This is ok for small n.
   For large n such that mean > 10, we use BTRD algorithm by Hoermann.
   See documentation of generateTrd() for further detail.
*/
double Binomial::getNextSample() const
{
    double sample = 0;
    if ( p_ == 0 )
    {
        sample = (double)0;
    }
    else if ( isClose<double>(1.0,p_, DBL_EPSILON))
    {
        sample = (double)n_;
    }
    else
    {
        if ( mean_ > 10 )
        {
            sample = isInverted_? n_ - generateTrd(): generateTrd();
        }
        else
        {
            for ( unsigned int i = 0; i < n_; ++i)
            {
                double myRand = mtrand();
                if ( myRand < p_ )
                {
                    sample+=1;
                }
            }
        }
//        cerr << "Sample value: " << sample << " " << isInverted_<< endl;
    }

    return sample;
}

/**
   Generate binomially distributed random numbers using transformed
   rejection with decomposition as described in "The Generation of
   Binomial Random Variable" by W Hoermann.
*/
double Binomial::generateTrd() const
{

    double varV;
    double varU;
    double varUs;
    double varK;
    double varKm;
    double varF;
    double varNm;
    double varH;
    double varNk;
    double varI;
    double varRho;
    double varT;

    while ( true )
    {
        // 1a: generate a uniform random number v
        varV = mtrand();
        if ( varV <= paramUrVr_ )
        {
            // 1b: if v <= urvr then u = v/vr - 0.43
            double varU = varV/paramVr_ - 0.43;
            // 1c: return floor( (2*a/(0.5 - |u|) + b ) * u + c )
            return floor((2*paramA_/(0.5 - fabs(varU)) + paramB_ )*varU+paramC_);
        }
        // 2a: if ( v >= vr ) then generate a uniform random number u in (-0.5,+0.5)
        if ( varV >= paramVr_ )
        {
            varU = mtrand() - 0.5;
        }
        else  // 2b: otherwise
        {
            // 2b(i): set u = v/vr - 0.93
            varU = varV/paramVr_ - 0.93;
            // 2b(ii): set u = sign(u)*0.5 - u
            varU = (varU > 0)? 0.5 - varU : - 0.5 - varU;
            // 2b(iii) generate a uniform random number v in (0,vr)
            varV = mtrand()*paramVr_;
        }
        // 3.0a: us = 0.5 - |u|
        varUs = (varU < 0) ? 0.5 + varU : 0.5 - varU;
        // 3.0b: k = floor( ( 2*a/us + b )*u + c )
        varK = floor( (2*paramA_/varUs+paramB_)*varU + paramC_);
        // 3.0c: if k < 0 or k > n go to (1)
        if ( (varK < 0) || ( varK > n_ ))
        {
            continue;
        }

        // 3.0d: v = v*alpha/(a/(us*us)+b)
        varV = varV*paramAlpha_/(paramA_/(varUs*varUs) + paramB_ );
        // 3.0e: km = | k - m |
        varKm = ( varK < paramM_)? paramM_ - varK : varK - paramM_;
        // 3.0f: if km > 15 go to 3.2 , else ...
        if ( varKm <= 15 )
        {
            // 3.1: recursive evaluation of f(k)
            // 3.1a: f = 1
            varF = 1;
            // 3.1b: if (m < k)
            if ( paramM_ < varK )
            {
                // 3.1b(i): set i = m
                varI = paramM_;
                // 3.1b(ii): repeat i = i+1, f = f*(nr/i - r) until (i == k).
                while ( varI < varK )
                {
                    ++varI;
                    varF *= (paramNr_/varI - paramR_);
                }
            }
            else // 3.1c: otherwise ...
            {
                // 3.1c(i) if ( m > k)
                if ( paramM_ > varK )
                {
                    // 3.1c(ii): i = k
                    varI = varK;
                    // 3.1c(ii): repeat i = i+1, v = v*(nr/i - r) until (i == m).
                    while ( varI < paramM_)
                    {
                        ++varI;
                        varV *= (paramNr_/varI -paramR_);
                    }
                }
                //3.1d: if v <= f return k, otherwise goto (1)
                if ( varV <= varF )
                {
                    return varK;
                }
            }
        }
        // 3.2: Squeeze acceptance or rejection
        // 3.2a: v = log(v)
        varV = log(varV);
        // 3.2b: rho = ( km/npq )*(((km/3+0.625)*km + 1/6)/npq+0.5)
        varRho = (varKm/paramNpq_)*(((varKm/3.0+ 0.625)*varKm+0.16666666666666667)/paramNpq_+0.5);
        // 3.2c: t = - km*km/(2*npq)
        varT = -varKm*varKm*0.5/paramNpq_;
        // 3.2d: if ( v < t - rho ) then return k
        if ( varV < ( varT - varRho ))
        {
            return varK;
        }
        // 3.2e: if ( v > t+rho ) then goto (1)
        if ( varV > (varT + varRho))
        {
            continue;
        }
        // 3.3: Set-up for 3.4
        // 3.3a: nm = n - m + 1
        varNm = n_ - paramM_ + 1;
        // 3.3b: h = (m+0.5)*log((m+1)/(r*nm)) + fc(m) + fc(n-m)
        varH = (paramM_ + 0.5)*log((paramM_+1)/(paramR_*varNm)) + getFc((int)paramM_) + getFc((int)(n_-paramM_));
        // 3.4: Final acceptance-rejection test
        // 3.4a: nk = n - k + 1;
        varNk = n_ - varK + 1;
        // 3.4b: if ( v <= h + (n+1)*log(nm/nk) + (k+0.5)*log(nk*r/(k+1)) - fc(k) -fc(n-k) then return k
        if ( varV <= varH + (n_ + 1)*log(varNm/varNk) +  ( varK + 0.5)*log(varNk*paramR_/(varK+1)) - getFc((int)varK) - getFc((int)(n_ - varK)))
        {
            return varK;
        }
        // 3.4c: otherwise goto (1)
    }
}


/**
   TODO: what to do to automatically test the quality of the random
   number generation? We can check the mean and variance perhaps?
   We should also check the plot of the distribution manually.
*/
void testBinomial()
{

    int trialMin = 2;
    int trialMax = trialMin*1000;

    double tmp;


    for ( int i = trialMin; i < trialMax; i =(int)( i* 1.5) )
    {
        for ( double p = 0.1; p < .95; p += 0.1)
        {
            Binomial b(i, p);
            tmp = 0;
            for ( int j = 0; j < i; ++j )
            {
                tmp += b.getNextSample();
            }
            cerr << "Diff( " << i << "," << p << ") "
                 << tmp/i - b.getMean()
                 << " [ " << tmp/i << " - " << b.getMean() <<" ]"
                 << endl;
        }
    }
}
#if 0 // test main
int main(void)
{
    testBinomial();
    return 0;
}

#endif // test main


#endif
