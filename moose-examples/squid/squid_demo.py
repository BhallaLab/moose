
 # -*- coding: utf-8 -*-
# squidgui.py --- 
# 
# Filename: squidgui.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Jul  9 18:23:55 2012 (+0530)
# Version: 
# Last-Updated: Tue May  3 00:18:47 2016 (-0400)
#           By: subha
#     Update #: 1078
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
sys.path.append('../../python')
import os
os.environ['NUMPTHREADS'] = '1'

from collections import defaultdict
import time

from PyQt4 import QtGui
from PyQt4 import QtCore
import numpy
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas, NavigationToolbar2QT as NavigationToolbar

import moose

from squid import *
from squid_setup import SquidSetup
from electronics import ClampCircuit


tooltip_Nernst = """<h3>Ionic equilibrium potential</h3>
<p/>
The equilibrium potential for ion C is given by Nernst equation:
<p>
E<sub>C</sub> = (RT/zF) * ln([C]<sub>out</sub> / [C]<sub>in</sub>)
</p>
where R is the ideal gas constant (8.3145 J/mol K),<br>
      T is absolute temperature,<br>
      z is the valence of the ion,<br>
      F is Faraday's constant 96480 C/mol,<br>
      [C]<sub>out</sub> is concentration of C outside the membrane,<br>
      [C]<sub>in</sub> is concentration of C inside the membrane."""

tooltip_Erest = """<h3>Resting membrane potential</h3>
<p/>
The resting membrane potential is determined by the ionic
concentrations inside and outside the cell membrane and is given by
the Goldman-Hodgkin-Katz equation:
<p>

V = (RT/F) * ln((P<sub>K</sub>[K<sup>+</sup>]<sub>out</sub> + P<sub>Na</sub>[Na<sup>+</sup>]<sub>out</sub> + P<sub>Cl</sub>[Cl<sup>-</sup>]<sub>in</sub>) / (P<sub>K</sub>[K<sup>+</sup>]in + P<sub>Na</sub>[Na<sup>+</sup>]<sub>in</sub> + P<sub>Cl</sub>[Cl<sup>-</sup>]<sub>out</sub>))

</p>
where P<sub>C</sub> is the permeability of the membrane to ion C.

"""

tooltip_NaChan = u"""<h3>Na+ channel conductance</h3>
<p/>
The Na<sup>+</sup> channel conductance in squid giant axon is given by:

<p> G<sub>Na</sub> = Ḡ<sub>Na</sub> * m<sup>3</sup> * h </p>

and the current through this channel is:
<p>
I<sub>Na</sub> = G<sub>Na</sub> * (V - E<sub>Na</sub>) = Ḡ<sub>Na</sub> * m<sup>3</sup> * h * (V - E<sub>Na</sub>)
</p>

where Ḡ<sub>Na</sub> is the peak conductance of Na<sup>+</sup> channel, m is
the fraction of activation gates open and h is the fraction of
deactivation gates open. The transition from open to closed state has
first order kinetics: 
<p> dm/dt = α<sub>m</sub> * ( 1 - m) - β<sub>m</sub> * m </p> 
and similarly for h.

The steady state values are:
<p> m<sub>∞</sub> = α<sub>m</sub>/(α<sub>m</sub> + β<sub>m</sub>) </p>
and time constant for steady state is:
<p>τ<sub>m</sub> = 1/ (α<sub>m</sub> + β<sub>m</sub>) </p>
and similarly for h.
"""

tooltip_KChan = u"""<h3>K+ channel conductance</h3>
<p/>The K+ channel conductance in squid giant axon is given by:

<p> G<sub>K</sub> = Ḡ<sub>K</sub> * n<sup>4</sup></p>

and the current through this channel is:
<p>
I<sub>K</sub> = G<sub>K</sub> * (V - E<sub>K</sub>) = Ḡ<sub>K</sub> * n<sup>4</sup> * (V - E<sub>K</sub>) 
</p>
where Ḡ<sub>K</sub> is the peak conductance of K<sup>+</sup> channel,
n is the fraction of activation gates open. The transition from open
to closed state has first order kinetics: <p> dn/dt = α<sub>n</sub> *
( 1 - n) - β<sub>n</sub> * n </p>.

The steady state values are:
<p>
n<sub>∞</sub> = α<sub>n</sub>/(α<sub>n</sub> + β<sub>n</sub>)
</p>
and time constant for steady state is:
<p>
τ<sub>n</sub> = 1/ (α<sub>n</sub> + β<sub>n</sub>)

</p>
and similarly for h.
"""

tooltip_Im = """<h3>Membrane current</h3>
<p/>
The current through the membrane is given by:
<p>
I<sub>m</sub> = C<sub>m</sub> dV/dt + I<sub>K</sub> + I<sub>Na</sub> + I<sub>L</sub>
</p><p>
 = C<sub>m</sub> dV/dt + G<sub>K</sub>(V, t) * (V - E<sub>K</sub>) + G<sub>Na</sub> * (V - E<sub>Na</sub>) + G<sub>L</sub> * (V - E<sub>L</sub>)
</p>
where G<sub>L</sub> is the leak current and E<sub>L</sub> is the leak reversal potential.

"""

default_line_edit_size = QtCore.QSize(80, 25)
def set_default_line_edit_size(widget):
    widget.setMinimumSize(default_line_edit_size)
    widget.setMaximumSize(default_line_edit_size)


