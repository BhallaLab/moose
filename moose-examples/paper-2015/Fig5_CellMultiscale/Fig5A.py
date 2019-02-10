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
    print( "[INFO ] Could not import moogli. Quitting..." )
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
useGssa = True
combineSegments = False
#### Choose your favourite models here. #################
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
    spineProto = [ \
        ['makeSpineProto()', 'spine' ]
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
    spineDistrib = [
            ["spine", '#apical#', "20e-6", "2e-6",
                "1", "0.5",
                "0", str( 2*PI ) ]
        ]

    ######################################################################
    # Having defined everything, now to create the rdesigneur and proceed
    # with creating the model.
    ######################################################################
    

    rdes = rd.rdesigneur(
        combineSegments = combineSegments, \
        stealCellFromLibrary = True, \
        passiveDistrib = passiveDistrib, \
        spineDistrib = spineDistrib, \
        spineProto = spineProto \
    )

    return rdes

def interlude( view ):
	view.yaw( 0.01 )

def create_viewer(rdes):
    print((' doing viewer for ', rdes.soma.path))
    network = moogli.extensions.moose.read(rdes.elecid.path)
    normalizer = moogli.utilities.normalizer(-0.08,
                                             0.02,
                                             clipleft=True,
                                             clipright=True)
    colormap = moogli.colors.UniformColorMap([moogli.colors.Color(0.0,
                                                                  0.5,
                                                                  1.0,
                                                                  1.0),
                                              moogli.colors.Color(1.0,
                                                                  0.0,
                                                                  0.0,
                                                                  0.9)])
    mapper = moogli.utilities.mapper(colormap, normalizer)

    vms = [moose.element(x).Vm for x in list(network.shapes.keys())]
    network.set("color", vms, mapper)
    viewer = moogli.Viewer("vm-viewer")
    viewer.attach_shapes(list(network.shapes.values()))
    view = moogli.View("vm-view", interlude=interlude )
    viewer.attach_view(view)
    viewer.show()
    viewer.start()
    view.pitch( PI/2.0 )
    return viewer

def main():
    app = QtGui.QApplication(sys.argv)
    numpy.random.seed(1234)
    rdes = buildRdesigneur()
    viewers = []
    j = 0
    for i in elecFileNames:
        print(i)
        ename = '/model' + str(j)
        rdes.cellProtoList = [ ['./cells/' + i, 'elec' ] ]
        rdes.buildModel( ename )
        assert( moose.exists( ename ) )
        moose.reinit()
        viewers.append(create_viewer(rdes))
        j = j + 1
    app.exec_()

if __name__ == '__main__':
    main()
