/* NSDFWriter.h ---
 *
 * Filename: NSDFWriter.h
 * Description:
 * Author: subha
 * Maintainer:
 * Created: Thu Jun 18 23:06:59 2015 (-0400)
 * Version:
 * Last-Updated: Sun Dec 20 23:17:32 2015 (-0500)
 *           By: subha
 *     Update #: 2
 * URL:
 * Keywords:
 * Compatibility:
 *
 */

/* Commentary:
 *
 *
 *
 */

/* Change log:
 *
 *
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

#ifdef USE_HDF5
#ifndef _NSDFWRITER_H
#define _NSDFWRITER_H

#include "HDF5DataWriter.h"

class InputVariable;

/**
   compound data type for storing source->data mapping for event data
*/
typedef struct {
    const char * source;
    hobj_ref_t data;
} map_type;

/**
   NSDFWriter dumps data in NSDF file format.

   - As of June 2015, MOOSE uses fixed time steps for updating field
   values. So we support only Uniform data.

   - This class has one SrcFinfo to request data. Multiple get{Field}
     DestFinfos can be connected to this and the NSDFWriter class will put
     together the ones with same name in one 2D dataset.

   - One DestFinfo where SrcFinfos sending out event times can be
     connected. These will go under Event data.

 */
class NSDFWriter: public HDF5DataWriter
{
  public:
    NSDFWriter();
    ~NSDFWriter();
    virtual void flush();
    // set the environment specs like title, author, tstart etc.
    void setEnvironment(string key, string value);
    // the model tree rooted here is to be copied to NSDF file
    void setModelRoot(string root);
    string getModelRoot() const;
    InputVariable *getEventInput(unsigned int index);
    void setNumEventInputs(unsigned int num);
    unsigned int getNumEventInputs() const;
    void setInput(unsigned int index, double value);
    void openUniformData(const Eref &eref);
    void closeUniformData();
    void openEventData(const Eref &eref);
    void closeEventData();
    virtual void close();
    void createUniformMap();
    void createEventMap();
    void writeModelTree();
    // Sort the incoming data lines according to source object/field.
    void process(const Eref &e, ProcPtr p);
    void reinit(const Eref &e, ProcPtr p);
    NSDFWriter& operator=(const NSDFWriter&other);

    static const Cinfo *initCinfo();

  protected:
    hid_t getEventDataset(string srcPath, string srcField);
    void sortOutUniformSources(const Eref& eref);
    /* hid_t getUniformDataset(string srcPath, string srcField); */
    map <string, string> env_; // environment attributes
    vector < hid_t > eventDatasets_;
    // event times data_ and datasets_ inherited from HDF5DataWriter
    // are still attached to requestOut message
    vector < vector < double > > events_;
    vector < InputVariable > eventInputs_;
    vector < string > eventSrcFields_;
    vector < string > eventSrc_;
    map < string, hid_t > eventSrcDataset_;
    hid_t eventGroup_; // handle for event container
    hid_t uniformGroup_; // handle for uniform container
    hid_t dataGroup_; // handle for data container.
    hid_t modelGroup_; // handle for model container
    hid_t mapGroup_; // handle for map container
    map< string, vector< hid_t > > classFieldToEvent_;
    map< string, vector< string > > classFieldToEventSrc_;
    map< string, hid_t > classFieldToUniform_;
    // maps a path.srcFinfo to the <target dataset, row index > for
    // storing uniform data in dataset.
    map< string, pair< hid_t, unsigned int > > uniformSrcDatasetIndex_;
    /**
       Storing uniform data:

       The data coming in return for requestOut gets stored in
       dataBuffer in the same sequence as the connections,
       irrespective of class and field.

       For each source object find the class and field and create map
       < class.field, vector<unsigned int> > that maps the class.field
       to a vector of indices in dataBuffer. the index vector contents
       should be in the same order as the sequence of objects

     */
    // The last number of rows in each dataset
    // (/data/uniform/className/fieldName -> size)

    map< string, vector< unsigned int > > classFieldToSrcIndex_;
    // the index of the recorded field in its dataset (objectPath/fieldName -> index)
    map< string, unsigned int > objectFieldToIndex_;
    // objectPath -> field - just for reuse later - avoid repeating
    // extraction of field name from func_.
    vector < pair< string, string > > objectField_;
    map< string, vector < string > > classFieldToObjectField_;
    vector < string > vars_;
    string modelRoot_;

};
#endif // _NSDFWRITER_H
#endif // USE_HDF5


/* NSDFWriter.h ends here */
