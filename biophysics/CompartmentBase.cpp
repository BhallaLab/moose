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
#include "CompartmentBase.h"
#include "CompartmentDataHolder.h"
#include "../shell/Wildcard.h"

static const double RANGE = 4.0e-17;
using namespace moose;

/*
 * This Finfo is used to send out Vm to channels, spikegens, etc.
 *
 * It is exposed here so that HSolve can also use it to send out
 * the Vm to the recipients.
 */
// Static function.
SrcFinfo1< double >* CompartmentBase::VmOut() {
	static SrcFinfo1< double > VmOut( "VmOut",
		"Sends out Vm value of compartment on each timestep" );
	return &VmOut;
}

// Here we send out Vm, but to a different subset of targets. So we
// have to define a new SrcFinfo even though the contents of the msg
// are still only Vm.

static SrcFinfo1< double >* axialOut() {
	static SrcFinfo1< double > axialOut( "axialOut",
		"Sends out Vm value of compartment to adjacent compartments,"
		"on each timestep" );
	return &axialOut;
}

static SrcFinfo2< double, double >* raxialOut()
{
	static SrcFinfo2< double, double > raxialOut( "raxialOut",
		"Sends out Raxial information on each timestep, "
		"fields are Ra and Vm" );
	return &raxialOut;
}

/**
 * The initCinfo() function sets up the CompartmentBase class.
 * This function uses the common trick of having an internal
 * static value which is created the first time the function is called.
 * There are several static arrays set up here. The ones which
 * use SharedFinfos are for shared messages where multiple kinds
 * of information go along the same connection.
 */
