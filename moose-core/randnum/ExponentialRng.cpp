/*******************************************************************
 * File:            ExponentialRng.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-08 11:33:45
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

#ifndef _EXPONENTIALRNG_CPP
#define _EXPONENTIALRNG_CPP

#include "RandGenerator.h"
#include "ExponentialRng.h"

const Cinfo* ExponentialRng::initCinfo()
{
    static ValueFinfo< ExponentialRng, double > mean(
        "mean",
        "Mean of the exponential distribution.",
        &ExponentialRng::setMean,
        &ExponentialRng::getMean);

    static ValueFinfo< ExponentialRng, int > method(
        "method",
        "The algorithm to use for computing the sample. Two methods are"
        " supported: 0 - logarithmic and 1 - random minimization."
        " The logarithmic method is slower (it computes a"
        " logarithm). Default is random minimization. See Knuth, Vol II Sec"
        " 3.4.1 : Algorithm S.",
        &ExponentialRng::setMethod,
        &ExponentialRng::getMethod);

    static Finfo* exponentialRngFinfos[] = {
        &mean,
        &method,
    };

    static string doc[] = {
        "Name", "ExponentialRng",
        "Author", "Subhasis Ray",
        "Description", "Exponentially distributed random number generator.\n"
        "Exponential distribution with mean k is defined by the probability"
        " density function p(x; k) = k * exp(-k * x) if x >= 0, else 0."
        " By default this class uses the random minimization method"
        " described in Knuth's TAOCP Vol II Sec 3.4.1 (Algorithm S).",
    };
    static Dinfo< ExponentialRng > dinfo;
    static Cinfo exponentialRngCinfo(
        "ExponentialRng",
        RandGenerator::initCinfo(),
        exponentialRngFinfos,
        sizeof(exponentialRngFinfos)/sizeof(Finfo*),
        &dinfo,
        doc,
        sizeof( doc ) / sizeof( string ));
    return &exponentialRngCinfo;
}


static const Cinfo* exponentialRngCinfo = ExponentialRng::initCinfo();

ExponentialRng::ExponentialRng()
{
    mean_ = 0;
    isMeanSet_ = false;
    method_ = RANDOM_MINIMIZATION;
}
/**
   Replaces the same method in base class.  Returns the mean as
   stored in this object independent of the actual generator object.
 */
double ExponentialRng::getMean() const
{
    return mean_;
}
/**
   Sets the mean. Since exponential distribution is defined in terms
   of this parameter, it is stored locally independent of the
   instantiation of the internal generator object.
*/
void ExponentialRng::setMean(double mean)
{
    if ( !rng_ ){
        rng_ = new Exponential(mean);
        isMeanSet_ = true;
    }
}

/**
   Reports error in case the parameter mean has not been set.
 */
void ExponentialRng::vReinit(const Eref& e, ProcPtr p)
{
    Exponential * erng = static_cast<Exponential *>(rng_);
    if (!erng){
        cerr << "ERROR: ExponentialRng::vReinit - mean must be set before using the Exponential distribution generator." << endl;
    }
}

/**
   Returns the algorithm used for sample generation.
   0 for logarithmic method.
   1 for random minimization method.
 */
int ExponentialRng::getMethod() const
{
   return method_;
}

/**
   Sets the algorithm used for sample generation.
   0 for logarithmic method.
   1 for random minimization method.
   Default is random minimization.
 */
void ExponentialRng::setMethod(int method)
{
    Exponential * erng = static_cast< Exponential * >(rng_);
    if (!erng){
        switch ( method ){
            case 0:
                method_ = LOGARITHMIC;
                break;
            default:
                method_ = RANDOM_MINIMIZATION;
                break;
        }
    } else {
        cerr << "Warning: Will not change method after generator object has been"
             << " created. Method in use:"
             << method << " ("
             << (method == 0? "logarithmic": "random minimization")
             << ")" << endl;
    }
}

#endif
