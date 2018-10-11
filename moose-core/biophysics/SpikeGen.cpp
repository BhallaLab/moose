/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "SpikeGen.h"

	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////
static SrcFinfo1< double > *spikeOut() {
	static SrcFinfo1< double > spikeOut( "spikeOut",
			"Sends out a trigger for an event.");
	return &spikeOut;
}

const Cinfo* SpikeGen::initCinfo()
{
	///////////////////////////////////////////////////////
	// Shared message definitions
	///////////////////////////////////////////////////////
	static DestFinfo process( "process",
		"Handles process call",
		new ProcOpFunc< SpikeGen >( &SpikeGen::process ) );
	static DestFinfo reinit( "reinit",
		"Handles reinit call",
		new ProcOpFunc< SpikeGen >( &SpikeGen::reinit ) );

	static Finfo* processShared[] =
	{
		&process, &reinit
	};

	static SharedFinfo proc( "proc",
		"Shared message to receive Process message from scheduler",
		processShared, sizeof( processShared ) / sizeof( Finfo* ) );

	//////////////////////////////////////////////////////////////////
	// Dest Finfos.
	//////////////////////////////////////////////////////////////////
	static DestFinfo Vm( "Vm",
			"Handles Vm message coming in from compartment",
			new OpFunc1< SpikeGen, double >( &SpikeGen::handleVm ) );

	//////////////////////////////////////////////////////////////////
	// Value Finfos.
	//////////////////////////////////////////////////////////////////

	static ValueFinfo< SpikeGen, double > threshold( "threshold",
		"Spiking threshold, must cross it going up",
		&SpikeGen::setThreshold,
		&SpikeGen::getThreshold
	);
	static ValueFinfo< SpikeGen, double > refractT( "refractT",
		"Refractory Time.",
		&SpikeGen::setRefractT,
		&SpikeGen::getRefractT
	);
	static ValueFinfo< SpikeGen, double > absRefract( "abs_refract",
		"Absolute refractory time. Synonym for refractT.",
		&SpikeGen::setRefractT,
		&SpikeGen::getRefractT
	);
	static ReadOnlyValueFinfo< SpikeGen, bool > hasFired( "hasFired",
		"True if SpikeGen has just fired",
		&SpikeGen::getFired
	);
	static ValueFinfo< SpikeGen, bool > edgeTriggered( "edgeTriggered",
		"When edgeTriggered = 0, the SpikeGen will fire an event in each "
		"timestep while incoming Vm is > threshold and at least abs_refract"
		"time has passed since last event. This may be problematic if the "
		"incoming Vm remains above threshold for longer than abs_refract. "
		"Setting edgeTriggered to 1 resolves this as the SpikeGen generates"
		"an event only on the rising edge of the incoming Vm and will "
		"remain idle unless the incoming Vm goes below threshold.",
		&SpikeGen::setEdgeTriggered,
		&SpikeGen::getEdgeTriggered
	);

	static Finfo* spikeGenFinfos[] =
	{
		spikeOut(),	// SrcFinfo
		&proc,		// Shared
		&Vm,		// Dest
		&threshold,	// Value
		&refractT,	// Value
		&absRefract,	// Value
		&hasFired,	// ReadOnlyValue
		&edgeTriggered,	// Value

	};

	static string doc[] =
	{
		"Name", "SpikeGen",
		"Author", "Upi Bhalla",
		"Description", "SpikeGen object, for detecting threshold crossings."
		"The threshold detection can work in multiple modes.\n "
		"If the refractT < 0.0, then it fires an event only at the rising "
		"edge of the input voltage waveform"
	};
	static Dinfo< SpikeGen > dinfo;
	static Cinfo spikeGenCinfo(
		"SpikeGen",
		Neutral::initCinfo(),
		spikeGenFinfos, sizeof( spikeGenFinfos ) / sizeof( Finfo* ),
		&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &spikeGenCinfo;
}

static const Cinfo* spikeGenCinfo = SpikeGen::initCinfo();

SpikeGen::SpikeGen()
	: threshold_(0.0),
      refractT_(0.0),
      lastEvent_(0.0),
      V_(0.0),
	  fired_( 0 ),
      edgeTriggered_(1)
{;}

//////////////////////////////////////////////////////////////////
// Here we put the SpikeGen class functions.
//////////////////////////////////////////////////////////////////

// Value Field access function definitions.
void SpikeGen::setThreshold( double threshold )
{
	threshold_ = threshold;
}
double SpikeGen::getThreshold() const
{
	return threshold_;
}

void SpikeGen::setRefractT( double val )
{
	refractT_ = val;
}
double SpikeGen::getRefractT() const
{
	return refractT_;
}

/*
void SpikeGen::setAmplitude( double val )
{
	amplitude_ = val;
}
double SpikeGen::getAmplitude() const
{
	return amplitude_;
}

void SpikeGen::setState( double val )
{
	state_ = val;
}
double SpikeGen::getState() const
{
	return state_;
}
*/

bool SpikeGen::getFired() const
{
	return fired_;
}

void SpikeGen::setEdgeTriggered( bool yes )
{
	edgeTriggered_ = yes;
}
bool SpikeGen::getEdgeTriggered() const
{
	return edgeTriggered_;
}

//////////////////////////////////////////////////////////////////
// SpikeGen::Dest function definitions.
//////////////////////////////////////////////////////////////////

void SpikeGen::process( const Eref& e, ProcPtr p )
{
	double t = p->currTime;
	if ( V_ > threshold_ ) {
		if ((t + p->dt/2.0) >= (lastEvent_ + refractT_)) {
			if ( !( edgeTriggered_ && fired_ ) ) {
				spikeOut()->send( e, t );
				lastEvent_ = t;
				fired_ = true;
			}
		}
	} else {
            fired_ = false;
	}
}

// Set it so that first spike is allowed.
void SpikeGen::reinit( const Eref& e, ProcPtr p )
{
	lastEvent_ = -refractT_ ;
}

void SpikeGen::handleVm( double val )
{
	V_ = val;
}

/////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS
#include "../shell/Shell.h"

void testSpikeGen()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	Id sid = shell->doCreate( "SpikeGen", Id(), "spike", 1, MooseGlobal );
	SpikeGen& sg = *( reinterpret_cast< SpikeGen* >( sid.eref().data() ) );

	Eref er( sid.eref() );
	ProcInfo p;
	p.dt = 0.001;
	p.currTime = 0.0;
	sg.setThreshold( 1.0 );
	sg.setRefractT( 0.005 );

	sg.reinit( er, &p );
	sg.handleVm( 0.5 );
	sg.process( er, &p );
	assert( !sg.getFired() );
	p.currTime += p.dt;

	sg.handleVm( 0.999 );
	sg.process( er, &p );
	assert( !sg.getFired() );
	p.currTime += p.dt;

	sg.handleVm( 1.001 );
	sg.process( er, &p );
	assert( sg.getFired() );
	p.currTime += p.dt;

	sg.handleVm( 0.999 );
	sg.process( er, &p );
	assert( !sg.getFired() );
	p.currTime += p.dt;

	sg.handleVm( 2.0 ); // Too soon, refractory
	sg.process( er, &p );
	assert( !sg.getFired() );

	p.currTime += 0.005; // Now post-refractory
	sg.handleVm( 2.0 ); // Now not refractory
	sg.process( er, &p );
	assert( sg.getFired() );

	sid.destroy();
	cout << "." << flush;
}
#endif
