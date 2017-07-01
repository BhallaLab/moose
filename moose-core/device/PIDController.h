// PIDController.h ---
//
// Filename: PIDController.h
// Description:
// Author: subhasis ray
// Maintainer:
// Created: Tue Dec 30 23:14:00 2008 (+0530)
// Version:
// Last-Updated: Wed Feb 22 18:34:37 2012 (+0530)
//           By: Subhasis Ray
//     Update #: 57
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
//
//
//

// Change log:
//
// 2012-02-22 17:14:29 (+0530) subha - started porting to dh_branch
//
//
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _PIDCONTROLLER_H
#define _PIDCONTROLLER_H

#include "../basecode/header.h"

class PIDController{
  public:
    PIDController();

    void setCommand( double command );
    double getCommand() const;
    void setSensed(double sensed );
    double getSensed() const;
    double getOutput() const;
    void setGain(double gain );
    double getGain() const;
    void setTauI(double tau_i );
    double getTauI() const;
    void setTauD(double tau_d );
    double getTauD() const;
    void setSaturation(double saturation );
    double getSaturation() const;
    double getError() const;
    double getEIntegral() const;
    double getEDerivative() const;
    double getEPrevious() const;
    void process(const Eref&e, ProcPtr process );
    void reinit(const Eref& e, ProcPtr process );
    static const Cinfo * initCinfo();

  private:
    double command_;
    double saturation_;
    double gain_;
    double tau_i_;
    double tau_d_;
    double sensed_;
    double output_;
    double error_; // e of PIDController in GENESIS ( error = command - sensed )
    double e_integral_; // integral of error dt
    double e_derivative_; // derivative of error
    double e_previous_;
};

#endif

// Code:




//
// PIDController.h ends here
