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
#include "ChanCommon.h"
#include "HHChannelBase.h"
#include "HHChannel.h"
#include "../shell/Shell.h"

const double HHChannel::EPSILON = 1.0e-10;
const int HHChannel::INSTANT_X = 1;
const int HHChannel::INSTANT_Y = 2;
const int HHChannel::INSTANT_Z = 4;

const Cinfo* HHChannel::initCinfo()
{
///////////////////////////////////////////////////////
	static string doc[] =
	{
		"Name", "HHChannel",
		"Author", "Upinder S. Bhalla, 2007, NCBS",
		"Description", "HHChannel: Hodgkin-Huxley type voltage-gated Ion channel. Something "
		"like the old tabchannel from GENESIS, but also presents "
		"a similar interface as hhchan from GENESIS. ",
	};

        static  Dinfo< HHChannel > dinfo;

	static Cinfo HHChannelCinfo(
		"HHChannel",
		HHChannelBase::initCinfo(),
		0,
		0,
                &dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &HHChannelCinfo;
}

static const Cinfo* hhChannelCinfo = HHChannel::initCinfo();
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
HHChannel::HHChannel()
			:
                  conc_( 0.0 ),
                  instant_( 0 ),
                  X_( 0.0 ), Y_( 0.0 ), Z_( 0.0 ),
                  xInited_( false ), yInited_( false ), zInited_( false ),
                  g_( 0.0 ),
                  xGate_( 0 ),
                  yGate_( 0 ),
                  zGate_( 0 ),
                  myId_()
{
	;
}

HHChannel::~HHChannel()
{
	// if ( xGate_ && reinterpret_cast< char* >( this ) ==
	// 	ObjId( xGate_->originalChannelId(), 0 ).data() )
	// 	delete xGate_;
	// if ( yGate_ && reinterpret_cast< char* >( this ) ==
	// 	ObjId( yGate_->originalChannelId(), 0 ).data() )
	// 	delete yGate_;
	// if ( zGate_ && reinterpret_cast< char* >( this ) ==
	// 	ObjId( zGate_->originalChannelId(), 0 ).data() )
	// 	delete zGate_;
}

bool HHChannel::setGatePower( const Eref& e, double power,
	double *assignee, const string& gateType )
{
	if ( doubleEq( power, *assignee ) )
		return false;

	if ( doubleEq( *assignee, 0.0 ) && power > 0 ) {
		createGate( e, gateType );
	} else if ( doubleEq( power, 0.0 ) ) {
		// destroyGate( e, gateType );
	}
	*assignee = power;

	return true;
}

/**
 * Assigns the Xpower for this gate. If the gate exists and has
 * only this element for input, then change the gate value.
 * If the gate exists and has multiple parents, then make a new gate.
 * If the gate does not exist, make a new gate
 */
void HHChannel::vSetXpower( const Eref& e, double power )
{
	if ( setGatePower( e, power, &Xpower_, "X" ) )
		takeXpower_ = selectPower( power );
}

void HHChannel::vSetYpower( const Eref& e, double power )
{
	if ( setGatePower( e, power, &Ypower_, "Y" ) )
		takeYpower_ = selectPower( power );
}

void HHChannel::vSetZpower( const Eref& e, double power )
{
	if ( setGatePower( e, power, &Zpower_, "Z" ) ) {
		takeZpower_ = selectPower( power );
		useConcentration_ = 1; // Not sure about this.
	}
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

bool HHChannel::checkOriginal( Id chanId ) const
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

void HHChannel::innerCreateGate( const string& gateName,
	HHGate** gatePtr, Id chanId, Id gateId )
{
	//Shell* shell = reinterpret_cast< Shell* >( ObjId( Id(), 0 ).data() );
	if ( *gatePtr ) {
		cout << "Warning: HHChannel::createGate: '" << gateName <<
			"' on Element '" << chanId.path() << "' already present\n";
		return;
	}
	*gatePtr = new HHGate( chanId, gateId );
}

void HHChannel::vCreateGate( const Eref& e,
	string gateType )
{
	if ( !checkOriginal( e.id() ) ) {
		cout << "Warning: HHChannel::createGate: Not allowed from copied channel:\n" << e.id().path() << "\n";
		return;
	}

	if ( gateType == "X" )
		innerCreateGate( "xGate", &xGate_, e.id(), Id(e.id().value() + 1) );
	else if ( gateType == "Y" )
		innerCreateGate( "yGate", &yGate_, e.id(), Id(e.id().value() + 2) );
	else if ( gateType == "Z" )
		innerCreateGate( "zGate", &zGate_, e.id(), Id(e.id().value() + 3) );
	else
		cout << "Warning: HHChannel::createGate: Unknown gate type '" <<
			gateType << "'. Ignored\n";
}

void HHChannel::innerDestroyGate( const string& gateName,
	HHGate** gatePtr, Id chanId )
{
	if ( *gatePtr == 0 ) {
		cout << "Warning: HHChannel::destroyGate: '" << gateName <<
			"' on Element '" << chanId.path() << "' not present\n";
		return;
	}
	delete (*gatePtr);
	*gatePtr = 0;
}

void HHChannel::destroyGate( const Eref& e,
	string gateType )
{
	if ( !checkOriginal( e.id() ) ) {
		cout << "Warning: HHChannel::destroyGate: Not allowed from copied channel:\n" << e.id().path() << "\n";
		return;
	}

	if ( gateType == "X" )
		innerDestroyGate( "xGate", &xGate_, e.id() );
	else if ( gateType == "Y" )
		innerDestroyGate( "yGate", &yGate_, e.id() );
	else if ( gateType == "Z" )
		innerDestroyGate( "zGate", &zGate_, e.id() );
	else
		cout << "Warning: HHChannel::destroyGate: Unknown gate type '" <<
			gateType << "'. Ignored\n";
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void HHChannel::vSetInstant( const Eref& e, int instant )
{
	instant_ = instant;
}

int HHChannel::vGetInstant( const Eref& e ) const
{
	return instant_;
}

void HHChannel::vSetX( const Eref& e, double X )
{
        X_ = X;
        xInited_ = true;
}
double HHChannel::vGetX( const Eref& e ) const
{
	return X_;
}

void HHChannel::vSetY( const Eref& e, double Y )
{
        Y_ = Y;
        yInited_ = true;
}
double HHChannel::vGetY( const Eref& e ) const
{
	return Y_;
}

void HHChannel::vSetZ( const Eref& e, double Z )
{
        Z_ = Z;
        zInited_ = true;
}
double HHChannel::vGetZ( const Eref& e ) const
{
	return Z_;
}

void HHChannel::vSetUseConcentration( const Eref& e, int value )
{
	useConcentration_ = value;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

/**
 * Returns the state variable for the new timestep based on
 * the internal variables A_ and B_ which were passed in from the gate.
 */
double HHChannel::integrate( double state, double dt, double A, double B )
{
	if ( B > EPSILON ) {
		double x = exp( -B * dt );
		return state * x + ( A / B ) * ( 1 - x );
	}
	return state + A * dt ;
}

void HHChannel::vProcess( const Eref& e, ProcPtr info )
{
	g_ += ChanCommon::vGetGbar( e );
	double A = 0;
	double B = 0;
	if ( Xpower_ > 0 ) {
		xGate_->lookupBoth( Vm_, &A, &B );
		if ( instant_ & INSTANT_X )
			X_ = A/B;
		else
			X_ = integrate( X_, info->dt, A, B );
		g_ *= takeXpower_( X_, Xpower_ );
	}

	if ( Ypower_ > 0 ) {
		yGate_->lookupBoth( Vm_, &A, &B );
		if ( instant_ & INSTANT_Y )
			Y_ = A/B;
		else
			Y_ = integrate( Y_, info->dt, A, B );

		g_ *= takeYpower_( Y_, Ypower_ );
	}

	if ( Zpower_ > 0 ) {
		if ( useConcentration_ )
			zGate_->lookupBoth( conc_, &A, &B );
		else
			zGate_->lookupBoth( Vm_, &A, &B );
		if ( instant_ & INSTANT_Z )
			Z_ = A/B;
		else
			Z_ = integrate( Z_, info->dt, A, B );

		g_ *= takeZpower_( Z_, Zpower_ );
	}

	ChanCommon::vSetGk( e, g_ * HHChannelBase::modulation_ );
	ChanCommon::updateIk();
	// Gk_ = g_;
	// Ik_ = ( Ek_ - Vm_ ) * g_;

	// Send out the relevant channel messages.
	ChanCommon::sendProcessMsgs( e, info );

	g_ = 0.0;
}

/**
 * Here we get the steady-state values for the gate (the 'instant'
 * calculation) as A_/B_.
 */
void HHChannel::vReinit( const Eref& er, ProcPtr info )
{
	g_ = ChanCommon::vGetGbar( er );
	Element* e = er.element();

	double A = 0.0;
	double B = 0.0;
	if ( Xpower_ > 0 ) {
		assert( xGate_ );
		xGate_->lookupBoth( Vm_, &A, &B );
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
		assert( yGate_ );
		yGate_->lookupBoth( Vm_, &A, &B );
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
		assert( zGate_ );
		if ( useConcentration_ )
			zGate_->lookupBoth( conc_, &A, &B );
		else
			zGate_->lookupBoth( Vm_, &A, &B );
		if ( B < EPSILON ) {
			cout << "Warning: B value for " << e->getName() <<
					" is ~0. Check Z table\n";
			return;
		}
                if (!zInited_)
                    Z_ = A/B;
		g_ *= takeZpower_( Z_, Zpower_ );
	}

	ChanCommon::vSetGk( er, g_ * HHChannelBase::modulation_ );
	ChanCommon::updateIk();
	// Gk_ = g_;
	// Ik_ = ( Ek_ - Vm_ ) * g_;

	// Send out the relevant channel messages.
	// Same for reinit as for process.
	ChanCommon::sendReinitMsgs( er, info );

	g_ = 0.0;
}

void HHChannel::vHandleConc( const Eref& e, double conc )
{
	conc_ = conc;
}

void HHChannel::vSetModulation( const Eref& e, double modulation )
{
	if ( modulation > 0.0 )
		HHChannelBase::modulation_ = modulation;
}

double HHChannel::vGetModulation( const Eref& e ) const
{
	return HHChannelBase::modulation_;
}


///////////////////////////////////////////////////
// HHGate functions
///////////////////////////////////////////////////


HHGate* HHChannel::vGetXgate( unsigned int i ) const
{
	return xGate_;
}

HHGate* HHChannel::vGetYgate( unsigned int i ) const
{
	return yGate_;
}

HHGate* HHChannel::vGetZgate( unsigned int i ) const
{
	return zGate_;
}
