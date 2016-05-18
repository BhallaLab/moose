from mplugin import RunBase
############################################################
#
# View for running a simulation and runtime visualization
#
############################################################


from mplot import CanvasWidget
import RunWidget
from PlotWidgetContainer import PlotWidgetContainer
from NeuroKitVisualizer import MorphologySimulator
from PyQt4 import QtGui, Qt
from PyQt4.QtGui import QLabel
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QToolBar
from PyQt4.QtGui import QPushButton
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QTextEdit
import pprint
# from PyQt4.QtGui import QToolBa
from default import *
import moose
import neuroextractor
import moogli
import numpy as np
from global_constants import preferences


class NeuroKitRunner(RunView):

    def __init__(self, modelRoot, plugin,*args):
        # super(NeuroKitRunner, self).__init__(self, moose.element(modelRoot), plugin, *args)
        RunView.__init__(self, modelRoot, plugin, *args)
        # RunBase.__init__(self, plugin)
        self.modelRoot      =   modelRoot
        self.plugin         =   plugin
        # self.geometry       =   None
        # self.morphology     =   None
        self.schedular      = self.getSchedulingDockWidget().widget()

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
        self._centralWidget      =  RunWidget.RunWidget(self.modelRoot)
        self.plotWidgetContainer =  PlotWidgetContainer(self.modelRoot)
        self.geometry            =  neuroextractor.model(moose.element(self.modelRoot + "/model"))
        self.morphology          =  self.createMorphology(self.geometry)

        self.morphology.set_compartment_order(
            map(lambda x : x.path, self.compartmentOrder)
                                           )

        self.vms = np.empty(len(self.compartmentOrder), dtype=np.float, order='C')
        self.ims = np.empty(len(self.compartmentOrder), dtype=np.float, order='C')
        # self.visualizer.insertPlainText(pprint.pformat(self.geometry, indent = 4))
        # self.visualizer          =  QTextEdit()#NeuroKitVisualizer(self.modelRoot)
        self.visualizer  = MorphologySimulator(self.morphology, 1000, 1000, self.plugin)
        # self.scheduler           = self.getSchedulingDockWidget().widget()
        # self._centralWidget.setChildWidget(self.scheduler, False, 0,0,1,-1)
        self.visualizer.setGeometry( 0, 0, 800, 600 )
        self.visualizer.show()
        # self.visualizer.start()
        self._centralWidget.setChildWidget(self.visualizer, False, 0, 0,-1,1)
        self._centralWidget.setChildWidget(self.plotWidgetContainer, False, 0, 1,-1,1)
        self._centralWidget.setPlotWidgetContainer(self.plotWidgetContainer)
        # label = QLabel("Aviral Goel")
        # self._centralWidget.setChildWidget(label, False, 0, 0)
        # self._centralWidget.setWindowTitle("Aviral Goel")
        # self.scheduler = self.getSchedulingDockWidget().widget()
        # self.scheduler.runner.update.connect(self.kkitRunView.getCentralWidget().changeBgSize)
        # self.scheduler.runner.resetAndRun.connect(self.kkitRunView.getCentralWidget().resetColor)
        self.schedular = self.getSchedulingDockWidget().widget()
        self.schedular.runner.simulationProgressed.connect(self.update)
        self.schedular.runner.simulationReset.connect(self.reset)
        self.schedular.preferences.getView().setCurrentIndex(1)
        self.schedular.preferences.getView().electricalBaseColorDialog.colorSelected.connect(lambda x: self.reset())
        self.schedular.preferences.getView().electricalPeakColorDialog.colorSelected.connect(lambda x: self.reset())
        self.schedular.preferences.getView().electricalBackgroundColorDialog.colorSelected.connect(lambda x: self.reset())
        self.schedular.preferences.getView().electricalPeakMembraneVoltage.editingFinished.connect(self.reset)
        self.schedular.preferences.getView().electricalBaseMembraneVoltage.editingFinished.connect(self.reset)


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
