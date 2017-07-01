// PIDController.cpp ---
//
// Filename: PIDController.cpp
// Description:
// Author: subhasis ray
// Maintainer:
// Created: Tue Dec 30 23:36:01 2008 (+0530)
// Version:
// Last-Updated: Tue Jun 11 17:00:51 2013 (+0530)
//           By: subha
//     Update #: 338
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

#include <cfloat>
#include "PIDController.h"


static SrcFinfo1< double > * outputOut()
{
    static SrcFinfo1 <double> outputOut("output",
                                        "Sends the output of the PIDController. This is known as manipulated"
                                        " variable (MV) in control theory. This should be fed into the process"
                                        " which we are trying to control.");
    return &outputOut;
}

const Cinfo* PIDController::initCinfo()
{
       static DestFinfo process( "process",
                       "Handle process calls.",
                       new ProcOpFunc<PIDController>( &PIDController::process));
	   static DestFinfo reinit( "reinit",
                       "Reinitialize the object.",
                       new ProcOpFunc<PIDController>( &PIDController::reinit ));
    static Finfo* processShared[] = {
		&process, &reinit
    };

        static ValueFinfo<PIDController, double> gain( "gain",
                                                "This is the proportional gain (Kp). This tuning parameter scales the"
                                                " proportional term. Larger gain usually results in faster response, but"
                                                " too much will lead to instability and oscillation.",
                                                &PIDController::setGain,
                                                &PIDController::getGain);
        static ValueFinfo<PIDController, double> saturation("saturation",
                                              "Bound on the permissible range of output. Defaults to maximum double"
                                              " value.",
                                              &PIDController::setSaturation,
                                              &PIDController::getSaturation);
        static ValueFinfo<PIDController, double> command("command",
                                              "The command (desired) value of the sensed parameter. In control theory"
                                              " this is commonly known as setpoint(SP).",
                                              &PIDController::setCommand,
                                              &PIDController::getCommand);
        static ReadOnlyValueFinfo<PIDController, double> sensed( "sensed",
                                                       "Sensed (measured) value. This is commonly known as process variable"
                                                       "(PV) in control theory.",
                                                       &PIDController::getSensed);
        static ValueFinfo<PIDController, double> tauI( "tauI",
                                               "The integration time constant, typically = dt. This is actually"
                                               " proportional gain divided by integral gain (Kp/Ki)). Larger Ki"
                                               " (smaller tauI) usually leads to fast elimination of steady state"
                                               " errors at the cost of larger overshoot.",
                                               &PIDController::setTauI,
                                               &PIDController::getTauI);
        static ValueFinfo<PIDController, double> tauD( "tauD",
                                               "The differentiation time constant, typically = dt / 4. This is"
                                               " derivative gain (Kd) times proportional gain (Kp). Larger Kd (tauD)"
                                               " decreases overshoot at the cost of slowing down transient response"
                                               " and may lead to instability.",
                                               &PIDController::setTauD,
                                               &PIDController::getTauD);
        static ReadOnlyValueFinfo<PIDController, double> outputValue( "outputValue",
                                                       "Output of the PIDController. This is given by:"
                                                       "      gain * ( error + INTEGRAL[ error dt ] / tau_i   + tau_d * d(error)/dt )\n"
                                                       "Where gain = proportional gain (Kp), tau_i = integral gain (Kp/Ki) and"
                                                       " tau_d = derivative gain (Kd/Kp). In control theory this is also known"
                                                       " as the manipulated variable (MV)",
                                                       &PIDController::getOutput);
        static ReadOnlyValueFinfo<PIDController, double> error( "error",
                                                       "The error term, which is the difference between command and sensed"
                                                       " value.",
                                                       &PIDController::getError);
        static ReadOnlyValueFinfo<PIDController, double> integral( "integral",
                                                       "The integral term. It is calculated as INTEGRAL(error dt) ="
                                                       " previous_integral + dt * (error + e_previous)/2.",
                                                       &PIDController::getEIntegral );
        static ReadOnlyValueFinfo<PIDController, double> derivative( "derivative",
                                                       "The derivative term. This is (error - e_previous)/dt.",
                                                       &PIDController::getEDerivative );
        static ReadOnlyValueFinfo<PIDController, double> e_previous( "e_previous",
                                                       "The error term for previous step.",
                                                       &PIDController::getEPrevious);
        static DestFinfo commandIn( "commandIn",
                       "Command (desired value) input. This is known as setpoint (SP) in"
                       " control theory." ,
                       new OpFunc1<PIDController, double>( &PIDController::setCommand ));
        static DestFinfo sensedIn( "sensedIn",
                       "Sensed parameter - this is the one to be tuned. This is known as"
                       " process variable (PV) in control theory. This comes from the process"
                       " we are trying to control.",
                       new OpFunc1<PIDController, double>( &PIDController::setSensed ));
        static DestFinfo gainDest( "gainDest",
                       "Destination message to control the PIDController gain dynamically.",
                       new OpFunc1<PIDController, double>(&PIDController::setGain));
        static SharedFinfo proc( "proc",
                         "This is a shared message to receive Process messages "
                         "from the scheduler objects."
                         "The first entry in the shared msg is a MsgDest "
                         "for the Process operation. It has a single argument, "
                         "ProcInfo, which holds lots of information about current "
                         "time, thread, dt and so on. The second entry is a MsgDest "
                         "for the Reinit operation. It also uses ProcInfo. ",
                         processShared, sizeof( processShared ) / sizeof( Finfo* )
                         );


    static Finfo* pidFinfos[] = {
		&gain,
		&saturation,
		&command,
		&sensed,
		&tauI,
		&tauD,
		&outputValue,
		&error,
		&integral,
		&derivative,
		&e_previous,
		outputOut(),
		&commandIn,
		&sensedIn,
		&gainDest,
		&proc
    };
    static string doc[] = {
        "Name", "PIDController",
        "Author", "Subhasis Ray",
        "Description", "PID feedback controller."
        "PID stands for Proportional-Integral-Derivative. It is used to"
        " feedback control dynamical systems. It tries to create a feedback"
        " output such that the sensed (measured) parameter is held at command"
        " value. Refer to wikipedia (http://wikipedia.org) for details on PID"
        " Controller." };
    static Dinfo<PIDController> dinfo;
    static Cinfo pidCinfo(
            "PIDController",
            Neutral::initCinfo(),
            pidFinfos,
            sizeof( pidFinfos ) / sizeof( Finfo* ),
            &dinfo,
            doc,
            sizeof(doc)/sizeof(string));
    return &pidCinfo;
}

