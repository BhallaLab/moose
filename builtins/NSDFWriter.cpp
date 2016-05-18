// NSDFWriter.cpp --- 
// 
// Filename: NSDFWriter.cpp
// Description: 
// Author: subha
// Maintainer: 
// Created: Thu Jun 18 23:16:11 2015 (-0400)
// Version: 
// Last-Updated: Sun Dec 20 23:20:19 2015 (-0500)
//           By: subha
//     Update #: 49
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
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
// 
// 

// Code:
#ifdef USE_HDF5

#include "hdf5.h"
#include "hdf5_hl.h"

#include <ctime>
#include <deque>

#include "header.h"
#include "../utility/utility.h"

#include "HDF5WriterBase.h"
#include "HDF5DataWriter.h"

#include "InputVariable.h"
#include "NSDFWriter.h"

extern template herr_t writeScalarAttr(hid_t file_id, string path, string value);
extern template herr_t writeScalarAttr(hid_t file_id, string path, double value);

const char* const EVENTPATH = "/data/event";
const char* const UNIFORMPATH = "/data/uniform";
const char* const MODELTREEPATH = "/model/modeltree";
const char* const MAPUNIFORMSRC = "/map/uniform";
const char* const MAPEVENTSRC = "/map/event";

string iso_time(time_t * t)
{
    struct tm * timeinfo;
    if (t == NULL){
        time_t current;
        std::time(&current);
        timeinfo = std::gmtime(&current);
    } else {
        timeinfo = std::gmtime(t);
    }
    assert(timeinfo != NULL);
    char buf[32];
    strftime(buf, 32, "%FT%T", timeinfo);
    return string(buf);
}

const Cinfo * NSDFWriter::initCinfo()
{    
    static FieldElementFinfo< NSDFWriter, InputVariable > eventInputFinfo(
        "eventInput",
        "Sets up field elements for event inputs",
        InputVariable::initCinfo(),
        &NSDFWriter::getEventInput,
        &NSDFWriter::setNumEventInputs,
        &NSDFWriter::getNumEventInputs);

    static ValueFinfo <NSDFWriter, string > modelRoot(
      "modelRoot",
      "The moose element tree root to be saved under /model/modeltree",
      &NSDFWriter::setModelRoot,
      &NSDFWriter::getModelRoot);
    
    static DestFinfo process(
        "process",
        "Handle process calls. Collects data in buffer and if number of steps"
        " since last write exceeds flushLimit, writes to file.",
        new ProcOpFunc<NSDFWriter>( &NSDFWriter::process));

    static  DestFinfo reinit(
        "reinit",
        "Reinitialize the object. If the current file handle is valid, it tries"
        " to close that and open the file specified in current filename field.",
        new ProcOpFunc<NSDFWriter>( &NSDFWriter::reinit ));

    static Finfo * processShared[] = {
        &process, &reinit
    };
    
    static SharedFinfo proc(
        "proc",
        "Shared message to receive process and reinit",
        processShared, sizeof( processShared ) / sizeof( Finfo* ));

    static Finfo * finfos[] = {
        &eventInputFinfo,
        &proc,
    };

    static string doc[] = {
        "Name", "NSDFWriter",
        "Author", "Subhasis Ray",
        "Description", "NSDF file writer for saving data."
    };

    static Dinfo< NSDFWriter > dinfo;
    static Cinfo cinfo(
        "NSDFWriter",
        HDF5DataWriter::initCinfo(),
        finfos,
        sizeof(finfos)/sizeof(Finfo*),
        &dinfo,
	doc, sizeof( doc ) / sizeof( string ));

    return &cinfo;
}

static const Cinfo * nsdfWriterCinfo = NSDFWriter::initCinfo();

NSDFWriter::NSDFWriter(): eventGroup_(-1), uniformGroup_(-1), dataGroup_(-1), modelGroup_(-1), mapGroup_(-1), modelRoot_("/")
{
    ;
}