const Cinfo* CompartmentBase::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Shared messages
	///////////////////////////////////////////////////////////////////
	static DestFinfo process( "process",
		"Handles 'process' call",
		new ProcOpFunc< CompartmentBase >( &CompartmentBase::process ) );

	static DestFinfo reinit( "reinit",
		"Handles 'reinit' call",
		new ProcOpFunc< CompartmentBase >( &CompartmentBase::reinit ) );

	static Finfo* processShared[] =
	{
		&process, &reinit
	};

	static SharedFinfo proc( "proc",
		"This is a shared message to receive Process messages from the scheduler"
		"objects. The Process should be called _second_ in each clock tick, after the Init message."
		"The first entry in the shared msg is a MsgDest "
		"for the Process operation. It has a single argument, "
		"ProcInfo, which holds lots of information about current "
		"time, thread, dt and so on. The second entry is a MsgDest "
		"for the Reinit operation. It also uses ProcInfo. "
		"- Handles 'reinit' and 'process' calls.",
		processShared, sizeof( processShared ) / sizeof( Finfo* )
	);
	///////////////////////////////////////////////////////////////////

	static DestFinfo initProc( "initProc",
		"Handles Process call for the 'init' phase of the CompartmentBase "
		"calculations. These occur as a separate Tick cycle from the "
		"regular proc cycle, and should be called before the proc msg.",
		new ProcOpFunc< CompartmentBase >( &CompartmentBase::initProc ) );
	static DestFinfo initReinit( "initReinit",
		"Handles Reinit call for the 'init' phase of the CompartmentBase "
		"calculations.",
		new ProcOpFunc< CompartmentBase >( &CompartmentBase::initReinit ) );
	static Finfo* initShared[] =
	{
		&initProc, &initReinit
	};

	static SharedFinfo init( "init",
			"This is a shared message to receive Init messages from "
			"the scheduler objects. Its job is to separate the "
			"compartmental calculations from the message passing. "
			"It doesn't really need to be shared, as it does not use "
			"the reinit part, but the scheduler objects expect this "
			"form of message for all scheduled output. The first "
			"entry is a MsgDest for the Process operation. It has a "
			"single argument, ProcInfo, which holds lots of "
			"information about current time, thread, dt and so on. "
			"The second entry is a dummy MsgDest for the Reinit "
			"operation. It also uses ProcInfo. "
			"- Handles 'initProc' and 'initReinit' calls.",
		initShared, sizeof( initShared ) / sizeof( Finfo* )
	);

	///////////////////////////////////////////////////////////////////

	static DestFinfo handleChannel( "handleChannel",
		"Handles conductance and Reversal potential arguments from Channel",
		new EpFunc2< CompartmentBase, double, double >( &CompartmentBase::handleChannel ) );
	// VmOut is declared above as it needs to be in scope for later funcs.

	static Finfo* channelShared[] =
	{
		&handleChannel, CompartmentBase::VmOut()
	};
	static SharedFinfo channel( "channel",
			"This is a shared message from a compartment to channels. "
			"The first entry is a MsgDest for the info coming from "
			"the channel. It expects Gk and Ek from the channel "
			"as args. The second entry is a MsgSrc sending Vm "
			"- Handles 'handleChannel' and 'VmOut' calls.",
		channelShared, sizeof( channelShared ) / sizeof( Finfo* )
	);
	///////////////////////////////////////////////////////////////////
	// axialOut declared above as it is needed in file scope
	static DestFinfo handleRaxial( "handleRaxial",
		"Handles Raxial info: arguments are Ra and Vm.",
		new OpFunc2< CompartmentBase, double, double >(
			&CompartmentBase::handleRaxial )
	);

	static Finfo* axialShared[] =
	{
		axialOut(), &handleRaxial
	};
	static SharedFinfo axial( "axial",
			"This is a shared message between asymmetric compartments. "
			"axial messages (this kind) connect up to raxial "
			"messages (defined below). The soma should use raxial "
			"messages to connect to the axial message of all the "
			"immediately adjacent dendritic compartments.This puts "
			"the (low) somatic resistance in series with these "
			"dendrites. Dendrites should then use raxial messages to"
			"connect on to more distal dendrites. In other words, "
			"raxial messages should face outward from the soma. "
			"The first entry is a MsgSrc sending Vm to the axialFunc"
			"of the target compartment. The second entry is a MsgDest "
			"for the info coming from the other compt. It expects "
			"Ra and Vm from the other compt as args. Note that the "
			"message is named after the source type. "
			"- Handles 'axialOut' and 'handleRaxial' calls.",
		axialShared, sizeof( axialShared ) / sizeof( Finfo* )
	);

	///////////////////////////////////////////////////////////////////
	static DestFinfo handleAxial( "handleAxial",
		"Handles Axial information. Argument is just Vm.",
		new OpFunc1< CompartmentBase, double >( &CompartmentBase::handleAxial ) );
	// rxialOut declared above as it is needed in file scope
	static Finfo* raxialShared[] =
	{
		&handleAxial, raxialOut()
	};
	static SharedFinfo raxial( "raxial",
			"This is a raxial shared message between asymmetric compartments. The \n"
			"first entry is a MsgDest for the info coming from the other compt. It \n"
			"expects Vm from the other compt as an arg. The second is a MsgSrc sending \n"
			"Ra and Vm to the raxialFunc of the target compartment. \n"
			"- Handles 'handleAxial' and 'raxialOut' calls.",
			raxialShared, sizeof( raxialShared ) / sizeof( Finfo* )
	);
	///////////////////////////////////////////////////////////////////
	// Value Finfos.
	///////////////////////////////////////////////////////////////////

		static ElementValueFinfo< CompartmentBase, double > Vm( "Vm",
			"membrane potential",
			&CompartmentBase::setVm,
			&CompartmentBase::getVm
		);
		static ElementValueFinfo< CompartmentBase, double > Cm( "Cm",
			"Membrane capacitance",
			 &CompartmentBase::setCm,
			&CompartmentBase::getCm
		);
		static ElementValueFinfo< CompartmentBase, double > Em( "Em",
			"Resting membrane potential",
			 &CompartmentBase::setEm,
			&CompartmentBase::getEm
		);
		static ReadOnlyElementValueFinfo< CompartmentBase, double > Im( "Im",
			"Current going through membrane",
			&CompartmentBase::getIm
		);
		static ElementValueFinfo< CompartmentBase, double > inject( "inject",
			"Current injection to deliver into compartment",
			&CompartmentBase::setInject,
			&CompartmentBase::getInject
		);
		static ElementValueFinfo< CompartmentBase, double > initVm( "initVm",
			"Initial value for membrane potential",
			&CompartmentBase::setInitVm,
			&CompartmentBase::getInitVm
		);
		static ElementValueFinfo< CompartmentBase, double > Rm( "Rm",
			"Membrane resistance",
			&CompartmentBase::setRm,
			&CompartmentBase::getRm
		);
		static ElementValueFinfo< CompartmentBase, double > Ra( "Ra",
			"Axial resistance of compartment",
			&CompartmentBase::setRa,
			&CompartmentBase::getRa
		);
		static ValueFinfo< CompartmentBase, double > diameter( "diameter",
			"Diameter of compartment",
			&CompartmentBase::setDiameter,
			&CompartmentBase::getDiameter
		);
		static ValueFinfo< CompartmentBase, double > length( "length",
			"Length of compartment",
			&CompartmentBase::setLength,
			&CompartmentBase::getLength
		);
		static ValueFinfo< CompartmentBase, double > x0( "x0",
			"X coordinate of start of compartment",
			&CompartmentBase::setX0,
			&CompartmentBase::getX0
		);
		static ValueFinfo< CompartmentBase, double > y0( "y0",
			"Y coordinate of start of compartment",
			&CompartmentBase::setY0,
			&CompartmentBase::getY0
		);
		static ValueFinfo< CompartmentBase, double > z0( "z0",
			"Z coordinate of start of compartment",
			&CompartmentBase::setZ0,
			&CompartmentBase::getZ0
		);
		static ValueFinfo< CompartmentBase, double > x( "x",
			"x coordinate of end of compartment",
			&CompartmentBase::setX,
			&CompartmentBase::getX
		);
		static ValueFinfo< CompartmentBase, double > y( "y",
			"y coordinate of end of compartment",
			&CompartmentBase::setY,
			&CompartmentBase::getY
		);
		static ValueFinfo< CompartmentBase, double > z( "z",
			"z coordinate of end of compartment",
			&CompartmentBase::setZ,
			&CompartmentBase::getZ
		);

	//////////////////////////////////////////////////////////////////
	// DestFinfo definitions
	//////////////////////////////////////////////////////////////////
		static DestFinfo injectMsg( "injectMsg",
			"The injectMsg corresponds to the INJECT message in the "
			"GENESIS compartment. Unlike the 'inject' field, any value "
			"assigned by handleInject applies only for a single timestep."
			"So it needs to be updated every dt for a steady (or varying)"
			"injection current",
			new EpFunc1< CompartmentBase,  double >( &CompartmentBase::injectMsg )
		);

		static DestFinfo randInject( "randInject",
			"Sends a random injection current to the compartment. Must be"
			"updated each timestep."
			"Arguments to randInject are probability and current.",
			new EpFunc2< CompartmentBase, double, double > (
				&CompartmentBase::randInject ) );

		static DestFinfo cable( "cable",
			"Message for organizing compartments into groups, called"
			"cables. Doesn't do anything.",
			new OpFunc0< CompartmentBase >( &CompartmentBase::cable )
		);
		static DestFinfo displace( "displace",
			"Displaces compartment by specified vector",
			new OpFunc3< CompartmentBase, double, double, double>(
				   	&CompartmentBase::displace )
		);
		static DestFinfo setGeomAndElec( "setGeomAndElec",
			"Assigns length and dia and accounts for any electrical "
			"scaling needed as a result.",
			new EpFunc2< CompartmentBase, double, double>(
				   	&CompartmentBase::setGeomAndElec )
		);
	///////////////////////////////////////////////////////////////////
	static Finfo* compartmentFinfos[] =
	{
		&Vm,				// Value
		&Cm,				// Value
		&Em,				// Value
		&Im,				// ReadOnlyValue
		&inject,			// Value
		&initVm,			// Value
		&Rm,				// Value
		&Ra,				// Value
		&diameter,			// Value
		&length,			// Value
		&x0,				// Value
		&y0,				// Value
		&z0,				// Value
		&x,					// Value
		&y,					// Value
		&z,					// Value
		&injectMsg,			// DestFinfo
		&randInject,		// DestFinfo
		&injectMsg,			// DestFinfo
		&cable,				// DestFinfo
		&displace,			// DestFinfo
		&setGeomAndElec,	// DestFinfo
		&proc,				// SharedFinfo
		&init,				// SharedFinfo
		&channel,			// SharedFinfo
		&axial,				// SharedFinfo
		&raxial				// SharedFinfo
	};

	static string doc[] =
	{
		"Name", "CompartmentBase",
		"Author", "Upi Bhalla",
		"Description", "CompartmentBase object, for branching neuron models.",
	};
        static ZeroSizeDinfo< int > dinfo;
	static Cinfo compartmentCinfo(
				"CompartmentBase",
				Neutral::initCinfo(),
				compartmentFinfos,
				sizeof( compartmentFinfos ) / sizeof( Finfo* ),
				&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &compartmentCinfo;
}

