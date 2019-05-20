# objectedit.py ---
#
# Filename: objectedit.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Wed Jun 30 11:18:34 2010 (+0530)
# Version:
# Last-Updated: Fri Feb 01 11:05:59 2017 (+0530)
#           By: Harsha
#     Update #: 
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# This code is for a widget to edit MOOSE objects. We can now track if
# a field is a Value field and make it editable accordingly. There
# seems to be no clean way of determining whether the field is worth
# plotting (without a knowledge of the model/biology there is no way
# we can tell this). But we can of course check if the field is a
# numeric one.
#
#

# Change log:
#
# Wed Jun 30 11:18:34 2010 (+0530) - Originally created by Subhasis
# Ray, the model and the view
#
# Modified/adapted to dh_branch by Chaitanya/Harsharani
#
# Thu Apr 18 18:37:31 IST 2013 - Reintroduced into multiscale GUI by
# Subhasis
#
# Fri Apr 19 15:05:53 IST 2013 - Subhasis added undo redo
# feature. Create ObjectEditModel as part of ObjectEditView.
# Tue Mar 7 16:10:54 IST 2017 - Harsha now Pool or BufPool can be interchangable
# by setting/unsetting isbuffered field
# Fri May 17 23:45:59 2017 (+0530) - Harsha added, notes header,
# Kd is calculated for the second order reaction and value is displayed
# Tue Jun 18 12:10:54 IST 2018 - Harsha now group boundary color can be editable from the object editor
# Mon Sep 10 16:21:00 IST 2018 - When name is edited, the editorTitle gets updated
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
import PyQt4
from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4.QtGui import QTextEdit
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QVBoxLayout
from PyQt4.QtGui import QSizePolicy
from PyQt4.QtCore import QMargins
from PyQt4.QtGui import QSplitter
import sys
from collections import deque
import traceback

#sys.path.append('../python')
import moose
import defaults
import config
#from plugins.kkitUtil import getColor
from moose.chemUtil.chemConnectUtil import getColor

#these fields will be ignored
extra_fields = ['this',
                'me',
                'parent',
                'path',
                'children',
                'linearSize',
                'objectDimensions',
                'lastDimension',
                'localNumField',
                'pathIndices',
                'msgOut',
                'msgIn',
                'diffConst',
                'speciesId',
                'Coordinates',
                'neighbors',
                'DiffusionArea',
                'DiffusionScaling',
                'x',
                'x0',
                'x1',
                'dx',
                'nx',
                'y',
                'y0',
                'y1',
                'dy',
                'ny',
                'z',
                'z0',
                'z1',
                'dz',
                'nz',
                'coords',
                'isToroid',
                'preserveNumEntries',
                # 'numKm',
                'numSubstrates',
                'concK1',
                'meshToSpace',
                'spaceToMesh',
                'surface',
                'method',
                'alwaysDiffuse',
                'numData',
                'numField',
                'valueFields',
                'sourceFields',
                'motorConst',
                'destFields',
                'dt',
                'tick',
	        'idValue',
		'index',
		'fieldIndex'
                ]


