/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _STDP_SYNAPSE_H
#define _STDP_SYNAPSE_H

class SynHandlerBase;
/**
 * This is the base class for synapses with STDP. It is meant to be used as a
 * FieldElement entry on a parent object, derived from the STDPSynHandlerBase.
 */
class STDPSynapse: public Synapse
{
	public:
		STDPSynapse();

		void setAPlus( double v );
		double getAPlus() const;

		void setHandler( SynHandlerBase* h );
		static const Cinfo* initCinfo();

	private:
		double aPlus_;
		SynHandlerBase* handler_;
};

#endif // _STDP_SYNAPSE_H
