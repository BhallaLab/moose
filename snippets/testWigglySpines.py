# -*- coding: utf-8 -*-
from __future__ import print_function

import math
import pylab
import numpy
import matplotlib.pyplot as plt
import moose
import sys
import rdesigneur as rd

try:
    from PyQt4 import QtGui
    import moogli
except ImportError as e:
    print( "[INFO ] Could not find moogli. Quitting..." )
    quit()
    

import os
scriptDir = os.path.dirname( os.path.realpath( __file__ ) )
sys.path.append( os.path.join( scriptDir, '../util' ) )

PI = 3.141592653
ScalingForTesting = 10
RM = 1.0 / ScalingForTesting
RA = 1.0 * ScalingForTesting
CM = 0.01 * ScalingForTesting
runtime = 1.0
diffConst = 1e-12
dendLen = 10e-6
spineSpacing = 1.5e-6
spineSpacingDistrib = 0
spineSize = 1.0
spineSizeDistrib = 0

def makeCellProto( name ):
    elec = moose.Neuron( '/library/' + name )
    ecompt = []
    ec = rd.buildCompt( elec, 'dend', dx = dendLen, dia = 2.0e-6, x = 0, RM = RM, RA = RA, CM = CM )
    elec.buildSegmentTree()

def makeChemProto( name ):
    chem = moose.Neutral( '/library/' + name )
    for i in ( ['dend', 1e-18], ['spine', 1e-19], ['psd', 1e-20] ):
        print('making ', i)
        compt = moose.CubeMesh( chem.path + '/' + i[0] )
        compt.volume = i[1]
        Ca = moose.Pool( compt.path + '/Ca' )
        Ca.concInit = 0.08
        Ca.diffConst = 1e-11

def makeSpineProto2( name ):
    spine = moose.Neutral( '/library/' + name )
    shaft = rd.buildCompt( spine, 'shaft', dz=1e-6, dx = 0, dia=0.2e-6, z=0, RM=RM, RA=RA, CM=CM )
    head = rd.buildCompt( spine, 'head', dz=0.5e-6, dx = 0, dia=0.5e-6, z=1e-6, RM=RM, RA=RA, CM=CM )
    moose.connect( shaft, 'axial', head, 'raxial' )

def makeModel():
    spineAngle= numpy.pi / 2.0
    spineAngleDistrib = 0.0
    
    moose.Neutral( '/library' )
    makeCellProto( 'cellProto' )
    makeChemProto( 'cProto' )
    makeSpineProto2( 'spine' )
    rdes = rd.rdesigneur( 
            elecDt = 50e-6,
            chemDt = 5e-3,
            useGssa = False, \
            combineSegments = False, \
            stealCellFromLibrary = True, \
            diffusionLength = 1e-6, \
            cellProto = [['cellProto', 'elec' ]] ,\
            spineProto = [['spineProto', 'spine' ]] ,\
            chemProto = [['cProto', 'chem' ]] ,\
            spineDistrib = [ \
                ['spine', '#', 
                    str( spineSpacing ), str( spineSpacingDistrib ),
                    str( spineSize ), str( spineSizeDistrib ),
                    str( spineAngle ), str( spineAngleDistrib ) ]
            ], \
            chemDistrib = [ \
                [ "chem", "#", "install", "1" ] \
            ],
            adaptorList = [ \
                [ 'psd/Ca', 'conc', '.', 'inject', False, 0, 2e-9 ], \
                ] \
        )
    rdes.buildModel( '/model' )

def p2(x):
    '''
    Print to 2 sig fig
    '''
    print(round(x, 1-int(numpy.floor(numpy.log10(x)))), end=' ')

def assertEq( a, b ):
    print('.', end=' ')
    if not( numpy.fabs( a-b) / (a+b) < 1e-10 ):
        print('a!=b:', a, b)
        assert(False)

