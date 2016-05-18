import moogli
import moose
from moose import neuroml
from PyQt4 import Qt, QtCore, QtGui
import sys
import os

frameRunTime = 0.0002
runtime = 1.0
inject = 5e-10
simdt = 5e-5

def main():
    app = QtGui.QApplication(sys.argv)
    filename = 'barrionuevo_cell1zr.CNG.swc'
    moose.Neutral( '/library' )
    moose.Neutral( '/model' )
    cell = moose.loadModel( filename, '/model/testSwc' )
    for i in range( 8 ):
        moose.setClock( i, simdt )
    hsolve = moose.HSolve( '/model/testSwc/hsolve' )
    hsolve.dt = simdt
    hsolve.target = '/model/testSwc/soma'
    moose.le( cell )
    moose.reinit()

    # Now we set up the display
    compts = moose.wildcardFind( "/model/testSwc/#[ISA=CompartmentBase]" )
    compts[0].inject = inject
    ecomptPath = map( lambda x : x.path, compts )
    morphology = moogli.read_morphology_from_moose(name = "", path = "/model/testSwc")
    morphology.create_group( "group_all", ecomptPath, -0.08, 0.02, \
            [0.0, 0.5, 1.0, 1.0], [1.0, 0.0, 0.0, 0.9] ) 

    viewer = moogli.DynamicMorphologyViewerWidget(morphology)
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
