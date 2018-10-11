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
##                      \
##    COMPT:    s <===> / s  :ENDO
##                      \
##
## This becomes:
##                                  \
##    COMPT:    s <===> s_xfer_endo / s     :ENDO
##                                  \
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
    moose.connect( rXfer, 'prd', es, 'reac' )
    volRatio = compartment.volume / endo.volume
    rXfer.Kf = 0.02 # 0.01/sec
    rXfer.Kb = 0.02 # 1/sec

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

def doPlot( ax, i, label ):
    plot1 = '/model/plot' + str(i)
    plot2 = '/model/plot' + str(i+1)
    plot3 = '/model/plot' + str(i+2)
    plt.ylabel( label )
    plt.xlabel( 'time(s)' )
    v1 = moose.element(plot1).vector
    v2 = moose.element(plot2).vector
    v3 = moose.element(plot3).vector
    ax.plot( v1, label='s' )
    ax.plot( v2, 'x',label='es' )
    ax.plot( v3, label='xfer' )
    ax.plot( np.array( v1 ) + np.array( v2 ), label='sum' )
    plt.legend()

def almostEq( a, b ):
    return abs(a-b)/(a+b) < 5e-5

def main( standalone = False ):
    runtime = 100
    makeModel()
    moose.reinit()
    moose.start( runtime )
    assert( almostEq( moose.element( 'model/compartment/s' ).conc,
        moose.element( '/model/endo/s' ).conc ) )

    if standalone:
        fig = plt.figure( figsize=(12,10) )
        ax1 = fig.add_subplot(211)
        doPlot( ax1, 1, '# of molecules' )
        ax2 = fig.add_subplot(212)
        doPlot( ax2, 4, 'conc (mM)' )
        plt.show()
    moose.delete( '/model' )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main( standalone = True )
