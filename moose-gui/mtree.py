# mtree.py --- 
# 
# Filename: mtree.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Tue May 14 11:51:35 2013 (+0530)
# Version: 
# Last-Updated: Fri Jun 14 16:13:08 2013 (+0530)
#           By: subha
#     Update #: 154
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Implementation of moose tree widget. This can be used by multiple
# components in the moose gui.
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
from PyQt4 import QtGui, QtCore
from PyQt4.Qt import Qt
import moose


class MooseTreeModel(QtCore.QAbstractItemModel):
    """Tree model for the MOOSE element tree.
    
    This is not going to work as the MOOSE tree nodes are
    inhomogeneous. The parent of a node is an melement, but the
    children of an melement are vec objects.

    Qt can handle only homogeneous tere nodes.
    """
    def __init__(self, *args):
        super(MooseTreeModel, self).__init__(*args)
        self.rootItem = moose.element('/')

    def index(self, row, column, parent):
        if not self.hasIndex(row, column, parent):
            return QtCore.QModelIndex()
        if not parent.isValid():
            parentItem = self.rootItem
        else:
            parentItem = parent.internalPointer()
        childItem = parentItem.children[row]
        if childItem.path == '/':
            return QtCore.QModelIndex()
        return self.createIndex(row, column, childItem)

    def parent(self, index):
        if not index.isValid():
            return QtCore.QModelIndex()
        childItem = index.internalPointer()
        parentItem = childItem.parent()
        if parentItem == self.rootItem:
            return QtCore.QModelIndex()
        return self.createIndex(parentItem.parent.children.index(parentItem), parentItem.getDataIndex(), parentItem)

    def rowCount(self, parent):
        if not parent.isValid():
            parentItem = self.rootItem
        else:
            parentItem = parent.internalPointer()
        ret = len(parentItem.children)
        return ret

    def columnCount(self, parent):
        if parent.isValid():
            return len(parent.internalPointer())
        ret = len(self.rootItem)
        return ret

    def data(self, index, role):
        if not index.isValid():
            return None
        item = index.internalPointer()
        return QtCore.QVariant(item[index.column()].name)

    def headerData(self, section, orientation, role=Qt.DisplayRole):
        if orientation == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:  
            return QtCore.QVariant('Model Tree')
        return None

    def flags(self, index):
         if not index.isValid():
             return QtCore.Qt.NoItemFlags
         return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable
        
    
class MooseTreeItem(QtGui.QTreeWidgetItem):
    def __init__(self, *args):
	QtGui.QTreeWidgetItem.__init__(self, *args)
	self.mobj = None

    def setObject(self, element):
        self.mobj = moose.element(element)
	self.setText(0, QtCore.QString(self.mobj.path.rpartition('/')[-1]))
	self.setText(1, QtCore.QString(self.mobj.className))
	#self.setToolTip(0, QtCore.QString('class:' + self.mooseObj_.className))

    def updateSlot(self):
	self.setText(0, QtCore.QString(self.mobj.name))


class MooseTreeWidget(QtGui.QTreeWidget):
    """Widget for displaying MOOSE model tree.

    """
    # Author: subhasis ray
    #
    # Created: Tue Jun 23 18:54:14 2009 (+0530)
    #
    # Updated for moose 2 and multiscale GUI: 2012-12-06
    # Further updated for pymoose2: Tue May 14 14:35:35 IST 2013

    # Ignored are elements to be ignored when building the tree
    ignored = ['/Msgs', '/classes']
    elementInserted = QtCore.pyqtSignal('PyQt_PyObject')
    def __init__(self, *args):
        """A tree widget to display model tree in MOOSE.

        Members:
        
        rootElement: melement
                    root element for the tree.

        SIGNAL:

        elementInserted(melement) emitted when a new element is inserted.

        """
	QtGui.QTreeWidget.__init__(self, *args)
        self.header().hide()
	self.rootElement = moose.element('/')
	self.odict = {}
        self.recreateTree()        

    def setupTree(self, obj, parent, odict):
        """Recursively setup the tree items.
        
        Parameters
        ----------
        obj: melement
        object to be associated with the tree item created.

        parent: MooseTreeItem
        parent item of the current tree item
                
        odict: dict
        dictionary to store melement to tree widget item mapping.
        
        """
        for ii in MooseTreeWidget.ignored:
            if obj.path.startswith(ii):
                return None
	item = MooseTreeItem(parent)
	item.setObject(obj)
	odict[obj] = item
        # TODO: check and verify that this still works with synapses -
        # there have been change in API. - Subha, Fri Sep 19 19:04:35 IST 2014

	for child in obj.children:    
            ch = child
            if child.name in obj.getFieldNames('fieldElementFinfo'):
                ch = obj.getField(child.name)
            for elm in ch:
                self.setupTree(moose.element(elm), item, odict)      
	return item

    def recreateTree(self, root=None):        
        """Clears the current tree and recreates the tree. If root is not
        specified it uses the current `root` otherwise replaces the
        rootElement with specified root element.
        
        Parameter
        ---------
        root: str or melement or vec 
              New root element of the tree. Use current rootElement if `None`
        """        
        self.clear()
        self.odict.clear()
        if root is not None:
            self.rootElement = moose.element(root)
        self.setupTree(self.rootElement, self, self.odict)
        self.setCurrentItem(self.rootElement)
        self.expandToDepth(0)

    def insertElementSlot(self, class_name):
        """Creates an instance of the class class_name and inserts it
        under currently selected element in the model tree."""
        # print 'Inserting element ...', class_name
        current = self.currentItem()
        print 'CLASS NAME:', class_name
        self.insertChildElement(current, class_name)

    def insertChildElement(self, item, class_name, name=''):
        if len(name) == 0:
            name = class_name
        path = '%s/%s' % (item.mobj.path, str(name))
        mclassname = 'moose.%s' % (str(class_name))
        mclass = eval(mclassname)
        obj = mclass(path)
        newitem = MooseTreeItem(item)
        newitem.setObject(obj)
        item.addChild(newitem)
        self.odict[obj] = newitem
        self.elementInserted.emit(obj)
        

    def setCurrentItem(self, item):
        """Overloaded version of QTreeWidget.setCurrentItem

        - adds ability to set item by corresponding moose object.
        """
        if isinstance(item, QtGui.QTreeWidgetItem):
            QtGui.QTreeWidget.setCurrentItem(self, item)
            return        
        mobj = moose.element(item)
        QtGui.QTreeWidget.setCurrentItem(self, self.odict[mobj])

    def updateItemSlot(self, element):
        self.odict[element].updateSlot()
        
def main():
    """Test main: load a model and display the tree for it"""
    model = moose.Neutral('/model')
    moose.loadModel('../Demos/Genesis_files/Kholodenko.g', '/model/Kholodenko')
    # tab = moose.element('/model/Kholodenko/graphs/conc1/MAPK_PP.Co')
    # print tab
    # for t in tab.children:
    #     print t
    app = QtGui.QApplication(sys.argv)
    mainwin = QtGui.QMainWindow()
    mainwin.setWindowTitle('Model tree test')
    tree = MooseTreeWidget()
    tree.recreateTree(root='/model/')
    mainwin.setCentralWidget(tree)
    mainwin.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
    

# 
# mtree.py ends here
