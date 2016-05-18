// vec.cpp ---
//
// Filename: vec.cpp
// Description:
// Author:
// Maintainer:
// Created: Mon Jul 22 16:46:37 2013 (+0530)
// Version:
// Last-Updated: Fri Sep 25 23:00:48 2015 (-0400)
//           By: subha
//     Update #: 80
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
// Mon Jul 22 16:47:10 IST 2013 - Splitting contents of
// moosemodule.cpp into speparate files.
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
#include <structmember.h> // This defines the type id macros like T_STRING
// #include "numpy/arrayobject.h"

#include "../utility/simple_logger.hpp"

#include <iostream>
#include <typeinfo>
#include <cstring>
#include <map>
#include <ctime>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "../basecode/header.h"
#include "../basecode/global.h"
#include "../basecode/Id.h"
#include "../basecode/ObjId.h"
#include "../utility/utility.h"
#include "../shell/Shell.h"
#include "../shell/Wildcard.h"

#include "moosemodule.h"

using namespace std;

extern int verbosity;

///////////////////////////////////////////////
// Python method lists for PyObject of Id
///////////////////////////////////////////////
PyDoc_STRVAR(moose_Id_delete_doc,
             "vec.delete() -> None"
             "\n"
             "\nDelete the underlying moose object. This will invalidate all"
             "\nreferences to this object and any attempt to access it will raise a"
             "\nValueError."
             "\n");

PyDoc_STRVAR(moose_Id_setField_doc,
             "setField(fieldname, value_vector) -> None\n"
             "\n"
             "Set the value of `fieldname` in all elements under this vec.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "fieldname: str\n"
             "    field to be set.\n"
             "value: sequence of values\n"
             "    sequence of values corresponding to individual elements under this\n"
             "    vec.\n"
             "\n"
             "Notes\n"
             "-----\n"
             "    This is an interface to SetGet::setVec\n"
            );

static PyMethodDef IdMethods[] =
{
    // {"init", (PyCFunction)moose_Id_init, METH_VARARGS,
    //  "Initialize a Id object."},
    {
        "delete", (PyCFunction)moose_Id_delete, METH_NOARGS,
        moose_Id_delete_doc
    },
    {
        "getValue", (PyCFunction)moose_Id_getValue, METH_NOARGS,
        "Return integer representation of the id of the element."
    },
    {
        "getPath", (PyCFunction)moose_Id_getPath, METH_NOARGS,
        "Return the path of this vec object."
    },
    {
        "getShape", (PyCFunction)moose_Id_getShape, METH_NOARGS,
        "Get the shape of the vec object as a tuple."
    },
    {
        "setField", (PyCFunction)moose_Id_setField, METH_VARARGS,
        moose_Id_setField_doc
    },
    {NULL, NULL, 0, NULL},        /* Sentinel */
};

static PySequenceMethods IdSequenceMethods =
{
    (lenfunc)moose_Id_getLength, // sq_length
    0, //sq_concat
    0, //sq_repeat
    (ssizeargfunc)moose_Id_getItem, //sq_item
#ifndef PY3K
    (ssizessizeargfunc)moose_Id_getSlice, // getslice
#else
    0,
#endif
    0, //sq_ass_item
    0, // setslice
    (objobjproc)moose_Id_contains, // sq_contains
    0, // sq_inplace_concat
    0 // sq_inplace_repeat
};

static PyMappingMethods IdMappingMethods =
{
    (lenfunc)moose_Id_getLength, //mp_length
    (binaryfunc)moose_Id_subscript, // mp_subscript
    0 // mp_ass_subscript
};

///////////////////////////////////////////////
// Type defs for PyObject of Id
///////////////////////////////////////////////

