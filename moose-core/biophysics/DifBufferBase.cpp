#include "../basecode/header.h"
#include "DifBufferBase.h"
#include "../basecode/ElementValueFinfo.h"
#include "../utility/numutil.h"
#include <cmath>

SrcFinfo4< double, double, double, double >* DifBufferBase::reactionOut()
{
  static SrcFinfo4< double, double, double, double > reactionOut(
								 "reactionOut",
								 "Sends out reaction rates (forward and backward), and concentrations"
								 " (free-buffer and bound-buffer molecules).");
  return &reactionOut;
}


SrcFinfo2< double, double >* DifBufferBase::innerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("innerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}

SrcFinfo2< double, double >* DifBufferBase::outerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("outerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}

const Cinfo * DifBufferBase::initCinfo()
{
  static DestFinfo process( "process",
                            "Handles process call",
                            new ProcOpFunc< DifBufferBase >(  &DifBufferBase::process) );
  static DestFinfo reinit( "reinit",
                           "Reinit happens only in stage 0",
                           new ProcOpFunc< DifBufferBase >( &DifBufferBase::reinit));

  static Finfo* processShared[] = {
    &process,
    &reinit
  };

  static SharedFinfo proc(
			  "proc",
			  "Here we create 2 shared finfos to attach with the Ticks. This is because we want to perform DifBufferBase "
			  "computations in 2 stages, much as in the Compartment object. "
			  "In the first stage we send out the concentration value to other DifBufferBases and Buffer elements. We also",
			  processShared,
			  sizeof( processShared ) / sizeof( Finfo* ));

  static DestFinfo concentration("concentration",
                                 "Receives concentration (from DifShell).",
                                 new EpFunc1<DifBufferBase, double>(&DifBufferBase::buffer));
  static Finfo* bufferShared[] = {
    &concentration, DifBufferBase::reactionOut()
  };
  static SharedFinfo buffer( "buffer",
                             "This is a shared message with DifShell. "
                             "During stage 0:\n "
                             " - DifBufferBase sends ion concentration\n"
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
                                new EpFunc2< DifBufferBase, double, double > ( &DifBufferBase::fluxFromOut ));

  static Finfo* innerDifShared[] = {
    &fluxFromOut,
    DifBufferBase::innerDifSourceOut()

  };

  static SharedFinfo innerDif( "innerDif",
                               "This shared message (and the next) is between DifBufferBases: adjoining shells exchange information to "
                               "find out the flux between them. "
                               "Using this message, an inner shell sends to, and receives from its outer shell." ,
                               innerDifShared,
                               sizeof( innerDifShared ) / sizeof( Finfo* ));

  static DestFinfo fluxFromIn( "fluxFromIn", "",
                               new EpFunc2< DifBufferBase, double, double> ( &DifBufferBase::fluxFromIn) );

  static Finfo* outerDifShared[] = {
    &fluxFromIn,
    DifBufferBase::outerDifSourceOut(),

  };

  static  SharedFinfo outerDif( "outerDif",
                                "Using this message, an outer shell sends to, and receives from its inner shell." ,
                                outerDifShared,
                                sizeof( outerDifShared ) / sizeof( Finfo* ));

  ////////////////////////////
  // Field defs
  ////////////////////////////
  static ElementValueFinfo<DifBufferBase, double> activation("activation",
							     "Ion concentration from incoming conc message.",
							     &DifBufferBase::setActivation,
							     &DifBufferBase::getActivation);
  static ElementValueFinfo<DifBufferBase, double> kf("kf",
						     "Forward rate constant of buffer molecules 1/mM/s (?)",
						     &DifBufferBase::setKf,
						     &DifBufferBase::getKf);
  static ElementValueFinfo<DifBufferBase, double> kb("kb",
						     "Backward rate constant of buffer molecules. 1/s",
						     &DifBufferBase::setKb,
						     &DifBufferBase::getKb);
  static ElementValueFinfo<DifBufferBase, double> D("D",
						    "Diffusion constant of buffer molecules. m^2/s",
						    &DifBufferBase::setD,
						    &DifBufferBase::getD);
  static ElementValueFinfo<DifBufferBase, double> bFree("bFree",
							"Free buffer concentration",
							&DifBufferBase::setBFree,
							&DifBufferBase::getBFree);
  static ElementValueFinfo<DifBufferBase, double> bBound("bBound",
							 "Bound buffer concentration",
							 &DifBufferBase::setBBound,
							 &DifBufferBase::getBBound);
  static ElementValueFinfo<DifBufferBase, double> bTot("bTot",
						       "Total buffer concentration.",
						       &DifBufferBase::setBTot,
						       &DifBufferBase::getBTot);
  static ElementValueFinfo<DifBufferBase, double> length("length",
							 "Length of shell",
							 &DifBufferBase::setLength,
							 &DifBufferBase::getLength);
  static ElementValueFinfo<DifBufferBase, double> diameter("diameter",
							   "Diameter of shell",
							   &DifBufferBase::setDiameter,
							   &DifBufferBase::getDiameter);
  static ElementValueFinfo<DifBufferBase, unsigned int> shapeMode("shapeMode",
								  "shape of the shell: SHELL=0, SLICE=SLAB=1, USERDEF=3",
								  &DifBufferBase::setShapeMode,
								  &DifBufferBase::getShapeMode);

  static ElementValueFinfo<DifBufferBase, double> thickness("thickness",
							    "Thickness of shell",
							    &DifBufferBase::setThickness,
							    &DifBufferBase::getThickness);

  static ElementValueFinfo<DifBufferBase, double> innerArea("innerArea",
							    "Inner area of shell",
							    &DifBufferBase::setInnerArea,
							    &DifBufferBase::getInnerArea);
  static ElementValueFinfo<DifBufferBase, double> outerArea("outerArea",
							    "Outer area of shell",
							    &DifBufferBase::setOuterArea,
							    &DifBufferBase::getOuterArea);
  static ElementValueFinfo< DifBufferBase, double> volume( "volume", "",
							   &DifBufferBase::setVolume,
							   &DifBufferBase::getVolume );

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
    reactionOut(),
    innerDifSourceOut(),
    outerDifSourceOut(),
    //////////////////////////////////////////////////////////////////
    // DestFinfo definitions
    //////////////////////////////////////////////////////////////////
    &concentration,
  };

  static string doc[] = {
    "Name", "DifBufferBase",
    "Author", "Subhasis Ray (ported from GENESIS2)",
    "Description", "Models diffusible buffer where total concentration is constant. It is"
    " coupled with a DifShell.",
  };
  static ZeroSizeDinfo<int> dinfo;
  static Cinfo difBufferCinfo(
			      "DifBufferBase",
			      Neutral::initCinfo(),
			      difBufferFinfos,
			      sizeof(difBufferFinfos)/sizeof(Finfo*),
			      &dinfo,
			      doc,
			      sizeof(doc)/sizeof(string));

  return &difBufferCinfo;
}

static const Cinfo * difBufferCinfo = DifBufferBase::initCinfo();


////////////////////////////////////////////////////////////////////////////////
// Class functions
////////////////////////////////////////////////////////////////////////////////

DifBufferBase::DifBufferBase()
{ ; }


double DifBufferBase::getActivation(const Eref& e) const
{
  return vGetActivation(e);
}

void DifBufferBase::setActivation(const Eref& e,double value)
{
  vSetActivation(e,value);
}


double DifBufferBase::getBFree(const Eref& e) const
{
  return vGetBFree(e);
}

void DifBufferBase::setBFree(const Eref& e,double value)
{
  vSetBFree(e,value);
}

double DifBufferBase::getBBound(const Eref& e) const
{
  return vGetBBound(e);
}
void DifBufferBase::setBBound(const Eref& e,double value)
{
  vSetBBound(e,value);
}

double DifBufferBase::getBTot(const Eref& e) const
{
  return vGetBTot(e);
}

void DifBufferBase::setBTot(const Eref& e,double value)
{
  vSetBTot(e,value);
}


double DifBufferBase::getKf(const Eref& e) const
{
  return vGetKf(e);
}

void DifBufferBase::setKf(const Eref& e,double value)
{
  vSetKf(e,value);
}

double DifBufferBase::getKb(const Eref& e) const
{
  return vGetKb(e);
}

void DifBufferBase::setKb(const Eref& e,double value)
{
  vSetKb(e,value);
}

double DifBufferBase::getD(const Eref& e) const
{
  return vGetD(e);
}

void DifBufferBase::setD(const Eref& e,double value)
{
  vSetD(e,value);
}

void DifBufferBase::setShapeMode(const Eref& e, unsigned int shapeMode )
{
  vSetShapeMode(e,shapeMode);
}

unsigned int DifBufferBase::getShapeMode(const Eref& e) const
{
  return vGetShapeMode(e);
}

void DifBufferBase::setLength(const Eref& e, double length )
{
  vSetLength(e,length);
}

double DifBufferBase::getLength(const Eref& e ) const
{
  return vGetLength(e);
}

void DifBufferBase::setDiameter(const Eref& e, double diameter )
{
  vSetDiameter(e,diameter);
}

double DifBufferBase::getDiameter(const Eref& e ) const
{
  return vGetDiameter(e);
}

void DifBufferBase::setThickness( const Eref& e, double thickness )
{
  vSetThickness(e,thickness);
}

double DifBufferBase::getThickness(const Eref& e) const
{
  return vGetThickness(e);
}

void DifBufferBase::setVolume(const Eref& e, double volume )
{
  vSetVolume(e,volume);
}

double DifBufferBase::getVolume(const Eref& e ) const
{
  return vGetVolume(e);
}

void DifBufferBase::setOuterArea(const Eref& e, double outerArea )
{
  vSetOuterArea(e,outerArea);
}

double DifBufferBase::getOuterArea(const Eref& e ) const
{
  return vGetOuterArea(e);
}

void DifBufferBase::setInnerArea(const Eref& e, double innerArea )
{
  vSetInnerArea(e,innerArea);
}

double DifBufferBase::getInnerArea(const Eref& e) const
{
  return vGetInnerArea(e);
}



void DifBufferBase::buffer(const Eref& e,double C)
{
  vBuffer(e,C);
}

void DifBufferBase::reinit( const Eref& e, ProcPtr p )
{
  vReinit( e, p );
}

void DifBufferBase::process( const Eref& e, ProcPtr p )
{
  vProcess( e, p );
}
void DifBufferBase:: fluxFromOut(const Eref& e,double outerC, double outerThickness )
{
  vFluxFromOut(e,outerC,outerThickness);
}
void DifBufferBase:: fluxFromIn(const Eref& e,double innerC, double innerThickness )
{
  vFluxFromIn(e,innerC,innerThickness);
}
