/*******************************************************************
 * File:            Poisson.h
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-02 09:43:47
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

#ifndef _POISSON_H
#define _POISSON_H

#include "Probability.h"
#include "Gamma.h"
#include "Binomial.h"

class Poisson;

typedef  double (Poisson::*PoissonGenerator)() const;

#define CALL_POSSON_GENERATOR(object,ptrToMember) ((object).*(ptrToMember))

class Poisson:public Probability
{
  public:
    Poisson(double mean=1.0);
    ~Poisson();

    void setMean(double mean);
    double getMean() const;
    double getVariance() const;
    double getNextSample() const;
  private:
    double mean_;
    static double poissonSmall(const Poisson&);
    static double poissonLarge(const Poisson&);
    Gamma* gammaGen_;
    //Binomial* binomialGen_;
    double (*generator_)(const Poisson&);
    double mValue_;
};

#endif
