// HDF5WriterBase.cpp --- 
// 
// Filename: HDF5WriterBase.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Created: Sat Feb 25 14:42:03 2012 (+0530)
// Version: 
// Last-Updated: Sun Dec 20 23:20:44 2015 (-0500)
//           By: subha
//     Update #: 298
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change log:
// 
// 
// 

// Code:

#ifdef USE_HDF5

#include <algorithm>
#include <string>
#include <fstream>

#include "hdf5.h"

#include "header.h"
#include "../utility/utility.h"

#include "HDF5WriterBase.h"

using namespace std;

///////////////////////
// Utility functions
///////////////////////
/**
   Create or open attribute at specified path.

   path should be `/node1/node2/.../nodeN/attribute`

   it is always relative to the file root
 */
hid_t require_attribute(hid_t file_id, string path,
                        hid_t data_type, hid_t data_id)
{
    size_t attr_start = path.rfind("/");
    string node_path = ".";
    string attr_name = "";
    if (attr_start == string::npos){
        attr_start = 0;
    } else {
        node_path = path.substr(0, attr_start);
        attr_start += 1;
    }
    attr_name = path.substr(attr_start);   
    if (H5Aexists_by_name(file_id, node_path.c_str(), attr_name.c_str(),
                          H5P_DEFAULT)){
        return H5Aopen_by_name(file_id, node_path.c_str(), attr_name.c_str(),
                               H5P_DEFAULT, H5P_DEFAULT);
    } else {
        return H5Acreate_by_name(file_id, node_path.c_str(), attr_name.c_str(),
                                   data_type, data_id,
                                   H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }   
}

/**
   Create or open group at specified path.
 */
hid_t require_group(hid_t file, string path)
{
    vector<string> pathTokens;
    moose::tokenize(path, "/", pathTokens);
    hid_t prev = file, current;
    htri_t exists;
    // Open the container for the event maps
    for (unsigned int ii = 0; ii < pathTokens.size(); ++ii){
        exists = H5Lexists(prev, pathTokens[ii].c_str(), H5P_DEFAULT);
        if (exists > 0){
            current = H5Gopen2(prev, pathTokens[ii].c_str(), H5P_DEFAULT);
        } else {
            current = H5Gcreate2(prev, pathTokens[ii].c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        }
        if (prev != file){
            if(H5Gclose(prev) < 0){
                return -1;
            }
        }
        if (current < 0){
            return current;
        }
        prev = current;
    }
    return current;
}

/**
   Create a new 1D dataset. Make it extensible.
*/
hid_t HDF5WriterBase::createDoubleDataset(hid_t parent_id, std::string name, hsize_t size, hsize_t maxsize)
{
    herr_t status;
    hsize_t dims[1] = {size};
    hsize_t maxdims[] = {maxsize};
    hsize_t _chunkSize = chunkSize_;
    if (_chunkSize > maxsize){
        _chunkSize = maxsize;
    }
    hsize_t chunk_dims[] = {_chunkSize};
    hid_t chunk_params = H5Pcreate(H5P_DATASET_CREATE);
    status = H5Pset_chunk(chunk_params, 1, chunk_dims);
    assert( status >= 0 );
    if (compressor_ == "zlib"){
        status = H5Pset_deflate(chunk_params, compression_);
    } else if (compressor_ == "szip"){
        // this needs more study
        unsigned sz_opt_mask = H5_SZIP_NN_OPTION_MASK;
        status = H5Pset_szip(chunk_params, sz_opt_mask,
                             HDF5WriterBase::CHUNK_SIZE);
    }
    hid_t dataspace = H5Screate_simple(1, dims, maxdims);            
    hid_t dataset_id = H5Dcreate2(parent_id, name.c_str(),
                                  H5T_NATIVE_DOUBLE, dataspace,
                                  H5P_DEFAULT, chunk_params, H5P_DEFAULT);
    H5Sclose(dataspace);
    H5Pclose(chunk_params);
    return dataset_id;
}

hid_t HDF5WriterBase::createStringDataset(hid_t parent_id, string name, hsize_t size, hsize_t maxsize)
{
    herr_t status;
    hid_t ftype = H5Tcopy(H5T_C_S1);
    if (H5Tset_size(ftype, H5T_VARIABLE) < 0){
        return -1;
    }
    hsize_t dims[] = {size};
    hsize_t maxdims[] = {maxsize};
    hsize_t _chunkSize = chunkSize_;
    if (maxsize < _chunkSize){
        _chunkSize = maxsize;
    }
    hsize_t chunk_dims[] = {_chunkSize};
    hid_t chunk_params = H5Pcreate(H5P_DATASET_CREATE);
    status = H5Pset_chunk(chunk_params, 1, chunk_dims);
    assert( status >= 0 );
    if (compressor_ == "zlib"){
        status = H5Pset_deflate(chunk_params, compression_);
    } else if (compressor_ == "szip"){
        // this needs more study
        unsigned sz_opt_mask = H5_SZIP_NN_OPTION_MASK;
        status = H5Pset_szip(chunk_params, sz_opt_mask,
                             HDF5WriterBase::CHUNK_SIZE);
    }
    hid_t dataspace = H5Screate_simple(1, dims, maxdims);            
    hid_t dataset_id = H5Dcreate2(parent_id, name.c_str(),
                                  ftype, dataspace,
                                  H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Sclose(dataspace);
    H5Tclose(ftype);
    H5Pclose(chunk_params);    
    return dataset_id;
}


/**
   Append a vector to a specified dataset and return the error status
   of the write operation. */
herr_t HDF5WriterBase::appendToDataset(hid_t dataset_id, const vector< double >& data)
{
    herr_t status;
    if (dataset_id < 0){
        return -1;
    }
    hid_t filespace = H5Dget_space(dataset_id);
    if (filespace < 0){
        return -1;
    }
    if (data.size() == 0){
        return 0;
    }
    hsize_t size = H5Sget_simple_extent_npoints(filespace) + data.size();
    status = H5Dset_extent(dataset_id, &size);
    if (status < 0){
        return status;
    }
    filespace = H5Dget_space(dataset_id);
    hsize_t size_increment = data.size();
    hid_t memspace = H5Screate_simple(1, &size_increment, NULL);
    hsize_t start = size - data.size();
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, &start, NULL,
                        &size_increment, NULL);
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, memspace, filespace,
                      H5P_DEFAULT, &data[0]);
    return status;
}


/**
   Create a 2D dataset under parent with name. It will have specified
   number of rows and unlimited columns.
 */
hid_t HDF5WriterBase::createDataset2D(hid_t parent, string name, unsigned int rows)
{
    if (parent < 0){
        return 0;
    }    
    herr_t status;
    // we need chunking here to allow extensibility
    hsize_t chunkdims[] = {rows, chunkSize_};
    hid_t chunk_params = H5Pcreate(H5P_DATASET_CREATE);
    status = H5Pset_chunk(chunk_params, 2, chunkdims);
    assert(status >= 0);
    if (compressor_ == "zlib"){
        status = H5Pset_deflate(chunk_params, compression_);
    } else if (compressor_ == "szip"){
        // this needs more study
        unsigned sz_opt_mask = H5_SZIP_NN_OPTION_MASK;
        status = H5Pset_szip(chunk_params, sz_opt_mask,
                             HDF5WriterBase::CHUNK_SIZE);
    }
    hsize_t dims[2] = {rows, 0};
    hsize_t maxdims[2] = {rows, H5S_UNLIMITED};
    hid_t dataspace = H5Screate_simple(2, dims, maxdims);
    hid_t dset = H5Dcreate2(parent, name.c_str(), H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT, chunk_params, H5P_DEFAULT);
    H5Pclose(chunk_params);
    H5Sclose(dataspace);
    return dset;
}

/**
   Iterate through the path->value map of scalar attributes of type
   `A` and write to HDF5 file handle `file_id`.
 */
template <typename A>
herr_t writeScalarAttributesFromMap(hid_t file_id, map < string, A > path_value_map)
{
    for (typename map< string, A >::const_iterator ii = path_value_map.begin();
         ii != path_value_map.end(); ++ii){
        herr_t status = writeScalarAttr<A>(file_id,
                                           ii->first, ii->second);
        if (status < 0){
            cerr << "Error: writing attribute " << ii->first
                 << " returned status code " << status << endl;
            return status;
        }
    }
    return 0;    
}

/**
   Iterate through the path->value map of vector attributes of type
   `A` and write to HDF5 file handle `file_id`.
 */
template <typename A>
herr_t writeVectorAttributesFromMap(hid_t file_id, map < string, vector < A > > path_value_map)
{
    for (typename map< string, vector < A > >::const_iterator ii = path_value_map.begin();
         ii != path_value_map.end(); ++ii){
        herr_t status = writeVectorAttr<A>(file_id,
                                           ii->first, ii->second);
        if (status < 0){
            cerr << "Error: writing attribute " << ii->first
                 << " returned status code " << status << endl;
            return status;
        }
    }
    return 0;    
}

////////////////////////////////////////////////////////////////////
// Template specializations for scalar attribute writing
////////////////////////////////////////////////////////////////////
template <>
herr_t writeScalarAttr(hid_t file_id, string path, string value)
{
    hid_t data_id = H5Screate(H5S_SCALAR);    
    hid_t dtype = H5Tcopy(H5T_C_S1);
    H5Tset_size(dtype, value.length()+1);
    const char * data = value.c_str();
    hid_t attr_id = require_attribute(file_id, path, dtype, data_id);
    herr_t status =  H5Awrite(attr_id, dtype, data);
    H5Aclose(attr_id);
    return status;
}

template <>
herr_t writeScalarAttr(hid_t file_id, string path, double value)
{
    hid_t data_id = H5Screate(H5S_SCALAR);    
    hid_t dtype = H5T_NATIVE_DOUBLE;
    hid_t attr_id = require_attribute(file_id, path, dtype, data_id);
    herr_t status = H5Awrite(attr_id, dtype, (void*)(&value));    
    H5Aclose(attr_id);
    return status;
}

template <>
herr_t writeScalarAttr(hid_t file_id, string path,  long value)
{
    hid_t data_id = H5Screate(H5S_SCALAR);    
    hid_t dtype = H5T_NATIVE_LONG;
    hid_t attr_id = require_attribute(file_id, path, dtype, data_id);
    herr_t status = H5Awrite(attr_id, dtype, (void*)(&value));
    H5Aclose(attr_id);
    return status;
}

template <>
herr_t writeScalarAttr(hid_t file_id, string path, int value)
{
    hid_t data_id = H5Screate(H5S_SCALAR);    
    hid_t dtype = H5T_NATIVE_INT;
    hid_t attr_id = require_attribute(file_id, path, dtype, data_id);
    herr_t status =  H5Awrite(attr_id, dtype, (void*)(&value));
    H5Aclose(attr_id);
    return status;
}

////////////////////////////////////////////////////////////////////
// Template specializations for vector attribute writing
////////////////////////////////////////////////////////////////////

template <>
herr_t writeVectorAttr(hid_t file_id, string path, vector < string > value)
{
    hsize_t dims[] = {value.size()};
    hid_t space = H5Screate_simple(1, dims, NULL);
    hid_t dtype = H5Tcopy(H5T_C_S1);
    H5Tset_size(dtype, H5T_VARIABLE);    
    const char ** data = (const char **)calloc(value.size(),
                                   sizeof(const char*));
    for (unsigned int ii = 0; ii < value.size(); ++ii){
        data[ii] = value[ii].c_str();
    }
    hid_t attr_id = require_attribute(file_id, path, dtype, space);
    herr_t status = H5Awrite(attr_id, dtype, data);
    free(data);
    H5Aclose(attr_id);
    return status;
}

template <>
herr_t writeVectorAttr(hid_t file_id, string path, vector < double > value)
{
    hsize_t dims[] = {value.size()};
    hid_t data_id = H5Screate_simple(1, dims, NULL);    
    hid_t dtype = H5T_NATIVE_DOUBLE;
    H5Tset_size(dtype, value.size());
    void * data = &value[0];
    hid_t attr_id = require_attribute(file_id, path, dtype, data_id);
    herr_t status = H5Awrite(attr_id, dtype, data);
    H5Aclose(attr_id);
    return status;
}

template <>
herr_t writeVectorAttr(hid_t file_id, string path, vector < long > value)
{
    hsize_t dims[] = {value.size()};
    hid_t data_id = H5Screate_simple(1, dims, NULL);    
    hid_t dtype = H5T_NATIVE_LONG;
    H5Tset_size(dtype, value.size());
    void * data = &value[0];
    hid_t attr_id = require_attribute(file_id, path, dtype, data_id);
    herr_t status = H5Awrite(attr_id, dtype, data);
    H5Aclose(attr_id);
    return status;
}



const Cinfo* HDF5WriterBase::initCinfo()
{

  //////////////////////////////////////////////////////////////
  // Field Definitions
  //////////////////////////////////////////////////////////////
  static ValueFinfo< HDF5WriterBase, string > fileName(
      "filename",
      "Name of the file associated with this HDF5 writer object.",
      &HDF5WriterBase::setFilename,
      &HDF5WriterBase::getFilename);
  
  static ReadOnlyValueFinfo < HDF5WriterBase, bool > isOpen(
      "isOpen",
      "True if this object has an open file handle.",
      &HDF5WriterBase::isOpen);

  static ValueFinfo <HDF5WriterBase, unsigned int > mode(
      "mode",
      "Depending on mode, if file already exists, if mode=1, data will be"
      " appended to existing file, if mode=2, file will be truncated, if "
      " mode=4, no writing will happen.",
      &HDF5WriterBase::setMode,
      &HDF5WriterBase::getMode);

  static ValueFinfo< HDF5WriterBase, unsigned int> chunkSize(
      "chunkSize",
      "Chunksize for writing array data. Defaults to 100.",
      &HDF5WriterBase::setChunkSize,
      &HDF5WriterBase::getChunkSize);

  static ValueFinfo< HDF5WriterBase, string> compressor(
      "compressor",
      "Compression type for array data. zlib and szip are supported. Defaults to zlib.",
      &HDF5WriterBase::setCompressor,
      &HDF5WriterBase::getCompressor);
  
  static ValueFinfo< HDF5WriterBase, unsigned int> compression(
      "compression",
      "Compression level for array data. Defaults to 6.",
      &HDF5WriterBase::setCompression,
      &HDF5WriterBase::getCompression);

  static LookupValueFinfo< HDF5WriterBase, string, string  > sattr(
      "stringAttr",
      "String attributes. The key is attribute name, value is attribute value"
      " (string).",
      &HDF5WriterBase::setStringAttr,
      &HDF5WriterBase::getStringAttr);
  
  static LookupValueFinfo< HDF5WriterBase, string, double > dattr(
      "doubleAttr",
      "Double precision floating point attributes. The key is attribute name,"
      " value is attribute value (double).",
      &HDF5WriterBase::setDoubleAttr,
      &HDF5WriterBase::getDoubleAttr);
  
  static LookupValueFinfo< HDF5WriterBase, string, long > lattr(
      "longAttr",
      "Long integer attributes. The key is attribute name, value is attribute"
      " value (long).",
      &HDF5WriterBase::setLongAttr,
      &HDF5WriterBase::getLongAttr);
  
  static LookupValueFinfo< HDF5WriterBase, string, vector < string >  > svecattr(
      "stringVecAttr",
      "String vector attributes. The key is attribute name, value is attribute value (string).",
      &HDF5WriterBase::setStringVecAttr,
      &HDF5WriterBase::getStringVecAttr);
  
  static LookupValueFinfo< HDF5WriterBase, string, vector < double > > dvecattr(
      "doubleVecAttr",
      "Double vector attributes. The key is attribute name, value is"
      " attribute value (vector of double).",
      &HDF5WriterBase::setDoubleVecAttr,
      &HDF5WriterBase::getDoubleVecAttr);
  
  static LookupValueFinfo< HDF5WriterBase, string, vector < long > > lvecattr(
      "longVecAttr",
      "Long integer vector attributes. The key is attribute name, value is"
      " attribute value (vector of long).",
      &HDF5WriterBase::setLongVecAttr,
      &HDF5WriterBase::getLongVecAttr);
  static DestFinfo flush(
      "flush",
      "Write all buffer contents to file and clear the buffers.",
      new OpFunc0 < HDF5WriterBase > ( &HDF5WriterBase::flush ));

  static DestFinfo close(
      "close",
      "Close the underlying file. This is a safety measure so that file is not in an invalid state even if a crash happens at exit.",
      new OpFunc0< HDF5WriterBase > ( & HDF5WriterBase::close ));
      

  static Finfo * finfos[] = {
    &fileName,
    &isOpen,
    &mode,
    &chunkSize,
    &compressor,
    &compression,
    &sattr,
    &dattr,
    &lattr,
    &svecattr,
    &dvecattr,
    &lvecattr,
    &flush,
    &close,
  };
  static string doc[] = {
    "Name", "HDF5WriterBase",
    "Author", "Subhasis Ray",
    "Description", "HDF5 file writer base class. This is not to be used directly. Instead,"
    " it should be subclassed to provide specific data writing functions."
    " This class provides most basic properties like filename, file opening"
    " mode, file open status."
  };


	static Dinfo< HDF5WriterBase > dinfo;
  static Cinfo hdf5Cinfo(
      "HDF5WriterBase",
      Neutral::initCinfo(),
      finfos,
      sizeof(finfos)/sizeof(Finfo*),
	  &dinfo,
      doc, sizeof(doc)/sizeof(string));
  return &hdf5Cinfo;                
}

const hssize_t HDF5WriterBase::CHUNK_SIZE = 1024; // default chunk size


HDF5WriterBase::HDF5WriterBase():
        filehandle_(-1),
        filename_("moose_output.h5"),
        openmode_(H5F_ACC_EXCL),
        chunkSize_(CHUNK_SIZE),
        compressor_("zlib"),
        compression_(6)
{
}

HDF5WriterBase::~HDF5WriterBase()
{
    // derived classes should flush data in their own destructors
    close();
}

void HDF5WriterBase::setFilename(string filename)
{
    if (filename_ == filename){
        return;
    }
     
    // // If file is open, close it before changing filename
    // if (filehandle_ >= 0){
    //     status = H5Fclose(filehandle_);
    //     if (status < 0){
    //         cerr << "Error: failed to close HDF5 file handle for " << filename_ << ". Error code: " << status << endl;
    //     }
    // }
    // filehandle_ = -1;
    filename_ = filename;
    // status = openFile(filename);
}

string HDF5WriterBase::getFilename() const
{
    return filename_;
}

bool HDF5WriterBase::isOpen() const
{
    return filehandle_ >= 0;
}

herr_t HDF5WriterBase::openFile()
{
    herr_t status = 0;
    if (filehandle_ >= 0){
        cout << "Warning: closing already open file and opening " << filename_ <<  endl;
        status = H5Fclose(filehandle_);
        filehandle_ = -1;
        if (status < 0){
            cerr << "Error: failed to close currently open HDF5 file. Error code: " << status << endl;
            return status;
        }
    }
    hid_t fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    // Ensure that all open objects are closed before the file is closed    
    H5Pset_fclose_degree(fapl_id, H5F_CLOSE_STRONG);
    ifstream infile(filename_.c_str());
    bool fexists = infile.good();
    infile.close();
    if (!fexists || openmode_ == H5F_ACC_TRUNC){
        filehandle_ = H5Fcreate(filename_.c_str(), openmode_, H5P_DEFAULT, fapl_id);
    } else if (openmode_ == H5F_ACC_RDWR) {
        filehandle_ = H5Fopen(filename_.c_str(), openmode_, fapl_id);
    } else {
        cerr << "Error: File \"" << filename_ << "\" already exists. Specify mode=" << H5F_ACC_RDWR
             << " for appending to it, mode=" << H5F_ACC_TRUNC
             << " for overwriting it. mode=" << H5F_ACC_EXCL
             << " requires the file does not exist." << endl;
        return -1;
    }
    if (filehandle_ < 0){
        cerr << "Error: Could not open file for writing: " << filename_ << endl;
        status = -1;
    }
    return status;
}

void HDF5WriterBase::setMode(unsigned int mode)
{
    if (mode == H5F_ACC_RDWR || mode == H5F_ACC_TRUNC || mode == H5F_ACC_EXCL){
        openmode_ = mode;
    }
}

unsigned HDF5WriterBase::getMode() const
{
    return openmode_;
}

void HDF5WriterBase::setChunkSize(unsigned int size)
{
    chunkSize_ = size;
}

unsigned int HDF5WriterBase::getChunkSize() const
{
    return chunkSize_;
}

void HDF5WriterBase::setCompressor(string name)
{
    compressor_ = name;
    std::transform(compressor_.begin(), compressor_.end(), compressor_.begin(), ::tolower);
}

string HDF5WriterBase::getCompressor() const
{
    return compressor_;
}

void HDF5WriterBase::setCompression(unsigned int level)
{
    compression_ = level;
}

unsigned int HDF5WriterBase::getCompression() const
{
    return compression_;
}

        
// Subclasses should reimplement this for flushing data content to
// file.
void HDF5WriterBase::flush()
{
    flushAttributes();
    sattr_.clear();
    dattr_.clear();
    lattr_.clear();
    svecattr_.clear();
    dvecattr_.clear();
    lvecattr_.clear();
}

void HDF5WriterBase::flushAttributes()
{
    if (filehandle_ < 0){
        return;
    }    
    // Write all scalar attributes
    writeScalarAttributesFromMap< string >(filehandle_, sattr_);
    writeScalarAttributesFromMap< double >(filehandle_, dattr_);
    writeScalarAttributesFromMap< long >(filehandle_, lattr_);
    // Write the vector attributes
    writeVectorAttributesFromMap< string >(filehandle_, svecattr_);
    writeVectorAttributesFromMap< double >(filehandle_, dvecattr_);
    writeVectorAttributesFromMap< long >(filehandle_, lvecattr_);

}
void HDF5WriterBase::close()
{
    if (filehandle_ < 0){
        return;
    }
    flush();
    herr_t status = H5Fclose(filehandle_);
    filehandle_ = -1;
    if (status < 0){
        cerr << "Error: closing file returned status code=" << status << endl;
    }
}

void HDF5WriterBase::setStringAttr(string name, string value)
{
    sattr_[name] = value;
}

void HDF5WriterBase::setDoubleAttr(string name, double value)
{
    dattr_[name] = value;
}

void HDF5WriterBase::setLongAttr(string name, long value)
{
    lattr_[name] = value;
}

string HDF5WriterBase::getStringAttr(string name) const
{
    map <string, string>::const_iterator ii = sattr_.find(name);
    if (ii != sattr_.end()){
        return ii->second;
    }
    cerr << "Error: no attribute named " << name << endl;
    return "";
}

double HDF5WriterBase::getDoubleAttr(string name) const
{
    map <string, double>::const_iterator ii = dattr_.find(name);
    if (ii != dattr_.end()){
        return ii->second;
    }
    cerr << "Error: no attribute named " << name << endl;
    return 0.0;
}

long HDF5WriterBase::getLongAttr(string name) const
{
    map <string, long>::const_iterator ii = lattr_.find(name);
    if (ii != lattr_.end()){
        return ii->second;
    }
    cerr << "Error: no attribute named " << name << endl;
    return 0;
}

///////////////////////////////////////////////////
// Vector attributes
///////////////////////////////////////////////////

void HDF5WriterBase::setStringVecAttr(string name, vector < string > value)
{
    svecattr_[name] = value;
}

void HDF5WriterBase::setDoubleVecAttr(string name, vector < double > value)
{
    dvecattr_[name] = value;
}

void HDF5WriterBase::setLongVecAttr(string name, vector < long  >value)
{
    lvecattr_[name] = value;
}

vector < string > HDF5WriterBase::getStringVecAttr(string name) const
{
    map <string, vector < string > >::const_iterator ii = svecattr_.find(name);
    if (ii != svecattr_.end()){
        return ii->second;
    }
    cerr << "Error: no attribute named " << name << endl;
    return vector<string>();
}

vector < double > HDF5WriterBase::getDoubleVecAttr(string name) const
{
    map <string, vector < double > >::const_iterator ii = dvecattr_.find(name);
    if (ii != dvecattr_.end()){
        return ii->second;
    }
    cerr << "Error: no attribute named " << name << endl;
    return vector<double>();
}

vector < long > HDF5WriterBase::getLongVecAttr(string name) const
{
    map <string, vector < long > >::const_iterator ii = lvecattr_.find(name);
    if (ii != lvecattr_.end()){
        return ii->second;
    }
    cerr << "Error: no attribute named " << name << endl;
    return vector<long>();
;
}



#endif // USE_HDF5
// 
// HDF5WriterBase.cpp ends here
