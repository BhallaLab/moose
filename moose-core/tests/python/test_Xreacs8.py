# -*- coding: utf-8 -*-
#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
## Here we set up equal Kf and Kb in a reversible reaction across a
## 1-voxel cyl compt and an internal endoMesh, having a smaller volume.
##                        \
##    COMPT:    2 s <===> / 2 s  :ENDO
##                        \
##
## This becomes:
##                                      \
##    COMPT:    2 s <===> 2 s_xfer_endo / 2s     :ENDO
##                                      \
##
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
    #####################################################################
    moose.connect( rXfer, 'sub', s, 'reac' )
    moose.connect( rXfer, 'sub', s, 'reac' )
    moose.connect( rXfer, 'prd', es, 'reac' )
    moose.connect( rXfer, 'prd', es, 'reac' )
    rXfer.Kf = 0.01 # 0.01/sec
    rXfer.Kb = 0.01 # 0.01/sec

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
    assert( edsolve.numPools == 1 )

    edsolve.buildMeshJunctions( dsolve )

    plot1 = moose.Table2( '/model/plot1' )
    plot2 = moose.Table2( '/model/plot2' )
    plot3 = moose.Table2( '/model/plot3' )
    moose.connect( '/model/plot1', 'requestOut', s, 'getN' )
    moose.connect( '/model/plot2', 'requestOut', es, 'getN' )
    moose.connect( '/model/plot3', 'requestOut', '/model/compartment/s_xfer_endo', 'getN' )
    plot4 = moose.Table2( '/model/plot4' )
    plot5 = moose.Table2( '/model/plot5' )
    plot6 = moose.Table2( '/model/plot6' )
    moose.connect( '/model/plot4', 'requestOut', s, 'getConc' )
    moose.connect( '/model/plot5', 'requestOut', es, 'getConc' )
    moose.connect( '/model/plot6', 'requestOut', '/model/compartment/s_xfer_endo', 'getConc' )

def almostEq( a, b ):
    return abs(a-b)/(a+b) < 5e-5

def main( standalone = False ):
    runtime = 100
    makeModel()
    moose.reinit()
    moose.start( runtime )
    assert( almostEq( moose.element( 'model/compartment/s' ).conc,
        moose.element( '/model/endo/s' ).conc ) )
    moose.delete( '/model' )


# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main( standalone = True )
