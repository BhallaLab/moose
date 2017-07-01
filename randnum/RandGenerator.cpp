/*******************************************************************
 * File:            RandGenerator.cpp
 * Description:     Interface class for MOOSE to access various
 *                  random number generator.
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-03 21:48:17
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

#ifndef _RANDGENERATOR_CPP
#define _RANDGENERATOR_CPP

#include "../basecode/header.h"
#include "RandGenerator.h"

static SrcFinfo1< double >* output()
{
    static SrcFinfo1< double > output(
        "output",
        "Generated random number.");
    return &output;
}

const Cinfo * RandGenerator::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////
    static DestFinfo process(
        "process",
        "Handles process call, updates internal time stamp.",
        new ProcOpFunc< RandGenerator >( &RandGenerator::process ));
    static DestFinfo reinit(
        "reinit",
        "Handles reinit call.",
        new ProcOpFunc< RandGenerator >( &RandGenerator::reinit ));
    //////////////////////////////////////////////////////////////
    // SharedMsg Definitions
    //////////////////////////////////////////////////////////////
    static Finfo* procShared[] = {
        &process, &reinit
    };
    static SharedFinfo proc(
        "proc",
        "Shared message for process and reinit",
        procShared, sizeof( procShared ) / sizeof( const Finfo* ));

    //////////////////////////////////////////////////////////////
    // ValueFinfo Definitions
    //////////////////////////////////////////////////////////////

    static ReadOnlyValueFinfo<RandGenerator, double> sample(
        "sample",
        "Generated pseudorandom number.",
        &RandGenerator::getSample);
    static ReadOnlyValueFinfo< RandGenerator, double > mean(
        "mean",
        "Mean of the distribution.",
        &RandGenerator::getMean);
    static ReadOnlyValueFinfo<RandGenerator, double> variance(
        "variance",
        "Variance of the distribution.",
        &RandGenerator::getVariance);

    static Finfo * randGeneratorFinfos[] = {
        &sample,
        &mean,
        &variance,
        output(),
        &proc,
    };

    static string doc[] = {
        "Name", "RandGenerator",
        "Author", "Subhasis Ray",
        "Description", "Base class for random number generators for sampling various"
        " probability distributions. This class should not be used"
        " directly. Instead, its subclasses named after specific distributions"
        " should be used.",
    };

    static Dinfo< RandGenerator > dinfo;
    static Cinfo randGeneratorCinfo(
        "RandGenerator",
        Neutral::initCinfo(),
        randGeneratorFinfos,
        sizeof(randGeneratorFinfos)/sizeof(Finfo*),
        &dinfo,
        doc,
        sizeof( doc ) / sizeof( string ));

    return &randGeneratorCinfo;
}

static const Cinfo * randGeneratorCinfo = RandGenerator::initCinfo();

RandGenerator::RandGenerator()
{
    sample_ = 0.0;
    rng_ = NULL;
}

RandGenerator::~RandGenerator()
{
    if (rng_)
    {
        delete rng_;
        rng_ = NULL;
    }
}

double RandGenerator::getMean() const
{
    if (rng_)
    {
        return rng_->getMean();
    }
    return 0.0;
}

double RandGenerator::getVariance() const
{
    if (rng_)
    {
        return rng_->getVariance();
    }
    return 0.0;
}

double RandGenerator::getSample() const
{
    return sample_;
}

void RandGenerator::process( const Eref& e, ProcPtr p )
{
    if (rng_){
        sample_ = rng_->getNextSample();
        output()->send(e, sample_);
    }
}

void RandGenerator::reinit(const Eref& e, ProcPtr p)
{
    vReinit(e, p);
}

void RandGenerator::vReinit(const Eref& e, ProcPtr p)
{
    cerr << "RandGenerator::vReinit() - this function should never be reached. Guilty party: " << e.id().path() << endl;
}

#endif
