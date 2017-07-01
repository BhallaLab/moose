/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _REAC_H
#define _REAC_H

class Reac: public ReacBase
{
	public:
		Reac();
		// Reac( double kf, double kb );

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void vSetNumKf( const Eref&e, double v );
		double vGetNumKf( const Eref& e ) const;
		void vSetNumKb( const Eref&e, double v );
		double vGetNumKb( const Eref& e ) const;

		/// set Kf in concentration units
		void vSetConcKf( const Eref& e, double v );
		/// vGet Kf in concentration units
		double vGetConcKf( const Eref& e ) const;

		/// set Kb in concentration units
		void vSetConcKb( const Eref& e, double v );
		/// get Kb in concentration units
		double vGetConcKb( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );
		void vSub( double v );
		void vPrd( double v );
		void vRemesh( const Eref& e );

		static const Cinfo* initCinfo();
	private:
		double kf_;	// Used for EE method, but secondary to the ConcKf
		double kb_;	// Used for EE method, but secondary to the ConcKf
		double sub_;	// State variable
		double prd_;	// State variable
};

#endif // REAC_H
