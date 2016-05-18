/**********************************************************************
** This program is part of 'MOOSE', the
** Multiscale Object Oriented Simulation Environment.
**           copyright (C) 2003-2008
**           Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "DifShell.h"
#include "../utility/numutil.h"
#include <cmath>

static SrcFinfo1< double >* concentrationOut()
{
    static SrcFinfo1< double > concentrationOut("concentrationOut",
                                                "Sends out concentration");
    return &concentrationOut;
}

static SrcFinfo2< double, double >* innerDifSourceOut(){
    static SrcFinfo2< double, double > sourceOut("innerDifSourceOut",
                                         "Sends out source information.");
    return & sourceOut;
}

static SrcFinfo2< double, double >* outerDifSourceOut(){
    static SrcFinfo2< double, double > sourceOut("outerDifSourceOut",
                                         "Sends out source information.");
    return & sourceOut;
}

const Cinfo* DifShell::initCinfo()
{
    
    static DestFinfo process( "process",
                              "Handles process call",
                              new ProcOpFunc< DifShell >(  &DifShell::process_0 ) );
    static DestFinfo reinit( "reinit",
                             "Reinit happens only in stage 0",
                             new ProcOpFunc< DifShell >( &DifShell::reinit_0 ));
    
    static Finfo* processShared_0[] = {
        &process,
        &reinit
    };

    static SharedFinfo process_0(
        "process_0", 
        "Here we create 2 shared finfos to attach with the Ticks. This is because we want to perform DifShell "
        "computations in 2 stages, much as in the Compartment object. "
        "In the first stage we send out the concentration value to other DifShells and Buffer elements. We also "
        "receive fluxes and currents and sum them up to compute ( dC / dt ). "
        "In the second stage we find the new C value using an explicit integration method. "
        "This 2-stage procedure eliminates the need to store and send prev_C values, as was common in GENESIS.",
        processShared_0,
        sizeof( processShared_0 ) / sizeof( Finfo* ));
	
    static DestFinfo process1( "process",
                               "Handle process call",
                               new ProcOpFunc< DifShell >( &DifShell::process_1 ) );
    static DestFinfo reinit1( "reinit", 
                              "Reinit happens only in stage 0",
                              new ProcOpFunc< DifShell >( &DifShell::reinit_1)
                              );
    static Finfo* processShared_1[] = {
        &process1, &reinit1        
    };
    
    static SharedFinfo process_1( "process_1",
                                  "Second process call",
                                  processShared_1,
                                  sizeof( processShared_1 ) / sizeof( Finfo* ) );
	
    
    static DestFinfo reaction( "reaction",
                               "Here the DifShell receives reaction rates (forward and backward), and concentrations for the "
                               "free-buffer and bound-buffer molecules.",
                               new OpFunc4< DifShell, double, double, double, double >( &DifShell::buffer ));
    static Finfo* bufferShared[] = {
        concentrationOut(), &reaction
    };
    static SharedFinfo buffer( "buffer",
                        "This is a shared message from a DifShell to a Buffer (FixBuffer or DifBuffer). "
			"During stage 0:\n "
			" - DifShell sends ion concentration\n"
			" - Buffer updates buffer concentration and sends it back immediately using a call-back.\n"
			" - DifShell updates the time-derivative ( dC / dt ) \n"
                        "\n"
	 		"During stage 1: \n"
			" - DifShell advances concentration C \n\n"
			"This scheme means that the Buffer does not need to be scheduled, and it does its computations when "
			"it receives a cue from the DifShell. May not be the best idea, but it saves us from doing the above "
			"computations in 3 stages instead of 2." ,
                        bufferShared,
			sizeof( bufferShared ) / sizeof( Finfo* ));

    static DestFinfo fluxFromOut( "fluxFromOut",
                                  "Destination message",
                                  new OpFunc2< DifShell, double, double > ( &DifShell::fluxFromOut ));
    
    static Finfo* innerDifShared[] = {
        innerDifSourceOut(),
        &fluxFromOut
    };
    static SharedFinfo innerDif( "innerDif",
                                 "This shared message (and the next) is between DifShells: adjoining shells exchange information to "
                                 "find out the flux between them. "
                                 "Using this message, an inner shell sends to, and receives from its outer shell." ,
                                 innerDifShared,
                                 sizeof( innerDifShared ) / sizeof( Finfo* ));

    static DestFinfo fluxFromIn( "fluxFromIn", "",
                                 new OpFunc2< DifShell, double, double> ( &DifShell::fluxFromIn ) );
    static Finfo* outerDifShared[] = {
        &fluxFromIn,
        outerDifSourceOut(),
    };

    static  SharedFinfo outerDif( "outerDif",
                                  "Using this message, an outer shell sends to, and receives from its inner shell." ,
                                  outerDifShared,
                                  sizeof( outerDifShared ) / sizeof( Finfo* ));

    static ReadOnlyValueFinfo< DifShell, double> C( "C", 
                                 "Concentration C is computed by the DifShell and is read-only",
                                 &DifShell::getC);
    static ValueFinfo< DifShell, double> Ceq( "Ceq", "",
                           &DifShell::setCeq,
                           &DifShell::getCeq);
    static ValueFinfo< DifShell, double> D( "D", "",
                         &DifShell::setD,
                         &DifShell::getD);
    static ValueFinfo< DifShell, double> valence( "valence", "",
                               &DifShell::setValence,
                               &DifShell::getValence);
    static ValueFinfo< DifShell, double> leak( "leak", "",
                            &DifShell::setLeak,
                            &DifShell::getLeak);
    static ValueFinfo< DifShell, unsigned int> shapeMode( "shapeMode", "",
                       &DifShell::setShapeMode,
                       &DifShell::getShapeMode);
    static ValueFinfo< DifShell, double> length( "length", "",
                              &DifShell::setLength,
                              &DifShell::getLength);
    static ValueFinfo< DifShell, double> diameter( "diameter", "",
                       &DifShell::setDiameter,
                       &DifShell::getDiameter );
    static ValueFinfo< DifShell, double> thickness( "thickness", "",
                                 &DifShell::setThickness,
                                 &DifShell::getThickness );
    static ValueFinfo< DifShell, double> volume( "volume", "",
                              &DifShell::setVolume,
                              &DifShell::getVolume );
    static ValueFinfo< DifShell, double> outerArea( "outerArea", "",
                                                    &DifShell::setOuterArea,
                                                    &DifShell::getOuterArea);
    static ValueFinfo< DifShell, double> innerArea( "innerArea", "",
                                 &DifShell::setInnerArea,
                                 &DifShell::getInnerArea );

    
    static DestFinfo influx( "influx", "",
                             new OpFunc1< DifShell, double > (&DifShell::influx ));
    static DestFinfo outflux( "outflux", "",
                              new OpFunc1< DifShell, double >( &DifShell::influx ));
    static DestFinfo fInflux( "fInflux", "",
                              new OpFunc2< DifShell, double, double >( &DifShell::fInflux ));
    static DestFinfo fOutflux( "fOutflux", "",
                               new OpFunc2< DifShell, double, double> (&DifShell::fOutflux ));
    static DestFinfo storeInflux( "storeInflux", "",
                                 new OpFunc1< DifShell, double >( &DifShell::storeInflux ) );
    static DestFinfo storeOutflux( "storeOutflux", "",
                                   new OpFunc1< DifShell, double > ( &DifShell::storeOutflux ) );
    static DestFinfo tauPump( "tauPump","",
                              new OpFunc2< DifShell, double, double >( &DifShell::tauPump ) );
    static DestFinfo eqTauPump( "eqTauPump", "",
                                new OpFunc1< DifShell, double >( &DifShell::eqTauPump ) );
    static DestFinfo mmPump( "mmPump", "",
                             new OpFunc2< DifShell, double, double >( &DifShell::mmPump ) );
    static DestFinfo hillPump( "hillPump", "",
                               new OpFunc3< DifShell, double, double, unsigned int >( &DifShell::hillPump ) );
    static Finfo* difShellFinfos[] = {
	//////////////////////////////////////////////////////////////////
	// Field definitions
	//////////////////////////////////////////////////////////////////
        &C,
        &Ceq,
        &D,
        &valence,
        &leak,
        &shapeMode,
        &length,
        &diameter,
        &thickness,
        &volume,
        &outerArea,
        &innerArea,
	//////////////////////////////////////////////////////////////////
	// MsgSrc definitions
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// SharedFinfo definitions
	//////////////////////////////////////////////////////////////////
        &process_0,
        &process_1,
        &buffer,
        &innerDif,
        &outerDif,
	//////////////////////////////////////////////////////////////////
	// DestFinfo definitions
	//////////////////////////////////////////////////////////////////
        &influx,
        &outflux,
        &fInflux,
        &fOutflux,
        &storeInflux,
        &storeOutflux,
        &tauPump,
        &eqTauPump,
        &mmPump,
        &hillPump,
	};

	static string doc[] =
	{
		"Name", "DifShell",
		"Author", "Niraj Dudani. Ported to async13 by Subhasis Ray.",
		"Description", "DifShell object: Models diffusion of an ion (typically calcium) within an "
				"electric compartment. A DifShell is an iso-concentration region with respect to "
				"the ion. Adjoining DifShells exchange flux of this ion, and also keep track of "
				"changes in concentration due to pumping, buffering and channel currents, by "
				"talking to the appropriate objects.",
	};
        static Dinfo< DifShell > dinfo;
	static Cinfo difShellCinfo(
            "DifShell",
            Neutral::initCinfo(),
            difShellFinfos,
            sizeof( difShellFinfos ) / sizeof( Finfo* ),
            &dinfo,
            doc,
            sizeof( doc ) / sizeof( string ));

	return &difShellCinfo;
}

static const Cinfo* difShellCinfo = DifShell::initCinfo();

////////////////////////////////////////////////////////////////////////////////
// Class functions
////////////////////////////////////////////////////////////////////////////////

/// Faraday's constant (Coulomb / Mole)
const double DifShell::F_ = 0.0;

DifShell::DifShell() :
	dCbyDt_( 0.0 ),
	C_( 0.0 ),
	Ceq_( 0.0 ),
	D_( 0.0 ),
	valence_( 0.0 ),
	leak_( 0.0 ),
	shapeMode_( 0 ),
	length_( 0.0 ),
	diameter_( 0.0 ),
	thickness_( 0.0 ),
	volume_( 0.0 ),
	outerArea_( 0.0 ),
	innerArea_( 0.0 )
{ ; }

////////////////////////////////////////////////////////////////////////////////
// Field access functions
////////////////////////////////////////////////////////////////////////////////
/// C is a read-only field
double DifShell::getC() const
{
	return C_;
}

void DifShell::setCeq( double Ceq )
{
	if ( Ceq < 0.0 ) {
		cerr << "Error: DifShell: Ceq cannot be negative!\n";
		return;
	}
	
	Ceq_ = Ceq;
}

double DifShell::getCeq( ) const
{
	return Ceq_;
}

void DifShell::setD( double D )
{
	if ( D < 0.0 ) {
		cerr << "Error: DifShell: D cannot be negative!\n";
		return;
	}
	
	D_ = D;
}

double DifShell::getD( ) const
{
	return D_;
}

void DifShell::setValence( double valence )
{
	if ( valence < 0.0 ) {
		cerr << "Error: DifShell: valence cannot be negative!\n";
		return;
	}
	
	valence_ = valence;
}

double DifShell::getValence( ) const 
{
	return valence_;
}

void DifShell::setLeak( double leak )
{
    leak_ = leak;
}

double DifShell::getLeak( ) const
{
	return leak_;
}

void DifShell::setShapeMode( unsigned int shapeMode )
{
	if ( shapeMode != 0 && shapeMode != 1 && shapeMode != 3 ) {
		cerr << "Error: DifShell: I only understand shapeModes 0, 1 and 3.\n";
		return;
	}
        shapeMode_ = shapeMode;
}

unsigned int DifShell::getShapeMode() const
{
	return shapeMode_;
}

void DifShell::setLength( double length )
{
	if ( length < 0.0 ) {
		cerr << "Error: DifShell: length cannot be negative!\n";
		return;
	}
	
	length_ = length;
}

double DifShell::getLength( ) const
{
	return length_;
}

void DifShell::setDiameter( double diameter )
{
	if ( diameter < 0.0 ) {
		cerr << "Error: DifShell: diameter cannot be negative!\n";
		return;
	}
	
	diameter_ = diameter;
}

double DifShell::getDiameter( ) const
{
	return diameter_;
}

void DifShell::setThickness( double thickness )
{
	if ( thickness < 0.0 ) {
		cerr << "Error: DifShell: thickness cannot be negative!\n";
		return;
	}
	
	thickness_ = thickness;
}

double DifShell::getThickness() const
{
    return thickness_;
}

void DifShell::setVolume( double volume )
{
	if ( shapeMode_ != 3 )
		cerr << "Warning: DifShell: Trying to set volume, when shapeMode is not USER-DEFINED\n";
	
	if ( volume < 0.0 ) {
		cerr << "Error: DifShell: volume cannot be negative!\n";
		return;
	}
	
	volume_ = volume;
}

double DifShell::getVolume( ) const
{
	return volume_;
}

void DifShell::setOuterArea( double outerArea )
{
    if (shapeMode_ != 3 )
        cerr << "Warning: DifShell: Trying to set outerArea, when shapeMode is not USER-DEFINED\n";
	
    if ( outerArea < 0.0 ) {
        cerr << "Error: DifShell: outerArea cannot be negative!\n";
        return;
    }
	
    outerArea_ = outerArea;
}

double DifShell::getOuterArea( ) const
{
    return outerArea_;
}

void DifShell::setInnerArea( double innerArea )
{
    if ( shapeMode_ != 3 )
        cerr << "Warning: DifShell: Trying to set innerArea, when shapeMode is not USER-DEFINED\n";
    
    if ( innerArea < 0.0 ) {
        cerr << "Error: DifShell: innerArea cannot be negative!\n";
        return;
    }
    
    innerArea_ = innerArea;
}

double DifShell::getInnerArea() const
{
    return innerArea_;
}

////////////////////////////////////////////////////////////////////////////////
// Dest functions
////////////////////////////////////////////////////////////////////////////////

void DifShell::reinit_0( const Eref& e, ProcPtr p )
{
    localReinit_0( e, p );
}

void DifShell::process_0( const Eref& e, ProcPtr p )
{
	localProcess_0( e, p );
}

void DifShell::process_1(const Eref& e, ProcPtr p )
{
    localProcess_1( e, p );
}

void DifShell::reinit_1(const Eref& e, ProcPtr p )
{
    ;
}

void DifShell::buffer(
	double kf,
	double kb,
	double bFree,
	double bBound )
{
    localBuffer( kf, kb, bFree, bBound );
}

void DifShell::fluxFromOut(
	double outerC,
	double outerThickness )
{
		localFluxFromOut( outerC, outerThickness );
}

void DifShell::fluxFromIn(
	double innerC,
	double innerThickness )
{
		localFluxFromIn( innerC, innerThickness );
}

void DifShell::influx(
	double I )
{
    localInflux( I );
}

void DifShell::outflux(
	double I )
{
    localOutflux( I );
}

void DifShell::fInflux(
	double I,
	double fraction )
{
localFInflux( I, fraction );
}

void DifShell::fOutflux(
	double I,
	double fraction )
{
localFOutflux( I, fraction );
}

void DifShell::storeInflux(
	double flux )
{
localStoreInflux( flux );
}

void DifShell::storeOutflux(
	double flux )
{
localStoreOutflux( flux );
}

void DifShell::tauPump(
	double kP,
	double Ceq )
{
localTauPump( kP, Ceq );
}

void DifShell::eqTauPump(
	double kP )
{
localEqTauPump( kP );
}

void DifShell::mmPump(
	double vMax,
	double Kd )
{
localMMPump( vMax, Kd );
}

void DifShell::hillPump(
	double vMax,
	double Kd,
	unsigned int hill )
{
localHillPump( vMax, Kd, hill );
}

////////////////////////////////////////////////////////////////////////////////
// Local dest functions
////////////////////////////////////////////////////////////////////////////////

void DifShell::localReinit_0( const Eref& e, ProcPtr p )
{
	dCbyDt_ = leak_;
	
	double Pi = M_PI;
	double dOut = diameter_;
	double dIn = diameter_ - thickness_;
	
	switch ( shapeMode_ )
	{
	/*
	 * Onion Shell
	 */
	case 0:
		if ( length_ == 0.0 ) { // Spherical shell
			volume_ = ( Pi / 6.0 ) * ( dOut * dOut * dOut - dIn * dIn * dIn );
			outerArea_ = Pi * dOut * dOut;
			innerArea_ = Pi * dIn * dIn;
		} else { // Cylindrical shell
			volume_ = ( Pi * length_ / 4.0 ) * ( dOut * dOut - dIn * dIn );
			outerArea_ = Pi * dOut * length_;
			innerArea_ = Pi * dIn * length_;
		}
		
		break;
	
	/*
	 * Cylindrical Slice
	 */
	case 1:
		volume_ = Pi * diameter_ * diameter_ * thickness_ / 4.0;
		outerArea_ = Pi * diameter_ * diameter_ / 4.0;
		innerArea_ = outerArea_;
		break;
	
	/*
	 * User defined
	 */
	case 3:
		// Nothing to be done here. Volume and inner-, outer areas specified by
		// user.
		break;
	
	default:
		assert( 0 );
	}
}

