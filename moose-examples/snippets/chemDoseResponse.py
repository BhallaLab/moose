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

def main():
    """
    This example builds a dose-response of a bistable model of a chemical
    system. It uses the kinetic solver *Ksolve* and the steady-state finder
    *SteadyState*.
    The model is set up within the script.

    The basic approach is to increment the control variable, **a** in this
    case, while monitoring **b**.
    The algorithm marches through a series of values of the buffered pool 
    **a** and measures resultant values of pool **b**. At each cycle
    the algorithm calls the steady-state finder. Since **a** is incremented
    only a small amount on each step, each new steady state is 
    (usually) quite close to the previous one. The exception is when there
    is a state transition. 

    Here we plot three dose-response curves to illustrate the bistable
    nature of the system. 

    On the upward going curve in blue, **a** starts low. Here, 
    **b** follows the low arm of the curve
    and then jumps up to the high value at roughly *log( [a] ) = -0.55*.

    On the downward going curve in green, **b** follows the high arm
    of the curve forming a nice hysteretic loop.
    Eventually **b** has to fall to the low state at about 
    *log( [a] ) = -0.83*

    Through nasty concentration manipulations, we find the third arm 
    of the curve, which tracks the unstable fixed point. This is in red.
    We find this arm by
    setting an initial point close to the unstable fixed point, which
    the steady-state finder duly locates. We then follow a dose-response
    curve as with the other arms of the curve. 

    Note that the steady-state solver doesn't always succeed in finding a
    good solution, despite moving only in small steps. Nevertheless the
    resultant curves are smooth because it gives up pretty close to the
    correct value, simply because the successive points are close together.
    Overall, the system is pretty robust despite the core root-finder
    computations in GSL being temperamental.

    In doing a production dose-response series
    you may wish to sample concentration space logarithmically rather than
    linearly.
    """
    compartment = makeModel()
    ksolve = moose.Ksolve( '/model/compartment/ksolve' )
    stoich = moose.Stoich( '/model/compartment/stoich' )
    stoich.compartment = compartment
    stoich.ksolve = ksolve
    stoich.path = "/model/compartment/##"
    state = moose.SteadyState( '/model/compartment/state' )

    moose.reinit()
    state.stoich = stoich
    state.convergenceCriterion = 1e-6
    moose.seed( 111 ) # Used when generating the samples in state space

    b = moose.element( '/model/compartment/b' )
    a = moose.element( '/model/compartment/a' )
    c = moose.element( '/model/compartment/c' )
    a.concInit = 0.1
    deltaA = 0.002
    num = 150
    avec = []
    bvec = []
    moose.reinit()

    # Now go up.
    for i in range( 0, num ):
        moose.start( 1.0 ) # Run the model for 1 seconds.
        state.settle() # This function finds the steady states.
        avec.append( a.conc )
        bvec.append( b.conc )
        a.concInit += deltaA
        #print i, a.conc, b.conc
    pylab.plot( numpy.log10( avec ), numpy.log10( bvec ), label='b vs a up' )
    # Now go down.
    avec = []
    bvec = []
    for i in range( 0, num ):
        moose.start( 1.0 ) # Run the model for 1 seconds.
        state.settle() # This function finds the steady states.
        avec.append( a.conc )
        bvec.append( b.conc )
        a.concInit -= deltaA
        #print i, a.conc, b.conc


    pylab.plot( numpy.log10( avec ), numpy.log10( bvec ), label='b vs a down' )
    # Now aim for the middle. We do this by judiciously choosing a 
    # start point that should be closer to the unstable fixed point.
    avec = []
    bvec = []
    a.concInit = 0.28
    b.conc = 0.15
    for i in range( 0, 65 ):
        moose.start( 1.0 ) # Run the model for 1 seconds.
        state.settle() # This function finds the steady states.
        avec.append( a.conc )
        bvec.append( b.conc )
        a.concInit -= deltaA
        #print i, a.conc, b.conc
    pylab.plot( numpy.log10( avec ), numpy.log10( bvec ), label='b vs a mid' )

    pylab.ylim( [-1.7, 1.2] )
    pylab.legend()
    pylab.show()

    quit()

def makeModel():
    """ This function creates a bistable reaction system using explicit
    MOOSE calls rather than load from a file.
    The reaction is::

        a ---b---> 2b    # b catalyzes a to form more of b.
        2b ---c---> a    # c catalyzes b to form a.
        a <======> 2b    # a interconverts to b.

    """
    # create container for model
    model = moose.Neutral( 'model' )
    compartment = moose.CubeMesh( '/model/compartment' )
    compartment.volume = 1e-15
    # the mesh is created automatically by the compartment
    mesh = moose.element( '/model/compartment/mesh' ) 

    # create molecules and reactions
    a = moose.BufPool( '/model/compartment/a' )
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
    moose.connect( enz1, 'prd', b, 'reac' ) # Note 2 molecules of b.
    moose.connect( enz1, 'enz', b, 'reac' )
    moose.connect( enz1, 'cplx', cplx1, 'reac' )

    moose.connect( enz2, 'sub', b, 'reac' )
    moose.connect( enz2, 'sub', b, 'reac' ) # Note 2 molecules of b.
    moose.connect( enz2, 'prd', a, 'reac' )
    moose.connect( enz2, 'enz', c, 'reac' )
    moose.connect( enz2, 'cplx', cplx2, 'reac' )

    moose.connect( reac, 'sub', a, 'reac' )
    moose.connect( reac, 'prd', b, 'reac' )
    moose.connect( reac, 'prd', b, 'reac' ) # Note 2 order in b.

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

    return compartment

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
