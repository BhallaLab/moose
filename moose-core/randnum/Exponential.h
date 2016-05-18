/*******************************************************************
 * File:            Exponential.h
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-01 08:59:47
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

#ifndef _EXPONENTIAL_H
#define _EXPONENTIAL_H
#include "Probability.h"
enum ExponentialGenerator
{
    LOGARITHMIC,
    RANDOM_MINIMIZATION
};

    
class Exponential: public Probability
{
  public:
    Exponential(double mean);
    Exponential( ExponentialGenerator generator, double mean);
    
    double getMean() const;
    double getVariance() const;
    double getNextSample() const;
  private:
    double mean_;
    double (*generator_)(double);
    static double logarithmic(double mean);
    static double randomMinimization(double mean);
    
    
};

    

#endif
