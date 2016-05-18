# mplot.py ---
#
# Filename: mplot.py
# Description:
# Author:
# Maintainer:
# Created: Mon Mar 11 20:24:26 2013 (+0530)
# Version:
# Last-Updated: Wed Jul  3 10:32:35 2013 (+0530)
#           By: subha
#     Update #: 309
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Moose plot widget default implementation. This should be rich enough
# to suffice for most purposes.
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
"""
    *TODO*

    1) Option for default colors, markers, etc.

    2) Option for configuring number of rows and columns of
    subplots. (I think matplotlib grids will be a bit too much to
    implement). Problem is this has to be done before actual axes are
    created (as far as I know). Idea: can we do something like movable
    widgets example in Qt?

    3) Option for selecting any line or set of lines and change its
    configuration (as in dataviz).

    4) Association between plots and the data source.

    5) Lots and lots of scipy/numpy/scikits/statsmodels utilities can be added. To
    start with, we should have
      a)digital filters
      b) fft
      c) curve fitting

    6) For (5), think of another layer of plugins. Think of this as a
    standalone program. All these facilities should again be
    pluggable. We do not want to overwhelm novice users with fancy
    machine-learning stuff. They should be made available only on
    request.
        - There is a proposal for data analysis library by Andrew Davison ...

"""


__author__ = "Subhasis Ray"
import sys
import numpy as np
from PyQt4 import QtGui, QtCore
from PyQt4.Qt import Qt
from matplotlib import mlab
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
#from moose import utils
import moose
from PyQt4.QtCore import *

class CanvasWidget(FigureCanvas):
    """Widget to draw plots on.

    This class keep track of all the axes in a dictionary. The key for
    an axis is its index number in sequence of creation.

    next_id: The key for the next axis.

    current_id: Key for current axis (anu plotting will happen on
    this).

    """
    updateSignal = pyqtSignal()

    def __init__(self, model, graph, index, *args, **kwargs):
        self.model = model
        self.graph = graph
        self.index = index
        # QColor(243, 239, 238, 255)
        self.figure = Figure(facecolor = '#F3EFEE')#figsize=(1,1))
        FigureCanvas.__init__(self, self.figure, *args, **kwargs)
        self.figure.set_canvas(self)
        # self.set_xlabel('Time (s)')
        # self.set_ylabel('Concentration (mM)')
        if len(args) > 0 and isinstance(args[0], QtGui.QWidget):
            self.reparent(args[0])
        elif (kwargs is not None) and ('parent' in kwargs):
            self.reparent(kwargs['parent'])
        #self.setAcceptDrops(True)
        # self.setMaximumSize(100, 100)
        FigureCanvas.updateGeometry(self)
        self.axes = {}
        self.next_id = 0
        self.current_id = -1
        tabList = []
        self.addTabletoPlot = ''
        self.setAcceptDrops(True)
        self.gridMode = False

    def dragEnterEvent(self, event):
        if event.mimeData().hasFormat('text/plain'):
            event.acceptProposedAction()

    def dragMoveEvent(self, event):
        if event.mimeData().hasFormat('text/plain'):
            event.acceptProposedAction()

    def eventFilter(self, source, event):
        if (event.type() == QtCore.QEvent.Drop):
            pass

    def dropEvent(self, event):
        """Insert an element of the specified class in drop location"""

        if not event.mimeData().hasFormat('text/plain'):
            return
        # print " active window ", self.isActiveWindow()
        # print "Mouse : ", self.mouse

        # pos = self.mapFromGlobal(QCursor.pos())
        # print "Mouse Position : ", pos
        modelRoot, element = event.mimeData().data
        if isinstance (element,moose.PoolBase):
            tablePath = moose.utils.create_table_path(self.model, self.graph, element, "Conc")
            table     = moose.utils.create_table(tablePath, element, "Conc","Table2")
            # moose.connect(table, 'requestOut', element, 'getConc')
            self.updateSignal.emit()
        elif isinstance(element, moose.CompartmentBase):
            tablePath = moose.utils.create_table_path(self.model, self.graph, element, "Vm")
            table     = moose.utils.create_table(tablePath, element, "Vm","Table")
            self.updateSignal.emit()
        else:
            QtGui.QMessageBox.question(self, 'Message',"This element's properties cannot be plotted.", QtGui.QMessageBox.Ok)

    def addSubplot(self, rows, cols):
        """Add a subplot to figure and set it as current axes."""
        assert(self.next_id <= rows * cols)
        axes = self.figure.add_subplot(rows, cols, self.next_id+1)
        axes.set_xlabel("Time (s)")
        axes.set_ylabel("Concentration (mM)")
        axes.set_xlim(left=0.0)
        axes.set_ylim(bottom=0.0)
        self.axes[self.next_id] = axes
        axes.set_title("Graph " + str(self.index + 1))
        self.current_id = self.next_id
        self.next_id += 1
        labelList = []
        axes.legend(loc='upper center')
        return axes

    def plot(self, *args, **kwargs):
        #self.callAxesFn('legend',loc='lower center',bbox_to_anchor=(0.5, -0.03),fancybox=True, shadow=True, ncol=3)
        return self.callAxesFn('plot', *args, **kwargs)

    def callAxesFn(self, fname, *args, **kwargs):
        """Call any arbitrary function of current axes object."""
        if self.current_id < 0:
            self.addSubplot(1,1)
        fn = eval('self.axes[self.current_id].%s' % (fname))

        return fn(*args, **kwargs)

    def resize_event(self, event):
        print("Resize event called ", event)

    def toggleGrid(self):
        self.gridMode = not self.gridMode
        for key in self.axes:
            self.axes[key].grid(self.gridMode)
        self.draw()

    def setXLimit(self, minX, maxX):
        for key in self.axes:
            self.axes[key].set_xlim([minX, maxX])
        self.draw()


import sys
import os
import config
import unittest

from PyQt4.QtTest import QTest

class CanvasWidgetTests(unittest.TestCase):
    def setUp(self):
        self.app = QtGui.QApplication([])
        QtGui.qApp = self.app
        icon = QtGui.QIcon(os.path.join(config.KEY_ICON_DIR,'moose_icon.png'))
        self.app.setWindowIcon(icon)
        self.window = QtGui.QMainWindow()
        self.cwidget = CanvasWidget()
        self.window.setCentralWidget(self.cwidget)
        self.window.show()

    def testPlot(self):
        """Test plot function"""
        self.cwidget.addSubplot(1,1)
        self.cwidget.plot(np.arange(1000), mlab.normpdf(np.arange(1000), 500, 150))

    def testCallAxesFn(self):
        self.cwidget.addSubplot(1,1)
        self.cwidget.callAxesFn('scatter', np.random.randint(0, 100, 100), np.random.randint(0, 100,100))

    def tearDown(self):
        self.app.exec_()

if __name__ == '__main__':
    unittest.main()

#
# mplot.py ends here
