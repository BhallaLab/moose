/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SYN_EVENT_H
#define _SYN_EVENT_H

class SynEvent
{
	public:
		SynEvent()
			: time( 0.0 ), weight( 0.0 )
		{;}

		SynEvent( double t, double w )
			: time( t ), weight( w )
		{;}

		double time;
		double weight;
};

struct CompareSynEvent
{
	bool operator()(const SynEvent& lhs, const SynEvent& rhs) const
	{
		// Note that this is backwards. We want the smallest timestamp
		// on the top of the events priority_queue.
		return lhs.time > rhs.time;
	}
};

class PreSynEvent: public SynEvent
{
	public:
		PreSynEvent()
			: SynEvent(),   // call the parent constructor with default args
                            // by default calls without args, so no need really
              synIndex( 0 )
		{}

		PreSynEvent( unsigned int i, double t, double w )
			: SynEvent(t,w),// call the parent constructor with given args
              synIndex( i )
		{;}

        unsigned int synIndex;
};

class PostSynEvent
{
	public:
		PostSynEvent()
			: time( 0.0 )
		{;}

		PostSynEvent( double t )
			: time( t )
		{;}

		double time;
};

struct ComparePostSynEvent
{
	bool operator()(const PostSynEvent& lhs, const PostSynEvent& rhs) const
	{
		// Note that this is backwards. We want the smallest timestamp
		// on the top of the events priority_queue.
		return lhs.time > rhs.time;
	}
};

#endif // _SYN_EVENT_H
