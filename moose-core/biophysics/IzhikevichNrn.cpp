// IzhikevichNrn.cpp ---
//
// Filename: IzhikevichNrn.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Fri Jul  8 10:00:33 2011 (+0530)
// Version:
// Last-Updated: Mon Nov  5 16:58:20 2012 (+0530)
//           By: subha
//     Update #: 110
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

#include "../basecode/header.h"
#include "IzhikevichNrn.h"


static SrcFinfo1< double >* spikeOut()
{
    static SrcFinfo1< double > spike("spikeOut",
                                      "Sends out spike events");
    return &spike;
}

static SrcFinfo1< double >* VmOut()
{
    static SrcFinfo1< double > VmOut("VmOut",
                               "Sends out Vm");
    return &VmOut;
}

const Cinfo* IzhikevichNrn::initCinfo()
{

    static DestFinfo process(
            "process",
            "Handles process call",
            new ProcOpFunc< IzhikevichNrn >( &IzhikevichNrn::process));
    static DestFinfo reinit(
            "reinit",
            "Handles reinit call",
            new ProcOpFunc<IzhikevichNrn>( &IzhikevichNrn::reinit));

    static Finfo * processShared[] = {
        &process,
        &reinit
    };

    static SharedFinfo proc(
            "proc",
            "Shared message to receive Process message from scheduler",
            processShared, sizeof( processShared ) / sizeof( Finfo* ) );

    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////

    static ValueFinfo<IzhikevichNrn, double> Vmax(
            "Vmax",
            "Maximum membrane potential. Membrane potential is reset to c whenever"
            " it reaches Vmax. NOTE: Izhikevich model specifies the PEAK voltage,"
            " rather than THRSHOLD voltage. The threshold depends on the previous"
            " history.",
            &IzhikevichNrn::setVmax,
            &IzhikevichNrn::getVmax);
    static ValueFinfo<IzhikevichNrn, double> c(
            "c",
            "Reset potential. Membrane potential is reset to c whenever it reaches"
            " Vmax.",
            &IzhikevichNrn::setC,
            &IzhikevichNrn::getC);
    static ValueFinfo<IzhikevichNrn, double> d(
            "d",
            "Parameter d in Izhikevich model. Unit is V/s.",
            &IzhikevichNrn::setD,
            &IzhikevichNrn::getD);
    static ValueFinfo<IzhikevichNrn, double> a(
            "a",
            "Parameter a in Izhikevich model. Unit is s^{-1}",
            &IzhikevichNrn::setA,
            &IzhikevichNrn::getA);
    static ValueFinfo<IzhikevichNrn, double> b(
            "b",
            "Parameter b in Izhikevich model. Unit is s^{-1}",
            &IzhikevichNrn::setB,
            &IzhikevichNrn::getB);
    static ValueFinfo<IzhikevichNrn, double> Vm(
            "Vm",
            "Membrane potential, equivalent to v in Izhikevich equation.",
            &IzhikevichNrn::setVm,
            &IzhikevichNrn::getVm);
    static ReadOnlyValueFinfo<IzhikevichNrn, double> u(
            "u",
            "Parameter u in Izhikevich equation. Unit is V/s",
            &IzhikevichNrn::getU);
    static ReadOnlyValueFinfo<IzhikevichNrn, double> Im(
            "Im",
            "Total current going through the membrane. Unit is A.",
            &IzhikevichNrn::getIm);
    static ValueFinfo<IzhikevichNrn, double> initVm(
            "initVm",
            "Initial membrane potential. Unit is V.",
            &IzhikevichNrn::setInitVm,
            &IzhikevichNrn::getInitVm);
    static ValueFinfo<IzhikevichNrn, double> inject(
            "inject",
            "External current injection into the neuron",
            &IzhikevichNrn::setInject,
            &IzhikevichNrn::getInject);
    static ValueFinfo<IzhikevichNrn, double> initU(
            "initU",
            "Initial value of u.",
            &IzhikevichNrn::setInitU,
            &IzhikevichNrn::getInitU);

    static ValueFinfo<IzhikevichNrn, double> alpha(
            "alpha",
            "Coefficient of v^2 in Izhikevich equation. Defaults to 0.04 in "
            "physiological unit. In SI it should be 40000.0. Unit is V^-1 s^{-1}",
            &IzhikevichNrn::setAlpha,
            &IzhikevichNrn::getAlpha);

    static ValueFinfo<IzhikevichNrn, double> beta(
            "beta",
            "Coefficient of v in Izhikevich model. Defaults to 5 in physiological"
            " unit, 5000.0 for SI units. Unit is s^{-1}",
            &IzhikevichNrn::setBeta,
            &IzhikevichNrn::getBeta);
    static ValueFinfo<IzhikevichNrn, double> gamma(
            "gamma",
            "Constant term in Izhikevich model. Defaults to 140 in both"
            " physiological and SI units. unit is V/s.",
            &IzhikevichNrn::setGamma,
            &IzhikevichNrn::getGamma);
    static ValueFinfo<IzhikevichNrn, double> RmByTau(
            "RmByTau",
            "Hidden coefficient of input current term (I) in Izhikevich model. Defaults to 1e9 Ohm/s.",
            &IzhikevichNrn::setRmByTau,
            &IzhikevichNrn::getRmByTau);

    static ValueFinfo<IzhikevichNrn, bool> accommodating(
            "accommodating",
            "True if this neuron is an accommodating one. The equation for recovery"
            " variable u is special in this case.",
            &IzhikevichNrn::setAccommodating,
            &IzhikevichNrn::getAccommodating);

    static ValueFinfo<IzhikevichNrn, double> u0(
            "u0",
            "This is used for accommodating neurons where recovery variables u is"
            " computed as: u += tau*a*(b*(Vm-u0))",
            &IzhikevichNrn::setU0,
            &IzhikevichNrn::getU0);

    ///////////////////////////////
    // MsgDest definition
    ///////////////////////////////
    static DestFinfo injectMsg(
            "injectMsg",
            "Injection current into the neuron.",
            new OpFunc1<IzhikevichNrn, double>( &IzhikevichNrn::setInject));

    static DestFinfo cDest(
            "cDest",
            "Destination message to modify parameter c at runtime.",
            new OpFunc1<IzhikevichNrn, double>(&IzhikevichNrn::setC));

    static DestFinfo dDest(
            "dDest",
            "Destination message to modify parameter d at runtime.",
            new OpFunc1<IzhikevichNrn, double>(&IzhikevichNrn::setD));

    static DestFinfo aDest(
            "aDest",
            "Destination message modify parameter a at runtime.",
            new OpFunc1<IzhikevichNrn, double>(&IzhikevichNrn::setA));

    static DestFinfo bDest(
            "bDest",
            "Destination message to modify parameter b at runtime",
            new OpFunc1<IzhikevichNrn, double>(&IzhikevichNrn::setB));


    static DestFinfo handleChannel("handleChannel",
                                   "Handles conductance and reversal potential arguments from Channel",
                                   new OpFunc2<IzhikevichNrn, double, double >(&IzhikevichNrn::handleChannel));

    static Finfo * channelShared[] = {
        &handleChannel,
        VmOut()
    };

    static SharedFinfo channel("channel",
			"This is a shared message from a IzhikevichNrn to channels."
			"The first entry is a MsgDest for the info coming from "
			"the channel. It expects Gk and Ek from the channel "
			"as args. The second entry is a MsgSrc sending Vm ",
                               channelShared, sizeof( channelShared ) / sizeof( Finfo* )
	);
    static Finfo* IzhikevichNrnFinfos[] = {
        &proc,
        &Vmax,
        &c,
        &d,
        &a,
        &b,
        &u,
        &Vm,
        &Im,
        &inject,
        &RmByTau,
        &accommodating,
        &u0,
        &initVm,
        &initU,
        &alpha,
        &beta,
        &gamma,
        &injectMsg,
        &cDest,
        &dDest,
        &bDest,
        &aDest,
        VmOut(),
        spikeOut(),
        &channel,
    };

    static string doc[] = {
        "Name", "IzhikevichNrn",
        "Author", "Subhasis Ray",
        "Description", "Izhikevich model of spiking neuron "
        "(Izhikevich,EM. 2003. Simple model of spiking neurons. Neural"
        " Networks, IEEE Transactions on 14(6). pp 1569-1572).\n"
        " This class obeys the equations (in physiological units):\n"
        "  dVm/dt = 0.04 * Vm^2 + 5 * Vm + 140 - u + inject\n"
        "  du/dt = a * (b * Vm - u)\n"
        " if Vm >= Vmax then Vm = c and u = u + d\n"
        " Vmax = 30 mV in the paper."
    };
    static Dinfo< IzhikevichNrn > dinfo;
    static Cinfo IzhikevichNrnCinfo(
        "IzhikevichNrn",
        Neutral::initCinfo(),
        IzhikevichNrnFinfos,
        sizeof( IzhikevichNrnFinfos ) / sizeof( Finfo* ),
        &dinfo,
        doc,
        sizeof(doc)/sizeof(string));

    return &IzhikevichNrnCinfo;
}

