/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../randnum/randnum.h"
#include "CompartmentBase.h"
#include "Compartment.h"

using namespace moose;
const double Compartment::EPSILON = 1.0e-15;

/**
 * The initCinfo() function sets up the Compartment class.
 * This function uses the common trick of having an internal
 * static value which is created the first time the function is called.
 * There are several static arrays set up here. The ones which
 * use SharedFinfos are for shared messages where multiple kinds
 * of information go along the same connection.
 */
const Cinfo* Compartment::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// static Finfo* compartmentFinfos[] = { };

	static string doc[] =
	{
		"Name", "Compartment",
		"Author", "Upi Bhalla",
		"Description", "Compartment object, for branching neuron models.",
	};
    static Dinfo< Compartment > dinfo;
	static Cinfo compartmentCinfo(
				"Compartment",
				CompartmentBase::initCinfo(),
				0,
				0,
				// compartmentFinfos,
				// sizeof( compartmentFinfos ) / sizeof( Finfo* ),
				&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &compartmentCinfo;
}

static const Cinfo* compartmentCinfo = Compartment::initCinfo();


/*
const SrcFinfo1< double >* VmOut =
	dynamic_cast< const SrcFinfo1< double >* >(
			compartmentCinfo->findFinfo( "VmOut" ) );
			*/

const SrcFinfo1< double >* axialOut =
	dynamic_cast< const SrcFinfo1< double >* > (
			compartmentCinfo->findFinfo( "axialOut" ) );

const SrcFinfo2< double, double >* raxialOut =
	dynamic_cast< const SrcFinfo2< double, double >* > (
			compartmentCinfo->findFinfo( "raxialOut" ) );

//////////////////////////////////////////////////////////////////
// Here we put the Compartment class functions.
//////////////////////////////////////////////////////////////////

Compartment::Compartment()
{
	Vm_ = -0.06;
	Em_ = -0.06;
	Cm_ = 1.0;
	Rm_ = 1.0;
	invRm_ = 1.0;
	Ra_ = 1.0;
	Im_ = 0.0;
    lastIm_ = 0.0;
	inject_ = 0.0;
	sumInject_ = 0.0;
	initVm_ = -0.06;
	A_ = 0.0;
	B_ = 0.0;
}

Compartment::~Compartment()
{
	;
}

// Value Field access function definitions.
void Compartment::vSetVm( const Eref& e, double Vm )
{
	Vm_ = Vm;
}

double Compartment::vGetVm( const Eref& e ) const
{
	return Vm_;
}

void Compartment::vSetEm( const Eref& e, double Em )
{
	Em_ = Em;
}

double Compartment::vGetEm( const Eref& e ) const
{
	return Em_;
}

void Compartment::vSetCm( const Eref& e, double Cm )
{
	if ( rangeWarning( "Cm", Cm ) ) return;
	Cm_ = Cm;
}

double Compartment::vGetCm( const Eref& e ) const
{
	return Cm_;
}

void Compartment::vSetRm( const Eref& e, double Rm )
{
	if ( rangeWarning( "Rm", Rm ) ) return;
	Rm_ = Rm;
	invRm_ = 1.0/Rm;
}

double Compartment::vGetRm( const Eref& e ) const
{
	return Rm_;
}

void Compartment::vSetRa( const Eref& e, double Ra )
{
	if ( rangeWarning( "Ra", Ra ) ) return;
	Ra_ = Ra;
}

double Compartment::vGetRa( const Eref& e ) const
{
	return Ra_;
}

double Compartment::vGetIm( const Eref& e ) const
{
	return lastIm_;
}

void Compartment::vSetInject( const Eref& e, double inject )
{
	inject_ = inject;
}

double Compartment::vGetInject( const Eref& e ) const
{
	return inject_;
}

void Compartment::vSetInitVm( const Eref& e, double initVm )
{
	initVm_ = initVm;
}

