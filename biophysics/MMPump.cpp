#include "../basecode/header.h"
#include "MMPump.h"
#include "../basecode/ElementValueFinfo.h"
#include "../utility/numutil.h"
#include <cmath>

SrcFinfo2< double,double >* MMPump::PumpOut()
{
  static SrcFinfo2< double,double > pumpOut( "PumpOut",
				    "Sends out MMPump parameters.");
  return &pumpOut;
}

const Cinfo * MMPump::initCinfo()
{
  static DestFinfo process( "process",
			    "Handles process call",
			    new ProcOpFunc< MMPump>(&MMPump::process ) );
  static DestFinfo reinit( "reinit",
			   "Reinit happens only in stage 0",
			   new ProcOpFunc< MMPump>( &MMPump::reinit ));

  static Finfo* processShared[] = {
    &process, &reinit
  };

  static SharedFinfo proc(
			  "proc",
			  "Shared message to receive Process message from scheduler",
			  processShared, sizeof( processShared ) / sizeof( Finfo* ));


  ////////////////////////////
  // Field defs
  ////////////////////////////



  static ElementValueFinfo<MMPump, double> Vmax("Vmax",
						  "maximum pump velocity, scaled by mebrane"
						    "surface area. i.e., max ion flux in moles/sec",
						  &MMPump::setVmax,
						  &MMPump::getVmax);

  static ElementValueFinfo<MMPump, double> Kd("Kd",
						 "half-maximal activating concentration in mM",
						 &MMPump::setKd,
						 &MMPump::getKd);

  ////
  // DestFinfo
  ////
  static Finfo * difMMPumpFinfos[] = {
    //////////////////////////////////////////////////////////////////
    // Field definitions
    //////////////////////////////////////////////////////////////////


    &Vmax,
    &Kd,
    //////////////////////////////////////////////////////////////////
    // SharedFinfo definitions
    /////////////////////////////////////////////////////////////////

    &proc,
    PumpOut(),
    //////////////////////////////////////////////////////////////////
    // DestFinfo definitions
    //////////////////////////////////////////////////////////////////

  };

  static string doc[] = {
    "Name", "MMPump",
    "Author", "Subhasis Ray (ported from GENESIS2)",
    "Description", "Models Michaelis-Menten pump. It is coupled with a DifShell.",
  };
  static ZeroSizeDinfo<int> dinfo;
  static Cinfo MMPumpCinfo(
			      "MMPump",
			      Neutral::initCinfo(),
			      difMMPumpFinfos,
			      sizeof(difMMPumpFinfos)/sizeof(Finfo*),
			      &dinfo,
			      doc,
			      sizeof(doc)/sizeof(string));

  return &MMPumpCinfo;
}

static const Cinfo * MMpumpCinfo = MMPump::initCinfo();


////////////////////////////////////////////////////////////////////////////////
// Class functions
////////////////////////////////////////////////////////////////////////////////

MMPump::MMPump()
{ ; }




double MMPump::getVmax(const Eref& e) const
{

  return Vmax_;
}
void MMPump::setVmax(const Eref& e,double value)
{
if ( value  < 0.0 ) {
    cerr << "Error: MMPump: Vmax cannot be negative!\n";
    return;
  }
  Vmax_ = value;
}


double MMPump::getKd(const Eref& e) const
{
  return Kd_;
}
void MMPump::setKd(const Eref& e,double value)
{
if ( value  < 0.0 ) {
    cerr << "Error: MMPump: Kd cannot be negative!\n";
    return;
  }
  Kd_ = value;
}


void MMPump::process(const Eref& e, ProcPtr p)
{
  PumpOut()->send(e,Vmax_,Kd_);
}

void MMPump::reinit(const Eref& e, ProcPtr p)
{
  return;
}
