#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
NeuroKitEditor for NeuroKit plugin.
"""

__author__      =   "Aviral Goel"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "Aviral Goel"
__email__       =   "goel.aviral@gmail.com"
__status__      =   "Development"

import mplugin
import moose
import pprint
# import NeuroKitEditorWidget
import default

from PyQt4 import QtGui
from PyQt4 import QtCore
from PyQt4 import Qt
from PyQt4.QtGui import QPushButton
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QHBoxLayout
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QDialog
from PyQt4.QtGui import QTableWidget
from PyQt4.QtGui import QTableWidgetItem
from PyQt4.QtGui import QCheckBox
from PyQt4.QtGui import QComboBox

from default import *
from mplugin import *
import moose
import neuroextractor
import moogli
import numpy as np
from global_constants import preferences
from NeuroKitVisualizer import MorphologyEditor

class NeuroKitEditor(mplugin.EditorBase):

    """
    NeuroKitEditor
    """

    def __init__(self, plugin, modelRoot):
        super(NeuroKitEditor, self).__init__(plugin)
        self._centralWidget = None #default.DefaultEditorWidget(None)
        self.modelRoot = modelRoot
        # self._centralWidget = NeuroKitEditorWidget.NeuroKitEditorWidget(modelRoot)
        self._menus         = []
        # self._propertyTable = MorphologyProperyTable()
        self._propertyTable = QWidget()
        self.__initMenus()
        self.__initToolBars()
        self.setModelRoot(modelRoot)
        #     if hasattr(self._centralWidget, 'init'):
        #         self._centralWidget.init()
        #     self._centralWidget.setModelRoot(self.plugin.modelRoot)
        # return self._centralWidget

    def __initMenus(self):
        return self._menus
        # editMenu = QtGui.QMenu('&Edit')
        # for menu in self.getCentralWidget().getMenus():
        #     editMenu.addMenu(menu)
        # self._menus.append(detailsButton)

    def __initToolBars(self):
        return self._toolBars

        # for toolbar in self.getCentralWidget().getToolBars():
        #     self._toolBars.append(toolbar)

    def getToolPanes(self):
        return super(NeuroKitEditor, self).getToolPanes()

    def getLibraryPane(self):
        return super(NeuroKitEditor, self).getLibraryPane()

    def getOperationsWidget(self):
        return super(NeuroKitEditor, self).getOperationsPane()

    # def getCentralWidget(self):
    #     """Retrieve or initialize the central widget.

    #     Note that we call the widget's setModelRoot() function
    #     explicitly with the plugin's modelRoot as the argument. This
    #     enforces an update of the widget display with the current
    #     modelRoot.

    #     This function should be overridden by any derived class as it
    #     has the editor widget class hard coded into it.
    #     """
    #     self._centralWidget.setModelRoot(self.plugin.modelRoot)
    #     return self._centralWidget

    def updateModelView(self):
        pass

    def setModelRoot(self, path):
        self.modelRoot = path
        self.updateModelView()

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
        self._centralWidget      =  default.EditorWidgetBase()#self.modelRoot)
        self._centralWidget.setLayout(QHBoxLayout())
        # self.plotWidgetContainer =  PlotWidgetContainer(self.modelRoot)
        self.geometry            =  neuroextractor.model(moose.element(self.modelRoot + "/model"))
        self.morphology          =  self.createMorphology(self.geometry)

        self.morphology.set_compartment_order(
            map(lambda x : x.path, self.compartmentOrder)
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
                                           , self.plugin
                                           )
        # self.scheduler           = self.getSchedulingDockWidget().widget()
        # self._centralWidget.setChildWidget(self.scheduler, False, 0,0,1,-1)
        self.visualizer.setGeometry( 0, 0, 1200, 400 )
        self.visualizer.show()
        # self.visualizer.start()
        self._centralWidget.layout().addWidget(self.visualizer)
        # self._centralWidget.setChildWidget(self.visualizer, False, 0, 0,-1,1)
        # self._centralWidget.setChildWidget(self.plotWidgetContainer, False, 0, 1,-1,1)
        # self._centralWidget.setPlotWidgetContainer(self.plotWidgetContainer)
        # label = QLabel("Aviral Goel")
        # self._centralWidget.setChildWidget(label, False, 0, 0)
        # self._centralWidget.setWindowTitle("Aviral Goel")
        # self.scheduler = self.getSchedulingDockWidget().widget()
        # self.scheduler.runner.update.connect(self.kkitRunView.getCentralWidget().changeBgSize)
        # self.scheduler.runner.resetAndRun.connect(self.kkitRunView.getCentralWidget().resetColor)

        ########################################################

        # self.schedular = self.getSchedulingDockWidget().widget()
        # self.schedular.runner.simulationProgressed.connect(self.update)
        # self.schedular.runner.simulationReset.connect(self.reset)
        preferences.getView().setCurrentIndex(1)
        preferences.getView().electricalBaseColorDialog.colorSelected.connect(lambda x: self.reset())
        preferences.getView().electricalPeakColorDialog.colorSelected.connect(lambda x: self.reset())
        preferences.getView().electricalBackgroundColorDialog.colorSelected.connect(lambda x: self.reset())
        preferences.getView().electricalPeakMembraneVoltage.editingFinished.connect(self.reset)
        preferences.getView().electricalBaseMembraneVoltage.editingFinished.connect(self.reset)


        # print("getting central widget")
        # self._centralWidget.show()
        # print(self.schedular.runner._updateInterval)
        self.reset()
        return self._centralWidget

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

    def reset(self):
        # print(" => reset called")
        prefs = preferences.getElectricalPreferences()
        self.setPeakColor(prefs["visualization"]["peak-color"])
        self.setBaseColor(prefs["visualization"]["base-color"])
        self.setBackgroundColor(prefs["visualization"]["background-color"])
        self.setBaseVm(prefs["visualization"]["base-membrane-voltage"])
        self.setPeakVm(prefs["visualization"]["peak-membrane-voltage"])
        self.updateVms()
        # self.visualizer.next()
        # self.updateIms()
            # self.ims[i] = self.compartmentOrder[i].Im
        # print(self.vms)
        # self.morphology.resetVms(self.vms)
        # self.morphology.resetIms(self.ims)


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
                # print( compartment_id
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

    def getCentralWidget(self):
        if self._centralWidget is None:
            self.createCentralWidget()
        # self._centralWidget.show()
        return self._centralWidget
