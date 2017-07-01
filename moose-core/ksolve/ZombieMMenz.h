/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_MM_ENZ_H
#define _ZOMBIE_MM_ENZ_H

/**
 * This class represents the Michaelis-Menten type enzyme, obeying the
 * equation
 * V = kcat.[Etot].[S]/( Km + [S] )
 */
class ZombieMMenz: public EnzBase
{
	public:
		ZombieMMenz();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff: All override virtual funcs
		//////////////////////////////////////////////////////////////////

		void vSetKm( const Eref& e, double v );
		double vGetKm( const Eref& e ) const;
		void vSetNumKm( const Eref& e, double v );
		double vGetNumKm( const Eref& e ) const;
		void vSetKcat( const Eref& e, double v );
		double vGetKcat( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs: All override virtual funcs
		//////////////////////////////////////////////////////////////////

		// void vRemesh( const Eref& e );

		//////////////////////////////////////////////////////////////////
		// Utility  funcs
		//////////////////////////////////////////////////////////////////

		/// Does ZombieMMenz specific functions during conversion to zombie
		/// virtual func overrides default.
		void setSolver( Id solver, Id orig );

		static const Cinfo* initCinfo();
	private:
		Stoich* stoich_;
		double Km_; /// Km in conc units: millimolar
};

#endif // _ZOMBIE_MM_ENZ_H
