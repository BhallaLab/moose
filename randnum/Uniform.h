/*******************************************************************
 * File:            Uniform.h
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2008-02-21 17:09:54
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

#ifndef _UNIFORM_H
#define _UNIFORM_H

#include "Probability.h"

class Uniform: public Probability
{
  public:
    Uniform();
    Uniform(double min, double max);
    double getMean() const;
    double getVariance() const;
    double getNextSample() const;
    double getMin() const;
    double getMax() const;
    void setMin(double min);
    void setMax(double max);
    
  private:
    double min_;
    double max_;
    
};

    
#endif
