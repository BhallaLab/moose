# -*- coding: utf-8 -*-
#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
## Here test ConcChan. We set up a channel and a pump and run two tests.
## First we have zero flux through the pump and check that the conc
## equalizes on both sides of the membrane. Then we change the Kf of
## the pump to a value designed to give twice the conc In as Out.
## The system is a 1-voxel cyl compt and an internal endoMesh, having a
## volume = 1/8 of the cylinder.
##                      \
##    COMPT:    s ----> / s  :ENDO
##                 s =chan= s
##                      \
##
## This becomes:
##                                  \
##    COMPT:    s ----> s_xfer_endo / s     :ENDO
##                             s =chan= s
##                                  \
##
#########################################################################

import math
import numpy as np
import moose
print( '[INFO ] Using moose from %s, %s' % (moose.__file__, moose.version()) )
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
    # Note that the chanPool must be on the 'In' compartment.
    #chanPool = moose.Pool( '/model/compartment/chanPool' )
    #chan = moose.ConcChan( '/model/compartment/chanPool/chan' )
    chanPool = moose.Pool( '/model/endo/chanPool' )
    chan = moose.ConcChan( '/model/endo/chanPool/chan' )
    endo.isMembraneBound = True
    endo.surround = compartment
    es = moose.Pool( '/model/endo/s' )
    #####################################################################
    moose.connect( rXfer, 'sub', s, 'reac' )
    moose.connect( rXfer, 'prd', es, 'reac' )
    moose.connect( chanPool, 'nOut', chan, 'setNumChan' )
    moose.connect( chan, 'out', s, 'reac' )
    moose.connect( chan, 'in', es, 'reac' )
    volRatio = compartment.volume / endo.volume
    rXfer.Kf = 0.0 # 0.02/sec
    rXfer.Kb = 0.0 #
    s.concInit = 0.001
    chanPool.nInit = 1000.0
    # Flux (#/s) = permeability * N * (#out/vol_out - #in/vol_in)
    chan.permeability = 0.1 * chanPool.volume / chanPool.nInit

    #####################################################################
    fixXreacs.fixXreacs( '/model' )
    #fixXreacs.restoreXreacs( '/model' )
    #fixXreacs.fixXreacs( '/model' )
    #####################################################################

    # Make solvers
    ksolve = moose.Ksolve( '/model/compartment/ksolve' )
    dsolve = moose.Dsolve( '/model/dsolve' )
    eksolve = moose.Ksolve( '/model/endo/ksolve' )
    eksolve.method = 'gsl'
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
    assert( edsolve.numPools == 2 )

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

def doPlot( ax, i, label ):
    scale = 1
    if i > 3:
        scale = 1000 # Just to plot in uM.
    plot1 = '/model/plot' + str(i)
    plot2 = '/model/plot' + str(i+1)
    plot3 = '/model/plot' + str(i+2)
    plt.ylabel( label )
    plt.xlabel( 'time(s)' )
    v1 = moose.element(plot1).vector * scale
    v2 = moose.element(plot2).vector * scale
    v3 = moose.element(plot3).vector * scale
    ax.plot( v1, label='s' )
    ax.plot( v2, 'x',label='es' )
    ax.plot( v3, label='xfer' )
    ax.plot( np.array( v1 ) + np.array( v2 ), label='sum' )
    plt.legend()


def almostEq( a, b ):
    #print a, b, (a-b)/(a+b)
    return abs(a-b)/(a+b) < 5e-5

def main( standalone = False ):
    runtime = 100
    displayInterval = 2
    makeModel()
    for i in range( 10, 18 ):
        moose.setClock( i, 0.01 )
    moose.reinit()
    moose.start( runtime )
    s = moose.element( '/model/compartment/s' )
    es = moose.element( '/model/endo/s' )
    assert almostEq( s.conc, es.conc ), 'Asserting %g=%g' % (s.conc, es.conc)
    # We go for concEndo = 2x concOut. Then
    # We already know volIn = volOut/8
    # #in/volIn = 2x #out/volOut
    # #in/volIn = 2x #out/(8*volIn)
    # so #in = #out/4
    # From consv, #in + #out = nInit
    # so 5/4#out = nInit =>
    #       #out = 0.8*nInit; #in = 0.2*nInit
    #
    # flux = perm * nChan * (0.8nInit/volOut - 0.2nInit/(volOut/8) ) =
    #        perm * (nChan*nInit/volOut) * (0.8 - 0.2*8) =
    #        perm * (nChan*nInit/volOut) * (-0.8)
    #
    # This has to be balanced by flux = numKf * nOut = numKf * 0.8 * nInit
    # So 0.8*numKf*nInit = 0.8*perm*nChan*nInit/volOut

    # Note that chan.permeability = 0.1*chanPool.volume / chanPool.nInit

    # so numKf = perm*nChan/volOut = (0.1*volIn/nChan)*nChan/volOut=0.1/8
    #
    rXfer = moose.element( '/model/compartment/rXfer' )
    rXfer.numKf = 0.1/8.0
    moose.start( runtime )
    assert( almostEq( 2 * s.conc, es.conc ) )

    if standalone:
        fig = plt.figure( figsize=(12,10) )
        ax1 = fig.add_subplot(211)
        doPlot( ax1, 1, '# of molecules' )
        ax2 = fig.add_subplot(212)
        doPlot( ax2, 4, 'conc (uM)' )
        plt.show()
    moose.delete( '/model' )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main( standalone = True )
