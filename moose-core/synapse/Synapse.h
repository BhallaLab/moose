/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SYNAPSE_H
#define _SYNAPSE_H

class SynHandlerBase;
/**
 * This is the base class for synapses. It is meant to be used as a
 * FieldElement entry on a parent object, derived from the SynHandlerBase.
 */
class Synapse
{
	public:
		Synapse();
		void setWeight( double v );
		void setDelay( double v );

		double getWeight() const;
		double getDelay() const;

		void addSpike( const Eref& e, double time );
		double getTopSpike( const Eref& e ) const;

		void setHandler( SynHandlerBase* h );

		///////////////////////////////////////////////////////////////
		static void addMsgCallback(
					const Eref& e, const string& finfoName,
					ObjId msg, unsigned int msgLookup );
		static void dropMsgCallback(
					const Eref& e, const string& finfoName,
					ObjId msg, unsigned int msgLookup );
		static const Cinfo* initCinfo();
	private:
		double weight_;
		double delay_;
		SynHandlerBase* handler_;
};

#endif // _SYNAPSE_H
