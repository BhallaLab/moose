# gui.py --- 
# 
# Filename: gui.py
# Description: 
# Author: 
# Maintainer: 
# Created: Fri Jul 12 11:53:50 2013 (+0530)
# Version: 
# Last-Updated: Thu Aug 11 17:21:24 2016 (-0400)
#           By: Subhasis Ray
#     Update #: 781
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

"""
Display channel properties graphically
"""
from __future__ import print_function
from datetime import datetime
from PyQt4 import QtCore, QtGui
from matplotlib import mlab
# in stead of "from matplotlib.figure import Figure" do -
# from matplotlib.pyplot import figure as Figure
# see: https://groups.google.com/forum/#!msg/networkx-discuss/lTVyrmFoURQ/SZNnTY1bSf8J
# but does not help after the first instance when displaying cell
from matplotlib.figure import Figure
from matplotlib import patches
from pylab import cm

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar

import networkx as nx
import numpy as np
import sys
from cells import *
from cell_test_util import setup_current_step_model
import matplotlib.gridspec as gridspec
import moose
from moose import utils as mutils

cmap = cm.jet

simdt = 0.025e-3
plotdt = 1e-4

class HHChanView(QtGui.QWidget):
    def __init__(self, *args, **kwargs):
        QtGui.QWidget.__init__(self, *args, **kwargs)
        self.channels = {}
        self.setLayout(QtGui.QHBoxLayout())
        self.layout().addWidget(self.getControlPanel())
        self.layout().addWidget(self.plotActInact())        

    def getControlPanel(self):
        try:
            return self.controlPanel
        except AttributeError:
            self.controlPanel = QtGui.QWidget()
            layout = QtGui.QVBoxLayout()
            self.controlPanel.setLayout(layout)
            self.rootEdit = QtGui.QLineEdit('/library')        
            self.rootEdit.returnPressed.connect(self.getUpdatedChannelListWidget)
            self.plotButton = QtGui.QPushButton('Plot selected channels')
            self.plotButton.clicked.connect(self.plotActInact)
            layout.addWidget(self.getUpdatedChannelListWidget())
            layout.addWidget(self.rootEdit)
            layout.addWidget(self.plotButton)
            return self.controlPanel

    def getChannels(self, root='/library'):
        root = moose.element(root)
        for channel in moose.wildcardFind('%s/#[ISA=HHChannel]' % (root.path)):
            self.channels[channel.name] = channel
        return self.channels
        
    def getUpdatedChannelListWidget(self):
        try:
            self.channelListWidget.clear()
            self.channels = {}
        except AttributeError:            
            self.channelListWidget = QtGui.QListWidget(self)
            self.channelListWidget.setSelectionMode( QtGui.QAbstractItemView.ExtendedSelection)
        root = str(self.rootEdit.text())
        for chan in self.getChannels(root).values():
            self.channelListWidget.addItem(chan.name)
        self.update()
        return self.channelListWidget

    def getChannelListWidget(self):
        try:
            return self.channelListWidget
        except AttributeError:
            return self.getUpdatedChannelListWidget()

    def __plotGate(self, gate, mlabel='', taulabel=''):
        gate = moose.element(gate)
        a = np.asarray(gate.tableA)
        b = np.asarray(gate.tableB)
        m = a/(a+b)
        tau = 1/(a+b)
        v = np.linspace(gate.min, gate.max, len(m))
        self.mhaxes.plot(v, m, label='%s %s' % (gate.path, mlabel))
        self.tauaxes.plot(v, tau, label='%s %s' % (gate.path, taulabel))
        print('Plotted', gate.path, 'vmin=', gate.min, 'vmax=', gate.max, 'm[0]=', m[0], 'm[last]=', m[-1], 'tau[0]=', tau[0], 'tau[last]=', tau[-1])
        
    def plotActInact(self):
        """Plot the activation and inactivation variables of the selected channels"""
        try:
            self.figure.clear()
        except AttributeError:
            self.figure = Figure()
            self.canvas = FigureCanvas(self.figure)
            self.nav = NavigationToolbar(self.canvas, self)
            self.plotWidget = QtGui.QWidget()
            layout = QtGui.QVBoxLayout()
            self.plotWidget.setLayout(layout)
            layout.addWidget(self.canvas)
            layout.addWidget(self.nav)
        self.mhaxes = self.figure.add_subplot(2, 1, 1)
        self.mhaxes.set_title('Activation/Inactivation')
        self.tauaxes = self.figure.add_subplot(2, 1, 2)
        self.tauaxes.set_title('Tau')
        for item in self.getChannelListWidget().selectedItems():
            chan = self.channels[str(item.text())]
            if chan.Xpower > 0:
                path = '{}/gateX'.format(chan.path)
                self.__plotGate(path)
            if chan.Ypower > 0:
                path = '{}/gateY'.format(chan.path)
                self.__plotGate(path)
            # if chan.Zpower > 0:
            #     self.__plotGate(chan.gateZ.path, mlabel='z', taulabel='tau-z')
        self.mhaxes.legend()
        self.tauaxes.legend()
        self.canvas.draw()
        return self.plotWidget


