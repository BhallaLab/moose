/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _POOL_BASE_H
#define _POOL_BASE_H

/**
 * SpeciesId identifies molecular species. This is a unique identifier for
 * any given molecular species, regardless of which compartment or solver
 * is handling it.
 */
typedef unsigned int SpeciesId;
extern const SpeciesId DefaultSpeciesId;

/**
 * The PoolBase class is the base class for molecular pools.
 * A pool is a set of molecules of a
 * given species, in a uniform chemical context. Note that the same
 * species might be present in other compartments, or be handled by
 * other solvers.
 * PoolBase is the base class for mass-action, single particle
 * and other numerical variants of pools.
 */
class PoolBase
{
    friend void testSyncArray( unsigned int size, unsigned int numThreads,
                               unsigned int method );
    friend void checkVal( double time, const PoolBase* m, unsigned int size );
    friend void forceCheckVal( double time, Element* e, unsigned int size );

public:
    PoolBase();
    virtual ~PoolBase();

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff: Interface for the Cinfo, hence regular
    // funcs. These internally call the virtual funcs that do the real
    // work.
    //////////////////////////////////////////////////////////////////
    void setN( const Eref& e, double v );
    double getN( const Eref& e ) const;
    void setNinit( const Eref& e, double v );
    double getNinit( const Eref& e ) const;
    void setDiffConst( const Eref& e, double v );
    double getDiffConst( const Eref& e ) const;
    void setMotorConst( const Eref& e, double v );
    double getMotorConst( const Eref& e ) const;

    void setConc( const Eref& e, double v );
    double getConc( const Eref& e ) const;
    void setConcInit( const Eref& e, double v );
    double getConcInit( const Eref& e ) const;

    /**
     * Volume is usually volume, but we also permit areal density
     * This is obtained by looking up the corresponding spatial mesh
     * entry in the parent compartment. If the message isn't set then
     * it defaults to 1.0.
     */
    void setVolume( const Eref& e, double v );
    double getVolume( const Eref& e ) const;

    void setSpecies( const Eref& e, SpeciesId v );
    SpeciesId getSpecies( const Eref& e ) const;
    /**
     * Functions to examine and change class between Pool and BufPool.
     */
    void setIsBuffered( const Eref& e, bool v );
    bool getIsBuffered( const Eref& e ) const;

    //////////////////////////////////////////////////////////////////
    // Here are the inner virtual funcs for fields.
    // All these are pure virtual
    //////////////////////////////////////////////////////////////////

    virtual void vSetN( const Eref& e, double v ) = 0;
    virtual double vGetN( const Eref& e ) const = 0;
    virtual void vSetNinit( const Eref& e, double v ) = 0;
    virtual double vGetNinit( const Eref& e ) const = 0;
    virtual void vSetDiffConst( const Eref& e, double v ) = 0;
    virtual double vGetDiffConst( const Eref& e ) const = 0;
    virtual void vSetMotorConst( const Eref& e, double v );
    virtual double vGetMotorConst( const Eref& e ) const;

    virtual void vSetConc( const Eref& e, double v ) = 0;
    virtual double vGetConc( const Eref& e ) const = 0;
    virtual void vSetConcInit( const Eref& e, double v ) = 0;
    virtual double vGetConcInit( const Eref& e ) const;
    virtual double vGetVolume( const Eref& e ) const = 0;
    virtual void vSetVolume( const Eref& e, double v ) = 0;
    virtual void vSetSpecies( const Eref& e, SpeciesId v ) = 0;
    virtual SpeciesId vGetSpecies( const Eref& e ) const = 0;
    /// I put in a default empty function for vSetIsBuffered.
    virtual void vSetIsBuffered( const Eref& e, bool v );
    virtual bool vGetIsBuffered( const Eref& e) const = 0;
    /**
     * Assign whatever info is needed by the zombie based on the
     * solver Element. Encapsulates some unpleasant field extraction,
     * casting, and assignment. Default version of this function does
     * nothing.
     */
    virtual void vSetSolver( Id ksolve, Id dsolve );

    //////////////////////////////////////////////////////////////////
    /**
     * zombify is the base function for conversion between pool
     * subclasses. This can be overridden, but should work for most
     * things. This takes the original Element, and without touching
     * its messaging, replaces it with a new data object of the
     * specified zClass. It does the best it can with conversion of
     * fields. Typically needs to be followed by rescheduling and
     * possibly a class-specific function for assigning further
     * zombie fields outside the ken of the PoolBase.
     * The 'solver' argument specifies which objects handle the solver
     * for this conversion. For the Pool this is either or both of
     * a kinetic solver /ksolve/ and a diffusion solver /dsolve/.
     * The term zombie arises because this operation was originally
     * carried out to strip an object of independent function, and
     * replace it with a solver-controlled facsimile.
     */
    static void zombify( Element* original, const Cinfo* zClass,
                         Id ksolve, Id dsolve );

    //////////////////////////////////////////////////////////////////
    // Dest funcs
    //////////////////////////////////////////////////////////////////
    void process( const Eref& e, ProcPtr p );
    void reinit( const Eref& e, ProcPtr p );
    void reac( double A, double B );
    void handleMolWt( const Eref& e, double v );
    void increment( double val );
    void decrement( double val );
    void nIn( double val );

    //////////////////////////////////////////////////////////////////
    // Virtual Dest funcs. Most of these have a generic do-nothing
    // function here, as most of the derived classes don't need to
    // do anything.
    //////////////////////////////////////////////////////////////////
    virtual void vProcess( const Eref& e, ProcPtr p );
    virtual void vReinit( const Eref& e, ProcPtr p );
    virtual void vReac( double A, double B );
    virtual void vHandleMolWt( const Eref& e, double v);
    virtual void vIncrement( double val );
    virtual void vDecrement( double val );
    virtual void vnIn( double val );

    //////////////////////////////////////////////////////////////////
    static const Cinfo* initCinfo();

private:
    double concInit_; /// Initial concentration.
    // We don't store the conc here as this is computed on the fly
    // by derived classes. But the PoolBase::concInit is authoritative.
};

#endif	// _POOL_BASE_H
