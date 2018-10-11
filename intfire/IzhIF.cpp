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
#include "IzhIF.h"

using namespace moose;

const Cinfo* IzhIF::initCinfo()
{
	static string doc[] =
	{
		"Name", "IzhIF",
		"Author", "Aditya Gilra",
		"Description", "Izhikevich neuron (integrate and fire)."
        "d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm "
        "d u / dt = a * ( b * Vm - u ) "
        "at each spike, u -> u + d "
        "by default, a0 = 0.04e6/V/s, b0 = 5e3/s, c0 = 140 V/s are set to SI units, "
        "so use SI consistently, or change a0, b0, c0 also if you wish to use other units. "
        "Rm, Em from Compartment are not used here, vReset is same as c in the usual formalism. "
        "At rest, u0 = b V0, and V0 = ( -(-b0-b) +/- sqrt((b0-b)^2 - 4*a0*c0)) / (2*a0) "
        "equivalently, to obtain resting Em, set b = (a0*Em^2 + b0*Em + c0)/Em"
	};

    static ElementValueFinfo< IzhIF, double > a0(
        "a0",
        "factor for Vm^2 term in evolution equation for Vm: "
        "d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm ",
        &IzhIF::setA0,
        &IzhIF::getA0
    );

    static ElementValueFinfo< IzhIF, double > b0(
        "b0",
        "factor for Vm term in evolution equation for Vm: "
        "d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm ",
        &IzhIF::setB0,
        &IzhIF::getB0
    );

    static ElementValueFinfo< IzhIF, double > c0(
        "c0",
        "constant term in evolution equation for Vm: "
        "d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm ",
        &IzhIF::setC0,
        &IzhIF::getC0
    );

    static ElementValueFinfo< IzhIF, double > a(
        "a",
        "a as in d u / dt = a * ( b * Vm - u ) ",
        &IzhIF::setA,
        &IzhIF::getA
    );

    static ElementValueFinfo< IzhIF, double > b(
        "b",
        "b as in d u / dt = a * ( b * Vm - u ) ",
        &IzhIF::setB,
        &IzhIF::getB
    );

    static ElementValueFinfo< IzhIF, double > d(
        "d",
        "u jumps by d at every spike",
        &IzhIF::setD,
        &IzhIF::getD
    );

    static ElementValueFinfo< IzhIF, double > u(
        "u",
        "u is an adaptation variable",
        &IzhIF::setU,
        &IzhIF::getU
    );

    static ElementValueFinfo< IzhIF, double > vPeak(
        "vPeak",
        "Vm is reset when Vm > vPeak",
        &IzhIF::setVPeak,
        &IzhIF::getVPeak
    );

    static ElementValueFinfo< IzhIF, double > uInit(
        "uInit",
        "Initial value of u. It is reset at reinit()",
        &IzhIF::setUInit,
        &IzhIF::getUInit
    );

	static Finfo* IzhIFFinfos[] = {
        &a0,            // Value
        &b0,            // Value
        &c0,            // Value
        &a,             // Value
        &b,             // Value
        &d,             // Value
        &u,             // Value
        &uInit,         // Value
        &vPeak,         // Value
	};

    static Dinfo< IzhIF > dinfo;
	static Cinfo IzhIFCinfo(
				"IzhIF",
				IntFireBase::initCinfo(),
                IzhIFFinfos,
				sizeof( IzhIFFinfos ) / sizeof (Finfo*),
				&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &IzhIFCinfo;
}

static const Cinfo* IzhIFCinfo = IzhIF::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the Compartment class functions.
//////////////////////////////////////////////////////////////////

IzhIF::IzhIF()
{
// by default, a0 = 0.04e6/V/s, b0 = 5e3/s, c0 = 140 V/s are set to SI units,
// so use SI consistently,
// or change a0, b0, c0 via python if you wish to use other units.
a0_ = 0.04e6;   // V^-1 s^-1
b0_ = 5e3;      // s^-1
c0_ = 140;      // V/s
a_ = 0.0;
b_ = 0.0;
d_ = 0.0;
uInit_ = 0.0;
u_ = 0.0;
vPeak_ = 0.0;
}

IzhIF::~IzhIF()
{;}

//////////////////////////////////////////////////////////////////
// IzhIF::Dest function definitions.
//////////////////////////////////////////////////////////////////

void IzhIF::vProcess( const Eref& e, ProcPtr p )
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
		if ( Vm_ > vPeak_ ) {
			Vm_ = vReset_;
            u_ += d_;
			lastEvent_ = p->currTime;
			fired_ = true;
			spikeOut()->send( e, p->currTime );
			VmOut()->send( e, Vm_ );
		} else {
            Vm_ += ( (inject_+sumInject_) / Cm_
                    + a0_*pow(Vm_,2.0) + b0_*Vm_ + c0_ - u_ ) * p->dt;
            u_ += a_ * (b_*Vm_ - u_) * p->dt;
            lastIm_ = Im_;
            Im_ = 0.0;
            sumInject_ = 0.0;
            // Send out Vm to channels, SpikeGens, etc.
            VmOut()->send( e, Vm_ );
		}
	}
}

void IzhIF::vReinit(  const Eref& e, ProcPtr p )
{
	activation_ = 0.0;
    u_ = uInit_;
	fired_ = false;
	lastEvent_ = -refractT_; // Allow it to fire right away.
	Compartment::vReinit( e, p );
}

void IzhIF::setA0( const Eref& e, double val )
{
	a0_ = val;
}

double IzhIF::getA0( const Eref& e ) const
{
	return a0_;
}

void IzhIF::setB0( const Eref& e, double val )
{
	b0_ = val;
}

double IzhIF::getB0( const Eref& e ) const
{
	return b0_;
}

void IzhIF::setC0( const Eref& e, double val )
{
	c0_ = val;
}

double IzhIF::getC0( const Eref& e ) const
{
	return c0_;
}

void IzhIF::setA( const Eref& e, double val )
{
	a_ = val;
}

double IzhIF::getA( const Eref& e ) const
{
	return a_;
}

void IzhIF::setB( const Eref& e, double val )
{
	b_ = val;
}

double IzhIF::getB( const Eref& e ) const
{
	return b_;
}

void IzhIF::setD( const Eref& e, double val )
{
	d_ = val;
}

double IzhIF::getD( const Eref& e ) const
{
	return d_;
}

void IzhIF::setVPeak( const Eref& e, double val )
{
	vPeak_ = val;
}

double IzhIF::getVPeak( const Eref& e ) const
{
	return vPeak_;
}

void IzhIF::setU( const Eref& e, double val )
{
	u_ = val;
}

double IzhIF::getU( const Eref& e ) const
{
	return u_;
}

void IzhIF::setUInit( const Eref& e, double val )
{
	uInit_ = val;
}

double IzhIF::getUInit( const Eref& e ) const
{
	return uInit_;
}
