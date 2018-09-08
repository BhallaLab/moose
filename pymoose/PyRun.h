// Filename: PyRun.h
// Description:
// Author: subha
// Created: Sat Oct 11 14:40:45 2014 (+0530)

#ifndef _PYCALL_H
#define _PYCALL_H

#include <climits>

#if PY_MAJOR_VERSION >= 3
#define PYCODEOBJECT PyObject

string get_program_name()
{
    wchar_t * progname = Py_GetProgramName();
    char buffer[PATH_MAX+1];
    size_t ret = wcstombs(buffer, progname, PATH_MAX);
    buffer[ret] = '\0';
    return string(buffer);
}
#else
#define PYCODEOBJECT PyCodeObject

string get_program_name()
{
    char * progname = Py_GetProgramName();
    return string(progname);
}
#endif

/**
   PyRun allows caling Python functions from moose.
 */
class PyRun
{
public:
    static const int RUNPROC; // only process call
    static const int RUNTRIG; // only trigger call
    static const int RUNBOTH; // both

    PyRun();
    ~PyRun();

    void setInitString(string str);
    string getInitString() const;

    void setRunString(string str);
    string getRunString() const;

    void setGlobals(PyObject *globals);
    PyObject * getGlobals() const;

    void setLocals(PyObject *locals);
    PyObject * getLocals() const;

    void setMode(int flag);
    int getMode() const;

    void setInputVar(string name);
    string getInputVar() const;

    void setOutputVar(string name);
    string getOutputVar() const;

    void run(const Eref& e, string statement);

     // this is a way to trigger execution via incoming message - can be useful for debugging
    void trigger(const Eref& e, double input);

    void process(const Eref& e, ProcPtr p);
    void reinit(const Eref& e, ProcPtr p);

    static const Cinfo * initCinfo();

protected:
    int mode_;                    // flag to decide when to run the Python string
    string initstr_;              // statement str for running at reinit
    string runstr_;               // statement str for running in each process call
    PyObject * globals_;          // global env dict
    PyObject * locals_;           // local env dict
    PYCODEOBJECT * runcompiled_;  // compiled form of procstr_
    PYCODEOBJECT * initcompiled_; // coimpiled form of initstr_
    string inputvar_;             // identifier for input variable.
    string outputvar_;            // identifier for output variable
};

#endif