class ObjectEditModel(QtCore.QAbstractTableModel):
    """Model class for editing MOOSE elements. This is not to be used
    directly, except that its undo and redo slots should be connected
    to by the GUI actions for the same.

    SIGNALS:

    objectNameChanged(PyQt_PyObject): when a moose object's name is
    changed, this signal is emitted with the object as argument. This
    can be captured by widgets that display the object name.

    dataChanged: emitted when any data is changed in the moose object

    """
    objectNameChanged = QtCore.pyqtSignal('PyQt_PyObject')
    # dataChanged = QtCore.pyqtSignal('PyQt_PyObject')
    def __init__(self, datain, headerdata=['Field','Value'], undolen=100, parent=None, *args):
        QtCore.QAbstractTableModel.__init__(self, parent, *args)
        self.fieldFlags = {}
        self.fields = []
        self.mooseObject = datain
        self.headerdata = headerdata
        self.undoStack = deque(maxlen=undolen)
        self.redoStack = deque(maxlen=undolen)
        self.checkState_ = False

        for fieldName in self.mooseObject.getFieldNames('valueFinfo'):
            if fieldName in extra_fields :
                continue

            value = self.mooseObject.getField(fieldName)
            self.fields.append(fieldName)
        #harsha: For signalling models will be pulling out notes field from Annotator
        #        can updates if exist for other types also
        if (isinstance (self.mooseObject,moose.ChemCompt) or \
            isinstance(self.mooseObject,moose.ReacBase)  or \
            isinstance(moose.element(moose.element(self.mooseObject).parent),moose.EnzBase) \
           ):
            pass
        else:
             self.fields.append("Color")
        
        flag = QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable | QtCore.Qt.ItemIsEditable
        self.fieldFlags[fieldName] = flag

        if ( isinstance(self.mooseObject, moose.ReacBase) ) :
            self.fields.append("Kd")
        flag = QtCore.Qt.ItemIsEnabled 
        self.fieldFlags[fieldName] = flag

    def rowCount(self, parent):
        return len(self.fields)

    def columnCount(self, parent):
        return len(self.headerdata)

    def setData(self, index, value, role=QtCore.Qt.EditRole):
        if not index.isValid() or index.row () >= len(self.fields) or index.column() != 1:
            return False
        field = self.fields[index.row()]
        if (role == QtCore.Qt.CheckStateRole):
            if (index.column() == 1):
                self.checkState_ = value
                return True

        else:
            value = str(value.toString()).strip() # convert Qt datastructure to Python string
            if len(value) == 0:
                return False
            if field == "Notes":
                field = "notes"
                ann = moose.Annotator(self.mooseObject.path+'/info')
                oldValue = ann.getField(field)
                value = type(oldValue)(value)
                ann.setField(field,value)
                self.undoStack.append((index,oldValue))
            elif field == "vector":
                for ch in ['[',']']:
                    if ch in value:
                        value = value.replace(ch," ")
                value = value.replace(",", " ")
                valuelist = []
                if value.find(',') != -1:
                    valuelist = value.split(",")
                elif value.find(' ') != -1:
                    valuelist = value.split(" ")
                else:
                    valuelist = value
                vectorlist = []
                for d in valuelist:
                    try:
                        float(d)
                        vectorlist.append(float(d))
                    except:
                        pass
                from numpy import array
                a = array( vectorlist )
                self.mooseObject.setField(field, a)
            
            else:
                oldValue = self.mooseObject.getField(field)
                if field != "isBuffered":
                    value = type(oldValue)(value)
                    self.mooseObject.setField(field, value)
                else:
                    if self.mooseObject.className == "ZombiePool" or self.mooseObject.className =="ZombieBufPool":
                        QtGui.QMessageBox.warning(None,'Solver is set, Could not set the value','\n Unset the solver by clicking \n run widget -> Preferences -> Exponential Euler->Apply')
                    else:
                        if value.lower() in ("yes", "true", "t", "1"):
                            self.mooseObject.setField(field, True)
                        else:
                            self.mooseObject.setField(field, False)
                self.undoStack.append((index, oldValue))
            if field == 'name':
                self.emit(QtCore.SIGNAL('objectNameChanged(PyQt_PyObject)'), self.mooseObject)
            return True

        self.dataChanged.emit(index, index)
        return True

    def undo(self):
        print ('Undo')
        if len(self.undoStack) == 0:
            raise Info('No more undo information')
        index, oldvalue, = self.undoStack.pop()
        field = self.fields[index.row()]
        currentvalue = self.mooseObject.getField(field)
        oldvalue = type(currentvalue)(oldvalue)
        self.redoStack.append((index, str(currentvalue)))
        self.mooseObject.setField(field, oldvalue)
        if field == 'name':
            self.objectNameChanged.emit(self.mooseObject)
        self.emit(QtCore.SIGNAL('dataChanged(const QModelIndex&, const QModelIndex&)'), index, index)

    def redo(self):
        if len(self.redoStack) ==0:
            raise Info('No more redo information')
        index, oldvalue, = self.redoStack.pop()
        currentvalue = self.mooseObject.getField(self.fields[index.row()])
        self.undoStack.append((index, str(currentvalue)))
        self.mooseObject.setField(self.fields[index.row()], type(currentvalue)(oldvalue))
        if field == 'name':
            self.emit(QtCore.SIGNAL('objectNameChanged(PyQt_PyObject)'), self.mooseObject)
        self.emit(QtCore.SIGNAL('dataChanged(const QModelIndex&, const QModelIndex&)'), index, index)

    def flags(self, index):
        flag =  QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable
        #flag = QtCore.Qt.NoItemFlags
        if not index.isValid():
            return None
        # Replacing the `outrageous` up stuff with something sensible
        field = self.fields[index.row()]
        newstr = field[0]
        newstr = newstr.upper()
        field_string = newstr + field[1:]
        setter = 'set%s' %(field_string)
        #setter = 'set_%s' % (self.fields[index.row()])
        #print " from Object setter",setter, "object",self.mooseObject, " ",self.mooseObject.getFieldNames('destFinfo');
        if index.column() == 1:
            # if field == "Color":
            #     flag = QtCore.Qt.ItemIsEnabled
            if field == "Notes":
                ann = moose.Annotator(self.mooseObject.path+'/info')
                if setter in ann.getFieldNames('destFinfo'):
                    flag |= QtCore.Qt.ItemIsEditable
            
            if isinstance(self.mooseObject, moose.PoolBase) or isinstance(self.mooseObject,moose.Function): 
                if field == 'volume' or field == 'expr':
                    pass
                elif setter in self.mooseObject.getFieldNames('destFinfo'):
                    flag |= QtCore.Qt.ItemIsEditable
            else:
                if setter in self.mooseObject.getFieldNames('destFinfo'):
                    flag |= QtCore.Qt.ItemIsEditable

            #if field == "Notes":
            #    flag |= QtCore.Qt.ItemIsEditable

        # !! Replaced till here

        return flag

    def data(self, index, role):
        ret = None
        field = self.fields[index.row()]
        if index.column() == 0 and role == QtCore.Qt.DisplayRole:
            try:
                ret = QtCore.QVariant(QtCore.QString(field)+' ('+defaults.FIELD_UNITS[field]+')')
            except KeyError:
                ret = QtCore.QVariant(QtCore.QString(field))
        elif index.column() == 1:
            if role==QtCore.Qt.CheckStateRole:
                if ((str(field) == "plot Conc") or (str(field) == "plot n") ):
                    # print index.data(QtCore.Qt. ), str(field)
                    return self.checkState_
            elif (role == QtCore.Qt.DisplayRole or role == QtCore.Qt.EditRole):
                try:
                    if (str(field) =="Color" ):
                        return QtGui.QPushButton("Press Me!")
                    if (str(field) =="Kd" ):
                        #ret = self.mooseObject.getField(str(field))
                        Kd = 0
                        
                        if self.mooseObject.className == "ZombieReac" or self.mooseObject.className == "Reac":
                            if self.mooseObject.numSubstrates > 1 or self.mooseObject.numProducts > 1:
                                if self.mooseObject.Kf != 0:
                                    Kd = self.mooseObject.Kb/self.mooseObject.Kf

                            #Kd = QtCore.QVariant(QtCore.QString(str(ret)))
                        ret = QtCore.QVariant(QtCore.QString(str(Kd)))
                    if ( (str(field) != "Notes") and (str(field) != "className") and (str(field) != "Kd")):
                        ret = self.mooseObject.getField(str(field))
                        ret = QtCore.QVariant(QtCore.QString(str(ret)))
                    elif(str(field) == "className"):
                        ret = self.mooseObject.getField(str(field))
                        if 'Zombie' in ret:
                            ret = ret.split('Zombie')[1]
                        ret = QtCore.QVariant(QtCore.QString(str(ret)))
                    elif(str(field) == "Notes"):
                        astr = self.mooseObject.path+'/info'
                        mastr = moose.Annotator(astr)
                        ret = (mastr).getField(str('notes'))
                        ret = QtCore.QVariant(QtCore.QString(str(ret)))
                except ValueError:
                    ret = None
        return ret

    def headerData(self, col, orientation, role):
        if orientation == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:
            return QtCore.QVariant(self.headerdata[col])
        return QtCore.QVariant()

