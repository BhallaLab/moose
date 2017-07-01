/*******************************************************************
 * File:            UniformRng.h
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2008-02-01 11:52:48
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

#ifndef _UNIFORMRNG_H
#define _UNIFORMRNG_H

#include "randnum.h"
#include "RandGenerator.h"

class UniformRng: public RandGenerator
{
  public:
    UniformRng();
    virtual ~UniformRng() { ; }
    double getMin() const;
    double getMax() const;
    void setMin(double min);
    void setMax(double max);
    virtual void vReinit(const Eref& e, ProcPtr p);

    static const Cinfo * initCinfo();
  private:
    double min_;
    double max_;
};



#endif
