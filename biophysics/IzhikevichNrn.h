// IzhikevichNrn.h ---
//
// Filename: IzhikevichNrn.h
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Fri Jul  8 09:55:21 2011 (+0530)
// Version:
// Last-Updated: Fri Jul  8 15:37:02 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 13
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
// Implementation of Izhikevich Neuron model.
//
//

// Change log:
//
//
//

// Code:


#ifndef _IZHIKEVICHNRN_H
#define _IZHIKEVICHNRN_H

class IzhikevichNrn
{
  public:
    IzhikevichNrn();
    ~IzhikevichNrn();
    void setA( double value );
    double getA() const;

    void setB( double value );
    double getB() const;

    void setC( double value );
    double getC() const;

    void setD( double value );
    double getD() const;

    void setVmax( double value );
    double getVmax() const;

    void setAlpha( double value );
    double getAlpha() const;

    void setBeta( double value );
    double getBeta() const;

    void setGamma( double value );
    double getGamma() const;

    double getIm() const;

    void setInject( double value );
    double getInject() const;

    void setVm( double value );
    double getVm() const;

    void setInitVm( double value );
    double getInitVm() const;

    void setInitU( double value );
    double getInitU() const;
    double getU() const;

    void setRmByTau(double value );
    double getRmByTau() const;

    void setAccommodating( bool value );
    bool getAccommodating() const;

    void setU0( double value );
    double getU0() const;

    void handleChannel( double Gk, double Ek);

    void process(const Eref& eref, ProcPtr proc );
    void reinit(const Eref& eref, ProcPtr proc );

    static const Cinfo * initCinfo();


  private:
    double alpha_;
    double beta_;
    double gamma_;
    double RmByTau_;
    double a_;
    double b_;
    double c_;
    double d_;
    double Vm_;
    double u_;
    double Vmax_;
    double initVm_;
    double initU_;
    double sum_inject_;
    double Im_;
    double savedVm_;
    bool accommodating_;
    double u0_;
    double inject_;
};

#endif

//
// IzhikevichNrn.h ends here
