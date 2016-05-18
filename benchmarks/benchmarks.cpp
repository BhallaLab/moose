/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <iostream>
using namespace std;

void runKineticsBenchmark1( const string& method );
void testIntFireNetwork( unsigned int runsteps );

void mooseBenchmarks( unsigned int option )
{
	switch ( option ) {
		case 1:
			cout << "Kinetics benchmark 1: small model, Exp Euler, 10Ksec, OSC_Cspace.g\n";
			runKineticsBenchmark1( "ee" );
			break;
		case 2:
			cout << "Kinetics benchmark 1: small model, Gsl Runge Kutta Fehlberg, 10Ksec, OSC_Cspace.g\n";
			runKineticsBenchmark1( "gsl" );
			break;
		case 3:
			cout << "Kinetics benchmark 1: small model, Gssa, 10Ksec, OSC_Cspace.g\n";
			runKineticsBenchmark1( "gssa" );
			break;

		case 4:
			cout << "intFire benchmark: 104576 synapses, pconnect = 0.1, 2e5 timesteps\n";
			testIntFireNetwork( 200000 );
			break;
		default:
			cout << "Unknown benchmark specified, quitting\n";
			break;
	}
}
