// DiffAmp.h ---
//
// Filename: DiffAmp.h
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Wed Feb 22 02:28:54 2012 (+0530)
// Version:
// Last-Updated: Wed Feb 22 03:03:01 2012 (+0530)
//           By: Subhasis Ray
//     Update #: 15
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

// Code:

#ifndef _DIFFAMP_H
#define _DIFFAMP_H

#include "../basecode/header.h"

class DiffAmp
{
  public:
    DiffAmp();
    ~DiffAmp();
    void setGain(double gain);
    double getGain() const;
    void setSaturation(double saturation);
    double getSaturation() const;
    double getOutput() const;
    void plusFunc(double input);
    void minusFunc(double input);
    void process(const Eref& e, ProcPtr p);
    void reinit(const Eref& e, ProcPtr p);

    static const Cinfo* initCinfo();

  protected:
    double gain_;
    double saturation_;
    double plus_;
    double minus_;
    double output_;
};

#endif

//
// DiffAmp.h ends here
