/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "../biophysics/CompartmentBase.h"
#include "../biophysics/Compartment.h"
#include "IntFireBase.h"
#include "ExIF.h"
#include "AdExIF.h"

using namespace moose;

const Cinfo* AdExIF::initCinfo()
{
	static string doc[] =
	{
		"Name", "AdExIF",
		"Author", "Aditya Gilra",
		"Description", "Leaky Integrate-and-Fire neuron with Exponential spike rise"
        " and adaptation via an adapting current w."
        "Rm*Cm * dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I - w "
        "tau_w * d w /dt = a0*(Vm-Em) - w "
        "at each spike, w -> w + b0 "
	};

    static ElementValueFinfo< AdExIF, double > w(
        "w",
        "adaptation current with time constant tauW",
        &AdExIF::setW,
        &AdExIF::getW
    );

    static ElementValueFinfo< AdExIF, double > tauW(
        "tauW",
        "time constant of adaptation current w",
        &AdExIF::setTauW,
        &AdExIF::getTauW
    );

    static ElementValueFinfo< AdExIF, double > a0(
        "a0",
        "factor for voltage-dependent term in evolution of adaptation current: "
        "tau_w dw/dt = a0*(Vm-Em) - w",
        &AdExIF::setA0,
        &AdExIF::getA0
    );

    static ElementValueFinfo< AdExIF, double > b0(
        "b0",
        "b0 is added to w, the adaptation current on each spike",
        &AdExIF::setB0,
        &AdExIF::getB0
    );

	static Finfo* AdExIFFinfos[] = {
        &w,        // Value
        &tauW,     // Value
        &a0,       // Value
        &b0,       // Value
	};

    static Dinfo< AdExIF > dinfo;
	static Cinfo AdExIFCinfo(
				"AdExIF",
				ExIF::initCinfo(),
                AdExIFFinfos,
				sizeof( AdExIFFinfos ) / sizeof (Finfo*),
				&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &AdExIFCinfo;
}

static const Cinfo* AdExIFCinfo = AdExIF::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the Compartment class functions.
//////////////////////////////////////////////////////////////////

AdExIF::AdExIF()
{w_ = 0.0;
tauW_ = 1.0;
a0_ = 0.0;
b0_ = 0.0;}

AdExIF::~AdExIF()
{;}

//////////////////////////////////////////////////////////////////
// AdExIF::Dest function definitions.
//////////////////////////////////////////////////////////////////

void AdExIF::vProcess( const Eref& e, ProcPtr p )
{
	fired_ = false;
	if ( p->currTime < lastEvent_ + refractT_ ) {
		Vm_ = vReset_;
		A_ = 0.0;
		B_ = 1.0 / Rm_;
		sumInject_ = 0.0;
		VmOut()->send( e, Vm_ );
	} else {
        // activation can be a continous variable (graded synapse).
        // So integrate it at every time step, thus *dt.
        // For a delta-fn synapse, SynHandler-s divide by dt and send activation.
        // See: http://www.genesis-sim.org/GENESIS/Hyperdoc/Manual-26.html#synchan
        //          for this continuous definition of activation.
		Vm_ += activation_ * p->dt;
		activation_ = 0.0;
		if ( Vm_ >= vPeak_ ) {
			Vm_ = vReset_;
            w_ += b0_;
			lastEvent_ = p->currTime;
			fired_ = true;
			spikeOut()->send( e, p->currTime );
			VmOut()->send( e, Vm_ );
		} else {
            Vm_ += ( deltaThresh_ * exp((Vm_-threshold_)/deltaThresh_) - Rm_*w_ )
                            *p->dt/Rm_/Cm_;
            w_ += (-w_ + a0_*(Vm_-Em_)) * p->dt/tauW_;
			Compartment::vProcess( e, p );  // this sends out Vm message also,
                                            // so do calculations before
		}
	}
}

void AdExIF::vReinit(  const Eref& e, ProcPtr p )
{
	activation_ = 0.0;
    w_ = 0.0;
	fired_ = false;
	lastEvent_ = -refractT_; // Allow it to fire right away.
	Compartment::vReinit( e, p );
}

void AdExIF::setW( const Eref& e, double val )
{
	w_ = val;
}

double AdExIF::getW( const Eref& e ) const
{
	return w_;
}

void AdExIF::setTauW( const Eref& e, double val )
{
	tauW_ = val;
}

double AdExIF::getTauW( const Eref& e ) const
{
	return tauW_;
}

void AdExIF::setA0( const Eref& e, double val )
{
	a0_ = val;
}

double AdExIF::getA0( const Eref& e ) const
{
	return a0_;
}

void AdExIF::setB0( const Eref& e, double val )
{
	b0_ = val;
}

double AdExIF::getB0( const Eref& e ) const
{
	return b0_;
}
