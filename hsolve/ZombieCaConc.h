/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_CACONC_H
#define _ZOMBIE_CACONC_H

/**
 * Zombie object that lets HSolve do its calculations, while letting the user
 * interact with this object as if it were the original object.
 */
class ZombieCaConc: public CaConcBase
{
public:
    ZombieCaConc()
        : CaConcBase(),
        hsolve_( NULL ),
        tau_( 0.0 ),
        B_( 0.0 ),
        thickness_( 0.0 )
    {
        ;
    }

    ///////////////////////////////////////////////////////////////
    // Message handling functions
    ///////////////////////////////////////////////////////////////
    void vReinit( const Eref& e, ProcPtr info );
    void vProcess( const Eref& e, ProcPtr info );

    void vCurrent( const Eref& e, double I );
    void vCurrentFraction( const Eref& e, double I, double fraction );
    void vIncrease( const Eref& e, double I );
    void vDecrease( const Eref& e, double I );
    ///////////////////////////////////////////////////////////////
    // Field handling functions. These are all virtual, overriding base.
    ///////////////////////////////////////////////////////////////
    void vSetCa( const Eref& e,   double val );
    double vGetCa( const Eref& e ) const;
    void vSetCaBasal( const Eref& e , double val );
    double vGetCaBasal( const Eref& e ) const;
    void vSetTau( const Eref& e , double val );
    double vGetTau( const Eref& e ) const;
    void vSetB( const Eref& e , double val );
    double vGetB( const Eref& e ) const;
    void vSetCeiling( const Eref& e , double val );
    double vGetCeiling( const Eref& e ) const;
    void vSetFloor( const Eref& e , double val );
    double vGetFloor( const Eref& e ) const;

    ///////////////////////////////////////////////////////////////
	void vSetSolver( const Eref& e, Id hsolve );
    ///////////////////////////////////////////////////////////////
    static const Cinfo* initCinfo();

private:
    HSolve* hsolve_;

    double tau_;
    double B_;
    double thickness_;
};


#endif // _ZOMBIE_CACONC_H
