/*******************************************************************
 * File:            Binomial.h
 * Description:      Implements binomial distribution
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-10-28 13:42:24
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

#ifndef _BINOMIAL_H
#define _BINOMIAL_H
#include "Probability.h"

class Binomial:public Probability
{
  public:
    Binomial(){};
    Binomial( long n, double p);
    long getN() const;
    double getP() const;        
    double getMean() const;
    double getVariance() const;
    double getNextSample() const;
    
  private:
    double generateTrd() const;
    bool isInverted_;
    
    unsigned long n_;
    double p_;
    double mean_;
    
    double paramC_;
    double paramB_;
    double paramA_;
    double paramAlpha_;
    double paramUr_; 
    double paramVr_; 
    double paramUrVr_;
    double paramM_; 
    double paramR_; 
    double paramNr_;
    double paramNpq_;
};

    
#endif
