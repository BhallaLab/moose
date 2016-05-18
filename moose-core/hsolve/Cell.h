/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **   copyright (C) 2003-2011 Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _CELL_H
#define _CELL_H

class Cell
{
private:
    struct MethodInfo
    {
        MethodInfo()
        {
            ;
        }

        MethodInfo( string v1, int v2, int v3 ):
            description( v1 ),
            isVariableDt( v2 ),
            isImplicit( v3 )
        {
            ;
        }

        string  description;
        int     isVariableDt;
        int     isImplicit;
    };

public:
    Cell();

    void setupf( Id cell );
    Id getSetup() const;

    void setMethod( string value );
    string getMethod() const;

    void setSolverClock( unsigned int value );
    unsigned int getSolverClock() const;

    void setSolverName( string value );
    string getSolverName() const;

    // Some readonly fields with more info about the methods.
    int getVariableDt() const;
    int getImplicit() const;
    string getDescription() const;

    void processDummy( const Eref& e, ProcPtr p );
    void reinit( const Eref& e, ProcPtr p );
    //~ void reinit( const Eref& e, const Qinfo* q );

    static void addMethod(
        const string& name,
        const string& description,
        int isVariableDt,
        int isImplicit );

    static const Cinfo* initCinfo();

private:
    void setupSolver( Id cell, Id seed ) const;

    string        method_;
    unsigned int  solverClock_;
    string        solverName_;
    Shell*        shell_;

    static vector< Id > children( Id obj );
    static Id findCompt( Id cell );

    static map< string, MethodInfo > methodMap_;
};

#endif // _CELL_H
