// PyRun.cpp ---
//
// Filename: PyRun.cpp
// Description:
// Author: subha
// Maintainer:
// Created: Sat Oct 11 14:47:22 2014 (+0530)
// Version:
// Last-Updated: Fri Jun 19 18:56:06 2015 (-0400)
//           By: Subhasis Ray
//     Update #: 15
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
#include "../basecode/header.h"
#include "PyRun.h"

const int PyRun::RUNPROC = 1;
const int PyRun::RUNTRIG = 2;
const int PyRun::RUNBOTH = 0;

static SrcFinfo1< double >* outputOut()
{
    static SrcFinfo1< double > outputOut( "output",
                                          "Sends out the value of local variable called `output`. Thus, you can"
                                          " have Python statements which compute some value and assign it to the"
                                          " variable called `output` (which is defined at `reinit` call). This"
                                          " will be sent out to any target connected to the `output` field.");
    return &outputOut;
}


const Cinfo * PyRun::initCinfo()
{
    static ValueFinfo< PyRun, string > runstring(
        "runString",
        "String to be executed at each time step.",
        &PyRun::setRunString,
        &PyRun::getRunString);

    static ValueFinfo< PyRun, string > initstring(
        "initString",
        "String to be executed at initialization (reinit).",
        &PyRun::setInitString,
        &PyRun::getInitString);

    static ValueFinfo< PyRun, string > inputvar(
        "inputVar",
        "Name of local variable in which input balue is to be stored. Default"
        " is `input_` (to avoid conflict with Python's builtin function"
        " `input`).",
        &PyRun::setInputVar,
        &PyRun::getInputVar);

    static ValueFinfo< PyRun, string > outputvar(
        "outputVar",
        "Name of local variable for storing output. Default is `output`.",
        &PyRun::setOutputVar,
        &PyRun::getOutputVar);

    static ValueFinfo< PyRun, int > mode(
        "mode",
        "Flag to indicate whether runString should be executed for both trigger and process, or one of them.",
        &PyRun::setMode,
        &PyRun::getMode);

    // static ValueFinfo< PyRun, PyObject* > globals(
    //     "globals",
    //     "Global environment dict",
    //     &PyRun::setGlobals,
    //     &PyRun::getGlobals);

    // static ValueFinfo< PyRun, PyObject* > locals(
    //     "locals",
    //     "Local environment dict",
    //     &PyRun::setLocals,
    //     &PyRun::getLocals);

    static DestFinfo trigger(
        "trigger",
        "Executes the current runString whenever a message arrives. It stores"
        " the incoming value in local variable named"
        " `input_`, which can be used in the"
        " `runString` (the underscore is added to avoid conflict with Python's"
        " builtin function `input`). If debug is True, it prints the input"
        " value.",
        new EpFunc1< PyRun, double >(&PyRun::trigger));

    static DestFinfo run(
        "run",
        "Runs a specified string. Does not modify existing run or init strings.",
        new EpFunc1< PyRun, string >(&PyRun::run));

    static DestFinfo process(
        "process",
        "Handles process call. Runs the current runString.",
        new ProcOpFunc< PyRun >(&PyRun::process));

    static DestFinfo reinit(
        "reinit",
        "Handles reinit call. Runs the current initString.",
        new ProcOpFunc< PyRun >( &PyRun::reinit ));

    static Finfo * processShared[] = { &process, &reinit };
    static SharedFinfo proc(
        "proc",
        "This is a shared message to receive Process messages "
        "from the scheduler objects."
        "The first entry in the shared msg is a MsgDest "
        "for the Process operation. It has a single argument, "
        "ProcInfo, which holds lots of information about current "
        "time, thread, dt and so on. The second entry is a MsgDest "
        "for the Reinit operation. It also uses ProcInfo. ",
        processShared, sizeof( processShared ) / sizeof( Finfo* ));

    static Finfo * pyRunFinfos[] = {
        &runstring,
        &initstring,
        &mode,
        &inputvar,
        &outputvar,
        &trigger,
        outputOut(),
        // &locals,
        // &globals,
        &run,
        &proc,
    };

    static string doc[] = {
        "Name", "PyRun",
        "Author", "Subhasis Ray",
        "Description", "Runs Python statements from inside MOOSE."};
    static Dinfo< PyRun > dinfo;
    static Cinfo pyRunCinfo(
        "PyRun",
        Neutral::initCinfo(),
        pyRunFinfos,
        sizeof(pyRunFinfos) / sizeof(Finfo*),
        &dinfo,
        doc,
        sizeof(doc) / sizeof(string));
    return &pyRunCinfo;
}

static const Cinfo * pyRunCinfo = PyRun::initCinfo();

PyRun::PyRun():mode_(0), initstr_(""), runstr_(""),
               globals_(0), locals_(0),
               runcompiled_(0), initcompiled_(0),
               inputvar_("input_"), outputvar_("output")
{
    locals_ = PyDict_New();
    if (!locals_){
        cerr << "Could not initialize locals dict" << endl;
        return;
    }
    PyObject * value = PyFloat_FromDouble(0.0);
    if (!value && PyErr_Occurred()){
        PyErr_Print();
        return;
    }
    if (PyDict_SetItemString(locals_, inputvar_.c_str(), value)){
        PyErr_Print();
    }
}