PyDoc_STRVAR(moose_Id_doc,
             "An object uniquely identifying a moose array-element.\n"
             "\n"
             "array-elements are array-like objects which can have one or more"
             " single-elements within them."
             " vec can be traversed like a Python sequence and is item is an"
             " element identifying single-objects contained in the array element.\n"
             "\n"
             "you can create multiple references to the same MOOSE object in Python,"
             " but as long as they have the same path/id value, they all point to"
             " the same entity in MOOSE.\n"
             "\n"
             "Field access are vectorized. For example, if `comp` is a vec of"
             " Compartments, which has a field called `Vm` for membrane voltage, then"
             " `comp.Vm` returns a"
             " tuple containing the `Vm` value of all the single-elements in this"
             " vec. There are a few special fields that are unique for vec and are not"
             " vectorized. These are `path`, `name`, `value`, `shape` and `className`."
             " There are two ways an vec can be initialized, (1) create a new array"
             " element or (2) create a reference to an existing object.\n"
             "\n    Fields:"
             "\n    -------"
             "\n    path : str"
             "\n       Path of the vec. In moose vecs are organized in a tree structure"
             " like unix file system and the paths follow the same convention."
             "\n"
             "\n    name : str"
             "\n        Name of the vec."
             "\n"
             "\n    value : int/long"
             "\n        Numeric identifier of the vec. This is unique within a single"
             " execution. vec comparison is based on this value and its hash is also"
             " this. So you can compare and sort vecs and use them as dict keys."
             "\n"
             "\n    shape : tuple of ints"
             "\n        Dimensions of the vec (as shape in numpy.ndarray). Currently only"
             " one-dimensional vecs are implemented."
             "\n"
             "\n    className: str"
             "\n         The class of the moose object this vec contains. MOOSE core"
             " implements its own class system independent of Python. pymoose creates"
             " thin wrappers around them. This field provides you the moose class"
             " name as defined in C++"
             "\n"
             "\n Constructor:"
             "\n"
             "\n    vec(self, path=path, n=size, g=isGlobal, dtype=className)"
             "\n    "
             "\n    "
             "\n    Parameters"
             "\n    ----------"
             "\n    path : str/vec/int "
             "\n        Path of an existing array element or for creating a new one. This has"
             "\n        the same format as unix file path: /{element1}/{element2} ... If there"
             "\n        is no object with the specified path, moose attempts to create a new"
             "\n        array element. For that to succeed everything until the last `/`"
             "\n        character must exist or an error is raised"
             "\n"
             "\n        Alternatively, path can be vec or integer value of the id of an"
             "\n        existing vec object. The new object will be another reference to"
             "\n        the existing object."
             "\n    "
             "\n    n : positive int"
             "\n        This is a positive integers specifying the size of the array element"
             "\n        to be created. Thus n=2 will create an"
             "\n        vec with 2 elements."
             "\n    "
             "\n    "
             "\n    g : int"
             "\n        Specify if this is a global or local element. Global elements are"
             "\n        shared between nodes in a computing cluster."
             "\n    "
             "\n    dtype: string"
             "\n        The vector will be of this moose-class."
             "\n    "
             "\n    "
             "\n    Examples"
             "\n    ---------"
             "\n        >>> iaf = moose.vec('/iaf', n=10, dtype='IntFire')"
             "\n        >>> iaf.Vm = range(10)"
             "\n        >>> print iaf[5].Vm"
             "\n        5.0"
             "\n        >>> print iaf.Vm"
             "\n        array([ 0.,  1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.])"
            );

PyTypeObject IdType =
{
    PyVarObject_HEAD_INIT(NULL, 0)               /* tp_head */
    "moose.vec",                  /* tp_name */
    sizeof(_Id),                    /* tp_basicsize */
    0,                                  /* tp_itemsize */
    0,                    /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    (reprfunc)moose_Id_repr,                        /* tp_repr */
    0,                                  /* tp_as_number */
    &IdSequenceMethods,             /* tp_as_sequence */
    &IdMappingMethods,              /* tp_as_mapping */
    (hashfunc)moose_Id_hash,                                  /* tp_hash */
    0,                                  /* tp_call */
    (reprfunc)moose_Id_str,               /* tp_str */
    (getattrofunc)moose_Id_getattro,            /* tp_getattro */
    (setattrofunc)moose_Id_setattro,            /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    moose_Id_doc,
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    (richcmpfunc)moose_Id_richCompare,       /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    IdMethods,                     /* tp_methods */
    0,                    /* tp_members */
    0,                                  /* tp_getset */
    0,                                 /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    (initproc) moose_Id_init,   /* tp_init */
    0,                /* tp_alloc */
    0,                  /* tp_new */
    0,                      /* tp_free */
};