class ObjectEditView(QtGui.QTableView):
    """View class for object editor.

    This class creates an instance of ObjectEditModel using the moose
    element passed as its first argument.

    undolen - specifies the size of the undo stack. By default set to
    OBJECT_EDIT_UNDO_LENGTH constant in defaults.py. Specify something smaller if
    large number of objects are likely to be edited.

    To enable undo/redo conect the corresponding actions from the gui
    to view.model().undo and view.model().redo slots.
    """
    def __init__(self, mobject, undolen=defaults.OBJECT_EDIT_UNDO_LENGTH, parent=None):
        QtGui.QTableView.__init__(self, parent)
        #self.setEditTriggers(self.DoubleClicked | self.SelectedClicked | self.EditKeyPressed)
        vh = self.verticalHeader()
        vh.setVisible(False)
        hh = self.horizontalHeader()
        hh.setStretchLastSection(True)
        self.setAlternatingRowColors(True)
        self.resizeColumnsToContents()
        self.setModel(ObjectEditModel(mobject, undolen=undolen))
        self.colorButton = QtGui.QPushButton()
        self.colorDialog = QtGui.QColorDialog()
        self.textEdit    = QTextEdit()
        try:
            notesIndex = self.model().fields.index("Notes")
            self.setIndexWidget(self.model().index(notesIndex,1), self.textEdit)
            info = moose.Annotator(self.model().mooseObject.path+'/info')
            self.textEdit.setText(QtCore.QString(info.getField('notes')))
            self.setRowHeight(notesIndex, self.rowHeight(notesIndex) * 3)

            # self.colorDialog.colorSelected.connect(
            #     lambda color:
            #
            # self.setColor(getColor(self.model().mooseObject.path+'/info')[1])
        except:
            pass


        try:
            colorIndex = self.model().fields.index("Color")
            self.colorButton.clicked.connect(self.colorDialog.show)
            self.colorButton.setFocusPolicy(PyQt4.QtCore.Qt.NoFocus)
            self.colorDialog.colorSelected.connect(
                lambda color: self.colorButton.setStyleSheet(
                            "QPushButton {"
                        +   "background-color: {0}; color: {0};".format(color.name())
                        +   "}"
                                                                             )
                                                                    )
            self.setIndexWidget(self.model().index(colorIndex,1), self.colorButton)
            # self.colorDialog.colorSelected.connect(
            #     lambda color:
            #
            self.setColor(getColor(self.model().mooseObject.path+'/info')[1])
        except:
            pass
        print ('Created view with %s' %(mobject))

    def setColor(self, color):
        self.colorButton.setStyleSheet(
                    "QPushButton {"
                +   "background-color: {0}; color: {0};".format(color)
                +   "}"
                                      )
        self.colorDialog.setCurrentColor(color)

    def dataChanged(self, tl, br):
        QtGui.QTableView.dataChanged(self, tl, br)
        self.viewport().update()