class SquidGui(QtGui.QMainWindow):
    defaults = {}
    defaults.update(SquidAxon.defaults)
    defaults.update(ClampCircuit.defaults)
    defaults.update({'runtime': 50.0,
                  'simdt': 0.01,
                  'plotdt': 0.1,
                  'vclamp.holdingV': 0.0,
                  'vclamp.holdingT': 10.0,
                  'vclamp.prepulseV': 0.0,
                  'vclamp.prepulseT': 0.0,
                  'vclamp.clampV': 50.0,
                  'vclamp.clampT': 20.0,
                  'iclamp.baseI': 0.0,
                  'iclamp.firstI': 0.1,
                  'iclamp.firstT': 40.0,
                  'iclamp.firstD': 5.0,
                  'iclamp.secondI': 0.0,
                  'iclamp.secondT': 0.0,
                  'iclamp.secondD': 0.0
                  })
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.squid_setup = SquidSetup()
        self._plotdt = SquidGui.defaults['plotdt']
        self._plot_dict = defaultdict(list)
        self.setWindowTitle('Squid Axon simulation')        
        self.setDockNestingEnabled(True)
        self._createRunControl()
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self._runControlDock) 
        self._runControlDock.setFeatures(QtGui.QDockWidget.AllDockWidgetFeatures)	 
        self._createChannelControl()
        self._channelCtrlBox.setWindowTitle('Channel properties')
        self._channelControlDock.setFeatures(QtGui.QDockWidget.AllDockWidgetFeatures)	 
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self._channelControlDock) 
        self._createElectronicsControl()
        self._electronicsDock.setFeatures(QtGui.QDockWidget.AllDockWidgetFeatures)	 
        self._electronicsDock.setWindowTitle('Electronics')
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self._electronicsDock) 
        self._createPlotWidget()             
        self.setCentralWidget(self._plotWidget)
        self._createStatePlotWidget()
        self._createHelpMessage()
        self._helpWindow.setVisible(False)
        self._statePlotWidget.setWindowFlags(QtCore.Qt.Window)
        self._statePlotWidget.setWindowTitle('State plot')
        self._initActions()
        self._createRunToolBar()
        self._createPlotToolBar()

    def getFloatInput(self, widget, name):
        try:
            return float(str(widget.text()))
        except ValueError:
            QtGui.QMessageBox.critical(self, 'Invalid input', 'Please enter a valid number for {}'.format(name))
            raise

        
    def _createPlotWidget(self):
        self._plotWidget = QtGui.QWidget()
        self._plotFigure = Figure()
        self._plotCanvas = FigureCanvas(self._plotFigure)
        self._plotCanvas.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self._plotCanvas.updateGeometry()
        self._plotCanvas.setParent(self._plotWidget)
        self._plotCanvas.mpl_connect('scroll_event', self._onScroll)
        self._plotFigure.set_canvas(self._plotCanvas)
        # Vm and command voltage go in the same subplot
        self._vm_axes = self._plotFigure.add_subplot(2,2,1, title='Membrane potential')
        self._vm_axes.set_ylim(-20.0, 120.0)
        # Channel conductances go to the same subplot
        self._g_axes = self._plotFigure.add_subplot(2,2,2, title='Channel conductance')
        self._g_axes.set_ylim(0.0, 0.5)
        # Injection current for Vclamp/Iclamp go to the same subplot
        self._im_axes = self._plotFigure.add_subplot(2,2,3, title='Injection current')
        self._im_axes.set_ylim(-0.5, 0.5)
        # Channel currents go to the same subplot
        self._i_axes = self._plotFigure.add_subplot(2,2,4, title='Channel current')
        self._i_axes.set_ylim(-10, 10)
        for axis in self._plotFigure.axes:
            axis.set_autoscale_on(False)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(self._plotCanvas)
        self._plotNavigator = NavigationToolbar(self._plotCanvas, self._plotWidget)
        layout.addWidget(self._plotNavigator)
        self._plotWidget.setLayout(layout)

    def _createStatePlotWidget(self):        
        self._statePlotWidget = QtGui.QWidget()
        self._statePlotFigure = Figure()
        self._statePlotCanvas = FigureCanvas(self._statePlotFigure)
        self._statePlotCanvas.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self._statePlotCanvas.updateGeometry()
        self._statePlotCanvas.setParent(self._statePlotWidget)
        self._statePlotFigure.set_canvas(self._statePlotCanvas)
        self._statePlotFigure.subplots_adjust(hspace=0.5)
        self._statePlotAxes = self._statePlotFigure.add_subplot(2,1,1, title='State plot')
        self._state_plot, = self._statePlotAxes.plot([], [], label='state')
        self._activationParamAxes = self._statePlotFigure.add_subplot(2,1,2, title='H-H activation parameters vs time')
        self._activationParamAxes.set_xlabel('Time (ms)')
        #for axis in self._plotFigure.axes:
        #    axis.autoscale(False)
        self._stateplot_xvar_label = QtGui.QLabel('Variable on X-axis')
        self._stateplot_xvar_combo = QtGui.QComboBox()
        self._stateplot_xvar_combo.addItems(['V', 'm', 'n', 'h'])
        self._stateplot_xvar_combo.setCurrentIndex(0)
        self._stateplot_xvar_combo.setEditable(False)
        self.connect(self._stateplot_xvar_combo,
                     QtCore.SIGNAL('currentIndexChanged(const QString&)'),
                     self._statePlotXSlot)                     
        self._stateplot_yvar_label = QtGui.QLabel('Variable on Y-axis')
        self._stateplot_yvar_combo = QtGui.QComboBox()
        self._stateplot_yvar_combo.addItems(['V', 'm', 'n', 'h'])
        self._stateplot_yvar_combo.setCurrentIndex(2)
        self._stateplot_yvar_combo.setEditable(False)
        self.connect(self._stateplot_yvar_combo,
                     QtCore.SIGNAL('currentIndexChanged(const QString&)'),
                     self._statePlotYSlot)
        self._statePlotNavigator = NavigationToolbar(self._statePlotCanvas, self._statePlotWidget)
        frame = QtGui.QFrame()
        frame.setFrameStyle(QtGui.QFrame.StyledPanel + QtGui.QFrame.Raised)
        layout = QtGui.QHBoxLayout()
        layout.addWidget(self._stateplot_xvar_label)
        layout.addWidget(self._stateplot_xvar_combo)
        layout.addWidget(self._stateplot_yvar_label)
        layout.addWidget(self._stateplot_yvar_combo)
        frame.setLayout(layout)
        self._closeStatePlotAction = QtGui.QAction('Close', self)
        self.connect(self._closeStatePlotAction, QtCore.SIGNAL('triggered()'), self._statePlotWidget.close)
        self._closeStatePlotButton = QtGui.QToolButton()
        self._closeStatePlotButton.setDefaultAction(self._closeStatePlotAction)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(frame)
        layout.addWidget(self._statePlotCanvas)
        layout.addWidget(self._statePlotNavigator)
        layout.addWidget(self._closeStatePlotButton)
        self._statePlotWidget.setLayout(layout)  
        # Setting the close event so that when the help window is
        # closed the ``State plot`` button becomes unchecked
        self._statePlotWidget.closeEvent = lambda event: self._showStatePlotAction.setChecked(False)

    def _createRunControl(self):
        self._runControlBox = QtGui.QGroupBox(self)
        self._runControlBox.setSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        self._runTimeLabel = QtGui.QLabel("Run time (ms)", self._runControlBox)
        self._simTimeStepLabel = QtGui.QLabel("Simulation time step (ms)", self._runControlBox)
        self._runTimeEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['runtime']), self._runControlBox)
        set_default_line_edit_size(self._runTimeEdit)
        self._simTimeStepEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['simdt']), self._runControlBox)
        set_default_line_edit_size(self._simTimeStepEdit)
        layout = QtGui.QGridLayout()
        layout.addWidget(self._runTimeLabel, 0, 0)
        layout.addWidget(self._runTimeEdit, 0, 1)
        layout.addWidget(self._simTimeStepLabel, 1, 0)
        layout.addWidget(self._simTimeStepEdit, 1, 1)
        layout.setColumnStretch(2, 1.0)
        layout.setRowStretch(2, 1.0)
        self._runControlBox.setLayout(layout)
        self._runControlDock = QtGui.QDockWidget('Simulation', self)
        self._runControlDock.setWidget(self._runControlBox)

    def _createChannelControl(self):
        self._channelControlDock = QtGui.QDockWidget('Channels', self)
        self._channelCtrlBox = QtGui.QGroupBox(self)
        self._naConductanceToggle = QtGui.QCheckBox('Block Na+ channel', self._channelCtrlBox)
        self._naConductanceToggle.setToolTip('<html>%s</html>' % (tooltip_NaChan))
        self._kConductanceToggle = QtGui.QCheckBox('Block K+ channel', self._channelCtrlBox)
        self._kConductanceToggle.setToolTip('<html>%s</html>' % (tooltip_KChan))
        self._kOutLabel = QtGui.QLabel('[K+]out (mM)', self._channelCtrlBox)
        self._kOutEdit = QtGui.QLineEdit('%g' % (self.squid_setup.squid_axon.K_out), 
                                         self._channelCtrlBox)
        self._kOutLabel.setToolTip('<html>%s</html>' % (tooltip_Nernst))
        self._kOutEdit.setToolTip('<html>%s</html>' % (tooltip_Nernst))
        set_default_line_edit_size(self._kOutEdit)
        self._naOutLabel = QtGui.QLabel('[Na+]out (mM)', self._channelCtrlBox)
        self._naOutEdit = QtGui.QLineEdit('%g' % (self.squid_setup.squid_axon.Na_out), 
                                         self._channelCtrlBox)
        self._naOutLabel.setToolTip('<html>%s</html>' % (tooltip_Nernst))
        self._naOutEdit.setToolTip('<html>%s</html>' % (tooltip_Nernst))
        set_default_line_edit_size(self._naOutEdit)
        self._kInLabel = QtGui.QLabel('[K+]in (mM)', self._channelCtrlBox)
        self._kInEdit = QtGui.QLineEdit('%g' % (self.squid_setup.squid_axon.K_in), 
                                         self._channelCtrlBox)
        self._kInEdit.setToolTip(tooltip_Nernst)
        self._naInLabel = QtGui.QLabel('[Na+]in (mM)', self._channelCtrlBox)
        self._naInEdit = QtGui.QLineEdit('%g' % (self.squid_setup.squid_axon.Na_in), 
                                         self._channelCtrlBox)
        self._naInEdit.setToolTip('<html>%s</html>' % (tooltip_Nernst))
        self._temperatureLabel = QtGui.QLabel('Temperature (C)', self._channelCtrlBox)
        self._temperatureEdit = QtGui.QLineEdit('%g' % (self.defaults['temperature'] - CELSIUS_TO_KELVIN),
                                                self._channelCtrlBox)
        self._temperatureEdit.setToolTip('<html>%s</html>' % (tooltip_Nernst))
        set_default_line_edit_size(self._temperatureEdit)
        for child in self._channelCtrlBox.children():
            if isinstance(child, QtGui.QLineEdit):
                set_default_line_edit_size(child)
        layout = QtGui.QGridLayout(self._channelCtrlBox)
        layout.addWidget(self._naConductanceToggle, 0, 0)
        layout.addWidget(self._kConductanceToggle, 1, 0)
        layout.addWidget(self._naOutLabel, 2, 0)
        layout.addWidget(self._naOutEdit, 2, 1)
        layout.addWidget(self._naInLabel, 3, 0)
        layout.addWidget(self._naInEdit, 3, 1)
        layout.addWidget(self._kOutLabel, 4, 0)
        layout.addWidget(self._kOutEdit, 4, 1)
        layout.addWidget(self._kInLabel, 5, 0)
        layout.addWidget(self._kInEdit, 5, 1)
        layout.addWidget(self._temperatureLabel, 6, 0)
        layout.addWidget(self._temperatureEdit, 6, 1)
        layout.setRowStretch(7, 1.0)
        self._channelCtrlBox.setLayout(layout)
        self._channelControlDock.setWidget(self._channelCtrlBox)
        return self._channelCtrlBox        

    def __get_stateplot_data(self, name):
        data = []
        if name == 'V':
            data = self.squid_setup.vm_table.vector
        elif name == 'm':
            data = self.squid_setup.m_table.vector
        elif name == 'h':
            data = self.squid_setup.h_table.vector
        elif name == 'n':
            data = self.squid_setup.n_table.vector
        else:
            raise ValueError('Unrecognized selection: %s' % (name))
        return numpy.asarray(data)
    
    def _statePlotYSlot(self, selectedItem):
        ydata = self.__get_stateplot_data(str(selectedItem))
        self._state_plot.set_ydata(ydata)
        self._statePlotAxes.set_ylabel(selectedItem)
        if str(selectedItem) == 'V':
            self._statePlotAxes.set_ylim(-20, 120)
        else:
            self._statePlotAxes.set_ylim(0, 1)
        self._statePlotCanvas.draw()
        
    def _statePlotXSlot(self, selectedItem):
        xdata = self.__get_stateplot_data(str(selectedItem))
        self._state_plot.set_xdata(xdata)
        self._statePlotAxes.set_xlabel(selectedItem)
        if str(selectedItem) == 'V':
            self._statePlotAxes.set_xlim(-20, 120)
        else:
            self._statePlotAxes.set_xlim(0, 1)
        self._statePlotCanvas.draw()

    def _createElectronicsControl(self):
        """Creates a tabbed widget of voltage clamp and current clamp controls"""
        self._electronicsTab = QtGui.QTabWidget(self)
        self._electronicsTab.addTab(self._getIClampCtrlBox(), 'Current clamp')
        self._electronicsTab.addTab(self._getVClampCtrlBox(), 'Voltage clamp')
        self._electronicsDock = QtGui.QDockWidget(self)
        self._electronicsDock.setWidget(self._electronicsTab)

    def _getVClampCtrlBox(self):
        vClampPanel = QtGui.QGroupBox(self)
        self._vClampCtrlBox = vClampPanel
        self._holdingVLabel = QtGui.QLabel("Holding Voltage (mV)", vClampPanel)
        self._holdingVEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['vclamp.holdingV']), vClampPanel)
        self._holdingTimeLabel = QtGui.QLabel("Holding Time (ms)", vClampPanel)
        self._holdingTimeEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['vclamp.holdingT']), vClampPanel)
        self._prePulseVLabel = QtGui.QLabel("Pre-pulse Voltage (mV)", vClampPanel)
        self._prePulseVEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['vclamp.prepulseV']), vClampPanel)
        self._prePulseTimeLabel = QtGui.QLabel("Pre-pulse Time (ms)", vClampPanel)
        self._prePulseTimeEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['vclamp.prepulseT']), vClampPanel)
        self._clampVLabel = QtGui.QLabel("Clamp Voltage (mV)", vClampPanel)
        self._clampVEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['vclamp.clampV']), vClampPanel)
        self._clampTimeLabel = QtGui.QLabel("Clamp Time (ms)", vClampPanel)
        self._clampTimeEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['vclamp.clampT']), vClampPanel)
        for child in vClampPanel.children():
            if isinstance(child, QtGui.QLineEdit):
                set_default_line_edit_size(child)
        layout = QtGui.QGridLayout(vClampPanel)
        layout.addWidget(self._holdingVLabel, 0, 0)
        layout.addWidget(self._holdingVEdit, 0, 1)
        layout.addWidget(self._holdingTimeLabel, 1, 0)
        layout.addWidget(self._holdingTimeEdit, 1, 1)
        layout.addWidget(self._prePulseVLabel, 2, 0)
        layout.addWidget(self._prePulseVEdit, 2, 1)
        layout.addWidget(self._prePulseTimeLabel,3,0)
        layout.addWidget(self._prePulseTimeEdit, 3, 1)
        layout.addWidget(self._clampVLabel, 4, 0)
        layout.addWidget(self._clampVEdit, 4, 1)
        layout.addWidget(self._clampTimeLabel, 5, 0)
        layout.addWidget(self._clampTimeEdit, 5, 1)
        layout.setRowStretch(6, 1.0)
        vClampPanel.setLayout(layout)
        return self._vClampCtrlBox

    def _getIClampCtrlBox(self):
        iClampPanel = QtGui.QGroupBox(self)
        self._iClampCtrlBox = iClampPanel
        self._baseCurrentLabel = QtGui.QLabel("Base Current Level (uA)",iClampPanel)
        self._baseCurrentEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['iclamp.baseI']),iClampPanel)
        self._firstPulseLabel = QtGui.QLabel("First Pulse Current (uA)", iClampPanel)
        self._firstPulseEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['iclamp.firstI']), iClampPanel)
        self._firstDelayLabel = QtGui.QLabel("First Onset Delay (ms)", iClampPanel)
        self._firstDelayEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['iclamp.firstD']),iClampPanel)
        self._firstPulseWidthLabel = QtGui.QLabel("First Pulse Width (ms)", iClampPanel)
        self._firstPulseWidthEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['iclamp.firstT']), iClampPanel)
        self._secondPulseLabel = QtGui.QLabel("Second Pulse Current (uA)", iClampPanel)
        self._secondPulseEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['iclamp.secondI']), iClampPanel)
        self._secondDelayLabel = QtGui.QLabel("Second Onset Delay (ms)", iClampPanel)
        self._secondDelayEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['iclamp.secondD']),iClampPanel)
        self._secondPulseWidthLabel = QtGui.QLabel("Second Pulse Width (ms)", iClampPanel)
        self._secondPulseWidthEdit = QtGui.QLineEdit('%g' % (SquidGui.defaults['iclamp.secondT']), iClampPanel)
        self._pulseMode = QtGui.QComboBox(iClampPanel)
        self._pulseMode.addItem("Single Pulse")
        self._pulseMode.addItem("Pulse Train")
        for child in iClampPanel.children():
            if isinstance(child, QtGui.QLineEdit):
                set_default_line_edit_size(child)
        layout = QtGui.QGridLayout(iClampPanel)
        layout.addWidget(self._baseCurrentLabel, 0, 0)
        layout.addWidget(self._baseCurrentEdit, 0, 1)
        layout.addWidget(self._firstPulseLabel, 1, 0)
        layout.addWidget(self._firstPulseEdit, 1, 1)
        layout.addWidget(self._firstDelayLabel, 2, 0)
        layout.addWidget(self._firstDelayEdit, 2, 1)
        layout.addWidget(self._firstPulseWidthLabel, 3, 0)
        layout.addWidget(self._firstPulseWidthEdit, 3, 1)
        layout.addWidget(self._secondPulseLabel, 4, 0)
        layout.addWidget(self._secondPulseEdit, 4, 1)
        layout.addWidget(self._secondDelayLabel, 5, 0)
        layout.addWidget(self._secondDelayEdit, 5, 1)
        layout.addWidget(self._secondPulseWidthLabel, 6, 0)
        layout.addWidget(self._secondPulseWidthEdit, 6, 1)
        layout.addWidget(self._pulseMode, 7, 0, 1, 2)
        layout.setRowStretch(8, 1.0)        
        # layout.setSizeConstraint(QtGui.QLayout.SetFixedSize)
        iClampPanel.setLayout(layout)
        return self._iClampCtrlBox

    def _overlayPlots(self, overlay):        
        if not overlay:
            for axis in (self._plotFigure.axes + self._statePlotFigure.axes):
                title = axis.get_title()
                axis.clear()
                axis.set_title(title)
            suffix = ''
        else:
            suffix = '_%d' % (len(self._plot_dict['vm']))
        self._vm_axes.set_xlim(0.0, self._runtime)
        self._g_axes.set_xlim(0.0, self._runtime)
        self._im_axes.set_xlim(0.0, self._runtime)
        self._i_axes.set_xlim(0.0, self._runtime)
        self._vm_plot, = self._vm_axes.plot([], [], label='Vm%s'%(suffix))
        self._plot_dict['vm'].append(self._vm_plot)
        self._command_plot, = self._vm_axes.plot([], [], label='command%s'%(suffix))
        self._plot_dict['command'].append(self._command_plot)
        # Channel conductances go to the same subplot
        self._gna_plot, = self._g_axes.plot([], [], label='Na%s'%(suffix))
        self._plot_dict['gna'].append(self._gna_plot)
        self._gk_plot, = self._g_axes.plot([], [], label='K%s'%(suffix))
        self._plot_dict['gk'].append(self._gk_plot)
        # Injection current for Vclamp/Iclamp go to the same subplot
        self._iclamp_plot, = self._im_axes.plot([], [], label='Iclamp%s'%(suffix))
        self._vclamp_plot, = self._im_axes.plot([], [], label='Vclamp%s'%(suffix))
        self._plot_dict['iclamp'].append(self._iclamp_plot)
        self._plot_dict['vclamp'].append(self._vclamp_plot)
        # Channel currents go to the same subplot
        self._ina_plot, = self._i_axes.plot([], [], label='Na%s'%(suffix))
        self._plot_dict['ina'].append(self._ina_plot)
        self._ik_plot, = self._i_axes.plot([], [], label='K%s'%(suffix))
        self._plot_dict['ik'].append(self._ik_plot)
        # self._i_axes.legend()
        # State plots
        self._state_plot, = self._statePlotAxes.plot([], [], label='state%s'%(suffix))
        self._plot_dict['state'].append(self._state_plot)
        self._m_plot, = self._activationParamAxes.plot([],[], label='m%s'%(suffix))
        self._h_plot, = self._activationParamAxes.plot([], [], label='h%s'%(suffix))
        self._n_plot, = self._activationParamAxes.plot([], [], label='n%s'%(suffix))
        self._plot_dict['m'].append(self._m_plot)
        self._plot_dict['h'].append(self._h_plot)
        self._plot_dict['n'].append(self._n_plot)
        if self._showLegendAction.isChecked():
            for axis in (self._plotFigure.axes + self._statePlotFigure.axes):            
                axis.legend()

    def _updateAllPlots(self):
        self._updatePlots()
        self._updateStatePlot()

    def _updatePlots(self):
        if len(self.squid_setup.vm_table.vector) <= 0:
            return        
        vm = numpy.asarray(self.squid_setup.vm_table.vector)
        cmd = numpy.asarray(self.squid_setup.cmd_table.vector)
        ik = numpy.asarray(self.squid_setup.ik_table.vector)
        ina = numpy.asarray(self.squid_setup.ina_table.vector)
        iclamp = numpy.asarray(self.squid_setup.iclamp_table.vector)
        vclamp = numpy.asarray(self.squid_setup.vclamp_table.vector)
        gk = numpy.asarray(self.squid_setup.gk_table.vector)
        gna = numpy.asarray(self.squid_setup.gna_table.vector)
        time_series = numpy.linspace(0, self._plotdt * len(vm), len(vm))        
        self._vm_plot.set_data(time_series, vm)
        time_series = numpy.linspace(0, self._plotdt * len(cmd), len(cmd))        
        self._command_plot.set_data(time_series, cmd)
        time_series = numpy.linspace(0, self._plotdt * len(ik), len(ik))
        self._ik_plot.set_data(time_series, ik)
        time_series = numpy.linspace(0, self._plotdt * len(ina), len(ina))
        self._ina_plot.set_data(time_series, ina)
        time_series = numpy.linspace(0, self._plotdt * len(iclamp), len(iclamp))
        self._iclamp_plot.set_data(time_series, iclamp)
        time_series = numpy.linspace(0, self._plotdt * len(vclamp), len(vclamp))
        self._vclamp_plot.set_data(time_series, vclamp)
        time_series = numpy.linspace(0, self._plotdt * len(gk), len(gk))
        self._gk_plot.set_data(time_series, gk)
        time_series = numpy.linspace(0, self._plotdt * len(gna), len(gna))
        self._gna_plot.set_data(time_series, gna)
        # self._vm_axes.margins(y=0.1)
        # self._g_axes.margin(y=0.1)
        # self._im_axes.margins(y=0.1)
        # self._i_axes.margins(y=0.1)
        if self._autoscaleAction.isChecked():
            for axis in self._plotFigure.axes:
                axis.relim()
                axis.margins(0.1, 0.1)
                axis.autoscale_view(tight=True)
        else:
            self._vm_axes.set_ylim(-20.0, 120.0)
            self._g_axes.set_ylim(0.0, 0.5)
            self._im_axes.set_ylim(-0.5, 0.5)
            self._i_axes.set_ylim(-10, 10)
        self._vm_axes.set_xlim(0.0, time_series[-1])
        self._g_axes.set_xlim(0.0, time_series[-1])
        self._im_axes.set_xlim(0.0, time_series[-1])
        self._i_axes.set_xlim(0.0, time_series[-1])
        self._plotCanvas.draw()

    def _updateStatePlot(self):
        if len(self.squid_setup.vm_table.vector) <= 0:
            return
        sx = str(self._stateplot_xvar_combo.currentText())
        sy = str(self._stateplot_yvar_combo.currentText())
        xdata = self.__get_stateplot_data(sx)
        ydata = self.__get_stateplot_data(sy)
        minlen = min(len(xdata), len(ydata))
        self._state_plot.set_data(xdata[:minlen], ydata[:minlen])
        self._statePlotAxes.set_xlabel(sx)
        self._statePlotAxes.set_ylabel(sy)
        if sx == 'V':
            self._statePlotAxes.set_xlim(-20, 120)
        else:
            self._statePlotAxes.set_xlim(0, 1)
        if sy == 'V':
            self._statePlotAxes.set_ylim(-20, 120)
        else:
            self._statePlotAxes.set_ylim(0, 1)
        self._activationParamAxes.set_xlim(0, self._runtime)
        m = self.__get_stateplot_data('m')
        n = self.__get_stateplot_data('n')
        h = self.__get_stateplot_data('h')
        time_series = numpy.linspace(0, self._plotdt*len(m), len(m))
        self._m_plot.set_data(time_series, m)
        time_series = numpy.linspace(0, self._plotdt*len(h), len(h))
        self._h_plot.set_data(time_series, h)
        time_series = numpy.linspace(0, self._plotdt*len(n), len(n))
        self._n_plot.set_data(time_series, n)
        if self._autoscaleAction.isChecked():
            for axis in self._statePlotFigure.axes:
                axis.relim()
                axis.set_autoscale_on(True)
                axis.autoscale_view(True)
        self._statePlotCanvas.draw()

    def _runSlot(self):
        if moose.isRunning():
            print 'Stopping simulation in progress ...'
            moose.stop()
        self._runtime = self.getFloatInput(self._runTimeEdit, self._runTimeLabel.text())
        self._overlayPlots(self._overlayAction.isChecked())
        self._simdt = self.getFloatInput(self._simTimeStepEdit, self._simTimeStepLabel.text())
        clampMode = None
        singlePulse = True
        if self._electronicsTab.currentWidget() == self._vClampCtrlBox:
            clampMode = 'vclamp'
            baseLevel = self.getFloatInput(self._holdingVEdit, self._holdingVLabel.text())
            firstDelay = self.getFloatInput(self._holdingTimeEdit, self._holdingTimeLabel.text())
            firstWidth = self.getFloatInput(self._prePulseTimeEdit, self._prePulseTimeLabel.text())
            firstLevel = self.getFloatInput(self._prePulseVEdit, self._prePulseVLabel.text())
            secondDelay = firstWidth
            secondWidth = self.getFloatInput(self._clampTimeEdit, self._clampTimeLabel.text())
            secondLevel = self.getFloatInput(self._clampVEdit, self._clampVLabel.text())
            if not self._autoscaleAction.isChecked():
                self._im_axes.set_ylim(-10.0, 10.0)
        else:
            clampMode = 'iclamp'
            baseLevel = self.getFloatInput(self._baseCurrentEdit, self._baseCurrentLabel.text())
            firstDelay = self.getFloatInput(self._firstDelayEdit, self._firstDelayLabel.text())
            firstWidth = self.getFloatInput(self._firstPulseWidthEdit, self._firstPulseWidthLabel.text())
            firstLevel = self.getFloatInput(self._firstPulseEdit, self._firstPulseLabel.text())
            secondDelay = self.getFloatInput(self._secondDelayEdit, self._secondDelayLabel.text())
            secondLevel = self.getFloatInput(self._secondPulseEdit, self._secondPulseLabel.text())
            secondWidth = self.getFloatInput(self._secondPulseWidthEdit, self._secondPulseWidthLabel.text())
            singlePulse = (self._pulseMode.currentIndex() == 0)
            if not self._autoscaleAction.isChecked():
                self._im_axes.set_ylim(-0.4, 0.4)
        self.squid_setup.clamp_ckt.configure_pulses(baseLevel=baseLevel,
                                                    firstDelay=firstDelay,
                                                    firstWidth=firstWidth,
                                                    firstLevel=firstLevel,
                                                    secondDelay=secondDelay,
                                                    secondWidth=secondWidth,
                                                    secondLevel=secondLevel,
                                                    singlePulse=singlePulse)
        if self._kConductanceToggle.isChecked():
            self.squid_setup.squid_axon.specific_gK = 0.0
        else:
            self.squid_setup.squid_axon.specific_gK = SquidAxon.defaults['specific_gK']
        if self._naConductanceToggle.isChecked():
            self.squid_setup.squid_axon.specific_gNa = 0.0
        else:
            self.squid_setup.squid_axon.specific_gNa = SquidAxon.defaults['specific_gNa']
        self.squid_setup.squid_axon.celsius = self.getFloatInput(self._temperatureEdit, self._temperatureLabel.text())
        self.squid_setup.squid_axon.K_out = self.getFloatInput(self._kOutEdit, self._kOutLabel.text())
        self.squid_setup.squid_axon.Na_out = self.getFloatInput(self._naOutEdit, self._naOutLabel.text())
        self.squid_setup.squid_axon.K_in = self.getFloatInput(self._kInEdit, self._kInLabel.text())
        self.squid_setup.squid_axon.Na_in = self.getFloatInput(self._naInEdit, self._naInLabel.text())
        self.squid_setup.squid_axon.updateEk()
        self.squid_setup.schedule(self._simdt, self._plotdt, clampMode)
        # The following line is for use with Qthread
        self.squid_setup.run(self._runtime)
        self._updateAllPlots()

    def _toggleDocking(self, on):
        self._channelControlDock.setFloating(on)
        self._electronicsDock.setFloating(on)
        self._runControlDock.setFloating(on)
        
    def _restoreDocks(self):
        self._channelControlDock.setVisible(True)
        self._electronicsDock.setVisible(True)
        self._runControlDock.setVisible(True)

    def _initActions(self):
        self._runAction = QtGui.QAction(self.tr('Run'), self)
        self._runAction.setShortcut(self.tr('F5'))
        self._runAction.setToolTip('Run simulation (F5)')
        self.connect(self._runAction, QtCore.SIGNAL('triggered()'), self._runSlot)
        self._resetToDefaultsAction = QtGui.QAction(self.tr('Restore defaults'), self)
        self._resetToDefaultsAction.setToolTip('Reset all settings to their default values')
        self.connect(self._resetToDefaultsAction, QtCore.SIGNAL('triggered()'), self._useDefaults)
        self._showLegendAction = QtGui.QAction(self.tr('Display legend'), self)
        self._showLegendAction.setCheckable(True)
        self.connect(self._showLegendAction, QtCore.SIGNAL('toggled(bool)'), self._showLegend)
        self._showStatePlotAction = QtGui.QAction(self.tr('State plot'), self)
        self._showStatePlotAction.setCheckable(True)
        self._showStatePlotAction.setChecked(False)
        self.connect(self._showStatePlotAction, QtCore.SIGNAL('toggled(bool)'), self._statePlotWidget.setVisible)
        self._autoscaleAction  = QtGui.QAction(self.tr('Auto-scale plots'), self)
        self._autoscaleAction.setCheckable(True)
        self._autoscaleAction.setChecked(False)
        self.connect(self._autoscaleAction, QtCore.SIGNAL('toggled(bool)'), self._autoscale)
        self._overlayAction = QtGui.QAction('Overlay plots', self)
        self._overlayAction.setCheckable(True)
        self._overlayAction.setChecked(False) 
        self._dockAction = QtGui.QAction('Undock all', self)
        self._dockAction.setCheckable(True)
        self._dockAction.setChecked(False)
        self.connect(self._dockAction, QtCore.SIGNAL('toggled(bool)'), self._toggleDocking)
        self._restoreDocksAction = QtGui.QAction('Show all', self)
        self.connect(self._restoreDocksAction, QtCore.SIGNAL('triggered()'), self._restoreDocks)
        self._quitAction = QtGui.QAction(self.tr('&Quit'), self)
        self._quitAction.setShortcut(self.tr('Ctrl+Q'))
        self.connect(self._quitAction, QtCore.SIGNAL('triggered()'), QtGui.qApp.closeAllWindows)

        

    def _createRunToolBar(self):
        self._simToolBar = self.addToolBar(self.tr('Simulation control'))
        self._simToolBar.addAction(self._quitAction)
        self._simToolBar.addAction(self._runAction)
        self._simToolBar.addAction(self._resetToDefaultsAction)
        self._simToolBar.addAction(self._dockAction)
        self._simToolBar.addAction(self._restoreDocksAction)

    def _createPlotToolBar(self):
        self._plotToolBar = self.addToolBar(self.tr('Plotting control'))
        self._plotToolBar.addAction(self._showLegendAction)
        self._plotToolBar.addAction(self._autoscaleAction)
        self._plotToolBar.addAction(self._overlayAction)
        self._plotToolBar.addAction(self._showStatePlotAction)
        self._plotToolBar.addAction(self._helpAction)
        self._plotToolBar.addAction(self._helpBiophysicsAction)

    def _showLegend(self, on):
        if on:
            for axis in (self._plotFigure.axes + self._statePlotFigure.axes):            
                axis.legend().set_visible(True)
        else:
            for axis in (self._plotFigure.axes + self._statePlotFigure.axes):            
                axis.legend().set_visible(False)
        self._plotCanvas.draw()
        self._statePlotCanvas.draw()

    def _autoscale(self, on):
        if on:
            for axis in (self._plotFigure.axes + self._statePlotFigure.axes):            
                axis.relim()
                axis.set_autoscale_on(True)
                axis.autoscale_view(True)
        else:
            for axis in self._plotFigure.axes:
                axis.set_autoscale_on(False)            
            self._vm_axes.set_ylim(-20.0, 120.0)
            self._g_axes.set_ylim(0.0, 0.5)
            self._im_axes.set_ylim(-0.5, 0.5)
            self._i_axes.set_ylim(-10, 10)
        self._plotCanvas.draw()
        self._statePlotCanvas.draw()
        
    def _useDefaults(self):
        self._runTimeEdit.setText('%g' % (self.defaults['runtime']))
        self._simTimeStepEdit.setText('%g' % (self.defaults['simdt']))
        self._overlayAction.setChecked(False)
        self._naConductanceToggle.setChecked(False)
        self._kConductanceToggle.setChecked(False)
        self._kOutEdit.setText('%g' % (SquidGui.defaults['K_out']))
        self._naOutEdit.setText('%g' % (SquidGui.defaults['Na_out']))
        self._kInEdit.setText('%g' % (SquidGui.defaults['K_in']))
        self._naInEdit.setText('%g' % (SquidGui.defaults['Na_in']))
        self._temperatureEdit.setText('%g' % (SquidGui.defaults['temperature'] - CELSIUS_TO_KELVIN))
        self._holdingVEdit.setText('%g' % (SquidGui.defaults['vclamp.holdingV']))
        self._holdingTimeEdit.setText('%g' % (SquidGui.defaults['vclamp.holdingT']))
        self._prePulseVEdit.setText('%g' % (SquidGui.defaults['vclamp.prepulseV']))
        self._prePulseTimeEdit.setText('%g' % (SquidGui.defaults['vclamp.prepulseT']))
        self._clampVEdit.setText('%g' % (SquidGui.defaults['vclamp.clampV']))
        self._clampTimeEdit.setText('%g' % (SquidGui.defaults['vclamp.clampT']))
        self._baseCurrentEdit.setText('%g' % (SquidGui.defaults['iclamp.baseI']))
        self._firstPulseEdit.setText('%g' % (SquidGui.defaults['iclamp.firstI']))
        self._firstDelayEdit.setText('%g' % (SquidGui.defaults['iclamp.firstD']))
        self._firstPulseWidthEdit.setText('%g' % (SquidGui.defaults['iclamp.firstT']))
        self._secondPulseEdit.setText('%g' % (SquidGui.defaults['iclamp.secondI']))
        self._secondDelayEdit.setText('%g' % (SquidGui.defaults['iclamp.secondD']))
        self._secondPulseWidthEdit.setText('%g' % (SquidGui.defaults['iclamp.secondT']))
        self._pulseMode.setCurrentIndex(0)

    def _onScroll(self, event):
        if event.inaxes is None:
            return  
        axes = event.inaxes
        zoom = 0.0
        if event.button == 'up':
            zoom = -1.0
        elif event.button == 'down':
            zoom = 1.0
        if zoom != 0.0:
            self._plotNavigator.push_current()
            axes.get_xaxis().zoom(zoom)
            axes.get_yaxis().zoom(zoom)        
        self._plotCanvas.draw()

    def closeEvent(self, event):
        QtGui.qApp.closeAllWindows()

    def _showBioPhysicsHelp(self):
        self._createHelpMessage()
        self._helpMessageText.setText('<html><p>%s</p><p>%s</p><p>%s</p><p>%s</p><p>%s</p></html>' % 
                                      (tooltip_Nernst, 
                                       tooltip_Erest,
                                       tooltip_KChan,
                                       tooltip_NaChan,
                                       tooltip_Im))
        self._helpWindow.setVisible(True)

    def _showRunningHelp(self):
        self._createHelpMessage()
        self._helpMessageText.setSource(QtCore.QUrl(self._helpBaseURL))
        self._helpWindow.setVisible(True)

    def _createHelpMessage(self):
        if hasattr(self, '_helpWindow'):
            return
        self._helpWindow = QtGui.QWidget()
        self._helpWindow.setWindowFlags(QtCore.Qt.Window)
        layout = QtGui.QVBoxLayout()
        self._helpWindow.setLayout(layout)
        self._helpMessageArea = QtGui.QScrollArea()
        self._helpMessageText = QtGui.QTextBrowser()
        self._helpMessageText.setOpenExternalLinks(True)
        self._helpMessageArea.setWidget(self._helpMessageText)
        layout.addWidget(self._helpMessageText)
        self._squidGuiPath = os.path.dirname(os.path.abspath(__file__))
        self._helpBaseURL = os.path.join(self._squidGuiPath,'help.html')
        self._helpMessageText.setSource(QtCore.QUrl(self._helpBaseURL))
        self._helpMessageText.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self._helpMessageArea.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self._helpMessageText.setMinimumSize(800, 600)
        self._closeHelpAction = QtGui.QAction('Close', self)
        self.connect(self._closeHelpAction, QtCore.SIGNAL('triggered()'), self._helpWindow.close)        
        # Setting the close event so that the ``Help`` button is
        # unchecked when the help window is closed
        self._helpWindow.closeEvent = lambda event: self._helpAction.setChecked(False)
        self._helpTOCAction = QtGui.QAction('Help running demo', self)
        self.connect(self._helpTOCAction, QtCore.SIGNAL('triggered()'), self._jumpToHelpTOC)                
        # This panel is for putting two buttons using horizontal
        # layout
        panel = QtGui.QFrame()
        panel.setFrameStyle(QtGui.QFrame.StyledPanel + QtGui.QFrame.Raised)
        layout.addWidget(panel)
        layout = QtGui.QHBoxLayout()
        panel.setLayout(layout)
        self._helpAction = QtGui.QAction('Help running', self)
        self.connect(self._helpAction, QtCore.SIGNAL('triggered()'), self._showRunningHelp)
        self._helpBiophysicsAction = QtGui.QAction('Help biophysics', self)
        self.connect(self._helpBiophysicsAction, QtCore.SIGNAL('triggered()'), self._showBioPhysicsHelp)
        self._helpTOCButton = QtGui.QToolButton()
        self._helpTOCButton.setDefaultAction(self._helpTOCAction)
        self._helpBiophysicsButton = QtGui.QToolButton()
        self._helpBiophysicsButton.setDefaultAction(self._helpBiophysicsAction)
        layout.addWidget(self._helpTOCButton)
        layout.addWidget(self._helpBiophysicsButton)
        self._closeHelpButton = QtGui.QToolButton()
        self._closeHelpButton.setDefaultAction(self._closeHelpAction)
        layout.addWidget(self._closeHelpButton)
        
    def _jumpToHelpTOC(self):
        self._helpMessageText.setSource(QtCore.QUrl(self._helpBaseURL))

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    app.connect(app, QtCore.SIGNAL('lastWindowClosed()'), app, QtCore.SLOT('quit()'))
    QtGui.qApp = app
    squid_gui = SquidGui()
    squid_gui.show()
    print squid_gui.size()
    sys.exit(app.exec_())

# 
# squidgui.py ends here
