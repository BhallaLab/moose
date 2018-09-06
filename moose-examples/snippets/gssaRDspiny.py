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

#diffConst = 10e-12 # m^2/sec
diffConst = 0.0
def makeModel():
    model = moose.Neutral( '/model' )
    # Make neuronal model. It has no channels, just for geometry
    cell = moose.loadModel( './spinyNeuron.p', '/model/cell', 'Neutral' )
    # We don't want the cell to do any calculations. Disable everything.
    for i in moose.wildcardFind( '/model/cell/##' ):
        i.tick = -1

    # create container for model
    model = moose.element( '/model' )
    chem = moose.Neutral( '/model/chem' )
    # The naming of the compartments is dicated by the places that the
    # chem model expects to be loaded.
    compt0 = moose.NeuroMesh( '/model/chem/compt0' )
    compt0.separateSpines = 1
    compt0.geometryPolicy = 'cylinder'
    compt1 = moose.SpineMesh( '/model/chem/compt1' )
    moose.connect( compt0, 'spineListOut', compt1, 'spineList', 'OneToOne' )
    compt2 = moose.PsdMesh( '/model/chem/compt2' )
    moose.connect( compt0, 'psdListOut', compt2, 'psdList', 'OneToOne' )

    #reacSystem = moose.loadModel( 'simpleOsc.g', '/model/chem', 'ee' )
    makeChemModel( compt0 ) # Populate all 3 compts with the chem system.
    makeChemModel( compt1 )
    makeChemModel( compt2 )

    compt0.diffLength = 2e-6 # This will be over 100 compartments.
    # This is the magic command that configures the diffusion compartments.
    compt0.subTreePath = cell.path + "/##"
    moose.showfields( compt0 )

    # Build the solvers. No need for diffusion in this version.
    ksolve0 = moose.Ksolve( '/model/chem/compt0/ksolve' )
    ksolve1 = moose.Gsolve( '/model/chem/compt1/ksolve' )
    ksolve2 = moose.Gsolve( '/model/chem/compt2/ksolve' )
    #dsolve0 = moose.Dsolve( '/model/chem/compt0/dsolve' )
    #dsolve1 = moose.Dsolve( '/model/chem/compt1/dsolve' )
    #dsolve2 = moose.Dsolve( '/model/chem/compt2/dsolve' )
    stoich0 = moose.Stoich( '/model/chem/compt0/stoich' )
    stoich1 = moose.Stoich( '/model/chem/compt1/stoich' )
    stoich2 = moose.Stoich( '/model/chem/compt2/stoich' )

    # Configure solvers
    stoich0.compartment = compt0
    stoich1.compartment = compt1
    stoich2.compartment = compt2
    stoich0.ksolve = ksolve0
    stoich1.ksolve = ksolve1
    stoich2.ksolve = ksolve2
    #stoich0.dsolve = dsolve0
    #stoich1.dsolve = dsolve1
    #stoich2.dsolve = dsolve2
    stoich0.path = '/model/chem/compt0/#'
    stoich1.path = '/model/chem/compt1/#'
    stoich2.path = '/model/chem/compt2/#'
    assert( stoich0.numVarPools == 3 )
    assert( stoich0.numProxyPools == 0 )
    assert( stoich0.numRates == 4 )
    assert( stoich1.numVarPools == 3 )
    assert( stoich1.numProxyPools == 0 )
    #assert( stoich1.numRates == 4 )
    assert( stoich2.numVarPools == 3 )
    assert( stoich2.numProxyPools == 0 )
    #assert( stoich2.numRates == 4 )
    #dsolve0.buildNeuroMeshJunctions( dsolve1, dsolve2 )
    stoich0.buildXreacs( stoich1 )
    stoich1.buildXreacs( stoich2 )
    stoich0.filterXreacs()
    stoich1.filterXreacs()
    stoich2.filterXreacs()

    moose.element( '/model/chem/compt2/a[0]' ).concInit *= 1.5

    # Create the output tables
    num = compt0.numDiffCompts - 1
    graphs = moose.Neutral( '/model/graphs' )
    makeTab( 'a_soma', '/model/chem/compt0/a[0]' )
    makeTab( 'b_soma', '/model/chem/compt0/b[0]' )
    makeTab( 'a_apical', '/model/chem/compt0/a[' + str( num ) + ']' )
    makeTab( 'b_apical', '/model/chem/compt0/b[' + str( num ) + ']' )
    makeTab( 'a_spine', '/model/chem/compt1/a[5]' )
    makeTab( 'b_spine', '/model/chem/compt1/b[5]' )
    makeTab( 'a_psd', '/model/chem/compt2/a[5]' )
    makeTab( 'b_psd', '/model/chem/compt2/b[5]' )

def makeTab( plotname, molpath ):
    tab = moose.Table2( '/model/graphs/' + plotname ) # Make output table
    # connect up the tables
    moose.connect( tab, 'requestOut', moose.element( molpath ), 'getConc' );


