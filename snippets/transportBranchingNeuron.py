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
import matplotlib.pyplot as plt
import moose

diffConst = 0.1e-12 # m^2/sec
motorConst = 0.1e-6 # m/sec
aConcInit = 1 # millimolar, at soma.
bConcInit = 10 # millimolar, at dend tips.
plotScale = 20

def findTwigs( compt ):
    pa = compt.parentVoxel
    numkids = [ 0 ] * len( pa ) 
    for i in pa:
        if i >= 0 and i < len( pa ):
            numkids[i] += 1
    twigs = []
    for i in range( len( numkids ) ):
        if numkids[i] == 0:
            twigs.append( i )
    return twigs

def makeModel():
    model = moose.Neutral( '/model' )
    # Make neuronal model. It has no channels, just for geometry
    cell = moose.loadModel( './branching.p', '/model/cell', 'Neutral' )
    # We don't want the cell to do any calculations. Disable everything.
    for i in moose.wildcardFind( '/model/cell/##' ):
        i.tick = -1

    # create container for model
    model = moose.element( '/model' )
    chem = moose.Neutral( '/model/chem' )
    # The naming of the compartments is dicated by the places that the
    # chem model expects to be loaded.
    compt0 = moose.NeuroMesh( '/model/chem/compt0' )
    compt0.separateSpines = 0
    compt0.geometryPolicy = 'cylinder'

    #reacSystem = moose.loadModel( 'simpleOsc.g', '/model/chem', 'ee' )
    makeChemModel( compt0 ) # Populate all compt with the chem system.

    compt0.diffLength = 1e-6 # This will be over 100 compartments.
    # This is the magic command that configures the diffusion compartments.
    compt0.subTreePath = "/model/cell/#"
    #compt0.cell = cell 

    # Build the solvers. No need for diffusion in this version.
    ksolve0 = moose.Ksolve( '/model/chem/compt0/ksolve' )
    dsolve0 = moose.Dsolve( '/model/chem/compt0/dsolve' )
    stoich0 = moose.Stoich( '/model/chem/compt0/stoich' )

    # Configure solvers
    stoich0.compartment = compt0
    stoich0.ksolve = ksolve0
    stoich0.dsolve = dsolve0
    stoich0.path = '/model/chem/compt0/#'
    assert( stoich0.numVarPools == 2 )
    assert( stoich0.numProxyPools == 0 )
    assert( stoich0.numRates == 0 )
    
    moose.element( '/model/chem/compt0/a[0]' ).concInit = aConcInit
    twigs = findTwigs( compt0 )
    print 'twigs = ', twigs
    for i in twigs:
        e = moose.element( '/model/chem/compt0/b[' + str(i) + ']' )
        e.concInit = bConcInit

    # Create the output tables
    graphs = moose.Neutral( '/model/graphs' )
    makeTab( 'a_soma', '/model/chem/compt0/a[0]' )
    makeTab( 'b_soma', '/model/chem/compt0/b[0]' )
    num = twigs[0]
    makeTab( 'a_apical', '/model/chem/compt0/a[' + str( num ) + ']' )
    makeTab( 'b_apical', '/model/chem/compt0/b[' + str( num ) + ']' )

def makeTab( plotname, molpath ):
    tab = moose.Table2( '/model/graphs/' + plotname ) # Make output table
    # connect up the tables
    moose.connect( tab, 'requestOut', moose.element( molpath ), 'getConc' );


