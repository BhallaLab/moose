// moosemodule.cpp ---
//
// Filename: moosemodule.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 11:26:00 2011 (+0530)
// Version:
// Last-Updated: Mon 25 Apr 2016 11:17:24 AM IST
//           By: Dilawar Singh
//

// Change log:
//
// 2011-03-10 Initial version. Starting coding directly with Python
//            API.  Trying out direct access to Python API instead of
//            going via SWIG. SWIG has this issue of creating huge
//            files and the resulting binaries are also very
//            large. Since we are not going to use any language but
//            Python in the foreseeable future, we can avoid the bloat
//            by coding directly with Python API.
//
// 2012-01-05 Much polished version. Handling destFinfos as methods in
//            Python class.
//
// 2012-04-13 Finished reimplementing the meta class system using
//            Python/C API.
//            Decided not to expose any lower level moose API.
//
// 2012-04-20 Finalized the C interface

#include <Python.h>

#include <structmember.h> // This defines the type id macros like T_STRING

#ifdef USE_NUMPY
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
#endif

#include <iostream>
#include <typeinfo>
#include <cstring>
#include <map>
#include <ctime>
#include <csignal>
#include <exception>


#if USE_BOOST
#include <boost/format.hpp>
#endif

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "../utility/print_function.hpp"
#include "../basecode/header.h"
#include "../basecode/global.h"
#include "../basecode/Id.h"
#include "../basecode/ObjId.h"
#include "../utility/utility.h"
#include "../shell/Shell.h"
#include "../shell/Wildcard.h"

#include "moosemodule.h"
#include "global.h"

using namespace std;

#ifdef DO_UNIT_TESTS

extern void testSync();
extern void testAsync();

extern void testSyncArray( 
        unsigned int size,
        unsigned int numThreads,
        unsigned int method 
        );

extern void testShell();
extern void testScheduling();
extern void testSchedulingProcess();
extern void testBuiltins();
extern void testBuiltinsProcess();

extern void testMpiScheduling();
extern void testMpiBuiltins();
extern void testMpiShell();
extern void testMsg();
extern void testMpiMsg();
extern void testKinetics();

#endif

extern void mpiTests();
extern void processTests( Shell* );
extern void nonMpiTests(Shell *);
extern void test_moosemodule();


extern Id init(
        int argc, char ** argv, bool& doUnitTests
        , bool& doRegressionTests, unsigned int& benchmark 
        );

extern void initMsgManagers();
extern void destroyMsgManagers();

extern void speedTestMultiNodeIntFireNetwork(
        unsigned int size, unsigned int runsteps 
        );

extern void mooseBenchmarks( unsigned int option );


/*-----------------------------------------------------------------------------
 *  Random number generator for this module.
 *
 *  moose.seed( int ) sets the global seed __rng_seed__ declared in global.h as
 *  extern and defined in global.cpp. Various RNGs uses this seed to initialize
 *  themselves. If seed is not set by user, it uses std::random_device to
 *  initialize itself.
 *-----------------------------------------------------------------------------*/
void pymoose_mtseed_( unsigned int seed )
{
    moose::mtseed( seed );
}

double pymoose_mtrand_( void )
{
    return moose::mtrand( );
}

/**
   Utility function to get all the individual elements when ALLDATA is dataIndex.
*/
vector<ObjId> all_elements(Id id)
{
    vector<ObjId> ret;
    unsigned int ii = 0; // storage for dataIndex
    unsigned int jj = 0; // storage for fieldIndex
    unsigned int * iptr = &ii; // this will point to the fastest changing index
    unsigned int length;
    if (id.element()->hasFields())
    {
        iptr = &jj;
        length = Field< unsigned int>::get(id, "numField");
    }
    else
    {
        length = id.element()->numData();
    }
    for (*iptr = 0; *iptr < length; ++(*iptr))
    {
        ret.push_back(ObjId(id, ii, jj));
    }
    return ret;
}


/**
 * @brief Handle signal raised by user during simulation.
 *
 * @param signum
 */
void handle_keyboard_interrupts( int signum )
{
    LOG( moose::info, "Interrupt signal (" << signum << ") received.");

    // Get the shell and cleanup.
    Shell* shell = reinterpret_cast<Shell*>(getShell(0, NULL).eref().data());
    shell->cleanSimulation();
    exit( signum );
}

// IdType and ObjIdType are defined in vec.cpp and
// melement.cpp respectively.
extern PyTypeObject IdType;
extern PyTypeObject ObjIdType;
extern PyTypeObject moose_DestField;
extern PyTypeObject moose_LookupField;
extern PyTypeObject moose_ElementField;

/////////////////////////////////////////////////////////////////
// Module globals
/////////////////////////////////////////////////////////////////
int verbosity = 1;
// static int isSingleThreaded = 0;
static int isInfinite = 0;
static unsigned int numNodes = 1;
static unsigned int numCores = 1;
static unsigned int myNode = 0;
// static unsigned int numProcessThreads = 0;
static int doUnitTests = 0;
static int doRegressionTests = 0;
static int quitFlag = 0;

/**
   Utility function to convert an Python integer or a sequence
   object into a vector of dimensions
*/
vector<int> pysequence_to_dimvec(PyObject * dims)
{
    vector <int> vec_dims;
    Py_ssize_t num_dims = 1;
    long dim_value = 1;
    if (dims)
    {
        // First try to use it as a tuple of dimensions
        if (PyTuple_Check(dims))
        {
            num_dims = PyTuple_Size(dims);
            for (Py_ssize_t ii = 0; ii < num_dims; ++ ii)
            {
                PyObject* dim = PyTuple_GetItem(dims, ii);
                dim_value = PyInt_AsLong(dim);
                if ((dim_value == -1) && PyErr_Occurred())
                {
                    return vec_dims;
                }
                vec_dims.push_back((unsigned int)dim_value);
            }
        }
        else if (PyInt_Check(dims))    // 1D array
        {
            dim_value = PyInt_AsLong(dims);
            if (dim_value <= 0)
            {
                dim_value = 1;
            }
            vec_dims.push_back(dim_value);
        }
    }
    else
    {
        vec_dims.push_back(dim_value);
    }
    return vec_dims;
}

/**
   Convert Python object into C++ data structure. The data
   structure is allocated here and it is the responsibility of the
   caller to free that memory.
*/
void * to_cpp(PyObject * object, char typecode)
{
    switch(typecode)
    {
    case 'i':
    {
        int * ret = new int();
        * ret = PyInt_AsLong(object);
        return (void*)ret;
    }
    case 'l':
    {
        long v = PyInt_AsLong(object);
        long * ret = new long();
        *ret = v;
        return (void*)ret;
    }
    case 'h':
    {
        short v = PyInt_AsLong(object);
        short * ret = new short();
        *ret = v;
        return (void*)ret;
    }
    case 'f':
    {
        float v = (float)PyFloat_AsDouble(object);
        if ( v == -1.0 && PyErr_Occurred())
        {
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of floating point numbers.");
        }
        else
        {
            float * ret = new float();
            *ret = v;
            return (void*)ret;
        }
    }
    case 'd':
    {
        double v = PyFloat_AsDouble(object);
        if ( v == -1.0 && PyErr_Occurred())
        {
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of floating point numbers.");
        }
        else
        {
            double * ret = new double();
            *ret = v;
            return (void*)ret;
        }
    }
    case 's':
    {
        char* tmp = PyString_AsString(object);
        if (tmp == NULL)
        {
            return NULL;
        }
        string * ret = new string(tmp);
        return (void*)ret;
    }
    case 'I':
    {
        unsigned int v = PyInt_AsUnsignedLongMask(object);
        unsigned int * ret = new unsigned int();
        *ret = v;
        return (void*)ret;
    }
    case 'k':
    {
        unsigned long v = PyInt_AsUnsignedLongMask(object);
        unsigned long * ret = new unsigned long();
        *ret = v;
        return (void*)ret;
    }
    case 'x':
    {
        _Id * value = (_Id*)object;
        if (value != NULL)
        {
            Id * ret = new Id();
            * ret = value->id_;
            return (void*)ret;
        }
    }
    case 'y':
    {
        _ObjId * value = (_ObjId*)object;
        if (value != NULL)
        {
            ObjId * ret = new ObjId();
            * ret = value->oid_;
            return (void*)ret;
        }
    }
    case 'v':
        return PySequenceToVector< int >(object, 'i');
    case 'N':
        return PySequenceToVector < unsigned int > (object, 'I');
    case 'w':
        return PySequenceToVector < short > (object, 'h');
    case 'M':
        return PySequenceToVector < long > (object, 'l');
    case 'P':
        return PySequenceToVector < unsigned long > (object, 'k');
    case 'F':
        return PySequenceToVector < float > (object, 'f');
    case 'D':
        return PySequenceToVector < double > (object, 'd');
    case 'S':
        return PySequenceToVector < string > (object, 's');
    case 'Y':
        return PySequenceToVector < ObjId > (object, 'y');
    case 'X':
        return PySequenceToVector < Id > (object, 'x');
    case 'R':
        return PySequenceToVectorOfVectors< double >(object, 'd');
    case 'Q':
        return PySequenceToVectorOfVectors< int >(object, 'i');
    case 'T':
        return PySequenceToVectorOfVectors< unsigned int > (object, 'I');
    }
    return NULL;
}


/**
   Utility function to convert C++ object into Python object.
*/
PyObject * to_py(void * obj, char typecode)
{
    switch(typecode)
    {
    case 'd':
    {
        double * ptr = static_cast<double *>(obj);
        assert(ptr != NULL);
        return PyFloat_FromDouble(*ptr);
    }
    case 's':   // handle only C++ string and NOT C char array because static cast cannot differentiate the two
    {
        string * ptr = static_cast< string * >(obj);
        assert (ptr != NULL);
        return PyString_FromString(ptr->c_str());
    }
    case 'x':
    {
        Id * ptr = static_cast< Id * >(obj);
        assert(ptr != NULL);
        _Id * id = PyObject_New(_Id, &IdType);
        id->id_ = *ptr;
        return (PyObject *)(id);
    }
    case 'y':
    {
        ObjId * ptr = static_cast< ObjId * >(obj);
        assert(ptr != NULL);
        _ObjId * oid = PyObject_New(_ObjId, &ObjIdType);
        oid->oid_ = *ptr;
        return (PyObject*)oid;
    }
    case 'l':
    {
        long * ptr = static_cast< long * >(obj);
        assert(ptr != NULL);
        return PyLong_FromLong(*ptr);
    }
    case 'k':
    {
        unsigned long * ptr = static_cast< unsigned long * >(obj);
        assert(ptr != NULL);
        return PyLong_FromUnsignedLong(*ptr);
    }
    case 'i':  // integer
    {
        int * ptr = static_cast< int * >(obj);
        assert(ptr != NULL);
        return PyInt_FromLong(*ptr);
    }
    case 'I':   // unsigned int
    {
        unsigned int * ptr = static_cast< unsigned int * >(obj);
        assert(ptr != NULL);
        return PyLong_FromUnsignedLong(*ptr);
    }
    case 'b':  //bool
    {
        bool * ptr = static_cast< bool * >(obj);
        assert(ptr != NULL);
        if (*ptr)
        {
            Py_RETURN_TRUE;
        }
        else
        {
            Py_RETURN_FALSE;
        }
    }
#ifdef HAVE_LONG_LONG
    case 'L':   //long long
    {
        long long * ptr = static_cast< long long * > (obj);
        assert(ptr != NULL);
        return PyLong_FromLongLong(*ptr);
    }
    case 'K':   // unsigned long long
    {
        unsigned long long * ptr = static_cast< unsigned long long * >(obj);
        assert(ptr != NULL);
        return PyLong_FromUnsignedLongLong(*ptr);
    }
#endif // HAVE_LONG_LONG
    case 'f':   // float
    {
        float * ptr = static_cast< float* >(obj);
        assert(ptr != NULL);
        return PyFloat_FromDouble(*ptr);
    }
    case 'c':   // char
    {
        char * ptr = static_cast< char * >(obj);
        assert(ptr != NULL);
        return Py_BuildValue("c", *ptr);
    }
    case 'h':   //short
    {
        short * ptr = static_cast< short* >(obj);
        assert(ptr != NULL);
        return Py_BuildValue("h", *ptr);
    }
    case 'H':   // unsigned short
    {
        unsigned short * ptr = static_cast< unsigned short * >(obj);
        assert(ptr != NULL);
        return Py_BuildValue("H", *ptr);
    }
    case 'D':
    case 'v':
    case 'M':
    case 'X':
    case 'Y':
    case 'C':
    case 'w':
    case 'N':
    case 'P':
    case 'F':
    case 'S':
    case 'T':
    case 'Q':
    case 'R':
    {
        return to_pytuple(obj, innerType(typecode));
    }
    default:
    {
        PyErr_SetString(PyExc_TypeError, "unhandled data type");
        return NULL;
    }
    } // switch(typecode)
} // to_py

