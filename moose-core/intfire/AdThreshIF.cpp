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
#include "AdThreshIF.h"

using namespace moose;

const Cinfo* AdThreshIF::initCinfo()
{
	static string doc[] =
	{
		"Name", "AdThreshIF",
		"Author", "Aditya Gilra",
		"Description", "Leaky Integrate-and-Fire neuron with adaptive threshold."
        "Based on Rossant, C., Goodman, D.F.M., Platkiewicz, J., and Brette, R. (2010)."
        "Rm*Cm * dVm/dt = -(Vm-Em) + Rm*I"
        "tauThresh * d threshAdaptive / dt = a0*(Vm-Em) - threshAdaptive "
        "at each spike, threshAdaptive is increased by threshJump "
        "the spiking threshold adapts as thresh + threshAdaptive "
	};

    static ElementValueFinfo< AdThreshIF, double > threshAdaptive(
        "threshAdaptive",
        "adaptative part of the threshold that decays with time constant tauThresh",
        &AdThreshIF::setThreshAdaptive,
        &AdThreshIF::getThreshAdaptive
    );

    static ElementValueFinfo< AdThreshIF, double > tauThresh(
        "tauThresh",
        "time constant of adaptative part of the threshold",
        &AdThreshIF::setTauThresh,
        &AdThreshIF::getTauThresh
    );

    static ElementValueFinfo< AdThreshIF, double > a0(
        "a0",
        "factor for voltage-dependent term in evolution of adaptative threshold: "
        "tauThresh * d threshAdaptive / dt = a0*(Vm-Em) - threshAdaptive ",
        &AdThreshIF::setA0,
        &AdThreshIF::getA0
    );

    static ElementValueFinfo< AdThreshIF, double > threshJump(
        "threshJump",
        "threshJump is added to threshAdaptive on each spike",
        &AdThreshIF::setThreshJump,
        &AdThreshIF::getThreshJump
    );

	static Finfo* AdThreshIFFinfos[] = {
        &threshAdaptive,// Value
        &tauThresh,     // Value
        &a0,            // Value
        &threshJump     // Value
	};

    static Dinfo< AdThreshIF > dinfo;
	static Cinfo AdThreshIFCinfo(
				"AdThreshIF",
				IntFireBase::initCinfo(),
                AdThreshIFFinfos,
				sizeof( AdThreshIFFinfos ) / sizeof (Finfo*),
				&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &AdThreshIFCinfo;
}

static const Cinfo* AdThreshIFCinfo = AdThreshIF::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the Compartment class functions.
//////////////////////////////////////////////////////////////////

AdThreshIF::AdThreshIF()
{threshAdaptive_ = 0.0;
tauThresh_ = 1.0;
a0_ = 0.0;
threshJump_ = 0.0;}

AdThreshIF::~AdThreshIF()
{;}

//////////////////////////////////////////////////////////////////
// AdThreshIF::Dest function definitions.
//////////////////////////////////////////////////////////////////

void AdThreshIF::vProcess( const Eref& e, ProcPtr p )
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
		if ( Vm_ > (threshold_+threshAdaptive_) ) {
			Vm_ = vReset_;
            threshAdaptive_ += threshJump_;
			lastEvent_ = p->currTime;
			fired_ = true;
			spikeOut()->send( e, p->currTime );
			VmOut()->send( e, Vm_ );
		} else {
            threshAdaptive_ += (-threshAdaptive_ + a0_*(Vm_-Em_)) * p->dt/tauThresh_;
			Compartment::vProcess( e, p );  // this sends out Vm message also,
                                            // so do Vm_ calculations before
		}
	}
}

void AdThreshIF::vReinit(  const Eref& e, ProcPtr p )
{
	activation_ = 0.0;
    threshAdaptive_ = 0.0;
	fired_ = false;
	lastEvent_ = -refractT_; // Allow it to fire right away.
	Compartment::vReinit( e, p );
}

void AdThreshIF::setThreshAdaptive( const Eref& e, double val )
{
	threshAdaptive_ = val;
}

double AdThreshIF::getThreshAdaptive( const Eref& e ) const
{
	return threshAdaptive_;
}

void AdThreshIF::setTauThresh( const Eref& e, double val )
{
	tauThresh_ = val;
}

double AdThreshIF::getTauThresh( const Eref& e ) const
{
	return tauThresh_;
}

void AdThreshIF::setA0( const Eref& e, double val )
{
	a0_ = val;
}

double AdThreshIF::getA0( const Eref& e ) const
{
	return a0_;
}

void AdThreshIF::setThreshJump( const Eref& e, double val )
{
	threshJump_ = val;
}

double AdThreshIF::getThreshJump( const Eref& e ) const
{
	return threshJump_;
}
