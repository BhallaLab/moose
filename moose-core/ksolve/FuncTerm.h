/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _FUNC_TERM_H
#define _FUNC_TERM_H

#include "../builtins/MooseParser.h"

class FuncTerm
{
public:
    FuncTerm();
    ~FuncTerm();

    /**
     * This computes the value. The time t is an argument needed by
     * some functions.
     */
    double operator() ( const double* S, double t ) const;
    const FuncTerm& operator=( const FuncTerm& other );

    void evalPool( double* s, double t ) const;

    /**
     * This function finds the reactant indices in the vector
     * S. It returns the number of indices found, which are the
     * entries in molIndex.
     */
    void setReactantIndex( const vector< unsigned int >& mol );
    const vector< unsigned int >& getReactantIndex() const;
    const string& getExpr() const;
    void setExpr( const string& e );
    const unsigned int getTarget() const;
    void setTarget( unsigned int tgt );
    void setVolScale( double vs );
    double getVolScale() const;

private:
    // Look up reactants in the S vec.
    vector< unsigned int > reactantIndex_;

    /**
     * Scale factor to account for pool volume if we are assigning conc
     * rather than N. Note that this conc will not be further updated
     * so this is an undesirable option.
     */
    double volScale_;
    unsigned int target_; /// Index of the entity to be updated by Func

    double* args_;

    string expr_;
    moose::MooseParser parser_;
};

#endif // _FUNC_TERM_H
