#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

import math
import pylab
import numpy
import moose

runtime = 120.0
def makeModel():
		# create container for model
		model = moose.Neutral( 'model' )
		harmonic = moose.CubeMesh( '/model/harmonic' )
		harmonic.volume = 1e-15
		lotka = moose.CubeMesh( '/model/lotka' )
		lotka.volume = 1e-15

		# create molecules and reactions
		p = moose.Pool( '/model/harmonic/p' )
		v = moose.Pool( '/model/harmonic/v' )
		pdot = moose.Function( '/model/harmonic/p/func' )
		vdot = moose.Function( '/model/harmonic/v/func' )

                # Parameters
                offset1 = 1.0
                offset2 = 1.0
                k = 0.1
                p.nInit = offset1
                v.nInit = offset2 + 0.1
                pdot.x.num = 1
                vdot.x.num = 1
                pdot.expr = "x0 - " + str( offset1 )
                vdot.expr = "-" + str( k ) + " * (x0 - " + str( offset2 ) + ")"

		# connect them up for reactions
		moose.connect( p, 'nOut', vdot.x[0], 'input' )
		moose.connect( v, 'nOut', pdot.x[0], 'input' )
                moose.connect( vdot, 'valueOut', v, 'increment' )
                moose.connect( pdot, 'valueOut', p, 'increment' )

		# Create the output tables
		graphs = moose.Neutral( '/model/graphs' )
		pplot = moose.Table2 ( '/model/graphs/p' )
		vplot = moose.Table2 ( '/model/graphs/v' )

		# connect up the tables
		moose.connect( pplot, 'requestOut', p, 'getN' );
		moose.connect( vplot, 'requestOut', v, 'getN' );

def main():

	"""
	funcRateHarmonicOsc illustrates the use of function objects to
        directly define the rates of change of pool concentration. This
        example shows how to set up a simple harmonic oscillator system
        of differential equations using the script. In normal use one would
        prefer to use SBML.

        The equations are ::

	        p' = v - offset1
	        v' = -k(p - offset2)

	where the rates for Pools p and v are computed using Functions.
	Note the use of offsets. This is because MOOSE chemical
	systems cannot have negative concentrations.
	
	The model is set up to run using default Exponential Euler 
	integration, and then using the GSL deterministic solver.
	"""
	makeModel()

        for i in range( 11, 18 ):
            moose.setClock( i, 0.01 )
        moose.setClock( 18, 0.1 )
	moose.reinit()
	moose.start( runtime ) # Run the model

	# Iterate through all plots, dump their contents to data.plot.
	for x in moose.wildcardFind( '/model/graphs/#' ):
		#x.xplot( 'scriptKineticModel.plot', x.name )
		t = numpy.arange( 0, x.vector.size, 1 ) * x.dt # sec
		pylab.plot( t, x.vector, label=x.name )
        pylab.suptitle( "Integration using ee" )
	pylab.legend()
        pylab.figure()

        compt = moose.element( '/model/harmonic' )
        ksolve = moose.Ksolve( '/model/harmonic/ksolve' )
        stoich = moose.Stoich( '/model/harmonic/stoich' )
        stoich.compartment = compt
        stoich.ksolve = ksolve
        stoich.path = '/model/harmonic/##'
        for i in range( 11, 18 ):
            moose.setClock( i, 0.1 )
	moose.reinit()
	moose.start( runtime ) # Run the model

	for x in moose.wildcardFind( '/model/graphs/#' ):
		t = numpy.arange( 0, x.vector.size, 1 ) * x.dt # sec
		pylab.plot( t, x.vector, label=x.name )
        pylab.suptitle( "Integration using gsl" )
	pylab.legend()
	pylab.show()

	quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
