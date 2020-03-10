// Function.h ---
// Description: moose.Function class.
// Author: Subhasis Ray
// Maintainer: Dilawar Singh
// Version: See git logs.

#ifndef FUNCTIONH_
#define FUNCTIONH_
#include <memory>

class Variable;
class Eref;
class Cinfo;

namespace moose { 
    class MooseParser;
};


// Symbol types.
enum VarType {XVAR_INDEX, XVAR_NAMED, YVAR, TVAR, CONSTVAR};

class Function 
{
public:
    static const int VARMAX;
    Function();

    // Destructor.
    ~Function();

    // copy operator.
    Function& operator=(const Function& rhs);

    static const Cinfo * initCinfo();

    void setExpr(const Eref& e, const string expr);
    bool innerSetExpr(const Eref& e, const string expr);

    string getExpr(const Eref& e) const;

    // get a list of variable identifiers.
    vector<string> getVars() const;
    void setVarValues(vector<string> vars, vector<double> vals);

    // get/set the value of variable `name`
    void setVar(unsigned int index, double value);

    Variable* getVar(unsigned int ii);

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

    void setAllowUnknownVariable(bool value);
    bool getAllowUnknowVariable() const;

    void setNumVar(unsigned int num);
    unsigned int getNumVar() const;

    void setConst(string name, double value);
    double getConst(string name) const;

    void setVarIndex(string name, unsigned int val);
    unsigned int getVarIndex(string name) const;

    void setIndependent(string index);
    string getIndependent() const;

    vector<double> getY() const;

    double getDerivative() const;

    void findXsYs( const string& expr, vector<string>& vars );

    unsigned int addVar();
    /* void dropVar(unsigned int msgLookup); */

    void process(const Eref& e, ProcPtr p);
    void reinit(const Eref& e, ProcPtr p);

    // This is also used as callback.
    void addVariable(const string& name);

    // Add unknown variable.
    void callbackAddSymbol(const string& name);

    bool symbolExists(const string& name) const;

    void addXByIndex(const unsigned int index);
    void addXByName(const string& name);

    void addY(const unsigned int index);

    VarType getVarType(const string& name) const;

    void clearAll();

protected:

    bool valid_;
    unsigned int numVar_;
    double lastValue_;
    double value_;
    double rate_;
    unsigned int mode_;
    bool useTrigger_;
    bool doEvalAtReinit_;
    bool allowUnknownVar_;

    double t_;                             // local storage for current time
    string independent_;                   // To take derivative.

    // this stores variables received via incoming messages, identifiers of
    // the form x{i} are included in this
    vector<shared_ptr<Variable>> xs_;

    // Keep the index of x's.
    map<string, unsigned int> varIndex_;

    // this stores variable values pulled by sending request. identifiers of
    // the form y{i} are included in this
    vector<shared_ptr<double>> ys_;
    map<string, shared_ptr<double>> consts_;

    // Used by kinetic solvers when this is zombified.
    void* stoich_;

    // pointer to the MooseParser
    shared_ptr<moose::MooseParser> parser_;

};

#endif /* end of include guard: FUNCTIONH_ */
