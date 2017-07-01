/*******************************************************************
 * File:            RandGenerator.h
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-07 16:25:08
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

#ifndef _RANDGENERATOR_H
#define _RANDGENERATOR_H

#include "randnum.h"
#include "basecode/header.h"
#include "Probability.h"

/**
   This class is a moose interface to underlying random number
   generators (which is an instance of Probability class).
 */
class RandGenerator
{
  public:
    RandGenerator();
    virtual ~RandGenerator();
    double getMean() const;
    double getVariance() const;
    double getSample() const;
    void process( const Eref& e, ProcPtr info);
    void reinit( const Eref& e, ProcPtr info);
    virtual void vReinit( const Eref& e, ProcPtr info);

    static const Cinfo * initCinfo();
  protected:
    Probability* rng_;
    double sample_;
};


#endif
