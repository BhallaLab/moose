##################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2015 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
##
## rxdSpineSize.py: Builds a cell with spines and a propagating reaction
## wave. Products diffuse into the spine and cause it to get bigger.
##################################################################
import math
import pylab
import numpy
import matplotlib.pyplot as plt
import moose
import sys
sys.path.append( '../util' )
import rdesigneur as rd
from PyQt4 import QtGui
import moogli
import moogli.extensions.moose
import matplotlib

PI = 3.141592653
ScalingForTesting = 10
RM = 1.0 / ScalingForTesting
RA = 1.0 * ScalingForTesting
CM = 0.01 * ScalingForTesting
runtime = 100.0
frameruntime = 1.0
diffConst = 5e-12
dendLen = 100e-6
diffLen = 1.0e-6
dendDia = 2e-6
somaDia = 5e-6
concInit = 0.001    # 1 millimolar
spineSpacing = 10e-6
spineSpacingDistrib = 1e-6
spineSize = 1.0
spineSizeDistrib = 0.5
spineAngle= numpy.pi / 2.0
spineAngleDistrib = 0.0

def makeCellProto( name ):
    elec = moose.Neuron( '/library/' + name )
    ecompt = []
    soma = rd.buildCompt( elec, 'soma', somaDia, somaDia, -somaDia, RM, RA, CM )
    dend = rd.buildCompt( elec, 'dend', dendLen, dendDia, 0, RM, RA, CM )
    moose.connect( soma, 'axial', dend, 'raxial' )
    elec.buildSegmentTree()

def makeChemProto( name ):
    chem = moose.Neutral( '/library/' + name )
    comptVol = diffLen * dendDia * dendDia * PI / 4.0
    for i in ( ['dend', comptVol], ['spine', 1e-19], ['psd', 1e-20] ):
        print 'making ', i
        compt = moose.CubeMesh( chem.path + '/' + i[0] )
        compt.volume = i[1]
        #x = moose.Pool( compt.path + '/x' )
        #y = moose.BufPool( compt.path + '/y' )
        z = moose.Pool( compt.path + '/z' )
        #x.concInit = 0.0
        #x.diffConst = diffConst
        #y.concInit = concInit
        z.concInit = 0.0
        z.diffConst = diffConst
    nInit = comptVol * 6e23 * concInit
    nstr = str( 1/nInit)

    x = moose.Pool( chem.path + '/dend/x' )
    x.diffConst = diffConst
    func = moose.Function( x.path + '/func' )
    func.expr = "-x0 * (0.3 - " + nstr + " * x0) * ( 1 - " + nstr + " * x0)"
    print func.expr
    func.x.num = 1
    moose.connect( x, 'nOut', func.x[0], 'input' )
    moose.connect( func, 'valueOut', x, 'increment' )
    z = moose.element( '/library/' + name + '/dend/z' )
    reac = moose.Reac( '/library/' + name + '/dend/reac' )
    reac.Kf = 1
    reac.Kb = 10
    moose.connect( reac, 'sub', x, 'reac' )
    moose.connect( reac, 'prd', z, 'reac' )

def makeSpineProto2( name ):
    spine = moose.Neutral( '/library/' + name )
    shaft = rd.buildCompt( spine, 'shaft', 0.5e-6, 0.4e-6, 0, RM, RA, CM )
    head = rd.buildCompt( spine, 'head', 0.5e-6, 0.5e-6, 0.5e-6, RM, RA, CM )
    moose.connect( shaft, 'axial', head, 'raxial' )

