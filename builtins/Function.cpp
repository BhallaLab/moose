// Description: Implementation of a wrapper around muParser.
// Author: Subhasis Ray
// Maintainer: Subhasis Ray

#include "../basecode/header.h"
#include "../utility/utility.h"
#include "../utility/numutil.h"
#include "../utility/print_function.hpp"
#include "../builtins/MooseParser.h"

#include "Variable.h"
#include "Function.h"

#include "../basecode/ElementValueFinfo.h"

static const double TriggerThreshold = 0.0;

static SrcFinfo1<double> *valueOut()
{
    static SrcFinfo1<double> valueOut("valueOut",
            "Evaluated value of the function for the current variable values."
            );
    return &valueOut;
}

static SrcFinfo1< double > *derivativeOut()
{
    static SrcFinfo1< double > derivativeOut("derivativeOut",
            "Value of derivative of the function for the current variable values");
    return &derivativeOut;
}

static SrcFinfo1< double > *rateOut()
{
    static SrcFinfo1< double > rateOut("rateOut",
            "Value of time-derivative of the function for the current variable values");
    return &rateOut;
}

static SrcFinfo1< vector < double > *> *requestOut()
{
    static SrcFinfo1< vector < double > * > requestOut(
        "requestOut",
        "Sends request for input variable from a field on target object");
    return &requestOut;

}