PyRun::~PyRun()
{
    Py_XDECREF(globals_);
    Py_XDECREF(locals_);
}

void PyRun::setRunString(string statement)
{
    runstr_ = statement;
}

string PyRun::getRunString() const
{
    return runstr_;
}

void PyRun::setInitString(string statement)
{
    initstr_ = statement;
}

string PyRun::getInitString() const
{
    return initstr_;
}

void PyRun::setInputVar(string name)
{
    PyDict_DelItemString(locals_, inputvar_.c_str());
    inputvar_ = name;
}

string PyRun::getInputVar() const
{
    return inputvar_;
}

void PyRun::setOutputVar(string name)
{
    PyDict_DelItemString(locals_, outputvar_.c_str());
    outputvar_ = name;
}

string PyRun::getOutputVar() const
{
    return outputvar_;
}

void PyRun::setMode(int flag)
{
    mode_ = flag;
}

int PyRun::getMode() const
{
    return mode_;
}

void PyRun::trigger(const Eref& e, double input)
{
    if (!runcompiled_){
        return;
    }
    if (mode_ == 1){
        return;
    }

    PyObject * value = PyDict_GetItemString(locals_, inputvar_.c_str());
    if (value){
        Py_DECREF(value);
    }
    value = PyFloat_FromDouble(input);
    if (!value && PyErr_Occurred()){
        PyErr_Print();
    }
    if (PyDict_SetItemString(locals_, inputvar_.c_str(), value)){
        PyErr_Print();
    }
    PyEval_EvalCode(runcompiled_, globals_, locals_);
    if (PyErr_Occurred()){
        PyErr_Print ();
    }
    value = PyDict_GetItemString(locals_, outputvar_.c_str());
    if (value){
        double output = PyFloat_AsDouble(value);
        if (PyErr_Occurred()){
            PyErr_Print ();
        } else {
            outputOut()->send(e, output);
        }
    }
}

void PyRun::run(const Eref&e, string statement)
{
    PyRun_SimpleString(statement.c_str());
    PyObject * value = PyDict_GetItemString(locals_, outputvar_.c_str());
    if (value){
        double output = PyFloat_AsDouble(value);
        if (PyErr_Occurred()){
            PyErr_Print ();
        } else {
            outputOut()->send(e, output);
        }
    }
}

void PyRun::process(const Eref & e, ProcPtr p)
{
    // PyRun_String(runstr_.c_str(), 0, globals_, locals_);
    // PyRun_SimpleString(runstr_.c_str());
    if (!runcompiled_ || mode_ == 2){
        return;
    }
    PyEval_EvalCode(runcompiled_, globals_, locals_);
    if (PyErr_Occurred()){
        PyErr_Print ();
    }
    PyObject * value = PyDict_GetItemString(locals_, outputvar_.c_str());
    if (value){
        double output = PyFloat_AsDouble(value);
        if (PyErr_Occurred()){
            PyErr_Print ();
        } else {
            outputOut()->send(e, output);
        }
    }
}

/**
   This is derived from:
   http://effbot.org/pyfaq/how-do-i-tell-incomplete-input-from-invalid-input.htm
 */
void handleError(bool syntax)
{
    PyObject *exc, *val, *trb;
    char * msg;

    if (syntax && PyErr_ExceptionMatches (PyExc_SyntaxError)){
        PyErr_Fetch (&exc, &val, &trb);        /* clears exception! */

        if (PyArg_ParseTuple (val, "sO", &msg, &trb) &&
            !strcmp (msg, "unexpected EOF while parsing")){ /* E_EOF */
            Py_XDECREF (exc);
            Py_XDECREF (val);
            Py_XDECREF (trb);
        } else {                                  /* some other syntax error */
            PyErr_Restore (exc, val, trb);
            PyErr_Print ();
        }
    } else {                                     /* some non-syntax error */
        PyErr_Print ();
    }
}

void PyRun::reinit(const Eref& e, ProcPtr p)
{
    PyObject * main_module;
    if (globals_ == NULL){
        main_module = PyImport_AddModule("__main__");
        globals_ = PyModule_GetDict(main_module);
        Py_XINCREF(globals_);
    }
    if (locals_ == NULL){
        locals_ = PyDict_New();
        if (!locals_){
            cerr << "Could not initialize locals dict" << endl;
        }
    }
    initcompiled_ = (PYCODEOBJECT*)Py_CompileString(
        initstr_.c_str(),
        get_program_name().c_str(),
        Py_file_input);
    if (!initcompiled_){
        cerr << "Error compiling initString" << endl;
        handleError(true);
    } else {
        PyEval_EvalCode(initcompiled_, globals_, locals_);
        if (PyErr_Occurred()){
            PyErr_Print ();
        }
    }
    runcompiled_ = (PYCODEOBJECT*)Py_CompileString(
        runstr_.c_str(),
        get_program_name().c_str(),
        Py_file_input);
    if (!runcompiled_){
        cerr << "Error compiling runString" << endl;
        handleError(true);
    } else {
        PyEval_EvalCode(runcompiled_, globals_, locals_);
        if (PyErr_Occurred()){
            PyErr_Print ();
        }
    }
}


//
// PyRun.cpp ends here
