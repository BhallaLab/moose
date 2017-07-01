/*******************************************************************
 * File:            NormalRng.cpp
 * Description:      This is the MOOSE front end for class Normal,
 *                   which generates normally distributed random
 *                   doubles.
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-03 22:07:04
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

#ifndef _NORMALRNG_CPP
#define _NORMALRNG_CPP
#include "NormalRng.h"
#include "Normal.h"
const Cinfo* NormalRng::initCinfo()
{
            static ValueFinfo< NormalRng, double > mean(
                "mean",
                "Mean of the normal distribution",
                &NormalRng::setMean,
                &NormalRng::getMean);
            static ValueFinfo< NormalRng, double > variance(
                "variance",
                "Variance of the normal distribution",
                &NormalRng::setVariance,
                &NormalRng::getVariance);
            static ValueFinfo< NormalRng, int > method(
                "method",
                "Algorithm used for computing the sample. The default is 0 = alias"
                " method by Ahrens and Dieter. Other options are: 1 = Box-Mueller method"
                " and 2 = ziggurat method.",
                &NormalRng::setMethod,
                &NormalRng::getMethod);
    static Finfo* normalRngFinfos[] = {
        &mean,
        &variance,
        &method,
    };

    static string doc[] = {
        "Name", "NormalRng",
        "Author", "Subhasis Ray",
        "Description", "Normally distributed random number generator.",
    };
    Dinfo< NormalRng > dinfo;
    static Cinfo normalRngCinfo(
        "NormalRng",
        RandGenerator::initCinfo(),
        normalRngFinfos,
        sizeof(normalRngFinfos)/sizeof(Finfo*),
        &dinfo,
        doc,
        sizeof( doc ) / sizeof( string ));
    return &normalRngCinfo;
}


static const Cinfo* normalRngCinfo = NormalRng::initCinfo();

/**
   Set the mean of the internal generator object.
 */
void NormalRng::setMean(double mean)
{
    Normal* nrng = static_cast < Normal* >(rng_);
    if (nrng){
        nrng->setMean(mean);
    }
}

/**
   Since normal distribution is defined in terms of mean and variance, we
   want to store them in order to create the internal generator object.
 */
void NormalRng::setVariance(double variance)
{
    if ( variance < 0 )
    {
        cerr << "ERROR: variance cannot be negative." << endl;
        return;
    }
    Normal * nrng = static_cast < Normal* >(rng_);
    if (nrng){
        nrng->setVariance(variance);
    }
}

/**
   Returns the algorithm used.
   0 for alias method.
   1 for BoxMueller method.
 */
int NormalRng::getMethod() const
{
    Normal* nrng = static_cast < Normal * > (rng_);
    if (nrng){
        return nrng->getMethod();
    }
    return 0;
}
/**
   Set the algorithm to be used.
   1 for BoxMueller method.
   Anything else for alias method.
 */
void NormalRng::setMethod(int method)
{
    Normal* nrng = static_cast <Normal*> (rng_);
    if ( nrng ){
        cout << "Warning: Changing method after generator object has been created. Current method: " << nrng->getMethod() << ". New method: " << method << endl;
        nrng->setMethod((NormalGenerator)method);
    }
}

void NormalRng::vReinit(const Eref& e, ProcPtr p)
{
    // do nothing
}

/**
   By default the method used for normal distribution is alias method
   by Ahrens and Dieter. In order to use some method other than the
   default Alias method, one should call setMethod with a proper
   method index before calling reset ( reinit ). Since different
   methods create different random sequences, the combined sequence
   may not have the intended distribution. By default mean and
   variance are set to 0.0 and 1.0 respectively.
 */
NormalRng::NormalRng():RandGenerator()
{
    rng_ = new Normal();
}
#endif
