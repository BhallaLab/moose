/*******************************************************************
 * File:            BinomialRng.h
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-08 10:48:59
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

#ifndef _BINOMIALRNG_H
#define _BINOMIALRNG_H

#include "randnum.h"
#include "../basecode/header.h"
#include "RandGenerator.h"
#include "Binomial.h"

class BinomialRng: public RandGenerator
{
  public:
    BinomialRng();
    void setN(double n);
    double getN() const;
    void setP(double p);
    double getP() const;
    virtual void vReinit( const Eref& e, ProcPtr p);

    static const Cinfo * initCinfo();
  private:
    bool isNSet_;
    unsigned long n_;
    bool isPSet_;
    double p_;
    bool isModified_;
};

#endif