def makeModel():
    moose.Neutral( '/library' )
    makeCellProto( 'cellProto' )
    makeChemProto( 'cProto' )
    makeSpineProto2( 'spine' )
    rdes = rd.rdesigneur( useGssa = False, \
            combineSegments = False, \
            stealCellFromLibrary = True, \
            meshLambda = 1e-6, \
            cellProto = [['cellProto', 'elec' ]] ,\
            spineProto = [['spineProto', 'spine' ]] ,\
            chemProto = [['cProto', 'chem' ]] ,\
            spineDistrib = [ \
                ['spine', '#', \
                'spacing', str( spineSpacing ), \
                'spacingDistrib', str( spineSpacingDistrib ), \
                'angle', str( spineAngle ), \
                'angleDistrib', str( spineAngleDistrib ), \
                'size', str( spineSize ), \
                'sizeDistrib', str( spineSizeDistrib ) ] \
            ], \
            chemDistrib = [ \
                [ "chem", "dend", "install", "1" ] \
            ],
            adaptorList = [ \
                [ 'psd/z', 'n', 'spine', 'psdArea', 10.0e-15, 300e-15 ], \
                ] \
        )
    rdes.buildModel( '/model' )
    x = moose.vec( '/model/chem/dend/x' )
    x.concInit = 0.0
    for i in range( 0,20 ):
        x[i].concInit = concInit

def makePlot( name, srcVec, field ):
    tab = moose.Table2('/graphs/' + name + 'Tab', len( srcVec ) ).vec
    for i in zip(srcVec, tab):
        moose.connect(i[1], 'requestOut', i[0], field)
    return tab


def displayPlots():
    for x in moose.wildcardFind( '/graphs/#[0]' ):
        tab = moose.vec( x )
        for i in range( len( tab ) ):
            pylab.plot( tab[i].vector, label=x.name[:-3] + " " + str( i ) )
        pylab.legend()
        pylab.figure()

def main():
    """
    This illustrates the use of rdesigneur to build a simple dendrite with
    spines, and then to resize them using spine fields. These are the
    fields that would be changed dynamically in a simulation with reactions
    that affect spine geometry.
    In this simulation there is a propagating reaction wave using a
    highly abstracted equation, whose product diffuses into the spines and
    makes them bigger.
    """
    makeModel()
    elec = moose.element( '/model/elec' )
    elec.setSpineAndPsdMesh( moose.element('/model/chem/spine'), moose.element('/model/chem/psd') )

    eHead = moose.wildcardFind( '/model/elec/#head#' )
    oldDia = [ i.diameter for i in eHead ]
    graphs = moose.Neutral( '/graphs' )
    #makePlot( 'psd_x', moose.vec( '/model/chem/psd/x' ), 'getN' )
    #makePlot( 'head_x', moose.vec( '/model/chem/spine/x' ), 'getN' )
    makePlot( 'dend_x', moose.vec( '/model/chem/dend/x' ), 'getN' )
    makePlot( 'dend_z', moose.vec( '/model/chem/dend/z' ), 'getN' )
    makePlot( 'head_z', moose.vec( '/model/chem/spine/z' ), 'getN' )
    makePlot( 'psd_z', moose.vec( '/model/chem/psd/z' ), 'getN' )
    makePlot( 'headDia', eHead, 'getDiameter' )

    '''
    debug = moose.PyRun( '/pyrun' )
    debug.tick = 10
    debug.runString = """print "RUNNING: ", moose.element( '/model/chem/psd/z' ).n, moose.element( '/model/elec/head0' ).diameter"""
    '''
    moose.reinit()
    moose.start( runtime )

    displayPlots()
    pylab.plot( oldDia, label = 'old Diameter' )
    pylab.plot( [ i.diameter for i in eHead ], label = 'new Diameter' )
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