double Compartment::vGetInitVm( const Eref& e ) const
{
	return initVm_;
}


//////////////////////////////////////////////////////////////////
// Compartment::Dest function definitions.
//////////////////////////////////////////////////////////////////

void Compartment::vProcess( const Eref& e, ProcPtr p )
{
        //cout << "Compartment " << e.id().path() << ":: process: A = " << A_ << ", B = " << B_ << endl;
	A_ += inject_ + sumInject_ + Em_ * invRm_;
	if ( B_ > EPSILON ) {
		double x = exp( -B_ * p->dt / Cm_ );
		Vm_ = Vm_ * x + ( A_ / B_ )  * ( 1.0 - x );
	} else {
		Vm_ += ( A_ - Vm_ * B_ ) * p->dt / Cm_;
	}
	A_ = 0.0;
	B_ = invRm_;
        lastIm_ = Im_;
	Im_ = 0.0;
	sumInject_ = 0.0;
	// Send out Vm to channels, SpikeGens, etc.
	VmOut()->send( e, Vm_ );

	// The axial/raxial messages go out in the 'init' phase.
}

void Compartment::vReinit(  const Eref& e, ProcPtr p )
{
	Vm_ = initVm_;
	A_ = 0.0;
	B_ = invRm_;
	Im_ = 0.0;
        lastIm_ = 0.0;
	sumInject_ = 0.0;
	dt_ = p->dt;

	// Send out the resting Vm to channels, SpikeGens, etc.
	VmOut()->send( e, Vm_ );
}

void Compartment::vInitProc( const Eref& e, ProcPtr p )
{
	// Send out the axial messages
	axialOut->send( e, Vm_ );
	// Send out the raxial messages
	raxialOut->send( e, Ra_, Vm_ );
}

void Compartment::vInitReinit( const Eref& e, ProcPtr p )
{
	; // Nothing happens here
}

void Compartment::vHandleChannel( const Eref& e, double Gk, double Ek)
{
	A_ += Gk * Ek;
	B_ += Gk;
}

void Compartment::vHandleRaxial( double Ra, double Vm)
{
	A_ += Vm / Ra;
	B_ += 1.0 / Ra;
	Im_ += ( Vm - Vm_ ) / Ra;
}

void Compartment::vHandleAxial( double Vm)
{
	A_ += Vm / Ra_;
	B_ += 1.0 / Ra_;
	Im_ += ( Vm - Vm_ ) / Ra_;
}

void Compartment::vInjectMsg( const Eref& e, double current)
{
	sumInject_ += current;
	Im_ += current;
}

void Compartment::vRandInject( const Eref& e, double prob, double current)
{
	if ( mtrand() < prob * dt_ ) {
		sumInject_ += current;
		Im_ += current;
	}
}

/////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS

#include "header.h"
#include "../shell/Shell.h"
//#include "../randnum/randnum.h"

void testCompartment()
{
	unsigned int size = 1;
	Eref sheller( Id().eref() );
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
        Id comptId = shell->doCreate("Compartment", Id(), "compt", size);
	assert( Id::isValid(comptId));
	Eref compter = comptId.eref();
	Compartment* c = reinterpret_cast< Compartment* >( comptId.eref().data() );
	ProcInfo p;
	p.dt = 0.002;
	c->setInject( compter, 1.0 );
	c->setRm( compter, 1.0 );
	c->setRa( compter, 0.0025 );
	c->setCm( compter, 1.0 );
	c->setEm( compter, 0.0 );
	c->setVm( compter, 0.0 );

	// First, test charging curve for a single compartment
	// We want our charging curve to be a nice simple exponential
	// Vm = 1.0 - 1.0 * exp( - t / 1.0 );
	double delta = 0.0;
	double Vm = 0.0;
	double tau = 1.0;
	double Vmax = 1.0;
	for ( p.currTime = 0.0; p.currTime < 2.0; p.currTime += p.dt )
	{
		Vm = c->getVm( compter );
		double x = Vmax - Vmax * exp( -p.currTime / tau );
		delta += ( Vm - x ) * ( Vm - x );
		c->process( compter, &p );
	}
	assert( delta < 1.0e-6 );
        shell->doDelete(comptId);
	cout << "." << flush;
}

