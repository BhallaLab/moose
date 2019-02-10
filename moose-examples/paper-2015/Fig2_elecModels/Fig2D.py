#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2015 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################
# This example illustrates loading a model from an SWC file, inserting
# spines, and viewing it.

try:
    import moogli
except Exception as e:
    print( "[INFO ] Could not import moogli. Quitting..." )
    quit()

import moose
from PyQt4 import Qt, QtCore, QtGui
import sys
import os
import rdesigneur as rd

PI = 3.14159265358979
frameRunTime = 0.0001
runtime = 0.1
inject = 15e-10
simdt = 5e-5
RM = 1.0
RA = 1.0
CM = 0.01
# This is the expression used to set spine spacing:
spineSpacing = "dia * 2"
minSpacing = 0.1e-6
spineSize = 1.0
spineSizeDistrib = 0.5
spineAngle = 0
spineAngleDistrib = 2*PI


def create_vm_viewer(rdes):
    network = moogli.extensions.moose.read(rdes.elecid.path,
                                           vertices=10)
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

    def interlude(view):
        moose.start(frameRunTime)
        #vms = [moose.element(x).Vm for x in network.shapes.keys()]
        #network.set("color", vms, mapper)
        view.pitch(0.01)
        currTime = moose.element('/clock').currentTime
        if currTime >= runtime:
            view.stop()

    viewer = moogli.Viewer("vm-viewer")
    viewer.attach_shapes(list(network.shapes.values()))
    view = moogli.View("vm-view",
                       interlude=interlude)
    viewer.attach_view(view)
    return viewer


def main():
    ######## Put your favourite cell model here ######
    ##This one is from PMID 19146814: Peng et al Neuron 2009
    filename = 'cells/K-18.CNG.swc'
    moose.Neutral( '/library' )
    rdes = rd.rdesigneur( \
            cellProto = [[ filename, 'elec' ] ],\
            spineProto = [['makeSpineProto()', 'spine' ]] ,\
            spineDistrib = [ \
                ['spine', '#', \
                'spacing', spineSpacing, \
                'spacingDistrib', str( minSpacing ), \
                'angle', str( spineAngle ), \
                'angleDistrib', str( spineAngleDistrib ), \
                'size', str( spineSize ), \
                'sizeDistrib', str( spineSizeDistrib ) ] \
            ] \
        )
    rdes.buildModel('/model')
    moose.reinit()
    compts = moose.wildcardFind( "/model/elec/#[ISA=CompartmentBase]" )
    compts[0].inject = inject

    ################## Now we set up the display ########################
    print("Setting Up 3D Display")
    app = QtGui.QApplication(sys.argv)
    vm_viewer = create_vm_viewer(rdes)
    vm_viewer.showMaximized()
    vm_viewer.start()
    return app.exec_()

if __name__ == '__main__':
    main()