/**
   Inner function to convert C++ object at vptr and set tuple
   entry ii to the created PyObject. typecode is passed to to_cpp
   for conversion.  If error occurs while setting tuple antry, it
   decrements the refcount of the tuple and returns NULL. Returns
   the tuple on success.
*/
PyObject * convert_and_set_tuple_entry(PyObject * tuple, unsigned int index, void * vptr, char typecode)
{
    PyObject * item = to_py(vptr, typecode);
    if (item == NULL)
    {
        return NULL; // the error message would have been populated by to_cpp
    }
    if (PyTuple_SetItem(tuple, (Py_ssize_t)index, item) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "convert_and_set_tuple_entry: could not set tuple entry.");
        return NULL;
    }
#ifndef NDEBUG
    if (verbosity > 2)
    {
        cout << "Set tuple " << tuple << " entry " << index << " with " << item << endl;
    }
#endif
    return tuple;
}

/**
   Convert a C++ vector to Python tuple
*/
PyObject * to_pytuple(void * obj, char typecode)
{
    PyObject * ret;
    switch (typecode)
    {
    case 'd':   // vector<double>
    {
        vector< double > * vec = static_cast< vector < double >* >(obj);
        assert(vec != NULL);
#ifndef USE_NUMPY
        ret = PyTuple_New((Py_ssize_t)vec->size());
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyFloat_FromDouble(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_DOUBLE);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], vec->size() * sizeof(double));
#endif
        return ret;
    }
    case 'i':   // vector<int>
    {
        vector< int > * vec = static_cast< vector < int >* >(obj);
        assert(vec != NULL);
#ifndef USE_NUMPY
        ret = PyTuple_New((Py_ssize_t)vec->size());
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyInt_FromLong(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_INT);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], size * sizeof(int));
#endif
        return ret;
    }
    case 'I':   // vector<unsigned int>
    {
        vector< int > * vec = static_cast< vector < int >* >(obj);
        assert(vec != NULL);
#ifndef USE_NUMPY
        ret = PyTuple_New((Py_ssize_t)vec->size());
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyLong_FromLong(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_UINT);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], size * sizeof(unsigned int));
#endif
        return ret;
    }
    case 'l':   // vector<long>
    {
        vector< long > * vec = static_cast< vector < long >* >(obj);
        assert(vec != NULL);
#ifndef USE_NUMPY
        ret = PyTuple_New((Py_ssize_t)vec->size());
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyLong_FromLong(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_INT);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], size * sizeof(long));
#endif
        return ret;
    }
    case 'x':   // vector<Id>
    {
        vector< Id > * vec = static_cast< vector < Id >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (convert_and_set_tuple_entry(ret, ii, (void*)&vec->at(ii), typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    case 'y':   // vector<ObjId>
    {
        vector< ObjId > * vec = static_cast< vector < ObjId >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (convert_and_set_tuple_entry(ret, ii, (void*)&vec->at(ii), typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    case 'c':   // vector<char>
    {
        vector< char > * vec = static_cast< vector < char >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (convert_and_set_tuple_entry(ret, ii, (void*)&vec->at(ii), typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    case 'h':   // vector<short>
    {
        vector< short > * vec = static_cast< vector < short >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (convert_and_set_tuple_entry(ret, ii, (void*)&vec->at(ii), typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    case 'k':   // vector<unsigned long>
    {
        vector< unsigned int > * vec = static_cast< vector < unsigned int >* >(obj);
        assert(vec != NULL);
#ifndef USE_NUMPY
        ret = PyTuple_New((Py_ssize_t)vec->size());
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyLong_FromUnsignedLong(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_UINT);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], size * sizeof(unsigned int));
#endif
        return ret;
    }
    case 'L':   // vector<long long> - this is not used at present
    {
        vector< long long> * vec = static_cast< vector < long long>* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
#ifndef USE_NUMPY
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyLong_FromLongLong(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_LONGLONG);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], size * sizeof(long long));
#endif
        return ret;
    }
    case 'K':   // vector<unsigned long long> - this is not used at present
    {
        vector< unsigned long long> * vec = static_cast< vector < unsigned long long>* >(obj);
        assert(vec != NULL);
#ifndef USE_NUMPY
        ret = PyTuple_New((Py_ssize_t)vec->size());
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyLong_FromUnsignedLongLong(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_ULONGLONG);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], size * sizeof(unsigned long long));
#endif
        return ret;
    }
    case 'F':   // vector<float>
    {
        vector< float > * vec = static_cast< vector < float >* >(obj);
        assert(vec != NULL);
#ifndef USE_NUMPY
        ret = PyTuple_New((Py_ssize_t)vec->size());
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (0 != PyTuple_SetItem(ret, ii, PyFloat_FromDouble(vec->at(ii))))
            {
                Py_DECREF(ret);
                return NULL;
            }
        }
#else
        npy_intp size = (npy_intp)(vec->size());
        ret = PyArray_SimpleNew(1, &size, NPY_FLOAT);
        assert(ret != NULL);
        char * ptr = PyArray_BYTES((PyArrayObject*)ret);
        memcpy(ptr, &(*vec)[0], size * sizeof(float));
#endif
        return ret;
    }
    case 's':   // vector<string>
    {
        vector< string > * vec = static_cast< vector < string >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            string v = vec->at(ii);
            if (convert_and_set_tuple_entry(ret, ii, (void*)&v, typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    case 'N':   // vector< vector <unsigned int > >
    {
        vector< vector< unsigned int > > * vec = static_cast< vector < vector< unsigned int > >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (convert_and_set_tuple_entry(ret, ii, (void*)&vec->at(ii), typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    case 'v':   // vector< vector < int > >
    {
        vector< vector< int > > * vec = static_cast< vector < vector< int > >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (convert_and_set_tuple_entry(ret, ii, (void*)&vec->at(ii), typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    case 'D':   // vector< vector <double > >
    {
        vector< vector< double > > * vec = static_cast< vector < vector< double > >* >(obj);
        assert(vec != NULL);
        ret = PyTuple_New((Py_ssize_t)vec->size());
        assert(ret != NULL);
        for (unsigned int ii = 0; ii < vec->size(); ++ii)
        {
            if (convert_and_set_tuple_entry(ret, ii, (void*)&vec->at(ii), typecode) == NULL)
            {
                return NULL;
            }
        }
        break;
    }
    default:
        PyErr_SetString(PyExc_TypeError, "unhandled type");
        return NULL;
    }
    return ret;
}

// Global store of defined MOOSE classes.
map<string, PyTypeObject *>& get_moose_classes()
{
    static map<string, PyTypeObject *> defined_classes;
    return defined_classes;
}

// Global storage for PyGetSetDef structs for LookupFields.
map<string, vector <PyGetSetDef> >& get_getsetdefs()
{
    static map<string, vector <PyGetSetDef>  > getset_defs;
    return getset_defs;
}

/**
   map of fields which are aliased in Python to avoid collision
   with Python keywords.
*/
const map<string, string>& get_field_alias()
{
    static map<string, string> alias;
    if (alias.empty())
    {
        // alias["class_"] = "class";
        alias["lambda_"] = "lambda";
    }
    return alias;
}


// Minimum number of arguments for setting destFinfo - 1-st
// the finfo name.
// Py_ssize_t minArgs = 1;

// // Arbitrarily setting maximum on variable argument list. Read:
// // http://www.swig.org/Doc1.3/Varargs.html to understand why
// Py_ssize_t maxArgs = 10;


///////////////////////////////////////////////////////////////////////////
// Helper routines
///////////////////////////////////////////////////////////////////////////


/**
   Get the environment variables and set runtime environment based
   on them.
*/
vector <string> setup_runtime_env()
{
    const map<string, string>& argmap = getArgMap();
    vector<string> args;
    args.push_back("moose");
    map<string, string>::const_iterator it;
    // it = argmap.find("SINGLETHREADED");
    // if (it != argmap.end()){
    //     istringstream(it->second) >> isSingleThreaded;
    //     if (isSingleThreaded){
    //         args.push_back("-s");
    //     }
    // }
    it = argmap.find("INFINITE");
    if (it != argmap.end())
    {
        istringstream(it->second) >> isInfinite;
        if (isInfinite)
        {
            args.push_back("-i");
        }
    }
    it = argmap.find("NUMNODES");
    if (it != argmap.end())
    {
        istringstream(it->second) >> numNodes;
        args.push_back("-n");
        args.push_back(it->second);
    }
    it = argmap.find("NUMCORES");
    if (it != argmap.end())
    {
        istringstream(it->second) >> numCores;
    }
    // it = argmap.find("NUMPTHREADS");
    // if (it != argmap.end()){
    //     istringstream(it->second) >> numProcessThreads;
    //     args.push_back("-t");
    //     args.push_back(it->second);
    // }
    it = argmap.find("QUIT");
    if (it != argmap.end())
    {
        istringstream(it->second) >> quitFlag;
        if (quitFlag)
        {
            args.push_back("-q");
        }
    }
    it = argmap.find("VERBOSITY");
    if (it != argmap.end())
    {
        istringstream(it->second) >> verbosity;
    }
    it = argmap.find("DOUNITTESTS");
    if (it != argmap.end())
    {
        istringstream(it->second) >> doUnitTests;
    }
    it = argmap.find("DOREGRESSIONTESTS");
    if (it != argmap.end())
    {
        istringstream(it->second) >> doRegressionTests;
    }

    if (verbosity > 0)
    {
        cout << "ENVIRONMENT: " << endl
             << "----------------------------------------" << endl
             // << "   SINGLETHREADED = " << isSingleThreaded << endl
             << "   INFINITE = " << isInfinite << endl
             << "   NUMNODES = " << numNodes << endl
             // << "   NUMPTHREADS = " << numProcessThreads << endl
             << "   VERBOSITY = " << verbosity << endl
             << "   DOUNITTESTS = " << doUnitTests << endl
             << "   DOREGRESSIONTESTS = " << doRegressionTests << endl
             << "========================================" << endl;
    }
    return args;
} //! setup_runtime_env()

/**
   Create the shell instance unless already created. This calls
   basecode/main.cpp:init(argc, argv) to do the initialization.

   Return the Id of the Shell object.
*/
Id getShell(int argc, char ** argv)
{
    static int inited = 0;
    if (inited)
    {
        return Id(0);
    }
    bool dounit = doUnitTests != 0;
    bool doregress = doRegressionTests != 0;
    unsigned int doBenchmark = 0;
    // Utilize the main::init function which has friend access to Id
    Id shellId = init(argc, argv, dounit, doregress, doBenchmark );
    inited = 1;
    Shell * shellPtr = reinterpret_cast<Shell*>(shellId.eref().data());
    if (dounit)
    {
        nonMpiTests( shellPtr ); // These tests do not need the process loop.
    }
    if ( shellPtr->myNode() == 0 )
    {
        if ( Shell::numNodes() > 1 )
        {
            // Use the last clock for the postmaster, so that it is called
            // after everything else has been processed and all messages
            // are ready to send out.
            shellPtr->doUseClock( "/postmaster", "process", 9 );
            shellPtr->doSetClock( 9, 1.0 ); // Use a sensible default.
        }
#ifdef DO_UNIT_TESTS
        if ( dounit )
        {
            mpiTests();
            processTests( shellPtr );
        }
        // if ( doRegressionTests ) regressionTests();
#endif
        // These are outside unit tests because they happen in optimized
        // mode, using a command-line argument. As soon as they are done
        // the system quits, in order to estimate timing.
        if ( doBenchmark != 0 )
        {
            mooseBenchmarks( doBenchmark );
        }
    }
    return shellId;
} //! create_shell()

/**
   Clean up after yourself.
*/
void finalize()
{
    static bool finalized = false;
    if (finalized)
    {
        return;
    }
    finalized = true;
    Id shellId = getShell(0, NULL);
    // Clear the memory for PyGetSetDefs. The key
    // (name) was dynamically allocated using calloc. So was the
    // docstring.
    for (map<string, vector<PyGetSetDef> >::iterator it =
                get_getsetdefs().begin();
            it != get_getsetdefs().end();
            ++it)
    {
        vector <PyGetSetDef> &getsets = it->second;
        // for (unsigned int ii = 0; ii < getsets.size()-1; ++ii){ // the -1 is for the empty sentinel entry
        //     free(getsets[ii].name);
        //     Py_XDECREF(getsets[ii].closure);
        // }
    }
    get_getsetdefs().clear();
    // deallocate the class names calloc-ed at initialization.
    // for(map< string, PyTypeObject* >::iterator it = get_moose_classes().begin();
    //     it != get_moose_classes().end(); ++it){
    //     PyTypeObject * classObject = it->second;
    //     free(classObject->tp_name); // skipping this as not sure whether this is useful - all gets deallocated at exit anyways.
    // }
    // get_moose_classes().clear();
    SHELLPTR->doQuit();
    Msg::clearAllMsgs();
    Id::clearAllElements();
#ifdef USE_MPI
    MPI_Finalize();
#endif
} //! finalize()


/**
   Return list of available Finfo types.
   Place holder for static const to avoid static initialization issues.
*/
const char ** getFinfoTypes()
{
    static const char * finfoTypes[] = {"valueFinfo",
                                        "srcFinfo",
                                        "destFinfo",
                                        "lookupFinfo",
                                        "sharedFinfo",
                                        "fieldElementFinfo",
                                        0
                                       };
    return finfoTypes;
}

/**
   get the field type for specified field

   Argument:
   className -- class to look in

   fieldName -- field to look for

   Return:

   string -- value of type field of the Finfo object. This is a
   comma separated list of C++ template arguments
*/
string getFieldType(string className, string fieldName)
{
    string fieldType = "";
    const Cinfo * cinfo = Cinfo::find(className);
    if (cinfo == NULL)
    {
        if (verbosity > 0)
        {
            cerr << "Unknown class " << className << endl;
        }
        return fieldType;
    }
    const Finfo * finfo = cinfo->findFinfo(fieldName);
    if (finfo == NULL)
    {
        if (verbosity > 0)
        {
            cerr << "Unknown field " << fieldName << endl;
        }
        return fieldType;
    }
    fieldType = finfo->rttiType();
    return fieldType;
}

/**
   Parse the type field of Finfo objects.

   The types field is a comma separated list of the template
   arguments. We populate `typeVec` with the individual
   type strings.
*/
int parseFinfoType(string className, string finfoType, string fieldName, vector<string> & typeVec)
{
    string typestring = getFieldType(className, fieldName);
    if (typestring.empty())
    {
        return -1;
    }
    moose::tokenize(typestring, ",", typeVec);
    if ((int)typeVec.size() > maxArgs)
    {
        return -1;
    }
    for (unsigned int ii = 0; ii < typeVec.size() ; ++ii)
    {
        char type_code = shortType(typeVec[ii]);
        if (type_code == 0)
        {
            return -1;
        }
    }
    return 0;
}

/**
   Return a vector of field names of specified finfo type.
*/
vector<string> getFieldNames(string className, string finfoType)
{
    vector <string> ret;
    const Cinfo * cinfo = Cinfo::find(className);
    if (cinfo == NULL)
    {
        cerr << "Invalid class name." << endl;
        return ret;
    }
    // Finfo * (Cinfo:: * getFinfo)(unsigned int); // causes : warning: ISO C++ forbids taking the address of a bound member function to form a pointer to member function.
    if (finfoType == "valueFinfo" || finfoType == "value")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumValueFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getValueFinfo(ii);
            ret.push_back(finfo->name());
        }
    }
    else if (finfoType == "srcFinfo" || finfoType == "src")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumSrcFinfo();  ++ii)
        {
            Finfo * finfo = cinfo->getSrcFinfo(ii);
            ret.push_back(finfo->name());
        }
    }
    else if (finfoType == "destFinfo" || finfoType == "dest")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumDestFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getDestFinfo(ii);
            ret.push_back(finfo->name());
        }
    }
    else if (finfoType == "lookupFinfo" || finfoType == "lookup")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumLookupFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getLookupFinfo(ii);
            ret.push_back(finfo->name());
        }
    }
    else if (finfoType == "sharedFinfo" || finfoType == "shared")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumSrcFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getSrcFinfo(ii);
            ret.push_back(finfo->name());
        }
    }
    else if (finfoType == "fieldElementFinfo" || finfoType == "fieldElement")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumFieldElementFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getFieldElementFinfo(ii);
            ret.push_back(finfo->name());
        }
    }
    return ret;
}

