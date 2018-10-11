/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "ChanBase.h"
#include "ChanCommon.h"
#include "MgBlock.h"

const double EPSILON = 1.0e-12;

const Cinfo* MgBlock::initCinfo()
{
	/////////////////////////////////////////////////////////////////////
	// Shared messages
	/////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// Dest definitions
	///////////////////////////////////////////////////////
	static DestFinfo origChannel( "origChannel",
		"",
		new EpFunc2< MgBlock, double, double > (&MgBlock::origChannel )
	);
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
	static ValueFinfo< MgBlock, double > KMg_A( "KMg_A",
			"1/eta",
			&MgBlock::setKMg_A,
			&MgBlock::getKMg_A
		);
	static ValueFinfo< MgBlock, double > KMg_B( "KMg_B",
			"1/gamma",
			&MgBlock::setKMg_B,
			&MgBlock::getKMg_B
		);
	static ValueFinfo< MgBlock, double > CMg( "CMg",
			"[Mg] in mM",
			&MgBlock::setCMg,
			&MgBlock::getCMg
		);
	static ValueFinfo< MgBlock, double > Zk( "Zk",
			"Charge on ion",
			&MgBlock::setZk,
			&MgBlock::getZk
		);
	/////////////////////////////////////////////////////////////////////
	static Finfo* MgBlockFinfos[] =
	{
		&origChannel,	// Dest
		&KMg_A,			// Value
		&KMg_B,			// Value
		&CMg,			// Value
		&Zk,			// Value
	};

	static string doc[] =
	{
		"Name", "MgBlock",
		"Author", "Upinder S. Bhalla, 2007, NCBS",
		"Description", "MgBlock: Hodgkin-Huxley type voltage-gated Ion channel. Something "
				"like the old tabchannel from GENESIS, but also presents "
				"a similar interface as hhchan from GENESIS. ",
	};
	static Dinfo< MgBlock > dinfo;
	static Cinfo MgBlockCinfo(
		"MgBlock",
		ChanBase::initCinfo(),
		MgBlockFinfos,
		sizeof( MgBlockFinfos )/sizeof(Finfo *),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &MgBlockCinfo;
}

static const Cinfo* MgBlockCinfo = MgBlock::initCinfo();

///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
MgBlock::MgBlock()
	:	Zk_( 0.0 ),
		KMg_A_( 1.0 ), // These are NOT the same as the A, B state
		KMg_B_( 1.0 ), // variables used for Exp Euler integration.
		CMg_( 1.0 ), 	// Conc of Mg in mM
                origGk_(0.0)
{;}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void MgBlock::setKMg_A( double KMg_A )
{
	if ( KMg_A < EPSILON ) {
		cout << "Error: KMg_A=" << KMg_A << " must be > 0. Not set.\n";
	} else {
		KMg_A_ = KMg_A;
	}
}
double MgBlock::getKMg_A() const
{
	return KMg_A_;
}
void MgBlock::setKMg_B( double KMg_B )
{
	if ( KMg_B < EPSILON ) {
		cout << "Error: KMg_B=" << KMg_B << " must be > 0. Not set.\n";
	} else {
		KMg_B_ = KMg_B;
	}
}
double MgBlock::getKMg_B() const
{
	return KMg_B_;
}
void MgBlock::setCMg( double CMg )
{
	if ( CMg < EPSILON ) {
		cout << "Error: CMg = " << CMg << " must be > 0. Not set.\n";
	} else {
		CMg_ = CMg;
	}
}
double MgBlock::getCMg() const
{
	return CMg_;
}
double MgBlock::getZk() const
{
	return Zk_;
}
void MgBlock::setZk( double Zk )
{
	Zk_ = Zk;
}

///////////////////////////////////////////////////////////
// Process functions
///////////////////////////////////////////////////////////

void MgBlock::vProcess( const Eref& e, ProcPtr info )
{
	double KMg = KMg_A_ * exp(Vm_/KMg_B_);
	ChanBase::setGk( e, origGk_ * KMg / ( KMg + CMg_ ) );
	// ChanBase::setGk( ChanBase::getGk() * KMg / ( KMg + CMg_ ) );
	// Gk_ = Gk_ * KMg / (KMg + CMg_);

	updateIk();
	// send2< double, double >( e, channelSlot, Gk_, Ek_ );
	// Ik_ = Gk_ * (Ek_ - Vm_);
	sendProcessMsgs( e, info );
}

void MgBlock::vReinit( const Eref& e, ProcPtr info )
{
	Zk_ = 0;
	if ( CMg_ < EPSILON || KMg_B_ < EPSILON || KMg_A_ < EPSILON ) {
		cout << "Error: MgBlock::innerReinitFunc: fields KMg_A, KMg_B, CMg\nmust be greater than zero. Resetting to 1 to avoid numerical errors\n";
		if ( CMg_ < EPSILON ) CMg_ = 1.0;
		if ( KMg_B_ < EPSILON ) KMg_B_ = 1.0;
		if ( KMg_A_ < EPSILON ) KMg_A_ = 1.0;
	}
	sendReinitMsgs( e, info );
}

///////////////////////////////////////////////////
// Dest functions
///////////////////////////////////////////////////
void MgBlock::origChannel( const Eref& e, double Gk, double Ek )
{
	// setGk( Gk );
    origGk_ = Gk;
	setEk( e, Ek );
}

///////////////////////////////////////////////////
// Unit tests
///////////////////////////////////////////////////