def makeDisplay():
        plt.ion()
        fig = plt.figure( figsize=(10,12) )
        compt = moose.element( '/model/chem/compt0' )

        aConc = fig.add_subplot( 311 )
        plt.ylabel( 'x position + 10*aconc' )
        plt.xlabel( 'y position (microns)' )
        timeLabel = plt.text(0, 20, 'time = 0')
        aConc.set_xlim( -5, 75 )
        aConc.set_ylim( -20, 40 )
        pos = compt.voxelMidpoint
        i = len( pos ) / 3
        r2 = numpy.sqrt( 0.5 )
        yp = [ -r2 * pos[j] * 1e6 for j in range( i ) ]
        xp = pos[i:2*i] * 1e6 - yp
        aline, = aConc.plot( xp, yp, 'b.' )

        bConc = fig.add_subplot( 312 )
        plt.ylabel( 'x position + 10*bconc' )
        plt.xlabel( 'y position (microns)' )
        bConc.set_xlim( -5, 75 )
        bConc.set_ylim( -20, 40 )
        bline, = bConc.plot( xp, yp, 'r.' )

        timeSeries = fig.add_subplot( 313 )
        timeSeries.set_ylim( 0, bConcInit / 2.0 )
        plt.ylabel( 'Conc (mM)' )
        plt.xlabel( 'time (seconds)' )

        fig.canvas.draw()
        return ( timeSeries, fig, aline, bline, timeLabel, yp )

def updateDisplay( plotlist ):
	a = moose.vec( '/model/chem/compt0/a' )
	b = moose.vec( '/model/chem/compt0/b' )
        plotlist[2].set_ydata( a.conc * plotScale + plotlist[5]  )
        plotlist[3].set_ydata( b.conc * plotScale + plotlist[5]  )
        plotlist[1].canvas.draw()


def finalizeDisplay( plotlist, cPlotDt ):
    for x in moose.wildcardFind( '/model/graphs/#[ISA=Table2]' ):
        pos = numpy.arange( 0, x.vector.size, 1 ) * cPlotDt
        line1, = plotlist[0].plot( pos, x.vector, label=x.name )
    plt.legend()
    plotlist[1].canvas.draw()
    print( "Hit 'enter' to exit" )
    raw_input()

def makeChemModel( compt ):
    
    # create molecules and reactions
    a = moose.Pool( compt.path + '/a' )
    b = moose.Pool( compt.path + '/b' )

    a.concInit = 0
    b.concInit = 0

    # Assign parameters
    a.diffConst = diffConst
    a.motorConst = motorConst
    b.diffConst = diffConst
    b.motorConst = -motorConst

def main():
    """
    transportBranchingNeuron:
    This example illustrates bidirectional transport 
    embedded in the branching pseudo 1-dimensional geometry of a neuron. 
    This means that diffusion and transport only happen along the axis of 
    dendritic segments, not radially from inside to outside a dendrite, 
    nor tangentially around the dendrite circumference. 
    In this model there is a molecule **a** starting at the soma, which is
    transported out to the dendrites. There is another molecule, **b**,
    which is initially present at the dendrite tips, and is transported
    toward the soma.
    This example uses an external model file to specify a binary branching 
    neuron. This model does not have any spines. The electrical model is 
    used here purely for the geometry and is not part of the computations.
    In this example we build trival chemical model just having
    molecules **a** and **b** throughout the neuronal geometry, using 
    the makeChemModel function.
    The model is set up to run using the Ksolve for integration and the
    Dsolve for handling diffusion.

    The display has three parts:

        a. Animated pseudo-3D plot of neuronal geometry, where each point
           represents a diffusive voxel and moves in the y-axis to show
           changes in concentration of molecule a.
        b. Similar animated pseudo-3D plot for molecule b.
        c. Time-series plot that appears after the simulation has 
           ended. The plots are for the first and last diffusive voxel,
           that is, the soma and the tip of one of the apical dendrites.

    """
    plotdt = 1
    animationdt = 5
    runtime = 600 

    makeModel()
    plotlist = makeDisplay()
    # Default Scheduling works fine for this model. 
    # Chemdt = 0.1, diffdt = 0.01, plotdt = 1

    moose.reinit()
    a = moose.vec( '/model/chem/compt0/a' )
    b = moose.vec( '/model/chem/compt0/b' )
    a0 = sum( a.n )
    b0 = sum( b.n )
    for i in range( 0, runtime, animationdt ):
        moose.start( animationdt )
        plotlist[4].set_text( "time = %d" % i )
        updateDisplay( plotlist )

    print 'mass consv a = ', a0, sum( a.n ), ', b = ', b0, sum( b.n )

    finalizeDisplay( plotlist, plotdt )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
