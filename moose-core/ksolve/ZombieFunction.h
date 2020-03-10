/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef ZOMBIE_FUNCTION_H_
#define ZOMBIE_FUNCTION_H_

class ZombieFunction: public Function
{
public:
    ZombieFunction();
    ~ZombieFunction();

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff
    //////////////////////////////////////////////////////////////////
    void setExpr( const Eref& e, string val );

    //////////////////////////////////////////////////////////////////
    // Dest funcs
    //////////////////////////////////////////////////////////////////
    void process(const Eref &e, ProcPtr p);
    void reinit(const Eref &e, ProcPtr p);

    //////////////////////////////////////////////////////////////////
    // utility funcs
    //////////////////////////////////////////////////////////////////

    void setSolver( Id solver, Id orig );

    static void zombify( Element* orig, const Cinfo* zClass,
                         Id ksolve, Id dsolve );

    static const Cinfo* initCinfo();

private:

};

#endif	// ZOMBIE_FUNCTION_H_
