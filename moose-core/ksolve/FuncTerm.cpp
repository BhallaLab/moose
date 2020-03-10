/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This little class sets up a muParser to execute on entries in the
 * molecule 'n' vector, and possibly on the time t.
 *
 * The user must first set the arg indices (FuncTerm::setArgIndex), before
 * specifying the function string.
 *
 * The arguments are named x0, x1, x2 ..., t )
 *
 */

#include <vector>
#include <sstream>
using namespace std;

#include "FuncTerm.h"
#include "../utility/numutil.h"

FuncTerm::FuncTerm():
    reactantIndex_(1, 0) , volScale_(1.0) , target_(~0U) , args_(nullptr)
{
}

FuncTerm::~FuncTerm()
{
    if(args_)
        delete[] args_;
}

void FuncTerm::setReactantIndex( const vector< unsigned int >& mol )
{
    reactantIndex_ = mol;
    if ( args_ )
    {
        delete[] args_;
        parser_.ClearAll();
    }

    args_ = new double[mol.size()+1];
    for ( unsigned int i = 0; i < mol.size(); ++i )
    {
        args_[i] = 0.0;
        parser_.DefineVar( 'x'+to_string(i), &args_[i] );
    }

    // Define a 't' variable even if we don't always use it.
    args_[mol.size()] = 0.0;
    parser_.DefineVar( "t", &args_[mol.size()] );
    setExpr(expr_);
}

const vector< unsigned int >& FuncTerm::getReactantIndex() const
{
    return reactantIndex_;
}


void showError(moose::Parser::exception_type &e)
{
    cerr << "Error occurred in parser.\n"
         << "Message:  " << e.GetMsg() << endl;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Set expression on FuncTerm. Before calling this function, user
 * should make sure that symbol_table has been linked to pointers to the variables.
 *
 * @Param expr
 */
/* ----------------------------------------------------------------------------*/
void FuncTerm::setExpr( const string& expr )
{
    // Empty expression are not allowed.
    if(expr.empty())
        return;

    try
    {
        if(! parser_.SetExpr(expr))
            MOOSE_WARN("Failed to set expression: '" << expr << "'");
        expr_ = expr;
    }
    catch(moose::Parser::exception_type &e)
    {
        showError(e);
        return;
    }
}

const string& FuncTerm::getExpr() const
{
    return expr_;
}

void FuncTerm::setTarget( unsigned int t )
{
    target_ = t;
}

const unsigned int FuncTerm::getTarget() const
{
    return target_;
}

void FuncTerm::setVolScale( double vs )
{
    volScale_ = vs;
}

double FuncTerm::getVolScale() const
{
    return volScale_;
}

const FuncTerm& FuncTerm::operator=( const FuncTerm& other )
{
    args_ = nullptr;  // other is still using it.
    expr_ = other.expr_;
    volScale_ = other.volScale_;
    target_ = other.target_;
    reactantIndex_ = other.reactantIndex_;
    parser_ = other.parser_;
    setReactantIndex(reactantIndex_);
    return *this;
}

/**
 * This computes the value. The time t is an argument needed by
 * some functions.
 */
double FuncTerm::operator() ( const double* S, double t ) const
{
    if ( ! args_ )
        return 0.0;

    unsigned int i = 0;
    for ( i = 0; i < reactantIndex_.size(); ++i )
        args_[i] = S[reactantIndex_[i]];
    args_[i] = t;

    try
    {
        return parser_.Eval() * volScale_;
    }
    catch (moose::Parser::exception_type &e )
    {
        cerr << "Error: " << e.GetMsg() << endl;
        throw e;
    }
}

void FuncTerm::evalPool( double* S, double t ) const
{
    if ( !args_ || target_ == ~0U )
        return;

    unsigned int i;
    for ( i = 0; i < reactantIndex_.size(); ++i )
        args_[i] = S[reactantIndex_[i]];
    args_[i] = t;

    try
    {
        S[ target_] = parser_.Eval() * volScale_;
        //assert(! std::isnan(S[target_]));
    }
    catch ( moose::Parser::exception_type & e )
    {
        showError( e );
        return;
    }
}
