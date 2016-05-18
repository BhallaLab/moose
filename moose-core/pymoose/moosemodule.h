// moosemodule.h ---
//
// Filename: moosemodule.h
// Description:
// Author: Subhasis Ray
// Maintainer: Dilawar Singh
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Last-Updated: Mon 25 Apr 2016 11:09:02 AM IST
//           By: Dilawar
// URL:

#ifndef _MOOSEMODULE_H
#define _MOOSEMODULE_H

#include <string>

extern char shortType(string);

// declared in utility.h, defined in utility/types.cpp
extern char innerType(char);

// This had to be defined for py3k, but does not harm 2.
struct module_state
{
    PyObject *error;
};


// The endianness check is from:
// http://stackoverflow.com/questions/2100331/c-macro-definition-to-determine-big-endian-or-little-endian-machine
enum
{
    O32_LITTLE_ENDIAN = 0x03020100ul,
    O32_BIG_ENDIAN = 0x00010203ul,
    O32_PDP_ENDIAN = 0x01000302ul
};

const union
{
    unsigned char bytes[4];
    uint32_t value;
} o32_host_order =  { { 0, 1, 2, 3 } };

#define O32_HOST_ORDER (o32_host_order.value)

#if PY_MAJOR_VERSION >= 3
#define PY3K
PyMODINIT_FUNC PyInit_moose();

// int has been replaced by long
#define PyInt_Check PyLong_Check
#define PyInt_AsLong PyLong_AsLong
#define PyInt_AsUnsignedLongMask PyLong_AsUnsignedLongMask
#define PyInt_FromLong PyLong_FromLong
// string has been replaced by unicode
#define PyString_Check PyUnicode_Check
#define PyString_FromString PyUnicode_FromString
#define PyString_FromFormat PyUnicode_FromFormat
#define PyString_AsString(str)                                          \
    PyBytes_AS_STRING(PyUnicode_AsEncodedString(str, "utf-8", "Error~"))
// Python 3 does not like global module state
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else // Python 2
PyMODINIT_FUNC init_moose();
static struct module_state _state;
#define GETSTATE(m) (&_state)
#endif // if PY_MAJOR_VERSION

/* Shortcut to raise an exception when a bad Id is encountered */
#define RAISE_INVALID_ID(ret, msg) {                            \
        PyErr_SetString(PyExc_ValueError, msg": invalid Id");   \
        return ret;                                             \
    } // RAISE_INVALID_ID

#define RAISE_TYPE_ERROR(ret, type) {                           \
        PyErr_SetString(PyExc_TypeError, "require " #type);     \
        return ret;                                             \
    }


// Minimum number of arguments for setting destFinfo - 1-st
// the finfo name.
#define minArgs 1

// Arbitrarily setting maximum on variable argument list. Read:
// http://www.swig.org/Doc1.3/Varargs.html to understand why
#define maxArgs 10


///////////////////////////////////
// Python datatype checking macros
///////////////////////////////////

#define Id_Check(v) (Py_TYPE(v) == &IdType)
#define Id_SubtypeCheck(v) (PyType_IsSubtype(Py_TYPE(v),&IdType))
#define ObjId_Check(v) (Py_TYPE(v) == &ObjIdType)
#define ObjId_SubtypeCheck(v) (PyType_IsSubtype(Py_TYPE(v), &ObjIdType))
#define LookupField_Check(v) (Py_TYPE(v) == &moose_LookupField)
#define ElementField_Check(v) (Py_TYPE(v) == &moose_ElementField)
#define ElementField_SubtypeCheck(v) (PyType_IsSubtype(Py_TYPE(v), &moose_ElementField))


// Macro to create the Shell * out of shellId
#define SHELLPTR (reinterpret_cast<Shell*>(getShell(0, NULL).eref().data()))


/**
   _Id wraps the Id class - where each element is identified by Id
*/
typedef struct
{
    PyObject_HEAD
    Id id_;
} _Id;
/**
   _ObjId wraps the subelements of a Id - identified by
   index. This is different from the Element in GENESIS
   terminology. Since Neutral is now by default an array element,
   we call the individual entries in it as Elements.

   According to MOOSE API, ObjId is a composition of Id and DataId
   - thus uniquely identifying a field of a subelement of Neutral.

   Since the individual subelements are identified by their ObjId
   only (there is no intermediate id for the subelements except
   ObjId with DataId(index, 0) ), we use ObjId for recognizing
   both.
*/
typedef struct
{
    PyObject_HEAD
    ObjId oid_;
} _ObjId;

