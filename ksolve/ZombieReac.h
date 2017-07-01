/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_REAC_H
#define _ZOMBIE_REAC_H

class ZombieReac: public ReacBase
{
	public:
		ZombieReac();
		~ZombieReac();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void vSetNumKf( const Eref& e, double v );
		double vGetNumKf( const Eref& e ) const;
		void vSetNumKb( const Eref& e, double v );
		double vGetNumKb( const Eref& e ) const;

		void vSetConcKf( const Eref& e, double v );
		double vGetConcKf( const Eref& e ) const;
		void vSetConcKb( const Eref& e, double v );
		double vGetConcKb( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////
		// utility funcs
		//////////////////////////////////////////////////////////////////
		/*
		ZeroOrder* makeHalfReaction(
			Element* orig, double rate, const SrcFinfo* finfo ) const;
			*/

		void setSolver( Id solver, Id orig );

		static const Cinfo* initCinfo();
	private:
		Stoich* stoich_;
};

#endif	// _ZOMBIE_REAC_H
