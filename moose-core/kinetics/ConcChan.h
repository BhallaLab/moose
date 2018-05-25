/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2018 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Public License version 3 or later.
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CONC_CHAN_H
#define _CONC_CHAN_H

class ConcChan
{
	public:
		ConcChan();
		~ConcChan();


		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void setPermeability( double v );
		double getPermeability() const;
		void setNumChan( double v );
		double getNumChan() const;
		double getFlux() const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref& e, ProcPtr p );
		void inPool( double conc );
		void outPool( double conc );
		//////////////////////////////////////////////////////////////////
		// Utility function
		//////////////////////////////////////////////////////////////////
		double flux( double inConc, double outConc, double dt );

		static const Cinfo* initCinfo();
	protected:
		double permeability_;	/// permeability in vol/(#.s) units.
		double n_;		/// Number of molecules of channel.
		double flux_;	/// Flux of molecule through channel
		double charge_;	/// Later: for including Nernst calculations
		double Vm_;		/// Later: for including Nernst calculations
};

#endif // CONC_CHAN_H
