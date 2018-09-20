########################################################################
# This program is copyright (c) Upinder S. Bhalla, NCBS, 2015.
# It is licenced under the GPL 2.1 or higher.
# There is no warranty of any kind. You are welcome to make copies under 
# the provisions of the GPL.
# This programme illustrates building a panel of multiscale models to
# test neuronal plasticity in different contexts. The simulation is set
# to settle for 5 seconds, then a 2 second tetanus is delivered, then
# the simulation continues for another 50 seconds.
# By default we set it to run the smallest model, that takes about 4 minutes
# to run 57 seconds of simulation time, on an Intel core I7 at 
# 2.2 GHz. The big model, VHC-neuron, takes almost 90 minutes.
# This program dumps data to text files for further analysis.
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
import rdesigneur as rd

PI = 3.14159265359
useGssa = True
combineSegments = False
#### Choose your favourite model here. #################
#elecFileNames = ( "ca1_minimal.p", )
#elecFileNames = ( "ca1_minimal.p", "h10.CNG.swc" )
elecFileNames = ( "CA1.morph.xml", "ca1_minimal.p", "VHC-neuron.CNG.swc", "h10.CNG.swc" )
synSpineList = []
synDendList = []
probeInterval = 0.1
probeAmplitude = 1.0
tetanusFrequency = 100.0
tetanusAmplitude = 1000
tetanusAmplitudeForSpines = 1000
baselineTime = 5
tetTime = 2
postTetTime = 50

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
    chanProto = [
        ['./chans/hd.xml'], \
        ['./chans/kap.xml'], \
        ['./chans/kad.xml'], \
        ['./chans/kdr.xml'], \
        ['./chans/na3.xml'], \
        ['./chans/nax.xml'], \
        ['./chans/CaConc.xml'], \
        ['./chans/Ca.xml'], \
        ['./chans/NMDA.xml'], \
        ['./chans/Glu.xml'] \
    ]
    spineProto = [ \
        ['makeSpineProto()', 'spine' ]
    ]
    chemProto = [ \
        ['./chem/' + 'psd53.g', 'ltpModel'] \
    ]

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
    passiveDistrib = [ 
            [ ".", "#", "RM", "2.8", "CM", "0.01", "RA", "1.5",  \
                "Em", "-58e-3", "initVm", "-65e-3" ], \
            [ ".", "#axon#", "RA", "0.5" ] \
        ]
    chanDistrib = [ \
            ["hd", "#dend#,#apical#", "Gbar", "5e-2*(1+(p*3e4))" ], \
            ["kdr", "#", "Gbar", "p < 50e-6 ? 500 : 100" ], \
            ["na3", "#soma#,#dend#,#apical#", "Gbar", "250" ], \
            ["nax", "#soma#,#axon#", "Gbar", "1250" ], \
            ["kap", "#axon#,#soma#", "Gbar", "300" ], \
            ["kap", "#dend#,#apical#", "Gbar", \
                "300*(H(100-p*1e6)) * (1+(p*1e4))" ], \
            ["Ca_conc", "#dend#,#apical#", "tau", "0.0133" ], \
            ["kad", "#soma#,#dend#,#apical#", "Gbar", \
                "300*H(p - 100e-6)*(1+p*1e4)" ], \
            ["Ca", "#dend#,#apical#", "Gbar", "50" ], \
            ["glu", "#dend#,#apical#", "Gbar", "200*H(p-200e-6)" ], \
            ["NMDA", "#dend#,#apical#", "Gbar", "2*H(p-200e-6)" ] \
        ]
    spineDistrib = [ \
            ["spine", '#apical#', "spineSpacing", "20e-6", \
                "spineSpacingDistrib", "2e-6", \
                "angle", "0", \
                "angleDistrib", str( 2*PI ), \
                "size", "1", \
                "sizeDistrib", "0.5" ] \
        ]
    chemDistrib = [ \
            [ "ltpModel", "#apical#", "install", "1"] 
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
        passiveDistrib = passiveDistrib, \
        spineDistrib = spineDistrib, \
        chanDistrib = chanDistrib, \
        chemDistrib = chemDistrib, \
        spineProto = spineProto, \
        chanProto = chanProto, \
        chemProto = chemProto, \
        adaptorList = adaptorList
    )

    return rdes