/**
   Populate the `fieldNames` vector with names of the fields of
   `finfoType` in the specified class.

   Populate the `fieldTypes` vector with corresponding C++ data
   type string (Finfo.type).
*/
int getFieldDict(string className, string finfoType,
                 vector<string>& fieldNames, vector<string>&fieldTypes)
{
    const Cinfo * cinfo = Cinfo::find(className);
    if (cinfo == NULL)
    {
        cerr << "Invalid class." << endl;
        return 0;
    }
    if (finfoType == "valueFinfo" || finfoType == "value")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumValueFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getValueFinfo(ii);
            fieldNames.push_back(finfo->name());
            fieldTypes.push_back(finfo->rttiType());
        }
    }
    else if (finfoType == "srcFinfo" || finfoType == "src")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumSrcFinfo();  ++ii)
        {
            Finfo * finfo = cinfo->getSrcFinfo(ii);
            fieldNames.push_back(finfo->name());
            fieldTypes.push_back(finfo->rttiType());
        }
    }
    else if (finfoType == "destFinfo" || finfoType == "dest")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumDestFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getDestFinfo(ii);
            fieldNames.push_back(finfo->name());
            fieldTypes.push_back(finfo->rttiType());
        }
    }
    else if (finfoType == "lookupFinfo" || finfoType == "lookup")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumLookupFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getLookupFinfo(ii);
            fieldNames.push_back(finfo->name());
            fieldTypes.push_back(finfo->rttiType());
        }
    }
    else if (finfoType == "sharedFinfo" || finfoType == "shared")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumSrcFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getSrcFinfo(ii);
            fieldNames.push_back(finfo->name());
            fieldTypes.push_back(finfo->rttiType());
        }
    }
    else if (finfoType == "fieldElementFinfo" || finfoType == "field" || finfoType == "fieldElement")
    {
        for (unsigned int ii = 0; ii < cinfo->getNumFieldElementFinfo(); ++ii)
        {
            Finfo * finfo = cinfo->getFieldElementFinfo(ii);
            fieldNames.push_back(finfo->name());
            fieldTypes.push_back(finfo->rttiType());
        }
    }
    return 1;
}

//////////////////////////
// Field Type definition
//////////////////////////
/////////////////////////////////////////////////////
// ObjId functions.
/////////////////////////////////////////////////////


/**
   Utility function to traverse python class hierarchy to reach closest base class.
   Ideally we should go via mro
*/
PyTypeObject * getBaseClass(PyObject * self)
{
    extern PyTypeObject ObjIdType;
    string basetype_str = "";
    PyTypeObject * base = NULL;
    for (base = Py_TYPE(self);
            base != &ObjIdType; base = base->tp_base)
    {
        basetype_str = base->tp_name;
        size_t dot = basetype_str.find('.');
        basetype_str = basetype_str.substr(dot+1);
        if (get_moose_classes().find(basetype_str) !=
                get_moose_classes().end())
        {
            return base;
        }
    }
    return NULL;
}

////////////////////////////////////////////
// Module functions
////////////////////////////////////////////
PyDoc_STRVAR(moose_getFieldNames_documentation,
             "getFieldNames(className, finfoType='valueFinfo') -> tuple\n"
             "\n"
             "Get a tuple containing the name of all the fields of `finfoType`\n"
             "kind.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "className : string\n"
             "    Name of the class to look up.\n"
             "finfoType : string\n"
             "    The kind of field (`valueFinfo`, `srcFinfo`, `destFinfo`,\n"
             "    `lookupFinfo`, `fieldElementFinfo`.).\n"
             "\n"
             "Returns\n"
             "-------\n"
             "tuple\n"
             "    Names of the fields of type `finfoType` in class `className`.\n"
            );

PyObject * moose_getFieldNames(PyObject * dummy, PyObject * args)
{
    char * className = NULL;
    char _finfoType[] = "valueFinfo";
    char * finfoType = _finfoType;
    if (!PyArg_ParseTuple(args, "s|s", &className, &finfoType))
    {
        return NULL;
    }
    vector <string> fieldNames = getFieldNames(className, finfoType);
    PyObject * ret = PyTuple_New(fieldNames.size());

    for (unsigned int ii = 0; ii < fieldNames.size(); ++ii)
    {
        if (PyTuple_SetItem(ret, ii, PyString_FromString(fieldNames[ii].c_str())) == -1)
        {
            Py_XDECREF(ret);
            return NULL;
        }
    }
    return ret;
}

PyDoc_STRVAR(moose_copy_documentation,
             "copy(src, dest, name, n, toGlobal, copyExtMsg) -> bool\n"
             "\n"
             "Make copies of a moose object.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "src : vec, element or str\n"
             "    source object.\n"
             "dest : vec, element or str\n"
             "    Destination object to copy into.\n"
             "name : str\n"
             "    Name of the new object. If omitted, name of the original will be used.\n"
             "n : int\n"
             "    Number of copies to make.\n"
             "toGlobal : int\n"
             "    Relevant for parallel environments only. If false, the copies will\n"
             "    reside on local node, otherwise all nodes get the copies.\n"
             "copyExtMsg : int\n"
             "    If true, messages to/from external objects are also copied.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "vec\n"
             "    newly copied vec\n"
            );
PyObject * moose_copy(PyObject * dummy, PyObject * args, PyObject * kwargs)
{
    PyObject * src = NULL, * dest = NULL;
    char * newName = NULL;
    static const char * kwlist[] = {"src", "dest", "name", "n", "toGlobal", "copyExtMsg", NULL};
    unsigned int num=1, toGlobal=0, copyExtMsgs=0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|sIII", const_cast<char**>(kwlist), &src, &dest, &newName, &num, &toGlobal, &copyExtMsgs))
    {
        return NULL;
    }
    Id _src;
    ObjId _dest;
    if (PyObject_IsInstance(src, (PyObject*)&IdType))
    {
        _src = ((_Id*)src)->id_;
    }
    else if (PyObject_IsInstance(src, (PyObject*)&ObjIdType))
    {
        _src = ((_ObjId*)src)->oid_.id;
    }
    else if (PyString_Check(src))
    {
        _src = Id(PyString_AsString(src));
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Source must be instance of vec, element or string.");
        return NULL;
    }
    if (_src == Id())
    {
        PyErr_SetString(PyExc_ValueError, "Cannot make copy of moose shell.");
        return NULL;
    }
    if (PyObject_IsInstance(dest, (PyObject*)&IdType))
    {
        _dest = ObjId(((_Id*)dest)->id_);
    }
    else if (PyObject_IsInstance(dest, (PyObject*)&ObjIdType))
    {
        _dest = ((_ObjId*)dest)->oid_;
    }
    else if (PyString_Check(dest))
    {
        _dest = ObjId(PyString_AsString(dest));
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "destination must be instance of vec, element or string.");
        return NULL;
    }
    if (!Id::isValid(_src))
    {
        RAISE_INVALID_ID(NULL, "moose_copy: invalid source Id.");
    }
    else if (_dest.bad())
    {
        RAISE_INVALID_ID(NULL, "moose_copy: invalid destination.");
    }
    string name;
    if (newName == NULL)
    {
        // Use the original name if name is not specified.
        name = Field<string>::get(ObjId(_src, 0), "name");
    }
    else
    {
        name = string(newName);
    }
    _Id * tgt = PyObject_New(_Id, &IdType);
    tgt->id_ = SHELLPTR->doCopy(_src, _dest, name, num, toGlobal, copyExtMsgs);
    PyObject * ret = (PyObject*)tgt;
    return ret;
}