from display_morphology import *

class NetworkXWidget(QtGui.QWidget):
    def __init__(self, *args, **kwargs):
        QtGui.QWidget.__init__(self, *args, **kwargs)
        layout = QtGui.QVBoxLayout()
        self.setLayout(layout)
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.nav = NavigationToolbar(self.canvas, self)
        layout.addWidget(self.canvas)
        layout.addWidget(self.nav)
        self.axes = self.figure.add_subplot(1, 1, 1)
        self.axes.set_frame_on(False)
        self.axes.get_xaxis().set_visible(False)
        self.axes.get_yaxis().set_visible(False)

    # TODO: bypass networkx draw as it uses pylab directly.
    def displayGraph(self, g, label=False):
        axon, sd = axon_dendrites(g)
        sizes = node_sizes(g) * 50
        if len(sizes) == 0:
            print('Empty graph for cell. Make sure proto file has `*asymmetric` on top. I cannot handle symmetric compartmental connections')
            return
        node_colors = ['k' if x in axon else 'gray' for x in g.nodes()]
        lw = [1 if n.endswith('comp_1') else 0 for n in g.nodes()]
        self.axes.clear()
        try:
            nx.draw_graphviz(g, ax=self.axes, prog='twopi', node_color=node_colors, lw=lw)
        except (NameError, AttributeError) as e:
            nx.draw_spectral(g, ax=self.axes, node_color=node_colors, lw=lw, with_labels=False, )


