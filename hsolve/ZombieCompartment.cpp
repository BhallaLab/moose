/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "ZombieCompartment.h"

using namespace moose;
const double ZombieCompartment::EPSILON = 1.0e-15;


/**
 * The initCinfo() function sets up the Compartment class.
 * This function uses the common trick of having an internal
 * static value which is created the first time the function is called.
 * There are several static arrays set up here. The ones which
 * use SharedFinfos are for shared messages where multiple kinds
 * of information go along the same connection.
 */
const Cinfo* ZombieCompartment::initCinfo()
{
    ///////////////////////////////////////////////////////////////////
    // static Finfo* compartmentFinfos[] = { };

    static string doc[] =
    {
        "Name", "ZombieCompartment",
        "Author", "Upi Bhalla",
        "Description", "Compartment object, for branching neuron models.",
    };

   	static Dinfo< ZombieCompartment > dinfo;
    static Cinfo zombieCompartmentCinfo(
        "ZombieCompartment",
        CompartmentBase::initCinfo(),
		0,
		0,
        // compartmentFinfos,
        // sizeof( compartmentFinfos ) / sizeof( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc )/ sizeof( string )
    );

    return &zombieCompartmentCinfo;
}

static const Cinfo* zombieCompartmentCinfo = ZombieCompartment::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the Compartment class functions.
//////////////////////////////////////////////////////////////////

ZombieCompartment::ZombieCompartment()
{
    hsolve_   = NULL;
}

ZombieCompartment::~ZombieCompartment()
{
    ;
}

// Value Field access function definitions.
void ZombieCompartment::vSetVm( const Eref& e , double Vm )
{
    assert(hsolve_);
    hsolve_->setVm( e.id(), Vm );
}

double ZombieCompartment::vGetVm( const Eref& e  ) const
{
    assert(hsolve_);
    return hsolve_->getVm( e.id() );
}

void ZombieCompartment::vSetEm( const Eref& e , double Em )
{
    hsolve_->setEm( e.id(), Em );
}

double ZombieCompartment::vGetEm( const Eref& e  ) const
{
    return hsolve_->getEm( e.id() );
}

void ZombieCompartment::vSetCm( const Eref& e , double Cm )
{
    if ( rangeWarning( "Cm", Cm ) ) return;
    hsolve_->setCm( e.id(), Cm );
}

double ZombieCompartment::vGetCm( const Eref& e  ) const
{
    return hsolve_->getCm( e.id() );
}

void ZombieCompartment::vSetRm( const Eref& e , double Rm )
{
    if ( rangeWarning( "Rm", Rm ) ) return;
    hsolve_->setRm( e.id(), Rm );
}

double ZombieCompartment::vGetRm( const Eref& e  ) const
{
    return hsolve_->getRm( e.id() );
}

void ZombieCompartment::vSetRa( const Eref& e , double Ra )
{
    if ( rangeWarning( "Ra", Ra ) ) return;
    hsolve_->setRa( e.id(), Ra );
}

double ZombieCompartment::vGetRa( const Eref& e  ) const
{
    return hsolve_->getRa( e.id() );
}

//~ void ZombieCompartment::setIm( const Eref& e , double Im )
//~ {
//~ Im_ = Im;
//~ }

double ZombieCompartment::vGetIm( const Eref& e  ) const
{
    return hsolve_->getIm( e.id() );
}

void ZombieCompartment::vSetInject( const Eref& e , double Inject )
{
    hsolve_->setInject( e.id(), Inject );
}

double ZombieCompartment::vGetInject( const Eref& e  ) const
{
    return hsolve_->getInject( e.id() );
}

void ZombieCompartment::vSetInitVm( const Eref& e , double initVm )
{
    hsolve_->setInitVm( e.id(), initVm );
}

double ZombieCompartment::vGetInitVm( const Eref& e  ) const
{
    return hsolve_->getInitVm( e.id() );
}

//////////////////////////////////////////////////////////////////
// ZombieCompartment::Dest function definitions.
//////////////////////////////////////////////////////////////////
//
void ZombieCompartment::vProcess( const Eref& e, ProcPtr p )
{
    ;
}

void ZombieCompartment::vReinit(  const Eref& e, ProcPtr p )
{
    rng.setSeed( moose::__rng_seed__ );
}

void ZombieCompartment::vInitProc( const Eref& e, ProcPtr p )
{
    ;
}

void ZombieCompartment::vInitReinit( const Eref& e, ProcPtr p )
{
    ;
}

void ZombieCompartment::vHandleChannel( const Eref& e, double Gk, double Ek)
{
    hsolve_->addGkEk( e.id(), Gk, Ek );
}

void ZombieCompartment::vHandleRaxial( double Ra, double Vm )
{
    ;
}

void ZombieCompartment::vHandleAxial( double Vm )
{
    ;
}

void ZombieCompartment::vInjectMsg( const Eref& e , double current )
{
    hsolve_->addInject( e.id(), current );
}

void ZombieCompartment::vRandInject( const Eref& e , double prob, double current )
{
    //~ if ( mtrand() < prob * dt_ ) {
    //~ hsolve_->addInject( e.id(), current );
    //~ }
}
//////////////////////////////////////////////////////////////////

void ZombieCompartment::vSetSolver( const Eref& e , Id hsolve )
{
	if ( !hsolve.element()->cinfo()->isA( "HSolve" ) ) {
		cout << "Error: ZombieCompartment::vSetSolver: Object: " <<
				hsolve.path() << " is not an HSolve. Aborted\n";
		hsolve_ = 0;
		return;
	}
	hsolve_ = reinterpret_cast< HSolve* >( hsolve.eref().data() );
}

double ZombieCompartment::mtrand( void )
{
    return rng.uniform( );
}
