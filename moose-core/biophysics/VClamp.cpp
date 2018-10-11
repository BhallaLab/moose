// VClamp.cpp ---
//
// Filename: VClamp.cpp
// Description:
// Author:
// Maintainer:
// Created: Fri Feb  1 19:30:45 2013 (+0530)
// Version:
// Last-Updated: Tue Jun 11 17:33:16 2013 (+0530)
//           By: subha
//     Update #: 297
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
// Implementation of Voltage Clamp
//
//

// Change log:
//
//
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 3, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
//
//

// Code:

#include "../basecode/header.h"
#include "../basecode/Dinfo.h"
#include "VClamp.h"

using namespace moose;

const unsigned int VClamp::DERIVATIVE_ON_PV = 1;
const unsigned int VClamp::PROPORTIONAL_ON_PV = 2;


SrcFinfo1< double >* VClamp::currentOut()
{
    static SrcFinfo1< double > currentOut("currentOut",
                                          "Sends out current output of the clamping circuit. This should be"
                                          " connected to the `injectMsg` field of a compartment to voltage clamp"
                                          " it.");
    return &currentOut;
}


const Cinfo * VClamp::initCinfo()
{
    static DestFinfo process("process",
                             "Handles 'process' call on each time step.",
                             new ProcOpFunc< VClamp >( &VClamp::process));
    static DestFinfo reinit("reinit",
                            "Handles 'reinit' call",
                            new ProcOpFunc< VClamp >( &VClamp::reinit));
    static Finfo * processShared[] = {
        &process,
        &reinit
    };

    static SharedFinfo proc("proc",
                            "Shared message to receive Process messages from the scheduler",
                            processShared, sizeof(processShared) / sizeof(Finfo*));

    static ValueFinfo< VClamp, double> command("command",
                                               "Command input received by the clamp circuit.",
                                               &VClamp::setCommand,
                                               &VClamp::getCommand);
    static ValueFinfo< VClamp, unsigned int> mode("mode",
                                                  "Working mode of the PID controller.\n"
                                                  "\n"
                                                  "   mode = 0, standard PID with proportional, integral and derivative"
                                                  " all acting on the error.\n"
                                                  "\n"
                                                  "   mode = 1, derivative action based on command input\n"
                                                  "\n"
                                                  "   mode = 2, proportional action and derivative action are based on"
                                                  " command input.",
                                                  &VClamp::setMode,
                                                  &VClamp::getMode);
    static ValueFinfo< VClamp, double> ti("ti",
                                          "Integration time of the PID controller. Defaults to 1e9, i.e. integral"
                                          " action is negligibly small.",
                                          &VClamp::setTi,
                                          &VClamp::getTi);

    static ValueFinfo< VClamp, double> td("td",
                                          "Derivative time of the PID controller. This defaults to 0,"
                                          "i.e. derivative action is unused.",
                                          &VClamp::setTd,
                                          &VClamp::getTd);
    static ValueFinfo< VClamp, double> tau("tau",
                                          "Time constant of the lowpass filter at input of the PID"
                                           " controller. This smooths out abrupt changes in the input. Set it to "
                                           " 5 * dt or more to avoid overshoots.",
                                          &VClamp::setTau,
                                          &VClamp::getTau);
    static ValueFinfo< VClamp, double> gain("gain",
                                          "Proportional gain of the PID controller.",
                                          &VClamp::setGain,
                                          &VClamp::getGain);
    static ReadOnlyValueFinfo< VClamp, double> current("current",
                                            "The amount of current injected by the clamp into the membrane.",
                                            &VClamp::getCurrent);


    static ReadOnlyValueFinfo<VClamp, double> sensed("sensed",
                                                     "Membrane potential read from compartment.",
                                                     &VClamp::getVin);
    static DestFinfo sensedIn("sensedIn",
                              "The `VmOut` message of the Compartment object should be connected"
                              " here.",
                              new OpFunc1<VClamp, double>( &VClamp::setVin));

    static DestFinfo commandIn("commandIn",
                              "  The command voltage source should be connected to this.",
                              new OpFunc1<VClamp, double>( &VClamp::setCommand));

    static Finfo* vclampFinfos[] = {
        currentOut(),
        &command,
        &current,
        &sensed,
        &mode,
        &ti,
        &td,
        &tau,
        &gain,
        &sensedIn,
        &commandIn,
        &proc
    };

    static string doc[] = {
        "Name", "VClamp",
        "Author", "Subhasis Ray",
        "Description", "Voltage clamp object for holding neuronal compartments at a specific"
        " voltage.\n"
        "\n"
        "This implementation uses a builtin RC circuit to filter the "
        " command input and then use a PID to bring the sensed voltage (Vm from"
        " compartment) to the filtered command potential.\n"
        "\n"
        "Usage: Connect the `currentOut` source of VClamp to `injectMsg`"
        " dest of Compartment. Connect the `VmOut` source of Compartment to"
        " `set_sensed` dest of VClamp. Either set `command` field to a"
        " fixed value, or connect an appropriate source of command potential"
        " (like the `outputOut` message of an appropriately configured"
        " PulseGen) to `set_command` dest.\n"
        "The default settings for the RC filter and PID controller should be"
        " fine. For step change in command voltage, good defaults with"
        "integration time step dt are as follows:\n"
        "    time constant of RC filter, tau = 5 * dt\n"
        "    proportional gain of PID, gain = Cm/dt where Cm is the membrane"
        "    capacitance of the compartment\n"
        "    integration time of PID, ti = dt\n"
        "    derivative time  of PID, td = 0\n"
        "\n",
    };

    static Dinfo< VClamp > dinfo;
    static Cinfo vclampCinfo(
        "VClamp",
        Neutral::initCinfo(),
        vclampFinfos,
        sizeof(vclampFinfos) / sizeof(Finfo*),
		&dinfo,
        doc,
        sizeof(doc)/sizeof(string));

    return &vclampCinfo;
}

