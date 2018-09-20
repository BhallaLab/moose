########################################################################
# This program is copyright (c) Upinder S. Bhalla, NCBS, 2015.
# It is licenced under the GPL 2.1 or higher.
# There is no warranty of any kind. You are welcome to make copies under 
# the provisions of the GPL.
# This program builds a multiscale model with a few spines inserted into
# a simplified cellular morphology. Each spine has a signaling model in it
# too. The program doesn't run the model, it just displays it in 3D.
########################################################################
try:
    import moogli
except Exception as e:
    print( "[INFO ] Could not import moogli. Quitting ..." )
    quit()

from PyQt4 import Qt, QtCore, QtGui
import numpy
import time
import pylab
import moose
from moose import neuroml
import matplotlib.pyplot as plt
import sys
import os
from moose.neuroml.ChannelML import ChannelML
sys.path.append('/home/bhalla/moose/trunk/Demos/util')
import rdesigneur as rd

PI = 3.14159265359
useGssa = True
combineSegments = False
baselineTime = 1
tetTime = 1
interTetTime = 0.1
postTetTime = 0.1
ltdTime = 0.1
postLtdTime = 0.1
do3D = True
dt = 0.01
plotdt = 0.1
psdTetCa = 8e-3
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
                "angleDistrib", "6.28", \
                "size", "6", \
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
        [ 'Ca_conc', 'Ca', 'dend/DEND/Ca_input', 'concInit', 8e-5, 1 ],
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

def createVmViewer(rdes):
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

    def prelude(view):
        vms = [moose.element(x).Vm for x in list(network.shapes.keys())]
        network.set("color", vms, mapper)
        view.pitch(PI/2.0)
        view.down(450)
        view.left(100)
        view.h = 2.0
        view.zoom(5.0)

    def interlude(view):
        if view.h > 0.10:
            view.h /= 1.005
            view.zoom(0.005)
        view.yaw(0.01)

    viewer = moogli.Viewer("vm-viewer")
    viewer.attach_shapes(list(network.shapes.values()))
    view = moogli.View("vm-view",
                       prelude=prelude,
                       interlude=interlude)
    viewer.attach_view(view)
    return viewer


def main():
    numpy.random.seed( 1234 )
    rdes = buildRdesigneur()
    rdes.buildModel( '/model' )
    assert( moose.exists( '/model' ) )
    moose.element( '/model/elec/hsolve' ).tick = -1
    for i in range( 10, 18 ):
        moose.setClock( i, dt )
    moose.setClock( 18, plotdt )
    moose.reinit()

    if do3D:
        app = QtGui.QApplication(sys.argv)
        compts = moose.wildcardFind( "/model/elec/#[ISA=CompartmentBase]" )
        print(("LEN = ", len( compts )))
        for i in compts:
            n = i.name[:4]
            if ( n == 'head' or n == 'shaf' ):
                i.diameter *= 1.0
                i.Vm = 0.02
            else:
                i.diameter *= 4.0
                i.Vm = -0.05
        vm_viewer = createVmViewer(rdes)
        vm_viewer.showMaximized()
        vm_viewer.start()
        app.exec_()

if __name__ == '__main__':
    main()
