// DifBuffer.cpp ---
//
// Filename: DifBuffer.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Mon Feb 16 12:02:11 2015 (-0500)
// Version:
// Package-Requires: ()
// Last-Updated: Mon Feb 23 13:07:56 2015 (-0500)
//           By: Subhasis Ray
//     Update #: 130
// URL:
// Doc URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
//
//
//

// Change Log:
// 5/25/16 Completing DifBuffer -- Asia J-Szmek (GMU)
// 9/21/16 rewrote DifBuffer to account for DifBufferBase (AJS)
//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//
//

// Code:

#include "../basecode/header.h"
#include "DifBufferBase.h"
#include "DifBuffer.h"
#include "../basecode/ElementValueFinfo.h"
#include "../utility/numutil.h"
#include <cmath>
const double DifBuffer::EPSILON = 1.0e-10;


const Cinfo * DifBuffer::initCinfo()
{

  static string doc[] = {
    "Name", "DifBuffer",
    "Author", "Subhasis Ray (ported from GENESIS2)",
    "Description", "Models diffusible buffer where total concentration is constant. It is"
    " coupled with a DifShell.",
  };
  static Dinfo<DifBuffer> dinfo;
  static Cinfo difBufferCinfo(
			      "DifBuffer",
			      DifBufferBase::initCinfo(),
			      0,
			      0,
			      &dinfo,
			      doc,
			      sizeof(doc)/sizeof(string));

  return &difBufferCinfo;
}

static const Cinfo * difBufferCinfo = DifBuffer::initCinfo();


////////////////////////////////////////////////////////////////////////////////
// Class functions
////////////////////////////////////////////////////////////////////////////////

DifBuffer::DifBuffer() :
  activation_(0),
  Af_(0),
  Bf_(0),
  bFree_(0),
  bBound_(0),
  prevFree_(0),
  prevBound_(0),
  bTot_(0),
  kf_(0),
  kb_(0),
  D_(0),
  shapeMode_(0),
  length_(0),
  diameter_(0),
  thickness_(0),
  volume_(0),
  outerArea_(0),
  innerArea_(0)

{}

////////////////////////////////////////////////////////////////////////////////
// Field access functions
////////////////////////////////////////////////////////////////////////////////
double DifBuffer::vGetActivation(const Eref& e) const
{
  return activation_;
}

void DifBuffer::vSetActivation(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Activation cannot be negative!\n";
    return;
  }
  activation_ = value;
}


double DifBuffer::vGetBFree(const Eref& e) const
{
  return bFree_;
}
void DifBuffer::vSetBFree(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Free Buffer cannot be negative!\n";
    return;
  }
  if (value > bTot_){
    cerr << "Error: DifBuffer: Free Buffer cannot exceed total buffer!\n";
    return;
  }
  bFree_ = value;
  bBound_ = bTot_ - bFree_;
  prevFree_= bFree_;
  prevBound_ = bBound_;

}

double DifBuffer::vGetBBound(const Eref& e) const
{
  return bBound_;
}

void DifBuffer::vSetBBound(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Bound Buffer cannot be negative!\n";
    return;
  }
  if (value > bTot_){
    cerr << "Error: DifBuffer: Bound Buffer cannot exceed total buffer!\n";
    return;
  }
  bBound_ = value;
  bFree_ = bTot_ - bBound_;
  prevFree_= bFree_;
  prevBound_ = bBound_;
}


double DifBuffer::vGetBTot(const Eref& e) const
{
  return bTot_;
}

void DifBuffer::vSetBTot(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Total buffer concentration cannot be negative!\n";
    return;
  }
  bTot_ = value;
  bFree_ = bTot_;
  bBound_ = 0;
}


double DifBuffer::vGetKf(const Eref& e) const
{
  return kf_;
}

void DifBuffer::vSetKf(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Kf cannot be negative!\n";
    return;
  }
  kf_ = value;
}


double DifBuffer::vGetKb(const Eref& e) const
{
  return kb_;
}

void DifBuffer::vSetKb(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Kb cannot be negative!\n";
    return;
  }
  kb_ = value;
}

double DifBuffer::vGetD(const Eref& e) const
{
  return D_;
}

void DifBuffer::vSetD(const Eref& e,double value)
{

  if ( value  < 0.0 ) {
    cerr << " Error: DifBuffer: Diffusion constant, D, cannot be negative!\n";
    return;
  }
  D_ = value;
}


void DifBuffer::vSetShapeMode(const Eref& e, unsigned int shapeMode )
{
  if ( shapeMode != 0 && shapeMode != 1 && shapeMode != 3 ) {
    cerr << "Error: DifBuffer: I only understand shapeModes 0, 1 and 3.\n";
    return;
  }
  shapeMode_ = shapeMode;
  calculateVolumeArea(e);
}

unsigned int DifBuffer::vGetShapeMode(const Eref& e) const
{
  return shapeMode_;
}

