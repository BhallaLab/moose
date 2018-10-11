/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#include "../basecode/header.h"
#include "DifShellBase.h"
#include "DifShell.h"


const double DifShell::EPSILON = 1.0e-10;
const double DifShell::F = 96485.3415; /* C / mol like in genesis */

const Cinfo* DifShell::initCinfo()
{


  static string doc[] =
    {
      "Name", "DifShell",
      "Author", "Niraj Dudani. Ported to async13 by Subhasis Ray. Rewritten by Asia Jedrzejewska-Szmek",
      "Description", "DifShell object: Models diffusion of an ion (typically calcium) within an "
      "electric compartment. A DifShell is an iso-concentration region with respect to "
      "the ion. Adjoining DifShells exchange flux of this ion, and also keep track of "
      "changes in concentration due to pumping, buffering and channel currents, by "
      "talking to the appropriate objects.",
    };
  static Dinfo< DifShell > dinfo;
  static Cinfo difShellCinfo(
			     "DifShell",
			     DifShellBase::initCinfo(),
			     0,
			     0,
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


DifShell::DifShell() :
  dCbyDt_( 0.0 ),
  Cmultiplier_(0.0),
  C_( 0.0 ),
  Ceq_( 0.0 ),
  prevC_(0.0),
  D_( 0.0 ),
  valence_( 0.0 ),
  leak_( 0.0 ),
  shapeMode_(0),
  length_(0),
  diameter_(0),
  thickness_(0),
  volume_(0),
  outerArea_(0),
  innerArea_(0)
{ ; }

////////////////////////////////////////////////////////////////////////////////
// Field access functions
////////////////////////////////////////////////////////////////////////////////
/// C is a read-only field


void DifShell::vSetC( const Eref& e, double C)
{
  if ( C < 0.0 ) {
    cerr << "Error: DifShell: C cannot be negative!\n";
    return;
  }

  C_ = C;
  prevC_ = C_;
}
double DifShell::vGetC(const Eref& e) const
{
  return C_;
}

void DifShell::vSetCeq( const Eref& e, double Ceq )
{
  if ( Ceq < 0.0 ) {
    cerr << "Error: DifShell: Ceq cannot be negative!\n";
    return;
  }

  Ceq_ = Ceq;
  prevC_ = Ceq;
  C_ = Ceq;
}

double DifShell::vGetCeq(const Eref& e ) const
{
  return Ceq_;
}

void DifShell::vSetD(const Eref& e, double D )
{
  if ( D < 0.0 ) {
    cerr << "Error: DifShell: D cannot be negative!\n";
    return;
  }

  D_ = D;
}

double DifShell::vGetD(const Eref& e ) const
{
  return D_;
}

void DifShell::vSetValence(const Eref& e, double valence )
{
  if ( valence < 0.0 ) {
    cerr << "Error: DifShell: valence cannot be negative!\n";
    return;
  }

  valence_ = valence;
}

double DifShell::vGetValence(const Eref& e ) const
{
  return valence_;
}

void DifShell::vSetLeak(const Eref& e, double leak )
{
  leak_ = leak;
}

double DifShell::vGetLeak(const Eref& e ) const
{
  return leak_;
}


void DifShell::vSetShapeMode(const Eref& e, unsigned int shapeMode )
{
  if ( shapeMode != 0 && shapeMode != 1 && shapeMode != 3 ) {
    cerr << "Error: DifShell: I only understand shapeModes 0, 1 and 3.\n";
    return;
  }
  shapeMode_ = shapeMode;
  calculateVolumeArea(e);
}

unsigned int DifShell::vGetShapeMode(const Eref& e) const
{
  return shapeMode_;
}

void DifShell::vSetLength(const Eref& e, double length )
{
  if ( length < 0.0 ) {
    cerr << "Error: DifShell: length cannot be negative!\n";
    return;
  }

  length_ = length;
  calculateVolumeArea(e);
}

double DifShell::vGetLength(const Eref& e ) const
{
  return length_;
}

void DifShell::vSetDiameter(const Eref& e, double diameter )
{
  if ( diameter < 0.0 ) {
    cerr << "Error: DifShell: diameter cannot be negative!\n";
    return;
  }

  diameter_ = diameter;
  calculateVolumeArea(e);
}

double DifShell::vGetDiameter(const Eref& e ) const
{
  return diameter_;
}

void DifShell::vSetThickness( const Eref& e, double thickness )
{
  if ( thickness < 0.0 ) {
    cerr << "Error: DifShell: thickness cannot be negative!\n";
    return;
  }

  thickness_ = thickness;
  calculateVolumeArea(e);
}

double DifShell::vGetThickness(const Eref& e) const
{
  return thickness_;
}

void DifShell::vSetVolume(const Eref&  e, double volume )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifShell: Trying to set volume, when shapeMode is not USER-DEFINED\n";

  if ( volume < 0.0 ) {
    cerr << "Error: DifShell: volume cannot be negative!\n";
    return;
  }

  volume_ = volume;
}

double DifShell::vGetVolume(const Eref& e ) const
{
  return volume_;
}

void DifShell::vSetOuterArea(const Eref& e, double outerArea )
{
  if (shapeMode_ != 3 )
    cerr << "Warning: DifShell: Trying to set outerArea, when shapeMode is not USER-DEFINED\n";

  if ( outerArea < 0.0 ) {
    cerr << "Error: DifShell: outerArea cannot be negative!\n";
    return;
  }

  outerArea_ = outerArea;
}

double DifShell::vGetOuterArea(const Eref& e ) const
{
  return outerArea_;
}

void DifShell::vSetInnerArea(const Eref& e, double innerArea )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifShell: Trying to set innerArea, when shapeMode is not USER-DEFINED\n";

  if ( innerArea < 0.0 ) {
    cerr << "Error: DifShell: innerArea cannot be negative!\n";
    return;
  }

  innerArea_ = innerArea;
}

