// Function.h --- 
// 
// Filename: Function.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Created: Fri May 30 19:34:13 2014 (+0530)
// Version: 
// Last-Updated: 
//           By: 
//     Update #: 0
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// A new version of Func with FieldElements to collect data.
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

#ifndef _MOOSE_FUNCTION_H_
#define _MOOSE_FUNCTION_H_

#include "muParser.h"

/**
   Simple function parser and evaluator for MOOSE. This can take a mathematical
   expression in standard C form and a list of variables values and
   evaluate the results.
 */
double *_functionAddVar(const char *name, void *data);

class Function
{
  public:
    static const int VARMAX;
    Function();
    Function(const Function& rhs);
    ~Function();
    virtual void innerSetExpr( const Eref& e, string expr);
    void setExpr( const Eref& e, string expr);
    string getExpr( const Eref& e ) const;
    
    
    // get a list of variable identifiers.
    // this is created by the parser
    vector<string> getVars() const;
    void setVarValues(vector< string > vars, vector < double > vals);

    
    // get/set the value of variable `name`
    void setVar(unsigned int index, double value);
    Variable * getVar(unsigned int ii);

    // get function eval result
    double getValue() const;

    double getRate() const;

    // get/set operation mode
    void setMode(unsigned int mode);
    unsigned int getMode() const;

	// set/get flag to use trigger mode.
    void setUseTrigger(bool useTrigger);
    bool getUseTrigger() const;

    void setNumVar(unsigned int num);
    unsigned int getNumVar() const;

    void setConst(string name, double value);
    double getConst(string name) const;

    void setIndependent(string index);
    string getIndependent() const;

    vector < double > getY() const;

    double getDerivative() const;

#if 0
    /**
     * @brief Extend standard muparser library. This add some binary operators
     * and user-defined functions.
     */
    void extendMuParser( void );

    /**
     * @brief callback function for floating point mod-operator.
     *
     * @param a double
     * @param b double
     *
     * @return  double using std::fmod(a, b)
     */
    static mu::value_type muCallbackFMod( mu::value_type a, mu::value_type b );
#endif

    Function& operator=(const Function rhs);

    unsigned int addVar();
    /* void dropVar(unsigned int msgLookup); */

    void process(const Eref& e, ProcPtr p);
    void reinit(const Eref& e, ProcPtr p);

    static const Cinfo * initCinfo();

protected:
    friend double * _functionAddVar(const char * name, void *data);
    double _t; // local storage for current time
    mutable bool _valid;
    unsigned int _numVar;
    double _lastValue;
    double _value;
    double _rate;
    unsigned int _mode;
    bool _useTrigger;
     // this stores variables received via incoming messages, identifiers of the form x{i} are included in this
    vector<Variable *> _varbuf;
    // this stores variable values pulled by sending request. identifiers of the form y{i} are included in this
    vector< double * > _pullbuf;
    map< string, double *> _constbuf;  // for constants
    string _independent; // index of independent variable
    mu::Parser _parser;
    void _clearBuffer();
    void _showError(mu::Parser::exception_type &e) const;
	char* _stoich; // Used by kinetic solvers when this is zombified.
};


#endif


// 
//// Function.h ends here_parser.DefineVar( _T("t"), 
