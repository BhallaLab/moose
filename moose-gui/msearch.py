# msearch.py --- 
# 
# Filename: msearch.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue Jun  4 15:10:16 2013 (+0530)
# Version: 
# Last-Updated: Thu Jun  6 18:26:02 2013 (+0530)
#           By: subha
#     Update #: 181
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
Widget giving access to wildcardFind function in MOOSE

"""
import sys

from PyQt4 import QtGui, QtCore
from PyQt4.Qt import Qt

import moose

class SearchWidget(QtGui.QWidget):
    """Widget to search MOOSE model tree using wildcards.
    
    SIGNALS:

    executed(list of elements matching search criterion).

    """
    executed = QtCore.pyqtSignal(list, name='executed')

    def __init__(self, *args):
        QtGui.QWidget.__init__(self, *args)
        layout = QtGui.QVBoxLayout()
        self.modeCombo = QtGui.QComboBox()
        self.modeCombo.addItem('Basic', QtCore.QVariant(QtCore.QString('basic')))
        self.modeCombo.addItem('Advanced', QtCore.QVariant(QtCore.QString('advanced')))
        self.modeCombo.setCurrentIndex(0)
        self.setLayout(layout)
        layout.addWidget(self.modeCombo)
        layout.addWidget(self.getBasicPanel())
        layout.addWidget(self.getAdvancedPanel())
        self.connect(self.modeCombo, QtCore.SIGNAL('currentIndexChanged(const QString &)'), self.switchModeSlot)
        self.switchModeSlot('basic')
        self.searchButton = QtGui.QPushButton('Search')
        self.searchButton.clicked.connect(self.searchSlot)
        layout.addWidget(self.searchButton)
        self.__results = []

    def switchModeSlot(self, text):
        basic = (str(text).lower() == 'basic')
        self.getBasicPanel().setVisible(basic)
        self.getAdvancedPanel().setVisible(not basic)

    def getBasicPanel(self):
        """Create a widget for basic level usage."""
        if not hasattr(self, '_basicPanel'):
            self._basicPanel = QtGui.QWidget()
            self.searchRootEdit = QtGui.QLineEdit()
            self.searchRootEdit.returnPressed.connect(self.searchSlot)
            self.searchRootLabel = QtGui.QLabel('Search under')
            self.typeEdit = QtGui.QLineEdit()
            self.typeEdit.returnPressed.connect(self.searchSlot)            
            self.typeLabel = QtGui.QLabel('for elements of type')
            self.fieldEdit = QtGui.QLineEdit()
            self.fieldEdit.returnPressed.connect(self.searchSlot)            
            self.fieldLabel = QtGui.QLabel('or elements with field')
            self.comparisonCombo = QtGui.QComboBox()
            self.comparisonCombo.addItem('=', QtCore.QVariant(QtCore.QString('=')))
            self.comparisonCombo.addItem('<', QtCore.QVariant(QtCore.QString('<')))
            self.comparisonCombo.addItem('>', QtCore.QVariant(QtCore.QString('>')))
            self.comparisonCombo.addItem('<=', QtCore.QVariant(QtCore.QString('<=')))
            self.comparisonCombo.addItem('>=', QtCore.QVariant(QtCore.QString('>=')))
            self.comparisonCombo.addItem('!=', QtCore.QVariant(QtCore.QString('!=')))
            self.valueEdit = QtGui.QLineEdit()
            self.valueEdit.returnPressed.connect(self.searchSlot)            
            self.recurseButton = QtGui.QRadioButton('Search inside children recursively')
            self.recurseButton.setChecked(True)
            layout = QtGui.QGridLayout()
            # layout.setVerticalSpacing(2)
            self._basicPanel.setLayout(layout)
            layout.addWidget(self.searchRootLabel, 0, 0, 1, 1)
            layout.addWidget(self.searchRootEdit, 0, 1, 1, 4)
            layout.addWidget(self.typeLabel, 1, 0, 1, 1)
            layout.addWidget(self.typeEdit, 1, 1, 1, 4)
            layout.addWidget(self.fieldLabel, 2, 0, 1, 1)
            layout.addWidget(self.fieldEdit, 2, 1, 1, 1)
            layout.addWidget(self.comparisonCombo, 2, 2, 1, 1)
            layout.addWidget(self.valueEdit, 2, 3, 1, 2)
            layout.addWidget(self.recurseButton, 3, 0, 1, 1)
            # layout.addItem(QtGui.QSpacerItem(1,1), 3, 0, 1, 7)
        return self._basicPanel

    def getAdvancedPanel(self):
        """Create a panel for advanced users who can enter the search string
        directly."""
        if not hasattr(self, '_advancedPanel'):
            self._advancedPanel = QtGui.QWidget(self)
            self._searchEdit = QtGui.QLineEdit()
            self._searchEdit.returnPressed.connect(self.searchSlot)
            self._searchLabel = QtGui.QLabel('Search string:')
            layout = QtGui.QHBoxLayout()
            self._advancedPanel.setLayout(layout)
            layout.addWidget(self._searchLabel)
            layout.addWidget(self._searchEdit)
        return self._advancedPanel
    
    def searchSlot(self):
        """Do the search and store the paths of the id's found. The results
        can be obtained later with getResults()"""
        search_str = ''
        if str(self.modeCombo.currentText()).lower() == 'basic':
            root = str(self.searchRootEdit.text()).strip()
            if len(root) == 0:
                root = '/'
            search_str += root
            if not search_str.endswith('/'):
                search_str += '/'
            if self.recurseButton.isChecked():
                search_str += '##'
            else:
                search_str += '#'
            type_ = str(self.typeEdit.text()).strip()
            if len(type_) > 0:
                search_str += '[ISA=%s]' % (type_)
            field = str(self.fieldEdit.text()).strip()
            if len(field) > 0:
                search_str += '[FIELD(%s)'
                operator = str(self.comparisonCombo.currentText())
                value = str(self.valueEdit.text())
                search_str += '%s%s]' % (operator, value)
        else:
            search_str = str(self._searchEdit.text()).strip()
        ids = moose.wildcardFind(search_str)
        self.__results = [el.path for el in ids]
        self.executed.emit(list(self.__results))

    def getResults(self):
        return list(self.__results)

    def setSearchRoot(self, root):
        self.searchRootEdit.setText(moose.element(root).path)

def main():
    """Test main"""
    model = moose.Neutral('/model')
    moose.loadModel('../Demos/Genesis_files/Kholodenko.g', '/model/Kholodenko')
    # tab = moose.element('/model/Kholodenko/graphs/conc1/MAPK_PP.Co')
    # print tab
    # for t in tab.children:
    #     print t
    app = QtGui.QApplication(sys.argv)
    mainwin = QtGui.QMainWindow()
    mainwin.setWindowTitle('Model tree test')
    wildcardWidget = SearchWidget()
    mainwin.setCentralWidget(wildcardWidget)
    mainwin.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()


# 
# msearch.py ends here