extern PyTypeObject ObjIdType;

//////////////////////////////////////////////////
// Id functions
//////////////////////////////////////////////////

PyObject* get_Id_attr(_Id * id, string attribute)
{
    if (attribute == "path")
    {
        return moose_Id_getPath(id);
    }
    else if (attribute == "name")
    {
        string name = Field<string>::get(id->id_, "name");
        return Py_BuildValue("s", name.c_str());
    }
    else if (attribute == "value")
    {
        return moose_Id_getValue(id);
    }
    else if (attribute == "shape")
    {
        return moose_Id_getShape(id);
    }
    else if (attribute == "className")
    {
        // !NOTE: Subha: 2012-08-20 19:52:21 (+0530) - the second
        // !check is to catch a strange bug where the field passed
        // !to moose_Id_getattro is 'class' in stead of
        // !'class_'. Need to figure out how it is happening.
        // !Subha: 2012-08-21 13:25:06 (+0530) It turned out to be
        // !a GCC optimization issue. GCC was optimizing the call
        // !to get_field_alias by directly replacing `class_` with
        // !`class`. This optimized run-path was somehow being
        // !used only when therewas access to
        // !obj.parent.class_. Possibly some form of cache
        // !optimization.
        // class is a common attribute to all ObjIds under this
        // Id. Expect it to be a single value in stead of a list
        // of class names.

        string classname = Field<string>::get(id->id_, "className");
        return Py_BuildValue("s", classname.c_str());
    }
    return NULL;
}

/**
   Utility function to create objects from full path, dimensions
   and classname.
*/
Id create_Id_from_path(string path, unsigned int numData, unsigned int isGlobal, string type)
{
    string parent_path;
    string name;

    string trimmed_path = moose::trim( path );
    size_t pos = trimmed_path.rfind("/");
    if (pos != string::npos)
    {
        name = trimmed_path.substr(pos+1);
        parent_path = trimmed_path.substr(0, pos);
        //cerr << "Parent path is : " << parent_path << endl;
    }
    else
    {
        name = trimmed_path;
    }
    // handle relative path
    if (trimmed_path[0] != '/')
    {
        string current_path = SHELLPTR->getCwe().path();
        if (current_path != "/")
        {
            parent_path = current_path + "/" + parent_path;
        }
        else
        {
            parent_path = current_path + parent_path;
        }
    }
    else if (parent_path.empty())
    {
        parent_path = "/";
    }
    ObjId parent_id(parent_path);
    if (parent_id.bad() )
    {
        string message = "Parent element does not exist: ";
        message += parent_path;
        PyErr_SetString(PyExc_ValueError, message.c_str());
        return Id();
    }
    Id nId =  SHELLPTR->doCreate(type,
                                 parent_id,
                                 string(name),
                                 numData,
                                 static_cast< NodePolicy >( isGlobal )
                                );
    if (nId == Id() && trimmed_path != "/" && trimmed_path != "/root")
    {
        string message = "no such moose class : " + type;
        PyErr_SetString(PyExc_TypeError, message.c_str());

    }

    return nId;
}

