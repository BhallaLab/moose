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
#include "../builtins/Interpol2D.h"
#include "ChanBase.h"
#include "ChanCommon.h"
#include "HHGate2D.h"
#include "HHChannel2D.h"

const double HHChannel2D::EPSILON = 1.0e-10;
const int HHChannel2D::INSTANT_X = 1;
const int HHChannel2D::INSTANT_Y = 2;
const int HHChannel2D::INSTANT_Z = 4;

const Cinfo* HHChannel2D::initCinfo()
{
	/////////////////////////////////////////////////////////////////////
	// Shared messages
	/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	static ValueFinfo< HHChannel2D, string > Xindex( "Xindex",
		"String for setting X index.",
		&HHChannel2D::setXindex,
		&HHChannel2D::getXindex
	);
	static ValueFinfo< HHChannel2D, string > Yindex( "Yindex",
		"String for setting Y index.",
		&HHChannel2D::setYindex,
		&HHChannel2D::getYindex
	);
	static ValueFinfo< HHChannel2D, string > Zindex( "Zindex",
		"String for setting Z index.",
		&HHChannel2D::setZindex,
		&HHChannel2D::getZindex
	);
		static ElementValueFinfo< HHChannel2D, double > Xpower( "Xpower",
			"Power for X gate",
			&HHChannel2D::setXpower,
			&HHChannel2D::getXpower
		);
		static ElementValueFinfo< HHChannel2D, double > Ypower( "Ypower",
			"Power for Y gate",
			&HHChannel2D::setYpower,
			&HHChannel2D::getYpower
		);
		static ElementValueFinfo< HHChannel2D, double > Zpower( "Zpower",
			"Power for Z gate",
			&HHChannel2D::setZpower,
			&HHChannel2D::getZpower
		);
		static ValueFinfo< HHChannel2D, int > instant( "instant",
			"Bitmapped flag: bit 0 = Xgate, bit 1 = Ygate, bit 2 = Zgate"
			"When true, specifies that the lookup table value should be"
			"used directly as the state of the channel, rather than used"
			"as a rate term for numerical integration for the state",
			&HHChannel2D::setInstant,
			&HHChannel2D::getInstant
		);
		static ValueFinfo< HHChannel2D, double > X( "X",
			"State variable for X gate",
			&HHChannel2D::setX,
			&HHChannel2D::getX
		);
		static ValueFinfo< HHChannel2D, double > Y( "Y",
			"State variable for Y gate",
			&HHChannel2D::setY,
			&HHChannel2D::getY
		);
		static ValueFinfo< HHChannel2D, double > Z( "Z",
			"State variable for Y gate",
			&HHChannel2D::setZ,
			&HHChannel2D::getZ
		);

///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	static DestFinfo concen( "concen",
		"Incoming message from Concen object to specific conc to use"
		"as the first concen variable",
		new OpFunc1< HHChannel2D, double >( &HHChannel2D::conc1 )
	);
	static DestFinfo concen2( "concen2",
		"Incoming message from Concen object to specific conc to use"
		"as the second concen variable",
		new OpFunc1< HHChannel2D, double >( &HHChannel2D::conc2 )
	);