// Not sure what this function should return... ideally the Id of the
// moved object - does it change though?
PyObject * moose_move(PyObject * dummy, PyObject * args)
{
    PyObject * src, * dest;
    if (!PyArg_ParseTuple(args, "OO:moose_move", &src, &dest))
    {
        return NULL;
    }
    Id _src;
    ObjId _dest;
    if (PyObject_IsInstance(src, (PyObject*)&IdType))
    {
        _src = ((_Id*)src)->id_;
    }
    else if (PyObject_IsInstance(src, (PyObject*)&ObjIdType))
    {
        _src = ((_ObjId*)src)->oid_.id;
    }
    else if (PyString_Check(src))
    {
        _src = Id(PyString_AsString(src));
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Source must be instance of vec, element or string.");
        return NULL;
    }
    if (_src == Id())
    {
        PyErr_SetString(PyExc_ValueError, "Cannot make move moose shell.");
        return NULL;
    }
    if (PyObject_IsInstance(dest, (PyObject*)&IdType))
    {
        _dest = ObjId(((_Id*)dest)->id_);
    }
    else if (PyObject_IsInstance(dest, (PyObject*)&ObjIdType))
    {
        _dest = ((_ObjId*)dest)->oid_;
    }
    else if (PyString_Check(dest))
    {
        _dest = ObjId(PyString_AsString(dest));
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "destination must be instance of vec, element or string.");
        return NULL;
    }
    if (!Id::isValid(_src))
    {
        RAISE_INVALID_ID(NULL, "moose_copy: invalid source Id.");
    }
    else if (_dest.bad())
    {
        RAISE_INVALID_ID(NULL, "moose_copy: invalid destination.");
    }
    SHELLPTR->doMove(_src, _dest);
    Py_RETURN_NONE;
}

PyDoc_STRVAR(moose_delete_documentation,
             "delete(obj)->None\n"
             "\n"
             "Delete the underlying moose object. This does not delete any of the\n"
             "Python objects referring to this vec but does invalidate them. Any\n"
             "attempt to access them will raise a ValueError.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "id : vec\n"
             "    vec of the object to be deleted.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "None\n"
            );
PyObject * moose_delete(PyObject * dummy, PyObject * args)
{
    PyObject * obj;
    bool isId_ = false;
    bool isObjId_ = false;
    if (!PyArg_ParseTuple(args, "O:moose.delete", &obj))
    {
        return NULL;
    }
    // if (!PyObject_IsInstance(obj, (PyObject*)&IdType)){
    //     PyErr_SetString(PyExc_TypeError, "vec instance expected");
    //     return NULL;
    // }
    ObjId oid_;
    if (PyObject_IsInstance(obj, (PyObject*)&IdType))
    {
        oid_ = ((_Id*)obj)->id_;
        isId_ = true;
    }
    else if (PyObject_IsInstance(obj, (PyObject*)&ObjIdType))
    {
        oid_ = ((_ObjId*)obj)->oid_;
        isObjId_ = true;
    }
    else if (PyString_Check(obj))
    {
        oid_ = ObjId(PyString_AsString(obj));
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "cannot delete moose shell.");
        return NULL;
    }
    if (oid_ == ObjId())
    {
        PyErr_SetString(PyExc_ValueError, "cannot delete moose shell.");
        return NULL;
    }
    if ( oid_.bad() )
    {
        RAISE_INVALID_ID(NULL, "moose_delete");
    }
    deleteObjId(oid_);
    if (isId_)
    {
        ((_Id*)obj)->id_ = Id();
    }
    if (isObjId_)
    {
        ((_ObjId*)obj)->oid_ = ObjId(0, BADINDEX, BADINDEX);
    }
    // SHELLPTR->doDelete(((_Id*)obj)->id_);
    Py_RETURN_NONE;
}

PyDoc_STRVAR(moose_useClock_documentation,
             "useClock(tick, path, fn)\n\n"
             "schedule `fn` function of every object that matches `path` on tick no. `tick`.\n\n "
             "Most commonly the function is 'process'.  NOTE: unlike earlier versions, now autoschedule is not available. You have to call useClock for every element that should be updated during the  simulation.\n\n "
             "The sequence of clockticks with the same dt is according to their number. This is utilized for controlling the order of updates in various objects where it matters. The following convention should be observed when assigning clockticks to various components of a model:\n\n "
             "Clock ticks 0-3 are for electrical (biophysical) components, 4 and 5 are for chemical kinetics, 6 and 7 are for lookup tables and stimulus, 8 and 9 are for recording tables.\n\n "
             "Generally, `process` is the method to be assigned a clock tick. Notable exception is `init` method of Compartment class which is assigned tick 0.\n\n"
             "     - 0 : Compartment: `init`\n"
             "     - 1 : Compartment: `process`\n"
             "     - 2 : HHChannel and other channels: `process`\n"
             "     - 3 : CaConc : `process`\n"
             "     - 4,5 : Elements for chemical kinetics : `process`\n"
             "     - 6,7 : Lookup (tables), stimulus : `process`\n"
             "     - 8,9 : Tables for plotting : `process`\n"
             " \n"
             "Parameters\n"
             "----------\n"
             "tick : int\n"
             "        tick number on which the targets should be scheduled.\n"
             "path : str\n"
             "        path of the target element(s). This can be a wildcard also.\n"
             "fn : str\n"
             "        name of the function to be called on each tick. Commonly `process`.\n"
             "\n"
             "Examples\n"
             "--------\n"
             "In multi-compartmental neuron model a compartment's membrane potential (Vm) is dependent on its neighbours' membrane potential. Thus it must get the neighbour's present Vm before computing its own Vm in next time step. This ordering is achieved by scheduling the `init` function, which communicates membrane potential, on tick 0 and `process` function on tick 1.\n\n"
             "    >>> moose.useClock(0, '/model/compartment_1', 'init')\n"
             "    >>> moose.useClock(1, '/model/compartment_1', 'process')\n");

PyObject * moose_useClock(PyObject * dummy, PyObject * args)
{
    char * path, * field;
    unsigned int tick;
    if(!PyArg_ParseTuple(args, "Iss:moose_useClock", &tick, &path, &field))
    {
        return NULL;
    }
    SHELLPTR->doUseClock(string(path), string(field), tick);
    Py_RETURN_NONE;
}


PyDoc_STRVAR(moose_setClock_documentation,
             "setClock(tick, dt)\n"
             "\n"
             "set the ticking interval of `tick` to `dt`.\n"
             "\n"
             "A tick with interval `dt` will call the functions scheduled on that tick every `dt` timestep.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    tick : int\n"
             "        tick number\n"
             "    dt : double\n"
             "        ticking interval\n");

PyObject * moose_setClock(PyObject * dummy, PyObject * args)
{
    unsigned int tick;
    double dt;
    if(!PyArg_ParseTuple(args, "Id:moose_setClock", &tick, &dt))
    {
        return NULL;
    }
    if (dt < 0)
    {
        PyErr_SetString(PyExc_ValueError, "dt must be positive.");
        return NULL;
    }
    SHELLPTR->doSetClock(tick, dt);
    Py_RETURN_NONE;
}

PyDoc_STRVAR(moose_start_documentation,
             "start(time, notify = False) -> None\n"
             "\n"
             "Run simulation for `t` time. Advances the simulator clock by `t`\n"
             "time. If 'notify = True', a message is written to terminal whenever \n"
             "10\% of simulation time is over. \n"
             "\n"
             "After setting up a simulation, YOU MUST CALL MOOSE.REINIT() before\n"
             "CALLING MOOSE.START() TO EXECUTE THE SIMULATION. Otherwise, the\n"
             "simulator behaviour will be undefined. Once moose.reinit() has been\n"
             "called, you can call moose.start(t) as many time as you like. This\n"
             "will continue the simulation from the last state for `t` time.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "t : float\n"
             "    duration of simulation.\n"
             "notify: bool\n"
             "     default False. If True, notify user whenever 10\% of simultion \n"
             "     is over.\n"
             "\n"
             "Returns\n"
             "--------\n"
             "    None\n"
             "\n"
             "See also\n"
             "--------\n"
             "moose.reinit : (Re)initialize simulation\n"
             "\n"
            );
PyObject * moose_start(PyObject * dummy, PyObject * args )
{
    double runtime = 0.0;
    bool notify = false;

    PyArg_ParseTuple(args, "d|I:moose_start", &runtime, &notify);

    if (runtime <= 0.0)
    {
        PyErr_SetString(PyExc_ValueError, "simulation runtime must be positive.");
        return NULL;
    }

    // This is from http://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event-c
    struct sigaction sigHandler;
    sigHandler.sa_handler = handle_keyboard_interrupts;
    sigemptyset(&sigHandler.sa_mask);
    sigHandler.sa_flags = 0;
    sigaction(SIGINT, &sigHandler, NULL);

#if 0
    // NOTE: (dilawar) Does not know if Py_BEGIN_ALLOW_THREADS is
    // neccessary.
    // Py_BEGIN_ALLOW_THREADS
    SHELLPTR->doStart(runtime);
    // Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
#endif
    SHELLPTR->doStart( runtime, notify );
    Py_RETURN_NONE;

}

PyDoc_STRVAR(moose_reinit_documentation,
             "reinit() -> None\n"
             "\n"
             "Reinitialize simulation.\n"
             "\n"
             "This function (re)initializes moose simulation. It must be called\n"
             "before you start the simulation (see moose.start). If you want to\n"
             "continue simulation after you have called moose.reinit() and\n"
             "moose.start(), you must NOT call moose.reinit() again. Calling\n"
             "moose.reinit() again will take the system back to initial setting\n"
             "(like clear out all data recording tables, set state variables to\n"
             "their initial values, etc.\n"
             "\n");
PyObject * moose_reinit(PyObject * dummy, PyObject * args)
{
    SHELLPTR->doReinit();
    Py_RETURN_NONE;
}
PyObject * moose_stop(PyObject * dummy, PyObject * args)
{
    SHELLPTR->doStop();
    Py_RETURN_NONE;
}
PyObject * moose_isRunning(PyObject * dummy, PyObject * args)
{
    return Py_BuildValue("i", SHELLPTR->isRunning());
}

PyObject * moose_exists(PyObject * dummy, PyObject * args)
{
    char * path;
    if (!PyArg_ParseTuple(args, "s", &path))
    {
        return NULL;
    }
    return Py_BuildValue("i", Id(path) != Id() || string(path) == "/" || string(path) == "/root");
}

//Harsha : For writing genesis file to sbml
PyObject * moose_writeSBML(PyObject * dummy, PyObject * args)
{
    char * fname = NULL, * modelpath = NULL;
    if(!PyArg_ParseTuple(args, "ss:moose_writeSBML", &modelpath, &fname))
    {
        return NULL;
    }
    int ret = SHELLPTR->doWriteSBML(string(modelpath),string(fname));
    return Py_BuildValue("i", ret);
}

PyObject * moose_readSBML(PyObject * dummy, PyObject * args)
{
    char * fname = NULL, * modelpath = NULL, * solverclass = NULL;
    if(!PyArg_ParseTuple(args, "ss|s:moose_readSBML", &fname, &modelpath, &solverclass))
    {
        return NULL;
    }
    //Id ret = SHELLPTR->doReadSBML(string(fname), string(modelpath));
    //return Py_BuildValue("i", ret);
    _Id * model = (_Id*)PyObject_New(_Id, &IdType);
    //model->id_ = SHELLPTR->doReadSBML(string(fname), string(modelpath), string(solverclass));
    if (!solverclass)
    {
        model->id_ = SHELLPTR->doReadSBML(string(fname), string(modelpath));
    }
    else
    {
        model->id_ = SHELLPTR->doReadSBML(string(fname), string(modelpath), string(solverclass));
    }

    if (model->id_ == Id())
    {
        Py_XDECREF(model);
        PyErr_SetString(PyExc_IOError, "could not load model");
        return NULL;
    }
    PyObject * ret = reinterpret_cast<PyObject*>(model);
    return ret;
}

PyDoc_STRVAR(moose_loadModel_documentation,
             "loadModel(filename, modelpath, solverclass) -> vec\n"
             "\n"
             "Load model from a file to a specified path.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "filename : str\n"
             "    model description file.\n"
             "modelpath : str\n"
             "    moose path for the top level element of the model to be created.\n"
             "solverclass : str, optional\n"
             "    solver type to be used for simulating the model.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "vec\n"
             "    loaded model container vec.\n"
            );

PyObject * moose_loadModel(PyObject * dummy, PyObject * args)
{
    char * fname = NULL, * modelpath = NULL, * solverclass = NULL;

    if(!PyArg_ParseTuple(args, "ss|s:moose_loadModel", &fname, &modelpath, &solverclass))
    {
        cout << "here in moose load";
        return NULL;
    }
    _Id * model = (_Id*)PyObject_New(_Id, &IdType);
    if (!solverclass)
    {
        model->id_ = SHELLPTR->doLoadModel(string(fname), string(modelpath));
    }
    else
    {
        model->id_ = SHELLPTR->doLoadModel(string(fname), string(modelpath), string(solverclass));
    }
    if (model->id_ == Id())
    {
        Py_XDECREF(model);
        PyErr_SetString(PyExc_IOError, "could not load model");
        return NULL;
    }
    PyObject * ret = reinterpret_cast<PyObject*>(model);
    return ret;
}

