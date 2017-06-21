/*******************************************************************
 * File:            Normal.h
 * Description:      Generates random numbers with normal
 *                      distribution.
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-10-30 11:22:51
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

#ifndef _NORMAL_H
#define _NORMAL_H
#include "Probability.h"
enum NormalGenerator
{
    ALIAS,
    BOX_MUELLER,
    ZIGGURAT
};

class Normal : public Probability
{
    
  public:
    Normal(double mean=0.0, double variance=1.0, NormalGenerator algorithm=ALIAS);
    double getMean()  const;
    void setMean(double value);
    double getVariance() const;
    void setVariance( double value );
    NormalGenerator getMethod(void);
    void setMethod(NormalGenerator method);
    double getNextSample() const;
  private:
    double mean_;
    double variance_;
    double (*generator_)();
    bool isStandard_;
    NormalGenerator method_;
    static double BoxMueller();
    static double aliasMethod();
    static double gslZiggurat();
    static bool testAcceptance(unsigned long t, unsigned long v);    
};

    
#endif
