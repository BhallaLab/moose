# test_plotwidget.py --- 
# 
# Filename: test_plotwidget.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue Mar 12 12:17:23 2013 (+0530)
# Version: 
# Last-Updated: Tue Mar 12 12:26:42 2013 (+0530)
#           By: subha
#     Update #: 31
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:
import sys
import os
import numpy as np
from PyQt4 import QtGui, QtCore
from PyQt4.Qt import Qt
from matplotlib import mlab
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
import unittest
sys.path.append('..')
from default import PlotWidget
import moose
import config

class PlotWidgetTests(unittest.TestCase):
    def setUp(self):
        self.app = QtGui.QApplication([])
        QtGui.qApp = self.app
        icon = QtGui.QIcon(os.path.join(config.KEY_ICON_DIR,'moose_icon.png'))
        self.model = moose.loadModel('../../Demos/Genesis_files/acc68.g', '/acc68')        
        self.app.setWindowIcon(icon)
        self.window = QtGui.QMainWindow()
        self.window.setWindowTitle('Test plot widget')
        self.mdiArea = QtGui.QMdiArea()
        self.window.setCentralWidget(self.mdiArea)
        self.widget = PlotWidget()
        self.widget.setDataRoot(self.model.path)
        self.mdiArea.addSubWindow(self.widget)
        self.window.show()

    def testPlotAllData(self):
        """Test plot function"""
        self.widget.plotAllData()

    def tearDown(self):
        self.app.exec_()
    
if __name__ == '__main__':
    unittest.main()



# 
# test_plotwidget.py ends here
