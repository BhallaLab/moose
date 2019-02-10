try:
    import moogli
except ImportError as e:
    print( "[INFO ] Could not import moogli. Quitting..." )
    quit()

import moose
from moose import neuroml
from PyQt4 import Qt, QtCore, QtGui
import sys
import os

app = QtGui.QApplication(sys.argv)
filename = os.path.join( os.path.split(os.path.realpath(__file__))[0]
                       , "../neuroml/PurkinjeCellPassivePulseInput/PurkinjePassive.net.xml"
                       )
moose.neuroml.loadNeuroML_L123(filename)
morphology_distal = moogli.read_morphology_from_moose(name = "", path = "/cells[0]", radius = moogli.DISTAL)
viewer_distal = moogli.MorphologyViewerWidget(morphology_distal)
viewer_distal.setWindowTitle("Distal Radius")

morphology_averaged = moogli.read_morphology_from_moose(name = "", path = "/cells[0]", radius = moogli.AVERAGED)
viewer_averaged = moogli.MorphologyViewerWidget(morphology_averaged)
viewer_averaged.setWindowTitle("Averaged Radius")

morphology_proximal_distal = moogli.read_morphology_from_moose(name = "", path = "/cells[0]", radius = moogli.PROXIMAL_DISTAL)
viewer_proximal_distal = moogli.MorphologyViewerWidget(morphology_proximal_distal)
viewer_proximal_distal.setWindowTitle("Continous Variation in Radius")

viewer_distal.show()
viewer_averaged.show()
viewer_proximal_distal.show()
app.exec_()
