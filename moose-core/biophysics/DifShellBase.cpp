/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 ****/

#include "../basecode/header.h"
#include "DifShellBase.h"
#include "../basecode/ElementValueFinfo.h"

SrcFinfo1< double >* DifShellBase::concentrationOut()
{
  static SrcFinfo1< double > concentrationOut("concentrationOut",
					      "Sends out concentration");
  return &concentrationOut;
}

SrcFinfo2< double, double >* DifShellBase::innerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("innerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}

SrcFinfo2< double, double >* DifShellBase::outerDifSourceOut(){
  static SrcFinfo2< double, double > sourceOut("outerDifSourceOut",
					       "Sends out source information.");
  return &sourceOut;
}

const Cinfo* DifShellBase::initCinfo()
{

  static DestFinfo process( "process",
			    "Handles process call",
			    new ProcOpFunc< DifShellBase>(&DifShellBase::process ) );
  static DestFinfo reinit( "reinit",
			   "Reinit happens only in stage 0",
			   new ProcOpFunc< DifShellBase>( &DifShellBase::reinit ));

  static Finfo* processShared[] = {
    &process, &reinit
  };

  static SharedFinfo proc(
			  "proc",
			  "Shared message to receive Process message from scheduler",
			  processShared, sizeof( processShared ) / sizeof( Finfo* ));




  static DestFinfo reaction( "reaction",
			     "Here the DifShell receives reaction rates (forward and backward), and concentrations for the "
			     "free-buffer and bound-buffer molecules.",
			     new EpFunc4< DifShellBase, double, double, double, double >( &DifShellBase::buffer ));

  static Finfo* bufferShared[] = {
    DifShellBase::concentrationOut(), &reaction
  };

  static SharedFinfo buffer( "buffer",
			     "This is a shared message from a DifShell to a Buffer (FixBuffer or DifBuffer). " ,
			     bufferShared,
			     sizeof( bufferShared ) / sizeof( Finfo* ));
  /////




  static DestFinfo fluxFromOut( "fluxFromOut",
				"Destination message",
				new EpFunc2< DifShellBase, double, double > ( &DifShellBase::fluxFromOut ));

  static Finfo* innerDifShared[] = {
    &fluxFromOut,  DifShellBase::innerDifSourceOut(),
  };
  static SharedFinfo innerDif( "innerDif",
			       "This shared message (and the next) is between DifShells: adjoining shells exchange information to "
			       "find out the flux between them. "
			       "Using this message, an inner shell sends to, and receives from its outer shell." ,
			       innerDifShared,
			       sizeof( innerDifShared ) / sizeof( Finfo* ));

  static DestFinfo fluxFromIn( "fluxFromIn", "",
			       new EpFunc2< DifShellBase, double, double> ( &DifShellBase::fluxFromIn ) );

  static Finfo* outerDifShared[] = {
    &fluxFromIn,  DifShellBase::outerDifSourceOut(),
  };

  static  SharedFinfo outerDif( "outerDif",
				"Using this message, an outer shell sends to, and receives from its inner shell." ,
				outerDifShared,
				sizeof( outerDifShared ) / sizeof( Finfo* ));

  static ElementValueFinfo< DifShellBase, double> C( "C",
						     "Concentration C",// is computed by the DifShell",
						     &DifShellBase::setC,
						     &DifShellBase::getC);
  static ElementValueFinfo< DifShellBase, double> Ceq( "Ceq", "",
						       &DifShellBase::setCeq,
						       &DifShellBase::getCeq);
  static ElementValueFinfo< DifShellBase, double> D( "D", "",
						     &DifShellBase::setD,
						     &DifShellBase::getD);
  static ElementValueFinfo< DifShellBase, double> valence( "valence", "",
							   &DifShellBase::setValence,
							   &DifShellBase::getValence);
  static ElementValueFinfo< DifShellBase, double> leak( "leak", "",
							&DifShellBase::setLeak,
							&DifShellBase::getLeak);
  static ElementValueFinfo< DifShellBase, unsigned int> shapeMode( "shapeMode", "",
								   &DifShellBase::setShapeMode,
								   &DifShellBase::getShapeMode);
  static ElementValueFinfo< DifShellBase, double> length( "length", "",
							  &DifShellBase::setLength,
							  &DifShellBase::getLength);
  static ElementValueFinfo< DifShellBase, double> diameter( "diameter", "",
							    &DifShellBase::setDiameter,
							    &DifShellBase::getDiameter );
  static ElementValueFinfo< DifShellBase, double> thickness( "thickness", "",
							     &DifShellBase::setThickness,
							     &DifShellBase::getThickness );
  static ElementValueFinfo< DifShellBase, double> volume( "volume", "",
							  &DifShellBase::setVolume,
							  &DifShellBase::getVolume );
  static ElementValueFinfo< DifShellBase, double> outerArea( "outerArea", "",
							     &DifShellBase::setOuterArea,
							     &DifShellBase::getOuterArea);
  static ElementValueFinfo< DifShellBase, double> innerArea( "innerArea", "",
							     &DifShellBase::setInnerArea,
							     &DifShellBase::getInnerArea );

  static DestFinfo mmPump( "mmPump", "Here DifShell receives pump outflux",
			   new EpFunc2< DifShellBase, double, double >( &DifShellBase::mmPump ) );
  static DestFinfo influx( "influx", "",
			   new EpFunc1< DifShellBase, double > (&DifShellBase::influx ));
  static DestFinfo outflux( "outflux", "",
			    new EpFunc1< DifShellBase, double >( &DifShellBase::influx ));
  static DestFinfo fInflux( "fInflux", "",
			    new EpFunc2< DifShellBase, double, double >( &DifShellBase::fInflux ));
  static DestFinfo fOutflux( "fOutflux", "",
			     new EpFunc2< DifShellBase, double, double> (&DifShellBase::fOutflux ));
  static DestFinfo storeInflux( "storeInflux", "",
				new EpFunc1< DifShellBase, double >( &DifShellBase::storeInflux ) );
  static DestFinfo storeOutflux( "storeOutflux", "",
				 new EpFunc1< DifShellBase, double > ( &DifShellBase::storeOutflux ) );
  static DestFinfo tauPump( "tauPump","",
			    new EpFunc2< DifShellBase, double, double >( &DifShellBase::tauPump ) );
  static DestFinfo eqTauPump( "eqTauPump", "",
			      new EpFunc1< DifShellBase, double >( &DifShellBase::eqTauPump ) );
  static DestFinfo hillPump( "hillPump", "",
			     new EpFunc3< DifShellBase, double, double, unsigned int >( &DifShellBase::hillPump ) );
  static Finfo* difShellBaseFinfos[] = {
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
    concentrationOut(),
    innerDifSourceOut(),
    outerDifSourceOut(),
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
      "Name", "DifShellBase",
      "Author", "Niraj Dudani. Ported to async13 by Subhasis Ray/Asia Jedrzejewska-Szmek",
      "Description", "DifShell object: Models diffusion of an ion (typically calcium) within an "
      "electric compartment. A DifShell is an iso-concentration region with respect to "
      "the ion. Adjoining DifShells exchange flux of this ion, and also keep track of "
      "changes in concentration due to pumping, buffering and channel currents, by "
      "talking to the appropriate objects.",
    };
  static ZeroSizeDinfo< int > dinfo;
  //static Dinfo< DifShellBase> dinfo;
  static Cinfo difShellBaseCinfo(
				 "DifShellBase",
				 Neutral::initCinfo(),
				 difShellBaseFinfos,
				 sizeof( difShellBaseFinfos ) / sizeof( Finfo* ),
				 &dinfo,
				 doc,
				 sizeof( doc ) / sizeof( string ));

  return &difShellBaseCinfo;
}

//Cinfo *object*  corresponding to the class.
static const Cinfo* difShellBaseCinfo = DifShellBase::initCinfo();

DifShellBase::DifShellBase()
{ ; }

void DifShellBase::setC( const Eref& e, double C)
{
  vSetC(e,C);
}
double DifShellBase::getC(const Eref& e) const
{
  return vGetC(e);
}

void DifShellBase::setCeq( const Eref& e, double Ceq )
{
  vSetCeq(e,Ceq);
}

double DifShellBase::getCeq(const Eref& e ) const
{
  return vGetCeq(e);
}

void DifShellBase::setD(const Eref& e, double D )
{
  vSetD(e,D);
}

double DifShellBase::getD(const Eref& e ) const
{
  return vGetD(e);
}

void DifShellBase::setValence(const Eref& e, double valence )
{
  vSetValence(e,valence);
}

double DifShellBase::getValence(const Eref& e ) const
{
  return vGetValence(e);
}

void DifShellBase::setLeak(const Eref& e, double leak )
{
  vSetLeak(e,leak);
}

double DifShellBase::getLeak(const Eref& e ) const
{
  return vGetLeak(e);
}

void DifShellBase::setShapeMode(const Eref& e, unsigned int shapeMode )
{
  vSetShapeMode(e,shapeMode);
}

unsigned int DifShellBase::getShapeMode(const Eref& e) const
{
  return vGetShapeMode(e);
}

void DifShellBase::setLength(const Eref& e, double length )
{
  vSetLength(e,length);
}

double DifShellBase::getLength(const Eref& e ) const
{
  return vGetLength(e);
}

void DifShellBase::setDiameter(const Eref& e, double diameter )
{
  vSetDiameter(e,diameter);
}

double DifShellBase::getDiameter(const Eref& e ) const
{
  return vGetDiameter(e);
}

void DifShellBase::setThickness( const Eref& e, double thickness )
{
  vSetThickness(e,thickness);
}

double DifShellBase::getThickness(const Eref& e) const
{
  return vGetThickness(e);
}

void DifShellBase::setVolume(const Eref& e, double volume )
{
  vSetVolume(e,volume);
}

double DifShellBase::getVolume(const Eref& e ) const
{
  return vGetVolume(e);
}

void DifShellBase::setOuterArea(const Eref& e, double outerArea )
{
  vSetOuterArea(e,outerArea);
}

double DifShellBase::getOuterArea(const Eref& e ) const
{
  return vGetOuterArea(e);
}

void DifShellBase::setInnerArea(const Eref& e, double innerArea )
{
  vSetInnerArea(e,innerArea);
}

double DifShellBase::getInnerArea(const Eref& e) const
{
  return vGetInnerArea(e);
}

////////////////////////////////////////////////////////////////////////////////
// Dest functions
////////////////////////////////////////////////////////////////////////////////

void DifShellBase::reinit( const Eref& e, ProcPtr p )
{
  vReinit( e, p );
}

void DifShellBase::process( const Eref& e, ProcPtr p )
{
  vProcess( e, p );
}

void DifShellBase::buffer(
			  const Eref& e,
			  double kf,
			  double kb,
			  double bFree,
			  double bBound )
{
  vBuffer( e, kf, kb, bFree, bBound );
}

void DifShellBase::fluxFromOut(const Eref& e,
			       double outerC,
			       double outerThickness )
{
  vFluxFromOut(e, outerC, outerThickness );
}

void DifShellBase::fluxFromIn(
			      const Eref& e,
			      double innerC,
			      double innerThickness )
{
  vFluxFromIn( e, innerC, innerThickness );
}

void DifShellBase::influx(const Eref& e,
			  double I )
{
  vInflux( e, I );
}

void DifShellBase::outflux(const Eref& e,
			   double I )
{
  vOutflux(e, I );
}

void DifShellBase::fInflux(const Eref& e,
			   double I,
			   double fraction )
{
  vFInflux(e, I, fraction );
}

void DifShellBase::fOutflux(const Eref& e,
			    double I,
			    double fraction )
{
  vFOutflux(e, I, fraction );
}

void DifShellBase::storeInflux(const Eref& e,
			       double flux )
{
  vStoreInflux( e, flux );
}

void DifShellBase::storeOutflux(const Eref& e,
				double flux )
{
  vStoreOutflux(e, flux );
}

void DifShellBase::tauPump(const Eref& e,
			   double kP,
			   double Ceq )
{
  vTauPump( e, kP, Ceq );
}

void DifShellBase::eqTauPump(const Eref& e,
			     double kP )
{
  vEqTauPump(e, kP );
}

void DifShellBase::mmPump(const Eref& e,
			  double vMax,
			  double Kd )
{
  vMMPump(e, vMax, Kd );
}

void DifShellBase::hillPump(const Eref& e,
			    double vMax,
			    double Kd,
			    unsigned int hill )
{
  vHillPump(e, vMax, Kd, hill );
}
