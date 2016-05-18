# checkcombobox.py --- 
# 
# Filename: checkcombobox.py
# Description: 
# Author: 
# Maintainer: 
# Created: Wed Jun  5 15:06:21 2013 (+0530)
# Version: 
# Last-Updated: Wed Jun  5 18:42:50 2013 (+0530)
#           By: subha
#     Update #: 188
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# ComboBox with checkable items. Inspired by the same in libqxt.
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

from PyQt4 import QtCore, QtGui
from PyQt4.Qt import Qt
from collections import defaultdict

class CheckComboBoxModel(QtGui.QStandardItemModel):
    """This is inspired by Qxt library."""
    checkStateChange = QtCore.pyqtSignal(name='checkStateChanged')
    def __init__(self, *args):
        QtGui.QStandardItemModel.__init__(self, *args)
        self.checked_dict = defaultdict(int)

    def flags(self, index):
        return Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled

    def data(self, index, role):
        if index.isValid() and role == Qt.CheckStateRole:
            return QtCore.QVariant(self.checked_dict[index])
        else:
            return QtGui.QStandardItemModel.data(self, index, role)
        
    def setData(self, index, value, role=Qt.EditRole):
        if not index.isValid():
            return False
        ok = QtGui.QStandardItemModel.setData(self, index, value, role)
        if ok and role == Qt.CheckStateRole:
            self.checked_dict[index] = value.toInt()[0]
            self.dataChanged.emit(index, index)
            self.checkStateChange.emit()    
        return ok

class ComboEventFilter(QtCore.QObject):
    """Event filter for CheckComboBox - inspired by Qxt library"""
    def __init__(self, parent):
        QtCore.QObject.__init__(self, parent)        

    def eventFilter(self, obj, event):
        etype = event.type()
        if etype == event.KeyPress or etype == event.KeyRelease:
            if obj == self and \
               (event.key() == Qt.Key_UP or 
                event.key() == Qt.Key_Down):
                self.parent().showPopup()
                return True
            elif event.key() == Qt.Key_Enter or \
                 event.key() == Qt.Key_Return or \
                 event.key() == Qt.Key_Escape:
                QtGui.QComboBox.hidePopup(self.parent())
                if event.key() != Qt.Key_Escape:
                    return True
        elif etype == event.MouseButtonPress:
            self.parent()._containerMousePress = (obj == self.parent().view().window())
        elif etype == event.MouseButtonRelease:
            self.parent()._containerMousePress = False
        return False
    
class CheckComboBox(QtGui.QComboBox):
    """Specialization of QComboBox to allow checkable items. This is
    inspired by the same class in Qxt"""
    def __init__(self, *args):
        QtGui.QComboBox.__init__(self, *args)
        self._containerMousePress = False
        self.setModel(CheckComboBoxModel())
        self.activated.connect(self.toggleCheckState)
        self.ef = ComboEventFilter(self)
        self.view().installEventFilter(self.ef)
        self.view().window().installEventFilter(self.ef)
        self.view().viewport().installEventFilter(self.ef)
        self.installEventFilter(self.ef)
        
    def hidePopup(self):
        """This is to disable hiding of the popup when an item is clicked."""
        if self._containerMousePress:
            QtGui.QComboBox.hidePopup(self)

    def itemCheckState(self, index):
        return self.itemData(index, Qt.CheckStateRole).toInt()[0]

    def setItemCheckState(self, index, state):
        self.setItemData(index, QtCore.QVariant(state), QtCore.Qt.CheckStateRole)

    def checkedItems(self):
        index = self.model().index(0,0)
        checked = self.model().match(index, Qt.CheckStateRole, Qt.Checked, -1, Qt.MatchExactly)
        return [index.data().toString() for index in checked]

    def setCheckedItems(self, textItemList):
        for text in textItemList:
            index = self.findText(text)
            if index.isValid():
                self.setItemCheckState(index, Qt.Checked)
            else:
                self.setItemCheckState(index, Qt.Unchecked)
                
    def toggleCheckState(self, index):
        value = self.itemData(index, Qt.CheckStateRole)
        if value.isValid():
            state = value.toInt()[0]
            if state == Qt.Checked:
                self.setItemData(index, Qt.Unchecked, Qt.CheckStateRole)
            else:
                self.setItemData(index, Qt.Checked, Qt.CheckStateRole)

def main():
    """Test main: load a model and display the tree for it"""
    app = QtGui.QApplication([])
    mainwin = QtGui.QMainWindow()
    mainwin.setWindowTitle('CheckComboBox test')
    box = CheckComboBox()
    for ii in range(5):
        box.addItem('myitem_%d' % (ii))
    mainwin.setCentralWidget(box)
    mainwin.show()
    sys.exit(app.exec_())
        
if __name__ == '__main__':
    main()


# 
# checkcombobox.py ends here
