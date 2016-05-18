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
    compt0.subTreePath = cell.path + "/#"
    moose.showfields( compt0 )

    # Build the solvers. No need for diffusion in this version.
    ksolve0 = moose.Ksolve( '/model/chem/compt0/ksolve' )
    dsolve0 = moose.Dsolve( '/model/chem/compt0/dsolve' )
    stoich0 = moose.Stoich( '/model/chem/compt0/stoich' )

    # Configure solvers
    stoich0.compartment = compt0
    stoich0.ksolve = ksolve0
    stoich0.dsolve = dsolve0
    stoich0.path = '/model/chem/compt0/#'
    assert( stoich0.numVarPools == 3 )
    assert( stoich0.numProxyPools == 0 )
    assert( stoich0.numRates == 4 )
    
    num = compt0.numDiffCompts - 1
    moose.element( '/model/chem/compt0/a[' + str(num) + ']' ).concInit *= 1.5

    # Create the output tables
    graphs = moose.Neutral( '/model/graphs' )
    makeTab( 'a_soma', '/model/chem/compt0/a[0]' )
    makeTab( 'b_soma', '/model/chem/compt0/b[0]' )
    makeTab( 'a_apical', '/model/chem/compt0/a[' + str( num ) + ']' )
    makeTab( 'b_apical', '/model/chem/compt0/b[' + str( num ) + ']' )

def makeTab( plotname, molpath ):
    tab = moose.Table2( '/model/graphs/' + plotname ) # Make output table
    # connect up the tables
    moose.connect( tab, 'requestOut', moose.element( molpath ), 'getConc' );


def makeDisplay():
        plt.ion()
        fig = plt.figure( figsize=(10,12) )

        layout = fig.add_subplot( 211 )
        plt.ylabel( 'x position + 10*conc' )
        plt.xlabel( 'y position (microns)' )
        timeLabel = plt.text(0, 20, 'time = 0')
        layout.set_xlim( -5, 75 )
        layout.set_ylim( -20, 25 )
        compt = moose.element( '/model/chem/compt0' )
        pos = compt.voxelMidpoint
        i = len( pos ) / 3
        r2 = numpy.sqrt( 0.5 )
        yp = [ -r2 * pos[j] * 1e6 for j in range( i ) ]
        xp = pos[i:2*i] * 1e6 - yp
        #xp = [ pos[i + j] for j in range( i ) ]
        #yp = [ -r2 * pos[j] for j in range( i ) ]
        #line0, = layout.plot( pos[:i], pos[i:2*i] , 'bo' )
        line, = layout.plot( xp, yp, 'bo' )

        timeSeries = fig.add_subplot( 212 )
        timeSeries.set_ylim( 0, 0.6 )
        plt.ylabel( 'Conc (mM)' )
        plt.xlabel( 'time (seconds)' )

        fig.canvas.draw()
        return ( timeSeries, fig, line, timeLabel, yp )

def updateDisplay( plotlist ):
	a = moose.vec( '/model/chem/compt0/a' )
	b = moose.vec( '/model/chem/compt0/b' )
        plotlist[2].set_ydata( a.conc * 10 + plotlist[4]  )

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
    """
    This function sets up a simple oscillatory chemical system within
    the script. The reaction system is::

        s ---a---> a  // s goes to a, catalyzed by a.
        s ---a---> b  // s goes to b, catalyzed by a.
        a ---b---> s  // a goes to s, catalyzed by b.
        b -------> s  // b is degraded irreversibly to s.

    in sum, **a** has a positive feedback onto itself and also forms **b**.
    **b** has a negative feedback onto **a**.
    Finally, the diffusion constant for **a** is 1/10 that of **b**.
    """
    # create container for model
    diffConst = 10e-12 # m^2/sec
    motorRate = 1e-6 # m/sec
    concA = 1 # millimolar
    
    # create molecules and reactions
    a = moose.Pool( compt.path + '/a' )
    b = moose.Pool( compt.path + '/b' )
    s = moose.Pool( compt.path + '/s' )
    e1 = moose.MMenz( compt.path + '/e1' )
    e2 = moose.MMenz( compt.path + '/e2' )
    e3 = moose.MMenz( compt.path + '/e3' )
    r1 = moose.Reac( compt.path + '/r1' )

    a.concInit = 0.1
    b.concInit = 0.1
    s.concInit = 1

    moose.connect( e1, 'sub', s, 'reac' )
    moose.connect( e1, 'prd', a, 'reac' )
    moose.connect( a, 'nOut', e1, 'enzDest' )
    e1.Km = 1
    e1.kcat = 1

    moose.connect( e2, 'sub', s, 'reac' )
    moose.connect( e2, 'prd', b, 'reac' )
    moose.connect( a, 'nOut', e2, 'enzDest' )
    e2.Km = 1
    e2.kcat = 0.5

    moose.connect( e3, 'sub', a, 'reac' )
    moose.connect( e3, 'prd', s, 'reac' )
    moose.connect( b, 'nOut', e3, 'enzDest' )
    e3.Km = 0.1
    e3.kcat = 1

    moose.connect( r1, 'sub', b, 'reac' )
    moose.connect( r1, 'prd', s, 'reac' )
    r1.Kf = 0.3 # 1/sec
    r1.Kb = 0 # 1/sec

    # Assign parameters
    a.diffConst = diffConst/10
    b.diffConst = diffConst
    s.diffConst = 0

def main():
    """
    reacDiffBranchingNeuron:
    This example illustrates how to define a kinetic model embedded in
    the branching pseudo 1-dimensional geometry of a neuron. This means
    that diffusion only happens along the axis of dendritic segments, not
    radially from inside to outside a dendrite, nor tangentially around
    the dendrite circumference. The model 
    oscillates in space and time due to a Turing-like reaction-diffusion
    mechanism present in all compartments. For the sake of this demo,
    the initial conditions are set to be slightly different on one of the
    terminal dendrites, so as to break the symmetry and initiate 
    oscillations.
    This example uses an external model file to specify a binary branching 
    neuron. This model does not have any spines. The electrical model is 
    used here purely for the geometry and is not part of the computations.
    In this example we build an identical chemical model throughout the
    neuronal geometry, using the makeChemModel function.
    The model is set up to run using the Ksolve for integration and the
    Dsolve for handling diffusion.

    The display has two parts:

        a. Animated pseudo-3D plot of neuronal geometry, where each point
           represents a diffusive voxel and moves in the y-axis to show
           changes in concentration.
        b. Time-series plot that appears after the simulation has 
           ended. The plots are for the first and last diffusive voxel,
           that is, the soma and the tip of one of the apical dendrites.

    """
    chemdt = 0.1 # Tested various dts, this is reasonable.
    diffdt = 0.01
    plotdt = 1
    animationdt = 5
    runtime = 750 

    makeModel()
    plotlist = makeDisplay()

    # Schedule the whole lot. Autoscheduling already takes care of these
    '''
    for i in range( 11, 17 ):
        moose.setClock( i, chemdt ) # for the chem objects
    moose.setClock( 10, diffdt ) # for the diffusion
    moose.setClock( 18, plotdt ) # for the output tables.
    '''
    moose.reinit()
    for i in range( 0, runtime, animationdt ):
        moose.start( animationdt )
        plotlist[3].set_text( "time = %d" % i )
        updateDisplay( plotlist )

    finalizeDisplay( plotlist, plotdt )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
