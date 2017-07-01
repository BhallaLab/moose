// HDF5DataWriter.h ---
//
// Filename: HDF5DataWriter.h
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Sat Feb 25 15:47:23 2012 (+0530)
// Version:
// Last-Updated: Sun Dec 20 23:17:22 2015 (-0500)
//           By: subha
//     Update #: 45
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
// Specialization of HDF5WriterBase to collect and save data from
// moose model. The data is appended to an existing dataset at each
// process step. An explicit writing is also allowed via the flush
// command.  As soon as the data from a table is written to file, the
// table is cleared.
//

// Change log:
//
// 2012-02-25 15:50:03 (+0530) Subha - started coding this class
//
// Thu Jul 31 16:56:47 IST 2014 Subha - updating to use new API on
// async13 branch.


// Code:
#ifdef USE_HDF5
#ifndef _HDF5DATAWRITER_H
#define _HDF5DATAWRITER_H

#include "HDF5WriterBase.h"

class HDF5DataWriter: public HDF5WriterBase
{
  public:
    HDF5DataWriter();
    virtual ~HDF5DataWriter();
    void setFlushLimit(unsigned int limit);
    unsigned int getFlushLimit() const;
    // void flush();
    void process(const Eref &e, ProcPtr p);
    void reinit(const Eref &e, ProcPtr p);
    virtual void flush();
    virtual void close();
    static const Cinfo* initCinfo();
  protected:
    unsigned int flushLimit_;
    // Maps the paths of data sources to vectors storing the data
    // locally
    vector <ObjId> src_;
    vector <vector < double > > data_;
    vector <string> func_;
    vector <hid_t> datasets_;
    unsigned long steps_;
    hid_t getDataset(string path);
};
#endif // _HDF5DATAWRITER_H
#endif // USE_HDF5

//
// HDF5DataWriter.h ends here
