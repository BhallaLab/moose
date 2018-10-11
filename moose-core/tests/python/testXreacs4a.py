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
    #####################################################################
    # Put in endo compartment. Add molecule s
    endo = moose.EndoMesh( '/model/endo' )
    endo.isMembraneBound = True
    endo.surround = compartment
    es = moose.Pool( '/model/endo/s' )
    rXfer = moose.Reac( '/model/endo/rXfer' )
    #####################################################################
    moose.connect( rXfer, 'sub', s, 'reac' )
    moose.connect( rXfer, 'prd', es, 'reac' )
    volRatio = compartment.volume / endo.volume
    rXfer.Kf = 0.04 # 0.04/sec
    rXfer.Kb = 0.02 # 0.02/sec

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
    assert( dsolve.numPools == 1 )
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

def doPlot( ax, plot1, plot2, label ):
    plt.ylabel( label )
    plt.xlabel( 'time(s)' )
    v1 = moose.element(plot1).vector
    v2 = moose.element(plot2).vector
    ax.plot( v1, label='s' )
    ax.plot( v2, label='es' )
    ax.plot( np.array( v1 ) + np.array( v2 ), label='sum' )
    plt.legend()

def almostEq( a, b ):
    return abs(a-b)/(a+b) < 5e-5

def main( standalone = False ):
    runtime = 200
    displayInterval = 2
    makeModel()
    moose.reinit()
    moose.start( runtime )
    assert( almostEq( 2.0 * moose.element( 'model/compartment/s' ).conc,
        moose.element( '/model/endo/s' ).conc ) )

    if standalone:
        fig = plt.figure( figsize=(12,10) )
        ax1 = fig.add_subplot(211)
        doPlot( ax1, '/model/plot1', '/model/plot2', '# of molecules' )
        ax2 = fig.add_subplot(212)
        doPlot( ax2, '/model/plot3', '/model/plot4', 'conc (mM)' )
        plt.show()
    moose.delete( '/model' )


# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main( standalone = True )
