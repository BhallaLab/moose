# -*- coding: utf-8 -*-
#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
## Here test diffusion across a junction. We set up a pump and change its
## rate to do a check on the calculation of the diffusion rate separate
## from the verification that diffusion gives equal concs on either side.
## The system is a ## 1-voxel cyl compt and an internal endoMesh,
## having a volume = 1/8 of the parent cylinder..
##                      \
##    COMPT:    s ----> / s  :ENDO
##                s =diffn= s
##                      \
##
## This becomes:
##                                  \
##    COMPT:    s ----> s_xfer_endo / s     :ENDO
#                             s =diffn= s
##                                  \
##
#########################################################################


import math
import numpy as np
import moose
print( '[INFO] Using moose from %s, %s' % (moose.__file__, moose.version()) )
import moose.fixXreacs as fixXreacs

diffConst = 1e-16

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
    endo.isMembraneBound = False
    endo.surround = compartment
    es = moose.Pool( '/model/endo/s' )
    #####################################################################
    moose.connect( rXfer, 'sub', s, 'reac' )
    moose.connect( rXfer, 'prd', es, 'reac' )
    volRatio = compartment.volume / endo.volume
    #print( "Volume ratio = {}".format( volRatio ) )
    rXfer.Kf = 0.0 # 0.02/sec
    rXfer.Kb = 0.0 #
    s.concInit = 0.001
    s.diffConst = diffConst
    es.diffConst = diffConst

    #####################################################################
    fixXreacs.fixXreacs( '/model' )
    #fixXreacs.restoreXreacs( '/model' )
    #fixXreacs.fixXreacs( '/model' )
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
    #  print( a, b, (a-b)/(a+b) )
    return abs(a-b)/(a+b) < 6e-5

def main( standalone = False ):
    runtime = 100
    makeModel()
    for i in range( 10, 18 ):
        moose.setClock( i, 0.01 )
    moose.reinit()
    moose.start( runtime, 1 )
    s = moose.element( '/model/compartment/s' )
    es = moose.element( '/model/endo/s' )
    assert( almostEq( s.conc, es.conc ) )
    rXfer = moose.element( '/model/compartment/rXfer' )
    endo = moose.element( '/model/endo' )
    compt = moose.element( '/model/compartment' )
    endoA = endo.aScale * pow( compt.volume, endo.aPower )
    rXfer.numKf = 2.0*diffConst*endoA/(compt.r0*compt.volume)
    moose.start( runtime )
    assert almostEq( 2 * s.conc, es.conc ), "%g=?%g" % (2*s.conc, es.conc)
    moose.delete( '/model' )

def test_xdiff1():
    main()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
