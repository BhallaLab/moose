// melement.cpp --- 
// 
// Filename: melement.cpp
// Description: 
// Author: 
// Maintainer: 
// Created: Mon Jul 22 16:50:41 2013 (+0530)
// Version: 
// Last-Updated: Fri Sep 25 23:02:53 2015 (-0400)
//           By: subha
//     Update #: 76
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// Mon Jul 22 16:50:47 IST 2013 - Taking out ObjId stuff from
// moosemodule.cpp
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

#include <Python.h>
#include <structmember.h>

#ifdef USE_BOOST
#include <boost/format.hpp>
#endif

#include <iostream>
#include <typeinfo>
#include <cstring>
#include <map>
#include <ctime>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "../basecode/header.h"
#include "../basecode/Id.h"
#include "../basecode/ObjId.h"
#include "../utility/utility.h"
#include "../shell/Shell.h"

#include "moosemodule.h"

using namespace std;


extern PyTypeObject ObjIdType;
extern PyTypeObject IdType;
extern int PyType_IsSubtype(PyTypeObject *, PyTypeObject *);


PyObject * get_ObjId_attr(_ObjId * oid, string attribute)
{
    if (attribute == "vec"){
        return moose_ObjId_getId(oid);
    } else if (attribute == "dindex"){
        return moose_ObjId_getDataIndex(oid);
    } else if (attribute == "findex"){
        return moose_ObjId_getFieldIndex(oid);
    }
    return NULL;
}

int moose_ObjId_init_from_id(_ObjId * self, PyObject * args, PyObject * kwargs)
{
    extern PyTypeObject ObjIdType;
    static const char* const kwlist[] = {"id", "dataIndex", "fieldIndex", NULL};
    unsigned int id = 0, data = 0, field = 0;
    PyObject * obj = NULL;
    if (PyArg_ParseTupleAndKeywords(args, kwargs,
                                    "I|II:moose_ObjId_init_from_id",
                                    (char**)kwlist,
                                    &id, &data, &field)){
        PyErr_Clear();
        if (!Id::isValid(id)){
            RAISE_INVALID_ID(-1, "moose_ObjId_init_from_id");
        }
        self->oid_ = ObjId(Id(id), data, field );
        if (self->oid_.bad()){
            PyErr_SetString(PyExc_ValueError, "Invalid ObjId");
            return -1;
        }
        return 0;
    }
    PyErr_Clear();
    if (PyArg_ParseTupleAndKeywords(args, kwargs,
                                    "O|II:moose_ObjId_init_from_id",
                                    (char**)kwlist,
                                    &obj, &data, &field)){
        PyErr_Clear();
        // If first argument is an Id object, construct an ObjId out of it
        if (Id_Check(obj)){
            if (!Id::isValid(((_Id*)obj)->id_)){
                RAISE_INVALID_ID(-1, "moose_ObjId_init_from_id");
            }                    
            self->oid_ = ObjId(((_Id*)obj)->id_, data, field );
            if (self->oid_.bad()){
                PyErr_SetString(PyExc_ValueError, "Invalid dataIndex/fieldIndex.");
                return -1;
            }
            return 0;
        } else if (PyObject_IsInstance(obj, (PyObject*)&ObjIdType)){
            if (!Id::isValid(((_ObjId*)obj)->oid_.id)){
                RAISE_INVALID_ID(-1, "moose_ObjId_init_from_id");
            }                    
            self->oid_ = ((_ObjId*)obj)->oid_;
            if (self->oid_.bad()){
                PyErr_SetString(PyExc_ValueError, "Invalid ObjId");
                return -1;
            }
            return 0;
        }
    }
    return -1;
}

int moose_ObjId_init_from_path(_ObjId * self, PyObject * args,
                               PyObject * kwargs)
{
    static const char* const kwlist[] = {"path", "n", "g", "dtype", NULL};
    const char* parsedPath;
    unsigned int numData = 1;
    unsigned int isGlobal = 0;
    char* type = NULL;

    self->oid_ = ObjId( 0, BADINDEX );
    PyTypeObject * mytype = Py_TYPE(self);
    string mytypename(mytype->tp_name);
    
    // First try to parse the arguments as (parsedPath, n, g, dtype)
    bool parse_success = false;
    if (PyArg_ParseTupleAndKeywords(args, kwargs,
                                    "s|IIs:moose_ObjId_init_from_path",
                                    (char**)kwlist,
                                    &parsedPath, &numData, &isGlobal, &type)){
        parse_success = true;
    }
    // we need to clear the parse error so that the callee can try
    // other alternative: moose_ObjId_init_from_id
    PyErr_Clear(); 
    if (!parse_success){
        return -2;
    }

    string path(parsedPath);
    // Remove one or more instances of '/' by a single '/' e.g. //a -> /a,
    // /a//b -> /a/b etc. 
    path = moose::fix(path);

    ostringstream err;
    // First see if there is an existing object with at path
    self->oid_ = ObjId(path);
    PyTypeObject * basetype = getBaseClass((PyObject*)self);
    string basetype_str;
    if (type == NULL){
        if (basetype == NULL){
            PyErr_SetString(PyExc_TypeError, "Unknown class. Need a valid MOOSE class or subclass thereof.");
            return -1;
        }
        basetype_str = string(basetype->tp_name).substr(6); // remove `moose.` prefix from type name
    } else {
        basetype_str = string(type);            
    }


    // If oid_ is bad, it can be either a nonexistent path or the root.
    if (self->oid_.bad()){
        // is this the root element?
        if ((path == "/") || (path == "/root")){
            if ((basetype == NULL) || PyType_IsSubtype(mytype, basetype)){
                return 0;
            }
            err << "cannot convert " << Field<string>::get(self->oid_, "className")
                << " to " << mytypename
                << "To get the existing object use `moose.element(obj)` instead.";
            PyErr_SetString(PyExc_TypeError, err.str().c_str());
            return -1;
        }
        // no - so we need to create a new element
    } else { // this is a non-root existing element
        // If the current class is a subclass of some predefined
        // moose class, do nothing.
        if ((basetype != NULL) && PyType_IsSubtype(mytype, basetype)){
            return 0;
        }
        // element exists at this path, but it does not inherit from any moose class.
        // throw an error
        err << "cannot convert "
            << Field<string>::get(self->oid_, "className")
            << " to "
            << basetype_str
            << ". To get the existing object use `moose.element(obj)` instead.";
        PyErr_SetString(PyExc_TypeError, err.str().c_str());
        return -1;
    }
    // try to create new vec
    Id new_id = create_Id_from_path(path, numData, isGlobal, basetype_str);
    // vec failed. throw error
    if (new_id == Id() && PyErr_Occurred()){
        // Py_XDECREF(self);
        return -1;
    }

    self->oid_ = ObjId(new_id);
    return 0;
}
    
int moose_ObjId_init(_ObjId * self, PyObject * args,
                     PyObject * kwargs)
{
    if (self && !PyObject_IsInstance((PyObject*)self, (PyObject*)Py_TYPE((PyObject*)self))){
        ostringstream error;
        error << "Expected an melement or subclass. Found "
              << Py_TYPE(self)->tp_name;
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return -1;
    }
    int ret = moose_ObjId_init_from_path(self, args, kwargs);
    if (ret >= -1){
        return ret;
    }
    // parsing arguments as (path, dims, classname) failed. See if it is existing Id or ObjId.
    if (moose_ObjId_init_from_id(self, args, kwargs) == 0){
        return 0;
    }
    PyErr_SetString(PyExc_ValueError,
                    "Could not parse arguments. "
                    " Call __init__(path, n, g, dtype) or"
                    " __init__(id, dataIndex, fieldIndex)");        
    return -1;
}