typedef struct
{
    PyObject_HEAD
    char * name;
    _ObjId * owner;
    ObjId myoid; // This is a placeholder for {Id, dataId} combo with fieldIndex=0, used only for fieldelements
} _Field;
//////////////////////////////////////////
// Methods for ElementField class
//////////////////////////////////////////
int moose_ElementField_init(_Field * self, PyObject * args, PyObject * kwargs);
int moose_ElementField_setNum(_Field * self, PyObject * num, void * closure);
PyObject* moose_ElementField_getNum(_Field * self, void * closure);
Py_ssize_t moose_ElementField_getLen(_Field * self, void * closure);
PyObject * moose_ElementField_getItem(_Field * self, Py_ssize_t index);
PyObject * moose_ElementField_getPath(_Field * self, void * closure);
PyObject * moose_ElementField_getId(_Field * self, void * closure);
PyObject * moose_ElementField_getName(_Field * self, void * closure);
PyObject * moose_ElementField_getOwner(_Field * self, void * closure);
PyObject * moose_ElementField_getDataId(_Field * self, void * closure);
PyObject * moose_ElementField_getSlice(_Field * self, Py_ssize_t start, Py_ssize_t end);
PyObject * moose_ElementField_getattro(_Field * self, PyObject * attr);
int moose_ElementField_setattro(_Field * self, PyObject * attr, PyObject * value);
/* PyObject * moose_ElementField_richcmp(_Field * self, void * closure); */
//////////////////////////////////////////
// Methods for Id class
//////////////////////////////////////////
int moose_Id_init(_Id * self, PyObject * args, PyObject * kwargs);
Id create_Id_from_path(string path, unsigned int numData, unsigned int isGlobal, string type); // inner function
long moose_Id_hash(_Id * self);

