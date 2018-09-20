#########################################################################
# insertSpines.py ---
#
# Filename:  insertSpines.py
# Author: Upinder S. Bhalla
# Maintainer:
# Created: Oct  12 16:26:05 2014 (+0530)
# Version:
# Last-Updated: May 15 2017
#           By: Upinder S. Bhalla
#     Update #:
# URL:
# Keywords:
# Compatibility:
#
#
# Commentary:
#
#
#
#
# Change log: updated with current API
#
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
    print( "[INFO ] Failed to import moogli. Quitting..." )
    quit()

import moose
from matplotlib import cm
from PyQt4 import Qt, QtCore, QtGui
import sys
import os
import rdesigneur as rd

def main():
    """
This example illustrates loading a model from an SWC file, inserting
spines, and viewing it.

    """
    PI = 3.14159265358979
    frameRunTime = 0.0002
    runtime = 1.0
    inject = 5e-10
    simdt = 5e-5
    RM = 1.0
    RA = 1.0
    CM = 0.01
    spineSpacing = 2.0e-6
    minSpacing = 0.2e-6
    spineSize = 1.0
    spineSizeDistrib = 0.5
    spineAngle = 0
    spineAngleDistrib = 2*PI

    app = QtGui.QApplication(sys.argv)
    filename = 'barrionuevo_cell1zr.CNG.swc'
    #filename = 'h10.CNG.swc'
    moose.Neutral( '/library' )
    rdes = rd.rdesigneur( \
            cellProto = [[ filename, 'elec' ] ],\
            spineProto = [['makeSpineProto()', 'spine' ]] ,\
            spineDistrib = [ \
                ['spine', '#apical#', \
                'spacing', str( spineSpacing ), \
                'spacingDistrib', str( minSpacing ), \
                'angle', str( spineAngle ), \
                'angleDistrib', str( spineAngleDistrib ), \
                'size', str( spineSize ), \
                'sizeDistrib', str( spineSizeDistrib ) ] \
            ] \
        )
    rdes.buildModel( '/model' )
    moose.reinit()

    # Now we set up the display
    compts = moose.wildcardFind( "/model/elec/#[ISA=CompartmentBase]" )
    compts[0].inject = inject
    ecomptPath = [x.path for x in compts]
    morphology = moogli.extensions.moose.read(path = "/model/elec", vertices=15)
    #morphology = moogli.read_morphology_from_moose(name = "", path = "/model/elec")
    #morphology.create_group( "group_all", ecomptPath, -0.08, 0.02, \
    #        [0.0, 0.5, 1.0, 1.0], [1.0, 0.0, 0.0, 0.9] )
    #morphology.create_group( "group_all", ecomptPath, -0.08, 0.02, gnuplot )

    #viewer = moogli.DynamicMorphologyViewerWidget(morphology)
    viewer = moogli.Viewer("Viewer")

    viewer.attach_shapes( morphology.shapes.values() )
    view = moogli.View("main-view")
    viewer.attach_view( view )
    #viewer.set_background_color( 1.0, 1.0, 1.0, 1.0 )
    def callback( morphology, viewer ):
        moose.start( frameRunTime )
        Vm = [moose.element( x ).Vm for x in compts]
        morphology.set_color( "group_all", Vm )
        currTime = moose.element( '/clock' ).currentTime
        #print currTime, compts[0].Vm
        if ( currTime < runtime ):
            return True
        return False

    #viewer.set_callback( callback, idletime = 0 )
    #viewer.showMaximized()
    viewer.show()
    app.exec_()

if __name__ == '__main__':
    main()
