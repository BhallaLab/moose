/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ENZ_H
#define _ENZ_H

class Enz: public CplxEnzBase
{
	public:
		Enz();
		~Enz();

		//////////////////////////////////////////////////////////////////
		// Virtual field stuff to overwrite EnzBase
		//////////////////////////////////////////////////////////////////
		void vSetKm( const Eref& e, double v );
		double vGetKm( const Eref& e ) const;
		void vSetNumKm( const Eref& e, double v );
		double vGetNumKm( const Eref& e ) const;
		void vSetKcat( const Eref& e, double v );
		double vGetKcat( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Virtual field stuff to override CplxEnzBase
		//////////////////////////////////////////////////////////////////
		void vSetK1( const Eref& e, double v );
		double vGetK1( const Eref& e ) const;
		void vSetK2( const Eref& e, double v );
		double vGetK2( const Eref& e ) const;
		void vSetRatio( const Eref& e, double v );
		double vGetRatio( const Eref& e ) const;
		void vSetConcK1( const Eref& e, double v );
		double vGetConcK1( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs, not virtual
		//////////////////////////////////////////////////////////////////
		void setKmK1( double Km, double k1 );

		//////////////////////////////////////////////////////////////////
		// Dest funcs, all virtual
		//////////////////////////////////////////////////////////////////

		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );
		void vSub( double n );
		void vEnz( double n );
		void vPrd( double n );
		void vCplx( double n );
		void vRemesh( const Eref& e );

		//////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();
	private:
		double Km_;	/// Km in concentration units: millimolar.
		double k1_;	/// in # and time units
		double k2_;	/// in time
		double k3_;	/// in time
		double r1_;
		double r2_;
		double r3_;
};

#endif // ENZ_H
