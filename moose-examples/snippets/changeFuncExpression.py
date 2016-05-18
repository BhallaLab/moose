#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

# This example illustrates how to set up a kinetic solver and kinetic model
# using the scripting interface. Normally this would be done using the
# Shell::doLoadModel command, and normally would be coordinated by the
# SimManager as the base of the entire model.
# This example creates a bistable model having two enzymes and a reaction.
# One of the enzymes is autocatalytic.
# The model is set up to run using Exponential Euler integration.

import math
import pylab
import numpy
import moose

useY = False

def makeModel():
		# create container for model
		model = moose.Neutral( 'model' )
		compartment = moose.CubeMesh( '/model/compartment' )
		compartment.volume = 1e-15
		# the mesh is created automatically by the compartment
		mesh = moose.element( '/model/compartment/mesh' ) 

		# create molecules and reactions
                # a <----> b
                # b + 10c ---func---> d
		a = moose.Pool( '/model/compartment/a' )
		b = moose.Pool( '/model/compartment/b' )
		c = moose.Pool( '/model/compartment/c' )
		d = moose.BufPool( '/model/compartment/d' )
		reac = moose.Reac( '/model/compartment/reac' )
                func = moose.Function( '/model/compartment/d/func' )
                func.numVars = 2
                #func.x.num = 2

		# connect them up for reactions

		moose.connect( reac, 'sub', a, 'reac' )
		moose.connect( reac, 'prd', b, 'reac' )
                if useY:
                    moose.connect( func, 'requestOut', b, 'getN' )
                    moose.connect( func, 'requestOut', c, 'getN' )
                else:
                    moose.connect( b, 'nOut', func.x[0], 'input' )
                    moose.connect( c, 'nOut', func.x[1], 'input' )

                moose.connect( func, 'valueOut', d, 'setN' )
                if useY:
                    func.expr = "y0 + 10*y1"
                else:
                    func.expr = "x0 + 10*x1"

		# connect them up to the compartment for volumes
		#for x in ( a, b, c, cplx1, cplx2 ):
		#			moose.connect( x, 'mesh', mesh, 'mesh' )

		# Assign parameters
		a.concInit = 1
		b.concInit = 0.5
		c.concInit = 0.1
		reac.Kf = 0.001
		reac.Kb = 0.01

		# Create the output tables
		graphs = moose.Neutral( '/model/graphs' )
		outputA = moose.Table2 ( '/model/graphs/concA' )
		outputB = moose.Table2 ( '/model/graphs/concB' )
		outputC = moose.Table2 ( '/model/graphs/concC' )
		outputD = moose.Table2 ( '/model/graphs/concD' )

		# connect up the tables
		moose.connect( outputA, 'requestOut', a, 'getConc' );
		moose.connect( outputB, 'requestOut', b, 'getConc' );
		moose.connect( outputC, 'requestOut', c, 'getConc' );
		moose.connect( outputD, 'requestOut', d, 'getConc' );

def displayPlots():
		for x in moose.wildcardFind( '/model/graphs/conc#' ):
				t = numpy.arange( 0, x.vector.size, 1 ) #sec
				pylab.plot( t, x.vector, label=x.name )
		pylab.legend()
		pylab.show()

def main():
		makeModel()
                '''
                '''
		ksolve = moose.Ksolve( '/model/compartment/ksolve' )
		stoich = moose.Stoich( '/model/compartment/stoich' )
		stoich.compartment = moose.element( '/model/compartment' )
		stoich.ksolve = ksolve
		stoich.path = "/model/compartment/##"
		#solver.method = "rk5"
		#mesh = moose.element( "/model/compartment/mesh" )
		#moose.connect( mesh, "remesh", solver, "remesh" )
                '''
		moose.setClock( 5, 1.0 ) # clock for the solver
		moose.useClock( 5, '/model/compartment/ksolve', 'process' )
                '''

		moose.reinit()
		moose.start( 100.0 ) # Run the model for 100 seconds.
                func = moose.element( '/model/compartment/d/func' )
                if useY:
                    func.expr = "-y0 + 10*y1"
                else:
                    func.expr = "-x0 + 10*x1"
		moose.start( 100.0 ) # Run the model for 100 seconds.
                #moose.showfields( '/model/compartment/d' )
                #moose.showfields( '/model/compartment/d/func' )
                print func.x.value
                print moose.element( '/model/compartment/b' ).n

		# Iterate through all plots, dump their contents to data.plot.
		displayPlots()

		quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
