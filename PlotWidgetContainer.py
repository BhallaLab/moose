
# -*- coding: utf-8 -*-
from __future__ import print_function

"""
"""
__credits__     =   ["Upi Lab"]
__author__      =   "Aviral Goel"
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "Aviral Goel", "HarshaRani"
__email__       =   "goel.aviral@gmail.com"
__status__      =   "Development"
__updated__     =   "Jul 26 2017"


import sys
import os
import PyQt4
from PyQt4 import QtGui, Qt
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QSizeGrip
from PyQt4.QtGui import QDockWidget
from PyQt4.QtGui import QLayout
from PyQt4.QtGui import QVBoxLayout
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QScrollArea
from PyQt4.QtGui import QToolBar
from PyQt4.QtGui import QSizeGrip
from PyQt4.QtGui import QSplitter

import moose
import default
import sidebar
# from default import PlotWidget

ELECTRICAL = 0
CHEMICAL = 1
class PlotWidgetContainer(QWidget):

    def __init__(self, modelRoot, *args, **kwargs):

        super(PlotWidgetContainer, self).__init__(*args)
        self.modelRoot = modelRoot
        if len(moose.wildcardFind(modelRoot + "/##[ISA=ChemCompt]")) == 0:
            self.modelType = ELECTRICAL
        else:
            self.modelType = CHEMICAL

        self.model          = moose.element(self.modelRoot)
        if self.modelRoot != "/":
            self.modelRoot = self.findModelPath(self.modelRoot)
            if moose.exists(modelRoot + "/data"):
                self.data   = moose.element(self.modelRoot + "/data")
            else:
                self.data   = moose.Neutral(self.modelRoot + "/data")

        else:
            self.data       = moose.element("/data")

        self._layout        = QVBoxLayout()
        self.graphs         = QSplitter()
        self.graphs.setOrientation(PyQt4.QtCore.Qt.Vertical)
        self.graphsArea     = QScrollArea()
        # self.graphsLayout   = QGridLayout()
        # self.menubar        = self.createMenuBar()
        self.rowIndex       = 0
            # self.setSizePolicy( QtGui.QSizePolicy.Expanding
        #                   , QtGui.QSizePolicy.Expanding
        #                   )

        self.graphs.setSizePolicy( QtGui.QSizePolicy.Expanding
                                 , QtGui.QSizePolicy.Expanding
                                 )
        self.setAcceptDrops(True)
        # self._layout.setSizeConstraint( QLayout.SetNoConstraint )
        # self.graphs.setLayout(self.graphsLayout)
        self.graphsArea.setWidget(self.graphs)
        self.graphsArea.setWidgetResizable(True)
        self.graphWidgets = []
        # self._layout.addWidget(self.menubar)
        self._layout.addWidget(self.graphsArea)
        self.setLayout(self._layout)

        for graph in self.data.children:
            self.addPlotWidget(graph = graph)

        if len(self.data.children) == 0:
            self.addPlotWidget()

    def mooseIsInstance(self,element, classNames):
        return moose.element(element).__class__.__name__ in classNames


    def findModelPath(self,element):
        child = element
        while not self.mooseIsInstance(element, "Shell"):
            child = moose.element(element).path
            element = moose.element(element).parent
        return child
        
    def deleteWidget(self, graphWidget):
        # print("Deleted => ", graphWidget)
        self.graphWidgets.remove(graphWidget)
        graphWidget.setParent(None)
        graphWidget.close()

    def createMenuBar(self):
        bar = sidebar.sidebar()
        bar.addAction(sidebar.add_graph_action(bar, lambda event: self.addPlotWidget() ))
        # bar.addAction(sidebar.delete_graph_action(bar, lambda event: self.addPlotWidget() ))
        # bar.addAction(sidebar.list_action(bar, self.showPlotView))
        return bar

    def addPlotWidget(self, row = None, col = 0, graph = None):
        if graph == None:
            graph = moose.Neutral(self.data.path + "/graph_" + str(self.rowIndex))
        widget = default.PlotWidget(self.model, graph, self.rowIndex, self)

        if self.modelType == ELECTRICAL:
            for axes in widget.canvas.axes.values():
            # axes.autoscale(False, axis='x', tight=True)
                axes.set_ylim(bottom = -0.07, top= 0.03)

        if row == None:
            row = self.rowIndex
        self.graphs.addWidget(widget)
        self.rowIndex += 1
        self.graphWidgets.append(widget)
        widget.widgetClosedSignal.connect(self.deleteWidget)
        widget.addGraph.connect(lambda event : self.addPlotWidget())
        # widget.resize(1, 1);
        return widget

    def showPlotView(self):
        pass

    def setModelRoot(self, *args):
        pass

    def getMenus(self, *args):
        return []

    def setDataRoot(self, *args):
        pass

    def updatePlots(self):
        for graphWidget in self.graphWidgets:
            graphWidget.updatePlots()

    def rescalePlots(self):
        for graphWidget in self.graphWidgets:
            graphWidget.rescalePlots()

    def extendXAxes(self, xlim):
        for graphWidget in self.graphWidgets:
            graphWidget.extendXAxes(xlim)

    def plotAllData(self):
        for graphWidget in self.graphWidgets:
            graphWidget.plotAllData()

    #print(graphWidget)
    # def plotAll(self):
    #     self.apply(lambda obj: obj.plotAll())

    # def plotAllData(self):
    #     selt.plotWidgetContainer.plotAllData()

    # def genColorMap(self,tableObject):
    #     pass

    # def onclick(self,event1):
    #     pass

    # def addTimeSeries(self, table, *args, **kwargs):
    #     pass

    # def addRasterPlot(self, eventtable, yoffset=0, *args, **kwargs):
    #     pass

    # def extendXAxes(self, xlim):
    #     pass

    # def saveCsv(self, line,directory):
    #     pass

    # def saveAllCsv(self):
    #     pass
