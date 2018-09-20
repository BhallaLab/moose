########################################################################
# This program is copyright (c) Upinder S. Bhalla, NCBS, 2015.
# It is licenced under the GPL 2.1 or higher.
# There is no warranty of any kind. You are welcome to make copies under 
# the provisions of the GPL.
# This program builds a multiscale model with a few spines inserted into
# a simplified cellular morphology. Each spine has a signaling model in it
# too. The program runs the model with a strong but brief calcium input
# synapses at 10 seconds, and a long but smaller Ca influx from 290 to 
# 650 seconds. This simulation runs using a deterministic method.
# The runtime on a 2.2 GHz Intel core I7 processor is about 550 s.
########################################################################

try:
    import moogli
except ImportError as e:
    print( "[INFO ] Could not import moogli. Quitting ..." )
    quit()
    
import numpy
import time
import pylab
import moose
from moose import neuroml
from PyQt4 import Qt, QtCore, QtGui
import matplotlib.pyplot as plt
import sys
import os
from moose.neuroml.ChannelML import ChannelML
sys.path.append('/home/bhalla/moose/trunk/Demos/util')
import rdesigneur as rd

PI = 3.14159265359
useGssa = False
combineSegments = False
baselineTime = 10
tetTime = 1
interTetTime = 20
postTetTime = 240
ltdTime = 360
postLtdTime = 60
do3D = False
dt = 0.001
plotdt = 0.1
psdTetCa = 8e-3
dendTetCa = 2e-3
basalCa = 0.08e-3
ltdCa = 0.25e-3

def buildRdesigneur():
    ##################################################################
    # Here we define which prototypes are to be loaded in to the system.
    # Each specification has the format
    # source [localName]
    # source can be any of
    # filename.extension,   # Identify type of file by extension, load it.
    # function(),           # func( name ) builds object of specified name
    # file.py:function() ,  # load Python file, run function(name) in it.
    # moose.Classname       # Make obj moose.Classname, assign to name.
    # path                  # Already loaded into library or on path.
    # After loading the prototypes, there should be an object called 'name'
    # in the library.
    ##################################################################
    cellProto = [ ['ca1_minimal.p', 'elec'] ]
    spineProto = [ ['makeSpineProto()', 'spine' ]]
    chemProto = [ ['CaMKII_merged77.g', 'chem'] ]

    ##################################################################
    # Here we define what goes where, and any parameters. Each distribution
    # has the format
    # protoName, path, field, expr, [field, expr]...
    # where 
    #   protoName identifies the prototype to be placed on the cell
    #   path is a MOOSE wildcard path specifying where to put things
    #   field is the field to assign.
    #   expr is a math expression to define field value. This uses the
    #     muParser. Built-in variables are p, g, L, len, dia.
    #     The muParser provides most math functions, and the Heaviside 
    #     function H(x) = 1 for x > 0 is also provided.
    ##################################################################
    chemRange = "H(1.1e-6 - dia) * H(p - 1300e-6)"
    spineDistrib = [ \
            ["spine", '#apical#', \
                "spineSpacing", chemRange + " * 5e-6", \
                "spineSpacingDistrib", "1e-6", \
                "angle", "0", \
                "angleDistrib", "0", \
                "size", "1", \
                "sizeDistrib", "0" ] \
        ]
    chemDistrib = [ \
            [ "chem", "#apical#", "install", chemRange ] 
        ]

    ######################################################################
    # Here we define the mappings across scales. Format:
    # sourceObj sourceField destObj destField couplingExpr [wildcard][spatialExpn]
    # where the coupling expression is anything a muParser can evaluate,
    # using the input variable x. For example: 8e-5 + 300*x
    # For now, let's use existing adaptors which take an offset and scale.
    ######################################################################
    adaptorList = [
        [ 'Ca_conc', 'Ca', 'psd/Ca_input', 'concInit', 8e-5, 1 ],
        [ 'Ca_conc', 'Ca', 'dend/Ca_dend_input', 'concInit', 8e-5, 1 ],
        [ 'psd/tot_PSD_R', 'n', 'glu', 'Gbar', 0, 0.01 ],
    ]

    ######################################################################
    # Having defined everything, now to create the rdesigneur and proceed
    # with creating the model.
    ######################################################################
    
    rdes = rd.rdesigneur(
        useGssa = useGssa, \
        combineSegments = combineSegments, \
        stealCellFromLibrary = True, \
        spineDistrib = spineDistrib, \
        chemDistrib = chemDistrib, \
        cellProto = cellProto, \
        spineProto = spineProto, \
        chemProto = chemProto
    )

    return rdes

def buildOnePlot( path, field = 'getConc' ):
    elist = moose.vec( '/model/chem/' + path )
    tabname = path.replace( '/', '_' )
    tab = moose.Table2( '/graphs/' + tabname, len( elist ) ).vec
    moose.connect( tab, 'requestOut', elist, field, 'OneToOne' )

