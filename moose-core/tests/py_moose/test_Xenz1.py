# -*- coding: utf-8 -*-
#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
##
##   Endo    /  Compartment
##           \
##  sub--enz-/-----> prd
##  sub<-----\----- prd
##           /
#########################################################################


import math
import numpy as np
import moose
import moose.fixXreacs as fixXreacs

subInit = 0.002
eInit = 0.001
Km = 0.001
kcat = 0.2
Kf = 0.1

def makeModel():
    # create container for model
    num = 1 # number of compartments
    model = moose.Neutral( '/model' )
    compartment = moose.CylMesh( '/model/compartment' )
    compartment.x1 = 1.0e-6 # Set it to a 1 micron single-voxel cylinder

    # create molecules and reactions
    prd = moose.Pool( '/model/compartment/prd' )
    rXfer = moose.Reac( '/model/compartment/rXfer' )
    #####################################################################
    # Put in endo compartment. Add molecule s
    endo = moose.EndoMesh( '/model/endo' )
    endo.isMembraneBound = True
    endo.surround = compartment
    sub = moose.Pool( '/model/endo/sub' )
    enzPool = moose.Pool( '/model/endo/enzPool' )
    enzPool.concInit = eInit
    enz = moose.MMenz( '/model/endo/enzPool/enz' )
    #####################################################################
    moose.connect( enz, 'sub', sub, 'reac' )
    moose.connect( enz, 'prd', prd, 'reac' )
    moose.connect( enzPool, 'nOut', enz, 'enzDest' )
    moose.connect( rXfer, 'sub', prd, 'reac' )
    moose.connect( rXfer, 'prd', sub, 'reac' )
    rXfer.Kf = Kf # 0.04/sec
    rXfer.Kb = 0.0 # 0.02/sec
    enz.Km = Km
    enz.kcat = kcat
    # v = es.kcat/(s+Km)
    # v = Kf * conc.
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
    sub.vec.concInit = subInit
    enzPool.vec.concInit = eInit

    estoich = moose.Stoich( '/model/endo/stoich' )
    estoich.compartment = endo
    estoich.ksolve = eksolve
    estoich.dsolve = edsolve
    estoich.path = "/model/endo/##"
    assert( edsolve.numPools == 3 )

    edsolve.buildMeshJunctions( dsolve )

    plot1 = moose.Table2( '/model/plot1' )
    plot2 = moose.Table2( '/model/plot2' )
    moose.connect( '/model/plot1', 'requestOut', sub, 'getN' )
    moose.connect( '/model/plot2', 'requestOut', prd, 'getN' )
    plot3 = moose.Table2( '/model/plot3' )
    plot4 = moose.Table2( '/model/plot4' )
    moose.connect( '/model/plot3', 'requestOut', sub, 'getConc' )
    moose.connect( '/model/plot4', 'requestOut', prd, 'getConc' )

def almostEq( a, b ):
    #print a, b, (a-b)/(a+b)
    return abs(a-b)/(a+b) < 5e-5

def test_xreac1():
    for i in range( 10, 18 ):
        moose.setClock( i, 0.001 )
    runtime = 100
    makeModel()
    moose.reinit()
    moose.start( runtime )
    assert( almostEq( moose.element( 'model/compartment/prd' ).n,
        moose.element( '/model/endo/sub' ).n ) )
    moose.delete( '/model' )

if __name__ == '__main__':
    test_xreac1()
