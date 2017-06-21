/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MSG_DIGEST_H
#define _MSG_DIGEST_H

/**
 * This class manages digested Messages. Each entry is boiled down to the
 * function, and an array of targets. The targets are actually stored
 * on the Msg, but they referenced in the MsgDigest.
 * As a further refinement, if the target DataIndex is ALLDATA, then it
 * means that all data entries in the target are to be iterated over. Note
 * that this does not extend to Field targets.
 */
class MsgDigest
{
	public:
		MsgDigest( const OpFunc* f, const vector< Eref >& t )
				: func( f ), targets( t )
		{;}
		const OpFunc* func;
		vector< Eref > targets;
};

#endif // _MSG_DIGEST_H
