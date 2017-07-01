/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

const double PI = 3.141592653589793;

// There is experimental error in the last sig fig here.
const double NA = 6.0221415e23;

const double FaradayConst =  96485.3415; // s A / mol

const double GasConst = 8.3144621; // R, units are J/(K.mol)

/// Used by ObjId and Eref
const unsigned int ALLDATA = ~0U;

/// Used by ObjId and Eref
const unsigned int BADINDEX = ~1U;