void DifBuffer::vSetLength(const Eref& e, double length )
{
  if ( length < 0.0 ) {
    cerr << "Error: DifBuffer: length cannot be negative!\n";
    return;
  }

  length_ = length;
    calculateVolumeArea(e);
}

double DifBuffer::vGetLength(const Eref& e ) const
{
  return length_;
}

void DifBuffer::vSetDiameter(const Eref& e, double diameter )
{
  if ( diameter < 0.0 ) {
    cerr << "Error: DifBuffer: diameter cannot be negative!\n";
    return;
  }

  diameter_ = diameter;
  calculateVolumeArea(e);
}

double DifBuffer::vGetDiameter(const Eref& e ) const
{
  return diameter_;
}

void DifBuffer::vSetThickness( const Eref& e, double thickness )
{
  if ( thickness < 0.0 ) {
    cerr << "Error: DifBuffer: thickness cannot be negative!\n";
    return;
  }

  thickness_ = thickness;
  calculateVolumeArea(e);
}

double DifBuffer::vGetThickness(const Eref& e) const
{
  return thickness_;
}

void DifBuffer::vSetVolume(const Eref& e, double volume )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifBuffer: Trying to set volume, when shapeMode is not USER-DEFINED\n";

  if ( volume < 0.0 ) {
    cerr << "Error: DifBuffer: volume cannot be negative!\n";
    return;
  }

  volume_ = volume;
}

double DifBuffer::vGetVolume(const Eref& e ) const
{
  return volume_;
}

void DifBuffer::vSetOuterArea(const Eref& e, double outerArea )
{
  if (shapeMode_ != 3 )
    cerr << "Warning: DifBuffer: Trying to set outerArea, when shapeMode is not USER-DEFINED\n";

  if ( outerArea < 0.0 ) {
    cerr << "Error: DifBuffer: outerArea cannot be negative!\n";
    return;
  }

  outerArea_ = outerArea;
}

double DifBuffer::vGetOuterArea(const Eref& e ) const
{
  return outerArea_;
}

void DifBuffer::vSetInnerArea(const Eref& e, double innerArea )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifBuffer: Trying to set innerArea, when shapeMode is not USER-DEFINED\n";

  if ( innerArea < 0.0 ) {
    cerr << "Error: DifBuffer: innerArea cannot be negative!\n";
    return;
  }

  innerArea_ = innerArea;
}

double DifBuffer::vGetInnerArea(const Eref& e) const
{
  return innerArea_;
}




void DifBuffer::vBuffer(const Eref& e,
		       double C )
{
  activation_ = C;
  //cout<<"Buffer"<< C<<" ";
}


double DifBuffer::integrate( double state, double dt, double A, double B )
{
	if ( B > EPSILON ) {
		double x = exp( -B * dt );
		return state * x + ( A / B ) * ( 1 - x );
	}
	return state + A * dt ;
}
void DifBuffer::calculateVolumeArea(const Eref& e)
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

void DifBuffer::vProcess( const Eref & e, ProcPtr p )
{
  /**
   * Send ion concentration and thickness to adjacent DifShells. They will
   * then compute their incoming fluxes.
   */



  Af_ += kb_ * bBound_;
  Bf_ += kf_ * activation_;

  bFree_ = integrate(bFree_,p->dt,Af_,Bf_);
  bBound_ = bTot_ - bFree_;
  prevFree_ = bFree_;
  prevBound_ = bBound_;

  /**
   * Send ion concentration to ion buffers. They will send back information on
   * the reaction (forward / backward rates ; free / bound buffer concentration)
   * immediately, which this DifShell will use to find amount of ion captured
   * or released in the current time-step.
   */
  innerDifSourceOut()->send( e, prevFree_, thickness_ );
  outerDifSourceOut()->send( e, prevFree_, thickness_ );
  reactionOut()->send(e,kf_,kb_,bFree_,bBound_);
  Af_ = 0;
  Bf_= 0;

}

void DifBuffer::vReinit( const Eref& e, ProcPtr p )
{

  Af_ = 0;
  Bf_= 0;
  double rOut = diameter_/2.;

  double rIn = rOut - thickness_;

  if (rIn<0)
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

  bFree_ = bTot_/(1+activation_*kf_/kb_);
  prevFree_ = bFree_;
  bBound_ = bTot_ - bFree_;
  prevBound_ = bBound_;
  innerDifSourceOut()->send( e, prevFree_, thickness_ );
  outerDifSourceOut()->send( e, prevFree_, thickness_ );

}

void DifBuffer::vFluxFromIn(const Eref& e,double innerC, double innerThickness)
{
  double dif = 2 * D_  / volume_* innerArea_ / (thickness_ + innerThickness);
  Af_ += dif * innerC;
  Bf_ += dif;
}

void DifBuffer::vFluxFromOut(const Eref& e,double outerC, double outerThickness)
{
  double dif = 2 * D_  / volume_* outerArea_ / (thickness_ + outerThickness);
  Af_ += dif * outerC;
  Bf_ += dif;
}