PyObject * moose_Id_repr(_Id * self);
PyObject * moose_Id_str(_Id * self);
PyObject * deleteObjId(ObjId obj); // inner function
PyObject * moose_Id_delete(_Id * self);
PyObject * moose_Id_getValue(_Id * self);
PyObject * moose_Id_getPath(_Id * self);
/* Id functions to allow part of sequence protocol */
Py_ssize_t moose_Id_getLength(_Id * self);
PyObject * moose_Id_getItem(_Id * self, Py_ssize_t index);
#ifndef PY3K
PyObject * moose_Id_getSlice(_Id * self, Py_ssize_t start, Py_ssize_t end);
#endif
PyObject * moose_Id_getShape(_Id * self);
PyObject * moose_Id_subscript(_Id * self, PyObject * op);
PyObject * moose_Id_richCompare(_Id * self, PyObject * args, int op);
int moose_Id_contains(_Id * self, PyObject * args);
PyObject * moose_Id_getattro(_Id * self, PyObject * attr);
int moose_Id_setattro(_Id * self, PyObject * attr, PyObject * value);
PyObject * moose_Id_setField(_Id * self, PyObject *args);
///////////////////////////////////////////
// Methods for ObjId class
///////////////////////////////////////////
int moose_ObjId_init(_ObjId * self, PyObject * args, PyObject * kwargs);
long moose_ObjId_hash(_ObjId * self);
PyObject * moose_ObjId_repr(_ObjId * self);
PyObject * moose_ObjId_getattro(_ObjId * self, PyObject * attr);
PyObject * moose_ObjId_getField(_ObjId * self, PyObject * args);
int moose_ObjId_setattro(_ObjId * self, PyObject * attr, PyObject * value);
PyObject * moose_ObjId_setField(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_getLookupField(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_setLookupField(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_setDestField(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_getFieldNames(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_getFieldType(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_getDataIndex(_ObjId * self);
PyObject * moose_ObjId_getFieldIndex(_ObjId * self);
PyObject * moose_ObjId_getNeighbors(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_getId(_ObjId * self);
PyObject * moose_ObjId_connect(_ObjId * self, PyObject * args);
PyObject * moose_ObjId_richcompare(_ObjId * self, PyObject * args, int op);
PyObject * moose_ObjId_getItem(_ObjId * self, Py_ssize_t index);
PyObject * moose_ObjId_getSlice(_ObjId * self, Py_ssize_t start, Py_ssize_t end);
Py_ssize_t moose_ObjId_getLength(_ObjId * self);

////////////////////////////////////////////
// Methods for LookupField
////////////////////////////////////////////
int moose_Field_init(_Field * self, PyObject * args, PyObject * kwds);
void moose_Field_dealloc(_Field * self);
long moose_Field_hash(_Field * self);
PyObject * moose_Field_repr(_Field * self);
PyObject * moose_LookupField_getItem(_Field * self, PyObject * key);
int moose_LookupField_setItem(_Field * self, PyObject * key, PyObject * value);


////////////////////////////////////////////////
//  functions to be accessed from Python
////////////////////////////////////////////////


// The following are global functions
PyObject * oid_to_element(ObjId oid);
PyObject * moose_element(PyObject * dummy, PyObject * args);
PyObject * moose_useClock(PyObject * dummy, PyObject * args);
PyObject * moose_setClock(PyObject * dummy, PyObject * args);
PyObject * moose_start(PyObject * dummy, PyObject * args);
PyObject * moose_reinit(PyObject * dummy, PyObject * args);
PyObject * moose_stop(PyObject * dummy, PyObject * args);
PyObject * moose_isRunning(PyObject * dummy, PyObject * args);
PyObject * moose_exists(PyObject * dummy, PyObject * args);
PyObject * moose_loadModel(PyObject * dummy, PyObject * args);
PyObject * moose_saveModel(PyObject * dummy, PyObject * args);
PyObject * moose_writeSBML(PyObject * dummy, PyObject * args);
PyObject * moose_readSBML(PyObject * dummy, PyObject * args);
PyObject * moose_setCwe(PyObject * dummy, PyObject * args);
PyObject * moose_getCwe(PyObject * dummy, PyObject * args);
PyObject * moose_copy(PyObject * dummy, PyObject * args, PyObject * kwargs);
PyObject * moose_move(PyObject * dummy, PyObject * args);
PyObject * moose_delete(PyObject * dummy, PyObject * args);
PyObject * moose_connect(PyObject * dummy, PyObject * args);
PyObject * moose_getFieldDict(PyObject * dummy, PyObject * args);
PyObject * moose_getField(PyObject * dummy, PyObject * args);
PyObject * moose_syncDataHandler(PyObject * dummy, PyObject * target);
PyObject * moose_seed(PyObject * dummy, PyObject * args);
PyObject * moose_wildcardFind(PyObject * dummy, PyObject * args);
// This should not be required or accessible to the user. Put here
// for debugging threading issue.
PyObject * moose_quit(PyObject * dummy);

//////////////////////////////////////////////////////////////
// These are internal functions and not exposed in Python
//////////////////////////////////////////////////////////////
PyObject * getLookupField(ObjId oid, char * fieldName, PyObject * key);
int setLookupField(ObjId oid, char * fieldName, PyObject * key, PyObject * value);
int defineClass(PyObject * module_dict, const Cinfo * cinfo);
int defineDestFinfos(const Cinfo * cinfo);
int defineAllClasses(PyObject* module_dict);
int defineLookupFinfos(const Cinfo * cinfo);
int defineElementFinfos(const Cinfo * cinfo);
PyObject * moose_ObjId_get_lookupField_attr(PyObject * self, void * closure);
PyObject * moose_ObjId_get_elementField_attr(PyObject * self, void * closure);
PyObject * moose_ObjId_get_destField_attr(PyObject * self, void * closure);
// PyObject * _setDestField(ObjId oid, PyObject * args);
PyObject * setDestFinfo(ObjId obj, string fieldName, PyObject *arg, string argType);
PyObject * setDestFinfo2(ObjId obj, string fieldName, PyObject * arg1, char type1, PyObject * arg2, char type2);
Id getShell(int argc, char **argv);
vector<int> pysequence_to_dimvec(PyObject * dims);
map<string, vector <PyGetSetDef> >& get_getsetdefs();
map<string, PyTypeObject *>& get_moose_classes();
int get_npy_typenum(const type_info& ctype);
string getFieldType(string className, string fieldName);
const map<string, string>& get_field_alias();
PyTypeObject * getBaseClass(PyObject * self);
int parseFinfoType(string className, string finfoType, string fieldName, vector<string> & typeVec);
vector<string> getFieldNames(string className, string finfoType);
PyObject * getObjIdAttr(_ObjId * oid, string attribute);
const char ** getFinfoTypes();
/**
   Convert PyObject to C++ object. Returns a pointer to the
   converted object. Deallocation is caller responsibility
*/
void * to_cpp(PyObject * object, char typecode);

/**
   Convert C++ object to Python.
*/
PyObject * to_py(void * obj, char typecode);
/**
   Convert C++ vector to Python tuple
*/
PyObject * to_pytuple(void * obj, char typecode);

/* inner fn for use in to_pytuple */
PyObject * convert_and_set_tuple_entry(PyObject * tuple, unsigned int index, void * vptr, char typecode);




/**
   Convert a Python sequence into a C++ vector.  This dynamically
   allocates the vector and it is the caller's responsibility to free
   it.
*/
template <typename T>
vector< T > * PySequenceToVector(PyObject * seq, char typecode)
{
    Py_ssize_t length = PySequence_Length(seq);
    vector <T> * ret = new vector<T>((unsigned int)length);
    T * value;
    for (unsigned int ii = 0; ii < length; ++ii)
    {
        PyObject * item = PySequence_GetItem(seq, ii);
        if (item == NULL)
        {
            ostringstream error;
            error << "Item # " << ii << "is NULL";
            PyErr_SetString(PyExc_ValueError, error.str().c_str());
            delete ret;
            return NULL;
        }
        value = (T*)to_cpp(item, typecode);
        Py_DECREF(item); // PySequence_GetItem returns a new reference. Reduce the refcount now.
        if (value == NULL)
        {
            ostringstream error;
            error << "Cannot handle sequence of type " << Py_TYPE(item)->tp_name;
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
            delete ret;
            return NULL;
        }
        ret->at(ii) = *value;
        delete value;
    }
    return ret;
}

template <typename T>
vector < vector < T > > * PySequenceToVectorOfVectors(PyObject * seq, char typecode)
{
    Py_ssize_t outerLength = PySequence_Length(seq);
    vector < vector <T> > * ret = new vector < vector < T > >((unsigned int) outerLength);
    for (unsigned int ii = 0; ii < outerLength; ++ii)
    {
        PyObject * innerSeq = PySequence_GetItem(seq, ii);
        if (innerSeq == NULL)
        {
            ostringstream error;
            error << "PySequenceToVectorOfVectors: error converting inner sequence " << ii;
            PyErr_SetString(PyExc_ValueError, error.str().c_str());
            delete ret;
            return NULL;
        }
        vector< T > * inner = PySequenceToVector< T >(innerSeq, typecode);
        Py_DECREF(innerSeq);
        if (inner == NULL)
        {
            delete ret;
            return NULL;
        }
        ret->at(ii).assign(inner->begin(), inner->end());
        delete inner;
    }
    return ret;
}

template <typename KeyType, typename ValueType>
PyObject * get_vec_lookupfield(ObjId oid, string fieldname, KeyType key, char vtypecode)
{
    vector<ValueType> value = LookupField<KeyType, vector<ValueType> >::get(oid, fieldname, key);
    return to_pytuple((void*)&value, innerType(vtypecode));
}

template <typename KeyType, typename ValueType>
PyObject * get_simple_lookupfield(ObjId oid, string fieldname, KeyType key, char vtypecode)
{
    ValueType value = LookupField<KeyType, ValueType>::get(oid, fieldname, key);
    PyObject * v = to_py(&value, vtypecode);
    return v;
}

template <class KeyType> PyObject *
lookup_value(const ObjId& oid,
             string fname,
             char value_type_code,
             char key_type_code,
             PyObject * key)
{
    PyObject * ret = NULL;
    KeyType * cpp_key = (KeyType *)to_cpp(key, key_type_code);
    if (cpp_key == NULL)
    {
        return NULL;
    }
    // The case statements are arranged roughly in order of frequency
    // of that data type. This is an attempt to optimize. We cannot
    // use a map of function pointers because part of the template
    // will be unspecialized (or we need N^2 templated fn instances).
    switch (value_type_code)
    {
    case 'd':
        ret = get_simple_lookupfield< KeyType, double>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'D':
        ret = get_vec_lookupfield<KeyType, double>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'S':
        ret = get_vec_lookupfield<KeyType, string>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'i':
        ret = get_simple_lookupfield < KeyType, int >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'I':
        ret = get_simple_lookupfield < KeyType, unsigned int >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'y':
        ret = get_simple_lookupfield< KeyType, ObjId >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'x':
        ret = get_simple_lookupfield<KeyType, Id>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'l':
        ret = get_simple_lookupfield< KeyType, long >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'k':
        ret = get_simple_lookupfield< KeyType, unsigned long >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'f':
        ret = get_simple_lookupfield< KeyType, float >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'b': // boolean is a special case that PyBuildValue does not handle
        ret = get_simple_lookupfield < KeyType, bool >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'c':
        ret = get_simple_lookupfield < KeyType, char >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'h':
        ret = get_simple_lookupfield < KeyType, short >(oid, fname, *cpp_key, value_type_code);
        break;
    case 'H':
        ret = get_simple_lookupfield< KeyType, unsigned short >(oid, fname, *cpp_key, value_type_code);
        break;
#ifdef HAVE_LONG_LONG
    case 'L':
        ret = get_simple_lookupfield< KeyType, long long>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'K':
        ret = get_simple_lookupfield< KeyType, unsigned long long>(oid, fname, *cpp_key, value_type_code);
        break;
#endif
    case 'X':
        ret = get_vec_lookupfield<KeyType, Id>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'Y':
        ret = get_vec_lookupfield<KeyType, ObjId>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'v':
        ret = get_vec_lookupfield<KeyType, int>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'w':
        ret = get_vec_lookupfield<KeyType, short>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'M':
        ret = get_vec_lookupfield<KeyType, long>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'N':
        ret = get_vec_lookupfield<KeyType, unsigned int>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'P':
        ret = get_vec_lookupfield<KeyType, unsigned long>(oid, fname, *cpp_key, value_type_code);
        break;
    case 'F':
        ret = get_vec_lookupfield<KeyType, float>(oid, fname, *cpp_key, value_type_code);
        break;
    default:
        PyErr_SetString(PyExc_TypeError, "invalid value type");
    }
    delete cpp_key;
    return ret;
}

template <class KeyType>
int set_lookup_value(const ObjId& oid, string fname, char value_type_code, char key_type_code, PyObject * key, PyObject * value_obj)
{
    bool success = false;
    KeyType *cpp_key = (KeyType*)to_cpp(key, key_type_code);
    if (cpp_key == NULL)
    {
        return -1;
    }
#define SET_LOOKUP_VALUE( TYPE )                                        \
    {                                                                   \
        TYPE * value = (TYPE*)to_cpp(value_obj, value_type_code);       \
        if (value){                                                     \
            success = LookupField < KeyType, TYPE > ::set(oid, fname, *cpp_key, *value); \
            delete value;                                               \
            delete cpp_key;                                             \
        }                                                               \
        break;                                                          \
    }

    switch (value_type_code)
    {
    case 'b':
        SET_LOOKUP_VALUE(bool)
    case 'c':
        SET_LOOKUP_VALUE(char)
    case 'h':
        SET_LOOKUP_VALUE(short)
    case 'H':
        SET_LOOKUP_VALUE(unsigned short)
    case 'i':
        SET_LOOKUP_VALUE(int)
    case 'I':
        SET_LOOKUP_VALUE(unsigned int)
    case 'l':
        SET_LOOKUP_VALUE(long)
    case 'k':
        SET_LOOKUP_VALUE(unsigned long)
#ifdef HAVE_LONG_LONG
    case 'L':
        SET_LOOKUP_VALUE(long long)
    case 'K':
        SET_LOOKUP_VALUE(unsigned long long);
#endif
    case 'd':
        SET_LOOKUP_VALUE(double)
    case 'f':
        SET_LOOKUP_VALUE(float)
    case 's':
        SET_LOOKUP_VALUE(string)
    case 'x':
        SET_LOOKUP_VALUE(Id)
    case 'y':
        SET_LOOKUP_VALUE(ObjId)
    case 'D':
        SET_LOOKUP_VALUE( vector <double> )
    case 'S':
        SET_LOOKUP_VALUE( vector <string> )
    case 'X':
        SET_LOOKUP_VALUE( vector <Id> )
    case 'Y':
        SET_LOOKUP_VALUE( vector <ObjId> )
    case 'v':
        SET_LOOKUP_VALUE( vector <int> )
    case 'M':
        SET_LOOKUP_VALUE( vector <long> )
    case 'N':
        SET_LOOKUP_VALUE( vector <unsigned int> )
    case 'P':
        SET_LOOKUP_VALUE( vector <unsigned long> )
    case 'F':
        SET_LOOKUP_VALUE( vector <float> )
    case 'w':
        SET_LOOKUP_VALUE( vector <short> )
    case 'C':
        SET_LOOKUP_VALUE( vector <char> )
    default:
        ostringstream err;
        err << "Value type " << value_type_code << " not supported yet.";
        PyErr_SetString(PyExc_TypeError, err.str().c_str());
    }
    if (success)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


template <class A>
PyObject* _set_vector_destFinfo(ObjId obj, string fieldName, PyObject * value, char vtypecode)
{
    ostringstream error;
    if (!PySequence_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "For setting vector field, specified value must be a sequence." );
        return NULL;
    }
    vector<A> * _value = (vector < A > *)to_cpp(value, vtypecode);
    if (_value == NULL)
    {
        return NULL;
    }
    bool ret = SetGet1< vector < A > >::set(obj, fieldName, *_value);
    delete _value;
    if (ret)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

/**
   Set destFinfo for 2 argument destination field functions.
*/
template <class A>
PyObject* setDestFinfo1(ObjId obj, string fieldName, PyObject* arg1, char type1, A arg2)
{
    bool ret;
    ostringstream error;
    error << "moose.setDestFinfo1: ";
    switch (type1)
    {
    case 'f':
    case 'd':
    {
        double param = PyFloat_AsDouble(arg1);
        if (type1 == 'f')
        {
            ret = SetGet2<float, A>::set(obj, fieldName, (float)param, arg2);
        }
        else
        {
            ret = SetGet2<double, A>::set(obj, fieldName, param, arg2);
        }
    }
    break;
    case 's':
    {
        char * param = PyString_AsString(arg1);
        ret = SetGet2<string, A>::set(obj, fieldName, string(param), arg2);
    }
    break;
    case 'i':
    case 'l':
    {
        long param = PyInt_AsLong(arg1);
        if (param == -1 && PyErr_Occurred())
        {
            return NULL;
        }
        if (type1 == 'i')
        {
            ret = SetGet2<int, A>::set(obj, fieldName, (int)param, arg2);
        }
        else
        {
            ret = SetGet2<long, A>::set(obj, fieldName, param, arg2);
        }
    }
    break;
    case 'I':
    case 'k':
    {
        unsigned long param =PyLong_AsUnsignedLong(arg1);
        if (PyErr_Occurred())
        {
            return NULL;
        }
        if (type1 == 'I')
        {
            ret = SetGet2< unsigned int, A>::set(obj, fieldName, (unsigned int)param, arg2);
        }
        else
        {
            ret = SetGet2<unsigned long, A>::set(obj, fieldName, param, arg2);
        }
    }
    break;
    case 'x':
    {
        Id param;
        // if (Id_SubtypeCheck(arg1)){
        _Id * id = (_Id*)(arg1);
        if (id == NULL)
        {
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
        ret = SetGet2<Id, A>::set(obj, fieldName, param, arg2);
    }
    break;
    case 'y':
    {
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
        _ObjId * oid = (_ObjId*)(arg1);
        if (oid == NULL)
        {
            error << "argument should be an vec or an melement";
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
            return NULL;
        }
        param = oid->oid_;
        // }
        ret = SetGet2<ObjId, A>::set(obj, fieldName, param, arg2);
    }
    break;
    case 'c':
    {
        char * param = PyString_AsString(arg1);
        if (!param)
        {
            error << "expected argument of type char/string";
            PyErr_SetString(PyExc_TypeError, error.str().c_str());
            return NULL;
        }
        else if (strlen(param) == 0)
        {
            error << "Empty string not allowed.";
            PyErr_SetString(PyExc_ValueError, error.str().c_str());
            return NULL;
        }
        ret = SetGet2<char, A>::set(obj, fieldName, param[0], arg2);
    }
    default:
    {
        error << "Unhandled argument 1 type (shortType=" << type1 << ")";
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return NULL;
    }
    }
    if (ret)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}



#endif // _MOOSEMODULE_H

//
// moosemodule.h ends here
