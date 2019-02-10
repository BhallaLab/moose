
# -*- coding: utf-8 -*-

"""
Moogli Visualizer.
"""
try:
    import moogli
except ImportError as e:
    print('[INFO] moogli not found' )
    quit()

# http://stackoverflow.com/questions/28104362/how-to-rotate-camera-in-openscenegraphwalk-through-in-model
from PyQt4 import QtGui
from PyQt4 import QtCore
from PyQt4 import Qt
from PyQt4.QtGui import QPushButton
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QVBoxLayout
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QDialog
from PyQt4.QtGui import QTableWidget
from PyQt4.QtGui import QTableWidgetItem
from PyQt4.QtGui import QCheckBox
from PyQt4.QtGui import QComboBox
import time
import moose
from moose import neuroml
import pprint
import numpy as np
import os
import sys


class MorphologyEditor(moogli.MorphologyViewer):

    def __init__(self, morphology, width, height):
        super(MorphologyEditor, self).__init__( morphology
                                              , width
                                              , height
                                              )
        self._timer = QtCore.QTimer(self)

    def start(self):
        self._timer.timeout.connect(self.__next__)
        self._timer.start(0)

    @QtCore.pyqtSlot()
    def show(self):
        self._timer.timeout.connect(self.__next__)
        super(MorphologyEditor, self).show()
        self._timer.start(0)

    @QtCore.pyqtSlot()
    def hide(self):
        self._timer.stop()
        super(MorphologyEditor, self).hide()

    def __next__(self):
        self.frame()
        info_id = self.select_info.get_id()
        info_event = self.select_info.get_event_type()
        if info_event == 1:
            self.select_info.set_event_type(0)


def model(element):
    """Given model root element, extracts morphology
    information about all neurons and compartments belonging
    to that model
    """
    return morphology(moose.element(element))
    # if element.path.endswith("/") :
    #     return morphology(moose.element(element.path + "cells"))
    # else:
    #     return morphology(moose.element(element.path + "/cells"))

def morphology(element):
    """Given the element containing all neurons, extracts
    morphology information from all child elements.
    """
    return { "name"     : element.name
           , "id"       : element.path
           , "type"     : etype(element)
           , "neurons"  : neurons(element)
           }

def neurons(element):
    """Given a neuron element, extracts morphology
    information from all child compartment elements.
    """
    retval = {}
    neurons = list(moose.wildcardFind(element.path +  "/##[TYPE=Neutral]"))
    neurons.extend(list(moose.wildcardFind(element.path +  "/##[TYPE=Neuron]")))
    for element in neurons:
        retval[element.path] = { "name"            : element.name
                              , "id"              : element.path
                              , "object"          : element
                              , "type"            : etype(element)
                              , "compartments"    : compartments(element)
                              }
    return retval

def compartments(element):
    retval = {}
    compartments = moose.wildcardFind(element.path + "/##[ISA=CompartmentBase]")
    for element in compartments:
        retval[element.path] = { "name"       : element.name
                         , "object"     : element
                         , "id"         : element.path
                         , "type"       : etype(element)
                         , "proximal"   : { "x" : element.x0
                                          , "y" : element.y0
                                          , "z" : element.z0
                                          }
                         , "distal"     : { "x" : element.x
                                          , "y" : element.y
                                          , "z" : element.z
                                          }
                         , "diameter"   : element.diameter
                         }
    return retval

def etype(element):
    element_type = element.name.rpartition("_")[0]
    if element_type == '': return element.name
    return element_type

def recurse(element, func, classType):
    return dict( [ func(child[0]) for child in element.children
                                  if isinstance(child[0], classType)
                 ]
               )

