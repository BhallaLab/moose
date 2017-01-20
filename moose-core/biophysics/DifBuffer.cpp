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

#include "header.h"
#include "DifBuffer.h"
#include "ElementValueFinfo.h"
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
			      Neutral::initCinfo(),
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
  shapeMode_(0),
  diameter_(0),
  length_(0),
  thickness_(0),
  outerArea_(0),
  innerArea_(0),
  volume_(0)
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

double DifBuffer::vGetBBound(const Eref& e) const
{
  return bBound_;
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


void DifBuffer::vBuffer(const Eref& e,
		       double C )
{
  activation_ = C;
}

double DifBuffer::integrate( double state, double dt, double A, double B )
{
	if ( B > EPSILON ) {
		double x = exp( -B * dt );
		return state * x + ( A / B ) * ( 1 - x );
	}
	return state + A * dt ;
}


void DifBuffer::vProcess( const Eref & e, ProcPtr p )
{
  /**
   * Send ion concentration and thickness to adjacent DifShells. They will
   * then compute their incoming fluxes.
   */

  innerDifSourceOut()->send( e, bFree_, thickness_ );
  outerDifSourceOut()->send( e, bFree_, thickness_ );
  Af_ += kb_ * bBound_;
  Bf_ += kf_ * activation_;
  bFree_ = integrate(bFree_,p->dt,Af_,Bf_);
  bBound_ = bTot_ - bFree_;
    
  reactionOut()->send(e,kf_,kb_,bFree_,bBound_);
  /**
   * Send ion concentration to ion buffers. They will send back information on
   * the reaction (forward / backward rates ; free / bound buffer concentration)
   * immediately, which this DifShell will use to find amount of ion captured
   * or released in the current time-step.
   */

}

void DifBuffer::vReinit( const Eref& e, ProcPtr p )
{
	
  const double dOut = diameter_;
  const double dIn = diameter_ - thickness_;
  bFree_ = bTot_;
  bBound_ = 0;
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

void DifBuffer::vFluxFromIn(const Eref& e,double innerC, double innerThickness)
{
  double dif = 2 * D_ * innerArea_ / ((thickness_ + innerThickness) * volume_);
  Af_ += dif * innerC;
  Bf_ += dif;
}

void DifBuffer::vFluxFromOut(const Eref& e,double outerC, double outerThickness)
{
  double dif = 2 * D_ * outerArea_ / ((thickness_ + outerThickness)  * volume_);
  Af_ += dif * outerC;
  Bf_ += dif;
}
