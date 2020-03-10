// Description:
// Author: Subhasis Ray

#ifndef _MOOSE_FUNCTION_H_
#define _MOOSE_FUNCTION_H_


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

    // set/get flag to do function evaluation at reinit
    void setDoEvalAtReinit(bool doEvalAtReinit);
    bool getDoEvalAtReinit() const;

    void setNumVar(unsigned int num);
    unsigned int getNumVar() const;

    void setConst(string name, double value);
    double getConst(string name) const;

    void setIndependent(string index);
    string getIndependent() const;

    vector < double > getY() const;

    double getDerivative() const;

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
    bool _doEvalAtReinit;

    // this stores variables received via incoming messages, identifiers of
    // the form x{i} are included in this
    vector<Variable *> _varbuf;

    // this stores variable values pulled by sending request. identifiers of
    // the form y{i} are included in this
    vector< double * > _pullbuf;
    map< string, double *> _constbuf;  // for constants
    string _independent; // index of independent variable

    MooseParser _parser;

    void _clearBuffer();
    void _showError(moose::Parser::exception_type &e) const;

    // Used by kinetic solvers when this is zombified.
    char* _stoich;
};

#endif