///////////////////////////////////////////////////////
// FieldElementFinfo definition for HHGates. Note that these are made
// with the deferCreate flag off, so that the HHGates are created
// right away even if they are empty.
// I assume that we only have a single HHGate entry for each one.
///////////////////////////////////////////////////////
		static FieldElementFinfo< HHChannel2D, HHGate2D > gateX( "gateX",
			"Sets up HHGate X for channel",
			HHGate2D::initCinfo(),
			&HHChannel2D::getXgate,
			&HHChannel2D::setNumGates,
			&HHChannel2D::getNumXgates
		);
		static FieldElementFinfo< HHChannel2D, HHGate2D > gateY( "gateY",
			"Sets up HHGate Y for channel",
			HHGate2D::initCinfo(),
			&HHChannel2D::getYgate,
			&HHChannel2D::setNumGates,
			&HHChannel2D::getNumYgates
		);
		static FieldElementFinfo< HHChannel2D, HHGate2D > gateZ( "gateZ",
			"Sets up HHGate Z for channel",
			HHGate2D::initCinfo(),
			&HHChannel2D::getZgate,
			&HHChannel2D::setNumGates,
			&HHChannel2D::getNumZgates
		);
	static Finfo* HHChannel2DFinfos[] =
	{
		&Xindex,	// Value
		&Yindex,	// Value
		&Zindex,	// Value
		&Xpower,	// Value
		&Ypower,	// Value
		&Zpower,	// Value
		&instant,	// Value
		&X,			// Value
		&Y,			// Value
		&Z,			// Value
		&concen,	// Dest
		&concen2,	// Dest
		&gateX,		// FieldElement
		&gateY,		// FieldElement
		&gateZ		// FieldElement
	};

	static string doc[] =
	{
		"Name", "HHChannel2D",
		"Author", "Niraj Dudani, 2009, NCBS, Updated Upi Bhalla, 2011",
		"Description", "HHChannel2D: Hodgkin-Huxley type voltage-gated Ion channel. Something "
		"like the old tabchannel from GENESIS, but also presents "
		"a similar interface as hhchan from GENESIS. ",
	};

        static Dinfo< HHChannel2D > dinfo;
	static Cinfo HHChannel2DCinfo(
		"HHChannel2D",
		ChanBase::initCinfo(),
		HHChannel2DFinfos,
		sizeof( HHChannel2DFinfos ) / sizeof(Finfo *),
		&dinfo,
                doc,
                sizeof(doc) / sizeof(string)
	);

	return &HHChannel2DCinfo;
}

static const Cinfo* HHChannel2DCinfo = HHChannel2D::initCinfo();

HHChannel2D::HHChannel2D()
	:	ChanCommon(),
		Xpower_( 0.0 ),
		Ypower_( 0.0 ),
		Zpower_( 0.0 ),
        instant_(0),
        X_(0.0),
        Y_(0.0),
        Z_(0.0),
        xInited_(false),
        yInited_(false),
        zInited_(false),
		conc1_(0.0),
		conc2_(0.0),
		Xdep0_( -1 ),
		Xdep1_( -1 ),
		Ydep0_( -1 ),
		Ydep1_( -1 ),
		Zdep0_( -1 ),
		Zdep1_( -1 ),
        xGate_( 0 ),
        yGate_( 0 ),
        zGate_( 0 )
{;}


///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////
/**
 * Assigns the Xpower for this gate. If the gate exists and has
 * only this element for input, then change the gate value.
 * If the gate exists and has multiple parents, then make a new gate.
 * If the gate does not exist, make a new gate
 */

double HHChannel2D::getXpower( const Eref& e ) const
{
	return Xpower_;
}

double HHChannel2D::getYpower( const Eref& e ) const
{
	return Ypower_;
}

double HHChannel2D::getZpower( const Eref& e ) const
{
	return Zpower_;
}

void HHChannel2D::setInstant( int instant )
{
	instant_ = instant;
}
int HHChannel2D::getInstant() const
{
	return instant_;
}

void HHChannel2D::setX( double X )
{
        X_ = X;
        xInited_ = true;
}
double HHChannel2D::getX() const
{
	return X_;
}

void HHChannel2D::setY( double Y )
{
        Y_ = Y;
        yInited_ = true;
}
double HHChannel2D::getY() const
{
	return Y_;
}

void HHChannel2D::setZ( double Z )
{
        Z_ = Z;
        zInited_ = true;
}
double HHChannel2D::getZ() const
{
	return Z_;
}

string HHChannel2D::getXindex() const
{
	return Xindex_;
}

void HHChannel2D::setXindex( string Xindex )
{
	if ( Xindex == Xindex_ )
		return;

	Xindex_ = Xindex;
	Xdep0_ = dependency( Xindex, 0 );
	Xdep1_ = dependency( Xindex, 1 );

	assert( Xdep0_ >= 0 );
}

string HHChannel2D::getYindex() const
{
	return Yindex_;
}

void HHChannel2D::setYindex( string Yindex )
{
	if ( Yindex == Yindex_ )
		return;

	Yindex_ = Yindex;
	Ydep0_ = dependency( Yindex, 0 );
	Ydep1_ = dependency( Yindex, 1 );

	assert( Ydep0_ >= 0 );
}

