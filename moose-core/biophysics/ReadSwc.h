/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2015 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef READSWC_H
#define READSWC_H

/**
 * This class is used to read in SWC files as found in
 * NeuroMorph.org and other places. Note that these are only passive
 * models at this stage.
 */

class ReadSwc
{
	/**
	 * Each line is of the form
	 * n T x y z R P
	 */
	public:
		ReadSwc( const string& fname );
		bool validate() const;
		void assignKids();
		void cleanZeroLength();
		void parseBranches();
		void traverseBranch( const SwcSegment& s, double &len, double& L,
					   vector< int >& cable	) const;
		bool build( Id parent,
					double lambda, double RM, double RA, double CM );
		void diagnostics() const;

	private:
		vector< SwcSegment > segs_;

		/**
		 * branches holds the NeuroMorpho index of the end segment of
		 * each branch. Note that NeuroMorph index starts from 1.
		 * To traverse the branch, go to the end segment, and traverse
		 * through all parents till you get to a fork.
		 */
		vector< SwcBranch > branches_;
};

#endif // READSWC_H
