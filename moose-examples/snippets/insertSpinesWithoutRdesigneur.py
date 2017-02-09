##svm,a jr  vmnhjg 
##avarbv adfba

import moogli
import moose
from PyQt4 import Qt, QtCore, QtGui
import sys
import os
sys.path.append( '../util' )

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
filename = 'barrionuevo_cell1zr.CNG.swc'
#filename = 'h10.CNG.swc'


    ################################################################
    # Utility function for building a compartment, used for spines.
def buildCompt( pa, name, length, dia, xoffset, RM, RA, CM ):
    compt = moose.Compartment( pa.path + '/' + name )
    compt.x0 = xoffset
    compt.y0 = 0
    compt.z0 = 0
    compt.x = length + xoffset
    compt.y = 0
    compt.z = 0
    compt.diameter = dia
    compt.length = length
    xa = dia * dia * PI / 4.0
    sa = length * dia * PI
    compt.Ra = length * RA / xa
    compt.Rm = RM / sa
    compt.Cm = CM * sa
    return compt

def makeSpineProto():
    spine = moose.Neutral( '/library/spine' )
    shaft = buildCompt( spine, 'shaft', 1e-6, 0.2e-6, 0, RM, RA, CM )
    head = buildCompt( spine, 'head', 0.5e-6, 0.5e-6, 1e-6, RM, RA, CM )
    moose.connect( shaft, 'axial', head, 'raxial' )

def main():
    """
This snippet illustrates how the Neuron class does the spine
specification, without the rdesigneur intermediate.

    """

    app = QtGui.QApplication(sys.argv)
    moose.Neutral( '/library' )
    makeSpineProto()
    model = moose.loadModel( filename, '/model' )
    model[0].buildSegmentTree()
    model[0].spineDistribution = [ \
                'spine', '#apical#', \
                'spacing', str( spineSpacing ), \
                'spacingDistrib', str( minSpacing ), \
                'angle', str( spineAngle ), \
                'angleDistrib', str( spineAngleDistrib ), \
                'size', str( spineSize ), \
                'sizeDistrib', str( spineSizeDistrib ), \
                '' \
               ]
    moose.reinit()

    # Now we set up the display
    compts = moose.wildcardFind( "/model/#[ISA=CompartmentBase]" )
    compts[0].inject = inject
    ecomptPath = [x.path for x in compts]
    morphology = moogli.read_morphology_from_moose(name = "", path = "/model")
    morphology.create_group( "group_all", ecomptPath, -0.08, 0.02, \
            [0.0, 0.5, 1.0, 1.0], [1.0, 0.0, 0.0, 0.9] )

    viewer = moogli.DynamicMorphologyViewerWidget(morphology)
    def callback( morphology, viewer ):
        moose.start( frameRunTime )
        Vm = [moose.element( x ).Vm for x in compts]
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
