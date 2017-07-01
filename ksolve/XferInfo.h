/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _XFER_INFO_H
#define _XFER_INFO_H

/**
 * Utility class holding the information required for setting up  the
 * data transfers needed on each timestep for the cross-solver reactions.
 */
class XferInfo {
	public:
		XferInfo( Id ks )
				: ksolve( ks )
		{;}

		/**
		 * Vector of the pool.n values participating in cross-compartment
		 * reactions. Latest values that have just come in.
		 */
		vector< double > values;
		/**
		 * Vector of the pool.n values participating in cross-compartment
		 * reactions. Retains the value from previous clock tick.
		 */
		vector< double > lastValues;

		/**
		 * Vector of cases where last transfer in led to a negative
		 * concentration. Track the negative value for correction in
		 * the next cycle should the remainder become positive.
		 */
		vector< double > subzero;

		/**
		 * Vector of the internal indices of pools involved in cross-
		 * compartment reactions.
		 */
		vector< unsigned int > xferPoolIdx;

		/**
		 * Vector of voxels that particpate in junctions with the
		 * communicating ksolve. This is a subset of the
		 * total number of voxels.
		 */
		vector< unsigned int > xferVoxel;

		/**
		 * Id of Ksolve that particpates in this set of
		 * cross-compartment reactions with self.
		 * This is used to identify with XferInfo to use for a given
		 * incoming message.
		 */
		Id ksolve;

};

#endif	// _XFER_INFO_H