double DifShell::vGetInnerArea(const Eref& e) const
{
  return innerArea_;
}



////////////////////////////////////////////////////////////////////////////////
// Local dest functions
////////////////////////////////////////////////////////////////////////////////
double DifShell::integrate( double state, double dt, double A, double B )
{
	if ( B > EPSILON ) {
		double x = exp( -B * dt );
		return state * x + ( A / B ) * ( 1 - x );
	}

	return state + A * dt ;
}

void DifShell::calculateVolumeArea(const Eref& e)
{
double rOut = diameter_/2.;

  double rIn = rOut - thickness_;

  if (rIn <0)
	  rIn = 0.;

  switch ( shapeMode_ )
    {
      /*
       * Onion Shell
       */
    case 0:
      if ( length_ == 0.0 ) { // Spherical shell
	volume_ = 4./3.* M_PI * ( rOut * rOut * rOut - rIn * rIn * rIn );
	outerArea_ = 4*M_PI * rOut * rOut;
	innerArea_ = 4*M_PI * rIn * rIn;
      } else { // Cylindrical shell
	volume_ = ( M_PI * length_  ) * ( rOut * rOut - rIn * rIn );
	outerArea_ = 2*M_PI * rOut * length_;
	innerArea_ = 2*M_PI * rIn * length_;
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

void DifShell::vReinit( const Eref& e, ProcPtr p )
{
  dCbyDt_ = leak_;
  Cmultiplier_ = 0;
  calculateVolumeArea(e);

  C_= Ceq_;
  prevC_ = Ceq_;
  concentrationOut()->send( e, C_ );
  innerDifSourceOut()->send( e, prevC_, thickness_ );
  outerDifSourceOut()->send( e, prevC_, thickness_ );

}

void DifShell::vProcess( const Eref & e, ProcPtr p )
{
  /**
   * Send ion concentration and thickness to adjacent DifShells. They will
   * then compute their incoming fluxes.
   */


  C_ = integrate(C_,p->dt,dCbyDt_,Cmultiplier_);

  /**
   * Send ion concentration to ion buffers. They will send back information on
   * the reaction (forward / backward rates ; free / bound buffer concentration)
   * immediately, which this DifShell will use to find amount of ion captured
   * or released in the current time-step.
   */


  dCbyDt_ = leak_;
  Cmultiplier_ = 0;
  prevC_ = C_;

  innerDifSourceOut()->send( e, prevC_, thickness_ );
  outerDifSourceOut()->send( e, prevC_, thickness_ );

  concentrationOut()->send( e, C_ );

}
void DifShell::vBuffer(const Eref& e,
			   double kf,
			   double kb,
			   double bFree,
			   double bBound )
{
  dCbyDt_ += kb * bBound;
  Cmultiplier_ += kf * bFree;
}

void DifShell::vFluxFromOut(const Eref& e, double outerC, double outerThickness )
{
  double diff =2.*  D_ /volume_ *  outerArea_ / (outerThickness + thickness_) ;
  //influx from outer shell
  /**
   * We could pre-compute ( D / Volume ), but let us leave the optimizations
   * for the solver.
   */

  dCbyDt_ +=  diff * outerC;
  Cmultiplier_ += diff ;

}

void DifShell::vFluxFromIn(const Eref& e, double innerC, double innerThickness )
{
  //influx from inner shell
  //double dx = ( innerThickness + thickness_ ) / 2.0;
  double diff = 2.* D_/volume_ * innerArea_ / (innerThickness + thickness_);

  dCbyDt_ +=  diff *  innerC ;
  Cmultiplier_ += diff ;
}

void DifShell::vInflux(const Eref& e,	double I )
{
  /**
   * I: Amperes
   * F_: Faraday's constant: Coulomb / mole
   * valence_: charge on ion: dimensionless
   */
  dCbyDt_ += I / ( F * valence_ * volume_ );

}

/**
 * Same as influx, except subtracting.
 */
void DifShell::vOutflux(const Eref& e, double I )
{
  dCbyDt_ -= I / ( F * valence_ * volume_ );
}

void DifShell::vFInflux(const Eref& e, double I, double fraction )
{
  dCbyDt_ += fraction * I / ( F * valence_ * volume_ );
}

void DifShell::vFOutflux(const Eref& e, double I, double fraction )
{
  dCbyDt_ -= fraction * I / ( F * valence_ * volume_ );
}

void DifShell::vStoreInflux(const Eref& e, double flux )
{
  dCbyDt_ += flux / volume_;
}

void DifShell::vStoreOutflux(const Eref& e, double flux )
{
  dCbyDt_ -= flux / volume_;
}

void DifShell::vTauPump(const Eref& e, double kP, double Ceq )
{
  //dCbyDt_ += -kP * ( C_ - Ceq );
  dCbyDt_ += kP*Ceq;
  Cmultiplier_ -= kP;
}

/**
 * Same as tauPump, except that we use the v value of Ceq.
 */
void DifShell::vEqTauPump(const Eref& e, double kP )
{
  dCbyDt_ += kP*Ceq_;
  Cmultiplier_ -= kP;
}

void DifShell::vMMPump(const Eref& e, double vMax, double Kd )
{
  Cmultiplier_ += ( vMax / volume_ )  / ( C_ + Kd ) ;

}

void DifShell::vHillPump(const Eref& e, double vMax, double Kd, unsigned int hill )
{
  //This needs fixing
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