int moose_Id_init(_Id * self, PyObject * args, PyObject * kwargs)
{
    extern PyTypeObject IdType;
    PyObject * src = NULL;
    unsigned int id = 0;
    unsigned int isGlobal = 0;
    char * type = NULL;
    // first try parsing the arguments as (path, size, classname)
    static char _path[] = "path";
    static char _dtype[] = "dtype";
    static char _size[] = "n";
    static char _global[] = "g";
    static char * kwlist[] = {_path, _size,  _global, _dtype, NULL};
    char * path = NULL;
    char _default_type[] = "Neutral";
    // set it to 0 for unspecified value in case the user tries to
    // get an existing id without specifying it. If the user is
    // specifying it but the existing vec does not have same
    // numData we warn the user about the misconception. If it is
    // nonexisting vec, we change the unspecifed numData to 1.
    unsigned int numData = 0;
    // char *type = _default_type;
    /* The expected arguments are:
       string path,
       [unsigned int numData] - default: 1
       [unsigned int isGlobal] - default: 0
       [string type] - default: Neutral
    */
    if (PyArg_ParseTupleAndKeywords(args,
                                    kwargs,
                                    "s|IIs:moose_Id_init",
                                    kwlist,
                                    &path,
                                    &numData,
                                    &isGlobal,
                                    &type))
    {
        // Parsing args successful, if any error happens now,
        // different argument processing will not help. Return error
        string trimmed_path(path);
        trimmed_path = moose::trim(trimmed_path);
        size_t length = trimmed_path.length();
        if (length <= 0)
        {
            PyErr_SetString(PyExc_ValueError,
                            "moose_Id_init: path must be non-empty string.");
            return -1;
        }
        self->id_ = Id(trimmed_path);
        // Return already existing object
        if (self->id_ != Id() ||
                trimmed_path == "/" ||
                trimmed_path == "/root")
        {
            if ((numData > 0) && (numData != Field<unsigned int>::get(self->id_, "numData")))
            {
                PyErr_WarnEx(NULL, "moose_Id_init_: Length specified does not match that of existing object.", 1);
            }
            return 0;
        }
        if (type == NULL)
        {
            type = _default_type;
        }
        if (numData <= 0)
        {
            numData = 1;
        }
        self->id_ = create_Id_from_path(trimmed_path, numData, isGlobal, type);
        if (self->id_ == Id() && PyErr_Occurred())
        {
            return -1;
        }
        return 0;
    }
    // The arguments could not be parsed as (path, dims, class),
    // try to parse it as an existing Id
    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O:moose_Id_init", &src) && Id_SubtypeCheck(src))
    {
        self->id_ = ((_Id*)src)->id_;
        return 0;
    }
    // The arguments could not be parsed as (path, dims, class), or Id
    // try to parse it as an existing ObjId
    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O:moose_Id_init", &src) && ObjId_SubtypeCheck(src))
    {
        self->id_ = ((_ObjId*)src)->oid_.id;
        return 0;
    }
    // Next try to parse it as an integer value for an existing Id
    PyErr_Clear(); // clear the error from parsing error
    if (PyArg_ParseTuple(args, "I:moose_Id_init", &id))
    {
        self->id_ = Id(id);
        return 0;
    }
    return -1;
}// ! moose_Id_init

long moose_Id_hash(_Id * self)
{
    return self->id_.value(); // hash is the same as the Id value
}


// 2011-03-23 15:14:11 (+0530)
// 2011-03-26 17:02:19 (+0530)
//
// 2011-03-26 19:14:34 (+0530) - This IS UGLY! Destroying one
// ObjId will destroy the containing element and invalidate all
// the other ObjId with the same Id.
// 2011-03-28 13:44:49 (+0530)
PyObject * deleteObjId(ObjId oid)
{
#ifndef NDEBUG
    if (verbosity > 1)
    {
        cout << "Deleting ObjId " << oid << endl;
    }
#endif
    SHELLPTR->doDelete(oid);
    Py_RETURN_NONE;
}

PyObject * moose_Id_delete(_Id * self)
{
    if (self->id_ == Id())
    {
        PyErr_SetString(PyExc_ValueError, "moose_Id_delete: cannot delete moose shell.");
        return NULL;
    }
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_delete");
    }
    deleteObjId(self->id_);
    self->id_ = Id();
    Py_CLEAR(self);
    Py_RETURN_NONE;
}

PyObject * moose_Id_repr(_Id * self)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_repr");
    }
    ostringstream repr;
    repr << "<moose.vec: class="
         << Field<string>::get(self->id_, "className") << ", "
         << "id=" << self->id_.value() << ", "
         << "path=" << self->id_.path() << ">";
    return PyString_FromString(repr.str().c_str());
} // !  moose_Id_repr

