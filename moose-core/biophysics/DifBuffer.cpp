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
#include "../utility/numutil.h"
#include <cmath>

static SrcFinfo4< double, double, double, double >* reactionOut()
{
  static SrcFinfo4< double, double, double, double > reactionOut(
      "reactionOut",
      "Sends out reaction rates (forward and backward), and concentrations"
       " (free-buffer and bound-buffer molecules).");
  return &reactionOut;
}
                                                                 

const Cinfo * DifBuffer::initCinfo()
{
  static DestFinfo process( "process",
                            "Handles process call",
                            new ProcOpFunc< DifBuffer >(  &DifBuffer::process) );
  static DestFinfo reinit( "reinit",
                           "Reinit happens only in stage 0",
                           new ProcOpFunc< DifBuffer >( &DifBuffer::reinit));
    
  static Finfo* processShared_0[] = {
    &process,
    &reinit
  };

  static SharedFinfo process_0(
      "process_0", 
      "Here we create 2 shared finfos to attach with the Ticks. This is because we want to perform DifBuffer "
      "computations in 2 stages, much as in the Compartment object. "
      "In the first stage we send out the concentration value to other DifBuffers and Buffer elements. We also "
      "receive fluxes and currents and sum them up to compute ( dC / dt ). "
      "In the second stage we find the new C value using an explicit integration method. "
      "This 2-stage procedure eliminates the need to store and send prev_C values, as was common in GENESIS.",
      processShared_0,
      sizeof( processShared_0 ) / sizeof( Finfo* ));
	
  static DestFinfo process1( "process",
                             "Handle process call",
                             new ProcOpFunc< DifBuffer >( &DifBuffer::process_1 ) );
  static DestFinfo reinit1( "reinit", 
                            "Reinit happens only in stage 0",
                            new ProcOpFunc< DifBuffer >( &DifBuffer::reinit_1)
                            );
  static Finfo* processShared_1[] = {
    &process1, &reinit1        
  };
    
  static SharedFinfo process_1( "process_1",
                                "Second process call",
                                processShared_1,
                                sizeof( processShared_1 ) / sizeof( Finfo* ) );

  //// Diffusion related shared messages
  static DestFinfo concentration("concentration",
                                 "Receives concentration (from DifShell).",
                                 new OpFunc1<DifBuffer, double>(&DifBuffer::concentration));
  
  static Finfo* bufferShared[] = {
    &concentration, reactionOut()
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
                                new OpFunc2< DifBuffer, double, double > ( &DifBuffer::fluxFromOut ));
    
  static Finfo* innerDifShared[] = {
    innerDifSourceOut(),
    &fluxFromOut
  };
    
  static SharedFinfo innerDif( "innerDif",
                               "This shared message (and the next) is between DifBuffers: adjoining shells exchange information to "
                               "find out the flux between them. "
                               "Using this message, an inner shell sends to, and receives from its outer shell." ,
                               innerDifShared,
                               sizeof( innerDifShared ) / sizeof( Finfo* ));

  static DestFinfo fluxFromIn( "fluxFromIn", "",
                               new OpFunc2< DifBuffer, double, double> ( &DifBuffer::fluxFromIn ) );
  static Finfo* outerDifShared[] = {
    &fluxFromIn,
    outerDifSourceOut(),
  };

  static  SharedFinfo outerDif( "outerDif",
                                "Using this message, an outer shell sends to, and receives from its inner shell." ,
                                outerDifShared,
                                sizeof( outerDifShared ) / sizeof( Finfo* ));
  
  ////////////////////////////
  // Field defs
  ////////////////////////////
  static ValueFinfo<DifBuffer, double> activation("activation",
                                                  "Ion concentration from incoming conc message.",
                                                  &DifBuffer::setActivation,
                                                  &DifBuffer::getActivation);
  static ValueFinfo<DifBuffer, double> D("D",
                                         "Diffusion constant of buffer molecules. m^2/s",
                                         &DifBuffer::setD,
                                         &DifBuffer::getD);
  static ReadOnlyValueFinfo<DifBuffer, double> bFree("bFree",
                                                     "Free buffer concentration",
                                                     &DifBuffer::getBFree);
  static ReadOnlyValueFinfo<DifBuffer, double> bBound("bBound",
                                                      "Bound buffer concentration",
                                                      &DifBuffer::getBBound);
  static ReadOnlyValueFinfo<DifBuffer, double> prevFree("prevFree",
                                                      "Free buffer concentration in previous step",
                                                      &DifBuffer::getPrevFree);
  static ReadOnlyValueFinfo<DifBuffer, double> prevBound("prevBound",
                                                      "Bound buffer concentration in previous step",
                                                      &DifBuffer::getPrevBound);
  static ValueFinfo<DifBuffer, double> bTot("bTot",
                                            "Total buffer concentration.",
                                            &DifBuffer::setBTot,
                                            &DifBuffer::getBTot);  
  static ValueFinfo<DifBuffer, double> length("length",
                                              "Length of shell",
                                              &DifBuffer::setLength,
                                              &DifBuffer::getLength);
  static ValueFinfo<DifBuffer, double> diameter("diameter",
                                                "Diameter of shell",
                                                &DifBuffer::setDiameter,
                                                &DifBuffer::getDiameter);
  static ValueFinfo<DifBuffer, int> shapeMode("shapeMode",
                                              "shape of the shell: SHELL=0, SLICE=SLAB=1, USERDEF=3",
                                              &DifBuffer::setShapeMode,
                                              &DifBuffer::getShapeMode);
  
  static ValueFinfo<DifBuffer, double> thick("thickness",
                                             "Thickness of shell",
                                             &DifBuffer::setThickness,
                                             &DifBuffer::getThickness);
 
  static ValueFinfo<DifBuffer, double> innerArea("innerArea",
                                                 "Inner area of shell",
                                                 &DifBuffer::setInnerArea,
                                                 &DifBuffer::getInnerArea);
  static ValueFinfo<DifBuffer, double> outerArea("outerArea",
                                                 "Outer area of shell",
                                                 &DifBuffer::setOuterArea,
                                                 &DifBuffer::getOuterArea);
  
  ////
  // DestFinfo
  ////
  static Finfo * difBufferFinfos[] = {
    &activation,
    &D,
    &bFree,
    &bBound,
    &bTot,
    // &prevFree,
    // &prevBound,
    &length,
    &diameter,
    &shapeMode,
    &thickness,
    &innerArea,
    &outerArea,
    &process,
    &reinit,
    &innerDif,
    &outerDif,
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

DifBuffer::DifBuffer()//konstruktor?
{ ; }

////////////////////////////////////////////////////////////////////////////////
// Field access functions
////////////////////////////////////////////////////////////////////////////////
double DifBuffer::getActivation() const
{
  return activation_;
}

void DifBuffer::setActivation(double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Activation cannot be negative!\n";
    return;
  }
  activation_ = value;
}

double DifBuffer::getBFree() const
{
  return bFree_;
}

double DifBuffer::getBBound() const
{
  return bBound_;
}

double DifBuffer::getPrevFree() const
{
  return prevFree_;
}

double DifBuffer::getPrevBound() const
{
  return prevBound_;
}

double DifBuffer::getBTot() const
{
  return bTot_;
}

void DifBuffer::setBTot(double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Total buffer concentration cannot be negative!\n";
    return;
  }
  bTot_ = value;
}


