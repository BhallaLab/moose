/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Messaging Object Oriented Simulation Environment,
 ** also known as GENESIS 3 base code.
 **           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/


#include "../basecode/header.h"
#include <fstream>
#include "TableBase.h"
#include "TimeTable.h"

static SrcFinfo1< double > *eventOut() {
    static SrcFinfo1< double > eventOut(
        "eventOut",
        "Sends out spike time if it falls in current timestep.");
    return &eventOut;
}

const Cinfo* TimeTable::initCinfo()
{
    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////
    static ValueFinfo< TimeTable, string > filename( "filename",
                                            "File to read lookup data from. The file should be contain two columns\n"
                                            "separated by any space character.",
                                            &TimeTable::setFilename,
                                            &TimeTable::getFilename);

    static ValueFinfo< TimeTable, int > method( "method",
                                                "Method to use for filling up the entries. Currently only method 4\n"
                                                "(loading from file) is supported.",
                                                &TimeTable::setMethod ,
                                                &TimeTable::getMethod);

    static ReadOnlyValueFinfo <TimeTable, double> state( "state",
                                                         "Current state of the time table.",
                                                         &TimeTable::getState );

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////
    static DestFinfo process("process",
                             "Handle process call",
                             new ProcOpFunc< TimeTable >( &TimeTable::process));

    static DestFinfo reinit("reinit",
                            "Handles reinit call",
                            new ProcOpFunc< TimeTable > ( &TimeTable::reinit));

    //////////////////////////////////////////////////////////////
    // SharedMsg Definitions
    //////////////////////////////////////////////////////////////
    static Finfo* procShared[] = {
        &process, &reinit
    };

    static SharedFinfo proc( "proc",
                             "Shared message for process and reinit",
                             procShared, sizeof( procShared ) / sizeof( const Finfo* ));

    static Finfo * timeTableFinfos[] = {
        &filename,
        &method,
        &state,
        eventOut(),
        &proc,
    };

    static string doc[] = {
        "Name", "TimeTable",
        "Author", "Johannes Hjorth, 2008, KTH, Stockholm. Ported to buildQ branch using new API by Subhasis Ray, NCBS, Bangalore, 2013.",
        "Description", "TimeTable: Read in spike times from file and send out eventOut messages\n"
        "at the specified times.",
    };

	static Dinfo< TimeTable > dinfo;
    static Cinfo timeTableCinfo(
        "TimeTable",
        TableBase::initCinfo(),
        timeTableFinfos,
        sizeof( timeTableFinfos )/sizeof(Finfo *),
		&dinfo,
        doc,
        sizeof(doc)/sizeof(string));

    return &timeTableCinfo;
}

static const Cinfo* timeTableCinfo = TimeTable::initCinfo();

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

TimeTable::TimeTable()
  :
  filename_(""),
  state_( 0.0 ),
  curPos_( 0 ),
  method_( 4 )
{ ; }

TimeTable::~TimeTable()
{ ; }

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

/* Filename */
string TimeTable::getFilename() const
{
  return filename_;
}

void TimeTable::setFilename( string filename )
{
  filename_ = filename;

  std::ifstream fin( filename_.c_str() );
  string line;

  if( !fin.good()) {
    cout << "Error: TimeTable::innerload: Unable to open file"
         << filename_ << endl;
  }

  //~ If lines need to be skipped:
  //~ for(unsigned int i = 0; (i < skipLines) & fin.good() ; i++)
    //~ getline( fin, line );

  vec().clear();

  double dataPoint, dataPointOld = -1000;
  while( fin >> dataPoint ) {
      vec().push_back(dataPoint);

    if(dataPoint < dataPointOld) {
      cerr << "TimeTable: Warning: Spike times in file " << filename_
           << " are not in increasing order."
           << endl;
    }

    dataPointOld = dataPoint;
  }
}

/* Method */
void TimeTable::setMethod(int method )
{
  if ( method != 4 ) {
    cerr <<
      "Error: TimeTable::setMethod: "
      "Currently only method 4 (loading from file) supported.\n";
    return;
  }
  method_ = method;
}

int TimeTable::getMethod() const
{
  return method_;
}

/* state */
double TimeTable::getState() const
{
  return state_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void TimeTable::reinit(const Eref& e, ProcPtr p)
{
  curPos_ = 0;
  state_ = 0;
}

void TimeTable::process(const Eref& e, ProcPtr p)
{

  // Two ways of telling the world about the spike events, both
  // happening in parallel
  //
  // state is a continous variable, switching from 0 to 1 when spiking
  // event is an event, that happens at the time of a spike
  //

  state_ = 0;

  if ( curPos_ < vec().size() &&
       p->currTime >= vec()[curPos_] ) {
      eventOut()->send( e, vec()[curPos_]);
      curPos_++;
      state_ = 1;
  }
}