static const Cinfo * vclampCinfo = VClamp::initCinfo();

VClamp::VClamp(): vIn_(0.0), command_(0.0), current_(0.0), mode_(0), ti_(0.0), td_(-1.0),
                  Kp_(0.0),
                  tau_(0.0),
                  tdByDt_(1.0),
                  dtByTi_(1.0),
                  e_(0.0),
                  e1_(0.0),
                  e2_(0.0)
{
}

VClamp::~VClamp()
{
    ;
}

void VClamp::setCommand(double value)
{
    cmdIn_ = value;
}

double VClamp::getCommand() const
{
    return cmdIn_;
}

void VClamp::setTi(double value)
{
    ti_ = value;
}

double VClamp::getTi() const
{
    return ti_;
}
void VClamp::setTd(double value)
{
    td_ = value;
}

double VClamp::getTd() const
{
    return td_;
}

void VClamp::setTau(double value)
{
    tau_ = value;
}

double VClamp::getTau() const
{
    return tau_;
}

void VClamp::setGain(double value)
{
    Kp_ = value;
}

double VClamp::getGain() const
{
    return Kp_;
}

double VClamp::getCurrent() const
{
    return current_;
}

void VClamp::setVin(double value)
{
    vIn_ = value;
}

double VClamp::getVin() const
{
    return vIn_;
}


void VClamp::setMode(unsigned int mode)
{
    mode_ = mode;
}

unsigned int VClamp::getMode() const
{
    return mode_;
}

void VClamp::process(const Eref& e, ProcPtr p)
{
    assert(ti_ > 0);
    assert(td_ >= 0);
    assert(tau_ > 0);
    double dCmd = cmdIn_ - oldCmdIn_;
    command_ = cmdIn_ + dCmd * ( 1 - tauByDt_) + (command_ - cmdIn_ + dCmd * tauByDt_) * expt_;
    oldCmdIn_ = cmdIn_;
    e_ = command_ - vIn_;
    if (mode_ == 0){
        current_ +=  Kp_ * ((1 + dtByTi_ + tdByDt_) * e_ - ( 1 + 2 * tdByDt_) * e1_ + tdByDt_ * e2_);
        e2_ = e1_;
        e1_ = e_;
    } else if (mode_ == DERIVATIVE_ON_PV){ // Here the derivative error term is replaced by process variable
        current_ +=  Kp_ * ((1 + dtByTi_) * e_ - e1_ + tdByDt_ * ( vIn_ - 2 * v1_ + e2_));
        e2_ = v1_;
        v1_ = vIn_;
        e1_ = e_;
    } else if (mode_ == PROPORTIONAL_ON_PV){ // Here the proportional as well as the derivative error term is replaced by process variable
        current_ +=  Kp_ * (vIn_ - v1_ + dtByTi_ * e_ + tdByDt_ * ( vIn_ - 2 * v1_ + e2_));
        e2_ = v1_;
        v1_ = vIn_;
    }
    currentOut()->send(e, current_);
}

void VClamp::reinit(const Eref& e, ProcPtr p)
{

    vIn_ = 0.0;
    v1_ = 0;
    command_ = cmdIn_ = oldCmdIn_ = e_ = e1_ = e2_ = 0;
    if (ti_ == 0){
        ti_ = p->dt;
    }
    if (td_ < 0){
        td_ = 0.0;
    }
    if (tau_ == 0.0){
        tau_ = 5 * p->dt;
    }
    if (p->dt / tau_ > 1e-15){
        expt_ = exp(-p->dt/tau_);
    } else {
        expt_ = 1 - p->dt/tau_;
    }
    tauByDt_ = tau_ / p->dt;
    dtByTi_ = p->dt/ti_;
    tdByDt_ = td_ / p->dt;

    vector<Id> compartments;
    unsigned int numComp = e.element()->getNeighbors(compartments, currentOut());
    if (numComp > 0){
        double Cm = Field<double>::get(compartments[0], "Cm");
    	if (Kp_ == 0){
            Kp_ = Cm / p->dt;
    	}
		command_ = cmdIn_ = oldCmdIn_ = 
           	Field<double>::get(compartments[0], "initVm");
    }
}

//
// VClamp.cpp ends here
