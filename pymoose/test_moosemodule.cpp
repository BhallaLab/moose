// test_moosemodule.cpp ---
//
// Description:
// Author: Subha
// Maintainer: Dilawar Singh
// Created: Tue Jul 23 11:37:57 2013 (+0530)

#include "Python.h"
#include <iostream>
#include <cstring>
#include "../basecode/header.h"
#include "../basecode/Id.h"
#include "../basecode/ObjId.h"
#include "../shell/Shell.h"

#include "moosemodule.h"
#include "../utility/utility.h"
using namespace std;

extern "C" {

    void test_to_py()
    {
#ifdef DO_UNIT_TESTS
        // conversion of double
        PyObject * pv;
        double dv = 10.0;
        pv = to_py((void*)&dv, 'd');
        assert(pv != NULL);
        assert(PyFloat_AsDouble(pv) == dv);
        Py_XDECREF(pv);
        cout << "." << flush;

        // conversion of long
        long lv = 1000000000;
        pv = to_py((void*)&lv, 'l');
        assert(pv != NULL);
        assert(PyLong_AsLong(pv) == lv);
        Py_XDECREF(pv);
        cout << "." << flush;

        // conversion of int
        int iv = 10;
        pv = to_py((void*)&iv, 'i');
        assert(pv != NULL);
        assert(PyInt_AsLong(pv) == iv);
        Py_XDECREF(pv);
        cout << "." << flush;

        // conversion of float
        float fv = 7e-3;
        pv = to_py((void*)&fv, 'f');
        assert(pv != NULL);
        assert(PyFloat_AsDouble(pv) == fv);
        Py_XDECREF(pv);
        cout << "." << flush;

        // string char-array
        string sv = "hello world";
        // C++ string
        pv = to_py((void*)&sv, 's');
        assert(pv != NULL);
        assert(strcmp(PyString_AsString(pv), sv.c_str()) == 0);
        Py_XDECREF(pv);
        cout << "." << flush;

        // Id
        Shell * shell = reinterpret_cast< Shell * >(ObjId( Id(), 0).data());
        Id id = shell->doCreate("Neutral", Id(), "n", 1);
        pv = to_py((void*)&id, 'x');
        assert(pv != NULL);
        assert(((_Id*)pv)->id_ == id);
        Py_XDECREF(pv);
        cout << "." << flush;

        // ObjId
        ObjId oid(id, 0);
        pv = to_py((void*)&oid, 'y');
        assert(pv != NULL);
        assert(((_ObjId*)pv)->oid_.id == oid.id);
	//Harsha: commeted this line to compile moose in debug mode
        //assert(((_ObjId*)pv)->oid_.dataId == oid.dataId);
        Py_XDECREF(pv);
        cout << "." << flush;

        // Incorrect type code
        pv = to_py((void*)&oid, '9');
        assert(pv == NULL);
        PyErr_Clear();
        cout << "." << flush;
        shell->doDelete(id);

#endif
    }

} // extern "C"

void test_moosemodule()
{
    test_to_py();
}
