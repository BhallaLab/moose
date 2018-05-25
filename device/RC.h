// RC.h ---
//
// Filename: RC.h
// Description:
// Author: subhasis ray
// Maintainer:
// Created: Wed Dec 31 15:18:22 2008 (+0530)
// Version:
// Last-Updated: Wed Feb 22 20:29:35 2012 (+0530)
//           By: Subhasis Ray
//     Update #: 54
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
//
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

// Code:

#ifndef _RC_H
#define _RC_H

#include "../basecode/header.h"

class RC{
  public:
    RC();
    void setV0(double voltage);
    double getV0() const;
    void setResistance(double resistance);
    double getResistance() const;
    void setCapacitance(double capacitance);
    double getCapacitance() const;
    double getState() const;
    void setInject(double inject);
    double getInject() const;
    void setInjectMsg(double inject);
    void process(const Eref& e, ProcPtr proc);
    void reinit(const Eref& e, ProcPtr proc);
    static const Cinfo * initCinfo();
  private:
    double v0_;
    double resistance_;
    double capacitance_;
    double state_;
    double inject_;
    double msg_inject_;
    double expTau_;
    double dt_tau_;
};

#endif


//
// RC.h ends here
