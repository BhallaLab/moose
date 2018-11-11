/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This file contains the MPI-handling functions in Shell.
 */

#ifdef USE_MPI
#include <mpi.h>
#endif
#include "../basecode/header.h"
#include "Shell.h"
#include "../basecode/Dinfo.h"

#define USE_NODES 1

///////////////////////////////////////////////////////////////////

/**
 * Launches Parser. Blocking when the parser blocks.
 */
void Shell::launchParser()
{
	Id shellId;
	Shell* s = reinterpret_cast< Shell* >( shellId.eref().data() );
	bool quit = 0;

	cout << "moose : " << flush;
	while ( !quit ) {
		string temp;
		cin >> temp;
		if ( temp == "quit" || temp == "q" ) {
			s->doQuit();
			quit = 1;
		}
	}
	cout << "\nQuitting Moose\n" << flush;
}

// Function to assign hardware availability
void Shell::setHardware( unsigned int numCores, unsigned int numNodes,
	unsigned int myNode )
{
	numCores_ = numCores;
	numNodes_ = numNodes;
	myNode_ = myNode;
	acked_.resize( numNodes, 0 );
}

/**
 * Regular shell function that requires that the information about the
 * hardware have been loaded in. For now the function just assigns SimGroups
 */
void Shell::loadBalance()
{
	// Need more info here on how to set up groups distributed over
	// nodes. In fact this will have to be computed _after_ the
	// simulation is loaded. Will also need quite a bit of juggling between
	// nodes when things get really scaled up.
	//
	// Note that the messages have to be rebuilt after this call.
	// Note that this function is called independently on each node.
}

unsigned int Shell::numCores()
{
	return numCores_;
}

unsigned int Shell::numNodes()
{
	return numNodes_;
}

unsigned int Shell::myNode()
{
	return myNode_;
}

bool Shell::inBlockingParserCall()
{
	return isBlockedOnParser_;
}
