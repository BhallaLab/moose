from __future__ import print_function
import sys
from PyQt4 import Qt, QtGui, QtCore
from PyQt4.QtCore import QObject
from PyQt4.QtCore import pyqtSignal
from PyQt4.QtGui import QColor
from PyQt4.QtGui import QGroupBox
from PreferencesView import PreferencesView
import moose
from defines import *
import json
import os

PREFERENCES_FILEPATH = os.path.join( os.path.dirname(os.path.abspath(__file__))
                                   , "datastore/preferences.json"
                                   )

class PreferencesPresenter(QObject):

    electricalSimulationIntervalChanged         =   pyqtSignal(float)
    electricalPlotUpdateIntervalChanged         =   pyqtSignal(float)
    electricalDefaultSimulationRuntimeChanged   =   pyqtSignal(float)
    electricalDiffusionIntervalChanged          =   pyqtSignal(float)
    electricalGuiUpdateIntervalChanged          =   pyqtSignal(float)
    electricalSolverChanged                     =   None

    chemicalSimulationIntervalChanged           =  pyqtSignal(float)
    chemicalPlotUpdateIntervalChanged           =  pyqtSignal(float)
    chemicalDefaultSimulationRuntimeChanged     =  pyqtSignal(float)
    chemicalDiffusionIntervalChanged            =  pyqtSignal(float)
    chemicalGuiUpdateIntervalChanged            =  pyqtSignal(float)
    chemicalSolverChanged                       =  None

    applyChemicalSettings   =  pyqtSignal(object)
    applyElectricalSettings =  pyqtSignal(object)

    def __init__(self, source = PREFERENCES_FILEPATH):
        super(PreferencesPresenter, self).__init__()
        self.source = source
        with open(self.source) as f:
            self.preferences = json.loads(f.read())

        self.view = self.createView()
        self.initializeView()
        # self.view.closed.connect(self.save)
        # self.connectSignals()
        # self.connectSlots()
        # self.initializeClocks()
        self.view.electricalSimulationApply.clicked.connect(self.applyElectricalSimulationSettings)
        self.view.electricalSimulationCancel.clicked.connect(self.cancelElectricalSimulationSettings)
        self.view.electricalVisualizationApply.clicked.connect(self.applyElectricalVisualizationSettings)
        self.view.electricalVisualizationCancel.clicked.connect(self.cancelElectricalVisualizationSettings)
        self.view.chemicalSimulationApply.clicked.connect(self.applyChemicalSimulationSettings)
        self.view.chemicalSimulationCancel.clicked.connect(self.cancelChemicalSimulationSettings)

    def save(self):
        self.getElectricalPreferences()
        self.getChemicalPreferences()
        # http://stackoverflow.com/questions/12309269/write-json-data-to-file-in-python
        with open(self.source, "w") as f:
            json.dump( self.preferences
                     , f
                     , sort_keys = True
                     , indent = 4
                     , ensure_ascii=False
                     )

    def getView(self):
        return self.view

    def createView(self):
        view = PreferencesView()
        return view

    def initializeView(self):
        self.setElectricalPreferences()
        self.setChemicalPreferences()

    def setElectricalPreferences(self):
        self.view.electricalSimulationDt.setText(
            str(float(self.preferences["electrical"]["simulation"]["simulation-dt"]))
                                                      )

        self.view.electricalPlotUpdateInterval.setText(
            str(float(self.preferences["electrical"]["simulation"]["plot-update-interval"]))
                                                      )

        self.view.electricalDefaultSimulationRuntime.setText(
            str(float(self.preferences["electrical"]["simulation"]["default-runtime"]))
                                                            )

        self.view.electricalGuiUpdateInterval.setText(
            str(float(self.preferences["electrical"]["simulation"]["gui-update-interval"]))
                                                     )

        self.view.electricalSolvers[str(self.preferences["electrical"]["simulation"]["solver"])].setChecked(True)

        self.view.electricalPeakMembraneVoltage.setText(
            str(float(self.preferences["electrical"]["visualization"]["peak-membrane-voltage"]))
                                                       )

        self.view.electricalBaseMembraneVoltage.setText(
            str(float(self.preferences["electrical"]["visualization"]["base-membrane-voltage"]))
                                                       )

        (r, g, b, a) = self.preferences["electrical"]["visualization"]["base-color"]
        # print((r,g,b,a))
        color = QColor(r, g, b, a)
        # print(color.name())
        self.view.electricalBaseColorDialog.setCurrentColor(color)
        self.view.electricalBaseColorButton.setStyleSheet(
            "QPushButton {"
        +   "background-color: {0}; color: {0};".format(color.name())
        +   "}"
                                                    )


        (r, g, b, a) = self.preferences["electrical"]["visualization"]["peak-color"]
        # print((r,g,b,a))
        color = QColor(r, g, b, a)
        # print(color.name())
        self.view.electricalPeakColorDialog.setCurrentColor(QColor(r, g, b, a))
        self.view.electricalPeakColorButton.setStyleSheet(
            "QPushButton {"
        +   "background-color: {0}; color: {0};".format(color.name())
        +   "}"
                                                    )


        (r, g, b, a) = self.preferences["electrical"]["visualization"]["background-color"]
        # print((r,g,b,a))
        color = QColor(r, g, b, a)
        # print(color.name())
        self.view.electricalBackgroundColorDialog.setCurrentColor(QColor(r, g, b, a))
        self.view.electricalBackgroundColorButton.setStyleSheet(
            "QPushButton {"
        +   "background-color: {0}; color: {0};".format(color.name())
        +   "}"
                                                    )

    def setChemicalPreferences(self):
        self.view.chemicalSimulationDt.setText(
            str(float(self.preferences["chemical"]["simulation"]["simulation-dt"]))
                                              )

        self.view.chemicalPlotUpdateInterval.setText(
            str(float(self.preferences["chemical"]["simulation"]["plot-update-interval"]))
                                                    )

        self.view.chemicalDefaultSimulationRuntime.setText(
            str(float(self.preferences["chemical"]["simulation"]["default-runtime"]))
                                                          )

        self.view.chemicalDiffusionDt.setText(
            str(float(self.preferences["chemical"]["simulation"]["diffusion-dt"]))
                                             )

        self.view.chemicalGuiUpdateInterval.setText(
            str(float(self.preferences["chemical"]["simulation"]["gui-update-interval"]))
                                                   )

        self.view.chemicalSolvers[str(self.preferences["chemical"]["simulation"]["solver"])].setChecked(True)


    def connectSignals(self):
        self.connectElectricalSignals()
        self.connectChemicalSignals()

    def connectElectricalSignals(self):
        self.connectIntervalEditorToSignal( self.view.electricalSimulationInterval
                                          , self.electricalSimulationIntervalChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.electricalPlotUpdateInterval
                                          , self.electricalPlotUpdateIntervalChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.electricalDefaultSimulationRuntime
                                          , self.electricalDefaultSimulationRuntimeChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.electricalDiffusionInterval
                                          , self.electricalDiffusionIntervalChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.electricalGuiUpdateInterval
                                          , self.electricalGuiUpdateIntervalChanged
                                          )
        self.electricalSolverChanged = self.view.electricalSolver.currentIndexChanged


    def connectChemicalSignals(self):
        self.connectFloatinEditorToSignal( self.view.chemicalSimulationInterval
                                          , self.chemicalSimulationIntervalChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.chemicalPlotUpdateInterval
                                          , self.chemicalPlotUpdateIntervalChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.chemicalDefaultSimulationRuntime
                                          , self.chemicalDefaultSimulationRuntimeChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.chemicalDiffusionInterval
                                          , self.chemicalDiffusionIntervalChanged
                                          )

        self.connectIntervalEditorToSignal( self.view.chemicalGuiUpdateInterval
                                          , self.chemicalGuiUpdateIntervalChanged
                                          )

        self.chemicalSolverChanged = self.view.chemicalSolver.currentIndexChanged
        # self.view.electricalVisualizationApply.connect(self.applyElectricalVisualizationSettings)
        # self.view.electricalVisualizationCancel.connect(self.cancelElectricalVisualizationSettings)



    def connectIntervalEditorToSignal(self, intervalEditor, signal):
        intervalEditor.returnPressed.connect(
            lambda: signal.emit(float(intervalEditor.text())))
        intervalEditor.editingFinished.connect(
            lambda : signal.emit(float(intervalEditor.text())))

    def setClocks(self, clocks, dt):
        return
        for clock in clocks:
            moose.setClock(clock, dt)

    def setElectricalSolver(self, solver):
        self.view.electricalSolvers[solver].setChecked(True)

    def setChemicalSolver(self, solver):
        self.view.chemicalSolvers[solver].setChecked(True)

    def getElectricalPreferences(self):
        self.preferences["electrical"]["simulation"]["simulation-dt"]           = float(self.view.electricalSimulationDt.text())
        self.preferences["electrical"]["simulation"]["plot-update-interval"]    = float(self.view.electricalPlotUpdateInterval.text())
        self.preferences["electrical"]["simulation"]["gui-update-interval"]     = float(self.view.electricalGuiUpdateInterval.text())
        self.preferences["electrical"]["simulation"]["default-runtime"]         = float(self.view.electricalDefaultSimulationRuntime.text())
        self.preferences["electrical"]["simulation"]["solver"]                  = str(self.view.electricalSolver.checkedButton().text())
        self.preferences["electrical"]["visualization"]["peak-membrane-voltage"]= float(self.view.electricalPeakMembraneVoltage.text())
        self.preferences["electrical"]["visualization"]["base-membrane-voltage"]= float(self.view.electricalBaseMembraneVoltage.text())
        self.preferences["electrical"]["visualization"]["base-color"]           = [ self.view.electricalBaseColorDialog.currentColor().red()
                                                                                  , self.view.electricalBaseColorDialog.currentColor().green()
                                                                                  , self.view.electricalBaseColorDialog.currentColor().blue()
                                                                                  , self.view.electricalBaseColorDialog.currentColor().alpha()
                                                                                  ]
        self.preferences["electrical"]["visualization"]["peak-color"]           = [ self.view.electricalPeakColorDialog.currentColor().red()
                                                                                  , self.view.electricalPeakColorDialog.currentColor().green()
                                                                                  , self.view.electricalPeakColorDialog.currentColor().blue()
                                                                                  , self.view.electricalPeakColorDialog.currentColor().alpha()
                                                                                  ]
        self.preferences["electrical"]["visualization"]["background-color"]     = [ self.view.electricalBackgroundColorDialog.currentColor().red()
                                                                                  , self.view.electricalBackgroundColorDialog.currentColor().green()
                                                                                  , self.view.electricalBackgroundColorDialog.currentColor().blue()
                                                                                  , self.view.electricalBackgroundColorDialog.currentColor().alpha()
                                                                                  ]
        return self.preferences["electrical"]

    def getChemicalPreferences(self):
        self.preferences["chemical"]["simulation"]["simulation-dt"]             = float(self.view.chemicalSimulationDt.text())
        self.preferences["chemical"]["simulation"]["diffusion-dt"]              = float(self.view.chemicalDiffusionDt.text())
        self.preferences["chemical"]["simulation"]["plot-update-interval"]      = float(self.view.chemicalPlotUpdateInterval.text())
        self.preferences["chemical"]["simulation"]["gui-update-interval"]       = float(self.view.chemicalGuiUpdateInterval.text())
        self.preferences["chemical"]["simulation"]["default-runtime"]           = float(self.view.chemicalDefaultSimulationRuntime.text())
        self.preferences["chemical"]["simulation"]["solver"]                    = str(self.view.chemicalSolver.checkedButton().text())
        return self.preferences["chemical"]

    # def setElectricalSolver(self):
    #     neurons = [ child[0]
    #                 for child in moose.element(self.model.path + "/cells")
    #                 if isinstance(child[0], moose.Neuron)
    #               ]
    #     for neuron in neurons:
    #         solver(neuron.path + "/solver")

    # def setChemicalSolver(self):
    #     pass


    # def setElectricalClocks(self, dt):
    #     self.setClocks(1, 7, dt)

    # def setChemicalClocks(self, dt):
    #     self.setClocks(11, 17, dt)

    # def setDiffusionClocks(self, dt):
    #     self.setClocks(10, 10, dt)

    # def setClocks(self, dt):
    #     self.setClocks(10, 10, dt)

    def setChemicalClocks(self):
        self.setClocks2(CHEMICAL_SIMULATION_DT_CLOCKS , self.preferences["chemical"]["simulation"]["simulation-dt"])
        self.setClocks2(CHEMICAL_PLOT_UPDATE_INTERVAL_CLOCKS, self.preferences["chemical"]["simulation"]["plot-update-interval"])
        # self.setClocks2(CHEMICAL_DIFFUSION_INTERVAL_CLOCKS, self.preferences["chemical"]["simulation"]["diffusion-dt"])

    def setClocks2(self, clocks, dt):
        for clock in clocks:
            moose.setClock(clock, dt)

    def connectSlots(self):
        self.connectElectricalSlots()
        self.connectChemicalSlots()

    def connectElectricalSlots(self):
        self.electricalSimulationIntervalChanged.connect(
            lambda dt: self.setClocks(ELECTRICAL_SIMULATION_INTERVAL_CLOCKS, dt))
        self.electricalPlotUpdateIntervalChanged.connect(
            lambda dt: self.setClocks(ELECTRICAL_PLOT_UPDATE_INTERVAL_CLOCKS, dt))
        self.electricalDiffusionIntervalChanged.connect(
            lambda dt: self.setClocks(ELECTRICAL_DIFFUSION_INTERVAL_CLOCKS, dt))
        self.electricalSimulationIntervalChanged.connect(
            lambda dt: self.setValue( self.electricalPreferences
                                    , "simulationInterval"
                                    , dt
                                    ))
        self.electricalPlotUpdateIntervalChanged.connect(
            lambda dt: self.setValue( self.electricalPreferences
                                    , "plotUpdateInterval"
                                    , dt
                                    ))
        self.electricalDefaultSimulationRuntimeChanged.connect(
            lambda dt: self.setValue( self.electricalPreferences
                                    , "defaultSimulationRuntime"
                                    , dt
                                    ))
        self.electricalDiffusionIntervalChanged.connect(
            lambda dt: self.setValue( self.electricalPreferences
                                    , "diffusionInterval"
                                    , dt
                                    ))
        self.electricalGuiUpdateIntervalChanged.connect(
            lambda dt: self.setValue( self.electricalPreferences
                                    , "guiUpdateInterval"
                                    , dt
                                    ))
        self.electricalSolverChanged.connect(
            lambda x: self.setValue( self.electricalPreferences
                                   , "solverIndex"
                                   , self.view.electricalSolver.currentIndex()
                                   ))
        # self.view.electricalSimulationApply.connect(
        #                                            )


    def setValue(self, dictionary, key, value):
        dictionary[key] = value

    def connectChemicalSlots(self):
        self.chemicalSimulationIntervalChanged.connect(
            lambda dt: self.setClocks(CHEMICAL_SIMULATION_INTERVAL_CLOCKS, dt))
        self.chemicalPlotUpdateIntervalChanged.connect(
            lambda dt: self.setClocks(CHEMICAL_PLOT_UPDATE_INTERVAL_CLOCKS, dt))
        self.chemicalDiffusionIntervalChanged.connect(
            lambda dt: self.setClocks(CHEMICAL_DIFFUSION_INTERVAL_CLOCKS, dt))
        self.chemicalSimulationIntervalChanged.connect(
            lambda dt: self.setValue( self.chemicalPreferences
                                    , "simulationInterval"
                                    , dt
                                    ))
        self.chemicalPlotUpdateIntervalChanged.connect(
            lambda dt: self.setValue( self.chemicalPreferences
                                    , "plotUpdateInterval"
                                    , dt
                                    ))
        self.chemicalDefaultSimulationRuntimeChanged.connect(
            lambda dt: self.setValue( self.chemicalPreferences
                                    , "defaultSimulationRuntime"
                                    , dt
                                    ))
        self.chemicalDiffusionIntervalChanged.connect(
            lambda dt: self.setValue( self.chemicalPreferences
                                    , "diffusionInterval"
                                    , dt
                                    ))
        self.chemicalGuiUpdateIntervalChanged.connect(
            lambda dt: self.setValue( self.chemicalPreferences
                                    , "guiUpdateInterval"
                                    , dt
                                    ))
        self.chemicalSolverChanged.connect(
            lambda dt: self.setValue( self.chemicalPreferences
                                    , "solverIndex"
                                    , self.view.chemicalSolver.currentIndex()
                                    ))

    def initializeClocks(self):
        self.initializeElectricalClocks()
        self.initializeChemicalClocks()

    def initializeElectricalClocks(self):
        self.setClocks( ELECTRICAL_SIMULATION_DT_CLOCKS
                      , self.preferences["electrical"]["simulation"]["simulation-dt"]
                      )
        self.setClocks( ELECTRICAL_PLOT_UPDATE_INTERVAL_CLOCKS
                      , self.preferences["electrical"]["simulation"]["plot-update-interval"]
                      )

    def initializeChemicalClocks(self):
        self.setClocks( CHEMICAL_SIMULATION_DT_CLOCKS
                      , self.preferences["chemical"]["simulation"]["simulation-dt"]
                      )
        self.setClocks( CHEMICAL_DIFFUSION_DT_CLOCKS
                      , self.preferences["chemical"]["simulation"]["diffusion-dt"]
                      )
        self.setClocks( CHEMICAL_PLOT_UPDATE_INTERVAL_CLOCKS
                      , self.preferences["chemical"]["simulation"]["plot-update-interval"]
                      )

    def applyElectricalSimulationSettings(self):
        self.getElectricalPreferences()
        self.view.close()
        self.applyElectricalSettings.emit(self.preferences["electrical"])

    def cancelElectricalSimulationSettings(self):
        self.setElectricalPreferences()
        self.view.close()

    def applyChemicalSimulationSettings(self):
        self.getChemicalPreferences()
        self.setChemicalClocks()
        self.view.close()
        self.applyChemicalSettings.emit(self.preferences["chemical"])

    def cancelChemicalSimulationSettings(self):
        self.setChemicalPreferences()
        self.view.close()

    def applyElectricalVisualizationSettings(self):
        self.getElectricalPreferences()
        self.applyElectricalSettings.emit(self.preferences["electrical"])
        self.view.close()

    def cancelElectricalVisualizationSettings(self):
        self.setElectricalPreferences()
        self.view.close()


def main():
    app     = QtGui.QApplication(sys.argv)
    preferences  = PreferencesPresenter()
    preferences.view.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
