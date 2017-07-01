/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CPLX_ENZ_BASE_H
#define _CPLX_ENZ_BASE_H

/**
 * This class is for mass-action enzymes, in which there is an explict
 * pool for the enzyme-substrate complex. It models the reaction
 * E + S <===> E.S ----> E + P
 */
class CplxEnzBase: public EnzBase
{
	public:
		CplxEnzBase();
		virtual ~CplxEnzBase();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////
		void setK1( const Eref& e, double v );
		double getK1( const Eref& e ) const;
		void setK2( const Eref& e, double v );
		double getK2( const Eref& e ) const;
		//void setK3( const Eref& e, double v );
		//double getK3( const Eref& e ) const;
		void setRatio( const Eref& e, double v );
		double getRatio( const Eref& e ) const;
		void setConcK1( const Eref& e, double v );
		double getConcK1( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Virtual field stuff to use as base class for Enz and ZombieEnz.
		//////////////////////////////////////////////////////////////////
		virtual void vSetK1( const Eref& e, double v ) = 0;
		virtual double vGetK1( const Eref& e ) const = 0;
		virtual void vSetK2( const Eref& e, double v ) = 0;
		virtual double vGetK2( const Eref& e ) const = 0;
		virtual void vSetRatio( const Eref& e, double v ) = 0;
		virtual double vGetRatio( const Eref& e ) const = 0;
		virtual void vSetConcK1( const Eref& e, double v ) = 0;
		virtual double vGetConcK1( const Eref& e ) const = 0;
		//////////////////////////////////////////////////////////////////
		// A new Dest function.
		//////////////////////////////////////////////////////////////////
		void cplx( double n );
		//////////////////////////////////////////////////////////////////
		// The other dest funcs, all virtual, come from EnzBase.
		//////////////////////////////////////////////////////////////////
		virtual void vCplx( double n );

		//////////////////////////////////////////////////////////////////
		// Zombification stuff, overrides the one from EnzBase.
		//////////////////////////////////////////////////////////////////
		static void zombify( Element* original, const Cinfo* zClass,
			Id solver );

		static const Cinfo* initCinfo();
	private:
};

#endif // CPLX_ENZ_BASE_H
