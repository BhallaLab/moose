/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
#include "HHGate.h"
#include "ChanBase.h"
#include "HHChannelBase.h"

const Cinfo* HHChannelBase::initCinfo()
{
	/////////////////////////////////////////////////////////////////////
	// Shared messages
	/////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
		static ElementValueFinfo< HHChannelBase, double > Xpower( "Xpower",
			"Power for X gate",
			&HHChannelBase::setXpower,
			&HHChannelBase::getXpower
		);
		static ElementValueFinfo< HHChannelBase, double > Ypower( "Ypower",
			"Power for Y gate",
			&HHChannelBase::setYpower,
			&HHChannelBase::getYpower
		);
		static ElementValueFinfo< HHChannelBase, double > Zpower( "Zpower",
			"Power for Z gate",
			&HHChannelBase::setZpower,
			&HHChannelBase::getZpower
		);
		static ElementValueFinfo< HHChannelBase, int > instant( "instant",
			"Bitmapped flag: bit 0 = Xgate, bit 1 = Ygate, bit 2 = Zgate"
			"When true, specifies that the lookup table value should be"
			"used directly as the state of the channel, rather than used"
			"as a rate term for numerical integration for the state",
			&HHChannelBase::setInstant,
			&HHChannelBase::getInstant
		);
		static ElementValueFinfo< HHChannelBase, double > X( "X",
			"State variable for X gate",
			&HHChannelBase::setX,
			&HHChannelBase::getX
		);
		static ElementValueFinfo< HHChannelBase, double > Y( "Y",
			"State variable for Y gate",
			&HHChannelBase::setY,
			&HHChannelBase::getY
		);
		static ElementValueFinfo< HHChannelBase, double > Z( "Z",
			"State variable for Y gate",
			&HHChannelBase::setZ,
			&HHChannelBase::getZ
		);
		static ElementValueFinfo< HHChannelBase, int > useConcentration(
			"useConcentration",
			"Flag: when true, use concentration message rather than Vm to"
			"control Z gate",
			&HHChannelBase::setUseConcentration,
			&HHChannelBase::getUseConcentration
		);

///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
	// IkOut SrcFinfo defined in base classes.

///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
		static DestFinfo concen( "concen",
			"Incoming message from Concen object to specific conc to use"
			"in the Z gate calculations",
			new EpFunc1< HHChannelBase, double >( &HHChannelBase::handleConc )
		);
		static DestFinfo createGate( "createGate",
			"Function to create specified gate."
			"Argument: Gate type [X Y Z]",
			new EpFunc1< HHChannelBase, string >( &HHChannelBase::createGate )
		);
///////////////////////////////////////////////////////
// FieldElementFinfo definition for HHGates. Note that these are made
// with the deferCreate flag off, so that the HHGates are created
// right away even if they are empty.
// Assume only a single entry allocated in each gate.
///////////////////////////////////////////////////////
		static FieldElementFinfo< HHChannelBase, HHGate > gateX( "gateX",
			"Sets up HHGate X for channel",
			HHGate::initCinfo(),
			&HHChannelBase::getXgate,
			&HHChannelBase::setNumGates,
			&HHChannelBase::getNumXgates
			// 1
		);
		static FieldElementFinfo< HHChannelBase, HHGate > gateY( "gateY",
			"Sets up HHGate Y for channel",
			HHGate::initCinfo(),
			&HHChannelBase::getYgate,
			&HHChannelBase::setNumGates,
			&HHChannelBase::getNumYgates
			// 1
		);
		static FieldElementFinfo< HHChannelBase, HHGate > gateZ( "gateZ",
			"Sets up HHGate Z for channel",
			HHGate::initCinfo(),
			&HHChannelBase::getZgate,
			&HHChannelBase::setNumGates,
			&HHChannelBase::getNumZgates
			// 1
		);

///////////////////////////////////////////////////////
	static Finfo* HHChannelBaseFinfos[] =
	{
		&Xpower,			// Value
		&Ypower,			// Value
		&Zpower,			// Value
		&instant,			// Value
		&X,					// Value
		&Y,					// Value
		&Z,					// Value
		&useConcentration,	// Value
		&concen,			// Dest
		&createGate,		// Dest
		&gateX,				// FieldElement
		&gateY,				// FieldElement
		&gateZ				// FieldElement
	};

	static string doc[] =
	{
		"Name", "HHChannelBase",
		"Author", "Upinder S. Bhalla, 2014, NCBS",
		"Description", "HHChannelBase: Base class for "
		"Hodgkin-Huxley type voltage-gated Ion channels. Something "
		"like the old tabchannel from GENESIS, but also presents "
		"a similar interface as hhchan from GENESIS. ",
	};

    static  ZeroSizeDinfo< int > dinfo;

	static Cinfo HHChannelBaseCinfo(
		"HHChannelBase",
		ChanBase::initCinfo(),
		HHChannelBaseFinfos,
		sizeof( HHChannelBaseFinfos )/sizeof(Finfo *),
                &dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &HHChannelBaseCinfo;
}

static const Cinfo* hhChannelCinfo = HHChannelBase::initCinfo();
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
HHChannelBase::HHChannelBase()
	:
			Xpower_( 0.0 ),
			Ypower_( 0.0 ),
			Zpower_( 0.0 ),
			useConcentration_( 0 ),
			modulation_( 1.0 )
{;}

HHChannelBase::~HHChannelBase()
{;}

bool checkPower( double power )
{
	if ( power < 0.0 ) {
		cout << "Warning: HHChannelBase::setPower: Cannot be negative\n";
		return false;
	}
	if ( power > 5.0 ) {
		cout << "Warning: HHChannelBase::setPower: unlikely to be > 5\n";
		return false;
	}
	return true;
}
/**
 * Assigns the Xpower for this gate. If the gate exists and has
 * only this element for input, then change the gate value.
 * If the gate exists and has multiple parents, then make a new gate.
 * If the gate does not exist, make a new gate
 */
void HHChannelBase::setXpower( const Eref& e, double power )
{
	if ( checkPower( power ) )
		vSetXpower( e, power );
}

void HHChannelBase::setYpower( const Eref& e, double power )
{
	if ( checkPower( power ) )
		vSetYpower( e, power );
}

void HHChannelBase::setZpower( const Eref& e, double power )
{
	if ( checkPower( power ) )
		vSetZpower( e, power );
}

void HHChannelBase::createGate( const Eref& e, string gateType )
{
	vCreateGate( e, gateType );
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

double HHChannelBase::getXpower( const Eref& e ) const
{
	return Xpower_;
}

double HHChannelBase::getYpower( const Eref& e ) const
{
	return Ypower_;
}

double HHChannelBase::getZpower( const Eref& e ) const
{
	return Zpower_;
}

void HHChannelBase::setInstant( const Eref& e, int instant )
{
	vSetInstant( e, instant );
}
int HHChannelBase::getInstant( const Eref& e ) const
{
	return vGetInstant( e );
}

void HHChannelBase::setX( const Eref& e, double X )
{
	vSetX( e, X );
}
double HHChannelBase::getX( const Eref& e ) const
{
	return vGetX( e );
}

void HHChannelBase::setY( const Eref& e, double Y )
{
	vSetY( e, Y );
}
double HHChannelBase::getY( const Eref& e ) const
{
	return vGetY( e );
}

void HHChannelBase::setZ( const Eref& e, double Z )
{
	vSetZ( e, Z );
}
double HHChannelBase::getZ( const Eref& e ) const
{
	return vGetZ( e );
}

void HHChannelBase::setUseConcentration( const Eref& e, int value )
{
	useConcentration_ = value;
	vSetUseConcentration( e, value );
}

int HHChannelBase::getUseConcentration( const Eref& e) const
{
	return useConcentration_;
}

double HHChannelBase::vGetModulation( const Eref& e) const
{
	return modulation_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void HHChannelBase::handleConc( const Eref& e, double conc )
{
	vHandleConc( e, conc );
}


///////////////////////////////////////////////////
// HHGate functions
///////////////////////////////////////////////////


HHGate* HHChannelBase::getXgate( unsigned int i )
{
	return vGetXgate( i );
}

HHGate* HHChannelBase::getYgate( unsigned int i )
{
	return vGetYgate( i );
}

HHGate* HHChannelBase::getZgate( unsigned int i )
{
	return vGetZgate( i );
}

void HHChannelBase::setNumGates( unsigned int num )
{ ; }

unsigned int  HHChannelBase::getNumXgates() const
{
	return ( vGetXgate(0) != 0 );
}

unsigned int  HHChannelBase::getNumYgates() const
{
	return ( vGetYgate(0) != 0 );
}

unsigned int  HHChannelBase::getNumZgates() const
{
	return ( vGetZgate(0) != 0 );
}
///////////////////////////////////////////////////
// Utility function
///////////////////////////////////////////////////
double HHChannelBase::powerN( double x, double p )
{
	if ( x > 0.0 )
		return exp( p * log( x ) );
	return 0.0;
}

PFDD HHChannelBase::selectPower( double power )
{
	if ( doubleEq( power, 0.0 ) )
		return powerN;
	else if ( doubleEq( power, 1.0 ) )
		return power1;
	else if ( doubleEq( power, 2.0 ) )
		return power2;
	else if ( doubleEq( power, 3.0 ) )
		return power3;
	else if ( doubleEq( power, 4.0 ) )
		return power4;
	else
		return powerN;
}
/////////////////////////////////////////////////////////////////////
// Dummy instantiation, the zombie derivatives make the real function
void HHChannelBase::vSetSolver( const Eref& e, Id hsolve )
{;}

void HHChannelBase::zombify( Element* orig, const Cinfo* zClass, Id hsolve )
{
	if ( orig->cinfo() == zClass )
		return;
	unsigned int start = orig->localDataStart();
	unsigned int num = orig->numLocalData();
	if ( num == 0 )
		return;
	// Parameters are Gbar, Ek, Xpower, Ypower, Zpower, useConcentration
	// We also want to haul the original gates over, this is done earlier
	// in the HSolve building process. So just six terms.
	vector< double > chandata( num * 6, 0.0 );
	vector< double >::iterator j = chandata.begin();

	for ( unsigned int i = 0; i < num; ++i ) {
		Eref er( orig, i + start );
		const HHChannelBase* hb =
			reinterpret_cast< const HHChannelBase* >( er.data() );
		*j = hb->vGetGbar( er );
		*(j+1) = hb->vGetEk( er);
		*(j+2) = hb->getXpower( er );
		*(j+3) = hb->getYpower( er );
		*(j+4) = hb->getZpower( er );
		*(j+5) = hb->getUseConcentration( er );
		j+= 6;
	}
	orig->zombieSwap( zClass );
	j = chandata.begin();
	for ( unsigned int i = 0; i < num; ++i ) {
		Eref er( orig, i + start );
		HHChannelBase* hb = reinterpret_cast< HHChannelBase* >( er.data() );
		hb->vSetSolver( er, hsolve );
		hb->vSetGbar( er, *j );
		hb->vSetEk( er, *(j+1) );
		hb->vSetXpower( er, *(j+2) );
		hb->vSetYpower( er, *(j+3) );
		hb->vSetZpower( er, *(j+4) );
		// hb->vSetUseConcentration( er, *(j+5) > 0.5 );
		// Disable this assignment because the Solver already reads the
		// value, and it triggers an error msg.
		j+= 6;
	}
}
