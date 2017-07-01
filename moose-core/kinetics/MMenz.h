/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MM_ENZ_H
#define _MM_ENZ_H

/**
 * This class represents the Michaelis-Menten type enzyme, obeying the
 * equation
 * V = kcat.[Etot].[S]/( Km + [S] )
 */
class MMenz: public EnzBase
{
	public:
		MMenz();
		virtual ~MMenz();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void vSetKm( const Eref& e, double v );
		double vGetKm( const Eref& e ) const;
		void vSetNumKm( const Eref& e, double v );
		double vGetNumKm( const Eref& e ) const;
		void vSetKcat( const Eref& e, double v );
		double vGetKcat(const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );
		void vSub( double n );
		void vEnz( double n );
		void vRemesh( const Eref& e );

		static const Cinfo* initCinfo();
	private:
		double Km_; /// Km in Concentration units, millimolar.
		double numKm_; /// Km in number units
		double kcat_; /// kcat in 1/sec
		double sub_;	/// State variable: substrate (in num units) * numKm
		double enz_;	/// State variable: enz number.
};

#endif // MM_ENZ_H