class MoogliViewer(QWidget):

    """
    NeuroKitEditor
    """

    def __init__(self, modelPath):
        super(MoogliViewer, self).__init__()
        self._centralWidget = None
        self.modelRoot = moose.element(modelPath)
        self.setLayout(QVBoxLayout())
        self.createCentralWidget()


    def setBaseColor(self, color):
        self.morphology.set_initial_color( color[0] / 255.0
                                         , color[1] / 255.0
                                         , color[2] / 255.0
                                         , color[3] / 255.0
                                         )

    def setPeakColor(self, color):
        self.morphology.set_final_color( color[0] / 255.0
                                       , color[1] / 255.0
                                       , color[2] / 255.0
                                       , color[3] / 255.0
                                       )

    def setBackgroundColor(self, color):
        self.visualizer.set_background_color( color[0] / 255.0
                                            , color[1] / 255.0
                                            , color[2] / 255.0
                                            , color[3] / 255.0
                                            )

    def setBaseVm(self, vm):
        self.morphology.set_base_membrane_voltage(vm)

    def setPeakVm(self, vm):
        self.morphology.set_peak_membrane_voltage(vm)


    def createCentralWidget(self):
        self.geometry            =  model(moose.element(self.modelRoot.path))
        self.morphology          =  self.createMorphology(self.geometry)

        self.morphology.set_compartment_order(
            [x.path for x in self.compartmentOrder]
                                           )

        self.vms = np.empty(len(self.compartmentOrder), dtype=np.float, order='C')
        self.ims = np.empty(len(self.compartmentOrder), dtype=np.float, order='C')
        # self.visualizer.insertPlainText(pprint.pformat(self.geometry, indent = 4))
        # self.visualizer          =  QTextEdit()#NeuroKitVisualizer(self.modelRoot)
        desktop = QtGui.QApplication.desktop()
        # print("**********************")
        # print(desktop.screenGeometry())
        # print("***********************")

        self.visualizer  = MorphologyEditor( self.morphology
                                           , desktop.screenGeometry().width()
                                           , desktop.screenGeometry().height()
                                           )
        # self.scheduler           = self.getSchedulingDockWidget().widget()
        # self._centralWidget.setChildWidget(self.scheduler, False, 0,0,1,-1)
        # self.visualizer.setGeometry( 0, 0, desktop.screenGeometry().width(), desktop.screenGeometry().height() )
        self.visualizer.showMaximized()
        self.visualizer.show()
        # self.visualizer.start()
        self.layout().addWidget(self.visualizer)

    def update(self, time):
        # print("Update called => ", time)
        # print("Update called")
        # for neuron_id in self.geometry["neurons"]:
        #     neuron = self.geometry["neurons"][neuron_id]
        #     for compartment_id in neuron["compartments"]:
        #         voltage = neuron["compartments"][compartment_id]["object"].Vm
        #         print(compartment_id + " => " + str(voltage))

        # self.visualizer.
        self.updateVms()
        # self.updateIms()
        # self.visualizer.next()
        # print(self.vms)

    def updateVms(self):
        for i in range(0, len(self.compartmentOrder)):
            self.vms[i] = self.compartmentOrder[i].Vm
        self.morphology.set_membrane_voltages(self.vms)

    def updateIms(self):
        for i in range(0, len(self.compartmentOrder)):
            self.ims[i] = self.compartmentOrder[i].Im

    def getCompartmentOrder(self):
        return self.compartmentOrder

    def createMorphology(self, geometry):
        # import json
        # f = open("/home/aviral/purkinje.json", "w")
        # f.write(json.dumps(geometry, indent=4))
        # f.close()
        # morphology = moogli.Morphology("morph")
        # morphology.add_compartment( "a"
        #                           , "b", 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0)
        # morphology.add_compartment( "c"
        #                           , "b"
        #                           , 3.0, 3.0, 3.0, 3.0, 4.0, 4.0, 4.0, 4.0)
        morphology = moogli.Morphology("morph", 1)
        self.compartmentOrder               = []
        for neuron_id in geometry["neurons"]:
            neuron = geometry["neurons"][neuron_id]
            for compartment_id in neuron["compartments"]:
                compartment = neuron["compartments"][compartment_id]
                print( compartment_id)
                #      , neuron_id
                #      , compartment["proximal"]["x"]
                #      , compartment["proximal"]["y"]
                #      , compartment["proximal"]["z"]
                #      , compartment["diameter"]
                #      , compartment["distal"]["x"]
                #      , compartment["distal"]["y"]
                #      , compartment["distal"]["z"]
                #      , compartment["diameter"]
                #      )
                self.compartmentOrder.append(compartment["object"])
                morphology.add_compartment( compartment_id
                                          , neuron_id
                                          , compartment["proximal"]["x"] * 10000000
                                          , compartment["proximal"]["y"] * 10000000
                                          , compartment["proximal"]["z"] * 10000000
                                          , compartment["diameter"]      * 10000000
                                          , compartment["distal"]["x"]   * 10000000
                                          , compartment["distal"]["y"]   * 10000000
                                          , compartment["distal"]["z"]   * 10000000
                                          , compartment["diameter"]      * 10000000
                                          )
        return morphology



def main():
    filename = os.path.join( os.path.split(os.path.realpath(__file__))[0]
                           , "../neuroml/CA1/CA1.morph.pop.xml")


    # filename = os.path.join( os.path.split(os.path.realpath(__file__))[0]
    #                        , "../neuroml/PurkinjeCellPassivePulseInput/PurkinjePassive.net.xml")
    # filename = os.path.join( os.path.split(os.path.realpath(__file__))[0]
    #                        , "../neuroml/OlfactoryBulbPassive/OBpassive_numgloms3_seed750.0.xml")

    popdict, projdict = moose.neuroml.loadNeuroML_L123(filename)
    modelRoot   = moose.Neutral("/" + os.path.splitext(os.path.basename(filename))[0])
    element = moose.Neutral(modelRoot.path + "/model")
    if(moose.exists("/cells"))  : moose.move("/cells"  , element.path)
    if(moose.exists("/elec"))   : moose.move("/elec"   , modelRoot.path)
    if(moose.exists("/library")): moose.move("/library", modelRoot.path)
    show_morphology(modelRoot.path)

def show_morphology(modelpath):
    app = QtGui.QApplication(sys.argv)
    widget = MoogliViewer(modelpath)
    # widget.setBaseVm()
    widget.showMaximized()
    # widget.setStyleSheet("background-color: black;")
    widget.show()
    return app.exec_()

if __name__ == "__main__":
    main()