static const Cinfo* compartmentBaseCinfo = CompartmentBase::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the CompartmentBase class functions.
//////////////////////////////////////////////////////////////////

CompartmentBase::CompartmentBase()
{
	x_ = 0.0;
	y_ = 0.0;
	z_ = 0.0;
	x0_ = 0.0;
	y0_ = 0.0;
	z0_ = 0.0;
	diameter_ = 0.0;
	length_ = 0.0;
}

CompartmentBase::~CompartmentBase()
{
	;
}

bool CompartmentBase::rangeWarning( const string& field, double value )
{
	if ( value < RANGE ) {
		cout << "Warning: Ignored attempt to set " << field <<
				" of compartment " <<
				// c->target().e->name() <<
				" to " << value << " as it is less than " << RANGE << endl;
		return 1;
	}
	return 0;
}

// Value Field access function definitions.
void CompartmentBase::setVm( const Eref& e, double Vm )
{
	vSetVm( e, Vm );
}

double CompartmentBase::getVm( const Eref& e ) const
{
	return vGetVm( e );
}

void CompartmentBase::setEm( const Eref& e, double Em )
{
	vSetEm( e, Em );
}

double CompartmentBase::getEm( const Eref& e ) const
{
	return vGetEm( e );
}

void CompartmentBase::setCm( const Eref& e, double Cm )
{
	if ( rangeWarning( "Cm", Cm ) ) return;
	vSetCm( e, Cm );
}

