########################################################################
# This program is copyright (c) Upinder S. Bhalla, NCBS, 2015.
# It is licenced under the GPL 2.1 or higher.
# There is no warranty of any kind. You are welcome to make copies under 
# the provisions of the GPL.
# This programme illustrates building a panel of multiscale models to
# test neuronal plasticity in different contexts.
########################################################################
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
sys.path.append('../../../Demos/util')
import rdesigneur as rd
import moogli

PI = 3.14159265359
useGssa = True
combineSegments = True
# Pick your favourite cell here.
#elecFileName = "ca1_minimal.p"
## Cell morphology from Bannister and Larkman J Neurophys 2015/NeuroMorpho
elecFileName = "h10.CNG.swc"
#elecFileName = "CA1.morph.xml"
#elecFileName = "VHC-neuron.CNG.swc"
synSpineList = []
synDendList = []
probeInterval = 0.1
probeAmplitude = 1.0
tetanusFrequency = 100.0
tetanusAmplitude = 1000
tetanusAmplitudeForSpines = 1000
frameRunTime = 1e-3 # 1 ms
baselineTime = 0.05
tetTime = 0.01
postTetTime = 0.01
runtime = baselineTime + tetTime + postTetTime

def buildRdesigneur():
    '''
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
    '''
    cellProto = [ [ "./cells/" + elecFileName, "elec" ] ]
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
    chemProto = []

    ##################################################################
    # Here we define what goes where, and any parameters. Each distribution
    # has the format
    # protoName, path, field, expr, [field, expr]...
    # where 
    #   protoName identifies the prototype to be placed on the cell
    #   path is a MOOSE wildcard path specifying where to put things
    #   field is the field to assign.
    #   expr is a math expression to define field value. This uses the
    #     muParser. Built-in variables are:
    #       p, g, L, len, dia, maxP, maxG, maxL.
    #     where
    #       p = path distance from soma, threaded along dendrite
    #       g = geometrical distance from soma (shortest distance)
    #       L = electrotonic distance from soma: number of length constants
    #       len = length of dendritic compartment
    #       dia = diameter of dendritic compartment
    #       maxP = maximal value of 'p' for the cell
    #       maxG = maximal value of 'g' for the cell
    #       maxL = maximal value of 'L' for the cell
    #
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
            ["Ca_conc", "#soma#,#dend#,#apical#", "tau", "0.0133" ], \
            ["kad", "#soma#,#dend#,#apical#", "Gbar", \
                "300*H(p - 100e-6)*(1+p*1e4)" ], \
                ["Ca", "#dend#,#apical#", "Gbar", "p<160e-6? 10+ p*0.25e-6 : 50" ], \
            ["Ca", "#soma#", "Gbar", "10" ], \
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
    chemDistrib = []

    ######################################################################
    # Here we define the mappings across scales. Format:
    # sourceObj sourceField destObj destField offset scale
    # where the coupling expression is anything a muParser can evaluate,
    # using the input variable x. For example: 8e-5 + 300*x
    # For now, let's use existing adaptors which take an offset and scale.
    ######################################################################
    adaptorList = []

    ######################################################################
    # Having defined everything, now to create the rdesigneur and proceed
    # with creating the model.
    ######################################################################
    
    rd.addSpineProto() # This adds a version with an LCa channel by default.

    rdes = rd.rdesigneur(
        useGssa = useGssa, \
        combineSegments = combineSegments, \
        stealCellFromLibrary = True, \
        passiveDistrib = passiveDistrib, \
        spineDistrib = spineDistrib, \
        chanDistrib = chanDistrib, \
        chemDistrib = chemDistrib, \
        cellProto = cellProto, \
        chanProto = chanProto, \
        chemProto = chemProto, \
        adaptorList = adaptorList
    )
    #spineProto = spineProto, \

    return rdes

def buildPlots( rdes ):
    graphs = moose.Neutral( '/graphs' )
    vtab = moose.Table( '/graphs/VmTab' )
    moose.connect( vtab, 'requestOut', rdes.soma, 'getVm' )

def displayPlots():
    pylab.figure(1, figsize = (8,10 ) )
    pylab.subplot( 1,1,1)
    for i in moose.wildcardFind( "/graphs/#VmTab" ):
        t = numpy.arange( 0, i.vector.size, 1 ) * i.dt
        pylab.plot( t, i.vector, label = i.name )
    pylab.xlabel( "Time (s)" )
    pylab.legend()
    pylab.title( 'Vm' )

    pylab.figure(2, figsize= (8,10))
    ax = pylab.subplot( 1,1,1 )
    neuron = moose.element( '/model/elec' )
    comptDistance = dict( zip( neuron.compartments, neuron.pathDistanceFromSoma ) )
    for i in moose.wildcardFind( '/library/#[ISA=ChanBase]' ):
        chans = moose.wildcardFind( '/model/elec/#/' + i.name )
        print i.name, len( chans )
        p = [ 1e6*comptDistance.get( j.parent, 0) for j in chans ]
        Gbar = [ j.Gbar/(j.parent.length * j.parent.diameter * PI) for j in chans ]
        if len( p ) > 2:
            pylab.plot( p, Gbar, linestyle = 'None', marker = ".", label = i.name )
            sortedGbar = sorted(zip(p, Gbar), key=lambda x: x[0])
    ax.set_yscale( 'log' )
    pylab.xlabel( "Distance from soma (microns)" )
    pylab.ylabel( "Channel density (Seimens/sq mtr)" )
    pylab.legend()
    pylab.title( 'Channel distribution' )
    pylab.show()