def makeDisplay():
    plt.ion()
    fig = plt.figure( figsize=(10,12) )

    dend = fig.add_subplot( 411 )
    plt.ylabel( 'Conc (mM)' )
    plt.xlabel( 'Dend voxel #' )
    plt.legend()
    timeLabel = plt.text(200, 0.5, 'time = 0')

    spine = fig.add_subplot( 412 )
    plt.ylabel( 'Conc (mM)' )
    plt.xlabel( 'Spine voxel #' )
    plt.legend()

    psd = fig.add_subplot( 413 )
    plt.ylabel( 'Conc (mM)' )
    plt.xlabel( 'PSD voxel #' )
    plt.legend()

    timeSeries = fig.add_subplot( 414 )
    timeSeries.set_ylim( 0, 0.6 )
    plt.ylabel( 'Conc (mM)' )
    plt.xlabel( 'time (seconds)' )
    plt.legend()

    a = moose.vec( '/model/chem/compt0/a' )
    b = moose.vec( '/model/chem/compt0/b' )
    line1, = dend.plot( list(range( len( a ))), a.conc, label='a' )
    line2, = dend.plot( list(range( len( b ))), b.conc, label='b' )
    dend.set_ylim( 0, 0.6 )

    a = moose.vec( '/model/chem/compt1/a' )
    b = moose.vec( '/model/chem/compt1/b' )
    line3, = spine.plot( list(range( len( a ))), a.conc, label='a' )
    line4, = spine.plot( list(range( len( b ))), b.conc, label='b' )
    spine.set_ylim( 0, 0.6 )

    a = moose.vec( '/model/chem/compt2/a' )
    b = moose.vec( '/model/chem/compt2/b' )
    line5, = psd.plot( list(range( len( a ))), a.conc, label='a' )
    line6, = psd.plot( list(range( len( b ))), b.conc, label='b' )
    psd.set_ylim( 0, 0.6 )

    fig.canvas.draw()
    return ( timeSeries, dend, spine, psd, fig, line1, line2, line3, line4, line5, line6, timeLabel )

def updateDisplay( plotlist ):
    a = moose.vec( '/model/chem/compt0/a' )
    b = moose.vec( '/model/chem/compt0/b' )
    plotlist[5].set_ydata( a.conc )
    plotlist[6].set_ydata( b.conc )
    print(('compt0:', min(a.n), max(a.n), min(b.n), max(b.n)))

    a = moose.vec( '/model/chem/compt1/a' )
    b = moose.vec( '/model/chem/compt1/b' )
    plotlist[7].set_ydata( a.conc )
    plotlist[8].set_ydata( b.conc )
    print(('compt1:', min(a.n), max(a.n), min(b.n), max(b.n)))

    a = moose.vec( '/model/chem/compt2/a' )
    b = moose.vec( '/model/chem/compt2/b' )
    plotlist[9].set_ydata( a.conc )
    plotlist[10].set_ydata( b.conc )
    plotlist[4].canvas.draw()
    print(('compt2:', min(a.n), max(a.n), min(b.n), max(b.n)))


def finalizeDisplay( plotlist, cPlotDt ):
    for x in moose.wildcardFind( '/model/graphs/#[ISA=Table2]' ):
        pos = numpy.arange( 0, x.vector.size, 1 ) * cPlotDt
        line1, = plotlist[0].plot( pos, x.vector, label=x.name )
    plotlist[4].canvas.draw()
    print( "Hit any key to exit" )
    try:
        raw_input( )
    except NameError as e:
        input( )

def makeChemModel( compt ):
    """
    This function setus up a simple oscillatory chemical system within
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
    This example illustrates how to define a kinetic model embedded in
    the branching pseudo-1-dimensional geometry of a neuron. The model
    oscillates in space and time due to a Turing-like reaction-diffusion
    mechanism present in all compartments. For the sake of this demo,
    the initial conditions are set up slightly different on the PSD
    compartments, so as to break the symmetry and initiate oscillations
    in the spines.
    This example uses an external electrical model file with basal
    dendrite and three branches on
    the apical dendrite. One of those branches has a dozen or so spines.
    In this example we build an identical model in each compartment, using
    the makeChemModel function. One could readily define a system with
    distinct reactions in each compartment.
    The model is set up to run using the Ksolve for integration and the
    Dsolve for handling diffusion.
    The display has four parts:

        a. animated line plot of concentration against main compartment#.
        b. animated line plot of concentration against spine compartment#.
        c. animated line plot of concentration against psd compartment#.
        d. time-series plot that appears after the simulation has
           ended. The plot is for the last (rightmost) compartment.

    """
    chemdt = 0.1 # Tested various dts, this is reasonable.
    diffdt = 0.01
    plotdt = 1
    animationdt = 5
    runtime = 200

    makeModel()

    plotlist = makeDisplay()

    # Schedule the whole lot - autoscheduling already does this.
    '''
    for i in range( 11, 17 ):
        moose.setClock( i, chemdt ) # for the chem objects
    moose.setClock( 10, diffdt ) # for the diffusion
    moose.setClock( 18, plotdt ) # for the output tables.
    '''
    moose.reinit()

    for i in range( 0, runtime, animationdt ):
        moose.start( animationdt )
        plotlist[11].set_text( "time = %d" % i )
        updateDisplay( plotlist )

    finalizeDisplay( plotlist, plotdt )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