class CellView(QtGui.QWidget):
    def __init__(self, *args, **kwargs):
        QtGui.QWidget.__init__(self, *args, **kwargs)
        self.cells = {}
        layout = QtGui.QGridLayout()
        self.setLayout(layout)
        layout.addWidget(self.getControlPanel(), 0, 0, 2, 1)
        layout.addWidget(self.getCellMorphologyWidget(), 0, 1, 1, 1)
        layout.addWidget(self.getPlotWidget(), 1, 1, 1, 1)

    def getControlPanel(self):
        try:
            return self.controlPanel
        except AttributeError:
            self.controlPanel = QtGui.QWidget()
            layout = QtGui.QGridLayout()
            self.controlPanel.setLayout(layout)
            self.simtimeLabel = QtGui.QLabel('Simulate for (ms)')
            self.simtimeEdit = QtGui.QLineEdit('100')
            self.plotButton = QtGui.QPushButton('Simulate selected cell model')
            self.plotButton.clicked.connect(self.simulateSelected)
            layout.addWidget(self.getCellListWidget(), 0, 0, 1, 2)
            layout.addWidget(self.getCurrentClampWidget(), 1, 0, 1, 2)
            layout.addWidget(self.simtimeLabel, 2, 0, 1, 1)
            layout.addWidget(self.simtimeEdit, 2, 1, 1, 1)
            layout.addWidget(self.plotButton, 3, 0, 1, 1)
            return self.controlPanel

    def getCells(self, root='/library'):
        if isinstance(root, str):
            root = moose.element(root)
        cells = []
        for cell in moose.wildcardFind('%s/#[ISA=Neuron]' % (root.path)):
            cells.append(cell.name)
        return cells
        
    def getUpdatedCellListWidget(self):
        try:
            self.cellListWidget.clear()
            self.cells = {}
        except AttributeError:            
            self.cellListWidget = QtGui.QListWidget(self)
            self.cellListWidget.itemSelectionChanged.connect(self.displaySelected)
            # self.cellListWidget.setSelectionMode( QtGui.QAbstractItemView.ExtendedSelection)
        # root = str(self.rootEdit.text())
        for cell in self.getCells():
            self.cellListWidget.addItem(cell)     
        self.cellListWidget.setCurrentItem(self.cellListWidget.item(0))
        self.update()
        return self.cellListWidget

    def getCellListWidget(self):
        try:
            return self.cellListWidget
        except AttributeError:
            return self.getUpdatedCellListWidget()

    def getCurrentClampWidget(self):
        try:
            return self.currentClampWidget
        except AttributeError:
            self.currentClampWidget = QtGui.QWidget()
            self.delayLabel = QtGui.QLabel('Delay (ms)')
            self.delayText = QtGui.QLineEdit('20')
            self.widthLabel = QtGui.QLabel('Duration (ms)')
            self.widthText = QtGui.QLineEdit('50')
            self.ampMinLabel = QtGui.QLabel('Starting amplitude (pA)')
            self.ampMinText = QtGui.QLineEdit('0')
            self.ampMaxLabel = QtGui.QLabel('Maximum amplitude (pA)')
            self.ampMaxText = QtGui.QLineEdit('100')
            self.ampStepLabel = QtGui.QLabel('Amplitude steps (pA)')
            self.ampStepText = QtGui.QLineEdit('10')
            title = QtGui.QLabel('Current pulse')
            layout = QtGui.QGridLayout()
            layout.addWidget(title, 0, 0, 1, 2)
            layout.addWidget(self.delayLabel, 1, 0, 1, 1)
            layout.addWidget(self.delayText, 1, 1, 1, 1)
            layout.addWidget(self.widthLabel, 2, 0, 1, 1)
            layout.addWidget(self.widthText, 2, 1, 1, 1)
            layout.addWidget(self.ampMinLabel, 3, 0, 1, 1)
            layout.addWidget(self.ampMinText, 3, 1, 1, 1)
            layout.addWidget(self.ampMaxLabel, 4, 0, 1, 1)
            layout.addWidget(self.ampMaxText, 4, 1, 1, 1)
            layout.addWidget(self.ampStepLabel, 5, 0, 1, 1)
            layout.addWidget(self.ampStepText, 5, 1, 1, 1)
            self.currentClampWidget.setLayout(layout)
        return self.currentClampWidget

    def getCellMorphologyWidget(self):
        try:
            return self.cellMorphologyWidget
        except AttributeError:
            self.cellMorphologyWidget = NetworkXWidget()
            return self.cellMorphologyWidget

    def displayCellMorphology(self, cellpath):
        cell = moose.element(cellpath)
        graph = cell_to_graph(cell)
        self.getCellMorphologyWidget().displayGraph(graph)

    def createCell(self, name):
        model_container = moose.Neutral('/model')
        data_container = moose.Neutral('/data')        
        # moose.le(model_container)
        # moose.le(data_container)
        for ch in model_container.children:            
            moose.delete(ch)
        for ch in data_container.children:
            moose.delete(ch)
        params = setup_current_step_model(model_container,
                                          data_container,
                                          name,
                                          [[0, 0, 0],
                                           [1e9, 0, 0]])
        # moose.le(model_container)
        # moose.le(data_container)
        params['modelRoot'] = model_container.path
        params['dataRoot'] = data_container.path
        return params

    def displaySelected(self):
        cellnames = [str(c.text()) for c in self.cellListWidget.selectedItems()]
        assert(len(cellnames) == 1)        
        name = cellnames[0]
        # params = self.createCell(name)
        # cell = params['cell']
        protopath = '/library/%s' % (name)
        self.displayCellMorphology(protopath)

    def simulateSelected(self):
        cellnames = [str(c.text()) for c in self.cellListWidget.selectedItems()]
        assert(len(cellnames) == 1)        
        name = cellnames[0]
        params = self.createCell(name)
        # hsolve = moose.HSolve('%s/solver' % (params['cell'].path))
        # hsolve.dt = simdt
        # hsolve.target = params['cell'].path
        mutils.setDefaultDt(elecdt=simdt, plotdt2=plotdt)
        mutils.assignDefaultTicks(modelRoot=params['modelRoot'],
                                  dataRoot=params['dataRoot'],
                                  solver='hsolve')
        delay = float(str(self.delayText.text())) * 1e-3
        width =  float(str(self.widthText.text())) * 1e-3
        levelMin =  float(str(self.ampMinText.text())) * 1e-12
        levelMax =  float(str(self.ampMaxText.text())) * 1e-12
        levelStep =  float(str(self.ampStepText.text())) * 1e-12
        params['stimulus'].delay[0] = delay 
        params['stimulus'].width[0] = width 
        params['stimulus'].level[0] = levelMin
        simtime = float(str(self.simtimeEdit.text()))*1e-3
        tdlist = []
        self.vmAxes.clear()       
        self.vmAxes.set_title('membrane potential at soma')
        self.vmAxes.set_ylabel('mV')            
        self.vmAxes.get_xaxis().set_visible(False)
        self.stimAxes.clear()
        self.stimAxes.set_title('current injected at soma')
        self.stimAxes.set_ylabel('pA')
        self.stimAxes.set_xlabel('ms')
        styles = ['-', '--', '-.', ':']
        cnt = int((levelMax - levelMin)/levelStep)
        ii = 0
        while params['stimulus'].level[0] < levelMax:
            tstart = datetime.now()
            moose.reinit()
            moose.start(simtime)
            tend = datetime.now()
            td = tend - tstart
            tdlist.append(td.days * 86400 + td.seconds + td.microseconds * 1e-6)
            ts = np.linspace(0, simtime, len(params['somaVm'].vector))
            vm = params['somaVm'].vector
            stim = params['injectionCurrent'].vector
            alpha = 0.1 + 0.9 * params['stimulus'].level[0] / levelMax
            color = cmap(ii*1.0/cnt, cnt)
            self.vmAxes.plot(ts * 1e3, vm * 1e3, color=color, label='%g pA' % (params['stimulus'].level[0]*1e12), alpha=0.8)
            self.stimAxes.plot(ts * 1e3, stim * 1e12, color=color, label='Current (pA)')
            self.plotCanvas.draw()
            params['stimulus'].level[0] += levelStep
            ii += 1
        self.gs.tight_layout(self.plotFigure)
        self.vmAxes.legend()
        self.plotCanvas.draw()
        td = np.mean(tdlist)
        print('Simulating %g s took %g s of computer time' % (simtime, td))
        # self.plotFigure.tight_layout()

    def getPlotWidget(self):
        try:
            return self.plotWidget
        except AttributeError:
            self.plotWidget = QtGui.QWidget()
            layout = QtGui.QVBoxLayout()
            self.plotFigure = Figure()
            self.plotCanvas = FigureCanvas(self.plotFigure)
            self.nav = NavigationToolbar(self.plotCanvas, self)
            self.gs = gridspec.GridSpec(3, 1)
            self.vmAxes = self.plotFigure.add_subplot(self.gs[:-1])
            self.vmAxes.set_frame_on(False)
            self.stimAxes = self.plotFigure.add_subplot(self.gs[-1], sharex=self.vmAxes)
            self.stimAxes.set_frame_on(False)
            self.gs.tight_layout(self.plotFigure)
            layout.addWidget(self.plotCanvas)
            layout.addWidget(self.nav)
            self.plotWidget.setLayout(layout)
        return self.plotWidget
                                              


if __name__ == '__main__':
    app = QtGui.QApplication([])
    win1 = HHChanView()
    win = CellView()
    win1.show()
    win.show()
    sys.exit(app.exec_())


# 
# gui.py ends here