static const Cinfo * IzhikevichNrnCinfo = IzhikevichNrn::initCinfo();

IzhikevichNrn::IzhikevichNrn():
        alpha_(40000.0), // 0.04 physiological unit
        beta_(5000.0), // 5 physiological unit
        gamma_(140.0), // 140 physiological unit
        RmByTau_(1e6), // Assuming Izhikevich was using nA as unit of
        // current, 1e6 Ohm will be the scaling term for SI
        a_(20.0),
        b_(200.0),
        c_(-0.065), // -65 mV
        d_(2.0), // assuming u is in mV/ms
        Vm_(-0.065),
        u_(-13.0),
        Vmax_(0.03), // 30 mV
        initVm_(-0.065),// -65 mV
        initU_(-13.0),
        sum_inject_(0.0),
        Im_(0.0),
        savedVm_(-0.065),
        accommodating_(false),
        u0_(-0.065),
        inject_(0.0)
{
    ;
}

IzhikevichNrn::~IzhikevichNrn()
{
    ;
}
void IzhikevichNrn::setA( double value)
{
    a_ = value;
}

double IzhikevichNrn::getA() const
{
    return a_;
}

void IzhikevichNrn::setB( double value)
{
    b_ = value;
}

double IzhikevichNrn::getB() const
{
    return b_;
}
void IzhikevichNrn::setC( double value)
{
    c_ = value;
}