// The string representation is unused. repr is used everywhere.
PyObject * moose_Id_str(_Id * self)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_str");
    }
    return PyString_FromFormat("<moose.vec: class=%s, id=%u, path=%s>",
                               Field<string>::get(self->id_, "className").c_str(),
                               self->id_.value(), self->id_.path().c_str());
} // !  moose_Id_str

// 2011-03-23 15:09:19 (+0530)
PyObject* moose_Id_getValue(_Id * self)
{
    unsigned int id = self->id_.value();
    PyObject * ret = Py_BuildValue("I", id);
    return ret;
}

/**
   Not to be redone. 2011-03-23 14:42:48 (+0530)
*/
PyObject * moose_Id_getPath(_Id * self)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_getPath");
    }
    string path = self->id_.path();
    string default_end("[0]");
    if (moose::endswith(path, default_end))
    {
        path.erase(path.length() - default_end.length(), default_end.length());
    }
    PyObject * ret = Py_BuildValue("s", path.c_str());
    return ret;
}

////////////////////////////////////////////
// Subset of sequence protocol functions
////////////////////////////////////////////
Py_ssize_t moose_Id_getLength(_Id * self)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(-1, "moose_Id_getLength");
    }
    if (self->id_.element()->hasFields())
    {
        return (Py_ssize_t)(Field < unsigned int >::get(self->id_, "numField"));
    }
    else
    {
        return (Py_ssize_t)(self->id_.element()->numData());
    }
}

PyObject * moose_Id_getShape(_Id * self)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_getShape");
    }
    unsigned int numData = 1;
    if (self->id_.element()->hasFields())
    {
        numData = Field < unsigned int >::get(self->id_, "numField");
    }
    else
    {
        //            numData = Field < unsigned int >::get(self->id_, "numData");
        numData = self->id_.element()->numData();
    }
    PyObject * ret = PyTuple_New((Py_ssize_t)1);
    if (PyTuple_SetItem(ret, (Py_ssize_t)0, Py_BuildValue("I", numData)))
    {
        Py_XDECREF(ret);
        PyErr_SetString(PyExc_RuntimeError, "moose_Id_getShape: could not set tuple entry.");
        return NULL;
    }
    return ret;
}

PyObject * moose_Id_getItem(_Id * self, Py_ssize_t index)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_getItem");
    }
    if (index < 0)
    {
        index += moose_Id_getLength(self);
    }
    if ((index < 0) || (index >= moose_Id_getLength(self)))
    {
        PyErr_SetString(PyExc_IndexError, "index out of bounds.");
        return NULL;
    }
    ObjId oid(self->id_.path()); // This is just to get the dataIndex of parent
    if (self->id_.element()->hasFields())
    {
        // How to efficiently get the dataIndex of parent element
        // without creating ObjId from path?
        oid = ObjId(self->id_, oid.dataIndex, index);
    }
    else
    {
        oid = ObjId(self->id_, index, 0);
    }
    return oid_to_element(oid);
}

static PyObject* moose_Id_fillSlice(_Id *self,
                                    Py_ssize_t start,
                                    Py_ssize_t end,
                                    Py_ssize_t step,
                                    Py_ssize_t slicelength)
{

    PyObject * ret = PyTuple_New(slicelength);
    bool has_fields = self->id_.element()->hasFields();
    for (int ii = start; ii < end; ii += step)
    {
        ObjId oid(self->id_.path());
        PyObject *value;
        if (has_fields)
            value = oid_to_element(ObjId(self->id_, oid.dataIndex, ii));
        else
            value = oid_to_element(ObjId(self->id_, ii));

        PyTuple_SET_ITEM(ret, (Py_ssize_t)(ii-start)/step, value);
    }
    return ret;
}