static const Cinfo* pidCinfo = PIDController::initCinfo();

PIDController::PIDController():
        command_(0),
        saturation_(DBL_MAX),
        gain_(1),
        tau_i_(0),
        tau_d_(0),
        sensed_(0),
        output_(0),
        error_(0),
        e_integral_(0),
        e_derivative_(0),
        e_previous_(0)
{
    ;    // do nothing else
}

void PIDController::setCommand(  double command)
{
    command_ = command;
}

double PIDController::getCommand(  ) const
{
    return command_;
}

void PIDController::setSensed(  double sensed )
{
    sensed_ = sensed;
}

double PIDController::getSensed(  ) const
{
    return sensed_;
}

double PIDController::getOutput(  ) const
{
    return output_;
}

void PIDController::setGain(  double gain )
{
    gain_ = gain;
}

double PIDController::getGain(  ) const
{
    return gain_;
}

void PIDController::setTauI(  double tau_i )
{
    tau_i_ = tau_i;
}

double PIDController::getTauI(  ) const
{
    return tau_i_;
}

void PIDController::setTauD(  double tau_d )
{
    tau_d_ = tau_d;
}

double PIDController::getTauD(  ) const
{
    return tau_d_;
}

void PIDController::setSaturation(  double saturation )
{
    if (saturation <= 0) {
        cout << "Error: PIDController::setSaturation - saturation must be positive." << endl;
    } else {
        saturation_ = saturation;
    }
}

double PIDController::getSaturation(  ) const
{
    return saturation_;
}

double PIDController::getError(  ) const
{
    return error_;
}

double PIDController::getEIntegral(  ) const
{
    return e_integral_;
}

double PIDController::getEDerivative(  ) const
{
    return e_derivative_;
}

double PIDController::getEPrevious(  ) const
{
    return e_previous_;
}

void PIDController::process(const Eref& e,  ProcPtr proc )
{
    double dt = proc->dt;
    e_previous_ = error_;
    error_ = command_ - sensed_;
    e_integral_ += 0.5 * (error_ + e_previous_) * dt;
    e_derivative_ = (error_ - e_previous_) / dt;
    output_ = gain_ * (error_ + e_integral_ / tau_i_ + e_derivative_ * tau_d_);
    if (output_ > saturation_){
        output_ = saturation_;
        e_integral_ -= 0.5 * (error_ + e_previous_) * dt;
    } else if (output_ < -saturation_){
        output_ = -saturation_;
        e_integral_ -= 0.5 * (error_ + e_previous_) * dt;
    }
    outputOut()->send(e, output_);
}


void PIDController::reinit(const Eref& e, ProcPtr proc )
{
    if ( tau_i_ <= 0.0 ){
        tau_i_ = proc->dt;
    }
    if ( tau_d_ < 0.0 ){
        tau_d_ = proc->dt / 4;
    }
    sensed_ = 0.0;
    output_ = 0;
    error_ = 0;
    e_previous_ = error_;
    e_integral_ = 0;
    e_derivative_ = 0;
    outputOut()->send(e, output_);
}



//
// PIDController.cpp ends here