void DifShell::localProcess_0( const Eref & e, ProcPtr p )
{
	/**
	 * Send ion concentration and thickness to adjacent DifShells. They will
	 * then compute their incoming fluxes.
	 */
    innerDifSourceOut()->send( e, C_, thickness_ );
    outerDifSourceOut()->send( e, C_, thickness_ );
	
	/**
	 * Send ion concentration to ion buffers. They will send back information on
	 * the reaction (forward / backward rates ; free / bound buffer concentration)
	 * immediately, which this DifShell will use to find amount of ion captured
	 * or released in the current time-step.
	 */
    concentrationOut()->send( e, C_ );
}

void DifShell::localProcess_1( const Eref& e, ProcPtr p )
{
	C_ += dCbyDt_ * p->dt;
	dCbyDt_ = leak_;
}

void DifShell::localBuffer(
	double kf,
	double kb,
	double bFree,
	double bBound )
{
	dCbyDt_ += -kf * bFree * C_ + kb * bBound;
}

void DifShell::localFluxFromOut( double outerC, double outerThickness )
{
	double dx = ( outerThickness + thickness_ ) / 2.0;
	
	/**
	 * We could pre-compute ( D / Volume ), but let us leave the optimizations
	 * for the solver.
	 */
	dCbyDt_ += ( D_ / volume_ ) * ( outerArea_ / dx ) * ( outerC - C_ );
}