// Comment out this define if it takes too long (about 5 seconds on
// a modest machine, but could be much longer with valgrind)
#define DO_SPATIAL_TESTS
/**
 * Function to test the spatial spread of charge.
 * We make the cable long enough to get another nice exponential.
 * Vm = Vm0 * exp( -x/lambda)
 * lambda = sqrt( Rm/Ra ) where these are the actual values, not
 * the values per unit length.
 * So here lambda = 20, so that each compt is lambda/20
 */
#include "../shell/Shell.h"
void testCompartmentProcess()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	unsigned int size = 100;
	double Rm = 1.0;
	double Ra = 0.01;
	double Cm = 1.0;
	double dt = 0.01;
	double runtime = 10;
	double lambda = sqrt( Rm / Ra );

	Id cid = shell->doCreate( "Compartment", Id(), "compt", size );
	assert( Id::isValid(cid));
	assert( cid.eref().element()->numData() == size );

	bool ret = Field< double >::setRepeat( cid, "initVm", 0.0 );
	assert( ret );
	Field< double >::setRepeat( cid, "inject", 0 );
	// Only apply current injection in first compartment
	Field< double >::set( ObjId( cid, 0 ), "inject", 1.0 );
	Field< double >::setRepeat( cid, "Rm", Rm );
	Field< double >::setRepeat( cid, "Ra", Ra );
	Field< double >::setRepeat( cid, "Cm", Cm );
	Field< double >::setRepeat( cid, "Em", 0 );
	Field< double >::setRepeat( cid, "Vm", 0 );

	// The diagonal message has a default stride of 1, so it connects
	// successive compartments.
	// Note that the src and dest elements here are identical, so we cannot
	// use a shared message. The messaging system will get confused about
	// direction to send data. So we split up the shared message that we
	// might have used, below, into two individual messages.
	// MsgId mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "raxial", ObjId( cid ), "axial" );
	ObjId mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "axialOut", ObjId( cid ), "handleAxial" );
	assert( !mid.bad());
	// mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "handleRaxial", ObjId( cid ), "raxialOut" );
	mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "raxialOut", ObjId( cid ), "handleRaxial" );
	assert( !mid.bad() );
	// ObjId managerId = Msg::getMsg( mid )->manager().objId();
	// Make the raxial data go from high to lower index compartments.
	Field< int >::set( mid, "stride", -1 );

#ifdef DO_SPATIAL_TESTS
	shell->doSetClock( 0, dt );
	shell->doSetClock( 1, dt );
	// Ensure that the inter_compt msgs go between nodes once every dt.
	shell->doSetClock( 9, dt );

	shell->doUseClock( "/compt", "init", 0 );
	shell->doUseClock( "/compt", "process", 1 );

	shell->doReinit();
	shell->doStart( runtime );

	double Vmax = Field< double >::get( ObjId( cid, 0 ), "Vm" );

	double delta = 0.0;
	// We measure only the first 50 compartments as later we
	// run into end effects because it is not an infinite cable
	for ( unsigned int i = 0; i < size; i++ ) {
		double Vm = Field< double >::get( ObjId( cid, i ), "Vm" );
		double x = Vmax * exp( - static_cast< double >( i ) / lambda );
		delta += ( Vm - x ) * ( Vm - x );
	 	// cout << i << " (x, Vm) = ( " << x << ", " << Vm << " )\n";
	}
	assert( delta < 1.0e-5 );
#endif // DO_SPATIAL_TESTS
	shell->doDelete( cid );
	cout << "." << flush;
}
#endif // DO_UNIT_TESTS