def buildPlots():
    if not moose.exists( '/graphs' ):
        moose.Neutral( '/graphs' )
    buildOnePlot( 'psd/actCaMKII' ) 
    buildOnePlot( 'spine/actCaMKII' ) 
    buildOnePlot( 'dend/DEND/actCaMKII' )
    buildOnePlot( 'psd/tot_PSD_R', 'getN' )

def displayPlots():
    #plt.style.use( 'ggplot' )
    #fig = plt.figure( figsize=( 6, 10 ), frameon=False, facecolor='white' )
    fig = plt.figure( figsize=( 5, 10 ), facecolor='white' )
    fig.subplots_adjust( left = 0.18 )
    elist = moose.wildcardFind( '/graphs/#[0]' )
    n = len(elist)
    j = 1
    timePts = numpy.arange( 0, len( elist[0].vector ) ) * elist[0].dt
    labelName = [ "[CaMKII] (uM)", "[CaMKII] (uM)",
            "[CaMKII] (uM)", "# AMPAR" ]
    showTickLabels = [0,0,0,1]
    plotTitle = ['C', 'D', 'E', 'F']
    plotScale = [1000,1000,1000,1]
    for i in zip( elist, labelName, showTickLabels, plotTitle, plotScale ):
        ax = plt.subplot( 4, 1, j )
        ax.spines['top'].set_visible( False )
        ax.spines['right'].set_visible( False )

        #This function does the same as below, one axis at a time
        #ax.xaxis.set_tick_params( direction = 'out' )
        #ax.yaxis.set_tick_params( direction = 'out' )
        ax.tick_params( direction = 'out' )

        # These two are equivalent, get rid of the ticks but not the border
        #ax.xaxis.set_visible( False )
        #ax.get_xaxis().set_visible( False )
        if not i[2]:
            ax.set_xticklabels([])
        for tick in ax.xaxis.get_major_ticks():
            tick.tick2On = False
        for tick in ax.yaxis.get_major_ticks():
            tick.tick2On = False

        plt.ylabel( i[1], fontsize = 16 )
        # alternate way of doing this separately.
        #plt.yaxis.label.size_size(16)
        #plt.title( 'B' )
        ax.text( -0.18, 1.0, i[3], fontsize = 18, weight = 'bold',
                transform=ax.transAxes )
        j = j + 1
        for k in i[0].vec:
            plt.plot( timePts, k.vector * i[4] )
        #plt.title( i.name )
        print((i[0].name))

    plt.xlabel( 'Time (s)', fontsize = 16 )
    plt.show()

def main():
    numpy.random.seed( 1234 )
    rdes = buildRdesigneur()
    rdes.buildModel( '/model' )
    assert( moose.exists( '/model' ) )
    moose.element( '/model/elec/hsolve' ).tick = -1
    for i in range( 0, 10 ):
        moose.setClock( i, 100 )
    for i in range( 10, 18 ):
        moose.setClock( i, dt )
    moose.setClock( 18, plotdt )
    moose.reinit()
    buildPlots()
    # Run for baseline, tetanus, and post-tetanic settling time 
    print('starting...')
    t1 = time.time()
    moose.start( baselineTime )
    caPsd = moose.vec( '/model/chem/psd/Ca_input' )
    caDend = moose.vec( '/model/chem/dend/DEND/Ca_input' )
    castim = (numpy.random.rand( len( caPsd.concInit ) ) * 0.8 + 0.2) * psdTetCa
    caPsd.concInit = castim
    caDend.concInit = numpy.random.rand( len( caDend.concInit ) ) * dendTetCa
    moose.start( tetTime )
    caPsd.concInit = basalCa
    caDend.concInit = basalCa
    moose.start( interTetTime )
    caPsd.concInit = castim
    caDend.concInit = numpy.random.rand( len( caDend.concInit ) ) * dendTetCa
    moose.start( tetTime )
    caPsd.concInit = basalCa
    caDend.concInit = basalCa
    moose.start( postTetTime )
    caPsd.concInit = ltdCa
    caDend.concInit = ltdCa
    moose.start( ltdTime )
    caPsd.concInit = basalCa
    caDend.concInit = basalCa
    moose.start( postLtdTime )
    print(('real time = ', time.time() - t1))

    if do3D:
        app = QtGui.QApplication(sys.argv)
        compts = moose.wildcardFind( "/model/elec/#[ISA=compartmentBase]" )
        ecomptPath = [x.path for x in compts]
        morphology = moogli.read_morphology_from_moose(name = "", path = "/model/elec")
        morphology.create_group( "group_all", ecomptPath, -0.08, 0.02, \
            [0.0, 0.5, 1.0, 1.0], [1.0, 0.0, 0.0, 0.9] )
        viewer = moogli.DynamicMorphologyViewerWidget(morphology)
        def callback( morphology, viewer ):
            moose.start( 0.1 )
            return True
        viewer.set_callback( callback, idletime = 0 )
        viewer.showMaximized()
        viewer.show()
        app.exec_()

    displayPlots()

if __name__ == '__main__':
    main()