string HHChannel2D::getZindex() const
{
	return Zindex_;
}

void HHChannel2D::setZindex( string Zindex )
{
	if ( Zindex == Zindex_ )
		return;

	Zindex_ = Zindex;
	Zdep0_ = dependency( Zindex, 0 );
	Zdep1_ = dependency( Zindex, 1 );

	assert( Zdep0_ >= 0 );
}

////////////////////////////////////////////////////////////////////
// HHGate2D access funcs
////////////////////////////////////////////////////////////////////

HHGate2D* HHChannel2D::getXgate( unsigned int i )
{
	return xGate_;
}

HHGate2D* HHChannel2D::getYgate( unsigned int i )
{
	return yGate_;
}

HHGate2D* HHChannel2D::getZgate( unsigned int i )
{
	return zGate_;
}

void HHChannel2D::setNumGates( unsigned int num )
{ ; }

unsigned int  HHChannel2D::getNumXgates() const
{
	return ( xGate_ != 0 );
}

unsigned int  HHChannel2D::getNumYgates() const
{
	return ( yGate_ != 0 );
}

unsigned int  HHChannel2D::getNumZgates() const
{
	return ( zGate_ != 0 );
}

double HHChannel2D::depValue( int dep )
{
	switch( dep )
	{
		case 0: return Vm_;
		case 1: return conc1_;
		case 2: return conc2_;
		default: assert( 0 ); return 0.0;
	}
}