/**
   This function combines Id, DataId and fieldIndex to construct
   the hash of this object. Here we assume 16 most significant
   bits for Id, next 32 bits for dataIndex and the least significant
   16 bits for fieldIndex. If these criteria are not met, the hash
   function will cause collissions. Note that the bitshift
   opeartions are byte order independent - so they should give the
   same result on both little- and big-endian systems.
*/
long moose_ObjId_hash(_ObjId * self)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(-1, "moose_ObjId_hash");
    }
    long long id = (long long)(self->oid_.id.value());
    long dataIndex = self->oid_.dataIndex;
    long fieldIndex = self->oid_.fieldIndex;
    /* attempt to make it with 32 bit system - assuming id will
     * have its value within least significant 16 bits and
     * dataIndex and fieldIndex will be limited to first 8 bits */
    if (sizeof(size_t) == 8){
        return id << 48 | dataIndex << 16 | fieldIndex;
    } else {
        return id << 16 | dataIndex << 8 | fieldIndex;
    }
}

PyObject * moose_ObjId_repr(_ObjId * self)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_repr");
    }
    ostringstream repr;
    repr << "<moose." << Field<string>::get(self->oid_, "className") << ": "
         << "id=" << self->oid_.id.value() << ", "
         << "dataIndex=" << self->oid_.dataIndex << ", "
         << "path=" << self->oid_.path() << ">";
    return PyString_FromString(repr.str().c_str());
} // !  moose_ObjId_repr

PyObject * moose_ObjId_str(_ObjId * self)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_str");
    }
    ostringstream repr;
    repr << "<moose." << Field<string>::get(self->oid_, "className") << ": "
         << "id=" << self->oid_.id.value() << ", "
         << "dataIndex=" << self->oid_.dataIndex << ", "
         << "path=" << self->oid_.path() << ">";
    return PyString_FromString(repr.str().c_str());
} // !  moose_ObjId_str

PyDoc_STRVAR(moose_ObjId_getId_documentation,
             "getId() -> vec\n"
             "\n"
             "Get the vec of this object\n"
             "\n");
PyObject* moose_ObjId_getId(_ObjId * self)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getId");
    }
    extern PyTypeObject IdType;        
    _Id * ret = PyObject_New(_Id, &IdType);
    ret->id_ = self->oid_.id;
    return (PyObject*)ret;
}

PyDoc_STRVAR(moose_ObjId_getFieldType_documentation,
             "getFieldType(fieldname)\n"
             "\n"
             "Get the string representation of the type of the field `fieldname`.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldname : string\n"
             "    Name of the field to be queried.\n"
             "\n");

PyObject * moose_ObjId_getFieldType(_ObjId * self, PyObject * args)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getFieldType");
    }
    char * fieldName = NULL;
    if (!PyArg_ParseTuple(args, "s:moose_ObjId_getFieldType", &fieldName)){
        return NULL;
    }
    string typeStr = getFieldType(Field<string>::get(self->oid_, "className"),
                                  string(fieldName));
    if (typeStr.length() <= 0){
        PyErr_SetString(PyExc_ValueError,
                        "Empty string for field type. "
                        "Field name may be incorrect.");
        return NULL;
    }
    PyObject * type = PyString_FromString(typeStr.c_str());
    return type;
}  // ! moose_Id_getFieldType

/**
   Wrapper over getattro to allow direct access as a function with variable argument list
*/

PyDoc_STRVAR(moose_ObjId_getField_documentation,
             "getField(fieldname)\n"
             "\n"
             "Get the value of the field `fieldname`.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldname : string\n"
             "    Name of the field.");
PyObject * moose_ObjId_getField(_ObjId * self, PyObject * args)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getField");
    }
    PyObject * attr;        
    if (!PyArg_ParseTuple(args, "O:moose_ObjId_getField", &attr)){
        return NULL;
    }
    return moose_ObjId_getattro(self, attr);
}