PyDoc_STRVAR(moose_saveModel_documentation,
             "saveModel(source, filename) -> None\n"
             "\n"
             "Save model rooted at `source` to file `filename`.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "source : vec/element/str\n"
             "    root of the model tree\n"
             "\n"
             "filename : str\n"
             "    destination file to save the model in.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "None\n"
             "\n");

PyObject * moose_saveModel(PyObject * dummy, PyObject * args)
{
    char * filename = NULL;
    PyObject * source = NULL;
    Id model;
    if (!PyArg_ParseTuple(args, "Os: moose_saveModel", &source, &filename))
    {
        return NULL;
    }
    if (PyString_Check(source))
    {

        char * srcPath = PyString_AsString(source);
        if (!srcPath)
        {
            cout << " Niull ";
            return NULL;
        }
        assert(srcPath != NULL);

        model = Id(string(srcPath));
    }
    else if (Id_SubtypeCheck(source))
    {
        model = ((_Id*)source)->id_;
    }
    else if (ObjId_SubtypeCheck(source))
    {
        model = ((_ObjId*)source)->oid_.id;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "moose_saveModel: need an vec, element or string for first argument.");
        return NULL;
    }
    SHELLPTR->doSaveModel(model, filename);
    Py_RETURN_NONE;
}

PyObject * moose_setCwe(PyObject * dummy, PyObject * args)
{
    PyObject * element = NULL;
    char * path = NULL;
    ObjId oid;
    if (PyTuple_Size(args) == 0)
    {
        oid = Id("/");
    }
    else if(PyArg_ParseTuple(args, "s:moose_setCwe", &path))
    {
        oid = ObjId(string(path));
    }
    else if (PyArg_ParseTuple(args, "O:moose_setCwe", &element))
    {
        PyErr_Clear();
        if (PyObject_IsInstance(element, (PyObject*)&IdType))
        {
            oid = (reinterpret_cast<_Id*>(element))->id_;
        }
        else if (PyObject_IsInstance(element, (PyObject*)&ObjIdType))
        {
            oid = (reinterpret_cast<_ObjId*>(element))->oid_;
        }
        else
        {
            PyErr_SetString(PyExc_NameError, "setCwe: Argument must be an vec or element");
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
    if (oid.bad())
    {
        RAISE_INVALID_ID(NULL, "moose_setCwe");
    }
    SHELLPTR->setCwe(oid);
    Py_RETURN_NONE;
}

PyObject * moose_getCwe(PyObject * dummy, PyObject * args)
{
    if (!PyArg_ParseTuple(args, ":moose_getCwe"))
    {
        return NULL;
    }
    // _Id * cwe = (_Id*)PyObject_New(_Id, &IdType);
    // cwe->id_ = SHELLPTR->getCwe();
    // PyObject * ret = (PyObject*)cwe;
    // return ret;
    return oid_to_element(SHELLPTR->getCwe());
}

PyDoc_STRVAR(moose_connect_documentation,
             "connect(src, src_field, dest, dest_field, message_type) -> bool\n"
             "\n"
             "Create a message between `src_field` on `src` object to `dest_field`\n"
             "on `dest` object.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "src : element/vec/string\n"
             "    the source object (or its path)\n"
             "src_field : str\n"
             "    the source field name. Fields listed under `srcFinfo` and\n"
             "    `sharedFinfo` qualify for this.\n"
             "dest : element/vec/string\n"
             "    the destination object.\n"
             "dest_field : str\n"
             "    the destination field name. Fields listed under `destFinfo`\n"
             "    and `sharedFinfo` qualify for this.\n"
             "message_type : str (optional)\n"
             "    Type of the message. Can be `Single`, `OneToOne`, `OneToAll`.\n"
             "    If not specified, it defaults to `Single`.\n"
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
             ">>> pulsegen = moose.PulseGen('pulsegen')\n"
             ">>> spikegen = moose.SpikeGen('spikegen')\n"
             ">>> moose.connect(pulsegen, 'output', spikegen, 'Vm')\n"
             "\n"
            );

PyObject * moose_connect(PyObject * dummy, PyObject * args)
{
    PyObject * srcPtr = NULL, * destPtr = NULL;
    char * srcField = NULL, * destField = NULL, * msgType = NULL;
    static char default_msg_type[] = "Single";
    if(!PyArg_ParseTuple(args, "OsOs|s:moose_connect", &srcPtr, &srcField, &destPtr, &destField, &msgType))
    {
        return NULL;
    }
    if (msgType == NULL)
    {
        msgType = default_msg_type;
    }
    ObjId dest, src;
    if (ObjId_SubtypeCheck(srcPtr))
    {
        _ObjId * _src = reinterpret_cast<_ObjId*>(srcPtr);
        src = _src->oid_;
    }
    else if (Id_SubtypeCheck(srcPtr))
    {
        _Id * _src = reinterpret_cast<_Id*>(srcPtr);
        src = ObjId(_src->id_);
    }
    else if (PyString_Check(srcPtr))
    {
        char * _src = PyString_AsString(srcPtr);
        src = ObjId(string(_src));
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "source does not resolve to an element.");
        return NULL;
    }
    if (ObjId_SubtypeCheck(destPtr))
    {
        _ObjId * _dest = reinterpret_cast<_ObjId*>(destPtr);
        dest = _dest->oid_;
    }
    else if (Id_SubtypeCheck(destPtr))
    {
        _Id * _dest = reinterpret_cast<_Id*>(destPtr);
        dest = ObjId(_dest->id_);
    }
    else if (PyString_Check(destPtr))
    {
        char * _dest = PyString_AsString(destPtr);
        dest = ObjId(string(_dest));
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "target does not resolve to an element.");
        return NULL;
    }
    if (!Id::isValid(dest.id) || !Id::isValid(src.id))
    {
        RAISE_INVALID_ID(NULL, "moose_connect");
    }
    ObjId mid = SHELLPTR->doAddMsg(msgType, src, string(srcField), dest, string(destField));
    if ( mid.bad() )
    {
        PyErr_SetString(PyExc_NameError, "check field names and type compatibility.");
        return NULL;
    }
    return oid_to_element(mid);
}


PyDoc_STRVAR(moose_getFieldDict_documentation,
             "getFieldDict(className, finfoType) -> dict\n"
             "\n"
             "Get dictionary of field names and types for specified class.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "className : str\n"
             "    MOOSE class to find the fields of.\n"
             "finfoType : str (optional)\n"
             "    Finfo type of the fields to find. If empty or not specified, all\n"
             "    fields will be retrieved.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "dict\n"
             "    field names and their types.\n"
             "\n"
             "Notes\n"
             "-----\n"
             "    This behaviour is different from `getFieldNames` where only\n"
             "    `valueFinfo`s are returned when `finfoType` remains unspecified.\n"
             "\n"
             "Examples\n"
             "--------\n"
             "List all the source fields on class Neutral::\n"
             "\n"
             ">>> moose.getFieldDict('Neutral', 'srcFinfo')\n"
             "{'childMsg': 'int'}\n"
             "\n"
             "\n");
PyObject * moose_getFieldDict(PyObject * dummy, PyObject * args)
{
    char * className = NULL;
    char * fieldType = NULL;
    if (!PyArg_ParseTuple(args, "s|s:moose_getFieldDict", &className, &fieldType))
    {
        return NULL;
    }
    if (!className || (strlen(className) <= 0))
    {
        PyErr_SetString(PyExc_ValueError, "Expected non-empty class name.");
        return NULL;
    }

    Id classId = Id("/classes/" + string(className));
    if (classId == Id())
    {
        string msg = string(className);
        msg += " not a valid MOOSE class.";
        PyErr_SetString(PyExc_NameError, msg.c_str());
        return NULL;
    }
    static const char * finfoTypes [] = {"valueFinfo", "lookupFinfo", "srcFinfo", "destFinfo", "sharedFinfo", NULL};
    vector <string> fields, types;
    if (fieldType && strlen(fieldType) > 0)
    {
        if (getFieldDict(className, string(fieldType), fields, types) == 0)
        {
            PyErr_SetString(PyExc_ValueError, "Invalid finfo type.");
            return NULL;
        }
    }
    else
    {
        for (const char ** ptr = finfoTypes; *ptr != NULL; ++ptr)
        {
            if (getFieldDict(className, string(*ptr), fields, types) == 0)
            {
                string message = "No such finfo type: ";
                message += string(*ptr);
                PyErr_SetString(PyExc_ValueError, message.c_str());
                return NULL;
            }
        }
    }
    PyObject * ret = PyDict_New();
    if (!ret)
    {
        PyErr_SetString(PyExc_SystemError, "Could not allocate dictionary object.");
        return NULL;
    }
    for (unsigned int ii = 0; ii < fields.size(); ++ ii)
    {
        PyObject * value = Py_BuildValue("s", types[ii].c_str());
        if (value == NULL || PyDict_SetItemString(ret, fields[ii].c_str(), value) == -1)
        {
            Py_XDECREF(ret);
            Py_XDECREF(value);
            return NULL;
        }
    }
    return ret;
}

PyObject * moose_getField(PyObject * dummy, PyObject * args)
{
    PyObject * pyobj;
    const char * field;
    const char * type;
    if (!PyArg_ParseTuple(args, "Oss:moose_getfield", &pyobj, &field, &type))
    {
        return NULL;
    }
    if (!PyObject_IsInstance(pyobj, (PyObject*)&ObjIdType))
    {
        PyErr_SetString(PyExc_TypeError, "moose.getField(element, fieldname, fieldtype): First argument must be an instance of element or its subclass");
        return NULL;
    }
    string fname(field), ftype(type);
    ObjId oid = ((_ObjId*)pyobj)->oid_;
    if (!Id::isValid(oid.id))
    {
        RAISE_INVALID_ID(NULL, "moose_getField");
    }
    // Let us do this version using brute force. Might be simpler than getattro.
    if (ftype == "char")
    {
        char value =Field<char>::get(oid, fname);
        return PyInt_FromLong(value);
    }
    else if (ftype == "double")
    {
        double value = Field<double>::get(oid, fname);
        return PyFloat_FromDouble(value);
    }
    else if (ftype == "float")
    {
        float value = Field<float>::get(oid, fname);
        return PyFloat_FromDouble(value);
    }
    else if (ftype == "int")
    {
        int value = Field<int>::get(oid, fname);
        return PyInt_FromLong(value);
    }
    else if (ftype == "string")
    {
        string value = Field<string>::get(oid, fname);
        return PyString_FromString(value.c_str());
    }
    else if (ftype == "unsigned int" || ftype == "unsigned" || ftype == "uint")
    {
        unsigned int value = Field<unsigned int>::get(oid, fname);
        return PyInt_FromLong(value);
    }
    else if (ftype == "Id")
    {
        _Id * value = (_Id*)PyObject_New(_Id, &IdType);
        value->id_ = Field<Id>::get(oid, fname);
        return (PyObject*) value;
    }
    else if (ftype == "ObjId")
    {
        _ObjId * value = (_ObjId*)PyObject_New(_ObjId, &ObjIdType);
        value->oid_ = Field<ObjId>::get(oid, fname);
        return (PyObject*)value;
    }
    else if (ftype == "vector<int>")
    {
        vector<int> value = Field< vector < int > >::get(oid, fname);
        PyObject * ret = PyTuple_New((Py_ssize_t)value.size());

        for (unsigned int ii = 0; ii < value.size(); ++ ii )
        {
            PyObject * entry = Py_BuildValue("i", value[ii]);
            if (!entry || PyTuple_SetItem(ret, (Py_ssize_t)ii, entry))
            {
                Py_XDECREF(ret);
                ret = NULL;
                break;
            }
        }
        return ret;
    }
    else if (ftype == "vector<double>")
    {
        vector<double> value = Field< vector < double > >::get(oid, fname);
        PyObject * ret = PyTuple_New((Py_ssize_t)value.size());

        for (unsigned int ii = 0; ii < value.size(); ++ ii )
        {
            PyObject * entry = Py_BuildValue("f", value[ii]);
            if (!entry || PyTuple_SetItem(ret, (Py_ssize_t)ii, entry))
            {
                Py_XDECREF(ret);
                ret = NULL;
                break;
            }
        }
        return ret;
    }
    else if (ftype == "vector<float>")
    {
        vector<float> value = Field< vector < float > >::get(oid, fname);
        PyObject * ret = PyTuple_New((Py_ssize_t)value.size());

        for (unsigned int ii = 0; ii < value.size(); ++ ii )
        {
            PyObject * entry = Py_BuildValue("f", value[ii]);
            if (!entry || PyTuple_SetItem(ret, (Py_ssize_t)ii, entry))
            {
                Py_XDECREF(ret);
                ret = NULL;
                break;
            }
        }
        return ret;
    }
    else if (ftype == "vector<string>")
    {
        vector<string> value = Field< vector < string > >::get(oid, fname);
        PyObject * ret = PyTuple_New((Py_ssize_t)value.size());

        for (unsigned int ii = 0; ii < value.size(); ++ ii )
        {
            PyObject * entry = Py_BuildValue("s", value[ii].c_str());
            if (!entry || PyTuple_SetItem(ret, (Py_ssize_t)ii, entry))
            {
                Py_XDECREF(ret);
                return NULL;
            }
        }
        return ret;
    }
    else if (ftype == "vector<Id>")
    {
        vector<Id> value = Field< vector < Id > >::get(oid, fname);
        PyObject * ret = PyTuple_New((Py_ssize_t)value.size());

        for (unsigned int ii = 0; ii < value.size(); ++ ii )
        {
            _Id * entry = PyObject_New(_Id, &IdType);
            entry->id_ = value[ii];
            if (PyTuple_SetItem(ret, (Py_ssize_t)ii, (PyObject*)entry))
            {
                Py_XDECREF(ret);
                return NULL;
            }
        }
        return ret;
    }
    else if (ftype == "vector<ObjId>")
    {
        vector<ObjId> value = Field< vector < ObjId > >::get(oid, fname);
        PyObject * ret = PyTuple_New((Py_ssize_t)value.size());

        for (unsigned int ii = 0; ii < value.size(); ++ ii )
        {
            _ObjId * entry = PyObject_New(_ObjId, &ObjIdType);
            entry->oid_ = value[ii];
            if (PyTuple_SetItem(ret, (Py_ssize_t)ii, (PyObject*)entry))
            {
                Py_XDECREF(ret);
                return NULL;
            }
        }
        return ret;
    }
    PyErr_SetString(PyExc_TypeError, "Field type not handled.");
    return NULL;
}

