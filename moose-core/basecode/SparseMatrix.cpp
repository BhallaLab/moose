/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <algorithm>
#include <vector>
#include <map>
#include <cassert>
#include <iostream>
#include "SparseMatrix.h"

using namespace std;


const unsigned int SM_MAX_ROWS = 200000;
const unsigned int SM_MAX_COLUMNS = 200000;
const unsigned int SM_RESERVE = 8;
