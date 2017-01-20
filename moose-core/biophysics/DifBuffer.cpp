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
// 
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

SrcFinfo4< double, double, double, double >* DifBuffer::reactionOut()
{
  static SrcFinfo4< double, double, double, double > reactionOut(
								 "reactionOut",
								 "Sends out reaction rates (forward and backward), and concentrations"
								 " (free-buffer and bound-buffer molecules).");
  return &reactionOut;
}
                                                                 

SrcFinfo2< double, double >* DifBuffer::innerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("innerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}

SrcFinfo2< double, double >* DifBuffer::outerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("outerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}

const Cinfo * DifBuffer::initCinfo()
{
  static DestFinfo process( "process",
                            "Handles process call",
                            new ProcOpFunc< DifBuffer >(  &DifBuffer::process) );
  static DestFinfo reinit( "reinit",
                           "Reinit happens only in stage 0",
                           new ProcOpFunc< DifBuffer >( &DifBuffer::reinit));
    
  static Finfo* processShared[] = {
    &process,
    &reinit
  };

  static SharedFinfo proc(
			       "proc", 
			       "Here we create 2 shared finfos to attach with the Ticks. This is because we want to perform DifBuffer "
			       "computations in 2 stages, much as in the Compartment object. "
			       "In the first stage we send out the concentration value to other DifBuffers and Buffer elements. We also",
			       processShared,
			       sizeof( processShared ) / sizeof( Finfo* ));
	
  /*static DestFinfo process1( "process",
                             "Handle process call",
                             new ProcOpFunc< DifBuffer >( &DifBuffer::process_1 ) );
  static DestFinfo reinit1( "reinit", 
                            "Reinit happens only in stage 0",
                            new ProcOpFunc< DifBuffer >( &DifBuffer::reinit_1 ) );
  
  static Finfo* processShared_1[] = {
    &process1, &reinit1        
  };
  
  static SharedFinfo process_1( "process_1",
                                "Second process call",
                                processShared_1,
                                sizeof( processShared_1 ) / sizeof( Finfo* ) );
  */
  //// Diffusion related shared messages
  
  
  static DestFinfo concentration("concentration",
                                 "Receives concentration (from DifShell).",
                                 new EpFunc1<DifBuffer, double>(&DifBuffer::buffer));
  static Finfo* bufferShared[] = {
    &concentration, DifBuffer::reactionOut()
  };
  static SharedFinfo buffer( "buffer",
                             "This is a shared message with DifShell. "
                             "During stage 0:\n "
                             " - DifBuffer sends ion concentration\n"
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
                                new EpFunc2< DifBuffer, double, double > ( &DifBuffer::fluxFromOut ));
    
  static Finfo* innerDifShared[] = {
      &fluxFromOut,
      DifBuffer::innerDifSourceOut()
    
  };
    
  static SharedFinfo innerDif( "innerDif",
                               "This shared message (and the next) is between DifBuffers: adjoining shells exchange information to "
                               "find out the flux between them. "
                               "Using this message, an inner shell sends to, and receives from its outer shell." ,
                               innerDifShared,
                               sizeof( innerDifShared ) / sizeof( Finfo* ));

  static DestFinfo fluxFromIn( "fluxFromIn", "",
                               new EpFunc2< DifBuffer, double, double> ( &DifBuffer::fluxFromIn) );
  
  static Finfo* outerDifShared[] = {
    &fluxFromIn,  
    DifBuffer::outerDifSourceOut(),

  };

  static  SharedFinfo outerDif( "outerDif",
                                "Using this message, an outer shell sends to, and receives from its inner shell." ,
                                outerDifShared,
                                sizeof( outerDifShared ) / sizeof( Finfo* ));
  
  ////////////////////////////
  // Field defs
  ////////////////////////////
  static ElementValueFinfo<DifBuffer, double> activation("activation",
                                                  "Ion concentration from incoming conc message.",
                                                  &DifBuffer::setActivation,
                                                  &DifBuffer::getActivation);
  static ElementValueFinfo<DifBuffer, double> kf("kf",
					  "Forward rate constant of buffer molecules 1/mM/s (?)",
					  &DifBuffer::setKf,
					  &DifBuffer::getKf);
  static ElementValueFinfo<DifBuffer, double> kb("kb",
					  "Backward rate constant of buffer molecules. 1/s",
					  &DifBuffer::setKb,
					  &DifBuffer::getKb);
  static ElementValueFinfo<DifBuffer, double> D("D",
                                         "Diffusion constant of buffer molecules. m^2/s",
                                         &DifBuffer::setD,
                                         &DifBuffer::getD);
  static ReadOnlyElementValueFinfo<DifBuffer, double> bFree("bFree",
                                                     "Free buffer concentration",
                                                     &DifBuffer::getBFree);
  static ReadOnlyElementValueFinfo<DifBuffer, double> bBound("bBound",
                                                      "Bound buffer concentration",
                                                      &DifBuffer::getBBound);
  /*  static ReadOnlyElementValueFinfo<DifBuffer, double> prevFree("prevFree",
							"Free buffer concentration in previous step",
							&DifBuffer::getPrevFree);
  static ReadOnlyElementValueFinfo<DifBuffer, double> prevBound("prevBound",
							 "Bound buffer concentration in previous step",
							 &DifBuffer::getPrevBound);*/
  static ElementValueFinfo<DifBuffer, double> bTot("bTot",
                                            "Total buffer concentration.",
                                            &DifBuffer::setBTot,
                                            &DifBuffer::getBTot);  
  static ElementValueFinfo<DifBuffer, double> length("length",
                                              "Length of shell",
                                              &DifBuffer::setLength,
                                              &DifBuffer::getLength);
  static ElementValueFinfo<DifBuffer, double> diameter("diameter",
                                                "Diameter of shell",
                                                &DifBuffer::setDiameter,
                                                &DifBuffer::getDiameter);
  static ElementValueFinfo<DifBuffer, int> shapeMode("shapeMode",
                                              "shape of the shell: SHELL=0, SLICE=SLAB=1, USERDEF=3",
                                              &DifBuffer::setShapeMode,
                                              &DifBuffer::getShapeMode);
  
  static ElementValueFinfo<DifBuffer, double> thickness("thickness",
						 "Thickness of shell",
						 &DifBuffer::setThickness,
						 &DifBuffer::getThickness);
 
  static ElementValueFinfo<DifBuffer, double> innerArea("innerArea",
                                                 "Inner area of shell",
                                                 &DifBuffer::setInnerArea,
                                                 &DifBuffer::getInnerArea);
  static ElementValueFinfo<DifBuffer, double> outerArea("outerArea",
                                                 "Outer area of shell",
                                                 &DifBuffer::setOuterArea,
                                                 &DifBuffer::getOuterArea);
  static ElementValueFinfo< DifBuffer, double> volume( "volume", "",
						&DifBuffer::setVolume,
						&DifBuffer::getVolume );
  
  ////
  // DestFinfo
  ////
  static Finfo * difBufferFinfos[] = {
    //////////////////////////////////////////////////////////////////
    // Field definitions
    //////////////////////////////////////////////////////////////////
    
    &activation,
    &D,
    &bFree,
    &bBound,
    &bTot,
    &kf,
    &kb,
    //&prevFree,
    //&prevBound,
    &length,
    &diameter,
    &shapeMode,
    &thickness,
    &innerArea,
    &outerArea,
    &volume,
    //////////////////////////////////////////////////////////////////
    // SharedFinfo definitions
    /////////////////////////////////////////////////////////////////
    &proc,
    &buffer,
    &innerDif,
    &outerDif,
    //
    //reactionOut(),
    //innerDifSourceOut(),
    //outerDifSourceOut(),
    //////////////////////////////////////////////////////////////////
    // DestFinfo definitions
    //////////////////////////////////////////////////////////////////
    &concentration,    
  };

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
			      difBufferFinfos,
			      sizeof(difBufferFinfos)/sizeof(Finfo*),
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
  //  prevFree_(0),
  //prevBound_(0),
  bTot_(0),
  kf_(0),
  kb_(0),
  D_(0),
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
double DifBuffer::getActivation(const Eref& e) const
{
  return activation_;
}

void DifBuffer::setActivation(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Activation cannot be negative!\n";
    return;
  }
  activation_ = value;
}


double DifBuffer::getBFree(const Eref& e) const
{
  return bFree_;
}

double DifBuffer::getBBound(const Eref& e) const
{
  return bBound_;
}

/*double DifBuffer::getPrevFree(const Eref& e) const
{
  return prevFree_;
}

double DifBuffer::getPrevBound(const Eref& e) const
{
  return prevBound_;
}
*/
double DifBuffer::getBTot(const Eref& e) const
{
  return bTot_;
}

void DifBuffer::setBTot(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Total buffer concentration cannot be negative!\n";
    return;
  }
  bTot_ = value;
}


double DifBuffer::getKf(const Eref& e) const
{
  return kf_;
}

void DifBuffer::setKf(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Kf cannot be negative!\n";
    return;
  }
  kf_ = value;
}


