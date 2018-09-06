/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CONC_CHAN_INFO_H
#define _CONC_CHAN_INFO_H

/**
 */
class ConcChanInfo
{
	public:
		ConcChanInfo();
		ConcChanInfo( unsigned int my, unsigned int other, 
						unsigned int chan, double perm, bool isSwapped )
				: myPool( my ), otherPool( other ), chanPool( chan ), 
				swapped( isSwapped ),
				permeability( perm )
		{;}

		unsigned int myPool;
		unsigned int otherPool;
		unsigned int chanPool;
		bool swapped;	// Flag records whether myPool/otherPool are 
						// swapped wrt inPool/outPool. System expects
						// that inPool should be in same compt as chanPool.
		double permeability;
};

#endif // _CONC_CHAN_INFO_H