def main():
    """
    This illustrates the use of rdesigneur to build a simple dendrite with
    spines, and then to resize them using spine fields. These are the
    fields that would be changed dynamically in a simulation with reactions
    that affect spine geometry.
    """
    makeModel()
    elec = moose.element( '/model/elec' )
    elec.setSpineAndPsdMesh( moose.element('/model/chem/spine'), moose.element('/model/chem/psd') )
    caDend = moose.vec( '/model/chem/dend/Ca' )
    caHead = moose.vec( '/model/chem/spine/Ca' )
    caPsd = moose.vec( '/model/chem/psd/Ca' )
    eHead = moose.wildcardFind( '/model/elec/#head#' )
    graphs = moose.Neutral( '/graphs' )
    psdTab = moose.Table2( '/graphs/psdTab', len( caPsd ) ).vec
    headTab = moose.Table2( '/graphs/headTab', len( caHead ) ).vec
    dendTab = moose.Table2( '/graphs/dendTab', len( caDend ) ).vec
    eTab = moose.Table( '/graphs/eTab', len( eHead ) ).vec
    stimtab = moose.StimulusTable( '/stim' )
    stimtab.stopTime = 0.3
    stimtab.loopTime = 0.3
    stimtab.doLoop = True
    stimtab.vector = [ 1.0 + numpy.sin( x ) for x in numpy.arange( 0, 2*PI, PI/1000 ) ]
    estimtab = moose.StimulusTable( '/estim' )
    estimtab.stopTime = 0.001
    estimtab.loopTime = 0.001
    estimtab.doLoop = True
    estimtab.vector = [ 1e-9*numpy.sin( x ) for x in numpy.arange( 0, 2*PI, PI/1000 ) ]

    for i in range( len( caPsd ) ):
        moose.connect( psdTab[ i ], 'requestOut', caPsd[i], 'getConc' )
    for i in range( len( caHead ) ):
        moose.connect( headTab[ i ], 'requestOut', caHead[i], 'getConc' )
    for i in range( len( caDend ) ):
        moose.connect( dendTab[ i ], 'requestOut', caDend[i], 'getConc' )
    for i in range( len( eHead ) ):
        moose.connect( eTab[ i ], 'requestOut', eHead[i], 'getVm' )
    moose.connect( stimtab, 'output', caDend, 'setConc', 'OneToAll' )
    dend = moose.element( '/model/elec/dend' )
    moose.connect( estimtab, 'output', dend, 'setInject' )

    moose.reinit()
    moose.start( 1 )

    head0 = moose.element( '/model/elec/head0' )
    shaft1 = moose.element( '/model/elec/shaft1' )
    head2 = moose.element( '/model/elec/head2' )

    # Here we scale the spine head length while keeping all vols constt.
    print("Spine 0: longer head, same vol\nSpine 1: longer shaft")
    print("Spine 2: Bigger head, same diffScale\n")
    elecParms = [ (i.Rm, i.Cm, i.Ra) for i in ( head0, shaft1, head2) ]
    chemParms = [ i.volume for i in ( caHead[0], caPsd[0], caHead[1], caPsd[1], caHead[2], caPsd[2] ) ]

    elec.spine[0].headLength *= 4 # 4x length
    print( "############## {0}   {1}    {2}".format( elecParms[0][0], head0.Rm, elec.spine[0].headLength ) )
    elec.spine[0].headDiameter *= 0.5 #  1/2 x dia

    # Here we scale the shaft length. Vols are not touched.
    elec.spine[1].shaftLength *= 2 # 2 x length

    #Here we scale the spine head vol while retaining diffScale = xArea/len
    # This gives 4x vol.
    hdia = elec.spine[2].headDiameter
    sdsolve = moose.element( '/model/chem/spine/dsolve' )
    elec.spine[2].headLength *= 2 # 2x length
    elec.spine[2].headDiameter *= numpy.sqrt(2) # sqrt(2) x dia
    hdia = elec.spine[2].headDiameter

    print("Checking scaling assertions: ")
    assertEq( elecParms[0][0] * 0.5 , head0.Rm )
    assertEq( elecParms[0][1] * 2 , head0.Cm )
    assertEq( elecParms[0][2] * 16 , head0.Ra )
    assertEq( chemParms[0] , caHead[0].volume )
    assertEq( chemParms[1] * 0.25 , caPsd[0].volume )

    assertEq( elecParms[1][0] * 0.5 , shaft1.Rm )
    assertEq( elecParms[1][1] * 2 , shaft1.Cm )
    assertEq( elecParms[1][2] * 2 , shaft1.Ra )
    assertEq( chemParms[2] , caHead[1].volume )
    assertEq( chemParms[3] , caPsd[1].volume )

    ratio = 2 * numpy.sqrt( 2 )
    assertEq( elecParms[2][0] / ratio , head2.Rm )
    assertEq( elecParms[2][1] * ratio , head2.Cm )
    assertEq( elecParms[2][2] , head2.Ra )
    assertEq( chemParms[4] * 4 , caHead[2].volume )
    assertEq( chemParms[5] * 2 , caPsd[2].volume )
    print("\nAll assertions cleared")

    moose.start( 2 )
    for i in range( len( psdTab ) ):
        pylab.plot( psdTab[i].vector, label= 'PSD' + str(i) )
    pylab.legend()
    pylab.figure()
    for i in range( len( headTab ) ):
        pylab.plot( headTab[i].vector, label= 'head' + str(i) )
    pylab.legend()
    pylab.figure()
    for i in range( len( dendTab ) ):
        pylab.plot( dendTab[i].vector, label= 'dendCa' + str(i) )
    pylab.legend()
    pylab.figure()
    for i in range( len( eTab ) ):
        pylab.plot( eTab[i].vector, label= 'headVm' + str(i) )
        #print i, len( eTab[i].vector ), eTab[i].vector
    pylab.legend()
    pylab.show()

    app = QtGui.QApplication(sys.argv)
    #widget = mv.MoogliViewer( '/model' )
    morphology = moogli.read_morphology_from_moose( name="", path = '/model/elec' )
    widget = moogli.MorphologyViewerWidget( morphology )
    widget.show()
    return app.exec_()
    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()
