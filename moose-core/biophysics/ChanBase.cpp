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
#include "ChanBase.h"

SrcFinfo1< double >* ChanBase::permeability()
{
	static SrcFinfo1< double > permeability( "permeabilityOut",
		"Conductance term going out to GHK object" );
	return &permeability;
}

SrcFinfo2< double, double >* ChanBase::channelOut()
{
	static SrcFinfo2< double, double > channelOut( "channelOut",
		"Sends channel variables Gk and Ek to compartment" );
	return &channelOut;
}

SrcFinfo1< double >* ChanBase::IkOut()
{
	static SrcFinfo1< double > IkOut( "IkOut",
		"Channel current. This message typically goes to concen"
		"objects that keep track of ion concentration." );
	return &IkOut;
}

const Cinfo* ChanBase::initCinfo()
{
	/////////////////////////////////////////////////////////////////////
	// Shared messages
	/////////////////////////////////////////////////////////////////////
	static DestFinfo process( "process",
		"Handles process call",
		new ProcOpFunc< ChanBase >( &ChanBase::process ) );
	static DestFinfo reinit( "reinit",
		"Handles reinit call",
		new ProcOpFunc< ChanBase >( &ChanBase::reinit ) );

	static Finfo* processShared[] =
	{
		&process, &reinit
	};

	static SharedFinfo proc( "proc",
		"Shared message to receive Process message from scheduler",
		processShared, sizeof( processShared ) / sizeof( Finfo* ) );

	/////////////////////////////////////////////////////////////////////
	/// ChannelOut SrcFinfo defined above.
	static DestFinfo Vm( "Vm",
		"Handles Vm message coming in from compartment",
		new OpFunc1< ChanBase, double >( &ChanBase::handleVm ) );

	static Finfo* channelShared[] =
	{
		channelOut(), &Vm
	};
	static SharedFinfo channel( "channel",
		"This is a shared message to couple channel to compartment. "
		"The first entry is a MsgSrc to send Gk and Ek to the compartment "
		"The second entry is a MsgDest for Vm from the compartment.",
		channelShared, sizeof( channelShared ) / sizeof( Finfo* )
	);

	///////////////////////////////////////////////////////
	// Here we reuse the Vm DestFinfo declared above.

	/// Permability SrcFinfo defined above.
	static Finfo* ghkShared[] =
	{
		&Vm, permeability()
	};
	static SharedFinfo ghk( "ghk",
		"Message to Goldman-Hodgkin-Katz object",
		ghkShared, sizeof( ghkShared ) / sizeof( Finfo* ) );

///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////

		static ElementValueFinfo< ChanBase, double > Gbar( "Gbar",
			"Maximal channel conductance",
			&ChanBase::setGbar,
			&ChanBase::getGbar
		);
		static ElementValueFinfo< ChanBase, double > modulation( "modulation",
			"Modulation, i.e, scale factor for channel conductance."
			"Note that this is a regular parameter, it is not "
			"recomputed each timestep. Thus one can use a slow update, "
			"say, from a molecule pool, to send a message to set "
			"the modulation, and it will stay at the set value even if "
			"the channel runs many timesteps before the next assignment. "
			"This differs from the GENESIS semantics of a similar message,"
			"which required update each timestep. ",
			&ChanBase::setModulation,
			&ChanBase::getModulation
		);
		static ElementValueFinfo< ChanBase, double > Ek( "Ek",
			"Reversal potential of channel",
			&ChanBase::setEk,
			&ChanBase::getEk
		);
		static ElementValueFinfo< ChanBase, double > Gk( "Gk",
			"Channel conductance variable",
			&ChanBase::setGk,
			&ChanBase::getGk
		);
		static ReadOnlyElementValueFinfo< ChanBase, double > Ik( "Ik",
			"Channel current variable",
			&ChanBase::getIk
		);

///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
	// IkOut SrcFinfo defined above.

///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
	static Finfo* ChanBaseFinfos[] =
	{
		&channel,			// Shared
		&ghk,				// Shared
		&Gbar,				// Value
		&modulation,		// Value
		&Ek,				// Value
		&Gk,				// Value
		&Ik,				// ReadOnlyValue
		IkOut(),			// Src
		&proc,				// Shared
	};

	static string doc[] =
	{
		"Name", "ChanBase",
		"Author", "Upinder S. Bhalla, 2007-2014, NCBS",
		"Description", "ChanBase: Base class for assorted ion channels."
		"Presents a common interface for all of them. ",
	};

        static  ZeroSizeDinfo< int > dinfo;

	static Cinfo ChanBaseCinfo(
		"ChanBase",
		Neutral::initCinfo(),
		ChanBaseFinfos,
		sizeof( ChanBaseFinfos )/sizeof(Finfo *),
                &dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &ChanBaseCinfo;
}

static const Cinfo* chanBaseCinfo = ChanBase::initCinfo();
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
ChanBase::ChanBase()
{ ; }

ChanBase::~ChanBase()
{;}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////
//
void ChanBase::setGbar( const Eref& e, double Gbar )
{
	// Call virtual functions of derived classes for this operation.
	vSetGbar( e, Gbar );
}

double ChanBase::getGbar( const Eref& e ) const
{
	return vGetGbar( e );
}

void ChanBase::setModulation( const Eref& e, double modulation )
{
	// Call virtual functions of derived classes for this operation.
	vSetModulation( e, modulation );
}

double ChanBase::getModulation( const Eref& e ) const
{
	return vGetModulation( e );
}

void ChanBase::setEk( const Eref& e, double Ek )
{
	vSetEk( e, Ek );
}
double ChanBase::getEk( const Eref& e ) const
{
	return vGetEk( e );
}

void ChanBase::setGk( const Eref& e, double Gk )
{
	vSetGk( e, Gk );
}
double ChanBase::getGk( const Eref& e ) const
{
	return vGetGk( e );
}

void ChanBase::setIk( const Eref& e, double Ik )
{
	vSetIk( e, Ik );
}
double ChanBase::getIk( const Eref& e ) const
{
	return vGetIk( e );
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void ChanBase::handleVm( double Vm )
{
	vHandleVm( Vm );
}

///////////////////////////////////////////////////
// Looks like a dest function, but it is only called
// from the child class. Sends out various messages.
///////////////////////////////////////////////////

void ChanBase::process(  const Eref& e, const ProcPtr info )
{
	vProcess( e, info );
}


void ChanBase::reinit(  const Eref& e, const ProcPtr info )
{
	vReinit( e, info );
}