double DifBuffer::getKf() const
{
  return kf_;
}

void DifBuffer::setKf(double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Kf cannot be negative!\n";
    return;
  }
  kf_ = value;
}


double DifBuffer::getKb() const
{
  return kb_;
}

void DifBuffer::setKb(double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Kb cannot be negative!\n";
    return;
  }
  kb_ = value;
}

double DifBuffer::getD() const
{
  return D_;
}

void DifBuffer::setD(double value)
{
  if ( value  < 0.0 ) {
    cerr << "Error: DifBuffer: Diffusion constant, D, cannot be negative!\n";
    return;
  }
  D_ = value;
}

double DifBuffer::getShapeMode() const
{
  return shapeMode_;
}

void DifBuffer::setShapeMode(int value)
{
  if ( value  != 1 && value !=2 && value != 3 ) {
    cerr << "Error: DifBuffer: Diffusion constant, D, cannot be negative!\n";
    return;
  }
  shapeMode_ = value;
}

double DifBuffer::getLength() const
{
  return length_;
}

void DifBuffer::setLength(int value)
{
  if ( value < 0.0) {
    cerr << "Error: DifBuffer: Length cannot be negative!\n";
    return;
  }
  length_ = value;
}

double DifBuffer::getDiameter() const
{
  return diameter_;
}

