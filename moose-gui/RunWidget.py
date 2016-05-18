#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function

"""Sidebar for plugins. The sidebar comprises of actions.
Currently mode, connect and settings are defined.
"""

__author__      =   "Aviral Goel"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "Aviral Goel"
__email__       =   "goel.aviral@gmail.com"
__status__      =   "Development"


import sys
import os
import SettingsDialog
from PyQt4 import QtGui, Qt
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QScrollArea
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QSplitter

class RunWidget(QSplitter):

    def __init__(self, modelRoot, *args, **kwargs):
        super(RunWidget, self).__init__(None)
        self.modelRoot  = modelRoot
        layout          = QGridLayout()
        self.setLayout(layout)
        self.plotWidgetContainer = None
    def setChildWidget(self, widget, wrap, row, col, rowspan = 1, colspan = 1):
        if wrap:
            scrollArea = QScrollArea()
            scrollArea.setWidget(widget)
            scrollArea.setWidgetResizable(True);
            self.layout().addWidget(scrollArea, row, col, rowspan, colspan)
        else:
            self.addWidget(widget)
            # self.layout().addWidget(widget, row, col, rowspan, colspan)

    def setPlotWidgetContainer(self, widget):
        self.plotWidgetContainer = widget

    def setModelRoot(self, *args):
        pass

    def getMenus(self, *args):
        return []

    def setDataRoot(self, *args):
        pass

    def updatePlots(self):
        self.plotWidgetContainer.updatePlots()

    def rescalePlots(self):
        self.plotWidgetContainer.rescalePlots()

    def extendXAxes(self, xlim):
        self.plotWidgetContainer.extendXAxes(xlim)

    def plotAllData(self):
        self.plotWidgetContainer.plotAllData()
