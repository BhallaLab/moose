// RC.cpp ---
//
// Filename: RC.cpp
// Description:
// Author: subhasis ray
// Maintainer:
// Created: Wed Dec 31 15:47:45 2008 (+0530)
// Version:
// Last-Updated: Tue Jun 11 17:01:03 2013 (+0530)
//           By: subha
//     Update #: 247
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
**           copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

// Code:

#include "RC.h"

static SrcFinfo1< double >* outputOut()
{
    static SrcFinfo1< double > outputOut( "output",
                                          "Current output level.");
    return &outputOut;
}

const Cinfo* RC::initCinfo()
{
        static DestFinfo process("process",
                      "Handles process call.",
                      new ProcOpFunc<RC>(&RC::process));
        static DestFinfo reinit( "reinit",
                       "Handle reinitialization",
                       new ProcOpFunc<RC>( &RC::reinit ));
    static Finfo* processShared[] = {
		&process, &reinit
    };
    static SharedFinfo proc("proc",
                                         "This is a shared message to receive Process messages "
                                         "from the scheduler objects."
                                         "The first entry in the shared msg is a MsgDest "
                                         "for the Process operation. It has a single argument, "
                                         "ProcInfo, which holds lots of information about current "
                                         "time, thread, dt and so on. The second entry is a MsgDest "
                                         "for the Reinit operation. It also uses ProcInfo. ",
                                         processShared,
                                         sizeof( processShared ) / sizeof( Finfo* ));
        static ValueFinfo<RC, double> V0( "V0",
                                    "Initial value of 'state'",
                                    &RC::setV0,
                                    &RC::getV0 );
        static ValueFinfo<RC, double> R( "R",
                                    "Series resistance of the RC circuit.",
                                    &RC::setResistance,
                                    &RC::getResistance);
        static ValueFinfo<RC, double> C( "C",
                                    "Parallel capacitance of the RC circuit.",
                                    &RC::setCapacitance,
                                    &RC::getCapacitance);
        static ReadOnlyValueFinfo<RC, double> state("state",
                               "Output value of the RC circuit. This is the voltage across the"
                               " capacitor.",
                               &RC::getState);
        static ValueFinfo<RC, double> inject( "inject",
                        "Input value to the RC circuit.This is handled as an input current to"
                        " the circuit.",
                        &RC::setInject,
                        &RC::getInject );
        static DestFinfo injectIn( "injectIn",
                       "Receives input to the RC circuit. All incoming messages are summed up"
                       " to give the total input current." ,
                       new OpFunc1<RC, double>(&RC::setInjectMsg));
    static Finfo* rcFinfos[] = {
        &V0,
        &R,
        &C,
        &state,
        &inject,
        outputOut(),
        &injectIn,
        &proc,
    };
    static string doc[] = {
        "Name", "RC",
        "Author", "Subhasis Ray, 2008, NCBS",
        "Description", "RC circuit: a series resistance R shunted by a capacitance C." };
    static Dinfo<RC> dinfo;
    static Cinfo rcCinfo("RC",
                         Neutral::initCinfo(),
                         rcFinfos,
                         sizeof( rcFinfos ) / sizeof( Finfo* ),
                         &dinfo,
                         doc,
                         sizeof(doc)/sizeof(string)
                         );
    return &rcCinfo;
}

static const Cinfo* rcCinfo = RC::initCinfo();


RC::RC():
        v0_(0),
        resistance_(1.0),
        capacitance_(1.0),
        state_(0),
        inject_(0),
        msg_inject_(0.0),
        expTau_(0.0),
        dt_tau_(0.0)
{
    ;   // Do nothing
}

void RC::setV0( double v0 )
{

    v0_ = v0;
}

double RC::getV0() const
{

    return v0_;
}

void RC::setResistance( double resistance )
{

    resistance_ = resistance;
}

double RC::getResistance( ) const
{

    return resistance_;
}

void RC::setCapacitance( double capacitance )
{

    capacitance_ = capacitance;
}

double RC::getCapacitance() const
{

    return capacitance_;
}

double RC::getState() const
{

    return state_;
}

void RC::setInject( double inject )
{

    inject_ = inject;
}

double RC::getInject() const
{

    return inject_;
}

void RC::setInjectMsg( double inject )
{

    msg_inject_ += inject;
}

/**
   calculates the new voltage across the capacitor.  this is the exact
   solution as described in Electronic Circuit and System Simulation
   Methods by Lawrance Pillage, McGraw-Hill Professional, 1999. pp
   87-100. Eqn: 4.7.21 */

void RC::process(const Eref& e, const ProcPtr proc )
{
	/*
    double sum_inject_prev = inject_ + msg_inject_;
    double sum_inject = inject_ + msg_inject_;
    double dVin = (sum_inject - sum_inject_prev) * resistance_;
    double Vin = sum_inject * resistance_;
    state_ = Vin + dVin - dVin / dt_tau_ +
            (state_ - Vin + dVin / dt_tau_) * expTau_;
    sum_inject_prev = sum_inject;
    msg_inject_ = 0.0;
    outputOut()->send(e, state_);
	*/

	////////////////////////////////////////////////////////////////
	// double A = inject + msgInject_;
	// double B = 1.0/resistance_;
	// double x = exp( -B * proc->dt / capacitance_ );
	// state_ = state_ * x + (A/B) * (1.0-x);
	// double x = exp( -dt_tau_ );
	state_ = state_ * expTau_ + 
		resistance_*(inject_ + msg_inject_) * (1.0-expTau_);
	
	/// OR, if we use simple Euler: ///
	// state_ += (inject_ + msgInject_ - state_/resistance_ ) * proc->dt / capacitance_;

    msg_inject_ = 0.0;
    outputOut()->send(e, state_);
}

void RC::reinit(const Eref& e, const ProcPtr proc)
{

    dt_tau_ = proc->dt / (resistance_ * capacitance_);
    state_ = v0_;
    if (dt_tau_ > 1e-15){
        expTau_ = exp(-dt_tau_);
    } else {// use approximation
        expTau_ = 1 - dt_tau_;
    }
    msg_inject_ = 0.0;
    outputOut()->send(e, state_);
}


//
// RC.cpp ends here
