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

def makeModel():
    # create container for model
    model = moose.Neutral( 'model' )
    compartment = moose.CubeMesh( '/model/compartment' )
    compartment.volume = 1e-15
    # the mesh is created automatically by the compartment
    moose.le( '/model/compartment' )
    mesh = moose.element( '/model/compartment/mesh' )

    # create molecules and reactions
    a = moose.Pool( '/model/compartment/a' )
    b = moose.Pool( '/model/compartment/b' )
    c = moose.Pool( '/model/compartment/c' )
    enz1 = moose.Enz( '/model/compartment/b/enz1' )
    enz2 = moose.Enz( '/model/compartment/c/enz2' )
    cplx1 = moose.Pool( '/model/compartment/b/enz1/cplx' )
    cplx2 = moose.Pool( '/model/compartment/c/enz2/cplx' )
    reac = moose.Reac( '/model/compartment/reac' )

    # connect them up for reactions
    moose.connect( enz1, 'sub', a, 'reac' )
    moose.connect( enz1, 'prd', b, 'reac' )
    moose.connect( enz1, 'enz', b, 'reac' )
    moose.connect( enz1, 'cplx', cplx1, 'reac' )

    moose.connect( enz2, 'sub', b, 'reac' )
    moose.connect( enz2, 'prd', a, 'reac' )
    moose.connect( enz2, 'enz', c, 'reac' )
    moose.connect( enz2, 'cplx', cplx2, 'reac' )

    moose.connect( reac, 'sub', a, 'reac' )
    moose.connect( reac, 'prd', b, 'reac' )

    # connect them up to the compartment for volumes
    #for x in ( a, b, c, cplx1, cplx2 ):
    #                        moose.connect( x, 'mesh', mesh, 'mesh' )

    # Assign parameters
    a.concInit = 1
    b.concInit = 0
    c.concInit = 0.01
    enz1.kcat = 0.4
    enz1.Km = 4
    enz2.kcat = 0.6
    enz2.Km = 0.01
    reac.Kf = 0.001
    reac.Kb = 0.01

    # Create the output tables
    graphs = moose.Neutral( '/model/graphs' )
    outputA = moose.Table2 ( '/model/graphs/concA' )
    outputB = moose.Table2 ( '/model/graphs/concB' )

    # connect up the tables
    moose.connect( outputA, 'requestOut', a, 'getConc' );
    moose.connect( outputB, 'requestOut', b, 'getConc' );

    # We need a finer timestep than the default 0.1 seconds,
    # in order to get numerical accuracy.
    for i in range (11, 15 ):
        moose.setClock( i, 0.001 ) # for computational objects

def main():
    """
    This example illustrates how to define a kinetic model using the
    scripting interface. Normally one uses standard model formats like
    SBML or kkit to concisely define kinetic models, but in some cases one
    would like to modify the model through the script.
    This example creates a bistable model having two enzymes and a reaction.
    One of the enzymes is autocatalytic.
    The model is set up to run using default Exponential Euler integration.
    The snippet **scriptKineticSolver.py** uses the much better GSL 
    Runge-Kutta-Fehlberg integration scheme on this same model.
    """
    makeModel()
    moose.reinit()
    moose.start( 100.0 ) # Run the model for 100 seconds.

    a = moose.element( '/model/compartment/a' )
    b = moose.element( '/model/compartment/b' )

    # move most molecules over to b
    b.conc = b.conc + a.conc * 0.9
    a.conc = a.conc * 0.1
    moose.start( 100.0 ) # Run the model for 100 seconds.

    # move most molecules back to a
    a.conc = a.conc + b.conc * 0.99
    b.conc = b.conc * 0.01
    moose.start( 100.0 ) # Run the model for 100 seconds.

    # Iterate through all plots, dump their contents to data.plot.
    for x in moose.wildcardFind( '/model/graphs/conc#' ):
        #x.xplot( 'scriptKineticModel.plot', x.name )
        t = numpy.arange( 0, x.vector.size, 1 ) # sec
        pylab.plot( t, x.vector, label=x.name )
    pylab.legend()
    pylab.show()

    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