PyDoc_STRVAR(moose_seed_documentation,
             "moose.seed(seedvalue) -> seed \n"
             "\n"
             "Reseed MOOSE random number generator.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "seed : int\n"
             "    Value to use for seeding. \n"
             "    All RNGs in moose except rand functions in moose.Function \n"
             "    expression use this seed.\n"
             "    By default (when this function is not called) seed is initializecd \n"
             "    to some random value using system random device (if available). \n"
             "    \n"
             "\n"
             "    default: random number generated using system random device\n"
             "\n"
             "Returns\n"
             "-------\n"
             "None\n"
             "\n"
             "See also\n"
             "--------\n"
             "moose.rand() : get a pseudorandom number in the [0,1) interval.\n"
            );

PyObject * moose_seed(PyObject * dummy, PyObject * args)
{
    long int seed = 0;
    if (!PyArg_ParseTuple(args, "|l", &seed))
    {
        return NULL;
    }
    pymoose_mtseed_(seed);
    Py_RETURN_NONE;
}

PyDoc_STRVAR(moose_rand_documentation,
             "moose.rand() -> [0,1)\n"
             "\n"
             "Returns\n"
             "-------\n"
             "float in [0, 1) real interval generated by MT19937.\n"
             "\n"
             "See also\n"
             "--------\n"
             "moose.seed() : reseed the random number generator.\n"
             "\n"
             "Notes\n"
             "-----\n"
             "MOOSE does not automatically seed the random number generator. You\n"
             "must explicitly call moose.seed() to create a new sequence of random\n"
             "numbers each time.\n"
             "\n");

PyObject * moose_rand(PyObject * dummy)
{
    return PyFloat_FromDouble(pymoose_mtrand_());
}

PyDoc_STRVAR(moose_wildcardFind_documentation,
             "moose.wildcardFind(expression) -> tuple of melements.\n"
             "\n"
             "Find an object by wildcard.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "expression : str\n"
             "    MOOSE allows wildcard expressions of the form::\n"
             "\n"
             "        {PATH}/{WILDCARD}[{CONDITION}]\n"
             "\n"
             "    where {PATH} is valid path in the element tree.\n"
             "    `{WILDCARD}` can be `#` or `##`.\n"
             "\n"
             "    `#` causes the search to be restricted to the children of the\n"
             "    element specified by {PATH}.\n"
             "\n"
             "    `##` makes the search to recursively go through all the descendants\n"
             "    of the {PATH} element.\n"
             "    {CONDITION} can be::\n"
             "\n"
             "        TYPE={CLASSNAME} : an element satisfies this condition if it is of\n"
             "        class {CLASSNAME}.\n"
             "        ISA={CLASSNAME} : alias for TYPE={CLASSNAME}\n"
             "        CLASS={CLASSNAME} : alias for TYPE={CLASSNAME}\n"
             "        FIELD({FIELDNAME}){OPERATOR}{VALUE} : compare field {FIELDNAME} with\n"
             "        {VALUE} by {OPERATOR} where {OPERATOR} is a comparison operator (=,\n"
             "        !=, >, <, >=, <=).\n"
             "\n"
             "    For example, /mymodel/##[FIELD(Vm)>=-65] will return a list of all\n"
             "    the objects under /mymodel whose Vm field is >= -65.\n"
             "\n"
             "Returns\n"
             "-------\n"
             "tuple\n"
             "   all elements that match the wildcard.\n"
             "\n");

PyObject * moose_wildcardFind(PyObject * dummy, PyObject * args)
{
    vector <ObjId> objects;
    char * wildcard_path = NULL;
    if (!PyArg_ParseTuple(args, "s:moose.wildcardFind", &wildcard_path))
    {
        return NULL;
    }
    wildcardFind(string(wildcard_path), objects);
    PyObject * ret = PyTuple_New(objects.size());
    if (ret == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "moose.wildcardFind: failed to allocate new tuple.");
        return NULL;
    }

    for (unsigned int ii = 0; ii < objects.size(); ++ii)
    {
        PyObject * entry = oid_to_element(objects[ii]);
        if (!entry)
        {
            Py_XDECREF(ret);
            PyErr_SetString(PyExc_RuntimeError, "moose.wildcardFind: failed to allocate new vec.");
            return NULL;
        }
        if (PyTuple_SetItem(ret, (Py_ssize_t)ii, entry))
        {
            Py_XDECREF(entry);
            Py_XDECREF(ret);
            return NULL;
        }
    }
    return ret;
}
/**
   This should not be required or accessible to the user. Put here
   for debugging threading issue.
*/
PyObject * moose_quit(PyObject * dummy)
{
    finalize();
    cout << "Quitting MOOSE." << endl;
    Py_RETURN_NONE;
}

/**
   Go through all elements under /classes and ask for defining a
   Python class for it.
*/
int defineAllClasses(PyObject * module_dict)
{
    static vector <Id> classes(Field< vector<Id> >::get(ObjId("/classes"),
                               "children"));
    for (unsigned ii = 0; ii < classes.size(); ++ii)
    {
        const string& className = classes[ii].element()->getName();
        if (verbosity > 0)
        {
            cout << "\nCreating " << className << endl;
        }
        const Cinfo * cinfo = Cinfo::find(className);
        if (!cinfo)
        {
            cerr << "Error: no cinfo found with name " << className << endl;
            return 0;
        }
        if (!defineClass(module_dict, cinfo))
        {
            return 0;
        }
    }
    return 1;
}
/**
  An attempt to define classes dynamically
  http://stackoverflow.com/questions/8066438/how-to-dynamically-create-a-derived-type-in-the-python-c-api
  gives a clue We pass class_name in stead of class_id because we
  have to recursively call this function using the base class
  string.
*/
PyDoc_STRVAR(moose_Class_documentation,
             "*-----------------------------------------------------------------*\n"
             "* This is Python generated documentation.                         *\n"
             "* Use moose.doc('classname') to display builtin documentation for *\n"
             "* class `classname`.                                              *\n"
             "* Use moose.doc('classname.fieldname') to display builtin         *\n"
             "* documentationfor `field` in class `classname`.                  *\n"
             "*-----------------------------------------------------------------*\n"
            );

int defineClass(PyObject * module_dict, const Cinfo * cinfo)
{
    const string& className = cinfo->name();
    map <string, PyTypeObject * >::iterator existing =
        get_moose_classes().find(className);
    if (existing != get_moose_classes().end())
    {
        return 1;
    }
    const Cinfo* base = cinfo->baseCinfo();
#ifndef NDEBUG
    if (verbosity > 1)
    {
        cout << "Defining class " << className << endl; //" with base=" << base->name() << endl;
    }
#endif
    if (base && !defineClass(module_dict, base))
    {
        return 0;
    }

    string str = "moose." + className;

    PyTypeObject * new_class =
        (PyTypeObject*)PyType_Type.tp_alloc(&PyType_Type, 0);
// Python3 does not like it without heaptype: aborts on import
// Fatal Python error:
// type_traverse() called for non-heap type 'moose.Neutral'
#ifdef PY3K
    new_class->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HEAPTYPE;
    PyHeapTypeObject* et = (PyHeapTypeObject*)new_class;
    et->ht_name = PyUnicode_FromString(className.c_str());
#if PY_MINOR_VERSION >= 3
    et->ht_qualname = PyUnicode_FromString(str.c_str());
#endif
#else
    new_class->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
#endif
    /*
      Thu Jul 9 09:58:09 IST 2015 - commenting out
      Py_TPFLAGS_HEAPTYPE because it causes segfault on accessing
      __class__ attribute of instances. Bug # 168. Another
      possible solution would be to catch __class__ request in
      tp_getattro and INCREF the class object when returning the
      same.

      ------
    should we avoid Py_TPFLAGS_HEAPTYPE as it imposes certain
    limitations:
    http://mail.python.org/pipermail/python-dev/2009-July/090921.html
    But otherwise somehow GC tries tp_traverse on these classes
    (even when I unset Py_TPFLAGS_HAVE_GC) and fails the
    assertion in debug build of Python:

    python: Objects/typeobject.c:2683: type_traverse: Assertion `type->tp_flags & Py_TPFLAGS_HEAPTYPE' failed.

    In released versions of Python there is a crash at
    Py_Finalize().

    Also if HEAPTYPE is true, then help(classname) causes a
    segmentation fault as it tries to convert the class object
    to a heaptype object (resulting in an invalid pointer). If
    heaptype is not set it uses tp_name to print the help.
    Py_SIZE(new_class) = sizeof(_ObjId);
    */
    new_class->tp_name = strdup(str.c_str());
    new_class->tp_doc = moose_Class_documentation;

    // strncpy(new_class->tp_doc, moose_Class_documentation, strlen(moose_Class_documentation));
    map<string, PyTypeObject *>::iterator base_iter =
        get_moose_classes().find(cinfo->getBaseClass());
    if (base_iter == get_moose_classes().end())
    {
        new_class->tp_base = &ObjIdType;
    }
    else
    {
        new_class->tp_base = base_iter->second;
    }
    Py_INCREF(new_class->tp_base);
    // Define all the lookupFields
    if (!defineLookupFinfos(cinfo))
    {
        return 0;
    }
    // Define the destFields
    if (!defineDestFinfos(cinfo))
    {
        return 0;
    }

    // Define the element fields
    if (!defineElementFinfos(cinfo))
    {
        return 0;
    }
    // #ifndef NDEBUG
    //         cout << "Get set defs:" << className << endl;
    //         for (unsigned int ii = 0; ii < get_getsetdefs()[className].size(); ++ii){
    //             cout << ii;
    //             if (get_getsetdefs()[className][ii].name != NULL){
    //                 cout << ": " << get_getsetdefs()[className][ii].name;
    //             } else {
    //                 cout << "Empty";
    //             }
    //             cout << endl;
    //         }
    //         cout << "End getsetdefs: " << className << endl;
    // #endif
    // The getsetdef array must be terminated with empty objects.
    PyGetSetDef empty;
    empty.name = NULL;
    get_getsetdefs()[className].push_back(empty);
    get_getsetdefs()[className].back().name = NULL;
    new_class->tp_getset = &(get_getsetdefs()[className][0]);
    /*
      Cannot do this for HEAPTYPE ?? but pygobject.c does this in
      pygobject_register_class
    */
    if (PyType_Ready(new_class) < 0)
    {
        cerr << "Fatal error: Could not initialize class '" << className
             << "'" << endl;
        return 0;
    }
    get_moose_classes().insert(pair<string, PyTypeObject*> (className, new_class));
    Py_INCREF(new_class);

#ifdef PY3K
    PyDict_SetItemString(new_class->tp_dict, "__module__", PyUnicode_InternFromString("moose"));
#endif
    // string doc = const_cast<Cinfo*>(cinfo)->getDocs();
    // PyDict_SetItemString(new_class->tp_dict, "__doc__", PyString_FromString(" \0"));
    // PyDict_SetItemString(module_dict, className.c_str(), (PyObject *)new_class);
    return 1;
}

