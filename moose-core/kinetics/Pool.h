/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _POOL_H
#define _POOL_H
#include "PoolBase.h"
/**
 * The Pool class is a molecular pool. This is a set of molecules of a
 * given species, in a uniform chemical context. Note that the same
 * species might be present in other compartments, or be handled by
 * other solvers.
 */
class Pool: public PoolBase
{
public:
    Pool();
    ~Pool();

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff. All override virtual funcs in the Pool
    // base class.
    //////////////////////////////////////////////////////////////////

    void vSetN( const Eref& e, double v );
    double vGetN( const Eref& e ) const;
    void vSetNinit( const Eref& e, double v );
    double vGetNinit( const Eref& e ) const;
    void vSetDiffConst( const Eref& e, double v );
    double vGetDiffConst( const Eref& e ) const;
    void vSetMotorConst( const Eref& e, double v );
    double vGetMotorConst( const Eref& e ) const;

    void vSetConc( const Eref& e, double v );
    double vGetConc( const Eref& e ) const;
    void vSetConcInit( const Eref& e, double v );
    // double vGetConcInit( const Eref& e ) const;

    /**
     * Volume is usually volume, but we also permit areal density
     * This is obtained by looking up the corresponding spatial mesh
     * entry in the parent compartment. If the message isn't set then
     * it defaults to 1.0.
     */
    void vSetVolume( const Eref& e, double v );
    double vGetVolume( const Eref& e ) const;

    void vSetSpecies( const Eref& e, SpeciesId v );
    SpeciesId vGetSpecies( const Eref& e ) const;


    /**
     * Functions to examine and change class between Pool and BufPool.
     */
    void vSetIsBuffered( const Eref& e, bool v );
    bool vGetIsBuffered( const Eref& e) const;

    //////////////////////////////////////////////////////////////////
    // Dest funcs. These too override virtual funcs in the Pool base
    // class.
    //////////////////////////////////////////////////////////////////

    void vHandleMolWt( const Eref& e, double v );
    void vProcess( const Eref& e, ProcPtr p );
    void vReinit( const Eref& e, ProcPtr p );
    void vReac( double A, double B );
    void vIncrement( double val );
    void vDecrement( double val );
    void vnIn( double val );

    //////////////////////////////////////////////////////////////////
    // Novel Dest funcs not present in Pool base class.
    //////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////
    static const Cinfo* initCinfo();
private:
    double n_; /// Number of molecules in pool
    double nInit_; /// initial number of molecules.
    double diffConst_;	/// Diffusion constant
    double motorConst_;	/// Motor transport constant
    double A_; /// Internal state variables, used only in explict mode
    double B_;

    /**
     * System wide identifier for all mol pools that are chemically
     * the same species.
     */
    unsigned int species_;
};

#endif	// _POOL_H
