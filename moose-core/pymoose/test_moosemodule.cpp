// test_moosemodule.cpp ---
//
// Filename: test_moosemodule.cpp
// Description:
// Author:
// Maintainer:
// Created: Tue Jul 23 11:37:57 2013 (+0530)
// Version:
// Last-Updated: Thu Jul 25 21:54:10 2013 (+0530)
//           By: subha
//     Update #: 85
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

//
// test_moosemodule.cpp ends here