def showVisualization():
    makeModel()
    elec = moose.element( '/model/elec' )
    elec.setSpineAndPsdMesh( moose.element('/model/chem/spine'), moose.element('/model/chem/psd') )

    eHead = moose.wildcardFind( '/model/elec/#head#' )
    oldDia = [ i.diameter for i in eHead ]
    graphs = moose.Neutral( '/graphs' )
    #makePlot( 'psd_x', moose.vec( '/model/chem/psd/x' ), 'getN' )
    #makePlot( 'head_x', moose.vec( '/model/chem/spine/x' ), 'getN' )
    makePlot( 'dend_x', moose.vec( '/model/chem/dend/x' ), 'getN' )
    dendZ = makePlot( 'dend_z', moose.vec( '/model/chem/dend/z' ), 'getN' )
    makePlot( 'head_z', moose.vec( '/model/chem/spine/z' ), 'getN' )
    psdZ = makePlot( 'psd_z', moose.vec( '/model/chem/psd/z' ), 'getN' )
    diaTab = makePlot( 'headDia', eHead, 'getDiameter' )
    # print diaTab[0].vector[-1]
    # return
    dendrite = moose.element("/model/elec/dend")
    dendrites = [dendrite.path + "/" + str(i) for i in range(len(dendZ))]
    # print dendrites
    moose.reinit()

    spineHeads = moose.wildcardFind( '/model/elec/#head#')
    # print moose.wildcardFind( '/model/elec/##')

    # print "dendZ", readValues(dendZ)
    # print dendrite

    app = QtGui.QApplication(sys.argv)
    viewer = create_viewer("/model/elec", dendrite, dendZ, diaTab, psdZ)
    viewer.showMaximized()
    viewer.start()
    return app.exec_()


def create_viewer(path, moose_dendrite, dendZ, diaTab, psdZ):
    network = moogli.extensions.moose.read(path=path,
                                           vertices=15)

    dendrite = network.groups["dendrite"].shapes[moose_dendrite.path]
    chem_compt_group = dendrite.subdivide(len(dendZ))
    normalizer = moogli.utilities.normalizer(0.0,
                                             300.0,
                                             clipleft=True,
                                             clipright=True)
    colormap = moogli.colors.MatplotlibColorMap(matplotlib.cm.rainbow)
    mapper = moogli.utilities.mapper(colormap, normalizer)

    def readValues(tables):
        values = []
        for i in range(len(tables)):
            values.append(tables[i].vector[-1])
        return values

    def prelude(view):
        view.home()
        view.pitch(math.pi / 3.0)
        view.zoom(0.3)
        network.groups["soma"].set("color", moogli.colors.RED)
        network.groups["spine"].groups["shaft"].set("color",
                                                    moogli.colors.RED)

    def interlude(view):
        moose.start(frameruntime)
        network.groups["spine"].groups["head"].set("radius",
                                                   readValues(diaTab),
                                                   lambda x: x * 0.5e6)
        network.groups["spine"].groups["head"].set("color",
                                                   readValues(psdZ),
                                                   mapper)
        chem_compt_group.set("color",
                             readValues(dendZ),
                             mapper)
        if moose.element("/clock").currentTime >= runtime:
            view.stop()

    viewer = moogli.Viewer("Viewer")
    viewer.attach_shapes(network.shapes.values())
    viewer.detach_shape(dendrite)
    viewer.attach_shapes(chem_compt_group.shapes.values())

    view = moogli.View("main-view",
                       prelude=prelude,
                       interlude=interlude)
    cb = moogli.widgets.ColorBar(id="cb",
                                 title="Molecule #",
                                 text_color=moogli.colors.BLACK,
                                 position=moogli.geometry.Vec3f(0.975, 0.5, 0.0),
                                 size=moogli.geometry.Vec3f(0.30, 0.05, 0.0),
                                 text_font="/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf",
                                 orientation=math.pi / 2.0,
                                 text_character_size=20,
                                 label_formatting_precision=0,
                                 colormap=moogli.colors.MatplotlibColorMap(matplotlib.cm.rainbow),
                                 color_resolution=100,
                                 scalar_range=moogli.geometry.Vec2f(0.0,
                                                                    300.0))
    view.attach_color_bar(cb)
    viewer.attach_view(view)
    return viewer


if __name__ == '__main__':
    showVisualization()
