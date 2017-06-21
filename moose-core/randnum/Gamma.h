/*******************************************************************
 * File:            Gamma.h
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-05 19:22:32
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

#ifndef _GAMMA_H
#define _GAMMA_H
#include "Probability.h"
#include <iostream>
using namespace std;

class Gamma: public Probability
{
  public:
    Gamma(double alpha, double theta);
    double getAlpha();
    double getTheta();    
    double getMean()  const;
    double getVariance() const;
    double getNextSample() const;
  private:
    double alpha_;
    double theta_;
    double gammaSmall() const;
    double gammaLarge() const;
    
};

    
#endif
