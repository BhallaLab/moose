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
						unsigned int chan, double perm )
				: myPool( my ), otherPool( other ), chanPool( chan ), 
				permeability( perm )
		{;}

		unsigned int myPool;
		unsigned int otherPool;
		unsigned int chanPool;
		double permeability;
};

#endif // _CONC_CHAN_INFO_H