void DifShell::localFluxFromIn( double innerC, double innerThickness )
{
	double dx = ( innerThickness + thickness_ ) / 2.0;
	
	dCbyDt_ += ( D_ / volume_ ) * ( innerArea_ / dx ) * ( innerC - C_ );
}

void DifShell::localInflux(	double I )
{
	/**
	 * I: Amperes
	 * F_: Faraday's constant: Coulomb / mole
	 * valence_: charge on ion: dimensionless
	 */
	dCbyDt_ += I / ( F_ * valence_ * volume_ );
}

/**
 * Same as influx, except subtracting.
 */
void DifShell::localOutflux( double I )
{
	dCbyDt_ -= I / ( F_ * valence_ * volume_ );
}

void DifShell::localFInflux( double I, double fraction )
{
	dCbyDt_ += fraction * I / ( F_ * valence_ * volume_ );
}

void DifShell::localFOutflux( double I, double fraction )
{
	dCbyDt_ -= fraction * I / ( F_ * valence_ * volume_ );
}

void DifShell::localStoreInflux( double flux )
{
	dCbyDt_ += flux / volume_;
}

void DifShell::localStoreOutflux( double flux )
{
	dCbyDt_ -= flux / volume_;
}

void DifShell::localTauPump( double kP, double Ceq )
{
	dCbyDt_ += -kP * ( C_ - Ceq );
}

/**
 * Same as tauPump, except that we use the local value of Ceq.
 */
void DifShell::localEqTauPump( double kP )
{
	dCbyDt_ += -kP * ( C_ - Ceq_ );
}

void DifShell::localMMPump( double vMax, double Kd )
{
	dCbyDt_ += -( vMax / volume_ ) * ( C_ / ( C_ + Kd ) );
}

void DifShell::localHillPump( double vMax, double Kd, unsigned int hill )
{
	double ch;
	switch( hill )
	{
	case 0:
		ch = 1.0;
		break;
	case 1:
		ch = C_;
		break;
	case 2:
		ch = C_ * C_;
		break;
	case 3:
		ch = C_ * C_ * C_;
		break;
	case 4:
		ch = C_ * C_;
		ch = ch * ch;
		break;
	default:
		ch = pow( C_, static_cast< double >( hill ) );
	};
	
	dCbyDt_ += -( vMax / volume_ ) * ( ch / ( ch + Kd ) );
}

////////////////////////////////////////////////////////////////////////////////


