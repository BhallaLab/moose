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

import moogli
import moose
from matplotlib.cm import gnuplot
from PyQt4 import Qt, QtCore, QtGui
import sys
import os
import rdesigneur as rd

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

def main():
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
    ecomptPath = map( lambda x : x.path, compts )
    morphology = moogli.read_morphology_from_moose(name = "", path = "/model/elec")
    #morphology.create_group( "group_all", ecomptPath, -0.08, 0.02, \
    #        [0.0, 0.5, 1.0, 1.0], [1.0, 0.0, 0.0, 0.9] ) 
    morphology.create_group( "group_all", ecomptPath, -0.08, 0.02, \
            gnuplot )

    viewer = moogli.DynamicMorphologyViewerWidget(morphology)
    viewer.set_background_color( 1.0, 1.0, 1.0, 1.0 )
    def callback( morphology, viewer ):
        moose.start( frameRunTime )
        Vm = map( lambda x: moose.element( x ).Vm, compts )
        morphology.set_color( "group_all", Vm )
        currTime = moose.element( '/clock' ).currentTime
        #print currTime, compts[0].Vm
        if ( currTime < runtime ):
            return True
        return False

    viewer.set_callback( callback, idletime = 0 )
    viewer.showMaximized()
    viewer.show()
    app.exec_()

if __name__ == '__main__':
    main()
