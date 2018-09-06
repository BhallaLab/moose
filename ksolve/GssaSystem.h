/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GSSA_SYSTEM_H
#define _GSSA_SYSTEM_H

/**
 * This class stores the common data needed across all voxels for doing
 * GSSA calculations.
 */

class Stoich;
class GssaSystem
{
public:
    GssaSystem()
        : stoich(0), useRandInit(true), isReady(false), honorMassConservation(true)
    {;}
    vector< vector< unsigned int > > dependency;
    vector< vector< unsigned int > > dependentMathExpn;
    vector< vector< unsigned int > > ratesDependentOnPool;

    /// Transpose of stoichiometry matrix.
    KinSparseMatrix transposeN;
    Stoich* stoich;

    /**
     * Flag: True when using probabilistic (random) rounding.
     * When initializing the mol# from floating-point Sinit values,
     * we have two options. One is to look at each Sinit, and round
     * to the nearest integer. The other is to look at each Sinit,
     * and probabilistically round up or down depending on the
     * value. For example, if we had a Sinit value of 1.49,
     * this would always be rounded to 1.0 if the flag is false,
     * and would be rounded to 1.0 and 2.0 in the ratio 51:49 if
     * the flag is true.
     */
    bool useRandInit = true;

    /**
     * Flag: True when all initialization is done.
     */
    bool isReady = false;

    /* When set to true, it makes sure that after rounding number of molecules,
     * the sum of molecules is does not differ more than 1.0 molecules.
     */
    bool honorMassConservation = true;
};

#endif	// _GSSA_SYSTEM_H
