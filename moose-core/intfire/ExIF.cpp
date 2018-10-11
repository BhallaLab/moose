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

using namespace moose;

const Cinfo* ExIF::initCinfo()
{
	static string doc[] =
	{
		"Name", "ExIF",
		"Author", "Aditya Gilra",
		"Description", "Leaky Integrate-and-Fire neuron with Exponential spike rise."
        "Rm*Cm dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I"
	};

    static ElementValueFinfo< ExIF, double > deltaThresh(
        "deltaThresh",
        "Parameter in Vm evolution equation:"
        "Rm*Cm * dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I",
        &ExIF::setDeltaThresh,
        &ExIF::getDeltaThresh
    );

    static ElementValueFinfo< ExIF, double > vPeak(
        "vPeak",
        "Vm is reset on reaching vPeak, different from spike thresh below:"
        "Rm*Cm dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I",
        &ExIF::setVPeak,
        &ExIF::getVPeak
    );

	static Finfo* ExIFFinfos[] = {
        &deltaThresh,       // Value
        &vPeak,             // Value
	};

    static Dinfo< ExIF > dinfo;
	static Cinfo ExIFCinfo(
				"ExIF",
				IntFireBase::initCinfo(),
                ExIFFinfos,
				sizeof( ExIFFinfos ) / sizeof (Finfo*),
				&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &ExIFCinfo;
}

static const Cinfo* ExIFCinfo = ExIF::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the Compartment class functions.
//////////////////////////////////////////////////////////////////

ExIF::ExIF()
{vPeak_ = 0.0;
deltaThresh_ = 0.0;}

ExIF::~ExIF()
{;}

//////////////////////////////////////////////////////////////////
// ExIF::Dest function definitions.
//////////////////////////////////////////////////////////////////

void ExIF::vProcess( const Eref& e, ProcPtr p )
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
			lastEvent_ = p->currTime;
			fired_ = true;
			spikeOut()->send( e, p->currTime );
			VmOut()->send( e, Vm_ );
		} else {
            Vm_ += deltaThresh_ * exp((Vm_-threshold_)/deltaThresh_) *p->dt/Rm_/Cm_;
			Compartment::vProcess( e, p );  // this sends out Vm message also,
                                            // so do calculations before
		}
	}
}

void ExIF::vReinit(  const Eref& e, ProcPtr p )
{
	activation_ = 0.0;
	fired_ = false;
	lastEvent_ = -refractT_; // Allow it to fire right away.
	Compartment::vReinit( e, p );
}

void ExIF::setDeltaThresh( const Eref& e, double val )
{
	deltaThresh_ = val;
}

double ExIF::getDeltaThresh( const Eref& e ) const
{
	return deltaThresh_;
}

void ExIF::setVPeak( const Eref& e, double val )
{
	vPeak_ = val;
}

double ExIF::getVPeak( const Eref& e ) const
{
	return vPeak_;
}
