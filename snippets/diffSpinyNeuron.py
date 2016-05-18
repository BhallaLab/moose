#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2015 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################


import math
import pylab
import numpy
import matplotlib.pyplot as plt
import moose

diffConst = 1e-11
chemdt = 0.001 # Tested various dts, this is reasonable.
diffdt = 0.001
plotdt = 0.01
animationdt = 0.01
runtime = 1
useGssa = False

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
    makeChemModel( compt0, True ) # Populate all 3 compts with the chem system.
    makeChemModel( compt1, False )
    makeChemModel( compt2, True )

    compt0.diffLength = 2e-6 # This will be over 100 compartments.
    # This is the magic command that configures the diffusion compartments.
    compt0.subTreePath = cell.path + "/#"
    moose.showfields( compt0 )

    # Build the solvers. No need for diffusion in this version.
    ksolve0 = moose.Ksolve( '/model/chem/compt0/ksolve' )
    if useGssa:
        ksolve1 = moose.Gsolve( '/model/chem/compt1/ksolve' )
        ksolve2 = moose.Gsolve( '/model/chem/compt2/ksolve' )
    else:
        ksolve1 = moose.Ksolve( '/model/chem/compt1/ksolve' )
        ksolve2 = moose.Ksolve( '/model/chem/compt2/ksolve' )
    dsolve0 = moose.Dsolve( '/model/chem/compt0/dsolve' )
    dsolve1 = moose.Dsolve( '/model/chem/compt1/dsolve' )
    dsolve2 = moose.Dsolve( '/model/chem/compt2/dsolve' )
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
    stoich0.dsolve = dsolve0
    stoich1.dsolve = dsolve1
    stoich2.dsolve = dsolve2
    stoich0.path = '/model/chem/compt0/#'
    stoich1.path = '/model/chem/compt1/#'
    stoich2.path = '/model/chem/compt2/#'
    assert( stoich0.numVarPools == 1 )
    assert( stoich0.numProxyPools == 0 )
    assert( stoich0.numRates == 1 )
    assert( stoich1.numVarPools == 1 )
    assert( stoich1.numProxyPools == 0 )
    if useGssa:
        assert( stoich1.numRates == 2 )
        assert( stoich2.numRates == 2 )
    else:
        assert( stoich1.numRates == 1 )
        assert( stoich2.numRates == 1 )
    assert( stoich2.numVarPools == 1 )
    assert( stoich2.numProxyPools == 0 )
    dsolve0.buildNeuroMeshJunctions( dsolve1, dsolve2 )
    stoich0.buildXreacs( stoich1 )
    stoich1.buildXreacs( stoich2 )
    stoich0.filterXreacs()
    stoich1.filterXreacs()
    stoich2.filterXreacs()
    
    Ca_input_dend = moose.vec( '/model/chem/compt0/Ca_input' )
    print len( Ca_input_dend )
    for i in range( 60 ):
        Ca_input_dend[ 3 + i * 3 ].conc = 2.0

    Ca_input_PSD = moose.vec( '/model/chem/compt2/Ca_input' )
    print len( Ca_input_PSD )
    for i in range( 5 ):
        Ca_input_PSD[ 2 + i * 2].conc = 1.0
    
    # Create the output tables
    num = compt0.numDiffCompts - 1
    graphs = moose.Neutral( '/model/graphs' )
    makeTab( 'Ca_soma', '/model/chem/compt0/Ca[0]' )
    makeTab( 'Ca_d1', '/model/chem/compt0/Ca[1]' )
    makeTab( 'Ca_d2', '/model/chem/compt0/Ca[2]' )
    makeTab( 'Ca_d3', '/model/chem/compt0/Ca[3]' )
    makeTab( 'Ca_s3', '/model/chem/compt1/Ca[3]' )
    makeTab( 'Ca_s4', '/model/chem/compt1/Ca[4]' )
    makeTab( 'Ca_s5', '/model/chem/compt1/Ca[5]' )
    makeTab( 'Ca_p3', '/model/chem/compt2/Ca[3]' )
    makeTab( 'Ca_p4', '/model/chem/compt2/Ca[4]' )
    makeTab( 'Ca_p5', '/model/chem/compt2/Ca[5]' )

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
        timeSeries.set_ylim( 0, 2 )
        plt.ylabel( 'Conc (mM)' )
        plt.xlabel( 'time (seconds)' )
        plt.legend()

	Ca = moose.vec( '/model/chem/compt0/Ca' )
	Ca_input = moose.vec( '/model/chem/compt0/Ca_input' )
        line1, = dend.plot( range( len( Ca ) ), Ca.conc, label='Ca' )
        line2, = dend.plot( range( len( Ca_input ) ), Ca_input.conc, label='Ca_input' )
        dend.set_ylim( 0, 2 )

	Ca = moose.vec( '/model/chem/compt1/Ca' )
        line3, = spine.plot( range( len( Ca ) ), Ca.conc, label='Ca' )
        spine.set_ylim( 0, 1 )

	Ca = moose.vec( '/model/chem/compt2/Ca' )
	Ca_input = moose.vec( '/model/chem/compt2/Ca_input' )
        line4, = psd.plot( range( len( Ca ) ), Ca.conc, label='Ca' )
        line5, = psd.plot( range( len( Ca_input ) ), Ca_input.conc, label='Ca_input' )
        psd.set_ylim( 0, 1 )

        fig.canvas.draw()
        return ( timeSeries, dend, spine, psd, fig, line1, line2, line3, line4, line5, timeLabel )

