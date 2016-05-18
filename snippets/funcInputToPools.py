#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

"""
This example describes the special (and discouraged) use case where 
functions provide input to a reaction system. Here we have two functions of
time which control the pool # and pool rate of change, respectively:

    number of molecules of a = 1 + sin(t)
    rate of change of number of molecules of b = 10 * cos(t)

In the stochastic case one must set a special flag *useClockedUpdate* 
in order to achieve clock-triggered updates from the functions. This is
needed because the functions do not have reaction events to trigger them,
and even if there were reaction events they might not be frequent enough to
track the periodic updates. The use of this flag slows down the calculations,
so try to use a table to control a pool instead.

To run in stochastic mode: 

    python funcInputToPools

To run in deterministic mode: 

    python funcInputToPools false

"""

import math
import pylab
import numpy
import moose
import sys

def makeModel():
		if len( sys.argv ) == 1:
			useGsolve = True
                else:
			useGsolve = ( sys.argv[1] == 'True' )
		# create container for model
		model = moose.Neutral( 'model' )
		compartment = moose.CubeMesh( '/model/compartment' )
		compartment.volume = 1e-22
		# the mesh is created automatically by the compartment
		moose.le( '/model/compartment' )
		mesh = moose.element( '/model/compartment/mesh' ) 

		# create molecules and reactions
		a = moose.Pool( '/model/compartment/a' )
		b = moose.Pool( '/model/compartment/b' )

		# create functions of time
                f1 = moose.Function( '/model/compartment/f1' )
                f2 = moose.Function( '/model/compartment/f2' )

		# connect them up for reactions
		moose.connect( f1, 'valueOut', a, 'setConc' )
		moose.connect( f2, 'valueOut', b, 'increment' )

		# Assign parameters
		a.concInit = 0
		b.concInit = 1
                #f1.numVars = 1
                #f2.numVars = 1
                f1.expr = '1 + sin(t)'
                f2.expr = '10 * cos(t)'

		# Create the output tables
		graphs = moose.Neutral( '/model/graphs' )
		outputA = moose.Table2 ( '/model/graphs/nA' )
		outputB = moose.Table2 ( '/model/graphs/nB' )

		# connect up the tables
		moose.connect( outputA, 'requestOut', a, 'getN' );
		moose.connect( outputB, 'requestOut', b, 'getN' );

                # Set up the solvers
                if useGsolve:
                    gsolve = moose.Gsolve( '/model/compartment/gsolve' )
                    gsolve.useClockedUpdate = True
                else:
                    gsolve = moose.Ksolve( '/model/compartment/gsolve' )
                stoich = moose.Stoich( '/model/compartment/stoich' )
                stoich.compartment = compartment
                stoich.ksolve = gsolve
                stoich.path = '/model/compartment/##'
                '''
                '''

		# We need a finer timestep than the default 0.1 seconds, 
                # in order to get numerical accuracy.
                for i in range (10, 19 ):
		    moose.setClock( i, 0.1 ) # for computational objects

def main():
		makeModel()
                moose.seed()

		moose.reinit()
		moose.start( 50.0 ) # Run the model for 100 seconds.

		a = moose.element( '/model/compartment/a' )
		b = moose.element( '/model/compartment/b' )

		# Iterate through all plots, dump their contents to data.plot.
		for x in moose.wildcardFind( '/model/graphs/n#' ):
				#x.xplot( 'scriptKineticModel.plot', x.name )
				t = numpy.arange( 0, x.vector.size, 1 ) * x.dt # sec
				pylab.plot( t, x.vector, label=x.name )
		pylab.legend()
		pylab.show()

		quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
