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

#include "muParser.h"
#include "FuncTerm.h"
#include "../utility/numutil.h"

FuncTerm::FuncTerm()
	: reactantIndex_( 1, 0 ),
		volScale_( 1.0 ),
		target_( ~0U)
{
	args_ = 0;
	parser_.DefineConst(_T("pi"), (mu::value_type)M_PI);
	parser_.DefineConst(_T("e"), (mu::value_type)M_E);
}

FuncTerm::~FuncTerm()
{
	if (args_) {
		delete[] args_;
	}
}

void FuncTerm::setReactantIndex( const vector< unsigned int >& mol )
{
	reactantIndex_ = mol;
	if ( args_ ) {
		delete[] args_;
		args_ = 0;
	}
	args_ = new double[ mol.size() + 1 ];
	// args_.resize( mol.size() + 1, 0.0 );
	for ( unsigned int i = 0; i < mol.size(); ++i ) {
		stringstream ss;
		args_[i] = 0.0;
		ss << "x" << i;
		parser_.DefineVar( ss.str(), &args_[i] );
	}
	// Define a 't' variable even if we don't always use it.
	args_[mol.size()] = 0.0;
	parser_.DefineVar( "t", &args_[mol.size()] );
}

const vector< unsigned int >& FuncTerm::getReactantIndex() const
{
	return reactantIndex_;
}


void showError(mu::Parser::exception_type &e)
{
    cout << "Error occurred in parser.\n" 
         << "Message:  " << e.GetMsg() << "\n"
         << "Formula:  " << e.GetExpr() << "\n"
         << "Token:    " << e.GetToken() << "\n"
         << "Position: " << e.GetPos() << "\n"
         << "Error code:     " << e.GetCode() << endl;
}

void FuncTerm::setExpr( const string& expr )
{
	try {
		parser_.SetExpr( expr );
		expr_ = expr;
	} catch(mu::Parser::exception_type &e) {
		showError(e);
		//return;
                throw(e);
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
	args_ = 0; // Don't delete it, the original one is still using it.
	parser_ = other.parser_;
	expr_ = other.expr_;
	volScale_ = other.volScale_;
	target_ = other.target_;
	setReactantIndex( other.reactantIndex_ );
	return *this;
}

/**
 * This computes the value. The time t is an argument needed by
 * some functions.
 */
double FuncTerm::operator() ( const double* S, double t ) const
{
	if ( !args_ )
		return 0.0;
	unsigned int i;
	for ( i = 0; i < reactantIndex_.size(); ++i )
		args_[i] = S[reactantIndex_[i]];
	args_[i] = t;
        try 
        {
            double result = parser_.Eval() * volScale_;
            return result;
        }
        catch (mu::Parser::exception_type &e )
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
        }
        catch ( mu::Parser::exception_type & e )
        {
            showError( e );
            //throw e;
        }
}