void DifBuffer::setDiameter(int value)
{
  if ( value < 0.0) {
    cerr << "Error: DifBuffer: Diameter cannot be negative!\n";
    return;
  }
  diameter_ = value;
}

double DifBuffer::getThickness() const
{
  return thickness_;
}

void DifBuffer::setThickness(int value)
{
  if ( value < 0.0) {
    cerr << "Error: DifBuffer: Thickness cannot be negative!\n";
    return;
  }
  thickness_ = value;
}

void DifBuffer::setVolume( double volume )
{
	if ( shapeMode_ != 3 )
		cerr << "Warning: DifBuffer: Trying to set volume, when shapeMode is not USER-DEFINED\n";
	
	if ( volume < 0.0 ) {
		cerr << "Error: DifBuffer: volume cannot be negative!\n";
		return;
	}
	
	volume_ = volume;
}

double DifBuffer::getVolume( ) const
{
	return volume_;
}

void DifBuffer::setOuterArea( double outerArea )
{
    if (shapeMode_ != 3 )
        cerr << "Warning: DifBuffer: Trying to set outerArea, when shapeMode is not USER-DEFINED\n";
	
    if ( outerArea < 0.0 ) {
        cerr << "Error: DifBuffer: outerArea cannot be negative!\n";
        return;
    }
	
    outerArea_ = outerArea;
}

double DifBuffer::getOuterArea( ) const
{
    return outerArea_;
}

void DifBuffer::setInnerArea( double innerArea )
{
    if ( shapeMode_ != 3 )
        cerr << "Warning: DifBuffer: Trying to set innerArea, when shapeMode is not USER-DEFINED\n";
    
    if ( innerArea < 0.0 ) {
        cerr << "Error: DifBuffer: innerArea cannot be negative!\n";
        return;
    }
    
    innerArea_ = innerArea;
}

double DifBuffer::getInnerArea() const
{
    return innerArea_;
}
// Dest function for conenctration received from DifShell.  This
// function updates buffer concentration and sends back immediately
// using call back.
// TODO: complete this ... where do we get dt from? ProcPtr p->dt
void DifBuffer::concentration(double conc)
{
  activation_ = conc;
  Af_ += kb_ * bBound_;
  Bf_ += kf_ * activation_;
  bFree_ += (Af_ - bFree_ * Bf_) * dt_;  
}

void DifBuffer::fluxFromIn(double innerC, double innerThickness)
{
  double dif = 2 * D_ * innerArea_ / ((thickness_ + innerThickness) * volume_);
  Af_ += dif * innerC;
  Bf_ += dif;
}

void DifBuffer::fluxFromOut(double outerC, double outerThickness)
{
  double dif = 2 * D_ * outerArea_ / ((thickness_ + outerThickness)  * volume_);
  Af_ += dif * outerC;
  Bf_ += dif;
}


// 
// DifBuffer.cpp ends here
