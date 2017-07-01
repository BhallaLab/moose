/*******************************************************************
 * File:            UniformRng.cpp
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2008-02-01 11:30:20
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

#ifndef _UNIFORMRNG_CPP
#define _UNIFORMRNG_CPP
#include "../basecode/header.h"
#include "randnum.h"
#include "UniformRng.h"
#include "Uniform.h"

const Cinfo* UniformRng::initCinfo()
{
    static ValueFinfo< UniformRng, double > min(
        "min",
        "The lower bound on the numbers generated ",
        &UniformRng::setMin,
        &UniformRng::getMin);
    static ValueFinfo< UniformRng, double > max(
        "max",
        "The upper bound on the numbers generated",
        &UniformRng::setMax,
        &UniformRng::getMax);

    static Finfo * uniformRngFinfos[] = {
        &min,
        &max,
    };
    static string doc[] =
	{
		"Name", "UniformRng",
		"Author", "Subhasis Ray",
		"Description", "Generates pseudorandom number from a unform distribution.",
	};
    static Dinfo< UniformRng > dinfo;
    static Cinfo uniformRngCinfo(
        "UniformRng",
        RandGenerator::initCinfo(),
        uniformRngFinfos,
        sizeof( uniformRngFinfos ) / sizeof( Finfo* ),
        &dinfo,
        doc,
        sizeof( doc ) / sizeof( string ));
    return &uniformRngCinfo;
}

static const Cinfo* uniformRngCinfo = UniformRng::initCinfo();

double UniformRng::getMin()const
{
    Uniform * urng = static_cast< Uniform * >(rng_);
    if (urng){
        return urng->getMin();
    }
    return 0.0;
}

double UniformRng::getMax() const
{
    Uniform * urng = static_cast< Uniform * >(rng_);
    if (urng){
        return urng->getMax();
    }
    return 0.0;
}

void UniformRng::setMin(double min)
{
    Uniform * urng = static_cast<Uniform *> (rng_);
    if (urng){
        urng->setMin(min);
    }
}

void UniformRng::setMax(double max)
{
    Uniform * urng = static_cast<Uniform *> (rng_);
    if (urng){
        urng->setMax(max);
    }
}

UniformRng::UniformRng():RandGenerator()
{
    rng_ = new Uniform();
}

void UniformRng::vReinit(const Eref& e, ProcPtr p)
{
    ;    /* no use */
}

#ifdef DO_UNIT_TESTS
void testUniformRng()
{
    cout << "testUniformRng(): yet to be implemented" << endl;
}

#endif
#endif
