/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _TIME_TABLE_H
#define _TIME_TABLE_H
class TimeTable: public TableBase
{
  public:
    TimeTable();
    ~TimeTable();

    /* Functions to set and get TimeTable fields */
    void setFilename(string filename );
    string getFilename() const;

    void setMethod(int method );
    int getMethod() const;

    double getState() const;

    /* Dest functions */
    /**
     * The process function called by scheduler on every tick
     */
    void process(const Eref& e, ProcPtr p);

    /**
     * The reinit function called by scheduler for the reset
     */
    void reinit(const Eref& e, ProcPtr p);

    static const Cinfo * initCinfo();

  private:
    /*
     * Fields
     */
    string filename_;

    /* The table with (spike)times */
    vector < double > timeTable_;

    /* 1 if it spiked in current step, 0 otherwise */
    double state_;

    /* Current position within the table */
    unsigned int curPos_;

    /* How to fill the timetable,
       currently only 4 = reading from ASCII file is supported */
    int method_;

};
#endif