NSDFWriter::~NSDFWriter()
{
    close();
}

void NSDFWriter::close()
{
    if (filehandle_ < 0){
        return;
    }
    flush();
    closeUniformData();
    if (uniformGroup_ >= 0){
        H5Gclose(uniformGroup_);
    }
    closeEventData();
    if (eventGroup_ >= 0){
        H5Gclose(eventGroup_);
    }
    if (dataGroup_ >= 0){
        H5Gclose(dataGroup_);
    }
    HDF5DataWriter::close();
}

void NSDFWriter::closeUniformData()
{
    for (map < string, hid_t>::iterator ii = classFieldToUniform_.begin();
         ii != classFieldToUniform_.end();
         ++ii){
        if (ii->second >= 0){
            H5Dclose(ii->second);
        }
    }
    vars_.clear();
    data_.clear();
    src_.clear();
    func_.clear();
    datasets_.clear();

}

void NSDFWriter::sortOutUniformSources(const Eref& eref)
{
    vars_.clear();
    classFieldToSrcIndex_.clear();
    objectField_.clear();
    objectFieldToIndex_.clear();
    const SrcFinfo * requestOut = (SrcFinfo*)eref.element()->cinfo()->findFinfo("requestOut");
    unsigned int numTgt = eref.element()->getMsgTargetAndFunctions(eref.dataIndex(),
                                                                requestOut,
                                                                src_,
                                                                func_);
    assert(numTgt ==  src_.size());
    /////////////////////////////////////////////////////////////
    // Go through all the sources and determine the index of the
    // source message in the dataset
    /////////////////////////////////////////////////////////////
    
    for (unsigned int ii = 0; ii < func_.size(); ++ii){
        string varname = func_[ii];
        size_t found = varname.find("get");
        if (found == 0){
            varname = varname.substr(3);
            if (varname.length() == 0){
                varname = func_[ii];
            } else {
                varname[0] = tolower(varname[0]);
            }
        }
        assert(varname.length() > 0);
        string className = Field<string>::get(src_[ii], "className");
        string datasetPath = className + "/"+ varname;
        classFieldToSrcIndex_[datasetPath].push_back(ii);
        vars_.push_back(varname);
    }
    data_.resize(numTgt);
}

/**
   Handle the datasets for the requested fields (connected to
   requestOut). This is is similar to what HDF5DataWriter does. 
 */
void NSDFWriter::openUniformData(const Eref &eref)
{
    sortOutUniformSources(eref);
    htri_t exists;
    herr_t status;
    if (uniformGroup_ < 0){
        uniformGroup_ = require_group(filehandle_, UNIFORMPATH);
    }

    // create the datasets and map them to className/field
    for (map< string, vector< unsigned int > >::iterator it = classFieldToSrcIndex_.begin();
         it != classFieldToSrcIndex_.end();
         ++it){
        vector< string > tokens;
        moose::tokenize(it->first, "/", tokens);
        string className = tokens[0];
        string fieldName = tokens[1];
        hid_t container = require_group(uniformGroup_, className);
        vector < string > srclist;
        hid_t dataset = createDataset2D(container, fieldName.c_str(), it->second.size());
        classFieldToUniform_[it->first] = dataset;
        writeScalarAttr<string>(dataset, "field", fieldName);
        H5Gclose(container);
    }
}

/**
   create the DS for uniform data.
 */
