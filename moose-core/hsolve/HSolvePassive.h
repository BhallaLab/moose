/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_PASSIVE_H
#define _HSOLVE_PASSIVE_H
#include "../basecode/header.h"
#include "../biophysics/CompartmentBase.h"
#include "../biophysics/Compartment.h"
using namespace moose; // For moose::Compartment from 'Compartment.h'
#include "HSolveUtils.h"
#include "HSolveStruct.h"
#include "HinesMatrix.h"

class HSolvePassive: public HinesMatrix
{
#ifdef DO_UNIT_TESTS
	friend void testHSolvePassive();
#endif

public:
	void setup( Id seed, double dt );
	void solve();

protected:
	// Integration
	void updateMatrix();
	void forwardEliminate();
	void backwardSubstitute();

	vector< CompartmentStruct >       compartment_;
	vector< Id >                      compartmentId_;
	vector< double >                  V_;				/**< Compartment Vm.
		* V_ is addressed using a compartment index. V_ stores the Vm value
		* of each compartment. */
	vector< TreeNodeStruct >          tree_;			/**< Tree info.
		* The tree is used to acquire various values during setup. It contains
		* the user-defined original values of all compartment parameters.
		* Therefore, it is also used during reinit. */
	map< unsigned int, InjectStruct > inject_;			/**< inject map.
		* contains the list of compartments that have current injections into
		* them. */

private:
	// Setting up of data structures
	void clear();
	void walkTree( Id seed );
	void initialize();
	void storeTree();

	// Used for unit tests.
	double getV( unsigned int row ) const;
};

#endif // _HSOLVE_PASSIVE_H