def updateDisplay( plotlist ):
	Ca = moose.vec( '/model/chem/compt0/Ca' )
	Ca_input = moose.vec( '/model/chem/compt0/Ca_input' )
        plotlist[5].set_ydata( Ca.conc )
        plotlist[6].set_ydata( Ca_input.conc )

	Ca = moose.vec( '/model/chem/compt1/Ca' )
        plotlist[7].set_ydata( Ca.conc )

	Ca = moose.vec( '/model/chem/compt2/Ca' )
	Ca_input = moose.vec( '/model/chem/compt2/Ca_input' )
        plotlist[8].set_ydata( Ca.conc )
        plotlist[9].set_ydata( Ca_input.conc )
        plotlist[4].canvas.draw()


def finalizeDisplay( plotlist, cPlotDt ):
    for x in moose.wildcardFind( '/model/graphs/#[ISA=Table2]' ):
        pos = numpy.arange( 0, x.vector.size, 1 ) * cPlotDt
        line1, = plotlist[0].plot( pos, x.vector, label=x.name )
    plotlist[4].canvas.draw()
    print( "Hit 'enter' to exit" )
    raw_input()

def makeChemModel( compt, doInput ):
    """
    This function setus up a simple chemical system in which Ca input 
    comes to the dend and to selected PSDs. There is diffusion between
    PSD and spine head, and between dend and spine head.
    
        Ca_input ------> Ca  // in dend and spine head only.
    """
    # create molecules and reactions
    Ca = moose.Pool( compt.path + '/Ca' )
    Ca.concInit = 0.08*1e-3
    Ca.diffConst = diffConst
    if doInput:
        Ca_input = moose.BufPool( compt.path + '/Ca_input' )
        Ca_input.concInit = 0.08*1e-3
        Ca_input.diffConst = diffConst
        rInput = moose.Reac( compt.path + '/rInput' )
        moose.connect( rInput, 'sub', Ca, 'reac' )
        moose.connect( rInput, 'prd', Ca_input, 'reac' )
        rInput.Kf = 100 # 1/sec
        rInput.Kb = 100 # 1/sec
    else:
        Ca_sink = moose.BufPool( compt.path + '/Ca_sink' )
        Ca_sink.concInit = 0.08*1e-3
        rSink = moose.Reac( compt.path + '/rSink' )
        moose.connect( rSink, 'sub', Ca, 'reac' )
        moose.connect( rSink, 'prd', Ca_sink, 'reac' )
        rSink.Kf = 10 # 1/sec
        rSink.Kb = 10 # 1/sec

def main():
    """
    This example illustrates and tests diffusion embedded in
    the branching pseudo-1-dimensional geometry of a neuron. 
    An input pattern of Ca stimulus is applied in a periodic manner both 
    on the dendrite and on the PSDs of the 13 spines. The Ca levels in
    each of the dend, the spine head, and the spine PSD are monitored.
    Since the same molecule name is used for Ca in the three compartments,
    these are automagially connected up for diffusion. The simulation
    shows the outcome of this diffusion.
    This example uses an external electrical model file with basal 
    dendrite and three branches on
    the apical dendrite. One of those branches has the 13 spines.
    The model is set up to run using the Ksolve for integration and the
    Dsolve for handling diffusion.
    The timesteps here are not the defaults. It turns out that the 
    chem reactions and diffusion in this example are sufficiently fast
    that the chemDt has to be smaller than default. Note that this example
    uses rates quite close to those used in production models.
    The display has four parts:

        a. animated line plot of concentration against main compartment#. 
        b. animated line plot of concentration against spine compartment#. 
        c. animated line plot of concentration against psd compartment#. 
        d. time-series plot that appears after the simulation has 
           ended. 

    """

    makeModel()
    plotlist = makeDisplay()

    # Schedule the whole lot - autoscheduling already does this.
    for i in range( 11, 17 ):
        moose.setClock( i, chemdt ) # for the chem objects
    moose.setClock( 10, diffdt ) # for the diffusion
    moose.setClock( 18, plotdt ) # for the output tables.
    '''
    '''
    moose.reinit()
    for i in numpy.arange( 0, runtime, animationdt ):
        moose.start( animationdt )
        plotlist[10].set_text( "time = %d" % i )
        updateDisplay( plotlist )

    finalizeDisplay( plotlist, plotdt )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