int HHChannel2D::dependency( string index, unsigned int dim )
{
	static vector< map< string, int > > dep;
	if ( dep.empty() ) {
		dep.resize( 2 );

		dep[ 0 ][ "VOLT_INDEX" ] = 0;
		dep[ 0 ][ "C1_INDEX" ] = 1;
		dep[ 0 ][ "C2_INDEX" ] = 2;

		dep[ 0 ][ "VOLT_C1_INDEX" ] = 0;
		dep[ 0 ][ "VOLT_C2_INDEX" ] = 0;
		dep[ 0 ][ "C1_C2_INDEX" ] = 1;

		dep[ 1 ][ "VOLT_INDEX" ] = -1;
		dep[ 1 ][ "C1_INDEX" ] = -1;
		dep[ 1 ][ "C2_INDEX" ] = -1;

		dep[ 1 ][ "VOLT_C1_INDEX" ] = 1;
		dep[ 1 ][ "VOLT_C2_INDEX" ] = 2;
		dep[ 1 ][ "C1_C2_INDEX" ] = 2;
	}

	if ( dep[ dim ].find( index ) == dep[ dim ].end() )
		return -1;

	if ( dep[ dim ][ index ] == 0 )
		return 0;
	if ( dep[ dim ][ index ] == 1 )
		return 1;
	if ( dep[ dim ][ index ] == 2 )
		return 2;

	return -1;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void HHChannel2D::conc1( double conc )
{
	conc1_ = conc;
}

void HHChannel2D::conc2( double conc )
{
	conc2_ = conc;
}

///////////////////////////////////////////////////
// utility function definitions
///////////////////////////////////////////////////

/**
 * Returns the state variable for the new timestep based on
 * the internal variables A_ and B_ which were passed in from the gate.
 */
double HHChannel2D::integrate( double state, double dt, double A, double B )
{
	if ( B > EPSILON ) {
		double x = exp( -B * dt );
		return state * x + ( A / B ) * ( 1 - x );
	}
	return state + A * dt ;
}

void HHChannel2D::vProcess( const Eref& e, ProcPtr info )
{
	g_ += ChanBase::getGbar( e );
	double A = 0;
	double B = 0;
	if ( Xpower_ > 0 ) {
		xGate_->lookupBoth( depValue( Xdep0_ ), depValue( Xdep1_ ), &A, &B );
		if ( instant_ & INSTANT_X )
			X_ = A/B;
		else
			X_ = integrate( X_, info->dt, A, B );
		g_ *= takeXpower_( X_, Xpower_ );
	}

	if ( Ypower_ > 0 ) {
		yGate_->lookupBoth( depValue( Ydep0_ ), depValue( Ydep1_ ), &A, &B );
		if ( instant_ & INSTANT_Y )
			Y_ = A/B;
		else
			Y_ = integrate( Y_, info->dt, A, B );

		g_ *= takeYpower_( Y_, Ypower_ );
	}

	if ( Zpower_ > 0 ) {
        zGate_->lookupBoth( depValue( Zdep0_ ), depValue( Zdep1_ ), &A, &B );
		if ( instant_ & INSTANT_Z )
			Z_ = A/B;
		else
			Z_ = integrate( Z_, info->dt, A, B );

		g_ *= takeZpower_( Z_, Zpower_ );
	}

	ChanBase::setGk( e, g_ * vGetModulation( e ) );
	updateIk();
	// Gk_ = g_;
	// Ik_ = ( Ek_ - Vm_ ) * g_;

	// Send out the relevant channel messages.
	sendProcessMsgs( e, info );
    g_ = 0.0;
 }

 /**
  * Here we get the steady-state values for the gate (the 'instant'
  * calculation) as A_/B_.
  */
 void HHChannel2D::vReinit( const Eref& er, ProcPtr info )
 {
     g_ = ChanBase::getGbar( er );
     Element* e = er.element();

     double A = 0.0;
     double B = 0.0;
     if ( Xpower_ > 0 ) {
         xGate_->lookupBoth( depValue( Xdep0_ ), depValue( Xdep1_ ), &A, &B );
         if ( B < EPSILON ) {
             cout << "Warning: B_ value for " << e->getName() <<
                     " is ~0. Check X table\n";
             return;
         }
                 if (!xInited_)
                     X_ = A/B;
         g_ *= takeXpower_( X_, Xpower_ );
     }

     if ( Ypower_ > 0 ) {
         yGate_->lookupBoth( depValue( Ydep0_ ), depValue( Ydep1_ ), &A, &B );
         if ( B < EPSILON ) {
             cout << "Warning: B value for " << e->getName() <<
                     " is ~0. Check Y table\n";
             return;
         }
                 if (!yInited_)
                     Y_ = A/B;
         g_ *= takeYpower_( Y_, Ypower_ );
     }

     if ( Zpower_ > 0 ) {
         zGate_->lookupBoth( depValue( Zdep0_ ), depValue( Zdep1_ ), &A, &B );
         if ( B < EPSILON ) {
             cout << "Warning: B value for " << e->getName() <<
                     " is ~0. Check Z table\n";
             return;
         }
                 if (!zInited_)
                     Z_ = A/B;
         g_ *= takeZpower_( Z_, Zpower_ );
     }

	 ChanBase::setGk( er, g_ * vGetModulation( er ) );
     updateIk();
     // Gk_ = g_;
     // Ik_ = ( Ek_ - Vm_ ) * g_;

     // Send out the relevant channel messages.
     // Same for reinit as for process.
     sendReinitMsgs( er, info );
	g_ = 0.0;
}

////////////////////////////////////////////////////////////////////////
// Gate management stuff.
////////////////////////////////////////////////////////////////////////

bool HHChannel2D::setGatePower( const Eref& e, double power,
	double *assignee, const string& gateType )
{
	if ( power < 0 ) {
		cout << "Error: HHChannel2D::set" << gateType <<
			"power: Cannot use negative power: " << power << endl;
		return 0;
	}

	if ( doubleEq( power, *assignee ) )
		return 0;

	if ( doubleEq( *assignee, 0.0 ) && power > 0 ) {
		createGate( e, gateType );
	} else if ( doubleEq( power, 0.0 ) ) {
		destroyGate( e, gateType );
	}

	*assignee = power;
	return 1;
}

/**
 * Assigns the Xpower for this gate. If the gate exists and has
 * only this element for input, then change the gate value.
 * If the gate exists and has multiple parents, then make a new gate.
 * If the gate does not exist, make a new gate
 */
void HHChannel2D::setXpower( const Eref& e, double Xpower )
{
	if ( setGatePower( e, Xpower, &Xpower_, "X" ) )
		takeXpower_ = selectPower( Xpower );
}

void HHChannel2D::setYpower( const Eref& e, double Ypower )
{
	if ( setGatePower( e, Ypower, &Ypower_, "Y" ) )
		takeYpower_ = selectPower( Ypower );
}

void HHChannel2D::setZpower( const Eref& e, double Zpower )
{
	if ( setGatePower( e, Zpower, &Zpower_, "Z" ) )
		takeZpower_ = selectPower( Zpower );
}

/**
 * If the gate exists and has only this element for input, then change
 * the gate power.
 * If the gate exists and has multiple parents, then make a new gate,
 * 	set its power.
 * If the gate does not exist, make a new gate, set its power.
 *
 * The function is designed with the idea that if copies of this
 * channel are made, then they all point back to the original HHGate.
 * (Unless they are cross-node copies).
 * It is only if we subsequently alter the HHGate of this channel that
 * we need to make our own variant of the HHGate, or disconnect from
 * an existing one.
 * \todo: May need to convert to handling arrays and Erefs.
 */
// Assuming that the elements are simple elements. Use Eref for
// general case

bool HHChannel2D::checkOriginal( Id chanId ) const
{
	bool isOriginal = 1;
	if ( xGate_ ) {
		isOriginal = xGate_->isOriginalChannel( chanId );
	} else if ( yGate_ ) {
		isOriginal = yGate_->isOriginalChannel( chanId );
	} else if ( zGate_ ) {
		isOriginal = zGate_->isOriginalChannel( chanId );
	}
	return isOriginal;
}

void HHChannel2D::innerCreateGate( const string& gateName,
	HHGate2D** gatePtr, Id chanId, Id gateId )
{
	//Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	if ( *gatePtr ) {
		cout << "Warning: HHChannel2D::createGate: '" << gateName <<
			"' on Element '" << chanId.path() << "' already present\n";
		return;
	}
	*gatePtr = new HHGate2D( chanId, gateId );
}

