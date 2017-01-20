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
#include "ElementValueFinfo.h"
#include "../utility/numutil.h"
#include <cmath>
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric;
SrcFinfo1< double >* DifShell::concentrationOut()
{
  static SrcFinfo1< double > concentrationOut("concentrationOut",
					      "Sends out concentration");
  return &concentrationOut;
}

SrcFinfo2< double, double >* DifShell::innerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("innerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}

SrcFinfo2< double, double >* DifShell::outerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("outerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}


const Cinfo* DifShell::initCinfo()
{
    
  static DestFinfo process( "process",
			    "Handles process call",
			    new ProcOpFunc< DifShell >(  &DifShell::process ) );
  static DestFinfo reinit( "reinit",
			   "Reinit happens only in stage 0",
			   new ProcOpFunc< DifShell >( &DifShell::reinit ));
    
  static Finfo* processShared[] = {
    &process,
    &reinit
  };

  static SharedFinfo proc(
			  "proc", 
			  "Here we create 2 shared finfos to attach with the Ticks. This is because we want to perform DifShell "
			  "computations in 2 stages, much as in the Compartment object. "
			  "In the first stage we send out the concentration value to other DifShells and Buffer elements. We also "
			  "receive fluxes and currents and sum them up to compute ( dC / dt ). "
			  "In the second stage we find the new C value using an explicit integration method. "
			  "This 2-stage procedure eliminates the need to store and send prev_C values, as was common in GENESIS.",
			  processShared,
			  sizeof( processShared ) / sizeof( Finfo* ));
	
  // static DestFinfo process1( "process",
  //                            "Handle process call",
  //                            new ProcOpFunc< DifShell >( &DifShell::process1 ) );
  // static DestFinfo reinit1( "reinit", 
  //                           "Reinit happens only in stage 0",
  //                           new ProcOpFunc< DifShell >( &DifShell::reinit1)
  //                           );
  // static Finfo* processShared_1[] = {
  //     &process1, &reinit1        
  // };
    
  // static SharedFinfo process_1( "process_1",
  //                               "Second process call",
  //                               processShared_1,
  //                               sizeof( processShared_1 ) / sizeof( Finfo* ) );
	
    
  static DestFinfo reaction( "reaction",
			     "Here the DifShell receives reaction rates (forward and backward), and concentrations for the "
			     "free-buffer and bound-buffer molecules.",
			     new EpFunc4< DifShell, double, double, double, double >( &DifShell::buffer ));
    
  static Finfo* bufferShared[] = {
    DifShell::concentrationOut(), &reaction
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
				new EpFunc2< DifShell, double, double > ( &DifShell::fluxFromOut ));
 
  static Finfo* innerDifShared[] = {
    &fluxFromOut,
    DifShell::innerDifSourceOut(),
  
  };
  static SharedFinfo innerDif( "innerDif",
			       "This shared message (and the next) is between DifShells: adjoining shells exchange information to "
			       "find out the flux between them. "
			       "Using this message, an inner shell sends to, and receives from its outer shell." ,
			       innerDifShared,
			       sizeof( innerDifShared ) / sizeof( Finfo* ));

  static DestFinfo fluxFromIn( "fluxFromIn", "",
			       new EpFunc2< DifShell, double, double> ( &DifShell::fluxFromIn ) );
    
  static Finfo* outerDifShared[] = {
    &fluxFromIn,
    DifShell::outerDifSourceOut(),
  };

  static  SharedFinfo outerDif( "outerDif",
				"Using this message, an outer shell sends to, and receives from its inner shell." ,
				outerDifShared,
				sizeof( outerDifShared ) / sizeof( Finfo* ));
  
  //Should be changed it to ElementValueFinfo, to correctly set initial
  //conditions (AJS)
 
  static ElementValueFinfo< DifShell, double> C( "C", 
						 "Concentration C",// is computed by the DifShell",
						 &DifShell::setC,
						 &DifShell::getC);
  static ElementValueFinfo< DifShell, double> Ceq( "Ceq", "",
						   &DifShell::setCeq,
						   &DifShell::getCeq);
  static ElementValueFinfo< DifShell, double> D( "D", "",
						 &DifShell::setD,
						 &DifShell::getD);
  static ElementValueFinfo< DifShell, double> valence( "valence", "",
						       &DifShell::setValence,
						       &DifShell::getValence);
  static ElementValueFinfo< DifShell, double> leak( "leak", "",
						    &DifShell::setLeak,
						    &DifShell::getLeak);
  static ElementValueFinfo< DifShell, unsigned int> shapeMode( "shapeMode", "",
							       &DifShell::setShapeMode,
							       &DifShell::getShapeMode);
  static ElementValueFinfo< DifShell, double> length( "length", "",
						      &DifShell::setLength,
						      &DifShell::getLength);
  static ElementValueFinfo< DifShell, double> diameter( "diameter", "",
							&DifShell::setDiameter,
							&DifShell::getDiameter );
  static ElementValueFinfo< DifShell, double> thickness( "thickness", "",
							 &DifShell::setThickness,
							 &DifShell::getThickness );
  static ElementValueFinfo< DifShell, double> volume( "volume", "",
						      &DifShell::setVolume,
						      &DifShell::getVolume );
  static ElementValueFinfo< DifShell, double> outerArea( "outerArea", "",
							 &DifShell::setOuterArea,
							 &DifShell::getOuterArea);
  static ElementValueFinfo< DifShell, double> innerArea( "innerArea", "",
							 &DifShell::setInnerArea,
							 &DifShell::getInnerArea );

    
  static DestFinfo influx( "influx", "",
			   new EpFunc1< DifShell, double > (&DifShell::influx ));
  static DestFinfo outflux( "outflux", "",
			    new EpFunc1< DifShell, double >( &DifShell::influx ));
  static DestFinfo fInflux( "fInflux", "",
			    new EpFunc2< DifShell, double, double >( &DifShell::fInflux ));
  static DestFinfo fOutflux( "fOutflux", "",
			     new EpFunc2< DifShell, double, double> (&DifShell::fOutflux ));
  static DestFinfo storeInflux( "storeInflux", "",
				new EpFunc1< DifShell, double >( &DifShell::storeInflux ) );
  static DestFinfo storeOutflux( "storeOutflux", "",
				 new EpFunc1< DifShell, double > ( &DifShell::storeOutflux ) );
  static DestFinfo tauPump( "tauPump","",
			    new EpFunc2< DifShell, double, double >( &DifShell::tauPump ) );
  static DestFinfo eqTauPump( "eqTauPump", "",
			      new EpFunc1< DifShell, double >( &DifShell::eqTauPump ) );
  static DestFinfo mmPump( "mmPump", "",
			   new EpFunc2< DifShell, double, double >( &DifShell::mmPump ) );
  static DestFinfo hillPump( "hillPump", "",
			     new EpFunc3< DifShell, double, double, unsigned int >( &DifShell::hillPump ) );
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
    &proc,
    &buffer,
    // concentrationOut(),
    //innerDifSourceOut(),
    //outerDifSourceOut(),
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

//Cinfo *object*  corresponding to the class.
static const Cinfo* difShellCinfo = DifShell::initCinfo();

////////////////////////////////////////////////////////////////////////////////
// Class functions
////////////////////////////////////////////////////////////////////////////////

/// Faraday's constant (Coulomb / Mole)
const double DifShell::F_ = 96485.3415; /* C / mol like in genesis */

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


void DifShell::setC( const Eref& e, double C)
{
  if ( C < 0.0 ) {
    cerr << "Error: DifShell: C cannot be negative!\n";
    return;
  }
	
  C_ = C;
}
double DifShell::getC(const Eref& e) const
{
  return C_;
}

void DifShell::setCeq( const Eref& e, double Ceq )
{
  if ( Ceq < 0.0 ) {
    cerr << "Error: DifShell: Ceq cannot be negative!\n";
    return;
  }
	
  Ceq_ = Ceq;
}

double DifShell::getCeq(const Eref& e ) const
{
  return Ceq_;
}

void DifShell::setD(const Eref& e, double D )
{
  if ( D < 0.0 ) {
    cerr << "Error: DifShell: D cannot be negative!\n";
    return;
  }
	
  D_ = D;
}

double DifShell::getD(const Eref& e ) const
{
  return D_;
}

void DifShell::setValence(const Eref& e, double valence )
{
  if ( valence < 0.0 ) {
    cerr << "Error: DifShell: valence cannot be negative!\n";
    return;
  }
	
  valence_ = valence;
}

double DifShell::getValence(const Eref& e ) const 
{
  return valence_;
}

void DifShell::setLeak(const Eref& e, double leak )
{
  leak_ = leak;
}

double DifShell::getLeak(const Eref& e ) const
{
  return leak_;
}

void DifShell::setShapeMode(const Eref& e, unsigned int shapeMode )
{
  if ( shapeMode != 0 && shapeMode != 1 && shapeMode != 3 ) {
    cerr << "Error: DifShell: I only understand shapeModes 0, 1 and 3.\n";
    return;
  }
  shapeMode_ = shapeMode;
}

unsigned int DifShell::getShapeMode(const Eref& e) const
{
  return shapeMode_;
}

void DifShell::setLength(const Eref& e, double length )
{
  if ( length < 0.0 ) {
    cerr << "Error: DifShell: length cannot be negative!\n";
    return;
  }
	
  length_ = length;
}

double DifShell::getLength(const Eref& e ) const
{
  return length_;
}

void DifShell::setDiameter(const Eref& e, double diameter )
{
  if ( diameter < 0.0 ) {
    cerr << "Error: DifShell: diameter cannot be negative!\n";
    return;
  }
	
  diameter_ = diameter;
}

double DifShell::getDiameter(const Eref& e ) const
{
  return diameter_;
}

void DifShell::setThickness( const Eref& e, double thickness )
{
  if ( thickness < 0.0 ) {
    cerr << "Error: DifShell: thickness cannot be negative!\n";
    return;
  }
	
  thickness_ = thickness;
}

double DifShell::getThickness(const Eref& e) const
{
  return thickness_;
}

void DifShell::setVolume(const Eref& e, double volume )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifShell: Trying to set volume, when shapeMode is not USER-DEFINED\n";
	
  if ( volume < 0.0 ) {
    cerr << "Error: DifShell: volume cannot be negative!\n";
    return;
  }
	
  volume_ = volume;
}

double DifShell::getVolume(const Eref& e ) const
{
  return volume_;
}

void DifShell::setOuterArea(const Eref& e, double outerArea )
{
  if (shapeMode_ != 3 )
    cerr << "Warning: DifShell: Trying to set outerArea, when shapeMode is not USER-DEFINED\n";
	
  if ( outerArea < 0.0 ) {
    cerr << "Error: DifShell: outerArea cannot be negative!\n";
    return;
  }
	
  outerArea_ = outerArea;
}

double DifShell::getOuterArea(const Eref& e ) const
{
  return outerArea_;
}

void DifShell::setInnerArea(const Eref& e, double innerArea )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifShell: Trying to set innerArea, when shapeMode is not USER-DEFINED\n";
    
  if ( innerArea < 0.0 ) {
    cerr << "Error: DifShell: innerArea cannot be negative!\n";
    return;
  }
    
  innerArea_ = innerArea;
}

double DifShell::getInnerArea(const Eref& e) const
{
  return innerArea_;
}

////////////////////////////////////////////////////////////////////////////////
// Dest functions
////////////////////////////////////////////////////////////////////////////////

void DifShell::reinit( const Eref& e, ProcPtr p )
{
  localReinit_0( e, p );
}

void DifShell::process( const Eref& e, ProcPtr p )
{
  localProcess_0( e, p );
}
/*
  void DifShell::process_1(const Eref& e, ProcPtr p )
  {
  localProcess_1( e, p );
  }

  void DifShell::reinit_1(const Eref& e, ProcPtr p )
  {
  ;
  }
*/
void DifShell::buffer(
		      const Eref& e,
		      double kf,
		      double kb,
		      double bFree,
		      double bBound )
{
  localBuffer( e, kf, kb, bFree, bBound );
}

void DifShell::fluxFromOut(const Eref& e,
			   double outerC,
			   double outerThickness )
{
  localFluxFromOut(e, outerC, outerThickness );
}

void DifShell::fluxFromIn(
			  const Eref& e,
			  double innerC,
			  double innerThickness )
{
  localFluxFromIn( e, innerC, innerThickness );
}

void DifShell::influx(const Eref& e,
		      double I )
{
  localInflux( e, I );
}

void DifShell::outflux(const Eref& e,
		       double I )
{
  localOutflux(e, I );
}

void DifShell::fInflux(const Eref& e,
		       double I,
		       double fraction )
{
  localFInflux(e, I, fraction );
}

void DifShell::fOutflux(const Eref& e,
			double I,
			double fraction )
{
  localFOutflux(e, I, fraction );
}

void DifShell::storeInflux(const Eref& e,
			   double flux )
{
  localStoreInflux( e, flux );
}

void DifShell::storeOutflux(const Eref& e,
			    double flux )
{
  localStoreOutflux(e, flux );
}

void DifShell::tauPump(const Eref& e,
		       double kP,
		       double Ceq )
{
  localTauPump( e, kP, Ceq );
}

void DifShell::eqTauPump(const Eref& e,
			 double kP )
{
  localEqTauPump(e, kP );
}

void DifShell::mmPump(const Eref& e,
		      double vMax,
		      double Kd )
{
  localMMPump(e, vMax, Kd );
}

void DifShell::hillPump(const Eref& e,
			double vMax,
			double Kd,
			unsigned int hill )
{
  localHillPump(e, vMax, Kd, hill );
}

////////////////////////////////////////////////////////////////////////////////
// Local dest functions
////////////////////////////////////////////////////////////////////////////////

void DifShell::localReinit_0( const Eref& e, ProcPtr p )
{
  dCbyDt_ = leak_;
  C_ = Ceq_;
  const double dOut = diameter_;
  const double dIn = diameter_ - thickness_;
	
  switch ( shapeMode_ )
    {
      /*
       * Onion Shell
       */
    case 0:
      if ( length_ == 0.0 ) { // Spherical shell
	volume_ = ( M_PI / 6.0 ) * ( dOut * dOut * dOut - dIn * dIn * dIn );
	outerArea_ = M_PI * dOut * dOut;
	innerArea_ = M_PI * dIn * dIn;
      } else { // Cylindrical shell
	volume_ = ( M_PI * length_ / 4.0 ) * ( dOut * dOut - dIn * dIn );
	outerArea_ = M_PI * dOut * length_;
	innerArea_ = M_PI * dIn * length_;
      }
		
      break;
	
      /*
       * Cylindrical Slice
       */
    case 1:
      volume_ = M_PI * diameter_ * diameter_ * thickness_ / 4.0;
      outerArea_ = M_PI * diameter_ * diameter_ / 4.0;
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
  C_ += dCbyDt_ * p->dt;
  
  /**
   * Send ion concentration to ion buffers. They will send back information on
   * the reaction (forward / backward rates ; free / bound buffer concentration)
   * immediately, which this DifShell will use to find amount of ion captured
   * or released in the current time-step.
   */
  concentrationOut()->send( e, C_ );
  dCbyDt_ = leak_;
}
/*
  void DifShell::localProcess_1( const Eref& e, ProcPtr p )
  {
  C_ += dCbyDt_ * p->dt;
  dCbyDt_ = leak_;
  }
*/
void DifShell::localBuffer(const Eref& e,
			   double kf,
			   double kb,
			   double bFree,
			   double bBound )
{
  dCbyDt_ += -kf * bFree * C_ + kb * bBound;
}

void DifShell::localFluxFromOut(const Eref& e, double outerC, double outerThickness )
{
  double dx = ( outerThickness + thickness_ ) / 2.0;
  //influx from outer shell
  /**
   * We could pre-compute ( D / Volume ), but let us leave the optimizations
   * for the solver.
   */
  dCbyDt_ += ( D_ / volume_ ) * ( outerArea_ / dx ) * ( outerC - C_ );
}

void DifShell::localFluxFromIn(const Eref& e, double innerC, double innerThickness )
{
  //influx from inner shell
  double dx = ( innerThickness + thickness_ ) / 2.0;
	
  dCbyDt_ += ( D_ / volume_ ) * ( innerArea_ / dx ) * ( innerC - C_ );
}

void DifShell::localInflux(const Eref& e,	double I )
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
void DifShell::localOutflux(const Eref& e, double I )
{
  dCbyDt_ -= I / ( F_ * valence_ * volume_ );
}

void DifShell::localFInflux(const Eref& e, double I, double fraction )
{
  dCbyDt_ += fraction * I / ( F_ * valence_ * volume_ );
}

void DifShell::localFOutflux(const Eref& e, double I, double fraction )
{
  dCbyDt_ -= fraction * I / ( F_ * valence_ * volume_ );
}

void DifShell::localStoreInflux(const Eref& e, double flux )
{
  dCbyDt_ += flux / volume_;
}

void DifShell::localStoreOutflux(const Eref& e, double flux )
{
  dCbyDt_ -= flux / volume_;
}

void DifShell::localTauPump(const Eref& e, double kP, double Ceq )
{
  dCbyDt_ += -kP * ( C_ - Ceq );
}

/**
 * Same as tauPump, except that we use the local value of Ceq.
 */
void DifShell::localEqTauPump(const Eref& e, double kP )
{
  dCbyDt_ += -kP * ( C_ - Ceq_ );
}

void DifShell::localMMPump(const Eref& e, double vMax, double Kd )
{
  dCbyDt_ += -( vMax / volume_ ) * ( C_ / ( C_ + Kd ) );
}

void DifShell::localHillPump(const Eref& e, double vMax, double Kd, unsigned int hill )
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


