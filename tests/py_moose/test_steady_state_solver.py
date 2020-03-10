# -*- coding: utf-8 -*-
from __future__ import print_function, division

# This program is part of 'MOOSE', the
# Messaging Object Oriented Simulation Environment.
#           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
# It is made available under the terms of the
# GNU Lesser General Public License version 2.1
# See the file COPYING.LIB for the full notice.
# Monday 17 September 2018 01:49:30 PM IST
# - Converted to a test script

import math
import numpy as np
import moose
print( "[INFO ] Using moose from %s" % moose.__file__ )

def test_SS_solver():
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

    aa, bb = avec, bvec
    got = np.mean(aa), np.std(aa)
    expected = 0.24899, 0.08660
    assert np.isclose(got, expected, atol = 1e-4).all(), "Got %s, expected %s" % (got, expected)
    print( "[INFO ] Test 1 PASSED" )
    

    # Now go down.
    avec = []
    bvec = []
    for i in range( 0, num ):
        moose.start( 1.0 ) # Run the model for 1 seconds.
        state.settle() # This function finds the steady states.
        avec.append( a.conc )
        bvec.append( b.conc )
        a.concInit -= deltaA

    aa, bb = avec,  bvec
    got = np.mean(aa), np.std(aa)
    expected = 0.251, 0.0866
    assert np.isclose(got, expected, atol = 1e-4).all(), "Got %s, expected %s" % (got, expected)
    print( "[INFO ] Test 2 PASSED" )

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

    aa, bb = avec, bvec
    got = np.mean(aa), np.std(aa)
    expected = 0.216, 0.03752
    assert np.isclose(got, expected, atol = 1e-4).all(), "Got %s, expected %s" % (got, expected)
    print( "[INFO ] Test 3 PASSED" )

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

def main():
    test_SS_solver()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