void NSDFWriter::createUniformMap()
{
    // Create the container for all the DS
    // TODO: make a common function like `mkdir -p` to avoid repeating this
    htri_t exists;
    herr_t status;
    hid_t uniformMapContainer = require_group(filehandle_, MAPUNIFORMSRC);
    // Create the DS themselves
    for (map< string, vector < unsigned int > >::iterator ii = classFieldToSrcIndex_.begin();
         ii != classFieldToSrcIndex_.end(); ++ii){
        vector < string > pathTokens;
        moose::tokenize(ii->first, "/", pathTokens);
        string className = pathTokens[0];
        string fieldName = pathTokens[1];
        hid_t container = require_group(uniformMapContainer, className);
        char ** sources = (char **)calloc(ii->second.size(), sizeof(char*));
        for (unsigned int jj = 0; jj < ii->second.size(); ++jj){
            sources[jj] = (char*)calloc(src_[ii->second[jj]].path().length()+1, sizeof(char));
            strcpy(sources[jj],src_[ii->second[jj]].path().c_str());
        }
        hid_t ds = createStringDataset(container, fieldName, (hsize_t)ii->second.size(), (hsize_t)ii->second.size());
        hid_t memtype = H5Tcopy(H5T_C_S1);
        status = H5Tset_size(memtype, H5T_VARIABLE);
        assert(status >= 0);
        status = H5Dwrite(ds, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, sources);
#ifndef NDEBUG        
        cout << "Write dataset: status=" << status << endl;
#endif
        assert(status >= 0);
        for (unsigned int jj = 0; jj < ii->second.size(); ++jj){
            free(sources[jj]);
        }
        free(sources);
        status = H5DSset_scale(ds, "source");
        status = H5DSattach_scale(classFieldToUniform_[ii->first], ds, 0);
        status = H5DSset_label(classFieldToUniform_[ii->first], 0, "source");
        status = H5Dclose(ds);
        status = H5Tclose(memtype);                          
    } 
}

void NSDFWriter::closeEventData()
{
    for (unsigned int ii = 0; ii < eventDatasets_.size(); ++ii){
        if (eventDatasets_[ii] >= 0){
            H5Dclose(eventDatasets_[ii]);
        }
    }
    events_.clear();
    eventInputs_.clear();            
    eventDatasets_.clear();
    eventSrc_.clear();
    eventSrcFields_.clear();
}

/**
   Populates the vector of event data buffers (vectors), vector of
   event source objects, vector of event source fields and the vector
   of event datasets by querying the messages on InputVariables.
 */
void NSDFWriter::openEventData(const Eref &eref)
{
    if (filehandle_ <= 0){
        return;
    }
    for (unsigned int ii = 0; ii < eventInputs_.size(); ++ii){
        stringstream path;
        path << eref.objId().path() << "/" << "eventInput[" << ii << "]";
        ObjId inputObj = ObjId(path.str());
        Element * el = inputObj.element();
        const DestFinfo * dest = static_cast<const DestFinfo*>(el->cinfo()->findFinfo("input"));
        vector < ObjId > src;
        vector < string > srcFields;
        el->getMsgSourceAndSender(dest->getFid(), src, srcFields);
        if (src.size() > 1){
            cerr << "NSDFWriter::openEventData - only one source can be connected to an eventInput" <<endl;
        } else if (src.size() == 1){
            eventSrcFields_.push_back(srcFields[0]);
            eventSrc_.push_back(src[0].path());
            events_.resize(eventSrc_.size());
            stringstream path;
            path << src[0].path() << "." << srcFields[0];
            hid_t dataSet = getEventDataset(src[0].path(), srcFields[0]);
            eventDatasets_.push_back(dataSet);            
        } else {
            cerr <<"NSDFWriter::openEventData - cannot handle multiple connections at single input." <<endl;
        }
    }
}