PyObject * moose_ObjId_get_destField_attr(PyObject * self, void * closure)
{
    if (!PyObject_IsInstance(self, (PyObject*)&ObjIdType))
    {
        PyErr_SetString(PyExc_TypeError, "First argument must be an instance of element");
        return NULL;
    }
    _ObjId * obj = (_ObjId*)self;
    if (!Id::isValid(obj->oid_.id))
    {
        RAISE_INVALID_ID(NULL, "moose_ObjId_get_destField_attr");
    }
    char * name = NULL;
    if (!PyArg_ParseTuple((PyObject *)closure,
                          "s:_get_destField: "
                          "expected a string in getter closure.",
                          &name))
    {
        return NULL;
    }
    PyObject * args = PyTuple_New(2);

    PyTuple_SetItem(args, 0, self);
    Py_INCREF(self); // compensate for reference stolen by PyTuple_SetItem
    PyTuple_SetItem(args, 1, PyString_FromString(name));
    _Field * ret = PyObject_New(_Field, &moose_DestField);
    if (moose_DestField.tp_init((PyObject*)ret, args, NULL) != 0)
    {
        Py_XDECREF((PyObject*)ret);
        ret = NULL;
        PyErr_SetString(PyExc_RuntimeError, "moose_ObjId_get_destField_attr: failed to init DestField object");
    }
    Py_DECREF(args);
    return (PyObject*)ret;
}


int defineDestFinfos(const Cinfo * cinfo)
{
    const string& className = cinfo->name();
#ifndef NDEBUG
    if (verbosity > 1)
    {
        cout << "\tCreating destField attributes for " << className << endl;
    }
#endif
    vector <PyGetSetDef>& vec = get_getsetdefs()[className];
    /*
      We do not know the final number of user-accessible
      destFinfos as we have to ignore the destFinfos starting
      with get/set. So use a vector instead of C array.
    */
    size_t currIndex = vec.size();
    for (unsigned int ii = 0; ii < cinfo->getNumDestFinfo(); ++ii)
    {
        Finfo * destFinfo = const_cast<Cinfo*>(cinfo)->getDestFinfo(ii);
        const string& name = destFinfo->name();
        /*
          get_{xyz} and set_{xyz} are internal destFinfos for
          accessing valueFinfos. Ignore them.

          With the '_' removed from internal get/set for value
          fields, we cannot separate them out. - Subha Fri Jan 31
          16:43:51 IST 2014

          The policy changed in the past and hence the following were commented out.
          - Subha Tue May 26 00:25:28 EDT 2015
         */
        // if (name.find("get") == 0 || name.find("set") == 0){
        //     continue;
        // }
        PyGetSetDef destFieldGetSet;
        vec.push_back(destFieldGetSet);

        vec[currIndex].name = (char*)calloc(name.size() + 1, sizeof(char));
        strncpy(vec[currIndex].name,
                const_cast<char*>(name.c_str()),
                name.size());

        vec[currIndex].doc = (char*) "Destination field";
        vec[currIndex].get = (getter)moose_ObjId_get_destField_attr;
        PyObject * args = PyTuple_New(1);
        if (args == NULL)
        {
            cerr << "moosemodule.cpp: defineDestFinfos: Failed to allocate tuple" << endl;
            return 0;
        }
        PyTuple_SetItem(args, 0, PyString_FromString(name.c_str()));
        vec[currIndex].closure = (void*)args;

        //LOG( debug, "\tCreated destField " << vec[currIndex].name );

        ++currIndex;
    } // ! for

    return 1;
}

/**
   Try to obtain a LookupField object for a specified
   lookupFinfo. The first item in `closure` must be the name of
   the LookupFinfo - {fieldname}. The LookupField is identified by
   {path}.{fieldname} where {path} is the unique path of the moose
   element `self`. We look for an already initialized LookupField
   object for this identifier and return if one is
   found. Otherwise, we create a new LookupField object and buffer
   it in a map before returning.

 */
PyObject * moose_ObjId_get_lookupField_attr(PyObject * self,
        void * closure)
{
    if (!PyObject_IsInstance(self, (PyObject*)&ObjIdType))
    {
        PyErr_SetString(PyExc_TypeError,
                        "First argument must be an instance of element");
        return NULL;
    }
    _ObjId * obj = (_ObjId*)self;
    if (!Id::isValid(obj->oid_.id))
    {
        RAISE_INVALID_ID(NULL, "moose_ObjId_get_lookupField_attr");
    }
    char * name = NULL;
    if (!PyArg_ParseTuple((PyObject *)closure,
                          "s:moose_ObjId_get_lookupField_attr: expected a string in getter closure.",
                          &name))
    {
        return NULL;
    }
    assert(name);
    /* Create a new instance of LookupField `name` and set it as
     * an attribute of the object self. Create the argument for
     * init method of LookupField.  This will be (fieldname, self) */
    PyObject * args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, self);
    Py_INCREF(self); // compensate for stolen ref
    PyTuple_SetItem(args, 1, PyString_FromString(name));
    _Field * ret = PyObject_New(_Field, &moose_LookupField);
    if (moose_LookupField.tp_init((PyObject*)ret, args, NULL) != 0)
    {
        Py_XDECREF((PyObject*)ret);
        ret = NULL;
        PyErr_SetString(PyExc_RuntimeError,
                        "moose_ObjId_get_lookupField_attr: failed to init LookupField object");
    }
    Py_DECREF(args);
    return (PyObject*)ret;
}

int defineLookupFinfos(const Cinfo * cinfo)
{
    const string & className = cinfo->name();
#ifndef NDEBUG
    if (verbosity > 1)
    {
        cout << "\tDefining lookupFields for " << className << endl;
    }
#endif
    unsigned int num = cinfo->getNumLookupFinfo();
    unsigned int currIndex = get_getsetdefs()[className].size();
    for (unsigned int ii = 0; ii < num; ++ii)
    {
        const string& name = const_cast<Cinfo*>(cinfo)->getLookupFinfo(ii)->name();
        PyGetSetDef getset;
        get_getsetdefs()[className].push_back(getset);
        get_getsetdefs()[className][currIndex].name = (char*)calloc(name.size() + 1, sizeof(char));
        strncpy(const_cast<char*>(get_getsetdefs()[className][currIndex].name)
                , const_cast<char*>(name.c_str()), name.size());
        get_getsetdefs()[className][currIndex].doc = (char*) "Lookup field";
        get_getsetdefs()[className][currIndex].get = (getter)moose_ObjId_get_lookupField_attr;
        PyObject * args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, PyString_FromString(name.c_str()));
        get_getsetdefs()[className][currIndex].closure = (void*)args;
#ifndef NDEBUG
        if (verbosity > 1)
        {
            cout << "\tDefined lookupField " << get_getsetdefs()[className][currIndex].name << endl;
        }
#endif

        ++currIndex;
    }
    return 1;
}

PyObject * moose_ObjId_get_elementField_attr(PyObject * self,
        void * closure)
{
    // if (!PyObject_IsInstance(self, (PyObject*)&ObjIdType)){
    //       PyErr_SetString(PyExc_TypeError,
    //                       "First argument must be an instance of element");
    //       return NULL;
    //   }
    _ObjId * obj = (_ObjId*)self;
    if (!Id::isValid(obj->oid_.id))
    {
        RAISE_INVALID_ID(NULL, "moose_ObjId_get_elementField_attr");
    }
    char * name = NULL;
    if (!PyArg_ParseTuple((PyObject *)closure,
                          "s:moose_ObjId_get_elementField_attr: expected a string in getter closure.",
                          &name))
    {
        return NULL;
    }
    // Create a new instance of ElementField `name` and set it as
    // an attribute of the object `self`.
    // 1. Create the argument for init method of ElementField.  This
    //   will be (fieldname, self)
    PyObject * args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, self);
    Py_INCREF(self); // compensate for stolen ref
    PyTuple_SetItem(args, 1, PyString_FromString(name));
    _Field * ret = PyObject_New(_Field, &moose_ElementField);
    // 2. Now use this arg to actually create the element field.
    if (moose_ElementField.tp_init((PyObject*)ret, args, NULL) != 0)
    {
        Py_DECREF((PyObject*)ret);
        ret = NULL;
        PyErr_SetString(PyExc_RuntimeError, "moose_ObjId_get_elementField_attr: failed to init ElementField object");
    }
    Py_DECREF(args);
    return (PyObject*)ret;
}

int defineElementFinfos(const Cinfo * cinfo)
{
    const string & className = cinfo->name();
#ifndef NDEBUG
    if (verbosity > 1)
    {
        cout << "\tDefining elementFields for " << className << endl;
    }
#endif
    unsigned int num = cinfo->getNumFieldElementFinfo();
    unsigned int currIndex = get_getsetdefs()[className].size();
    for (unsigned int ii = 0; ii < num; ++ii)
    {
        const string& name = const_cast<Cinfo*>(cinfo)->getFieldElementFinfo(ii)->name();
        PyGetSetDef getset;
        get_getsetdefs()[className].push_back(getset);
        get_getsetdefs()[className][currIndex].name = (char*)calloc(name.size() + 1, sizeof(char));
        strncpy(const_cast<char*>(get_getsetdefs()[className][currIndex].name)
                , const_cast<char*>(name.c_str()), name.size());
        get_getsetdefs()[className][currIndex].doc = (char*) "Element field";
        get_getsetdefs()[className][currIndex].get = (getter)moose_ObjId_get_elementField_attr;
        PyObject * args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, PyString_FromString(name.c_str()));
        get_getsetdefs()[className][currIndex].closure = (void*)args;
#ifndef NDEBUG
        if (verbosity > 1)
        {
            cout << "\tDefined elementField " << get_getsetdefs()[className][currIndex].name << endl;
        }
#endif

        ++currIndex;
    }
    return 1;
}

PyObject * oid_to_element(ObjId oid)
{
    string classname = Field<string>::get(oid, "className");
    map<string, PyTypeObject *>::iterator it = get_moose_classes().find(classname);
    if (it == get_moose_classes().end())
    {
        return NULL;
    }
    PyTypeObject * pyclass = it->second;
    _ObjId * new_obj = PyObject_New(_ObjId, pyclass);
    new_obj->oid_ = oid;
    // Py_XINCREF(new_obj);  // why? PyObject_New initializes refcnt to 1
    return (PyObject*)new_obj;
}

PyDoc_STRVAR(moose_element_documentation,
             "moose.element(arg) -> moose object\n"
             "\n"
             "Convert a path or an object to the appropriate builtin moose class\n"
             "instance\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "arg : str/vec/moose object\n"
             "    path of the moose element to be converted or another element (possibly\n"
             "    available as a superclass instance).\n"
             "\n"
             "Returns\n"
             "-------\n"
             "melement\n"
             "    MOOSE element (object) corresponding to the `arg` converted to write subclass.\n"
             "\n");