const Cinfo * Function::initCinfo()
{
    ////////////////////////////////////////////////////////////
    // Value fields
    ////////////////////////////////////////////////////////////
    static  ReadOnlyValueFinfo< Function, double > value(
        "value",
        "Result of the function evaluation with current variable values.",
        &Function::getValue);
    static ReadOnlyValueFinfo< Function, double > derivative(
        "derivative",
        "Derivative of the function at given variable values. This is calulated"
        " using 5-point stencil "
        " <http://en.wikipedia.org/wiki/Five-point_stencil> at current value of"
        " independent variable. Note that unlike hand-calculated derivatives,"
        " numerical derivatives are not exact.",
        &Function::getDerivative);
    static ReadOnlyValueFinfo< Function, double > rate(
        "rate",
        "Derivative of the function at given variable values. This is computed"
        " as the difference of the current and previous value of the function"
        " divided by the time step.",
        &Function::getRate);
    static ValueFinfo< Function, unsigned int > mode(
        "mode",
        "Mode of operation: \n"
        " 1: only the function value will be sent out.\n"
        " 2: only the derivative with respect to the independent variable will be sent out.\n"
        " 3: only rate (time derivative) will be sent out.\n"
        " anything else: all three, value, derivative and rate will be sent out.\n",
        &Function::setMode,
        &Function::getMode);
    static ValueFinfo< Function, bool > useTrigger(
        "useTrigger",
        "When *false*, disables event-driven calculation and turns on "
        "Process-driven calculations. \n"
        "When *true*, enables event-driven calculation and turns off "
        "Process-driven calculations. \n"
        "Defaults to *false*. \n",
        &Function::setUseTrigger,
        &Function::getUseTrigger);
    static ValueFinfo< Function, bool > doEvalAtReinit(
        "doEvalAtReinit",
        "When *false*, disables function evaluation at reinit, and "
        "just emits a value of zero to any message targets. \n"
        "When *true*, does a function evaluation at reinit and sends "
        "the computed value to any message targets. \n"
        "Defaults to *false*. \n",
        &Function::setDoEvalAtReinit,
        &Function::getDoEvalAtReinit);
    static ElementValueFinfo< Function, string > expr(
        "expr",
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
        "ln  	     1       logarithm to base e (2.71828...)\n"
        "exp         1       e raised to the power of x\n"
        "sqrt        1       square root of a value\n"
        "sign        1       sign function -1 if x<0; 1 if x>0\n"
        "rint        1       round to nearest integer\n"
        "abs         1       absolute value\n"
        "min         var.    min of all arguments\n"
        "max         var.    max of all arguments\n"
        "sum         var.    sum of all arguments\n"
        "avg         var.    mean value of all arguments\n"
        "rnd         0       rand(), random float between 0 and 1, honors global moose.seed.\n"
        "rand        1       rand(seed), random float between 0 and 1, \n"
        "                    if seed = -1, then a 'random' seed is used.\n"
        "rand2       3       rand(a, b, seed), random float between a and b, \n"
        "                    if seed = -1, a 'random' seed is created using either\n"
        "                    by random_device or by reading system clock\n"
        "\nOperators\n"
        "Op  meaning         		priority\n"
        "=   assignment     		-1\n"
        "&&  logical and     		1\n"
        "||  logical or      		2\n"
        "<=  less or equal   		4\n"
        ">=  greater or equal  		4\n"
        "!=  not equal         		4\n"
        "==  equal   			4\n"
        ">   greater than    		4\n"
        "<   less than       		4\n"
        "+   addition        		5\n"
        "-   subtraction     		5\n"
        "*   multiplication  		6\n"
        "/   division        		6\n"
        "^   raise x to the power of y  7\n"
        "%   floating point modulo      7\n"
        "\n"
        "?:  if then else operator   	C++ style syntax\n",
        &Function::setExpr,
        &Function::getExpr
    );

    static ValueFinfo< Function, unsigned int > numVars(
        "numVars",
        "Number of variables used by Function.",
        &Function::setNumVar,
        &Function::getNumVar
    );

    static FieldElementFinfo< Function, Variable > inputs(
        "x",
        "Input variables to the function. These can be passed via messages.",
        Variable::initCinfo(),
        &Function::getVar,
        &Function::setNumVar,
        &Function::getNumVar
    );

    static LookupValueFinfo < Function, string, double > constants(
        "c",
        "Constants used in the function. These must be assigned before"
        " specifying the function expression.",
        &Function::setConst,
        &Function::getConst);

    static ReadOnlyValueFinfo< Function, vector < double > > y(
        "y",
        "Variable values received from target fields by requestOut",
        &Function::getY);

    static ValueFinfo< Function, string > independent(
        "independent",
        "Index of independent variable. Differentiation is done based on this. Defaults"
        " to the first assigned variable.",
        &Function::setIndependent,
        &Function::getIndependent);

    ///////////////////////////////////////////////////////////////////
    // Shared messages
    ///////////////////////////////////////////////////////////////////
    static DestFinfo process( "process",
                              "Handles process call, updates internal time stamp.",
                              new ProcOpFunc< Function >( &Function::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles reinit call.",
                             new ProcOpFunc< Function >( &Function::reinit ) );
    static Finfo* processShared[] = { &process, &reinit };

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
    /*
       static DestFinfo trigger( "trigger",
       "Handles trigger input. Argument is timestamp of event. This is "
       "compatible with spike events as well as chemical ones. ",
       new OpFunc1< Function, double >( &Function::trigger ) );
       */

    static Finfo *functionFinfos[] =
    {
        &value,
        &rate,
        &derivative,
        &mode,
        &useTrigger,
        &doEvalAtReinit,
        &expr,
        &numVars,
        &inputs,
        &constants,
        &independent,
        &proc,
        requestOut(),
        valueOut(),
        rateOut(),
        derivativeOut(),
    };

    static string doc[] =
    {
        "Name", "Function",
        "Author", "Subhasis Ray",
        "Description",
        "General purpose function calculator using real numbers.\n"
        "It can parse mathematical expression defining a function and evaluate"
        " it and/or its derivative for specified variable values."
        "You can assign expressions of the form::\n"
        "\n"
        "f(c0, c1, ..., cM, x0, x1, ..., xN, y0,..., yP ) \n"
        "\n"
        " where `ci`'s are constants and `xi`'s and `yi`'s are variables."

        "The constants must be defined before setting the expression and"
        " variables are connected via messages. The constants can have any"
        " name, but the variable names must be of the form x{i} or y{i}"
        "  where i is increasing integer starting from 0.\n"
        " The variables can be input from other moose objects."
        " Such variables must be named `x{i}` in the expression and the source"
        " field is connected to Function.x[i]'s `input` destination field.\n"
        " In case the input variable is not available as a source field, but is"
        " a value field, then the value can be requested by connecting the"
        " `requestOut` message to the `get{Field}` destination on the target"
        " object. Such variables must be specified in the expression as y{i}"
        " and connecting the messages should happen in the same order as the"
        " y indices.\n"
        " This class handles only real numbers (C-double). Predefined constants"
        " are: pi=3.141592..., e=2.718281..."
    };

    static Dinfo< Function > dinfo;
    static Cinfo functionCinfo("Function",
                               Neutral::initCinfo(),
                               functionFinfos,
                               sizeof(functionFinfos) / sizeof(Finfo*),
                               &dinfo,
                               doc,
                               sizeof(doc)/sizeof(string));
    return &functionCinfo;

}

static const Cinfo * functionCinfo = Function::initCinfo();

Function::Function(): _t(0.0), _valid(false), _numVar(0), _lastValue(0.0),
    _value(0.0), _rate(0.0), _mode(1),
    _useTrigger( false ), _doEvalAtReinit( false ), _stoich(0)
{
    _parser.SetVarFactory(_functionAddVar, this);
    _independent = "x0";
    //extendMuParser( );

    // Adding this default expression by default to avoid complains from GUI
    try
    {
        _parser.SetExpr("0");
    }
    catch (moose::Parser::exception_type &e)
    {
        _showError(e);
        _clearBuffer();
        return;
    }
    _valid = true;
}

Function::Function(const Function& rhs):
    _numVar(rhs._numVar),
    _lastValue(rhs._lastValue),
    _value(rhs._value), _rate(rhs._rate),
    _mode(rhs._mode),
    _useTrigger( rhs._useTrigger),
    _stoich(nullptr)
{
    static Eref er;
    _independent = rhs._independent;

    _parser.SetVarFactory(_functionAddVar, this);
    //extendMuParser( );

    // Copy the constants
    moose::Parser::valmap_type cmap = rhs._parser.GetConst();
    if (cmap.size())
    {
        moose::Parser::valmap_type::const_iterator item = cmap.begin();
        for (; item!=cmap.end(); ++item)
        {
            _parser.DefineConst(item->first, item->second);
        }
    }


    setExpr(er, rhs.getExpr( er ));
    // Copy the values from the var pointers in rhs
    assert(_varbuf.size() == rhs._varbuf.size());
    for (unsigned int ii = 0; ii < rhs._varbuf.size(); ++ii)
    {
        _varbuf[ii]->value = rhs._varbuf[ii]->value;
    }
    assert(_pullbuf.size() == rhs._pullbuf.size());
    for (unsigned int ii = 0; ii < rhs._pullbuf.size(); ++ii)
    {
        *_pullbuf[ii] = *(rhs._pullbuf[ii]);
    }
}

Function& Function::operator=(const Function rhs)
{
    static Eref er;
    _clearBuffer();
    _mode = rhs._mode;
    _lastValue = rhs._lastValue;
    _value = rhs._value;
    _rate = rhs._rate;
    _independent = rhs._independent;
    // Adding pi and e, the defaults are `_pi` and `_e`
    _parser.DefineConst(_T("pi"), (moose::Parser::value_type)M_PI);
    _parser.DefineConst(_T("e"), (moose::Parser::value_type)M_E);
    // Copy the constants
    moose::Parser::valmap_type cmap = rhs._parser.GetConst();
    if (cmap.size())
    {
        moose::Parser::valmap_type::const_iterator item = cmap.begin();
        for (; item!=cmap.end(); ++item)
        {
            _parser.DefineConst(item->first, item->second);
        }
    }
    // Copy the values from the var pointers in rhs
    setExpr(er, rhs.getExpr( er ));
    assert(_varbuf.size() == rhs._varbuf.size());
    for (unsigned int ii = 0; ii < rhs._varbuf.size(); ++ii)
    {
        _varbuf[ii]->value = rhs._varbuf[ii]->value;
    }
    assert(_pullbuf.size() == rhs._pullbuf.size());
    for (unsigned int ii = 0; ii < rhs._pullbuf.size(); ++ii)
    {
        *_pullbuf[ii] = *(rhs._pullbuf[ii]);
    }
    return *this;
}

Function::~Function()
{
    _clearBuffer();
}

// do not know what to do about Variables that have already been
// connected by message.
void Function::_clearBuffer()
{
    _numVar = 0;
    _parser.ClearVar();
    for (unsigned int ii = 0; ii < _varbuf.size(); ++ii)
    {
        if ( _varbuf[ii] )
        {
            delete _varbuf[ii];
        }
    }
    _varbuf.clear();
    for (unsigned int ii = 0; ii < _pullbuf.size(); ++ii)
    {
        if ( _pullbuf[ii] )
        {
            delete _pullbuf[ii];
        }
    }
    _pullbuf.clear();
}

void Function::_showError(moose::Parser::exception_type &e) const
{
    cerr << "Error occurred in parser.\n"
         << "Message:  " << e.GetMsg() << endl;
}

/**
   Call-back to add variables to parser automatically.

   We use different storage for constants and variables. Variables are
   stored in a vector of Variable object pointers. They must have the
   name x{index} where index is any non-negative integer. Note that
   the vector is resized to whatever the maximum index is. It is up to
   the user to make sure that indices are sequential without any
   gap. In case there is a gap in indices, those entries will remain
   unused.

   If the name starts with anything other than `x` or `y`, then it is taken
   to be a named constant, which must be set before any expression or
   variables and error is thrown.

   NOTE: this is called not on setting expression but on first attempt
   at evaluation of the same, i.e. when you access `value` of the
   Function object.
 */
double * _functionAddVar(const char *name, void *data)
{
    Function* function = reinterpret_cast< Function * >(data);
    double * ret = NULL;
    string strname(name);

    // Names starting with x are variables, everything else is constant.
    if (name[0] == 'x')
    {
        int index = atoi(strname.substr(1).c_str());
        if ((unsigned)index >= function->_varbuf.size())
        {
            function->_varbuf.resize(index+1, 0);
            for (int ii = 0; ii <= index; ++ii)
            {
                if (function->_varbuf[ii] == 0)
                {
                    function->_varbuf[ii] = new Variable();
                }
            }
            function->_numVar = function->_varbuf.size();
        }
        ret = &(function->_varbuf[index]->value);
    }
    else if (name[0] == 'y')
    {
        int index = atoi(strname.substr(1).c_str());
        if ((unsigned)index >= function->_pullbuf.size())
        {
            function->_pullbuf.resize(index+1, 0 );
            for (int ii = 0; ii <= index; ++ii)
            {
                if (function->_pullbuf[ii] == 0)
                {
                    function->_pullbuf[ii] = new double();
                }
            }
        }
        ret = function->_pullbuf[index];
    }
    else if (strname == "t")
    {
        ret = &function->_t;
    }
    else
    {
        MOOSE_WARN( "Got an undefined symbol: " << strname << ".\n"
                    << "Variables must be named xi, yi, where i is integer index."
                    << " You must define the constants beforehand using LookupField c: c[name]"
                    " = value"
                  );
        throw moose::Parser::exception_type("Undefined constant.");
    }

    return ret;
}

/**
   This function is for automatically adding variables when setVar
   messages are connected.

   There are two ways new variables can be created :

   (1) When the user sets the expression, muParser calls _functionAddVar to
   get the address of the storage for each variable name in the
   expression.

   (2) When the user connects a setVar message to a Variable. ??

   This is called by Variable::addMsgCallback - which is unused.
 */
unsigned int Function::addVar()
{
    return 0;
}

void Function::setExpr(const Eref& eref, string expr)
{
    this->innerSetExpr( eref, expr ); // Refer to the virtual function here.
}

// Virtual function, this does the work.
void Function::innerSetExpr(const Eref& eref, string expr)
{
    _valid = false;
    _clearBuffer();
    _varbuf.resize(_numVar);
    // _pullbuf.resize(_num
    moose::Parser::varmap_type vars;
    try
    {
        _parser.SetExpr(expr);
    }
    catch (moose::Parser::exception_type &e)
    {
        cerr << "Error setting expression on: " << eref.objId().path() << endl;
        _showError(e);
        _clearBuffer();
        return;
    }
    // Force variable creation right away. Otherwise numVar does not
    // get set properly
    try
    {
        _parser.Eval();
        _valid = true;
    }
    catch (moose::Parser::exception_type &e)
    {
        _showError(e);
    }
}

string Function::getExpr( const Eref& e ) const
{
    if (!_valid)
    {
        cout << "Error: " << e.objId().path() << "::getExpr() - invalid parser state" << endl;
        return "";
    }
    return _parser.GetExpr();
}

void Function::setMode(unsigned int mode)
{
    _mode = mode;
}

unsigned int Function::getMode() const
{
    return _mode;
}

void Function::setUseTrigger(bool useTrigger )
{
    _useTrigger = useTrigger;
}

bool Function::getUseTrigger() const
{
    return _useTrigger;
}

void Function::setDoEvalAtReinit(bool doEvalAtReinit )
{
    _doEvalAtReinit = doEvalAtReinit;
}

bool Function::getDoEvalAtReinit() const
{
    return _doEvalAtReinit;
}

double Function::getValue() const
{
    double value = 0.0;
    if (!_valid)
    {
        cout << "Error: Function::getValue() - invalid state" << endl;
        return value;
    }
    try
    {
        value = _parser.Eval();
    }
    catch (moose::Parser::exception_type &e)
    {
        _showError(e);
    }
    return value;
}

double Function::getRate() const
{
    if (!_valid)
    {
        cout << "Error: Function::getValue() - invalid state" << endl;
    }
    return _rate;
}

void Function::setIndependent(string var)
{
    _independent = var;
}

string Function::getIndependent() const
{
    return _independent;
}

vector< double > Function::getY() const
{
    vector < double > ret(_pullbuf.size());
    for (unsigned int ii = 0; ii < ret.size(); ++ii)
    {
        ret[ii] = *_pullbuf[ii];
    }
    return ret;
}

double Function::getDerivative() const
{
    double value = 0.0;
    if (!_valid)
    {
        cout << "Error: Function::getDerivative() - invalid state" << endl;
        return value;
    }
    moose::Parser::varmap_type variables = _parser.GetVar();
    moose::Parser::varmap_type::const_iterator item = variables.find(_independent);
    if (item != variables.end())
    {
        try
        {
            value = _parser.Diff(item->second, *(item->second));
        }
        catch (moose::Parser::exception_type &e)
        {
            _showError(e);
        }
    }
    return value;
}

void Function::setNumVar(const unsigned int num)
{
    _clearBuffer();
    for (unsigned int ii = 0; ii < num; ++ii)
        _functionAddVar( ("x"+std::to_string(ii)).c_str(), this);
}

unsigned int Function::getNumVar() const
{
    return _numVar;
}

void Function::setVar(unsigned int index, double value)
{
    cout << "varbuf[" << index << "]->setValue(" << value << ")\n";
    if (index < _varbuf.size())
    {
        _varbuf[index]->setValue(value);
    }
    else
    {
        cerr << "Function: index " << index << " out of bounds." << endl;
    }
}

Variable * Function::getVar(unsigned int ii)
{
    static Variable dummy;
    if ( ii < _varbuf.size())
    {
        return _varbuf[ii];
    }
    cout << "Warning: Function::getVar: index: "
         << ii << " is out of range: "
         << _varbuf.size() << endl;
    return &dummy;
}

void Function::setConst(string name, double value)
{
    _parser.DefineConst(name, value);
}

double Function::getConst(string name) const
{
    auto cmap = _parser.GetConst();
    if (cmap.size())
    {
        auto it = cmap.find(name);
        if (it != cmap.end())
            return it->second;
    }
    return 0;
}

void Function::process(const Eref &e, ProcPtr p)
{
    if (!_valid)
    {
        return;
    }
    vector < double > databuf;
    requestOut()->send(e, &databuf);
    for (unsigned int ii = 0;
            (ii < databuf.size()) && (ii < _pullbuf.size());
            ++ii)
    {
        *_pullbuf[ii] = databuf[ii];
    }
    _t = p->currTime;
    _value = getValue();
    _rate = (_value - _lastValue) / p->dt;
    if ( _useTrigger && _value < TriggerThreshold )
    {
        _lastValue = _value;
        return;
    }
    switch (_mode)
    {
    case 1:
    {
        valueOut()->send(e, _value);
        return;
    }
    case 2:
    {
        derivativeOut()->send(e, getDerivative());
        return;
    }
    case 3:
    {
        rateOut()->send(e, _rate);
        return;
    }
    default:
    {
        valueOut()->send(e, _value);
        derivativeOut()->send(e, getDerivative());
        rateOut()->send(e, _rate);
        return;
    }
    }
    _lastValue = _value;
}

void Function::reinit(const Eref &e, ProcPtr p)
{
    if (!_valid)
    {
        cout << "Error: Function::reinit() - invalid parser state. Will do nothing." << endl;
        return;
    }
    if (moose::trim(_parser.GetExpr(), " \t\n\r").length() == 0)
    {
        cout << "Error: no expression set. Will do nothing." << endl;
        setExpr(e, "0.0");
        _valid = false;
    }
    _t = p->currTime;
    if (_doEvalAtReinit)
    {
        _lastValue = _value = getValue();
    }
    else
    {
        _lastValue = _value = 0.0;
    }
    _rate = 0.0;
    switch (_mode)
    {
    case 1:
    {
        valueOut()->send(e, _value);
        break;
    }
    case 2:
    {
        derivativeOut()->send(e, 0.0);
        break;
    }
    case 3:
    {
        rateOut()->send(e, _rate);
        break;
    }
    default:
    {
        valueOut()->send(e, _value);
        derivativeOut()->send(e, 0.0);
        rateOut()->send(e, _rate);
        break;
    }
    }
}

// Function.cpp ends here