double CompartmentBase::getCm( const Eref& e ) const
{
	return vGetCm( e );
}

void CompartmentBase::setRm( const Eref& e, double Rm )
{
	if ( rangeWarning( "Rm", Rm ) ) return;
	vSetRm( e, Rm );
}

double CompartmentBase::getRm( const Eref& e ) const
{
	return vGetRm( e );
}

void CompartmentBase::setRa( const Eref& e, double Ra )
{
	if ( rangeWarning( "Ra", Ra ) ) return;
	vSetRa( e, Ra );
}

double CompartmentBase::getRa( const Eref& e ) const
{
	return vGetRa( e );
}

double CompartmentBase::getIm( const Eref& e ) const
{
	return vGetIm( e );
}

void CompartmentBase::setInject( const Eref& e, double Inject )
{
	vSetInject( e, Inject );
}

double CompartmentBase::getInject( const Eref& e ) const
{
	return vGetInject( e );
}

void CompartmentBase::setInitVm( const Eref& e, double initVm )
{
	vSetInitVm( e, initVm );
}

double CompartmentBase::getInitVm( const Eref& e ) const
{
	return vGetInitVm( e );
}

void CompartmentBase::setDiameter( double value )
{
	diameter_ = value;
}

double CompartmentBase::getDiameter() const
{
	return diameter_;
}

void CompartmentBase::setLength( double value )
{
	// If length is assigned correctly, also redo the end coords to match.
	if ( value > 0 && length_ > 0 &&
			doubleEq( length_ * length_,
			(x_-x0_)*(x_-x0_) + (y_-y0_)*(y_-y0_) + (z_-z0_)*(z_-z0_) ) ) {
		double ratio = value / length_;
		x_ = x0_ + ratio * ( x_ - x0_ );
		y_ = y0_ + ratio * ( y_ - y0_ );
		z_ = z0_ + ratio * ( z_ - z0_ );
	}

	length_ = value;
}

double CompartmentBase::getLength() const
{
	return length_;
}

void CompartmentBase::updateLength()
{
	length_ = sqrt( (x_-x0_)*(x_-x0_) +
					(y_-y0_)*(y_-y0_) + (z_-z0_)*(z_-z0_) );
}

void CompartmentBase::setX0( double value )
{
	x0_ = value;
	updateLength();
}

double CompartmentBase::getX0() const
{
	return x0_;
}

void CompartmentBase::setY0( double value )
{
	y0_ = value;
	updateLength();
}

double CompartmentBase::getY0() const
{
	return y0_;
}

void CompartmentBase::setZ0( double value )
{
	z0_ = value;
	updateLength();
}

double CompartmentBase::getZ0() const
{
	return z0_;
}

void CompartmentBase::setX( double value )
{
	x_ = value;
	updateLength();
}

double CompartmentBase::getX() const
{
	return x_;
}

void CompartmentBase::setY( double value )
{
	y_ = value;
	updateLength();
}

double CompartmentBase::getY() const
{
	return y_;
}

void CompartmentBase::setZ( double value )
{
	z_ = value;
	updateLength();
}

double CompartmentBase::getZ() const
{
	return z_;
}

//////////////////////////////////////////////////////////////////
// CompartmentBase::Dest function definitions.
//////////////////////////////////////////////////////////////////

