
# -*- coding: utf-8 -*-

"""
NeuroKit plugin for MOOSE.
"""

__author__      =   "Aviral Goel"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "Aviral Goel"
__email__       =   "goel.aviral@gmail.com"
__status__      =   "Development"

# import sys
# import config
# import pickle
# import os
# from collections import defaultdict
# import numpy as np
# import re
# from PyQt4 import QtGui, QtCore
# from PyQt4.Qt import Qt
# from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar

# import moose
# from moose import utils
# import mtree
# from mtoolbutton import MToolButton
# from msearch import SearchWidget
# from checkcombobox import CheckComboBox
import sys
import default
import NeuroKitEditor
import NeuroKitRunner
import mplugin
import moose
from PyQt4.QtGui import QWidget
# import NeuroKitPlotter
# import NeuroKitRunner

#from defaultToolPanel import DefaultToolPanel


class NeuroKit(mplugin.MoosePluginBase):

    """Neurokit plugin for MOOSE GUI"""

    def __init__(self, root, mainwindow):
        super(NeuroKit, self).__init__(root, mainwindow)
        self._editor    = NeuroKitEditor.NeuroKitEditor(self, root)
        # self._plotter   = NeuroKitPlotter.NeuroKitPlotter(self)
        self._runner    = NeuroKitRunner.NeuroKitRunner(root, self)
        self._current   = self._editor
        self._views     = [ self._editor
                          , self._runner
                          # , self._plotter
                          ]
        self._menus     = []

    def getPreviousPlugin(self):
        return None

    def getNextPlugin(self):
        return None

    def getAdjacentPlugins(self):
        return []

    def getViews(self):
        return self._views

    def getCurrentView(self):
        return self.currentView

    def getEditorView(self):
        return  self._editor

    def getPlotView(self):
        return self._plotter

    def getRunView(self):
        return self._runner

    def getMenus(self):
        """Create a custom set of menus."""
        return self._menus