double DifBuffer::getKb(const Eref& e) const
{
  return kb_;
}

void DifBuffer::setKb(const Eref& e,double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Kb cannot be negative!\n";
    return;
  }
  kb_ = value;
}

double DifBuffer::getD(const Eref& e) const
{
  return D_;
}

void DifBuffer::setD(const Eref& e,double value)
{
  
  if ( value  < 0.0 ) {
    cerr << " Error: DifBuffer: Diffusion constant, D, cannot be negative!\n";
    return;
  }
  D_ = value;
}

int DifBuffer::getShapeMode(const Eref& e) const
{
  return shapeMode_;
}

void DifBuffer::setShapeMode(const Eref& e,int value)
{
  if ( value  != 0 && value !=1 && value != 3 ) {
    cerr << "Error: DifBuffer: Shape mode can only be 0, 1 or 3";
    return;
  }
  shapeMode_ = value;
}

double DifBuffer::getLength(const Eref& e) const
{
  return length_;
}

void DifBuffer::setLength(const Eref& e,double value)
{
  if ( value < 0.0) {
    cerr << "Error: DifBuffer: Length cannot be negative!\n";
    return;
  }
  length_ = value;
}

double DifBuffer::getDiameter(const Eref& e) const
{
  return diameter_;
}

void DifBuffer::setDiameter(const Eref& e,double value)
{
  if ( value < 0.0) {
    cerr << "Error: DifBuffer: Diameter cannot be negative!\n";
    return;
  }
  diameter_ = value;
}

