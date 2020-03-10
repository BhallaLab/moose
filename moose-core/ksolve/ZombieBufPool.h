/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_BUF_POOL_H
#define _ZOMBIE_BUF_POOL_H

class ZombieBufPool: public ZombiePool
{
public:
    ZombieBufPool();
    ~ZombieBufPool();

    /// The 'get' functions are simply inherited from ZombiePool
    void vSetN( const Eref& e, double v );
    void vSetNinit( const Eref& e, double v );
    void vSetConc( const Eref& e, double v );
    void vSetConcInit( const Eref& e, double v );
    bool vGetIsBuffered( const Eref& e ) const;

    static const Cinfo* initCinfo();
private:
};

#endif	// _ZOMBIE_BUF_POOL_H