void CompartmentBase::process( const Eref& e, ProcPtr p )
{
	vProcess( e, p );
}

void CompartmentBase::reinit(  const Eref& e, ProcPtr p )
{
	vReinit( e, p );
}

void CompartmentBase::initProc( const Eref& e, ProcPtr p )
{
	vInitProc( e, p );
}

void CompartmentBase::initReinit( const Eref& e, ProcPtr p )
{
	vInitReinit( e, p );
}

void CompartmentBase::handleChannel( const Eref& e, double Gk, double Ek)
{
	vHandleChannel( e, Gk, Ek );
}

void CompartmentBase::handleRaxial( double Ra, double Vm)
{
	vHandleRaxial( Ra, Vm );
}

void CompartmentBase::handleAxial( double Vm)
{
	vHandleAxial( Vm );
}

void CompartmentBase::injectMsg( const Eref& e, double current)
{
	vInjectMsg( e, current );
}

void CompartmentBase::randInject( const Eref& e, double prob, double current)
{
	vRandInject( e, prob, current );
}

void CompartmentBase::cable()
{
	;
}

void CompartmentBase::displace( double dx, double dy, double dz )
{
	x0_ += dx;
	x_ += dx;
	y0_ += dy;
	y_ += dy;
	z0_ += dz;
	z_ += dz;
}

static bool hasScaleFormula( const Eref& e ) {
	vector< Id > kids;
	Neutral::children( e, kids );
	for ( vector< Id >::iterator j = kids.begin(); j != kids.end(); j++ )
		if ( j->element()->getName() == "scaleFormula" )
			return true;
	return false;
}

void CompartmentBase::setGeomAndElec( const Eref& e,
				double len, double dia )
{
	if ( length_ > 0 && diameter_ > 0 && len > 0 && dia > 0 &&
			doubleEq( length_ * length_,
			(x_-x0_)*(x_-x0_) + (y_-y0_)*(y_-y0_) + (z_-z0_)*(z_-z0_) ) ) {
		vSetRm( e, vGetRm( e ) * diameter_ * length_ / ( dia * len ) );
		vSetCm( e, vGetCm( e ) * dia * len / ( diameter_ * length_ ) );
		vSetRa( e, vGetRa( e ) * len * (diameter_ * diameter_) /
				( length_ * dia * dia ) );
		// Rescale channel Gbars here
		vector< ObjId > chans;
		allChildren( e.objId(), ALLDATA, "ISA=ChanBase", chans );
		for ( unsigned int i = 0; i < chans.size(); ++i ) {
			if ( hasScaleFormula( chans[i].eref() ) )
				continue; // Later we will eval the formula with len and dia
			double gbar = Field< double >::get( chans[i], "Gbar" );
			gbar *= len * dia / ( length_ * diameter_ );
			Field< double >::set( chans[i], "Gbar", gbar );
		}
		// Rescale CaConc sizes here
		vector< ObjId > concs;
		allChildren( e.objId(), ALLDATA, "ISA=CaConcBase", concs );
		for ( unsigned int i = 0; i < concs.size(); ++i ) {
			Field< double >::set( concs[i], "length", len );
			Field< double >::set( concs[i], "diameter", dia );
		}

		setLength( len );
		setDiameter( dia );
	}
}

//////////////////////////////////////////////////////////////////
// CompartmentBase::Zombification.
//////////////////////////////////////////////////////////////////

// Dummy instantiation of vSetSolve, does nothing
void CompartmentBase::vSetSolver( const Eref& e, Id hsolve )
{;}

// static func
void CompartmentBase::zombify( Element* orig, const Cinfo* zClass,
				Id hsolve )
{
	if ( orig->cinfo() == zClass )
		return;
	unsigned int start = orig->localDataStart();
	unsigned int num = orig->numLocalData();
	if ( num == 0 )
		return;
	vector< CompartmentDataHolder > cdh( num );

	for ( unsigned int i = 0; i < num; ++i ) {
		Eref er( orig, i + start );
		const CompartmentBase* cb =
			reinterpret_cast< const CompartmentBase* >( er.data() );
		cdh[i].readData( cb, er );
	}
	orig->zombieSwap( zClass );
	for ( unsigned int i = 0; i < num; ++i ) {
		Eref er( orig, i + start );
		CompartmentBase* cb = reinterpret_cast< CompartmentBase* >( er.data() );
		cb->vSetSolver( er, hsolve );
		cdh[i].writeData( cb, er );
	}
}

/////////////////////////////////////////////////////////////////////
