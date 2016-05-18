/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifdef DO_UNIT_TESTS

#include <queue>
#include "header.h"
#include "Synapse.h"
#include "SynHandlerBase.h"
#include "SimpleSynHandler.h"
#include "../shell/Shell.h"
#include "../randnum/randnum.h"


// This tests stuff without using the messaging.
void testSynapse()
{
}

// This is applicable to tests that use the messaging and scheduling.
void testSynapseProcess()
{
}

#endif // DO_UNIT_TESTS