double DifBuffer::getThickness(const Eref& e) const
{
  return thickness_;
}

void DifBuffer::setThickness(const Eref& e,double value)
{
  if ( value < 0.0) {
    cerr << "Error: DifBuffer: Thickness cannot be negative!\n";
    return;
  }
  thickness_ = value;
}

void DifBuffer::setVolume(const Eref& e, double volume )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifBuffer: Trying to set volume, when shapeMode is not USER-DEFINED\n";
	
  if ( volume < 0.0 ) {
    cerr << "Error: DifBuffer: volume cannot be negative!\n";
    return;
  }
	
  volume_ = volume;
}

double DifBuffer::getVolume(const Eref& e ) const
{
  return volume_;
}

void DifBuffer::setOuterArea(const Eref& e, double outerArea )
{
  if (shapeMode_ != 3 )
    cerr << "Warning: DifBuffer: Trying to set outerArea, when shapeMode is not USER-DEFINED\n";
	
  if ( outerArea < 0.0 ) {
    cerr << "Error: DifBuffer: outerArea cannot be negative!\n";
    return;
  }
	
  outerArea_ = outerArea;
}

double DifBuffer::getOuterArea(const Eref& e ) const
{
  return outerArea_;
}

void DifBuffer::setInnerArea(const Eref& e, double innerArea )
{
  if ( shapeMode_ != 3 )
    cerr << "Warning: DifBuffer: Trying to set innerArea, when shapeMode is not USER-DEFINED\n";
    
  if ( innerArea < 0.0 ) {
    cerr << "Error: DifBuffer: innerArea cannot be negative!\n";
    return;
  }
    
  innerArea_ = innerArea;
}

double DifBuffer::getInnerArea(const Eref& e) const
{
  return innerArea_;
}
// Dest function for conenctration received from DifShell.  This
// function updates buffer concentration and sends back immediately
// using call back.
// TODO: complete this ... where do we get dt from? ProcPtr p->dt
//Check DifShell

//In Genesis the following are messages:


void DifBuffer::reinit( const Eref& e, ProcPtr p )
{
  localReinit_0( e, p );
}

void DifBuffer::process( const Eref& e, ProcPtr p )
{
  localProcess_0( e, p );
}

/*void DifBuffer::process_1(const Eref& e, ProcPtr p )
{
  localProcess_1( e, p );
}

void DifBuffer::reinit_1(const Eref& e, ProcPtr p )
{
  ;
}
*/
void DifBuffer::buffer(const Eref& e,
		       double C )
{
  activation_ = C;
}


void DifBuffer::localProcess_0( const Eref & e, ProcPtr p )
{
  /**
   * Send ion concentration and thickness to adjacent DifShells. They will
   * then compute their incoming fluxes.
   */

  innerDifSourceOut()->send( e, bFree_, thickness_ );
  outerDifSourceOut()->send( e, bFree_, thickness_ );
  Af_ += kb_ * bBound_;
  Bf_ += kf_ * activation_;
  bFree_ += (Af_ - bFree_ * Bf_) * p->dt;
  reactionOut()->send(e,kf_,kb_,bFree_,bBound_);
  /**
   * Send ion concentration to ion buffers. They will send back information on
   * the reaction (forward / backward rates ; free / bound buffer concentration)
   * immediately, which this DifShell will use to find amount of ion captured
   * or released in the current time-step.
   */

}

void DifBuffer::localReinit_0( const Eref& e, ProcPtr p )
{
	
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
/*
void DifBuffer::localProcess_1(const Eref& e, ProcPtr p )
{
  //activation_ = conc;
  Af_ += kb_ * bBound_;
  Bf_ += kf_ * activation_;
  bFree_ += (Af_ - bFree_ * Bf_) * p->dt;  
}
*/
void DifBuffer::fluxFromIn(const Eref& e,double innerC, double innerThickness)
{
  double dif = 2 * D_ * innerArea_ / ((thickness_ + innerThickness) * volume_);
  Af_ += dif * innerC;
  Bf_ += dif;
}

void DifBuffer::fluxFromOut(const Eref& e,double outerC, double outerThickness)
{
  double dif = 2 * D_ * outerArea_ / ((thickness_ + outerThickness)  * volume_);
  Af_ += dif * outerC;
  Bf_ += dif;
}