class ObjectEditDockWidget(QtGui.QDockWidget):
    """A dock widget whose title is set by the current moose
    object. Allows switching the moose object. It stores the created
    view in a dict for future use.

    TODO possible performance issue: storing the views (along with
    their models) ensures the undo history for each object is
    retained. But without a limit on the number of views stored, it
    will be wasteful on memory.

    """
    objectNameChanged = QtCore.pyqtSignal('PyQt_PyObject')
    colorChanged = QtCore.pyqtSignal(object, object)
    def __init__(self, mobj='/', parent=None, flags=None):
        QtGui.QDockWidget.__init__(self, parent=parent)
        mobj = moose.element(mobj)
        #self.view = view = ObjectEditView(mobj)
        self.view = view = ObjectEditView(mobj)
        self.view_dict = {mobj: view}
        base = QWidget()
        layout = QVBoxLayout()
        base.setLayout(layout)
        layout.addWidget(self.view)
        layout.addWidget(QTextEdit())
        self.setWidget(base)
        self.setWindowTitle('Edit: %s' % (mobj.path))
        # self.view.colorDialog.colorSelected.connect(self.colorChangedEmit)

    # def clearDict(self):
    #     self.view_dict.clear()

    def setObject(self, mobj):
        element = moose.element(mobj)
        try:
            view = self.view_dict[element]
        except KeyError:
            view = ObjectEditView(element)
            self.view_dict[element] = view
            view.model().objectNameChanged.connect(self.emitObjectNameChanged)
        view.colorDialog.colorSelected.connect(lambda color: self.colorChanged.emit(element, color))
        textEdit = QTextEdit()
        view.setSizePolicy( QSizePolicy.Ignored
                          , QSizePolicy.Ignored
                          )
        textEdit.setSizePolicy(QSizePolicy.Maximum, QSizePolicy.Maximum)
        base = QSplitter()
        base.setOrientation(PyQt4.QtCore.Qt.Vertical)
        layout = QVBoxLayout()
        layout.addWidget(view)#, 0, 0)
        lineedit = QtGui.QLineEdit("Notes:")
        lineedit.setReadOnly(True)
        layout.addWidget(lineedit)
        
        if ( isinstance(mobj, moose.PoolBase)
           or isinstance(mobj,moose.ReacBase)
           or isinstance(mobj,moose.EnzBase)
           ) :
            info = moose.Annotator(mobj.path +'/info')
            textEdit.setText(QtCore.QString(info.getField('notes')))
            textEdit.textChanged.connect(lambda : info.setField('notes', str(textEdit.toPlainText())))
            layout.addWidget(textEdit)#,1,0)

            # self.setRowHeight(notesIndex, self.rowHeight(notesIndex) * 3)
        base.setLayout(layout)
        # base.setSizes( [ view.height()
        #                , base.height() - view.height()
        #                ]
        #              )
        # print("a =>", view.height())
        # print("b =>", base.height())


        # layout.setStretch(0,3)
        # layout.setStretch(1,1)
        # layout.setContentsMargins(QMargins(0,0,0,0))
        self.setWidget(base)
        self.setWindowTitle('Edit: %s' % (element.path))
        view.update()

    def emitObjectNameChanged(self, mobj):
        self.objectNameChanged.emit(mobj)
        self.setWindowTitle('Edit:%s'%(mobj.path))
def main():
    app = QtGui.QApplication(sys.argv)
    mainwin = QtGui.QMainWindow()
    c = moose.Compartment("test")
    view = ObjectEditView(c, undolen=3)
    mainwin.setCentralWidget(view)
    action = QtGui.QAction('Undo', mainwin)
    action.setShortcut('Ctrl+z')
    action.triggered.connect(view.model().undo)
    mainwin.menuBar().addAction(action)
    action = QtGui.QAction('Redo', mainwin)
    action.setShortcut('Ctrl+y')
    action.triggered.connect(view.model().redo)
    mainwin.menuBar().addAction(action)
    mainwin.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
# ojectedit.py ends here