double IzhikevichNrn::getC() const
{
    return c_;
}

void IzhikevichNrn::setD( double value)
{
    d_ = value;
}

double IzhikevichNrn::getD() const
{
    return d_;
}

void IzhikevichNrn::setRmByTau( double value)
{
    RmByTau_ = value;
}

double IzhikevichNrn::getRmByTau() const
{
    return RmByTau_;
}
void IzhikevichNrn::setVm( double value)
{
    Vm_ = value;
}
double IzhikevichNrn::getU() const
{
    return u_;
}

double IzhikevichNrn::getVm() const
{
    return savedVm_;
}

void IzhikevichNrn::setVmax( double value)
{
    Vmax_ = value;
}

double IzhikevichNrn::getVmax() const
{
    return Vmax_;
}

void IzhikevichNrn::setAlpha( double value)
{
    alpha_ = value;
}

double IzhikevichNrn::getAlpha() const
{
    return alpha_;
}

void IzhikevichNrn::setBeta( double value)
{
    beta_ = value;
}

double IzhikevichNrn::getBeta() const
{
    return beta_;
}

void IzhikevichNrn::setGamma( double value)
{
    gamma_ = value;
}

double IzhikevichNrn::getGamma() const
{
    return gamma_;
}

void IzhikevichNrn::setInject( double value)
{
    inject_ = value;
}

double IzhikevichNrn::getInject() const
{
    return inject_;
}

double IzhikevichNrn::getIm() const
{
    return Im_;
}

void IzhikevichNrn::setInitVm( double value)
{
    initVm_ = value;
}

double IzhikevichNrn::getInitVm() const
{
    return initVm_;
}

void IzhikevichNrn::setInitU( double value)
{
    initU_ = value;
}

double IzhikevichNrn::getInitU() const
{
    return initU_;
}

void IzhikevichNrn::setAccommodating( bool value)
{
    accommodating_ = value;
}

bool IzhikevichNrn::getAccommodating() const
{
    return accommodating_;
}

void IzhikevichNrn::setU0( double value)
{
    u0_ = value;
}

double IzhikevichNrn::getU0() const
{
    return u0_;
}

void IzhikevichNrn::handleChannel(double Gk, double Ek)
{
    sum_inject_ += Gk * (Ek - Vm_);
}

void IzhikevichNrn::process(const Eref& eref, ProcPtr proc)
{
    Vm_ += proc->dt * ((alpha_ * Vm_ + beta_) * Vm_
                       + gamma_ - u_ + RmByTau_ * sum_inject_);
    if (accommodating_){
        u_ += proc->dt * a_ * b_ * (Vm_ - u0_);
    } else {
        u_ += proc->dt * a_ * (b_ * Vm_ - u_);
    }
    Im_ = sum_inject_;
    sum_inject_ = inject_;
    // This check is to ensure that checking Vm field will always
    // return Vmax when Vm actually crosses Vmax.
    if (Vm_ >= Vmax_){
        Vm_ = c_;
        u_ += d_;
        savedVm_ = Vmax_;
        VmOut()->send(eref, Vmax_);
        spikeOut()->send(eref, proc->currTime);
    } else {
        savedVm_ = Vm_;
        VmOut()->send(eref, Vm_);
    }
}

void IzhikevichNrn::reinit(const Eref& eref, ProcPtr proc)
{
    sum_inject_ = 0.0;
    Vm_ = initVm_;
    u_ = initU_;
    Im_ = 0.0;
    savedVm_ = Vm_;
    VmOut()->send(eref, Vm_);
}

//
// IzhikevichNrn.cpp ends here
