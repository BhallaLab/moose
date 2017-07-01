/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _SPIKE_STATS_H
#define _SPIKE_STATS_H

class SpikeStats: public Stats
{
	public:
		SpikeStats();

		/**
 		 * Inserts an event into the pendingEvents queue for spikes.
 		 */
		void addSpike( DataId synIndex, const double time );

		////////////////////////////////////////////////////////////////
		// Field assignment stuff.
		////////////////////////////////////////////////////////////////

		void setThreshold( double thresh );
		double getThreshold() const;

		////////////////////////////////////////////////////////////////
		// Dest Func
		////////////////////////////////////////////////////////////////
		void addSpike( double v );
		void Vm( double v );

		/// Virtual func for handling process calls.
		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );

		////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	private:
		unsigned int numSpikes_;
		double threshold_;
		bool fired_;
};

#endif // _SPIKE_STATS_H
