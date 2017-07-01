/*******************************************************************
 * File:            BinomialRng.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-08 10:58:01
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

#ifndef _BINOMIALRNG_CPP
#define _BINOMIALRNG_CPP
#include "BinomialRng.h"
#include "Binomial.h"
#include "utility/numutil.h"
#include <cmath>
extern const Cinfo* initRandGeneratorCinfo();

const Cinfo* BinomialRng::initCinfo()
{
    static ValueFinfo< BinomialRng, double > n(
        "n",
        "Parameter n of the binomial distribution. In a coin toss experiment,"
        " this is the number of tosses.",
        &BinomialRng::setN,
        &BinomialRng::getN);
    static ValueFinfo < BinomialRng, double > p(
        "p",
        "Parameter p of the binomial distribution. In a coin toss experiment,"
" this is the probability of one of the two sides of the coin being on"
" top.",
        &BinomialRng::setP,
        &BinomialRng::getP);
    static Finfo* binomialRngFinfos[] = {
        &n,
        &p,
    };

    static string doc[] = {
        "Name", "BinomialRng",
        "Author", "Subhasis Ray",
        "Description", "Binomially distributed random number generator.",
    };
    Dinfo < BinomialRng> dinfo;
    static Cinfo binomialRngCinfo(
        "BinomialRng",
        RandGenerator::initCinfo(),
        binomialRngFinfos,
        sizeof(binomialRngFinfos)/sizeof(Finfo*),
        &dinfo,
        doc,
        sizeof( doc ) / sizeof( string ));
    return &binomialRngCinfo;
}


static const Cinfo* binomialRngCinfo = BinomialRng::initCinfo();

BinomialRng::BinomialRng()
{
    isNSet_ = false;
    isPSet_ = false;
    isModified_ = true;

    n_ = 0;
    p_ = 0;
}

/**
   Set parameter n ( number of trials for a two-outcome experiment).
   This must be set before the actual generator is instantiated.
 */
void BinomialRng::setN(double value)
{
    unsigned long n = (unsigned long)value;
    if ( n <= 0 )
    {
        cerr << "ERROR: BinomialRng::innerSetN - n must be a positive integer." << endl;
        return;
    }

    if(!isNSet_)
    {
        isNSet_ = true;
        n_ = n;
    }
    else
    {
        if (n_!= n  )
        {
            n_ = n;
            isModified_ = true;
        }
    }

    if ( isNSet_ && isPSet_ && isModified_)
    {   {
            if ( rng_ )
            {
                delete rng_;
            }
            rng_ = new Binomial((unsigned long)n_,p_);
            isModified_ = false;
        }
    }
}

/**
   Returns parameter n.
 */
double BinomialRng::getN() const
{
    return n_;
}

/**
   Set parameter p ( the probability of the outcome of interest ).
   This must be set before the actual generator is instantiated.
 */
void BinomialRng::setP(double p)
{
    if ( p < 0 || p > 1) {
        cerr << "ERROR: BinomialRng::setP - p must be in (0,1) range." << endl;
        return;
    }
    if ( !isPSet_) {
        p_ = p;
        isPSet_ = true;
    } else {
        if (!isClose< double >(p_,p, DBL_EPSILON)) {
            p_ = p;
            isModified_ = true;
        }
    }

    if ( isNSet_ && isPSet_ && isModified_ ){
        if ( rng_ ){
            delete rng_;
        }
        rng_ = new Binomial((long)(n_),p_);
        isModified_ = false;
    }
}

/**
   returns parameter p.
*/
double BinomialRng::getP() const
{
    return p_;
}


/**
   reports error if one or more of the parameters are not set.
*/
void BinomialRng::vReinit( const Eref& e, ProcPtr p)
{
    if ( isNSet_ ){
        if ( isPSet_ ){
            if ( !rng_ ){
                rng_ = new Binomial((unsigned long)(n_), p_);
            }
        } else {
            cerr << "ERROR: BinomialRng::reinit - first set value of p." << endl;
        }
    } else {
        cerr << "ERROR: BinomialRng::reinit - first set value of n." << endl;
    }
}


#endif