PyObject * moose_element(PyObject* dummy, PyObject * args)
{
    char * path = NULL;
    PyObject * obj = NULL;
    ObjId oid;
    unsigned nid = 0, did = 0, fidx = 0;
    Id id;
    unsigned int numData = 0;
    if (PyArg_ParseTuple(args, "s", &path))
    {
        oid = ObjId(path);
        //            cout << "Original Path " << path << ", Element Path: " << oid.path() << endl;
        if ( oid.bad() )
        {
            PyErr_SetString(PyExc_ValueError, ( std::string("moose_element: '")
                                                + std::string(path)
                                                + std::string("' does not exist!")
                                              ).c_str()
                           );
            return NULL;
        }
        PyObject * new_obj = oid_to_element(oid);
        if (new_obj)
        {
            return new_obj;
        }
        PyErr_SetString(PyExc_TypeError, "moose_element: unknown class");
        return NULL;
    }
    PyErr_Clear();
    if (PyArg_ParseTuple(args, "I|II", &nid, &did, &fidx))
    {
        oid = ObjId(id, did, fidx);
        // Todo: test for validity
        PyObject * new_obj = oid_to_element(oid);
        if (!new_obj)
        {
            PyErr_SetString(PyExc_RuntimeError, "moose_element: not a moose class.");
        }
        return new_obj;
    }
    PyErr_Clear();
    if (!PyArg_ParseTuple(args, "O|II", &obj, &did, &fidx))
    {
        PyErr_SetString(PyExc_TypeError, "moose_element: argument must be a path or an existing element or an vec");
        return NULL;
    }
    // PyErr_Clear();
    if (PyObject_IsInstance(obj, (PyObject*)&ObjIdType))
    {
        oid = ((_ObjId*)obj)->oid_;
    }
    else if (PyObject_IsInstance(obj, (PyObject*)&IdType))
    {
        oid = ObjId(((_Id*)obj)->id_, did, fidx); // TODO: check for validity
    }
    else if (ElementField_SubtypeCheck(obj))
    {
        oid = ObjId(((_Id*)moose_ElementField_getId((_Field*)obj, NULL))->id_);
    }
    if (oid.bad())
    {
        PyErr_SetString(PyExc_TypeError, "moose_element: cannot convert to moose element.");
        return NULL;
    }
    PyObject * new_obj = oid_to_element(oid);
    if (!new_obj)
    {
        PyErr_SetString(PyExc_RuntimeError, "moose_element: not a moose class.");
    }
    return new_obj;
}


/////////////////////////////////////////////////////////////////////
// Method definitions for MOOSE module
/////////////////////////////////////////////////////////////////////
static PyMethodDef MooseMethods[] =
{
    {"element", (PyCFunction)moose_element, METH_VARARGS, moose_element_documentation},
    {"getFieldNames", (PyCFunction)moose_getFieldNames, METH_VARARGS, moose_getFieldNames_documentation},
    {"copy", (PyCFunction)moose_copy, METH_VARARGS|METH_KEYWORDS, moose_copy_documentation},
    {"move", (PyCFunction)moose_move, METH_VARARGS, "Move a vec object to a destination."},
    {"delete", (PyCFunction)moose_delete, METH_VARARGS, moose_delete_documentation},
    {"useClock", (PyCFunction)moose_useClock, METH_VARARGS, moose_useClock_documentation},
    {"setClock", (PyCFunction)moose_setClock, METH_VARARGS, moose_setClock_documentation},
    {"start", (PyCFunction)moose_start, METH_VARARGS, moose_start_documentation},
    {"reinit", (PyCFunction)moose_reinit, METH_VARARGS, moose_reinit_documentation},
    {"stop", (PyCFunction)moose_stop, METH_VARARGS, "Stop simulation"},
    {"isRunning", (PyCFunction)moose_isRunning, METH_VARARGS, "True if the simulation is currently running."},
    {"exists", (PyCFunction)moose_exists, METH_VARARGS, "True if there is an object with specified path."},
    {"writeSBML", (PyCFunction)moose_writeSBML, METH_VARARGS, "Export biochemical model to an SBML file."},
    {"readSBML",  (PyCFunction)moose_readSBML,  METH_VARARGS, "Import SBML model to Moose."},
    {"loadModel", (PyCFunction)moose_loadModel, METH_VARARGS, moose_loadModel_documentation},
    {"saveModel", (PyCFunction)moose_saveModel, METH_VARARGS, moose_saveModel_documentation},
    {"connect", (PyCFunction)moose_connect, METH_VARARGS, moose_connect_documentation},
    {"getCwe", (PyCFunction)moose_getCwe, METH_VARARGS, "Get the current working element. 'pwe' is an alias of this function."},
    // {"pwe", (PyCFunction)moose_getCwe, METH_VARARGS, "Get the current working element. 'getCwe' is an alias of this function."},
    {"setCwe", (PyCFunction)moose_setCwe, METH_VARARGS, "Set the current working element. 'ce' is an alias of this function"},
    // {"ce", (PyCFunction)moose_setCwe, METH_VARARGS, "Set the current working element. setCwe is an alias of this function."},
    {"getFieldDict", (PyCFunction)moose_getFieldDict, METH_VARARGS, moose_getFieldDict_documentation},
    {
        "getField", (PyCFunction)moose_getField, METH_VARARGS,
        "getField(element, field, fieldtype) -- Get specified field of specified type from object vec."
    },
    {"seed", (PyCFunction)moose_seed, METH_VARARGS, moose_seed_documentation},
    {"rand", (PyCFunction)moose_rand, METH_NOARGS, moose_rand_documentation},
    {"wildcardFind", (PyCFunction)moose_wildcardFind, METH_VARARGS, moose_wildcardFind_documentation},
    {
        "quit", (PyCFunction)moose_quit, METH_NOARGS, "Finalize MOOSE threads and quit MOOSE. This is made available for"
        " debugging purpose only. It will automatically get called when moose"
        " module is unloaded. End user should not use this function."
    },

    {NULL, NULL, 0, NULL}        /* Sentinel */
};



///////////////////////////////////////////////////////////
// module initialization
///////////////////////////////////////////////////////////
PyDoc_STRVAR(moose_module_documentation,
             "MOOSE = Multiscale Object-Oriented Simulation Environment.\n"
             "\n"
             "Moose is the core of a modern software platform for the simulation\n"
             "of neural systems ranging from subcellular components and\n"
             "biochemical reactions to complex models of single neurons, large\n"
             "networks, and systems-level processes.");

#ifdef PY3K

int moose_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

int moose_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    // I did get a segmentation fault at exit (without running a reinit() or start()) after creating a compartment. After putting the finalize here it went away. But did not reoccur even after commenting it out. Will need closer debugging.
    // - Subha 2012-08-18, 00:36
    finalize();
    return 0;
}


static struct PyModuleDef MooseModuleDef =
{
    PyModuleDef_HEAD_INIT,
    "moose", /* m_name */
    moose_module_documentation, /* m_doc */
    sizeof(struct module_state), /* m_size */
    MooseMethods, /* m_methods */
    0, /* m_reload */
    moose_traverse, /* m_traverse */
    moose_clear, /* m_clear */
    NULL /* m_free */
};

#define INITERROR return NULL
#define MODINIT(name) PyInit_##name()
#else // Python 2
#define INITERROR return
#define MODINIT(name) init##name()
#endif

PyMODINIT_FUNC MODINIT(_moose)
{
    clock_t modinit_start = clock();
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    // First of all create the Shell.  We convert the environment
    // variables into c-like argv array
    vector<string> args = setup_runtime_env();
    int argc = args.size();
    char ** argv = (char**)calloc(args.size(), sizeof(char*));
    for (int ii = 0; ii < argc; ++ii)
    {
        argv[ii] = (char*)(calloc(args[ii].length()+1, sizeof(char)));
        strncpy(argv[ii], args[ii].c_str(), args[ii].length()+1);
    }
    // Should not call. No pthreads now. PyEval_InitThreads();
    Id shellId = getShell(argc, argv);
    for (int ii = 1; ii < argc; ++ii)
    {
        free(argv[ii]);
    }
    // Now initialize the module
#ifdef PY3K
    PyObject * moose_module = PyModule_Create(&MooseModuleDef);
#else
    PyObject *moose_module = Py_InitModule3("_moose",
                                            MooseMethods,
                                            moose_module_documentation);
#endif
    if (moose_module == NULL)
    {
        INITERROR;
    }
    struct module_state * st = GETSTATE(moose_module);
    char error[] = "moose.Error";
    st->error = PyErr_NewException(error, NULL, NULL);
    if (st->error == NULL)
    {
        Py_XDECREF(moose_module);
        INITERROR;
    }
    int registered = Py_AtExit(&finalize);
    if (registered != 0)
    {
        cerr << "Failed to register finalize() to be called at exit. " << endl;
    }
#ifdef USE_NUMPY
    import_array();
#endif
    // Add Id type
    // Py_TYPE(&IdType) = &PyType_Type; // unnecessary - filled in by PyType_Ready
    IdType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&IdType) < 0)
    {
        PyErr_Print();
        exit(-1);
    };
    Py_INCREF(&IdType);
    PyModule_AddObject(moose_module, "vec", (PyObject*)&IdType);

    // Add ObjId type
    // Py_TYPE(&ObjIdType) = &PyType_Type; // unnecessary - filled in by PyType_Ready
    ObjIdType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&ObjIdType) < 0)
    {
        PyErr_Print();
        exit(-1);
    };
    Py_INCREF(&ObjIdType);
    PyModule_AddObject(moose_module, "melement", (PyObject*)&ObjIdType);

    // Add LookupField type
    // Py_TYPE(&moose_LookupField) = &PyType_Type;  // unnecessary - filled in by PyType_Ready
    // moose_LookupField.tp_new = PyType_GenericNew;
    if (PyType_Ready(&moose_LookupField) < 0)
    {
        PyErr_Print();
        exit(-1);
    }
    Py_INCREF(&moose_LookupField);
    PyModule_AddObject(moose_module, "LookupField", (PyObject*)&moose_LookupField);

    if (PyType_Ready(&moose_ElementField) < 0)
    {
        PyErr_Print();
        exit(-1);
    }
    Py_INCREF(&moose_ElementField);
    PyModule_AddObject(moose_module, "ElementField", (PyObject*)&moose_ElementField);
    // Add DestField type
    // Py_TYPE(&moose_DestField) = &PyType_Type; // unnecessary - filled in by PyType_Ready
    // moose_DestField.tp_flags = Py_TPFLAGS_DEFAULT;
    // moose_DestField.tp_call = moose_DestField_call;
    // moose_DestField.tp_doc = DestField_documentation;
    // moose_DestField.tp_new = PyType_GenericNew;
    if (PyType_Ready(&moose_DestField) < 0)
    {
        PyErr_Print();
        exit(-1);
    }
    Py_INCREF(&moose_DestField);
    PyModule_AddObject(moose_module, "DestField", (PyObject*)&moose_DestField);

    // PyModule_AddIntConstant(moose_module, "SINGLETHREADED", isSingleThreaded);
    PyModule_AddIntConstant(moose_module, "NUMCORES", numCores);
    PyModule_AddIntConstant(moose_module, "NUMNODES", numNodes);
    // PyModule_AddIntConstant(moose_module, "NUMPTHREADS", numProcessThreads);
    PyModule_AddIntConstant(moose_module, "MYNODE", myNode);
    PyModule_AddIntConstant(moose_module, "INFINITE", isInfinite);
    PyModule_AddStringConstant(moose_module, "__version__", SHELLPTR->doVersion().c_str());
    PyModule_AddStringConstant(moose_module, "VERSION", SHELLPTR->doVersion().c_str());
    PyObject * module_dict = PyModule_GetDict(moose_module);
    clock_t defclasses_start = clock();
    if (!defineAllClasses(module_dict))
    {
        PyErr_Print();
        exit(-1);
    }
    for (map <string, PyTypeObject * >::iterator ii = get_moose_classes().begin();
            ii != get_moose_classes().end(); ++ii)
    {
        PyModule_AddObject(moose_module, ii->first.c_str(), (PyObject*)(ii->second));
    }

    clock_t defclasses_end = clock();

    LOG( moose::info, "`Time to define moose classes:" 
            << (defclasses_end - defclasses_start) * 1.0 /CLOCKS_PER_SEC
       );

    PyGILState_Release(gstate);
    clock_t modinit_end = clock();

    LOG( moose::info, "`Time to initialize module:" 
            << (modinit_end - modinit_start) * 1.0 /CLOCKS_PER_SEC 
       );

    if (doUnitTests)
    {
        test_moosemodule();
    }
#ifdef PY3K
    return moose_module;
#endif
} //! init_moose


//////////////////////////////////////////////
// Main function
//////////////////////////////////////////////

// int main(int argc, char* argv[])
// {
// #ifdef PY3K
//     size_t len = strlen(argv[0]);
//     wchar_t * warg = (wchar_t*)calloc(sizeof(wchar_t), len);
//     mbstowcs(warg, argv[0], len);
// #else
//     char * warg = argv[0];
// #endif
//     for (int ii = 0; ii < argc; ++ii){
//     cout << "ARGV: " << argv[ii];
// }
//     cout << endl;
//     Py_SetProgramName(warg);
//     Py_Initialize();
//     MODINIT(_moose);
// #ifdef PY3K
//     free(warg);
// #endif
//     return 0;
// }

//
// moosemodule.cpp ends here
