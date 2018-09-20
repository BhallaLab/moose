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
import random
import numpy as np

app = QtGui.QApplication(sys.argv)
filename = os.path.join( os.path.split(os.path.realpath(__file__))[0]
                       , "../neuroml/PurkinjeCellPassivePulseInput/PurkinjePassive.net.xml"
                       )
moose.neuroml.loadNeuroML_L123(filename)
morphology = moogli.read_morphology_from_moose(name = "", path = "/cells[0]")
morphology.create_group( "group-1"
                       , [ "/cells[0]/BigCellCML_0[0]/Seg0_dend_1043_1044[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1033_1034[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1019_1020[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1018_1019[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1016_1017[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1539_1540[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1579_1580[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1573_1574[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1572_1573[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1569_1570[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1559_1560[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1045_1046[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1021_1022[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1020_1021[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1581_1582[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1580_1581[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1046_1047[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1022_1023[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1027_1028[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1023_1024[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1028_1029[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1025_1026[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1024_1025[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1026_1027[0]"
                         ]
                       , 10.0
                       , 200.0
                       , [1.0, 0.0, 0.0, 1.0]
                       , [0.0, 1.0, 0.0, 1.0]
                       )
morphology.create_group( "group-2"
                       , [ "/cells[0]/BigCellCML_0[0]/Seg0_dend_1076_1077[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1072_1073[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1099_1100[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1096_1097[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1108_1109[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1135_1136[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_35_36[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_655_656[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_535_536[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_716_717[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_14_15[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_87_88[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_236_237[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_218_219[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_172_173[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_152_153[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_363_364[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_362_363[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_344_345[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_341_342[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_320_321[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_312_313[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_453_454[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_449_450[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_437_438[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_426_427[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_418_419[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_409_410[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_407_408[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_396_397[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_394_395[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_390_391[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_389_390[0]"
                         , "/cells[0]/BigCellCML_0[0]/Seg0_dend_1079_1080[0]"
                         ]
                       , 0.0
                       , 1.0
                       , [0.0, 1.0, 0.0, 1.0]
                       , [0.0, 0.0, 1.0, 1.0]
                       )
def callback(morphology, viewer):
    morphology.set_color( "group-1"
                        , np.random.random_sample((24,)) * (100.0 - 20.0) + 20.0
                        )
    morphology.set_color( "group-2"
                        , np.random.random_sample((34,))
                        )
    viewer.roll(0.05, 0)
    viewer.pitch(0.05, 1)
    viewer.yaw(0.05, 2)
    return True

viewer = moogli.DynamicMorphologyViewerWidget(morphology)
viewer.showMaximized()
viewer.split_horizontally(0)
viewer.split_vertically(1)
viewer.zoom(0.5, 0)
viewer.zoom(0.5, 1)
viewer.zoom(0.5, 2)
viewer.set_callback(callback)
app.exec_()
