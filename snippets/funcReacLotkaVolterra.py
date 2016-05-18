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

runtime = 50.0
def makeModel():
		# create container for model
		model = moose.Neutral( 'model' )
		harmonic = moose.CubeMesh( '/model/harmonic' )
		harmonic.volume = 1e-15
		lotka = moose.CubeMesh( '/model/lotka' )
		lotka.volume = 1e-15

		# create molecules and reactions
		x = moose.Pool( '/model/lotka/x' )
		y = moose.Pool( '/model/lotka/y' )
                z = moose.BufPool( '/model/lotka/z' ) # Dummy molecule.
                xreac = moose.Reac( '/model/lotka/xreac' )
                yreac = moose.Reac( '/model/lotka/yreac' )
		xrate = moose.Function( '/model/lotka/xreac/func' )
		yrate = moose.Function( '/model/lotka/yreac/func' )

                # Parameters
                alpha = 1.0
                beta = 1.0
                gamma = 1.0
                delta = 1.0
                k = 1.0
                x.nInit = 2.0
                y.nInit = 1.0
                z.nInit = 0.0
                xrate.x.num = 1
                yrate.x.num = 1
                xrate.expr = "x0 * " + str( beta ) + " - " + str( alpha )
                yrate.expr = str( gamma ) + " - x0 * " + str( delta )
                xreac.Kf = k
                yreac.Kf = k
                xreac.Kb = 0
                yreac.Kb = 0

		# connect them up for reactions
		moose.connect( y, 'nOut', xrate.x[0], 'input' )
		moose.connect( x, 'nOut', yrate.x[0], 'input' )
                moose.connect( xrate, 'valueOut', xreac, 'setNumKf' )
                moose.connect( yrate, 'valueOut', yreac, 'setNumKf' )
                moose.connect( xreac, 'sub', x, 'reac' )
                moose.connect( xreac, 'prd', z, 'reac' )
                moose.connect( yreac, 'sub', y, 'reac' )
                moose.connect( yreac, 'prd', z, 'reac' )

		# Create the output tables
		graphs = moose.Neutral( '/model/graphs' )
		xplot = moose.Table2 ( '/model/graphs/x' )
		yplot = moose.Table2 ( '/model/graphs/y' )

		# connect up the tables
		moose.connect( xplot, 'requestOut', x, 'getN' );
		moose.connect( yplot, 'requestOut', y, 'getN' );

def main():
    """
    The funcReacLotkaVolterra example shows how to use function objects
    as part of differential equation systems in the framework of the MOOSE
    kinetic solvers. Here the system is set up explicitly using the 
    scripting, in normal use one would expect to use SBML.
    
    In this example we set up a Lotka-Volterra system. The equations
    are readily expressed as a pair of reactions each of whose rate is
    governed by a function::
    
            x' = x( alpha - beta.y )
            y' = -y( gamma - delta.x )
    
    This translates into two reactions::
    
            x ---> z        Kf = beta.y - alpha
            y ---> z        Kf = gamma - delta.x
            
    Here z is a dummy molecule whose concentration is buffered to zero.
    
    The model first runs using default Exponential Euler integration.
    This is not particularly accurate even with a small timestep.
    The model is then converted to use the deterministic Kinetic solver
    Ksolve. This is accurate and faster.
    Note that we cannot use the stochastic GSSA solver for this system, it
    cannot handle a reaction term whose rate keeps changing.
    """
    makeModel()

    for i in range( 11, 18 ):
        moose.setClock( i, 0.001 )
    moose.setClock( 18, 0.1 )
    moose.reinit()
    moose.start( runtime ) # Run the model

    # Iterate through all plots, dump their contents to data.plot.
    for x in moose.wildcardFind( '/model/graphs/#' ):
        #x.xplot( 'scriptKineticModel.plot', x.name )
        t = numpy.arange( 0, x.vector.size, 1 ) * x.dt # sec
        pylab.plot( t, x.vector, label=x.name )
    pylab.ylim( 0, 2.5 )
    pylab.title( "Exponential Euler solution. Note slight error buildup" )
    pylab.legend()


    pylab.figure()
    compt = moose.element( '/model/lotka' )
    ksolve = moose.Ksolve( '/model/lotka/ksolve' )
    stoich = moose.Stoich( '/model/lotka/stoich' )
    stoich.compartment = compt
    stoich.ksolve = ksolve
    stoich.path = '/model/lotka/##'
    moose.reinit()
    moose.start( runtime ) # Run the model

    for i in range( 11, 18 ):
        moose.setClock( i, 0.1 )
    for x in moose.wildcardFind( '/model/graphs/#' ):
        t = numpy.arange( 0, x.vector.size, 1 ) * x.dt # sec
        pylab.plot( t, x.vector, label=x.name )
    pylab.ylim( 0, 2.5 )
    pylab.title( "Runge-Kutta solution." )
    pylab.legend()
    pylab.show()

    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