void NSDFWriter::createEventMap()
{
    herr_t status;    
    hid_t eventMapContainer = require_group(filehandle_, MAPEVENTSRC);
    // Open the container for the event maps
    // Create the Datasets themselves (one for each field - each row
    // for one object).
    for (map< string, vector < string > >::iterator ii = classFieldToEventSrc_.begin();
         ii != classFieldToEventSrc_.end();
         ++ii){
        vector < string > pathTokens;
        moose::tokenize(ii->first, "/", pathTokens);
        string className = pathTokens[0];
        string fieldName = pathTokens[1];
        hid_t classGroup = require_group(eventMapContainer, className);
        hid_t strtype = H5Tcopy(H5T_C_S1);
        status = H5Tset_size(strtype, H5T_VARIABLE);
        // create file space
        hid_t ftype = H5Tcreate(H5T_COMPOUND, sizeof(hvl_t) +sizeof(hobj_ref_t));
        status = H5Tinsert(ftype, "source", 0, strtype);
        status = H5Tinsert(ftype, "data", sizeof(hvl_t), H5T_STD_REF_OBJ);
        hsize_t dims[1] = {ii->second.size()};
        hid_t space = H5Screate_simple(1, dims, NULL);
        // The dataset for mapping is named after the field
        hid_t ds = H5Dcreate2(classGroup, fieldName.c_str(), ftype, space,
                              H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        status = H5Sclose(space);
        map_type * buf = (map_type*)calloc(ii->second.size(), sizeof(map_type));
        // Populate the buffer entries with source uid and data
        // reference
        for (unsigned int jj = 0; jj < ii->second.size(); ++jj){
            buf->source = ii->second[jj].c_str();
            char * dsname = (char*)calloc(256, sizeof(char));
            ssize_t size = H5Iget_name(classFieldToEvent_[ii->first][jj], dsname, 255);
            if (size > 255){
                free(dsname);
                dsname = (char*)calloc(size, sizeof(char));
                size = H5Iget_name(classFieldToEvent_[ii->first][jj], dsname, 255);
            }
            status = H5Rcreate(&(buf->data), filehandle_, dsname, H5R_OBJECT, -1);
            free(dsname);
            assert(status >= 0);            
        }
        // create memory space
        hid_t memtype = H5Tcreate(H5T_COMPOUND, sizeof(map_type));
        status = H5Tinsert(memtype, "source",
                           HOFFSET(map_type, source), strtype);
        status = H5Tinsert(memtype, "data",
                           HOFFSET(map_type, data), H5T_STD_REF_OBJ);
        status = H5Dwrite(ds, memtype,  H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
        free(buf);
        status = H5Tclose(strtype);
        status = H5Tclose(ftype);
        status = H5Tclose(memtype);
        status = H5Dclose(ds);
    }
}

/**
   Create or retrieve a dataset for an event input.  The dataset path
   will be /data/event/{class}/{srcFinfo}/{id}_{dataIndex}_{fieldIndex}.

   path : {source_object_id}.{source_field_name}

   TODO: check the returned hid_t and show appropriate error messages.
*/
hid_t NSDFWriter::getEventDataset(string srcPath, string srcField)
{
    string eventSrcPath = srcPath + string("/") + srcField;
    map< string, hid_t >::iterator it = eventSrcDataset_.find(eventSrcPath);
    if (it != eventSrcDataset_.end()){
        return it->second;
    }
    ObjId source(srcPath);
    herr_t status;
    htri_t exists = -1;
    string className = Field<string>::get(source, "className");
    string path = EVENTPATH + string("/") + className + string("/") + srcField;
    hid_t container = require_group(filehandle_, path);
    stringstream dsetname;
    dsetname << source.id.value() <<"_" << source.dataIndex << "_" << source.fieldIndex;
    hid_t dataset = createDoubleDataset(container, dsetname.str().c_str());
    classFieldToEvent_[className + "/" + srcField].push_back(dataset);
    classFieldToEventSrc_[className + "/" + srcField].push_back(srcPath);
    status = writeScalarAttr<string>(dataset, "source", srcPath);
    assert(status >= 0);
    status = writeScalarAttr<string>(dataset, "field", srcField);
    assert(status >= 0);
    eventSrcDataset_[eventSrcPath] = dataset;
    return dataset;
}

void NSDFWriter::flush()
{
    // We need to update the tend on each write since we do not know
    // when the simulation is getting over and when it is just paused.
    writeScalarAttr<string>(filehandle_, "tend", iso_time(NULL));    
    
    // append all uniform data
    for (map< string, hid_t>::iterator it = classFieldToUniform_.begin();
         it != classFieldToUniform_.end(); ++it){
        map< string, vector < unsigned int > >::iterator idxit = classFieldToSrcIndex_.find(it->first);
        if (idxit == classFieldToSrcIndex_.end()){
            cerr << "Error: NSDFWriter::flush - could not find entry for " << it->first <<endl;
            break;
        }
        if (data_.size() == 0 || data_[0].size() == 0){
            break;
        }        
        double * buffer = (double*)calloc(idxit->second.size() * steps_, sizeof(double));
        vector< double > values;
        for (unsigned int ii = 0; ii < idxit->second.size(); ++ii){
            for (unsigned int jj = 0; jj < steps_; ++jj){
                buffer[ii * steps_ + jj] = data_[idxit->second[ii]][jj];
            }
            data_[idxit->second[ii]].clear();
        }
        
        hid_t filespace = H5Dget_space(it->second);
        if (filespace < 0){
            break;
        }
        hsize_t dims[2];
        hsize_t maxdims[2];
        // retrieve current datset dimensions
        herr_t status = H5Sget_simple_extent_dims(filespace, dims, maxdims);        
        hsize_t newdims[] = {dims[0], dims[1] + steps_}; // new column count
        status = H5Dset_extent(it->second, newdims); // extend dataset to new column count
        H5Sclose(filespace);
        filespace = H5Dget_space(it->second); // get the updated filespace 
        hsize_t start[2] = {0, dims[1]};
        dims[1] = steps_; // change dims for memspace & hyperslab
        hid_t memspace = H5Screate_simple(2, dims, NULL);
        H5Sselect_hyperslab(filespace, H5S_SELECT_SET, start, NULL, dims, NULL);
        status = H5Dwrite(it->second, H5T_NATIVE_DOUBLE,  memspace, filespace, H5P_DEFAULT, buffer);
        H5Sclose(memspace);
        H5Sclose(filespace);
        free(buffer);
    }
    
    // append all event data
    for (unsigned int ii = 0; ii < eventSrc_.size(); ++ii){
        appendToDataset(getEventDataset(eventSrc_[ii], eventSrcFields_[ii]),
                        events_[ii]);
        events_[ii].clear();
    }
    // flush HDF5 nodes.
    HDF5DataWriter::flush();
}

void NSDFWriter::reinit(const Eref& eref, const ProcPtr proc)
{
    // write environment
    // write model
    // write map
    if (filehandle_ >0){
        close();
    }
    // TODO: what to do when reinit is called? Close the existing file
    // and open a new one in append mode? Or keep adding to the
    // current file?
    if (filename_.empty()){
        filename_ = "moose_data.nsdf.h5";
    }
    openFile();
    writeScalarAttr<string>(filehandle_, "created", iso_time(0));
    writeScalarAttr<string>(filehandle_, "tstart", iso_time(0));
    writeScalarAttr<string>(filehandle_, "nsdf_version", "1.0");
    openUniformData(eref);
    for (map < string, hid_t >::iterator it = classFieldToUniform_.begin();
         it != classFieldToUniform_.end();
         ++it){
        // tstart is reset to 0 on reinit
        writeScalarAttr< double >(it->second, "tstart", 0.0);
        // dt is same for all requested data - that of the NSDFWriter
        writeScalarAttr< double >(it->second, "dt", proc->dt);
    }
    openEventData(eref);
    writeModelTree();
    createUniformMap();
    createEventMap();
    steps_ = 0;
}

void NSDFWriter::process(const Eref& eref, ProcPtr proc)
{
    if (filehandle_ < 0){
        return;
    }
    vector < double > uniformData;
    const Finfo* tmp = eref.element()->cinfo()->findFinfo("requestOut");
    const SrcFinfo1< vector < double > *>* requestOut = static_cast<const SrcFinfo1< vector < double > * > * >(tmp);
    requestOut->send(eref, &uniformData);
    for (unsigned int ii = 0; ii < uniformData.size(); ++ii){
        data_[ii].push_back(uniformData[ii]);
    }
    ++steps_;
    if (steps_ < flushLimit_){
        return;
    }
    // // TODO this is place holder. Will convert to 2D datasets to
    // // collect same field from object on same clock
    // for (unsigned int ii = 0; ii < datasets_.size(); ++ii){
    //     herr_t status = appendToDataset(datasets_[ii], data_[ii]);
    //     data_[ii].clear();
    // }
    // for (unsigned int ii = 0; ii < events_.size(); ++ii){
    //     herr_t status = appendToDataset(eventDatasets_[ii], events_[ii]);
    // }
    NSDFWriter::flush();
    steps_ = 0;
 }

NSDFWriter& NSDFWriter::operator=( const NSDFWriter& other)
{
	eventInputs_ = other.eventInputs_;
	for ( vector< InputVariable >::iterator 
					i = eventInputs_.begin(); i != eventInputs_.end(); ++i )
			i->setOwner( this );
        for (unsigned int ii = 0; ii < getNumEventInputs(); ++ii){
            events_[ii].clear();
        }
	return *this;
}

void NSDFWriter::setNumEventInputs(unsigned int num)
{
    unsigned int prevSize = eventInputs_.size();
    eventInputs_.resize(num);
    for (unsigned int ii = prevSize; ii < num; ++ii){
        eventInputs_[ii].setOwner(this);
    }
}

unsigned int NSDFWriter::getNumEventInputs() const
{
    return eventInputs_.size();
}

void NSDFWriter::setEnvironment(string key, string value)
{
    env_[key] = value;
}


void NSDFWriter::setInput(unsigned int index, double value)
{
    events_[index].push_back(value);
}

InputVariable* NSDFWriter::getEventInput(unsigned int index)
{
    static InputVariable dummy;
    if (index < eventInputs_.size()){
        return &eventInputs_[index];
    }
    cout << "Warning: NSDFWriter::getEventInput: index: " << index <<
		" is out of range: " << eventInputs_.size() << endl;
   return &dummy;
}

void NSDFWriter::setModelRoot(string value)
{
    modelRoot_ = value;
}

string NSDFWriter::getModelRoot() const
{
    return modelRoot_;
}
        

void NSDFWriter::writeModelTree()
{
    vector< string > tokens;
    ObjId mRoot(modelRoot_);
    string rootPath = MODELTREEPATH + string("/") + mRoot.element()->getName();
    hid_t rootGroup = require_group(filehandle_, rootPath);
    hid_t tmp;
    htri_t exists;
    herr_t status;
    deque<Id> nodeQueue;
    deque<hid_t> h5nodeQueue;
    nodeQueue.push_back(mRoot);
    h5nodeQueue.push_back(rootGroup);
    // TODO: need to clarify what happens with array elements. We can
    // have one node per vec and set a count field for the number of
    // elements
    while (nodeQueue.size() > 0){
        ObjId node = nodeQueue.front();
        nodeQueue.pop_front();
        hid_t prev = h5nodeQueue.front();;
        h5nodeQueue.pop_front();
        vector < Id > children;
        Neutral::children(node.eref(), children);
        for ( unsigned int ii = 0; ii < children.size(); ++ii){
            string name = children[ii].element()->getName();
            // skip the system elements
            if (children[ii].path() == "/Msgs"
                || children[ii].path() == "/clock"
                || children[ii].path() == "/classes"
                || children[ii].path() == "/postmaster"){
                continue;
            }
            exists = H5Lexists(prev, name.c_str(), H5P_DEFAULT);
            if (exists > 0){
                tmp = H5Gopen2(prev, name.c_str(), H5P_DEFAULT);
            } else {
                tmp = H5Gcreate2(prev, name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            }
            writeScalarAttr< string >(tmp, "uid", children[ii].path());
            nodeQueue.push_back(children[ii]);
            h5nodeQueue.push_back(tmp);
        }
        status = H5Gclose(prev);
    }
}
#endif // USE_HDF5

// 
// NSDFWriter.cpp ends here
