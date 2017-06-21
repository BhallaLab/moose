/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**		   Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "ZombieHHChannel.h"

const Cinfo* ZombieHHChannel::initCinfo()
{
    static string doc[] =
    {
        "Name", "ZombieHHChannel",
        "Author", "Upinder S. Bhalla, 2007, 2014 NCBS",
        "Description", "ZombieHHChannel: Hodgkin-Huxley type voltage-gated Ion channel. Something "
        "like the old tabchannel from GENESIS, but also presents "
        "a similar interface as hhchan from GENESIS. ",
    };

	static Dinfo< ZombieHHChannel > dinfo;
    static Cinfo zombieHHChannelCinfo(
        "ZombieHHChannel",
        HHChannelBase::initCinfo(),
        0,
        0,
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
    );

    return &zombieHHChannelCinfo;
}

static const Cinfo* zombieHHChannelCinfo = ZombieHHChannel::initCinfo();
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
ZombieHHChannel::ZombieHHChannel()
{ ; }

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void ZombieHHChannel::vSetXpower( const Eref& e , double Xpower )
{
    Xpower_ = Xpower;
    hsolve_->setPowers( e.id(), Xpower_, Ypower_, Zpower_ );
}

void ZombieHHChannel::vSetYpower( const Eref& e , double Ypower )
{
    Ypower_ = Ypower;
    hsolve_->setPowers( e.id(), Xpower_, Ypower_, Zpower_ );
}

void ZombieHHChannel::vSetZpower( const Eref& e , double Zpower )
{
    Zpower_ = Zpower;
    hsolve_->setPowers( e.id(), Xpower_, Ypower_, Zpower_ );
}

void ZombieHHChannel::vSetGbar( const Eref& e , double Gbar )
{
    // cout << "in ZombieHHChannel::setGbar( " << e.id().path() << ", " << Gbar << " )\n";
    hsolve_->setHHChannelGbar( e.id(), Gbar );
}

double ZombieHHChannel::vGetGbar( const Eref& e  ) const
{
    return hsolve_->getHHChannelGbar( e.id() );
}

void ZombieHHChannel::vSetGk( const Eref& e , double Gk )
{
    hsolve_->setGk( e.id(), Gk );
}

double ZombieHHChannel::vGetGk( const Eref& e  ) const
{
    return hsolve_->getGk( e.id() );
}

void ZombieHHChannel::vSetEk( const Eref& e , double Ek )
{
    hsolve_->setEk( e.id(), Ek );
}

double ZombieHHChannel::vGetEk( const Eref& e  ) const
{
    return hsolve_->getEk( e.id() );
}

void ZombieHHChannel::vSetIk( const Eref& e , double Ik )
{
	;	// dummy
}

double ZombieHHChannel::vGetIk( const Eref& e  ) const
{
    return hsolve_->getIk( e.id() );
}

void ZombieHHChannel::vSetInstant( const Eref& e , int instant )
{
    hsolve_->setInstant( e.id(), instant );
}

int ZombieHHChannel::vGetInstant( const Eref& e  ) const
{
    return hsolve_->getInstant( e.id() );
}

void ZombieHHChannel::vSetX( const Eref& e , double X )
{
    hsolve_->setX( e.id(), X );
}

double ZombieHHChannel::vGetX( const Eref& e  ) const
{
    return hsolve_->getX( e.id() );
}

void ZombieHHChannel::vSetY( const Eref& e , double Y )
{
    hsolve_->setY( e.id(), Y );
}

double ZombieHHChannel::vGetY( const Eref& e  ) const
{
    return hsolve_->getY( e.id() );
}

void ZombieHHChannel::vSetZ( const Eref& e , double Z )
{
    hsolve_->setZ( e.id(), Z );
}

double ZombieHHChannel::vGetZ( const Eref& e  ) const
{
    return hsolve_->getZ( e.id() );
}

void ZombieHHChannel::vSetUseConcentration( const Eref& e, int value )
{
    cerr << "Error: HSolve::setUseConcentration(): Cannot change "
         "'useConcentration' once HSolve has been setup.\n";
}

void ZombieHHChannel::vSetModulation( const Eref& e , double modulation )
{
	if ( modulation > 0.0 ) {
		modulation_ = modulation;
    	hsolve_->setHHmodulation( e.id(), modulation );
	}
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void ZombieHHChannel::vProcess( const Eref& e, ProcPtr info )
{
    ;
}

void ZombieHHChannel::vReinit( const Eref& er, ProcPtr info )
{
    ;
}

void ZombieHHChannel::vHandleConc( const Eref& e, double conc )
{

  	hsolve_->addConc(e.id(),conc);

}

void ZombieHHChannel::vCreateGate(const Eref& e, string name)
{
	cout << "Warning: ZombieHHChannel::vCreateGate\n";
}

///////////////////////////////////////////////////
// HHGate functions
///////////////////////////////////////////////////

HHGate* ZombieHHChannel::vGetXgate( unsigned int i ) const
{
    return 0;
}

HHGate* ZombieHHChannel::vGetYgate( unsigned int i ) const
{
    return 0;
}

HHGate* ZombieHHChannel::vGetZgate( unsigned int i ) const
{
    return 0;
}

///////////////////////////////////////////////////
// Assign solver
///////////////////////////////////////////////////
void ZombieHHChannel::vHandleVm( double Vm )
{;}

void ZombieHHChannel::vSetSolver( const Eref& e , Id hsolve )
{
	if ( !hsolve.element()->cinfo()->isA( "HSolve" ) ) {
		cout << "Error: ZombieHHChannel::vSetSolver: Object: " <<
				hsolve.path() << " is not an HSolve. Aborted\n";
		hsolve_ = 0;
		assert( 0 );
		return;
	}
	hsolve_ = reinterpret_cast< HSolve* >( hsolve.eref().data() );
}
