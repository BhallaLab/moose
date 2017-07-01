/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _INT_FIRE_H
#define _INT_FIRE_H


class IntFire
{
	friend void testStandaloneIntFire();
	friend void testSynapse();
	public:
		IntFire();
		IntFire( double thresh, double tau );

		/**
		 * Overrides base class function.
 		 * Inserts an event into the pendingEvents queue for spikes.
 		 */
		void innerAddSpike( unsigned int synIndex, double time );

		////////////////////////////////////////////////////////////////
		// Field assignment stuff.
		////////////////////////////////////////////////////////////////

		void setVm( double v );
		double getVm() const;
		void setTau( double v );
		double getTau() const;
		void setThresh( double v );
		double getThresh() const;
		void setRefractoryPeriod( double v );
		double getRefractoryPeriod() const;

		////////////////////////////////////////////////////////////////
		// Dest Func
		////////////////////////////////////////////////////////////////
		void activation( double v );
		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref&  e, ProcPtr p );

		static const Cinfo* initCinfo();
	private:
		double Vm_; // State variable: Membrane potential. Resting pot is 0.
		double thresh_;	// Firing threshold
		double tau_; // Time course of membrane settling.
		double refractoryPeriod_; // Minimum time between successive spikes
		double lastSpike_; // Time of last action potential.
		double activation_; // Total synaptic activation
};

#endif // _INT_FIRE_H
