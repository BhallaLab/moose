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
    makeModel()
    for i in range( 10, 18 ):
        moose.setClock( i, 0.01 )
    moose.reinit()
    moose.start( runtime )
    s = moose.element( '/model/compartment/s' )
    es = moose.element( '/model/endo/s' )
    assert( almostEq( s.conc, es.conc ) )
    # We go for concEndo = 2x concOut. Then
    # We already know volIn = volOut/8
    # #in/volIn = 2x #out/volOut
    # #in/volIn = 2x #out/(8*volIn)
    # so #in = #out/4
    # From consv, #in + #out = nInit
    # so 5/4#out = nInit =>
    #       #out = 0.8*nInit; #in = 0.2*nInit
    #
    # Diffn flux = D * (xa/L) * (0.8nInit/volOut - 0.2nInit/(volOut/8) )
    #   = -D * (xa/L) * (nInit/volOut) * 0.8
    #       We can obtain xa and L from the endo/mesh
    #
    # This has to be balanced by flux = numKf * nOut = numKf * 0.8 * nInit
    # So 0.8*numKf*nInit = 0.8*(D.Xa/L)*(nInit/volOut)
    # So numKf = D.Xa/(L*volOut)
    #
    rXfer = moose.element( '/model/compartment/rXfer' )
    endo = moose.element( '/model/endo' )
    compt = moose.element( '/model/compartment' )
    endoA = endo.aScale * pow( compt.volume, endo.aPower )
    rXfer.numKf = 2.0*diffConst*endoA/(compt.r0*compt.volume)
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
