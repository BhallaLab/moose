#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2015 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################
# This example illustrates loading a model from an SWC file, inserting
# channels, and running it.

try:
    import moogli
except Exception as e:
    print( "[INFO ] Could not import moogli. Quitting..." )
    quit()
    
import moose
from PyQt4 import Qt, QtCore, QtGui
import numpy
import pylab
import sys
import os
import rdesigneur as rd

PI = 3.14159265358979
frameRunTime = 0.0005
runtime = 0.1
inject = 1e-9
chanProto_ = [
        ['./chans/KChannel_HH.xml'], 
        ['./chans/NaChannel_HH.xml'],
        ['./chans/LeakConductance.xml'],
        [ './chans/kdr.xml' ]
        ]

passiveDistrib_ = [ 
        [ ".", "#", "RM", "2.8", "CM", "0.01", "RA", "1.5",  
            "Em", "-58e-3", "initVm", "-65e-3" ]
        , [ ".", "#axon#", "RA", "0.5" ] 
        ]

chanDistrib_ = [
        [  "NaConductance", "#", "Gbar", "1200" ]
        , [ "KConductance", "#", "Gbar", "360" ]
        , [ "LeakConductance", "#", "Gbar", "3" ]
        , [ "kdr", "#", "Gbar", "10" ]
        ]


def create_vm_viewer(rdes, somaVm):
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

    def prelude(view):
        view.pitch(PI/2.0)

    def interlude(view):
        moose.start(frameRunTime)
        vms = [moose.element(x).Vm for x in list(network.shapes.keys())]
        network.set("color", vms, mapper)
        view.yaw(0.01)
        currTime = moose.element('/clock').currentTime
        if currTime >= runtime:
            view.stop()

    def postlude(view):
        pylab.plot(numpy.linspace(0,
                                  runtime,
                                  len(somaVm.vector)),
                   somaVm.vector * 1000)
        pylab.xlabel("Time (s)")
        pylab.ylabel("Vm (mV)")
        pylab.show()

    viewer = moogli.Viewer("vm-viewer")
    viewer.attach_shapes(list(network.shapes.values()))
    view = moogli.View("vm-view",
                       prelude=prelude,
                       interlude=interlude,
                       postlude=postlude)
    viewer.attach_view(view)
    return viewer


def main():
    ######## Put your favourite cell model here ######
    ##This one is from PMID 22730554: Suo et al J. Mol Cell Biol 2012
    filename = 'cells/ko20x-07.CNG.swc'
    moose.Neutral( '/library' )
    rdes = rd.rdesigneur( \
            cellProto = [[ filename, 'elec' ] ],\
            passiveDistrib = passiveDistrib_,
            chanProto = chanProto_,
            chanDistrib = chanDistrib_
        )
    rdes.buildModel( '/model' )
    moose.reinit()

    ################## Now we store plots ########################
    somaVm = moose.Table( '/somaVm' )
    moose.connect( somaVm, 'requestOut', rdes.soma, 'getVm' )
    ################## Now we set up the display ########################
    compts = moose.wildcardFind( "/model/elec/#[ISA=CompartmentBase]" )
    compts[0].inject = inject

    print("Setting Up 3D Display")
    app = QtGui.QApplication(sys.argv)
    vm_viewer = create_vm_viewer(rdes, somaVm)
    vm_viewer.show()
    vm_viewer.start()
    return app.exec_()

if __name__ == '__main__':
    main()