#ifndef PY3K
PyObject * moose_Id_getSlice(_Id * self, Py_ssize_t start, Py_ssize_t end)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_getSlice");
    }
    Py_ssize_t len = moose_Id_getLength(self);
    while (start < 0)
    {
        start += len;
    }
    while (end < 0)
    {
        end += len;
    }
    return moose_Id_fillSlice(self, start, end, 1, std::max(end - start, (Py_ssize_t) 0));
}
#endif

#ifdef PY3K
#  define SLICE_OBJ(x) (x)
#else
#  define SLICE_OBJ(x) ((PySliceObject*)(x))
#endif

///////////////////////////////////////////////////
// Mapping protocol
///////////////////////////////////////////////////
PyObject * moose_Id_subscript(_Id * self, PyObject *op)
{
    if (PySlice_Check(op))
    {
        const Py_ssize_t len = moose_Id_getLength(self);
        Py_ssize_t start, stop, step, slicelength;

        if (PySlice_GetIndicesEx(SLICE_OBJ(op), len, &start, &stop, &step, &slicelength) < 0)
            return NULL;

        return moose_Id_fillSlice(self, start, stop, step, slicelength);
    }

    if (PyInt_Check(op) || PyLong_Check(op))
    {
        Py_ssize_t value = PyInt_AsLong(op);
        return moose_Id_getItem(self, value);
    }
    else
    {
        PyErr_SetString(PyExc_KeyError, "moose_Id_subscript: invalid index.");
        return NULL;
    }
}

