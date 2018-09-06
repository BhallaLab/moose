/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _BUF_POOL_H
#define _BUF_POOL_H

class BufPool: public Pool
{
public:
    BufPool();
    ~BufPool();

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff
    //////////////////////////////////////////////////////////////////
    /// The 'get' functions are simply inherited from Pool
    void vSetN( const Eref& e, double v );
    void vSetNinit( const Eref& e, double v );
    void vSetConc( const Eref& e, double v );
    void vSetConcInit( const Eref& e, double v );

    //////////////////////////////////////////////////////////////////
    // Dest funcs
    //////////////////////////////////////////////////////////////////

    void vProcess( const Eref& e, ProcPtr p );
    void vReinit( const Eref& e, ProcPtr p );

    static const Cinfo* initCinfo();
private:
};

#endif	// _BUF_POOL_H
