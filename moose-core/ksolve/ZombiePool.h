/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_POOL_H
#define _ZOMBIE_POOL_H

/**
 * This class is used by the Dsolve and Ksolve to take over from
 * regular pools. Possibly other solver classes will eventually use it too.
 */
class ZombiePool: public PoolBase
{
public:
    ZombiePool();
    ~ZombiePool();

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff
    //////////////////////////////////////////////////////////////////

    void vSetN( const Eref& e, double v );
    double vGetN( const Eref& e ) const;
    void vSetNinit( const Eref& e, double v );
    double vGetNinit( const Eref& e ) const;
    void vSetDiffConst( const Eref& e, double v );
    double vGetDiffConst( const Eref& e ) const;

    void vSetConc( const Eref& e, double v );
    double vGetConc( const Eref& e ) const;
    void vSetConcInit( const Eref& e, double v );
    double vGetConcInit( const Eref& e ) const;

    void vSetVolume( const Eref& e, double v );
    double vGetVolume( const Eref& e ) const;

    void vSetSpecies( const Eref& e, unsigned int v );
    unsigned int vGetSpecies( const Eref& e ) const;

    void vSetSolver( Id ksolve, Id dsolve );

    void vSetMotorConst( const Eref& e, double v );
    double vGetMotorConst( const Eref& e ) const;
    bool vGetIsBuffered( const Eref& e ) const;
    //////////////////////////////////////////////////////////////////
    // Dest funcs
    //////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////
    // utility funcs
    //////////////////////////////////////////////////////////////////

    static const Cinfo* initCinfo();
protected:
    /**
     * The ZombiePoolInterface pointers hold the solvers for the
     * ZombiePool. At least one must be assigned. Field assignments
     * propagate from the pool to whichever is assigned. Field
     * lookups first check the dsolve, then the ksolve.
     * The ZombiePool may be managed by the diffusion solver without
     * the involvement of the Stoich class at all. So instead of
     * routing the zombie operations through the Stoich, we have
     * pointers directly into the Dsolve and Ksolve.
     */
    ZombiePoolInterface* dsolve_;
    ZombiePoolInterface* ksolve_;
    double diffConst_;
    double motorConst_;
};

#endif	// _ZOMBIE_POOL_H