def create_vm_viewer(rdes):
    network = moogli.extensions.moose.read(rdes.elecid.path)
    normalizer = moogli.utilities.normalizer(-0.08,
                                             0.02,
                                             clipleft=True,
                                             clipright=True)
    colormap = moogli.colors.UniformColorMap([moogli.colors.Color(0.0,
                                                                  0.0,
                                                                  1.0,
                                                                  1.0),
                                              moogli.colors.Color(1.0,
                                                                  1.0,
                                                                  0.0,
                                                                  0.1)])
    mapper = moogli.utilities.mapper(colormap, normalizer)
    vms = [moose.element(x).Vm for x in network.shapes.keys()]
    network.set("color", vms, mapper)

    def prelude(view):
        view.pitch(PI/2)
        view.zoom(0.4)

    def interlude(view):
        moose.start(frameRunTime)
        vms = [moose.element(x).Vm for x in network.shapes.keys()]
        network.set("color", vms, mapper)
        view.yaw(0.01)
        currTime = moose.element('/clock').currentTime
        if currTime < runtime:
            deliverStim(currTime)
        else:
            view.stop()

    def postlude(view):
        displayPlots()

    viewer = moogli.Viewer("vm-viewer")
    viewer.attach_shapes(network.shapes.values())
    view = moogli.View("vm-view",
                       prelude=prelude,
                       interlude=interlude,
                       postlude=postlude)
    viewer.attach_view(view)
    return viewer


def create_ca_viewer(rdes):
    network = moogli.extensions.moose.read(rdes.elecid.path)
    ca_elements = []
    for compartment_path in network.shapes.keys():
        if moose.exists(compartment_path + '/Ca_conc'):
            ca_elements.append(moose.element(compartment_path + '/Ca_conc'))
        else:
            ca_elements.append(moose.element('/library/Ca_conc'))

    normalizer = moogli.utilities.normalizer(0.0,
                                             0.002,
                                             clipleft=True,
                                             clipright=True)
    colormap = moogli.colors.UniformColorMap([moogli.colors.Color(1.0,
                                                                  0.0,
                                                                  0.0,
                                                                  1.0),
                                              moogli.colors.Color(0.0,
                                                                  1.0,
                                                                  1.0,
                                                                  0.1)])
    mapper = moogli.utilities.mapper(colormap, normalizer)

    cas = [element.Ca for element in ca_elements]
    network.set("color", cas, mapper)

    def prelude(view):
        view.pitch(PI/2)
        view.zoom(0.4)

    def interlude(view):
        moose.start(frameRunTime)
        cas = [element.Ca for element in ca_elements]
        network.set("color", cas, mapper)
        view.yaw(0.01)
        currTime = moose.element('/clock').currentTime
        if currTime < runtime:
            deliverStim(currTime)
        else:
            view.stop()

    viewer = moogli.Viewer("ca-viewer")
    viewer.attach_shapes(network.shapes.values())
    view = moogli.View("ca-view",
                       prelude=prelude,
                       interlude=interlude)
    viewer.attach_view(view)
    return viewer

def build3dDisplay(rdes):
    print "building 3d Display"
    app = QtGui.QApplication(sys.argv)

    vm_viewer = create_vm_viewer(rdes)
    vm_viewer.resize(700, 900)
    vm_viewer.show()
    vm_viewer.start()

    ca_viewer = create_ca_viewer(rdes)
    ca_viewer.resize(700, 900)
    ca_viewer.show()
    ca_viewer.start()

    return app.exec_()


def deliverStim( currTime ):
    if currTime > baselineTime and currTime < baselineTime + tetTime:
        # deliver tet stim
        step = int ( (currTime - baselineTime) / frameRunTime )
        tetStep = int( 1.0 / (tetanusFrequency * frameRunTime ) )
        if step % tetStep == 0:
            for i in synDendList:
                i.activation( tetanusAmplitude )
            for i in synSpineList:
                i.activation( tetanusAmplitudeForSpines )
    else:
        # deliver probe stim
        step = int (currTime / frameRunTime )
        probeStep = int( probeInterval / frameRunTime )
        if step % probeStep == 0:
            print "Doing probe Stim at ", currTime
            for i in synSpineList:
                i.activation( probeAmplitude )


def main():
    global synSpineList 
    global synDendList
    numpy.random.seed( 1234 )
    rdes = buildRdesigneur()
    rdes.buildModel( '/model' )
    assert( moose.exists( '/model' ) )
    synSpineList = moose.wildcardFind( "/model/elec/#head#/glu,/model/elec/#head#/NMDA" )
    temp = set( moose.wildcardFind( "/model/elec/#/glu,/model/elec/#/NMDA" ) )

    synDendList = list( temp - set( synSpineList ) )
    print "num spine, dend syns = ", len( synSpineList ), len( synDendList )
    moose.reinit()
    #for i in moose.wildcardFind( '/model/elec/#apical#/#[ISA=CaConcBase]' ):
        #print i.path, i.length, i.diameter, i.parent.length, i.parent.diameter

    buildPlots(rdes)
    # Run for baseline, tetanus, and post-tetanic settling time 
    t1 = time.time()
    build3dDisplay(rdes)
    print 'real time = ', time.time() - t1

if __name__ == '__main__':
    main()
