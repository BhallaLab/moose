// HDF5WriterBase.h ---
//
// Filename: HDF5WriterBase.h
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Sat Feb 25 14:39:19 2012 (+0530)
// Version:
// Last-Updated: Tue Aug 25 23:11:28 2015 (-0400)
//           By: subha
//     Update #: 57
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
// HDF5WriterBase provides a common interface for writing data/model to file.
//
//

// Change log:
//
// 2012-02-25 14:39:36 (+0530) subha - started initial implementation
//

// Code:

#ifdef USE_HDF5
#ifndef _HDF5IO_H
#define _HDF5IO_H
#include <typeinfo>

hid_t require_attribute(hid_t file_id, string path,
                        hid_t data_type, hid_t data_id);

hid_t require_group(hid_t file_id, string path);

class HDF5WriterBase
{
  public:
    static const hssize_t CHUNK_SIZE;
    HDF5WriterBase();
    virtual ~HDF5WriterBase();
    void setFilename(string filename);
    string getFilename() const;
    bool isOpen() const;
    void setMode(unsigned int mode);
    unsigned int getMode() const;
    void setChunkSize(unsigned int);
    unsigned int getChunkSize() const;
    void setCompressor(string compressor);
    string getCompressor() const;
    void setCompression(unsigned int level);
    unsigned int getCompression() const;
    void setStringAttr(string name, string value);
    void setDoubleAttr(string name, double value);
    void setLongAttr(string name, long value);
    string getStringAttr(string name) const;
    double getDoubleAttr(string name) const;
    long getLongAttr(string name) const;

    void setStringVecAttr(string name, vector < string > value);
    void setDoubleVecAttr(string name, vector < double > value);
    void setLongVecAttr(string name, vector < long > value);
    vector < string > getStringVecAttr(string name) const;
    vector < double > getDoubleVecAttr(string name) const;
    vector < long > getLongVecAttr(string name) const;

    virtual void flushAttributes();
    virtual void flush();
    virtual void close();

    static const Cinfo* initCinfo();

  protected:
    friend void testCreateStringDataset();

    herr_t openFile();
    // C++ sucks - does not allow template specialization inside class
    hid_t createDoubleDataset(hid_t parent, std::string name, hsize_t size=0, hsize_t maxsize=H5S_UNLIMITED);
    hid_t createStringDataset(hid_t parent, std::string name, hsize_t size=0, hsize_t maxsize=H5S_UNLIMITED);

    herr_t appendToDataset(hid_t dataset, const vector<double>& data);
    hid_t createDataset2D(hid_t parent, string name, unsigned int rows);

    /// map from element path to nodes in hdf5file.  Multiple MOOSE
    /// tables can be written to the single file corresponding to a
    /// HDF5Writer. Each one will be represented by a specific data
    /// node in the file.
    map <string, hid_t> nodemap_;
    /// File handle for the HDF5 file opened by this object
    hid_t filehandle_;
    string filename_;
    unsigned int openmode_;
    // We also allow attributes of string, double or long int type on / node
    map<string, string> sattr_;
    map<string, double> dattr_;
    map<string, long> lattr_;
    map<string, vector < string > > svecattr_;
    map<string, vector < double > > dvecattr_;
    map<string, vector < long > > lvecattr_;
    // These are for compressing data
    unsigned int chunkSize_;
    string compressor_; // can be zlib or szip
    unsigned int compression_;

};


template <typename A> herr_t writeScalarAttr(hid_t file_id, string path, A value)
{
    cerr << "This should be never be called."
         << " Specialized version exist for basic types." << endl;
    return -1;
}


////////////////////////////////////////////////////////////
// Write vector attributes
////////////////////////////////////////////////////////////

template <typename A> herr_t writeVectorAttr(hid_t file_id, string path,
                                             vector < A > value)
{
    cerr << "writeVectorAttr: This should be never be called."
         << " Specialized version exist for basic types." << endl;
    return -1;
}

#endif // _HDF5IO_H
#endif // USE_HDF5



//
// HDF5WriterBase.h ends here