/**
   2011-03-28 13:59:41 (+0530)
   
   Get a specified field. Re-done on: 2011-03-23 14:42:03 (+0530)

   I wonder how to cleanly do this. The Id - ObjId dichotomy is
   really ugly. When you don't pass an index, it is just treated
   as 0. Then what is the point of having Id separately? ObjId
   would been just fine!
*/
PyObject * moose_ObjId_getattro(_ObjId * self, PyObject * attr)
{
    int new_attr = 0;
    if (self->oid_.bad()){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getattro");
    }
    // extern PyTypeObject IdType;
    // extern PyTypeObject ObjIdType;
    const char * field;
    char ftype;
    if (PyString_Check(attr)){
        field = PyString_AsString(attr);
    } else {
        return PyObject_GenericGetAttr((PyObject*)self, attr);
    }
    PyObject * _ret = get_ObjId_attr(self, field);
    if (_ret != NULL){
        return _ret;
    }
    string fieldName(field);
    string className = Field<string>::get(self->oid_, "className");
    vector<string> valueFinfos = getFieldNames(className, "valueFinfo");
    bool isValueField = false;
    for (unsigned int ii = 0; ii < valueFinfos.size(); ++ii){
        if (fieldName == valueFinfos[ii]){
            isValueField = true;
            break;
        }
    }
            
    string type = getFieldType(className, fieldName);
    if (type.empty() || !isValueField ){
        // Check if this field name is aliased and update fieldName and type if so.
        map<string, string>::const_iterator it = get_field_alias().find(fieldName);
        if (it != get_field_alias().end()){
            fieldName = it->second;
            field = fieldName.c_str();
            isValueField = false;
            for (unsigned int ii = 0; ii < valueFinfos.size(); ++ii){
                if (fieldName == valueFinfos[ii]){
                    isValueField = true;
                    break;
                }
            }
            type = getFieldType(Field<string>::get(self->oid_, "className"), fieldName);
            // Update attr for next level (PyObject_GenericGetAttr) in case.
            // Py_XDECREF(attr);
            attr = PyString_FromString(field);
            new_attr = 1;
        }
    }
    if (type.empty() || !isValueField){
        _ret = PyObject_GenericGetAttr((PyObject*)self, attr);
        if (new_attr){
            Py_DECREF(attr);
        }
        return _ret;
    }
    ftype = shortType(type);
    if (!ftype){
        _ret = PyObject_GenericGetAttr((PyObject*)self, attr);
        if (new_attr){
            Py_DECREF(attr);
        }
        return _ret;
    }
    fieldName= string(field);
    switch(ftype){
        case 's': {
            string _s = Field<string>::get(self->oid_, fieldName);
            _ret = Py_BuildValue("s", _s.c_str());
            break;
        }
        case 'd': {
            double value = Field< double >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'i': {
            int value = Field<int>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'I': {
            unsigned int value = Field<unsigned int>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'l': {
            long value = Field<long>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'L': {
            long long value = Field<long long>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'k': {
            unsigned long value = Field<unsigned long>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'K': {
            unsigned long long value = Field<unsigned long long>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'f': {
            float value = Field<float>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'x': {                    
            Id value = Field<Id>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'y': {                    
            ObjId value = Field<ObjId>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'z': {
            PyErr_SetString(PyExc_NotImplementedError, "DataId handling not implemented yet.");
            _ret = NULL;
            break;
        }
        case 'D': {
            vector< double > value = Field< vector < double > >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'X': { // vector<Id>
            vector < Id > value = Field<vector <Id> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        } 
        case 'Y': { // vector<ObjId>
            vector < ObjId > value = Field<vector <ObjId> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        } 
        case 'M': {
            vector< long > value = Field< vector <long> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'P': {
            vector < unsigned long > value = Field< vector < unsigned long > >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'S': {
            vector < string > value = Field<vector <string> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'v': {
            vector < int > value = Field<vector <int> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'N': {
            vector <unsigned int > value = Field< vector < unsigned int> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'T': { // vector<vector < unsigned int >>
            vector < vector < unsigned int > > value = Field<vector <vector < unsigned int > > >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        } 
        case 'Q': { // vector< vector < int > >
            vector <  vector < int >  > value = Field<vector < vector < int > > >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        } 
        case 'R': { // vector< vector < double > >
            vector <  vector < double >  > value = Field<vector < vector < double > > >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'F': {
            vector <float> value = Field< vector < float > >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'c': {
            char value = Field<char>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'h': {
            short value = Field<short>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'H': {
            unsigned short value = Field<unsigned short>::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'w': {
            vector < short > value = Field<vector <short> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }
        case 'C': {
            vector < char > value = Field<vector <char> >::get(self->oid_, fieldName);
            _ret = to_py(&value, ftype);
            break;
        }

        case 'b': {
            bool value = Field<bool>::get(self->oid_, fieldName);
            if (value){
                _ret = Py_True;
                Py_INCREF(Py_True);
            } else {
                _ret = Py_False;
                Py_INCREF(Py_False);
            }
            break;
        }

        default:
            _ret = PyObject_GenericGetAttr((PyObject*)self, attr);
            
    }
    if (new_attr){
        Py_DECREF(attr);
    }
    return _ret;        
}

/**
   Wrapper over setattro to make METHOD_VARARG
*/
PyDoc_STRVAR(moose_ObjId_setField_documentation,
             "setField(fieldname, value)\n"
             "\n"
             "Set the value of specified field.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldname : string\n"
             "    Field to be assigned value to.\n"
             "\n"
             "value : python datatype compatible with the type of the field\n"
             "    The value to be assigned to the field.");

PyObject * moose_ObjId_setField(_ObjId * self, PyObject * args)
{
    PyObject * field;
    PyObject * value;
    if (!PyArg_ParseTuple(args, "OO:moose_ObjId_setField", &field, &value)){
        return NULL;
    }
    if (moose_ObjId_setattro(self, field, value) == -1){
        return NULL;
    }
    Py_RETURN_NONE;
}

/**
   Set a specified field. Redone on 2011-03-23 14:41:45 (+0530)
*/
int  moose_ObjId_setattro(_ObjId * self, PyObject * attr, PyObject * value)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(-1, "moose_ObjId_setattro");
    }
    const char * field;
    if (PyString_Check(attr)){
        field = PyString_AsString(attr);
    } else {
        PyErr_SetString(PyExc_TypeError, "Attribute name must be a string");
        return -1;
    }
    string fieldtype = getFieldType(Field<string>::get(self->oid_, "className"), string(field));
    if (fieldtype.length() == 0){
        // If it is instance of a MOOSE built-in class then throw
        // error (to avoid silently creating new attributes due to
        // typos). Otherwise, it must have been subclassed in
        // Python. Then we allow normal Pythonic behaviour and
        // consider such mistakes user's responsibility.
        string className = ((PyTypeObject*)PyObject_Type((PyObject*)self))->tp_name;            
        if (get_moose_classes().find(className) == get_moose_classes().end()){
            return PyObject_GenericSetAttr((PyObject*)self, PyString_FromString(field), value);
        }
        ostringstream msg;
        msg << "'" << className << "' class has no field '" << field << "'" << endl;
        PyErr_SetString(PyExc_AttributeError, msg.str().c_str());
        return -1;
    }
    char ftype = shortType(fieldtype);
    int ret = 0;
    switch(ftype){
        case 'd': {
            double _value = PyFloat_AsDouble(value);
            ret = Field<double>::set(self->oid_, string(field), _value);
            break;
        }
        case 'l': {
            long _value = PyInt_AsLong(value);
            if ((_value != -1) || (!PyErr_Occurred())){
                ret = Field<long>::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'I': {
            unsigned long _value = PyInt_AsUnsignedLongMask(value);
            ret = Field<unsigned int>::set(self->oid_, string(field), (unsigned int)_value);
            break;
        }
        case 'k': {
            unsigned long _value = PyInt_AsUnsignedLongMask(value);
            ret = Field<unsigned long>::set(self->oid_, string(field), _value);
            break;
        }                
        case 'f': {
            float _value = PyFloat_AsDouble(value);
            ret = Field<float>::set(self->oid_, string(field), _value);
            break;
        }
        case 's': {
            char * _value = PyString_AsString(value);
            if (_value){
                ret = Field<string>::set(self->oid_, string(field), string(_value));
            }
            break;
        }
        case 'x': {// Id
            if (value){
                ret = Field<Id>::set(self->oid_, string(field), ((_Id*)value)->id_);
            } else {
                PyErr_SetString(PyExc_ValueError, "Null pointer passed as vec Id value.");
                return -1;
            }
            break;
        }
        case 'y': {// ObjId
            if (value){
                ret = Field<ObjId>::set(self->oid_, string(field), ((_ObjId*)value)->oid_);
            } else {
                PyErr_SetString(PyExc_ValueError, "Null pointer passed as vec Id value.");
                return -1;
            }
            break;
        }
        case 'D': {//SET_VECFIELD(double, d)
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<double> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<double> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    double v = PyFloat_AsDouble(vo);
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < double > >::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'b': {
            bool _value = (Py_True == value) || (PyInt_AsLong(value) != 0);
            ret = Field<bool>::set(self->oid_, string(field), _value);
            break;
        }
        case 'c': {
            char * _value = PyString_AsString(value);
            if (_value && _value[0]){
                ret = Field<char>::set(self->oid_, string(field), _value[0]);
            }
            break;
        }
        case 'i': {
            int _value = PyInt_AsLong(value);
            if ((_value != -1) || (!PyErr_Occurred())){
                ret = Field<int>::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'h':{
            short _value = (short)PyInt_AsLong(value);
            if ((_value != -1) || (!PyErr_Occurred())){
                ret = Field<short>::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'z': {// DataId
            PyErr_SetString(PyExc_NotImplementedError, "DataId handling not implemented yet.");
            return -1;
        }
        case 'v': {
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
            }
            Py_ssize_t length = PySequence_Length(value);
            vector<int> _value;
            for ( int ii = 0; ii < length; ++ii){
                PyObject * vo = PySequence_GetItem(value, ii);
                int v = PyInt_AsLong(vo);
                Py_XDECREF(vo);
                _value.push_back(v);
            }
            ret = Field< vector < int > >::set(self->oid_, string(field), _value);
            break;
        }
        case 'w': {
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<short> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<short> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    short v = PyInt_AsLong(vo);
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < short > >::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'L': {//SET_VECFIELD(long, l)
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError,
                                "For setting vector<long> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<long> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    long v = PyInt_AsLong(vo);
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < long > >::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'N': {//SET_VECFIELD(unsigned int, I)
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<unsigned int> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<unsigned int> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    unsigned int v = PyInt_AsUnsignedLongMask(vo);
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < unsigned int > >::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'K': {//SET_VECFIELD(unsigned long, k)
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<unsigned long> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<unsigned long> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    unsigned long v = PyInt_AsUnsignedLongMask(vo);
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < unsigned long > >::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'F': {//SET_VECFIELD(float, f)
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<float> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<float> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    float v = PyFloat_AsDouble(vo);
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < float > >::set(self->oid_, string(field), _value);
            }
            break;
        }              
        case 'S': {
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<string> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<string> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    char * v = PyString_AsString(vo);
                    Py_XDECREF(vo);
                    _value.push_back(string(v));
                }
                ret = Field< vector < string > >::set(self->oid_, string(field), _value);
            }
            break;
        }
        case 'T': {// vector< vector<unsigned int> >
            vector < vector <unsigned> > * _value = (vector < vector <unsigned> > *)to_cpp(value, ftype);
            if (!PyErr_Occurred()){
                ret = Field < vector < vector <unsigned> > >::set(self->oid_, string(field), *_value);
            }
            delete _value;
            break;
        }
        case 'Q': {// vector< vector<int> >
            vector < vector <int> > * _value = (vector < vector <int> > *)to_cpp(value, ftype);
            if (!PyErr_Occurred()){
                ret = Field < vector < vector <int> > >::set(self->oid_, string(field), *_value);
            }
            delete _value;
            break;
        }
        case 'R': {// vector< vector<double> >
            vector < vector <double> > * _value = (vector < vector <double> > *)to_cpp(value, ftype);
            if (!PyErr_Occurred()){
                ret = Field < vector < vector <double> > >::set(self->oid_, string(field), *_value);
            }
            delete _value;
            break;
        }
        case 'X': {//SET_VECFIELD(Id, f)
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<Id> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<Id> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    Id v = ((_Id*)vo)->id_;
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < Id > >::set(self->oid_, string(field), _value);
            }
            break;
        }              
        case 'Y': {//SET_VECFIELD(ObjId, f)
            if (!PySequence_Check(value)){
                PyErr_SetString(PyExc_TypeError, "For setting vector<ObjId> field, specified value must be a sequence." );
            } else {
                Py_ssize_t length = PySequence_Length(value);
                vector<ObjId> _value;
                for ( int ii = 0; ii < length; ++ii){
                    PyObject * vo = PySequence_GetItem(value, ii);
                    ObjId v = ((_ObjId*)vo)->oid_;
                    Py_XDECREF(vo);
                    _value.push_back(v);
                }
                ret = Field< vector < ObjId > >::set(self->oid_, string(field), _value);
            }
            break;
        }              
        default:                
            break;
    }
    // MOOSE Field::set returns 1 for success 0 for
    // failure. Python treats return value 0 from stters as
    // success, anything else failure.
    if (ret){
        return 0;
    } else {
        ostringstream msg;
        msg <<  "Failed to set field '"  << field << "'";
        PyErr_SetString(PyExc_AttributeError,msg.str().c_str());
        return -1;
    }
} // moose_ObjId_setattro

PyObject * moose_ObjId_getItem(_ObjId * self, Py_ssize_t index)
{
    if (index < 0){
        index += moose_ObjId_getLength(self);
    }
    if ((index < 0) || (index >= moose_ObjId_getLength(self))){
        PyErr_SetString(PyExc_IndexError, "Index out of bounds.");
        return NULL;
    }
    // Here I am assuming the user can start with any ObjId and
    // ask for an index - which will be field index.
    // Thus if syn[0...9] correspond to chan[0...9], then syn[0] is still a valid ObjId.
    // For example, syn has Id X, dataIndex 0...9, and under dataIndex=0, we have 5 field elements f[0...5]
    // Then syn = Id(X)
    // syn[0] = ObjId(X, 0, 0) = syn[0][0]
    // assign s0 <- syn[0]
    // what is s0[1]? ObjId(X
    // syn[0][1]->ObjId(X, 0, 1) =syn[0][0][0] - which is an ObjId.
    // Now, what is syn[0][1][2] ?
    
    // In PyMOOSE, user is allowed to directly put in the numbers
    // for Id, dataIndex and fieldIndex directly and construct an
    // ObjId. 
    _ObjId * ret = PyObject_New(_ObjId, &ObjIdType);
    ret->oid_ = ObjId(self->oid_.id, self->oid_.dataIndex, index);
    return (PyObject*)ret;
}

PyObject * moose_ObjId_getSlice(_ObjId * self, Py_ssize_t start, Py_ssize_t end)
{
    Py_ssize_t len = moose_ObjId_getLength(self);
    while (start < 0){
        start += len;
    }
    while (end < 0){
        end += len;
    }
    if (start > end){
        // PyErr_SetString(PyExc_IndexError, "Start index must be less than end.");
        // python itself returns empty tuple - follow that
        return PyTuple_New(0);
    }
    PyObject * ret = PyTuple_New((Py_ssize_t)(end - start));
    
    // Py_XINCREF(ret);        
    for ( int ii = start; ii < end; ++ii){
        _ObjId * value = PyObject_New(_ObjId, &ObjIdType);
        value->oid_ = ObjId(self->oid_.id, self->oid_.dataIndex, ii);
        if (PyTuple_SetItem(ret, (Py_ssize_t)(ii-start), (PyObject*)value)){ // danger - must we DECREF all prior values?
            Py_XDECREF(ret);
            // Py_XDECREF(value);
            PyErr_SetString(PyExc_RuntimeError, "Failed to assign tuple entry.");
            return NULL;
        }
    }
    return ret;
}


Py_ssize_t moose_ObjId_getLength(_ObjId * self)
{
    Element * el = self->oid_.element();
    if (!el->hasFields()){
        return 0;
    }
    FieldElement * fe = reinterpret_cast< FieldElement* >(el);
    if (fe == NULL){
        return 0;
    }
    return (Py_ssize_t)(fe->numData());
}

/// Inner function for looking up value from LookupField on object
/// with ObjId target.
///
/// args should be a tuple (lookupFieldName, key)
PyObject * getLookupField(ObjId target, char * fieldName, PyObject * key)
{
    vector<string> type_vec;
    if (parseFinfoType(Field<string>::get(target, "className"), "lookupFinfo", string(fieldName), type_vec) < 0){
        ostringstream error;
        error << "Cannot handle key type for LookupField `" << Field<string>::get(target, "className") << "." << fieldName << "`.";
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return NULL;
    }
    if (type_vec.size() != 2){
        ostringstream error;
        error << "LookupField type signature should be <keytype>, <valuetype>. But for `"
              << Field<string>::get(target, "className") << "." << fieldName << "` got " << type_vec.size() << " components." ;
        PyErr_SetString(PyExc_AssertionError, error.str().c_str());
        return NULL;
    }
    PyObject * ret = NULL;
    char key_type_code = shortType(type_vec[0]);
    char value_type_code = shortType(type_vec[1]);
    switch(key_type_code){
        case 'b': {
            ret = lookup_value <bool> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'c': {
            ret = lookup_value <char> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'h': {
            ret = lookup_value <short> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }            
        case 'H': {
            ret = lookup_value <unsigned short> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }            
        case 'i': {
            ret = lookup_value <int> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }            
        case 'I': {
            ret = lookup_value <unsigned int> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }            
        case 'l': {
            ret = lookup_value <long> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }                        
        case 'k': {
            ret = lookup_value <unsigned long> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }                        
        case 'L': {
            ret = lookup_value <long long> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }                        
        case 'K': {
            ret = lookup_value <unsigned long long> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }                        
        case 'd': {
            ret = lookup_value <double> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }                        
        case 'f': {
            ret = lookup_value <float> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 's': {
            ret = lookup_value <string> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'x': {
            ret = lookup_value <Id> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'y': {
            ret = lookup_value <ObjId> (target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'D': {
            ret = lookup_value < vector <double> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }                
        case 'S': {
            ret = lookup_value < vector <string> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'X': {
            ret = lookup_value < vector <Id> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'Y': {
            ret = lookup_value < vector <ObjId> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'v': {
            ret = lookup_value < vector <int> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'N': {
            ret = lookup_value < vector <unsigned int> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'P': {
            ret = lookup_value < vector <unsigned long> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        case 'F': {
            ret = lookup_value < vector <float> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }                
        case 'w': {
            ret = lookup_value < vector <short> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }   
        case 'C': {
            ret = lookup_value < vector <char> >(target, string(fieldName), value_type_code, key_type_code, key);
            break;
        }
        default:
            ostringstream error;
            error << "Unhandled key type `" << type_vec[0] << "` for " << Field<string>::get(target, "className") << "." << fieldName;
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
    }
    return ret;
}

PyDoc_STRVAR(moose_ObjId_getLookupField_documentation,
             "getLookupField(fieldname, key) -> value type\n"
             "\n"
             "Lookup entry for `key` in `fieldName`\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldname : string\n"
             "    Name of the lookupfield.\n"
             "\n"
             "key : appropriate type for key of the lookupfield (as in the dict "
             "    getFieldDict).\n"
             "    Key for the look-up.");

PyObject * moose_ObjId_getLookupField(_ObjId * self, PyObject * args)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getLookupField");
    }
    char * fieldName = NULL;
    PyObject * key = NULL;
    if (!PyArg_ParseTuple(args, "sO:moose_ObjId_getLookupField", &fieldName,  &key)){
        return NULL;
    }
    return getLookupField(self->oid_, fieldName, key);
} // moose_ObjId_getLookupField

int setLookupField(ObjId target, char * fieldName, PyObject * key, PyObject * value)
{
    vector<string> type_vec;
    if (parseFinfoType(Field<string>::get(target, "className"), "lookupFinfo", string(fieldName), type_vec) < 0){
        ostringstream error;
        error << "Cannot handle key type for LookupField `" << Field<string>::get(target, "className") << "." << fieldName << "`.";
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return -1;
    }
    if (type_vec.size() != 2){
        ostringstream error;
        error << "LookupField type signature should be <keytype>, <valuetype>. But for `"
              << Field<string>::get(target, "className") << "." << fieldName << "` got " << type_vec.size() << " components." ;
        PyErr_SetString(PyExc_AssertionError, error.str().c_str());
        return -1;
    }
    char key_type_code = shortType(type_vec[0]);
    char value_type_code = shortType(type_vec[1]);
    int ret = -1;
    switch(key_type_code){
        case 'I': {
            ret = set_lookup_value <unsigned int> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }            
        case 'k': {
            ret = set_lookup_value <unsigned long> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }                        
        case 's': {
            ret = set_lookup_value <string> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }
        case 'i': {
            ret = set_lookup_value <int> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }            
        case 'l': {
            ret = set_lookup_value <long> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }                        
        case 'L': {
            ret = set_lookup_value <long long> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }                        
        case 'K': {
            ret = set_lookup_value <unsigned long long> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }                        
        case 'b': {
            ret = set_lookup_value <bool> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }
        case 'c': {
            ret = set_lookup_value <char> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }
        case 'h': {
            ret = set_lookup_value <short> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }            
        case 'H': {
            ret = set_lookup_value <unsigned short> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }            
        case 'd': {
            ret = set_lookup_value <double> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }                        
        case 'f': {
            ret = set_lookup_value <float> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }
        case 'x': {
            ret = set_lookup_value <Id> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }
        case 'y': {
            ret = set_lookup_value <ObjId> (target, string(fieldName), value_type_code, key_type_code, key, value);
            break;
        }
        default:
            ostringstream error;
            error << "setLookupField: invalid key type " << type_vec[0];
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
    }
    return ret;        
}// setLookupField

PyDoc_STRVAR(moose_ObjId_setLookupField_documentation,
             "setLookupField(fieldname, key, value)\n"
             "\n"
             "Set a lookup field entry.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldname : str\n"
             "    name of the field to be set\n"
             "key : key type\n"
             "    key in the lookup field for which the value is to be set.\n"
             "value : value type\n"
             "    value to be set for `key` in the lookup field.\n");

PyObject * moose_ObjId_setLookupField(_ObjId * self, PyObject * args)
{
    if (!Id::isValid(self->oid_.id)){
        return NULL;
    }
    PyObject * key;
    PyObject * value;
    char * field;
    if (!PyArg_ParseTuple(args, "sOO:moose_ObjId_setLookupField", &field,  &key, &value)){
        return NULL;
    }
    if ( setLookupField(self->oid_, field, key, value) == 0){
        Py_RETURN_NONE;
    }
    return NULL;
}// moose_ObjId_setLookupField

PyDoc_STRVAR(moose_ObjId_setDestField_documentation,
             "setDestField(arg0, arg1, ...)\n"
             "\n"
             "Set a destination field. This is for advanced uses. destFields can\n"
             "(and should) be directly called like functions as\n"
             "`element.fieldname(arg0, ...)`\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "The number and type of paramateres depend on the destFinfo to be\n"
             "set. Use moose.doc('{classname}.{fieldname}') to get builtin\n"
             "documentation on the destFinfo `fieldname`\n"
             );

PyObject * moose_ObjId_setDestField(_ObjId * self, PyObject * args)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_setDestField");
    }
    PyObject * arglist[10] = {NULL, NULL, NULL, NULL, NULL,
                              NULL, NULL, NULL, NULL, NULL};
    ostringstream error;
    ObjId oid = ((_ObjId*)self)->oid_;

    error << "moose.setDestField: ";
    // Unpack the arguments
    if (!PyArg_UnpackTuple(args, "setDestField", minArgs, maxArgs,
                           &arglist[0], &arglist[1], &arglist[2],
                           &arglist[3], &arglist[4], &arglist[5],
                           &arglist[6], &arglist[7], &arglist[8],
                           &arglist[9])){
        error << "At most " << maxArgs - 1 << " arguments can be handled.";
        PyErr_SetString(PyExc_ValueError, error.str().c_str());
        return NULL;
    }
    
    // Get the destFinfo name
    char * fieldName = PyString_AsString(arglist[0]);
    if (!fieldName){ // not a string, raises TypeError
        error << "first argument must be a string specifying field name.";
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return NULL;
    }
    
    // Try to parse the arguments.
    vector< string > argType;
    if (parseFinfoType(Field<string>::get(oid, "className"),
                         "destFinfo", string(fieldName), argType) < 0){
        error << "Arguments not handled: " << fieldName << "(";
        for (unsigned int ii = 0; ii < argType.size(); ++ii){
            error << argType[ii] << ",";
        }
        error << ")";
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return NULL;
    }
    if (argType.size() == 1){
        if ( arglist[1] == NULL && argType[0] == "void"){
            bool ret = SetGet0::set(oid, string(fieldName));
            if (ret){
                Py_RETURN_TRUE;
            } else {
                Py_RETURN_FALSE;
            }
        }
        return setDestFinfo(oid, string(fieldName), arglist[1], argType[0]);
    } else if (argType.size() == 2){
        return setDestFinfo2(oid, string(fieldName), arglist[1], shortType(argType[0]), arglist[2], shortType(argType[1]));
    } else {
        error << "Can handle only up to 2 arguments" << endl;
        return NULL;
    }
} // moose_ObjId_setDestField

PyObject * setDestFinfo(ObjId obj, string fieldName, PyObject *arg, string argType)
{
    char typecode = shortType(argType);
    bool ret;
    ostringstream error;
    error << "moose.setDestFinfo: ";

switch (typecode){                    
    case 'f': case 'd': {
        double param = PyFloat_AsDouble(arg);
        if (typecode == 'f'){
            ret = SetGet1<float>::set(obj, fieldName, (float)param);
        } else {
            ret = SetGet1<double>::set(obj, fieldName, param);
        }
    }
        break;
    case 's': {
        char * param = PyString_AsString(arg);
        ret = SetGet1<string>::set(obj, fieldName, string(param));
    }
        break;
    case 'i': case 'l': {
        long param = PyInt_AsLong(arg);
        if (param == -1 && PyErr_Occurred()){
            return NULL;
        }
        if (typecode == 'i'){
            ret = SetGet1<int>::set(obj, fieldName, (int)param);
        } else {
            ret = SetGet1<long>::set(obj, fieldName, param);
        }
    }
        break;
    case 'I': case 'k':{
        unsigned long param =PyLong_AsUnsignedLong(arg);
        if (PyErr_Occurred()){
            return NULL;
        }
        if (typecode == 'I'){
            ret = SetGet1< unsigned int>::set(obj, fieldName, (unsigned int)param);
        } else {
            ret = SetGet1<unsigned long>::set(obj, fieldName, param);
        }
    }
        break;
    case 'x': {
        Id param;
        _Id * id = (_Id*)(arg);
        if (id == NULL){
            error << "argument should be an vec or an melement";
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
            return NULL;                                
        }
        param = id->id_;
        ret = SetGet1<Id>::set(obj, fieldName, param);
    }
        break;
    case 'y': {
        ObjId param;
        _ObjId * oid = (_ObjId*)(arg);
        if (oid == NULL){
            error << "argument should be vec or an melement";
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
            return NULL;                                
        }
        param = oid->oid_;
        ret = SetGet1<ObjId>::set(obj, fieldName, param);
    }
        break;
    case 'c': {
        char * param = PyString_AsString(arg);
        if (!param){
            error << "expected argument of type char/string";
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
            return NULL;
        } else if (strlen(param) == 0){
            error << "Empty string not allowed.";
            PyErr_SetString(PyExc_ValueError, error.str().c_str());
            return NULL;
        }
        ret = SetGet1<char>::set(obj, fieldName, param[0]);
    }
        break;
        ////////////////////////////////////////////////////
        // We do NOT handle multiple vectors. Use the argument
        // list as a single vector argument.
        ////////////////////////////////////////////////////
    case 'v': {
        return _set_vector_destFinfo<int>(obj, string(fieldName), arg, typecode);
    }
    case 'w': {
        return _set_vector_destFinfo<short>(obj, string(fieldName), arg, typecode);
    }
    case 'L': {//SET_VECFIELD(long, l) {
        return _set_vector_destFinfo<long>(obj, string(fieldName), arg, typecode);
    }            
    case 'N': { //SET_VECFIELD(unsigned int, I)
        return _set_vector_destFinfo<unsigned int>(obj, string(fieldName), arg, typecode);
    }
    case 'K': {//SET_VECFIELD(unsigned long, k)
        return _set_vector_destFinfo<unsigned long>(obj, string(fieldName), arg, typecode);
    }
    case 'F': {//SET_VECFIELD(float, f)
        return _set_vector_destFinfo<float>(obj, string(fieldName), arg, typecode);
    }
    case 'D': {//SET_VECFIELD(double, d)
        return _set_vector_destFinfo<double>(obj, string(fieldName), arg, typecode);
    }                
    case 'S': {
        return _set_vector_destFinfo<string>(obj, string(fieldName), arg, typecode);
    }
    case 'X': {
        return _set_vector_destFinfo<Id>(obj, string(fieldName), arg, typecode);
    }
    case 'Y': {
        return _set_vector_destFinfo<ObjId>(obj, string(fieldName), arg, typecode);
    }
    default: {
        error << "Cannot handle argument type: " << argType;
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return NULL;
    }
} // switch (shortType(argType[ii])
if (ret){
    Py_RETURN_TRUE;
} else {
    Py_RETURN_FALSE;
}

}

/**
Set destFinfo for 2 argument destination field functions.
*/
// template <class A>
PyObject* setDestFinfo2(ObjId obj, string fieldName, PyObject * arg1, char type1, PyObject * arg2, char type2)
{
ostringstream error;
error << "moose.setDestFinfo2: ";
switch (type2){
    case 'f': case 'd': {
        double param = PyFloat_AsDouble(arg2);
        if (type2 == 'f'){
            return setDestFinfo1<float>(obj, fieldName, arg1, type1, (float)param);
        } else {
            return setDestFinfo1<double>(obj, fieldName, arg1, type1, param);
        }
    }
    case 's': {
        char * param = PyString_AsString(arg2);
        return setDestFinfo1<string>(obj, fieldName, arg1, type1, string(param));
    }
    case 'i': case 'l': {
        long param = PyInt_AsLong(arg2);
        if (param == -1 && PyErr_Occurred()){
            return NULL;
        }
        if (type2 == 'i'){
            return setDestFinfo1< int>(obj, fieldName, arg1, type1, (int)param);
        } else {
            return setDestFinfo1< long>(obj, fieldName, arg1, type1, param);
        }
    }
    case 'I': case 'k':{
        unsigned long param =PyLong_AsUnsignedLong(arg2);
        if (PyErr_Occurred()){
            return NULL;
        }
        if (type2 == 'I'){
            return setDestFinfo1< unsigned int>(obj, fieldName, arg1, type1, (unsigned int)param);
        } else {
            return setDestFinfo1< unsigned long>(obj, fieldName, arg1, type1, param);
        }
    }
    case 'x': {
        Id param;
        // if (Id_SubtypeCheck(arg)){
            _Id * id = (_Id*)(arg2);
            if (id == NULL){
                error << "argument should be an vec or an melement";
                PyErr_SetString(PyExc_TypeError, error.str().c_str());
                return NULL;                                
            }
            param = id->id_;
        // } else if (ObjId_SubtypeCheck(arg)){
        //     _ObjId * oid = (_ObjId*)(arg);
        //     if (oid == NULL){
        //         error << "argument should be an vec or an melement";
        //         PyErr_SetString(PyExc_TypeError, error.str().c_str());
        //         return NULL;                                
        //     }
        //     param = oid->oid_.id;
        // }
        return setDestFinfo1< Id>(obj, fieldName, arg1, type1, param);
    }
    case 'y': {
        ObjId param;
        // if (Id_SubtypeCheck(arg)){
        //     _Id * id = (_Id*)(arg);
        //     if (id == NULL){
        //         error << "argument should be an vec or an melement";
        //         PyErr_SetString(PyExc_TypeError, error.str().c_str());
        //         return NULL;                                
        //     }
        //     param = ObjId(id->id_);
        // } else if (ObjId_SubtypeCheck(arg)){
            _ObjId * oid = (_ObjId*)(arg2);
            if (oid == NULL){
                error << "argument should be an vec or an melement";
                PyErr_SetString(PyExc_TypeError, error.str().c_str());
                return NULL;                                
            }
            param = oid->oid_;
        // }
        return setDestFinfo1< ObjId>(obj, fieldName, arg1, type1, param);
    }
    case 'c': {
        char * param = PyString_AsString(arg2);
        if (!param){
            error << "expected argument of type char/string";
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
            return NULL;
        } else if (strlen(param) == 0){
            error << "Empty string not allowed.";
            PyErr_SetString(PyExc_ValueError, error.str().c_str());
            return NULL;
        }
        return setDestFinfo1< char>(obj, fieldName, arg1, type1, param[0]);
    }
    default: {
        error << "Unhandled argument 2 type (shortType=" << type2 << ")";
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return NULL;
    }
}
}


PyDoc_STRVAR(moose_ObjId_getFieldNames_documenation,
             "getFieldNames(fieldType='') -> tuple of str\n"
             "\n"
             "Get the names of fields on this element.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldType : str\n"
             "    Field type to retrieve. Can be `valueFinfo`, `srcFinfo`,\n"
             "   `destFinfo`, `lookupFinfo`, etc. If an empty string is specified,\n"
             "    names of all avaialable fields are returned.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "names : tuple of strings.\n"
             "    names of the fields of the specified type.\n"   
             "\n"
             "Examples\n"
             "--------\n"
             "List names of all the source fields in PulseGen class:\n"
             "\n"
             "    >>> moose.getFieldNames('PulseGen', 'srcFinfo')\n"
             "    ('childMsg', 'output')\n"
             "\n");
// 2011-03-23 15:28:26 (+0530)
PyObject * moose_ObjId_getFieldNames(_ObjId * self, PyObject *args)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getFieldNames");
    }
    char * ftype = NULL;
    if (!PyArg_ParseTuple(args, "|s:moose_ObjId_getFieldNames", &ftype)){
        return NULL;
    }
    string ftype_str = (ftype != NULL)? string(ftype): "";
    vector<string> ret;
    string className = Field<string>::get(self->oid_, "className");
    if (ftype_str == ""){
        for (const char **a = getFinfoTypes(); *a; ++a){
            vector<string> fields = getFieldNames(className, string(*a));
            ret.insert(ret.end(), fields.begin(), fields.end());
        }            
    } else {
        ret = getFieldNames(className, ftype_str);
    }
    
    PyObject * pyret = PyTuple_New((Py_ssize_t)ret.size());
            
    for (unsigned int ii = 0; ii < ret.size(); ++ ii ){
        PyObject * fname = Py_BuildValue("s", ret[ii].c_str());
        if (!fname){
            Py_XDECREF(pyret);
            pyret = NULL;
            break;
        }
        if (PyTuple_SetItem(pyret, (Py_ssize_t)ii, fname)){
            Py_XDECREF(pyret);
            // Py_DECREF(fname);
            pyret = NULL;
            break;
        }
    }
    return pyret;             
}

PyDoc_STRVAR(moose_ObjId_getNeighbors_documentation,
             "getNeighbors(fieldName) -> tuple of vecs\n"
             "\n"
             "Get the objects connected to this element on specified field.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldName : str\n"
             "    name of the connection field (a destFinfo/srcFinfo/sharedFinfo)\n"
             "\n"
             "Returns\n"
             "-------\n"
             "neighbors: tuple of vecs.\n"
             "    tuple containing the ids of the neighbour vecs.\n"
             "\n");
             
PyObject * moose_ObjId_getNeighbors(_ObjId * self, PyObject * args)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getNeighbors");
    }
    char * field = NULL;
    if (!PyArg_ParseTuple(args, "s:moose_ObjId_getNeighbors", &field)){
        return NULL;
    }
    vector< Id > val = LookupField< string, vector< Id > >::get(self->oid_, "neighbors", string(field));

    PyObject * ret = PyTuple_New((Py_ssize_t)val.size());
            
    for (unsigned int ii = 0; ii < val.size(); ++ ii ){            
        _Id * entry = PyObject_New(_Id, &IdType);
        if (!entry || PyTuple_SetItem(ret, (Py_ssize_t)ii, (PyObject*)entry)){
            Py_DECREF(ret);
            // Py_DECREF((PyObject*)entry);
            ret = NULL;                                 
            break;                                      
        }
        entry->id_ = val[ii];
    }
    return ret;
}

// 2011-03-28 10:10:19 (+0530)
// 2011-03-23 15:13:29 (+0530)
// getChildren is not required as it can be accessed as getField("children")

// 2011-03-28 10:51:52 (+0530)
PyDoc_STRVAR(moose_ObjId_connect_documentation,
             "connect(srcfield, destobj, destfield, msgtype) -> bool\n"
             "\n"
             "Connect another object via a message.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "srcfield : str\n"
             "    source field on self.\n"
             "destobj : element\n"
             "    Destination object to connect to.\n"
             "destfield : str\n"
             "    field to connect to on `destobj`.\n"
             "msgtype : str\n"
             "    type of the message. Can be `Single`, `OneToAll`, `AllToOne`,\n"
             "   `OneToOne`, `Reduce`, `Sparse`. Default: `Single`.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "msgmanager: melement\n"
             "    message-manager for the newly created message.\n"
             "\n"
             "Examples\n"
             "--------\n"
             "Connect the output of a pulse generator to the input of a spike\n"
             "generator::\n"
             "\n"
             "    >>> pulsegen = moose.PulseGen('pulsegen')\n"
             "    >>> spikegen = moose.SpikeGen('spikegen')\n"
             "    >>> pulsegen.connect('output', spikegen, 'Vm')\n"
             "\n"
             "See also\n"
             "--------\n"
             "moose.connect\n"
             "\n"
             );
PyObject * moose_ObjId_connect(_ObjId * self, PyObject * args)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_connect");
    }
    extern PyTypeObject ObjIdType;        
    PyObject * destPtr = NULL;
    char * srcField = NULL, * destField = NULL, * msgType = NULL;
    static char default_msg_type[] = "Single";
    if(!PyArg_ParseTuple(args,
                         "sOs|s:moose_ObjId_connect",
                         &srcField,
                         &destPtr,
                         &destField,
                         &msgType)){
        return NULL;
    }
    if (msgType == NULL){
        msgType = default_msg_type;
    }
    _ObjId * dest = reinterpret_cast<_ObjId*>(destPtr);
    ObjId mid = SHELLPTR->doAddMsg(msgType,
                                   self->oid_,
                                   string(srcField),
                                   dest->oid_,
                                   string(destField));
    if (mid.bad()){
        PyErr_SetString(PyExc_NameError,
                        "connect failed: check field names and type compatibility.");
        return NULL;
    }
    _ObjId* msgMgrId = (_ObjId*)PyObject_New(_ObjId, &ObjIdType);        
    msgMgrId->oid_ = mid;
    return (PyObject*)msgMgrId;
}

PyDoc_STRVAR(moose_ObjId_richcompare_documentation,
             "Compare two element instances. This just does a string comparison of\n"
             "the paths of the element instances. This function exists only to\n"
             "facilitate certain operations requiring sorting/comparison, like\n"
             "using elements for dict keys. Conceptually only equality comparison is\n"
             "meaningful for elements.\n"); 
PyObject* moose_ObjId_richcompare(_ObjId * self, PyObject * other, int op)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_richcompare");
    }
    extern PyTypeObject ObjIdType;
    if ((self != NULL && other == NULL) || (self == NULL && other != NULL)){
        if (op == Py_EQ){
            Py_RETURN_FALSE;
        } else if (op == Py_NE){
            Py_RETURN_TRUE;
        } else {
            PyErr_SetString(PyExc_TypeError, "Cannot compare NULL with non-NULL");
            return NULL;
        }
    }
    if (!PyObject_IsInstance(other, (PyObject*)&ObjIdType)){
        ostringstream error;
        error << "Cannot compare ObjId with "
              << Py_TYPE(other)->tp_name;
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return NULL;
    }
    if (!Id::isValid(((_ObjId*)other)->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_richcompare");
    }

    string l_path = self->oid_.path();
    string r_path = ((_ObjId*)other)->oid_.path();
    int result = l_path.compare(r_path);
    if (result == 0){
        if (op == Py_EQ || op == Py_LE || op == Py_GE){
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    } else if (result < 0){
        if (op == Py_LT || op == Py_LE || op == Py_NE){
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    } else {
        if (op == Py_GT || op == Py_GE || op == Py_NE){
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
}

PyDoc_STRVAR(moose_ObjId_getDataIndex_documentation,
             "getDataIndex() -> int\n"
             "\n"
             "Get the dataIndex of this object.\n"
             );
PyObject * moose_ObjId_getDataIndex(_ObjId * self)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getDataIndex");
    }        
    PyObject * ret = Py_BuildValue("I", self->oid_.dataIndex);
    return ret;
}

// WARNING: fieldIndex has been deprecated in dh_branch. This
// needs to be updated accordingly.  The current code is just
// place-holer to avoid compilation errors.
PyObject * moose_ObjId_getFieldIndex(_ObjId * self)
{
    if (!Id::isValid(self->oid_.id)){
        RAISE_INVALID_ID(NULL, "moose_ObjId_getFieldIndex");
    }
    PyObject * ret = Py_BuildValue("I", self->oid_.dataIndex);
    return ret;
}

///////////////////////////////////////////////
// Python method lists for PyObject of ObjId
///////////////////////////////////////////////

static PyMethodDef ObjIdMethods[] = {
    {"getFieldType", (PyCFunction)moose_ObjId_getFieldType, METH_VARARGS,
     moose_ObjId_getFieldType_documentation},        
    {"getField", (PyCFunction)moose_ObjId_getField, METH_VARARGS,
     moose_ObjId_getField_documentation},
    {"setField", (PyCFunction)moose_ObjId_setField, METH_VARARGS,
     moose_ObjId_setField_documentation},
    {"getLookupField", (PyCFunction)moose_ObjId_getLookupField, METH_VARARGS,
     moose_ObjId_getLookupField_documentation},
    {"setLookupField", (PyCFunction)moose_ObjId_setLookupField, METH_VARARGS,
     moose_ObjId_setLookupField_documentation},
    {"getId", (PyCFunction)moose_ObjId_getId, METH_NOARGS,
     moose_ObjId_getId_documentation},
    {"vec", (PyCFunction)moose_ObjId_getId, METH_NOARGS,
     "Return the vec this element belongs to. This is overridden by the"
     " attribute of the same name for quick access."},
    {"getFieldNames", (PyCFunction)moose_ObjId_getFieldNames, METH_VARARGS,
     moose_ObjId_getFieldNames_documenation},
    {"getNeighbors", (PyCFunction)moose_ObjId_getNeighbors, METH_VARARGS,
     moose_ObjId_getNeighbors_documentation},
    {"connect", (PyCFunction)moose_ObjId_connect, METH_VARARGS,
     moose_ObjId_connect_documentation},
    {"getDataIndex", (PyCFunction)moose_ObjId_getDataIndex, METH_NOARGS,
     moose_ObjId_getDataIndex_documentation},
    {"getFieldIndex", (PyCFunction)moose_ObjId_getFieldIndex, METH_NOARGS,
     "Get the index of this object as a field."},
    {"setDestField", (PyCFunction)moose_ObjId_setDestField, METH_VARARGS,
     moose_ObjId_setDestField_documentation},
    {NULL, NULL, 0, NULL},        /* Sentinel */        
};


///////////////////////////////////////////////
// Type defs for PyObject of ObjId
///////////////////////////////////////////////
PyDoc_STRVAR(moose_ObjId_documentation,
             "Individual moose element contained in an array-type object"
             " (vec).\n"
             "\n"
             "Each element has a unique path, possibly with its index in"
             " the vec. These are identified by three components: vec, dndex and"
             " findex. vec is the containing vec, which is identified by a unique"
             " number (field `value`). `dindex` is the index of the current"
             " item in the containing vec. `dindex` is 0 for single elements."
             " findex is field index, specifying the index of elements which exist"
             " as fields of another moose element.\n"
             "\n"
             "Notes\n"
             "-----\n"
             "Users need not create melement directly. Instead use the named moose"
             " class for creating new elements. To get a reference to an existing"
             " element, use the :ref:`moose.element` function.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "path : str\n"
             "    path of the element to be created.\n"
             "\n"
             "n : positive int, optional\n"
             "    defaults to 1. If greater, then a vec of that size is created and\n"
             "    this element is a reference to the first entry of the vec.\n"
             "\n"
             "g : int, optional\n"
             "    if 1, this is a global element, else if 0 (default), the element\n"
             "    is local to the current node.\n"
             "\n"
             "dtype : str\n"
             "    name of the class of the element. If creating any concrete\n"
             "    subclass, this is automatically taken from the class name and\n"
             "    should not be specified explicitly.\n"
             "\n"
             "Attributes\n"
             "----------\n"
             "vec : moose.vec\n"
             "    The vec containing this element. `vec` wraps the Id of the object in MOOSE C++ API.\n"
             "\n"
             "dindex : int\n"
             "    index of this element in the container vec\n"
             "\n"
             "findex: int\n"
             "    if this is a tertiary object, i.e. acts as a field in another\n"
             "    element (like synapse[0] in IntFire[1]), then the index of\n"
             "    this field in the containing element.\n"
             "\n"
             "Examples\n"
             "--------\n"
             "\n"
             ">>> a = Neutral('alpha') # Creates element named `alpha` under current working element\n"
             ">>> b = Neutral('alpha/beta') # Creates the element named `beta` under `alpha`\n"
             ">>> c = moose.melement(b)"
             );

PyTypeObject ObjIdType = { 
    PyVarObject_HEAD_INIT(NULL, 0)            /* tp_head */
    "moose.melement",                      /* tp_name */
    sizeof(_ObjId),                     /* tp_basicsize */
    0,                                  /* tp_itemsize */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,       /* tp_compare */
    (reprfunc)moose_ObjId_repr,         /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    (hashfunc)moose_ObjId_hash,         /* tp_hash */
    0,                                  /* tp_call */
    (reprfunc)moose_ObjId_str,         /* tp_str */
    (getattrofunc)moose_ObjId_getattro, /* tp_getattro */
    (setattrofunc)moose_ObjId_setattro, /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    moose_ObjId_documentation,
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    (richcmpfunc)moose_ObjId_richcompare, /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    ObjIdMethods,                       /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    (initproc) moose_ObjId_init,        /* tp_init */
    0,                                  /* tp_alloc */
    0,                                  /* tp_new */
    0,                                  /* tp_free */
};



// 
// melement.cpp ends here
