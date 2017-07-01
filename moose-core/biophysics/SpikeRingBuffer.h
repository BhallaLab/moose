/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SPIKE_RING_BUFFER
#define _SPIKE_RING_BUFFER
/**
 * This ring buffer handles incoming spikes. It spans an interval equal to
 * the longest arrival delay. When a spike event is notified it puts it into
 * the slot defined by the arrival time. It just adds the weight onto the
 * existing value in this slot, assuming a linear summation of the
 * weights of coincident inputs.
 */
class SpikeRingBuffer
{
	public:
		SpikeRingBuffer();

		/// Sets up buffer parameters
		void reinit( double dt, double bufferTime );

		/// Adds spike into the buffer.
		void addSpike( double timestamp, double weight );
		/// Advances the buffer one step, returns the current weight
		double pop( double currTime );
	private:
		static const unsigned int MAXBIN;
		double dt_;
		double currTime_;
		unsigned int currentBin_;
		vector< double > weightSum_;
};
#endif // _SPIKE_RING_BUFFER
