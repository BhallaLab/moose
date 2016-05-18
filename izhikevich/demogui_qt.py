# demo_gui.py --- 
# 
# Filename: demo_gui.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Jun 16 05:41:58 2010 (+0530)
# Version: 
# Last-Updated: Tue Sep 11 14:26:13 2012 (+0530)
#           By: subha
#     Update #: 318
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

from PyQt4 import QtGui, QtCore
from PyQt4.Qt import Qt
import PyQt4.Qwt5 as Qwt
import numpy
from Izhikevich import IzhikevichDemo


class IzhikevichGui(QtGui.QMainWindow):
    """This is a Qt version of the GUI"""
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.demo = IzhikevichDemo()
        self.signalMapper = QtCore.QSignalMapper(self)
        self.demoFrame = QtGui.QFrame(self)
        self.controlPanel = QtGui.QFrame(self.demoFrame)
        self.figureNo = {}
        self.buttons = {}
        for key, value in IzhikevichDemo.parameters.items():
            button = QtGui.QPushButton(key, self.controlPanel)
            self.figureNo[value[0]] = key
            self.buttons[key] = button
        keys = self.figureNo.keys()
        keys.sort()
        length = len(keys)
        rows = int(numpy.rint(numpy.sqrt(length)))
        cols = int(numpy.ceil(length * 1.0 / rows))
        layout = QtGui.QGridLayout()
        for ii in range(rows):
            for jj in range(cols):
                index = ii * cols + jj
                if  index < length:
                    key = self.figureNo[keys[index]]
                    button = self.buttons[key]
                    button.setToolTip(self.tr(IzhikevichDemo.documentation[key]))
                    layout.addWidget(button, ii, jj)
                    self.connect(button, QtCore.SIGNAL('clicked()'), self.signalMapper, QtCore.SLOT('map()'))
                    self.signalMapper.setMapping(button, key)

        self.connect(self.signalMapper, QtCore.SIGNAL('mapped(const QString &)'), self._simulateAndPlot)         
        self.controlPanel.setLayout(layout)
        self.plotPanel = QtGui.QFrame(self.demoFrame)
        self.VmPlot = Qwt.QwtPlot(self.plotPanel)
        self.VmPlot.setAxisTitle(Qwt.QwtPlot.xBottom, 'time (ms)')
        self.VmPlot.setAxisTitle(Qwt.QwtPlot.yLeft, 'Vm (mV)')
        self.VmPlot.replot()
        self.ImPlot = Qwt.QwtPlot(self.plotPanel)
        self.ImPlot.setAxisTitle(Qwt.QwtPlot.xBottom, 'time (ms)')
        self.ImPlot.setAxisTitle(Qwt.QwtPlot.yLeft, 'Im (nA)')
        self.vmPlotZoomer = self._make_zoomer(self.VmPlot)
        self.imPlotZoomer = self._make_zoomer(self.ImPlot)
        self.descriptionWidget = QtGui.QLabel('Click any of the buttons to simulate and plot the corresponding neuron.')
        self.descriptionWidget.setFrameStyle(QtGui.QFrame.Panel | QtGui.QFrame.Sunken)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self.descriptionWidget.setSizePolicy(sizePolicy)
        self.VmPlot.setSizePolicy(sizePolicy)
        self.ImPlot.setSizePolicy(sizePolicy)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.VmPlot)
        layout.addWidget(self.ImPlot)
        layout.addWidget(self.descriptionWidget)
        self.plotPanel.setLayout(layout)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.plotPanel)
        layout.addWidget(self.controlPanel)
        self.demoFrame.setLayout(layout)
        self.setCentralWidget(self.demoFrame)

    def _simulateAndPlot(self, key):
        key = str(key)
        equationText = self.demo.getEquation(key).replace('\n', '<br/>')
        doc = IzhikevichDemo.documentation[key].replace('\n', '<br/>')
        text = '<b>%s:</b> %s<p><b>Equation:</b><br/> %s' % (key, doc, equationText)
        self.descriptionWidget.setText(self.tr(text))
#         if key == 'accommodation':
#             mbox = QtGui.QMessageBox(self)
#             mbox.setText(self.tr('Accommodation cannot be shown with regular Izhikevich model.'))
#             mbox.setDetailedText(self.tr('\
# Equation for u for the accommodating neuron is: \
# u\' = a * b * (V + 65)\n Which is different from \
# the regular equation u\' = a * (b*V - u) and cannot \
# be obtained from the latter by any choice of a and b.'))
#             mbox.show()
#             return
        (time, Vm, Im) = self.demo.simulate(key)
        Vm = numpy.array(Vm.vector) * 1e3
        Im = numpy.array(Im.vector) * 1e9
        self.VmPlot.clear()
        self.ImPlot.clear()
        curve = Qwt.QwtPlotCurve(self.tr(key + '_Vm'))
        curve.setPen(QtCore.Qt.red)
        curve.setData(time, numpy.array(Vm))
        curve.attach(self.VmPlot)
        curve = Qwt.QwtPlotCurve(self.tr(key + '_Im'))
        curve.setPen(QtCore.Qt.blue)
        curve.setData(time, Im)
        curve.attach(self.ImPlot)
        self.imPlotZoomer.setZoomBase()
        self.vmPlotZoomer.setZoomBase()
        self.ImPlot.replot()
        self.VmPlot.replot()
        

    def _make_zoomer(self, plot):
        zoomer = Qwt.QwtPlotZoomer(Qwt.QwtPlot.xBottom,
                                   Qwt.QwtPlot.yLeft,
                                   Qwt.QwtPicker.DragSelection,
                                   Qwt.QwtPicker.AlwaysOn,
                                   plot.canvas())
        zoomer.setRubberBandPen(QtGui.QPen(QtCore.Qt.white))
        zoomer.setTrackerPen(QtGui.QPen(QtCore.Qt.cyan))
        return zoomer
        

import sys
if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    mainWin = IzhikevichGui()
    mainWin.show()
    sys.exit(app.exec_())

# 
# demo_gui.py ends here
