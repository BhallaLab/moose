# fileloader.py --- 
# 
# Filename: fileloader.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Feb 25 15:59:54 2013 (+0530)
# Version: 
# Last-Updated: Wed May 22 12:30:14 2013 (+0530)
#           By: subha
#     Update #: 106
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Dialog for loading model files
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
from PyQt4 import QtGui,QtCore,Qt
import moose
import os
import posixpath

class LoaderDialog(QtGui.QFileDialog):
    # Update ftypes to include new file types 
    ftypes='All Supported Files (*.cspace *.g *.xml *.p);; CSPACE (*.cspace);; GENESIS (*.g);; GENESIS Prototype (*.p);; NeuroML/SBML (*.xml)'
    target_default = '' # The default target when loading a model
    def __init__(self, *args):
        self.modelpath = None
        super(LoaderDialog, self).__init__(*args)
        self.setNameFilter(self.tr(self.ftypes))
        self.setNameFilterDetailsVisible(True)
        self.setReadOnly(True)
        self.setFileMode(self.ExistingFile)
        # self.targetPanel = QtGui.QFrame()
        # self.targetLabel = QtGui.QLabel('Model name')
        # self.targetText = QtGui.QLineEdit(self.target_default)
        # form = QtGui.QFormLayout()
        # form.addRow(self.targetLabel, self.targetText)
        # self.modelChoiceBox = QtGui.QGroupBox('Model name')
        # self.replaceExistingButton = QtGui.QRadioButton('&Replace current model')
        # self.mergeExistingButton = QtGui.QRadioButton('&Keep current model')
        # self.replaceExistingButton.setChecked(True)
        # vbox = QtGui.QVBoxLayout()
        # vbox.addWidget(self.replaceExistingButton)
        # vbox.addWidget(self.mergeExistingButton)
        # self.modelChoiceBox.setLayout(vbox)
        # self.targetPanel.setLayout(form)
        # self.layout().addWidget(self.targetPanel)
        # self.layout().addWidget(self.modelChoiceBox)
        self.fileSelected.connect(self.fileSelectedSlot)
        
    def fileSelectedSlot(self, fpath):
        """On selecting a file, this function will cause the target location to change to:

        /model/filename_minus_extension

        """
        self.modelpath = os.path.splitext(os.path.basename(str(fpath)))[0]
                                  
    # def isReplace(self):
    #     return self.replaceExistingButton.isChecked()

    # def isMerge(self):
    #     return self.mergeExistingButton.isChecked()

    def getTargetPath(self):
        return self.modelpath


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    QtGui.qApp = app
    mw = LoaderDialog()
    mw.show()
    # mw.exec_()
    sys.exit(app.exec_())
        


# 
# fileloader.py ends here
