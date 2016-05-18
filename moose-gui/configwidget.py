# firsttime.py --- 
# 
# Filename: firsttime.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Sun Jul 11 15:31:00 2010 (+0530)
# Version: 
# Last-Updated: Sat Sep 22 15:30:15 2012 (+0530)
#           By: subha
#     Update #: 505
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Wizard to take the user through selection of some basic
# configurations for MOOSE gui.
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

import shutil
import os
import config
from PyQt4 import QtGui, QtCore
import config
from utils import *
        
class ConfigWidget(QtGui.QDialog):
    """Widget to configure MOOSE."""
    def __init__(self, *args, **kwargs):
        super(QtGui.QDialog, self).__init__(*args, **kwargs)
        self.tabWidget = QtGui.QTabWidget(self)
        self.buttonBox = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Ok | 
                                                QtGui.QDialogButtonBox.Cancel|
                                                QtGui.QDialogButtonBox.RestoreDefaults)
        self.connect(self.buttonBox, QtCore.SIGNAL('accepted()'), self.accept)
        self.connect(self.buttonBox, QtCore.SIGNAL('rejected()'), self.reject)
        self.connect(self.buttonBox.button(QtGui.QDialogButtonBox.RestoreDefaults), 
                     QtCore.SIGNAL('clicked()'),
                     self._onRestoreDefaults)
        self.connect(self, QtCore.SIGNAL('accepted()'), self._onAccept)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.tabWidget)
        layout.addWidget(self.buttonBox)
        self.setLayout(layout)
        self._createGeneralPanel()
    
    def _createGeneralPanel(self):
        if hasattr(self, 'generalPanel'):
            return self.generalPanel
        self.generalPanel = QtGui.QWidget(self)
        self.generalPanelLabel = QtGui.QLabel(self.generalPanel)
        self.generalPanelLabel.setText('<center>General configuration for MOOSE</center>')
        self.colormapDirLabel = QtGui.QLabel('Colormap directory', 
                                             self.generalPanel)
        self.colormapDirEdit = QtGui.QLineEdit(config.settings[config.KEY_COLORMAP_DIR], 
                                               self.generalPanel)
        self.demosDirLabel = QtGui.QLabel('Demos directory', self.generalPanel)
        self.demosDirEdit = QtGui.QLineEdit(config.settings[config.KEY_DEMOS_DIR])
        self.numPThreadsLabel = QtGui.QLabel('Number of process threads<p><i>(restart MOOSE to take effect)</i></p>', self.generalPanel)
        self.numPThreadsEdit = QtGui.QLineEdit(config.settings[config.KEY_NUMPTHREADS])
        layout = QtGui.QGridLayout()
        layout.addWidget(self.generalPanelLabel, 0, 0, 1, 4)
        layout.setRowStretch(0, 2)
        layout.addWidget(self.colormapDirLabel, 1, 0)
        layout.addWidget(self.colormapDirEdit, 1, 2)
        layout.addWidget(self.demosDirLabel, 2, 0)
        layout.addWidget(self.demosDirEdit, 2, 2)
        layout.addWidget(self.numPThreadsLabel, 3, 0)
        layout.addWidget(self.numPThreadsEdit, 3, 2)
        self.generalPanel.setLayout(layout)
        self.tabWidget.addTab(self.generalPanel, 'General')
        return self.generalPanel

    def _onAccept(self):
        config.settings[config.KEY_DEMOS_DIR] = self.demosDirEdit.text()
        config.settings[config.KEY_COLORMAP_DIR] = self.colormapDirEdit.text()
        config.settings[config.KEY_NUMPTHREADS] = self.numPThreadsEdit.text()

    def _onRestoreDefaults(self):        
        self.demosDirEdit.setText(config.MOOSE_DEMOS_DIR)
        self.colormapDirEdit.setText(os.path.join(config.MOOSE_GUI_DIR, 'colormaps'))
        self.numPThreadsEdit.setText('1')

        
if __name__ == '__main__':
    """Test the configuration widget"""
    app =QtGui.QApplication([])
    widget = ConfigWidget()
    widget.setMinimumSize(600, 200)
    widget.show()
    app.exec_()
        