def buildPlots( rdes ):
    numPlots = 10
    caPsd = moose.vec( '/model/chem/psd/Ca' )
    caHead = moose.vec( '/model/chem/spine/Ca' )
    psdR = moose.vec( '/model/chem/psd/tot_PSD_R' )
    numSpines = rdes.spineCompt.mesh.num
    assert( 2 * numSpines == len( rdes.spineComptElist ) )
    if not moose.exists( '/graphs' ):
        moose.Neutral( '/graphs' )
    assert( len( caPsd ) == numSpines )
    assert( len( caHead ) == numSpines )
    if numSpines < numPlots:
        caPsdTab = moose.Table2( '/graphs/caPsdTab', numSpines ).vec
        caHeadTab = moose.Table2( '/graphs/caHeadTab', numSpines ).vec
        psdRtab = moose.Table2( '/graphs/psdRtab', numSpines ).vec
        for i in range( numSpines ):
            moose.connect( caPsdTab[i], 'requestOut', caPsd[i], 'getConc' )
            moose.connect( caHeadTab[i], 'requestOut', caHead[i], 'getConc')
            moose.connect( psdRtab[i], 'requestOut', psdR[i], 'getN' )
    else:
        caPsdTab = moose.Table2( '/graphs/caPsdTab', numPlots ).vec
        caHeadTab = moose.Table2( '/graphs/caHeadTab', numPlots ).vec
        psdRtab = moose.Table2( '/graphs/psdRtab', numPlots ).vec
        dx = numSpines / numPlots
        for i in range( numPlots ):
            moose.connect( caPsdTab[i], 'requestOut', caPsd[i*dx], 'getConc' )
            moose.connect( caHeadTab[i], 'requestOut', caHead[i*dx], 'getConc' )
            moose.connect( psdRtab[i], 'requestOut', psdR[i*dx], 'getN' )
    vtab = moose.Table( '/graphs/vtab' )
    moose.connect( vtab, 'requestOut', rdes.soma, 'getVm' )
    eSpineCaTab = moose.Table( '/graphs/eSpineCaTab' )
    path = rdes.spineComptElist[1].path + "/Ca_conc"
    moose.connect( eSpineCaTab, 'requestOut', path, 'getCa' )
    eSpineVmTab = moose.Table( '/graphs/eSpineVmTab' )
    moose.connect( eSpineVmTab, 'requestOut', rdes.spineComptElist[1], 'getVm' )
    eSpineGkTab = moose.Table( '/graphs/eSpineGkTab' )
    path = rdes.spineComptElist[1].path + "/NMDA"
    moose.connect( eSpineGkTab, 'requestOut', path, 'getGk' )

def saveAndClearPlots( name ):
    print(('saveAndClearPlots( ', name, ' )'))
    for i in moose.wildcardFind( "/graphs/#" ):
        #plot stuff
        i.xplot( name + '.xplot', i.name )
    moose.delete( "/graphs" )

def printPsd( name ):
    # Print the vol, the path dist from soma, the electrotonic dist, and N
    psdR = moose.vec( '/model/chem/psd/tot_PSD_R' )
    neuronVoxel = moose.element( '/model/chem/spine' ).neuronVoxel
    elecComptMap = moose.element( '/model/chem/dend' ).elecComptMap
    print(("len( neuronVoxel = ", len( neuronVoxel), min( neuronVoxel), max( neuronVoxel)))
    print((len( elecComptMap), elecComptMap[0], elecComptMap[12]))
    neuron = moose.element( '/model/elec' )
    ncompts = neuron.compartments
    d = {}
    j = 0
    for i in ncompts:
        #print i
        d[i] = j
        j += 1

    f = open( name + ".txt", 'w' )
    for i in range( len( psdR ) ):
        n = psdR[i].n
        conc = psdR[i].conc
        vol = psdR[i].volume
        compt = elecComptMap[ neuronVoxel[i] ]
        #print compt
        segIndex = d[compt[0]]
        p = neuron.geometricalDistanceFromSoma[ segIndex ]
        L = neuron.electrotonicDistanceFromSoma[ segIndex ]
        s = str( i ) + "    " + str(n) + "  " + str( conc ) + "  " + str(p) + "  " + str(L) + "\n"
        f.write( s )
    f.close()


def probeStimulus( time ):
    for t in numpy.arange( 0, time, probeInterval ):
        moose.start( probeInterval )
        for i in synSpineList:
            i.activation( probeAmplitude )

def tetanicStimulus( time ):
    tetInterval = 1.0/tetanusFrequency
    for t in numpy.arange( 0, time, tetInterval ):
        moose.start( tetInterval )
        for i in synDendList:
            i.activation( tetanusAmplitude )
        for i in synSpineList:
            i.activation( tetanusAmplitudeForSpines )

def main():
    global synSpineList 
    global synDendList 
    numpy.random.seed( 1234 )
    rdes = buildRdesigneur( )
    for i in elecFileNames:
        print(i)
        rdes.cellProtoList = [ ['./cells/' + i, 'elec'] ]
        rdes.buildModel( )
        assert( moose.exists( '/model' ) )
        synSpineList = moose.wildcardFind( "/model/elec/#head#/glu,/model/elec/#head#/NMDA" )
        temp = set( moose.wildcardFind( "/model/elec/#/glu,/model/elec/#/NMDA" ) )

        synDendList = list( temp - set( synSpineList ) )
        moose.reinit()
        buildPlots( rdes )
        # Run for baseline, tetanus, and post-tetanic settling time 
        t1 = time.time()
        probeStimulus( baselineTime )
        tetanicStimulus( tetTime )
        probeStimulus( postTetTime )
        print(('real time = ', time.time() - t1))

        printPsd( i + ".fig5" )
        saveAndClearPlots( i + ".fig5" )
        moose.delete( '/model' )
        rdes.elecid = moose.element( '/' )

if __name__ == '__main__':
    main()
