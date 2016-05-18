#########################################################################
#
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

def makeCompt( name, parent, dx, dy, dia ):
    RM = 1.0
    RA = 1.0
    CM = 0.01
    EM = -0.065
    pax = 0
    pay = 0
    if ( parent.className == "Compartment" ):
        pax = parent.x
        pay = parent.y
    compt = moose.Compartment( name )
    compt.x0 = pax
    compt.y0 = pay
    compt.z0 = 0
    compt.x = pax + dx
    compt.y = pay + dy
    compt.z = 0
    compt.diameter = dia
    clen = numpy.sqrt( dx * dx + dy * dy )
    compt.length = clen
    compt.Rm = RM / (numpy.pi * dia * clen)
    compt.Ra = RA * 4.0 * numpy.pi * clen / ( dia * dia )
    compt.Cm = CM * numpy.pi * dia * clen
    if ( parent.className == "Compartment" ):
        moose.connect( parent, 'raxial', compt, 'axial' )
    return compt


def makeNeuron( numSeg ):
    segmentLength = 1e-6
    segmentDia = 1e-6
    shaftLength = 1e-6
    shaftDia = 0.2e-6
    headLength = 0.5e-6
    headDia = 0.5e-6
    
    cell = moose.Neutral( '/model/cell' )
    model = moose.element( '/model' )
    prev = makeCompt( '/model/cell/soma', 
            model, 0.0, segmentLength, segmentDia )
    dend = prev
    for i in range( 0, numSeg ):
        name = '/model/cell/dend' + str( i )
        dend = makeCompt( name, dend, 0.0, segmentLength, segmentDia )
        name = '/model/cell/shaft' + str( i )
        shaft = makeCompt( name, dend, shaftLength, 0.0, shaftDia )
        name = '/model/cell/head' + str( i )
        head = makeCompt( name, shaft, headLength, 0.0, headDia )
    return cell

def makeModel():
                numSeg = 5
                diffConst = 0.0
		# create container for model
		model = moose.Neutral( 'model' )
		compt0 = moose.NeuroMesh( '/model/compt0' )
                compt0.separateSpines = 1
                compt0.geometryPolicy = 'cylinder'
		compt1 = moose.SpineMesh( '/model/compt1' )
                moose.connect( compt0, 'spineListOut', compt1, 'spineList', 'OneToOne' )
		compt2 = moose.PsdMesh( '/model/compt2' )
                moose.connect( compt0, 'psdListOut', compt2, 'psdList', 'OneToOne' )

		# create molecules and reactions
		a = moose.Pool( '/model/compt0/a' )
		b = moose.Pool( '/model/compt1/b' )
		c = moose.Pool( '/model/compt2/c' )
		reac0 = moose.Reac( '/model/compt0/reac0' )
		reac1 = moose.Reac( '/model/compt1/reac1' )

		# connect them up for reactions
		moose.connect( reac0, 'sub', a, 'reac' )
		moose.connect( reac0, 'prd', b, 'reac' )
		moose.connect( reac1, 'sub', b, 'reac' )
		moose.connect( reac1, 'prd', c, 'reac' )

		# Assign parameters
		a.diffConst = diffConst
		b.diffConst = diffConst
		c.diffConst = diffConst
		a.concInit = 1
		b.concInit = 12.1
		c.concInit = 1
		reac0.Kf = 1
		reac0.Kb = 1
		reac1.Kf = 1
		reac1.Kb = 1

                # Create a 'neuron' with a dozen spiny compartments.
                elec = makeNeuron( numSeg )
                # assign geometry to mesh
                compt0.diffLength = 10e-6
                #compt0.cell = elec
                compt0.subTreePath = elec.path + "/##"

                # Build the solvers. No need for diffusion in this version.
                ksolve0 = moose.Ksolve( '/model/compt0/ksolve' )
                ksolve1 = moose.Ksolve( '/model/compt1/ksolve' )
                ksolve2 = moose.Ksolve( '/model/compt2/ksolve' )
                stoich0 = moose.Stoich( '/model/compt0/stoich' )
                stoich1 = moose.Stoich( '/model/compt1/stoich' )
                stoich2 = moose.Stoich( '/model/compt2/stoich' )

                # Configure solvers
                stoich0.compartment = compt0
                stoich1.compartment = compt1
                stoich2.compartment = compt2
                stoich0.ksolve = ksolve0
                stoich1.ksolve = ksolve1
                stoich2.ksolve = ksolve2
                stoich0.path = '/model/compt0/#'
                stoich1.path = '/model/compt1/#'
                stoich2.path = '/model/compt2/#'
                assert( stoich0.numVarPools == 1 )
                assert( stoich0.numProxyPools == 1 )
                assert( stoich0.numRates == 1 )
                assert( stoich1.numVarPools == 1 )
                assert( stoich1.numProxyPools == 1 )
                assert( stoich1.numRates == 1 )
                assert( stoich2.numVarPools == 1 )
                assert( stoich2.numProxyPools == 0 )
                assert( stoich2.numRates == 0 )
                stoich0.buildXreacs( stoich1 )
                stoich1.buildXreacs( stoich2 )
                stoich0.filterXreacs()
                stoich1.filterXreacs()
                stoich2.filterXreacs()


                print a.vec.volume, b.vec.volume, c.vec.volume
		a.vec.concInit = range( numSeg + 1, 0, -1 )
		b.vec.concInit = [5.0 * ( 1 + x ) for x in range( numSeg )]
		c.vec.concInit = range( 1, numSeg + 1 )
                print a.vec.concInit, b.vec.concInit, c.vec.concInit

		# Create the output tables
		graphs = moose.Neutral( '/model/graphs' )
		outputA = moose.Table2 ( '/model/graphs/concA' )
		outputB = moose.Table2 ( '/model/graphs/concB' )
		outputC = moose.Table2 ( '/model/graphs/concC' )

		# connect up the tables
                a1 = moose.element( '/model/compt0/a[' + str( numSeg )+ ']')
                b1 = moose.element( '/model/compt1/b[' +str(numSeg - 1)+']')
                c1 = moose.element( '/model/compt2/c[' +str(numSeg - 1)+']')
		moose.connect( outputA, 'requestOut', a1, 'getConc' );
		moose.connect( outputB, 'requestOut', b1, 'getConc' );
		moose.connect( outputC, 'requestOut', c1, 'getConc' );