PyObject * moose_Id_richCompare(_Id * self, PyObject * other, int op)
{
    extern PyTypeObject IdType;
    bool ret = false;
    Id other_id = ((_Id*)other)->id_;
    if (!self || !other)
    {
        ret = false;
    }
    else if (!PyObject_IsInstance(other, (PyObject*)&IdType))
    {
        ret = false;
    }
    else if (op == Py_EQ)
    {
        ret = self->id_ == other_id;
    }
    else if (op == Py_NE)
    {
        ret = self->id_ != other_id;
    }
    else if (op == Py_LT)
    {
        ret = self->id_ < other_id;
    }
    else if (op == Py_GT)
    {
        ret = other_id < self->id_;
    }
    else if (op == Py_LE)
    {
        ret = (self->id_ < other_id) || (self->id_ == other_id);
    }
    else if (op == Py_GE)
    {
        ret = (other_id < self->id_) || (self->id_ == other_id);
    }
    if (ret)
    {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

int moose_Id_contains(_Id * self, PyObject * obj)
{
    extern PyTypeObject ObjIdType;
    int ret = 0;
    if (ObjId_SubtypeCheck(obj))
    {
        ret = (((_ObjId*)obj)->oid_.id == self->id_);
    }
    return ret;
}

PyObject * moose_Id_getattro(_Id * self, PyObject * attr)
{
    int new_attr = 0;
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_getattro");
    }
    char * field = PyString_AsString(attr);
    PyObject * _ret = get_Id_attr(self, field);
    if (_ret != NULL)
    {
        return _ret;
    }
    string className = Field<string>::get(self->id_, "className");
    string type = getFieldType(className, string(field));
    if (type.empty())
    {
        // Check if this field name is aliased and update fieldname and type if so.
        map<string, string>::const_iterator it = get_field_alias().find(string(field));
        if (it != get_field_alias().end())
        {
            field = const_cast<char*>((it->second).c_str());
            type = getFieldType(Field<string>::get(self->id_, "className"), it->second);
            // Update attr for next level (PyObject_GenericGetAttr) in case.
            // Py_XDECREF(attr);
            attr = PyString_FromString(field);
            new_attr = 1;
        }
    }
    if (type.empty())
    {
        return PyObject_GenericGetAttr((PyObject*)self, attr);
    }
    char ftype = shortType(type);
    if (!ftype)
    {
        return PyObject_GenericGetAttr((PyObject*)self, attr);
    }

    switch (ftype)
    {
    case 'd':
    {
        vector < double > val;
        Field< double >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 's':
    {
        vector < string > val;
        Field< string >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'l':
    {
        vector < long > val;
        Field< long >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'x':
    {
        vector < Id > val;
        Field< Id >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'y':
    {
        vector < ObjId > val;
        Field< ObjId >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'i':
    {
        vector < int > val;
        Field< int >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'I':
    {
        vector < unsigned int > val;
        Field< unsigned int >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'k':
    {
        vector < unsigned long > val;
        Field< unsigned long >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'f':
    {
        vector < float > val;
        Field< float >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'b':
    {
        vector<bool> val;
        Field< bool >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'c':
    {
        vector < char > val;
        Field< char >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'h':
    {
        vector < short > val;
        Field< short >::getVec(self->id_, string(field), val);
        _ret = to_pytuple(&val, ftype);
        break;
    }
    case 'z':
    {
        PyErr_SetString(PyExc_NotImplementedError,
                        "moose_Id_getattro: DataId handling not implemented yet.");
        _ret = NULL;
        break;
    }
    default:
        ostringstream msg;
        msg << "moose_Id_getattro: unhandled field type '" << type << "'\n"
            << "This is a vec object. Perhaps you are trying to access the field in an"
            << " element in this. Then use indexing to get the element first.";
        PyErr_SetString(PyExc_ValueError, msg.str().c_str());
        _ret = NULL;
        break;
    }
    if (new_attr)
    {
        Py_DECREF(attr);
    }
    return _ret;
}

PyObject * moose_Id_setField(_Id * self, PyObject * args)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(NULL, "moose_Id_setField");
    }
    PyObject * field = NULL;
    PyObject * value = NULL;
    if (!PyArg_ParseTuple(args, "OO:moose_Id_setField", &field, &value))
    {
        return NULL;
    }
    if (moose_Id_setattro(self, field, value) == -1)
    {
        return NULL;
    }
    Py_RETURN_NONE;
}

int moose_Id_setattro(_Id * self, PyObject * attr, PyObject *value)
{
    if (!Id::isValid(self->id_))
    {
        RAISE_INVALID_ID(-1, "moose_Id_setattro");
    }
    char * fieldname = NULL;
    int ret = -1;
    if (PyString_Check(attr))
    {
        fieldname = PyString_AsString(attr);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "moose_Id_setattro: Attribute name must be a string");
        return -1;
    }
    string moose_class = Field<string>::get(self->id_, "className");
    string fieldtype = getFieldType(moose_class, string(fieldname));
    if (fieldtype.length() == 0)
    {
        // If it is instance of a MOOSE Id then throw
        // error (to avoid silently creating new attributes due to
        // typos). Otherwise, it must have been subclassed in
        // Python. Then we allow normal Pythonic behaviour and
        // consider such mistakes user's responsibility.
        string className = ((PyTypeObject*)PyObject_Type((PyObject*)self))->tp_name;
        if (className != "vec")
        {
            Py_INCREF(attr);
            ret = PyObject_GenericSetAttr((PyObject*)self, attr, value);
            Py_XDECREF(attr);
            return ret;
        }
        ostringstream msg;
        msg << "moose_Id_setattro: '" << moose_class << "' class has no field '" << fieldname << "'" << endl;
        PyErr_SetString(PyExc_AttributeError, msg.str().c_str());
        return -1;
    }
    char ftype = shortType(fieldtype);
    Py_ssize_t length = moose_Id_getLength(self);
    bool is_seq = true;
    if (!PySequence_Check(value))
    {
        is_seq = false;
    }
    else if (length != PySequence_Length(value))
    {
        PyErr_SetString(PyExc_IndexError,
                        "moose_Id_setattro: length of the sequence on the right hand side does not match Id size.");
        return -1;
    }
    switch(ftype)
    {
    case 'd':  //SET_VECFIELD(double, d)
    {
        vector<double> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                double v = PyFloat_AsDouble(vo);
                Py_XDECREF(vo);
                _value.push_back(v);
            }
        }
        else
        {
            double v = PyFloat_AsDouble(value);
            _value.assign(length, v);
        }
        ret = Field<double>::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 's':
    {
        vector<string> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                char * v = PyString_AsString(vo);
                Py_XDECREF(v);
                _value.push_back(string(v));
            }
        }
        else
        {
            char * v = PyString_AsString(value);
            _value.assign(length, string(v));
        }
        ret = Field<string>::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'i':
    {
        vector<int> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                int v = PyInt_AsLong(vo);
                Py_XDECREF(vo);
                _value.push_back(v);
            }
        }
        else
        {
            int v = PyInt_AsLong(value);
            _value.assign(length, v);
        }
        ret = Field< int >::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'I':  //SET_VECFIELD(unsigned int, I)
    {
        vector<unsigned int> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                unsigned int v = PyInt_AsUnsignedLongMask(vo);
                Py_DECREF(vo);
                _value.push_back(v);
            }
        }
        else
        {
            unsigned int v = PyInt_AsUnsignedLongMask(value);
            _value.assign(length, v);
        }
        ret = Field< unsigned int >::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'l':  //SET_VECFIELD(long, l)
    {
        vector<long> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                long v = PyInt_AsLong(vo);
                Py_DECREF(vo);
                _value.push_back(v);
            }
        }
        else
        {
            long v = PyInt_AsLong(value);
            _value.assign(length, v);
        }
        ret = Field<long>::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'k':  //SET_VECFIELD(unsigned long, k)
    {
        vector<unsigned long> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                unsigned long v = PyInt_AsUnsignedLongMask(vo);
                Py_XDECREF(vo);
                _value.push_back(v);
            }
        }
        else
        {
            unsigned long v = PyInt_AsUnsignedLongMask(value);
            _value.assign(length, v);
        }
        ret = Field< unsigned long >::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'b':
    {
        vector<bool> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * _v = PySequence_GetItem(value, ii);
                bool v = (Py_True ==_v) || (PyInt_AsLong(_v) != 0);
                Py_XDECREF(_v);
                _value.push_back(v);
            }
        }
        else
        {
            bool v = (Py_True ==value) || (PyInt_AsLong(value) != 0);
            _value.assign(length, v);
        }
        ret = Field< bool >::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'c':
    {
        vector<char> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * _v = PySequence_GetItem(value, ii);
                char * v = PyString_AsString(_v);
                Py_XDECREF(_v);
                if (v && v[0])
                {
                    _value.push_back(v[0]);
                }
                else
                {
                    ostringstream err;
                    err << "moose_Id_setattro:" << ii << "-th element is NUL";
                    PyErr_SetString(PyExc_ValueError, err.str().c_str());
                    return -1;
                }
            }
        }
        else
        {
            char * v = PyString_AsString(value);
            if (v && v[0])
            {
                _value.assign(length, v[0]);
            }
            else
            {
                PyErr_SetString(PyExc_ValueError,  "moose_Id_setattro: value is an empty string");
                return -1;
            }
        }
        ret = Field< char >::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'h':
    {
        vector<short> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                short v = PyInt_AsLong(vo);
                Py_XDECREF(vo);
                _value.push_back(v);
            }
        }
        else
        {
            short v = PyInt_AsLong(value);
            _value.assign(length, v);
        }
        ret = Field< short >::setVec(self->id_, string(fieldname), _value);
        break;
    }
    case 'f':  //SET_VECFIELD(float, f)
    {
        vector<float> _value;
        if (is_seq)
        {
            for ( int ii = 0; ii < length; ++ii)
            {
                PyObject * vo = PySequence_GetItem(value, ii);
                float v = PyFloat_AsDouble(vo);
                Py_XDECREF(vo);
                _value.push_back(v);
            }
        }
        else
        {
            float v = PyFloat_AsDouble(value);
            _value.assign(length, v);
        }
        ret = Field<float>::setVec(self->id_, string(fieldname), _value);
        break;
    }
    default:
        break;
    }
    // MOOSE Field::set returns 1 for success 0 for
    // failure. Python treats return value 0 from setters as
    // success, anything else failure.
    if (ret && (PyErr_Occurred() == NULL))
    {
        return 0;
    }
    else
    {
        return -1;
    }

}


//
// vec.cpp ends here
