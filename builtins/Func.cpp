// Func.cpp ---
//
// Filename: Func.cpp
// Description: Implementation of a wrapper around GNU libmatheval to calculate arbitrary functions.
// Author: Subhasis Ray
// Maintainer: Subhasis Ray
// Created: Sat May 25 16:35:17 2013 (+0530)
// Version:
// Last-Updated: Tue Jun 11 16:49:01 2013 (+0530)
//           By: subha
//     Update #: 619
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
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 3, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
//
//

// Code:

#include "header.h"
#include "../utility/utility.h"
#include "../utility/numutil.h"
#include "Func.h"

static SrcFinfo1<double> *valueOut()
{
    static SrcFinfo1<double> valueOut("valueOut",
                                      "Evaluated value of the function for the current variable values.");
    return &valueOut;
}

static SrcFinfo1< double > *derivativeOut()
{
    static SrcFinfo1< double > derivativeOut("derivativeOut",
            "Value of derivative of the function for the current variable values");
    return &derivativeOut;
}

const Cinfo * Func::initCinfo()
{
    ////////////////////////////////////////////////////////////
    // Value fields
    ////////////////////////////////////////////////////////////
    static  ReadOnlyValueFinfo< Func, double > value("value",
            "Result of the function evaluation with current variable values.",
            &Func::getValue);
    static ReadOnlyValueFinfo< Func, double > derivative("derivative",
            "Derivative of the function at given variable values.",
            &Func::getDerivative);
    static ValueFinfo< Func, unsigned int > mode("mode",
            "Mode of operation: \n"
            " 1: only the function value will be calculated\n"
            " 2: only the derivative will be calculated\n"
            " 3: both function value and derivative at current variable values will be calculated.",
            &Func::setMode,
            &Func::getMode);
    static ValueFinfo< Func, string > expr("expr",
                                           "Mathematical expression defining the function. The underlying parser\n"
                                           "is muParser. In addition to the available functions and operators  from\n"
                                           "muParser, some more functions are added.\n"
                                           "\nFunctions\n"
                                           "Name        args    explanation\n"
                                           "sin         1       sine function\n"
                                           "cos         1       cosine function\n"
                                           "tan         1       tangens function\n"
                                           "asin        1       arcus sine function\n"
                                           "acos        1       arcus cosine function\n"
                                           "atan        1       arcus tangens function\n"
                                           "sinh        1       hyperbolic sine function\n"
                                           "cosh        1       hyperbolic cosine\n"
                                           "tanh        1       hyperbolic tangens function\n"
                                           "asinh       1       hyperbolic arcus sine function\n"
                                           "acosh       1       hyperbolic arcus tangens function\n"
                                           "atanh       1       hyperbolic arcur tangens function\n"
                                           "log2        1       logarithm to the base 2\n"
                                           "log10       1       logarithm to the base 10\n"
                                           "log         1       logarithm to the base 10\n"
                                           "ln          1       logarithm to base e (2.71828...)\n"
                                           "exp         1       e raised to the power of x\n"
                                           "sqrt        1       square root of a value\n"
                                           "sign        1       sign function -1 if x<0; 1 if x>0\n"
                                           "rint        1       round to nearest integer\n"
                                           "abs         1       absolute value\n"
                                           "min         var.    min of all arguments\n"
                                           "max         var.    max of all arguments\n"
                                           "sum         var.    sum of all arguments\n"
                                           "avg         var.    mean value of all arguments\n"
                                           "rand        1       rand(seed), random float between 0 and 1, \n"
                                           "                    if seed = -1, then a 'random' seed is created.\n"
                                           "rand2       3       rand(a, b, seed), random float between a and b, \n"
                                           "                    if seed = -1, a 'random' seed is created using either\n"
                                           "                    by random_device or by reading system clock\n"
                                           "\nOperators\n"
                                           "Op  meaning         prioroty\n"
                                           "=   assignement     -1\n"
                                           "&&  logical and     1\n"
                                           "||  logical or      2\n"
                                           "<=  less or equal   4\n"
                                           ">=  greater or equal        4\n"
                                           "!=  not equal       4\n"
                                           "==  equal   4\n"
                                           ">   greater than    4\n"
                                           "<   less than       4\n"
                                           "+   addition        5\n"
                                           "-   subtraction     5\n"
                                           "*   multiplication  6\n"
                                           "/   division        6\n"
                                           "^   raise x to the power of y       7\n"
                                           "\n"
                                           "?:  if then else operator   C++ style syntax\n",
                                           &Func::setExpr,
                                           &Func::getExpr);
    static LookupValueFinfo < Func, string, double > var("var",
            "Lookup table for variable values.",
            &Func::setVar,
            &Func::getVar);
    static ReadOnlyValueFinfo< Func, vector<string> > vars("vars",
            "Variable names in the expression",
            &Func::getVars);
    static ValueFinfo< Func, double > x("x",
                                        "Value for variable named x. This is a shorthand. If the\n"
                                        "expression does not have any variable named x, this the first variable\n"
                                        "in the sequence `vars`.",
                                        &Func::setX,
                                        &Func::getX);
    static ValueFinfo< Func, double > y("y",
                                        "Value for variable named y. This is a utility for two/three\n"
                                        " variable functions where the y value comes from a source separate\n"
                                        " from that of x. This is a shorthand. If the\n"
                                        "expression does not have any variable named y, this the second\n"
                                        "variable in the sequence `vars`.",
                                        &Func::setY,
                                        &Func::getY);
    static ValueFinfo< Func, double > z("z",
                                        "Value for variable named z. This is a utility for three\n"
                                        " variable functions where the z value comes from a source separate\n"
                                        " from that of x or z. This is a shorthand. If the expression does not\n"
                                        " have any variable named z, this the third variable in the sequence `vars`.",
                                        &Func::setZ,
                                        &Func::getZ);
    ////////////////////////////////////////////////////////////
    // DestFinfos
    ////////////////////////////////////////////////////////////
    static DestFinfo varIn("varIn",
                           "Handle value for specified variable coming from other objects",
                           new OpFunc2< Func, string, double > (&Func::setVar));
    static DestFinfo xIn("xIn",
                         "Handle value for variable named x. This is a shorthand. If the\n"
                         "expression does not have any variable named x, this the first variable\n"
                         "in the sequence `vars`.",
                         new OpFunc1< Func, double > (&Func::setX));
    static DestFinfo yIn("yIn",
                         "Handle value for variable named y. This is a utility for two/three\n"
                         " variable functions where the y value comes from a source separate\n"
                         " from that of x. This is a shorthand. If the\n"
                         "expression does not have any variable named y, this the second\n"
                         "variable in the sequence `vars`.",
                         new OpFunc1< Func, double > (&Func::setY));
    static DestFinfo zIn("zIn",
                         "Handle value for variable named z. This is a utility for three\n"
                         " variable functions where the z value comes from a source separate\n"
                         " from that of x or y. This is a shorthand. If the expression does not\n"
                         " have any variable named y, this the second variable in the sequence `vars`.",
                         new OpFunc1< Func, double > (&Func::setZ));
    static DestFinfo xyIn("xyIn",
                          "Handle value for variables x and y for two-variable function",
                          new OpFunc2< Func, double, double > (&Func::setXY));
    static DestFinfo xyzIn("xyzIn",
                           "Handle value for variables x, y and z for three-variable function",
                           new OpFunc3< Func, double, double, double > (&Func::setXYZ));

    static DestFinfo setVars("setVars",
                             "Utility function to assign the variable values of the function.\n"
                             "Takes a list of variable names and a list of corresponding values.",
                             new OpFunc2< Func, vector< string >, vector< double > > (&Func::setVarValues));

    // TODO - a way to allow connect a source to a specific variable without the source knowing the variable name
    // simple case of x, [y, [z]] variables

    ///////////////////////////////////////////////////////////////////
    // Shared messages
    ///////////////////////////////////////////////////////////////////
    static DestFinfo process( "process",
                              "Handles process call, updates internal time stamp.",
                              new ProcOpFunc< Func >( &Func::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles reinit call.",
                             new ProcOpFunc< Func >( &Func::reinit ) );
    static Finfo* processShared[] =
    {
        &process, &reinit
    };

    static SharedFinfo proc( "proc",
                             "This is a shared message to receive Process messages "
                             "from the scheduler objects."
                             "The first entry in the shared msg is a MsgDest "
                             "for the Process operation. It has a single argument, "
                             "ProcInfo, which holds lots of information about current "
                             "time, thread, dt and so on. The second entry is a MsgDest "
                             "for the Reinit operation. It also uses ProcInfo. ",
                             processShared, sizeof( processShared ) / sizeof( Finfo* )
                           );

    static Finfo *funcFinfos[] =
    {
        &value,
        &derivative,
        &mode,
        &expr,
        &var,
        &vars,
        &x,
        &y,
        &z,
        &varIn,
        &xIn,
        &yIn,
        &zIn,
        &xyIn,
        &xyzIn,
        &proc,
        valueOut(),
        derivativeOut(),
    };

    static string doc[] =
    {
        "Name", "Func",
        "Author", "Subhasis Ray",
        "Description",
        "Func: general purpose function calculator using real numbers. It can\n"
        "parse mathematical expression defining a function and evaluate it\n"
        "and/or its derivative for specified variable values.\n"
        "The variables can be input from other moose objects. In case of\n"
        "arbitrary variable names, the source message must have the variable\n"
        "name as the first argument. For most common cases, input messages to\n"
        "set x, y, z and xy, xyz are made available without such\n"
        "requirement. This class handles only real numbers\n"
        "(C-double). Predefined constants are: pi=3.141592...,\n"
        "e=2.718281... \n"
    };

    static Dinfo< Func > dinfo;
    static Cinfo funcCinfo("Func",
                           Neutral::initCinfo(),
                           funcFinfos,
                           sizeof(funcFinfos) / sizeof(Finfo*),
                           &dinfo,
                           doc,
                           sizeof(doc)/sizeof(string));
    return &funcCinfo;

}

static const Cinfo * funcCinfo = Func::initCinfo();

const int Func::VARMAX = 10;

Func::Func():_x(NULL), _y(NULL), _z(NULL), _mode(1), _valid(false)
{
    _varbuf.reserve(VARMAX);
    _parser.SetVarFactory(_addVar, this);
    // Adding pi and e, the defaults are `_pi` and `_e`
    _parser.DefineConst(_T("pi"), (mu::value_type)M_PI);
    _parser.DefineConst(_T("e"), (mu::value_type)M_E);
}

Func::Func(const Func& rhs): _mode(rhs._mode)
{
    _varbuf.reserve(VARMAX);
    _parser.SetVarFactory(_addVar, this);
    // Adding pi and e, the defaults are `_pi` and `_e`
    _parser.DefineConst(_T("pi"), (mu::value_type)M_PI);
    _parser.DefineConst(_T("e"), (mu::value_type)M_E);
    setExpr(rhs.getExpr());
    vector <string> vars = rhs.getVars();
    for (unsigned int ii = 0; ii < vars.size(); ++ii)
    {
        setVar(vars[ii], rhs.getVar(vars[ii]));
    }
}

Func& Func::operator=(const Func rhs)
{
    _clearBuffer();
    _mode = rhs._mode;
    // Adding pi and e, the defaults are `_pi` and `_e`
    _parser.DefineConst(_T("pi"), (mu::value_type)M_PI);
    _parser.DefineConst(_T("e"), (mu::value_type)M_E);
    setExpr(rhs.getExpr());
    vector <string> vars = rhs.getVars();
    for (unsigned int ii = 0; ii < vars.size(); ++ii)
    {
        setVar(vars[ii], rhs.getVar(vars[ii]));
    }
    return *this;
}

Func::~Func()
{

    _clearBuffer();
}

void Func::_clearBuffer()
{
    _parser.ClearVar();
    for (unsigned int ii = 0; ii < _varbuf.size(); ++ii)
    {
        delete _varbuf[ii];
    }
    _varbuf.clear();
}

void Func::_showError(mu::Parser::exception_type &e) const
{
    cout << "Error occurred in parser.\n"
         << "Message:  " << e.GetMsg() << "\n"
         << "Formula:  " << e.GetExpr() << "\n"
         << "Token:    " << e.GetToken() << "\n"
         << "Position: " << e.GetPos() << "\n"
         << "Error code:     " << e.GetCode() << endl;
}
/**
   Call-back to add variables to parser automatically.
 */
static double * _addVar(const char *name, void *data)
{
    Func* func = reinterpret_cast< Func * >(data);
    double *ret = new double;
    *ret = 0.0;
    func->_varbuf.push_back(ret);
    return ret;
}

void Func::setExpr(string expr)
{
    _valid = false;
    _x = NULL;
    _y = NULL;
    _z = NULL;
    mu::varmap_type vars;
    try
    {
        _parser.SetExpr(expr);
        vars = _parser.GetUsedVar();
    }
    catch (mu::Parser::exception_type &e)
    {
        _showError(e);
        _clearBuffer();
        return;
    }
    mu::varmap_type::iterator v = vars.find("x");
    if (v != vars.end())
    {
        _x = v->second;
    }
    else if (vars.size() >= 1)
    {
        v = vars.begin();
        _x = v->second;
    }
    v = vars.find("y");
    if (v != vars.end())
    {
        _y = v->second;
    }
    else if (vars.size() >= 2)
    {
        v = vars.begin();
        ++v;
        _y = v->second;
    }
    v = vars.find("z");
    if (v != vars.end())
    {
        _z = v->second;
    }
    else if (vars.size() >= 3)
    {
        v = vars.begin();
        v++;
        v++;
        _z = v->second;
    }
    _valid = true;
}

string Func::getExpr() const
{
    if (!_valid)
    {
        cout << "Error: Func::getExpr() - invalid parser state" << endl;
        return "";
    }
    return _parser.GetExpr();
}

/**
   Set value of variable `name`
*/
void Func::setVar(string name, double value)
{
    if (!_valid)
    {
        cout << "Error: Func::setVar() - invalid parser state" << endl;
        return;
    }
    mu::varmap_type vars;
    try
    {
        vars = _parser.GetVar();
    }
    catch (mu::Parser::exception_type &e)
    {
        _valid = false;
        _showError(e);
        return;
    }
    mu::varmap_type::iterator v = vars.find(name);
    if (v != vars.end())
    {
        *v->second = value;
    }
    else
    {
        cout << "Error: no such variable " << name << endl;
    }
}

/**
   Get value of variable `name`
*/
double Func::getVar(string name) const
{
    if (!_valid)
    {
        cout << "Error: Func::getVar() - invalid parser state" << endl;
        return 0.0;
    }
    try
    {
        const mu::varmap_type &vars = _parser.GetVar();
        mu::varmap_type::const_iterator v = vars.find(name);
        if (v != vars.end())
        {
            return *v->second;
        }
        else
        {
            cout << "Error: no such variable " << name << endl;
            return 0.0;
        }
    }
    catch (mu::Parser::exception_type &e)
    {
        _showError(e);
        return 0.0;
    }
}

void Func::setX(double x)
{
    if (_x != NULL)
    {
        *_x = x;
    }
}

double Func::getX() const
{
    if (_x != NULL)
    {
        return *_x;
    }
    return 0.0;
}

void Func::setY(double y)
{
    if (_y != NULL)
    {
        *_y = y;
    }
}

double Func::getY() const
{
    if (_y != NULL)
    {
        return *_y;
    }
    return 0.0;
}
void Func::setZ(double z)
{
    if (_z != NULL)
    {
        *_z = z;
    }
}

double Func::getZ() const
{
    if (_z != NULL)
    {
        return *_z;
    }
    return 0.0;
}

void Func::setXY(double x, double y)
{
    if (_x != NULL)
    {
        *_x = x;
    }
    if (_y != NULL)
    {
        *_y = y;
    }
}

void Func::setXYZ(double x, double y, double z)
{
    if (_x != NULL)
    {
        *_x = x;
    }
    if (_y != NULL)
    {
        *_y = y;
    }
    if (_z != NULL)
    {
        *_z = z;
    }
}

void Func::setMode(unsigned int mode)
{
    _mode = mode;
}

unsigned int Func::getMode() const
{
    return _mode;
}

double Func::getValue() const
{
    double value = 0.0;
    if (!_valid)
    {
        cout << "Error: Func::getValue() - invalid state" << endl;
        return value;
    }
    try
    {
        value = _parser.Eval();
    }
    catch (mu::Parser::exception_type &e)
    {
        _showError(e);
    }
    return value;
}

double Func::getDerivative() const
{
    double value = 0.0;
    if (!_valid)
    {
        cout << "Error: Func::getDerivative() - invalid state" << endl;
        return value;
    }
    if (_x != NULL)
    {
        try
        {
            value = _parser.Diff(_x, *_x);
        }
        catch (mu::Parser::exception_type &e)
        {
            _showError(e);
        }
    }
    return value;
}


vector<string> Func::getVars() const
{
    vector< string > ret;
    if (!_valid)
    {
        cout << "Error: Func::getVars() - invalid parser state" << endl;
        return ret;
    }
    mu::varmap_type vars;
    try
    {
        vars = _parser.GetVar();
        for (mu::varmap_type::iterator ii = vars.begin();
                ii != vars.end(); ++ii)
        {
            ret.push_back(ii->first);
        }
    }
    catch (mu::Parser::exception_type &e)
    {
        _showError(e);
    }
    return ret;
}

void Func::setVarValues(vector<string> vars, vector<double> vals)
{

    if (vars.size() > vals.size() || !_valid)
    {
        return;
    }
    mu::varmap_type varmap = _parser.GetVar();
    for (unsigned int ii = 0; ii < vars.size(); ++ii)
    {
        mu::varmap_type::iterator v = varmap.find(vars[ii]);
        if ( v != varmap.end())
        {
            *v->second = vals[ii];
        }
    }
}

void Func::process(const Eref &e, ProcPtr p)
{
    if (!_valid)
    {
        return;
    }
    if (_mode & 1)
    {
        valueOut()->send(e, getValue());
    }
    if (_mode & 2)
    {
        derivativeOut()->send(e, getDerivative());
    }
}

void Func::reinit(const Eref &e, ProcPtr p)
{
    if (!_valid)
    {
        cout << "Error: Func::reinit() - invalid parser state. Will do nothing." << endl;
        return;
    }
    if (moose::trim(_parser.GetExpr(), " \t\n\r").length() == 0)
    {
        cout << "Error: no expression set. Will do nothing." << endl;
        setExpr("0.0");
        _valid = false;
    }
}
//
// Func.cpp ends here
