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
						unsigned int chan, double perm, bool isSwapped, bool _isLocal )
				: myPool( my ), otherPool( other ), chanPool( chan ), 
				swapped( isSwapped ),
				isLocal( _isLocal ),
				permeability( perm )
		{;}

		unsigned int myPool; /// This is not Id, it is internal PoolIndex
		/// Obtained using convertIdToPoolIndex.
		unsigned int otherPool; /// Internal PoolIndex
		unsigned int chanPool; /// Internal PoolIndex 
		bool swapped;	// Flag records whether myPool/otherPool are 
						// swapped wrt inPool/outPool. System expects
						// that inPool should be in same compt as chanPool.
		bool isLocal;	// Flag, records odd cases where the channel is
						// entirely local to one compartment.
		double permeability;
};

#endif // _CONC_CHAN_INFO_H