void HHChannel2D::createGate( const Eref& e,
	string gateType )
{
	if ( !checkOriginal( e.id() ) ) {
		cout << "Warning: HHChannel2D::createGate: Not allowed from copied channel:\n" << e.id().path() << "\n";
		return;
	}

	if ( gateType == "X" )
		innerCreateGate( "xGate", &xGate_, e.id(), Id(e.id().value() + 1) );
	else if ( gateType == "Y" )
		innerCreateGate( "yGate", &yGate_, e.id(), Id(e.id().value() + 2) );
	else if ( gateType == "Z" )
		innerCreateGate( "zGate", &zGate_, e.id(), Id(e.id().value() + 3) );
	else
		cout << "Warning: HHChannel2D::createGate: Unknown gate type '" <<
			gateType << "'. Ignored\n";
}

void HHChannel2D::innerDestroyGate( const string& gateName,
	HHGate2D** gatePtr, Id chanId )
{
	if ( *gatePtr == 0 ) {
		cout << "Warning: HHChannel2D::destroyGate: '" << gateName <<
			"' on Element '" << chanId.path() << "' not present\n";
		return;
	}
	delete (*gatePtr);
	*gatePtr = 0;
}

void HHChannel2D::destroyGate( const Eref& e,
	string gateType )
{
	if ( !checkOriginal( e.id() ) ) {
		cout << "Warning: HHChannel2D::destroyGate: Not allowed from copied channel:\n" << e.id().path() << "\n";
		return;
	}

	if ( gateType == "X" )
		innerDestroyGate( "xGate", &xGate_, e.id() );
	else if ( gateType == "Y" )
		innerDestroyGate( "yGate", &yGate_, e.id() );
	else if ( gateType == "Z" )
		innerDestroyGate( "zGate", &zGate_, e.id() );
	else
		cout << "Warning: HHChannel2D::destroyGate: Unknown gate type '" <<
			gateType << "'. Ignored\n";
}

double HHChannel2D::powerN( double x, double p )
{
	if ( x > 0.0 )
		return exp( p * log( x ) );
	return 0.0;
}

PFDD HHChannel2D::selectPower( double power )
{
	if ( power == 0.0 )
		return powerN;
	else if ( power == 1.0 )
		return power1;
	else if ( power == 2.0 )
		return power2;
	else if ( power == 3.0 )
		return power3;
	else if ( power == 4.0 )
		return power4;
	else
		return powerN;
}

///////////////////////////////////////////////////
// Unit tests
///////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS

void testHHChannel2D2D()
{
	;
}
#endif
