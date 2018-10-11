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
#include "QIF.h"

using namespace moose;

const Cinfo* QIF::initCinfo()
{
	static string doc[] =
	{
		"Name", "QIF",
		"Author", "Aditya Gilra",
		"Description", "Leaky Integrate-and-Fire neuron with Quadratic term in Vm."
        "Based on Spiking Neuron Models book by Gerstner and Kistler."
        "Rm*Cm * dVm/dt = a0*(Vm-Em)*(Vm-vCritical) + Rm*I"
	};

    static ElementValueFinfo< QIF, double > vCritical(
        "vCritical",
        "Critical voltage for spike initiation",
        &QIF::setVCritical,
        &QIF::getVCritical
    );


    static ElementValueFinfo< QIF, double > a0(
        "a0",
        "Parameter in Rm*Cm dVm/dt = a0*(Vm-Em)*(Vm-vCritical) + Rm*I, a0>0",
        &QIF::setA0,
        &QIF::getA0
    );

	static Finfo* QIFFinfos[] = {
		&vCritical,		// Value
        &a0             // Value
	};

    static Dinfo< QIF > dinfo;
	static Cinfo QIFCinfo(
				"QIF",
				IntFireBase::initCinfo(), // this is the initCinfo of the parent class
                QIFFinfos,
				sizeof( QIFFinfos ) / sizeof (Finfo*),
				&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &QIFCinfo;
}

static const Cinfo* QIFCinfo = QIF::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the QIF class functions.
//////////////////////////////////////////////////////////////////

QIF::QIF()
{a0_ = 0.0;
vCritical_ = 0.0;}

QIF::~QIF()
{;}

//////////////////////////////////////////////////////////////////
// QIF::Dest function definitions.
//////////////////////////////////////////////////////////////////

void QIF::vProcess( const Eref& e, ProcPtr p )
{
    // fully taking over Compartment's vProcess due to quadratic term in Vm
    // we no longer care about A and B
	fired_ = false;
	if ( p->currTime < lastEvent_ + refractT_ ) {
		Vm_ = vReset_;
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
		if ( Vm_ > threshold_ ) {
			Vm_ = vReset_;
			lastEvent_ = p->currTime;
			fired_ = true;
			spikeOut()->send( e, p->currTime );
			VmOut()->send( e, Vm_ );
		} else {
            Vm_ += ( (inject_+sumInject_)
                    + a0_*(Vm_-Em_)*(Vm_-vCritical_)/Rm_ ) * p->dt / Cm_;
            lastIm_ = Im_;
            Im_ = 0.0;
            sumInject_ = 0.0;
            // Send out Vm to channels, SpikeGens, etc.
            VmOut()->send( e, Vm_ );
		}
	}
}

void QIF::vReinit(  const Eref& e, ProcPtr p )
{
	activation_ = 0.0;
	fired_ = false;
	lastEvent_ = -refractT_; // Allow it to fire right away.
	Compartment::vReinit( e, p );
}

void QIF::setVCritical( const Eref& e, double val )
{
	vCritical_ = val;
}

double QIF::getVCritical( const Eref& e ) const
{
	return vCritical_;
}

void QIF::setA0( const Eref& e, double val )
{
	a0_ = val;
}

double QIF::getA0( const Eref& e ) const
{
	return a0_;
}