def main():
    """
    This example illustrates how to define a kinetic model embedded in
    a NeuroMesh, and undergoing cross-compartment reactions. It is 
    completely self-contained and does not use any external model definition
    files.  Normally one uses standard model formats like
    SBML or kkit to concisely define kinetic and neuronal models.
    This example creates a simple reaction::
        a <==> b <==> c 
    in which 
    **a, b**, and **c** are in the dendrite, spine head, and PSD 
    respectively.
    The model is set up to run using the Ksolve for integration. Although
    a diffusion solver is set up, the diff consts here are set to zero.
    The display has two parts: 
    Above is a line plot of concentration against compartment#. 
    Below is a time-series plot that appears after # the simulation has 
    ended. The plot is for the last (rightmost) compartment.
    Concs of **a**, **b**, **c** are plotted for both graphs.
    """
    simdt = 0.01
    plotdt = 0.01

    makeModel()

    # Schedule the whole lot
    for i in range( 10, 19):
        moose.setClock( i, simdt ) # for the compute objects
    moose.reinit()
    display()
    quit()

def display():
    dt = 0.01
    runtime = 1
    a = moose.element( '/model/compt0/a' )
    b = moose.element( '/model/compt1/b' )
    c = moose.element( '/model/compt2/c' )
    plt.ion()
    fig = plt.figure( figsize=(12,10))
    timeseries = fig.add_subplot( 212 )
    spatial = fig.add_subplot( 211)
    spatial.set_ylim(0, 15)
    pos = numpy.arange( 0, a.vec.conc.size, 1 )
    line1, = spatial.plot( pos, a.vec.conc, 'b-', label='a' )
    line2, = spatial.plot( pos[1:], b.vec.conc, 'g-', label='b' )
    line3, = spatial.plot( pos[1:], c.vec.conc, 'r-', label='c' )
    timeLabel = plt.text( 3, 12, 'time = 0' )
    plt.legend()
    fig.canvas.draw()

    for t in numpy.arange( dt, runtime, dt ):
        line1.set_ydata( a.vec.conc )
        line2.set_ydata( b.vec.conc )
        line3.set_ydata( c.vec.conc )
        #print b.vec.volume
        #print a.vec.n, b.vec.n, c.vec.n
        timeLabel.set_text( "time = %f" % t )
        fig.canvas.draw()
        #raw_input()
        moose.start( dt )

    timeseries.set_ylim( 0, 20 )
    for x in moose.wildcardFind( '/model/graphs/conc#' ):
        t = numpy.arange( 0, x.vector.size *dt , dt ) # sec
        line4, = timeseries.plot( t, x.vector, label=x.name )
    plt.legend()
    fig.canvas.draw()

    print( "Hit 'enter' to exit" )
    raw_input()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
