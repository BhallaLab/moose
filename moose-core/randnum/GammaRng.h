/*******************************************************************
 * File:            GammaRng.h
 * Description:
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-11-08 11:53:29
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

#ifndef _GAMMARNG_H
#define _GAMMARNG_H
#include "randnum.h"
#include "basecode/header.h"
#include "RandGenerator.h"
#include "Gamma.h"

/**
   This is MOOSE wrapper for Gammaly distributed random number generator class, Gamma.
   The default
 */
class GammaRng: public RandGenerator
{
  public:
    GammaRng();
    virtual ~GammaRng() { ; }
    double getAlpha() const;
    double getTheta() const;
    void setAlpha(double alpha);
    void setTheta(double theta);

    virtual void vReinit( const Eref& e, ProcPtr p);

    static const Cinfo * initCinfo();

  private:
    double alpha_;
    double theta_;

    bool isAlphaSet_;
    bool isThetaSet_;
};


#endif
