# -*- coding: utf-8 -*-
#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################


import math
import numpy as np
import moose
import moose.fixXreacs as fixXreacs

def makeModel():
    # create container for model
    num = 1 # number of compartments
    model = moose.Neutral( '/model' )
    compartment = moose.CylMesh( '/model/compartment' )
    compartment.x1 = 1.0e-6 # Set it to a 1 micron single-voxel cylinder

    # create molecules and reactions
    s = moose.Pool( '/model/compartment/s' )
    rXfer = moose.Reac( '/model/compartment/rXfer' )
    #####################################################################
    # Put in endo compartment. Add molecule s
    endo = moose.EndoMesh( '/model/endo' )
    endo.isMembraneBound = True
    endo.surround = compartment
    es = moose.Pool( '/model/endo/s' )
    erXfer = moose.Reac( '/model/endo/erXfer' )
    #####################################################################
    moose.connect( erXfer, 'sub', es, 'reac' )
    moose.connect( erXfer, 'prd', s, 'reac' )
    moose.connect( rXfer, 'sub', s, 'reac' )
    moose.connect( rXfer, 'prd', es, 'reac' )
    rXfer.Kf = 0.04 # 0.04/sec
    rXfer.Kb = 0.0 # 0.00/sec
    erXfer.Kf = 0.16 # 0.16/sec
    erXfer.Kb = 0.0 # 0.00/sec

    #####################################################################
    fixXreacs.fixXreacs( '/model' )
    fixXreacs.restoreXreacs( '/model' )
    fixXreacs.fixXreacs( '/model' )
    #####################################################################

    # Make solvers
    ksolve = moose.Ksolve( '/model/compartment/ksolve' )
    dsolve = moose.Dsolve( '/model/dsolve' )
    eksolve = moose.Ksolve( '/model/endo/ksolve' )
    edsolve = moose.Dsolve( '/model/endo/dsolve' )

    stoich = moose.Stoich( '/model/compartment/stoich' )
    stoich.compartment = compartment
    stoich.ksolve = ksolve
    stoich.dsolve = dsolve
    stoich.path = "/model/compartment/##"
    assert( dsolve.numPools == 2 )
    s.vec.concInit = [1]*num

    estoich = moose.Stoich( '/model/endo/stoich' )
    estoich.compartment = endo
    estoich.ksolve = eksolve
    estoich.dsolve = edsolve
    estoich.path = "/model/endo/##"
    assert( edsolve.numPools == 2 )

    edsolve.buildMeshJunctions( dsolve )

    plot1 = moose.Table2( '/model/plot1' )
    plot2 = moose.Table2( '/model/plot2' )
    moose.connect( '/model/plot1', 'requestOut', s, 'getN' )
    moose.connect( '/model/plot2', 'requestOut', es, 'getN' )
    plot3 = moose.Table2( '/model/plot3' )
    plot4 = moose.Table2( '/model/plot4' )
    moose.connect( '/model/plot3', 'requestOut', s, 'getConc' )
    moose.connect( '/model/plot4', 'requestOut', es, 'getConc' )

def almostEq( a, b ):
    #print a, b, (a-b)/(a+b)
    return abs(a-b)/(a+b) < 5e-5

def test_xreac5():
    for i in range( 10, 18):
        moose.setClock( i, 0.001 )
    runtime = 100
    makeModel()
    moose.reinit()
    moose.start( runtime )

    e1 = moose.element( 'model/compartment/s' )
    e2 = moose.element( 'model/endo/s' )
    assert almostEq(2.0 * e1.conc, e2.conc), \
            "Expected %g, Got %g" % (e1.conc, e2.conc )

    moose.delete( '/model' )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    test_xreac5()
