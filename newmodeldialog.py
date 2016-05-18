import os
from PyQt4 import QtGui, QtCore,Qt
import config
from mplugin import MoosePluginBase, EditorBase, EditorWidgetBase, PlotBase, RunBase
import re

class DialogWidget(QtGui.QDialog):
    def __init__(self,parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.warning = None
        self._currentRadioButton ="kkit"
        self.layout = QtGui.QGridLayout()
        self.modelPathLabel = QtGui.QLabel('Model Name')
        self.modelPathEdit =  QtGui.QLineEdit('')
        self.layout.addWidget(self.modelPathLabel, 0, 0)
        self.layout.addWidget(self.modelPathEdit, 0, 1,1,1)
        # self.defaultRadio = QtGui.QRadioButton('default')
        # #self.defaultRadio.setChecked(True);
        # self.kkitRadio = QtGui.QRadioButton('kkit')
        # self.kkitRadio.setChecked(True)
        # self.defaultRadio.toggled.connect(lambda : self.setcurrentRadioButton('default'))
        # self.kkitRadio.toggled.connect(lambda : self.setcurrentRadioButton('kkit'))
        # layout.addWidget(self.defaultRadio,2,1)
        # layout.addWidget(self.kkitRadio,2,0)
        self.hbox = QtGui.QHBoxLayout()
        self.buttonBox = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Ok)
        self.connect(self.buttonBox, QtCore.SIGNAL('accepted()'), self.validateAccept)
        self.hbox.addWidget(self.buttonBox,1)

        self.buttonBox1 = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Cancel)
        self.connect(self.buttonBox1, QtCore.SIGNAL('rejected()'), self.Cancel)
        self.hbox.addWidget(self.buttonBox1,0)
        #self.hbox.setBackgroundColor(Blue)
        #self.modelPathEdit.returnPressed.connect(self.validateAccept)
        # self.connect(self.buttonBox, QtCore.SIGNAL('rejected()'), self.reject)
        #self.hbox.addStretch(1)
        self.layout.addLayout(self.hbox,1,1)
        self.setLayout(self.layout)

    def Cancel (self):
        self.close()

    def validateAccept(self):
        text = str(self.modelPathEdit.text())
        self.layout.removeWidget(self.warning)
        #replace / to _
        text = text.replace('/','_')

        #print(self.layout.widgets())
        if len(text) == 0:
            self.warning = QtGui.QLabel("Model name cannot be empty!")
            self.layout.addWidget(self.warning, 1, 0, 1, -1)
        elif not re.match("^[a-zA-Z]+.*",text):
            self.warning = QtGui.QLabel("Start special characters not allowed!")
            self.layout.addWidget(self.warning, 1, 0, 1, -1)
        else:
            self.accept()
        return False

    def getcurrentRadioButton(self):
        return self._currentRadioButton

if __name__ == '__main__':
    app =QtGui.QApplication([])
    widget = DialogWidget()
    widget.setWindowTitle('New Model')
    widget.setMinimumSize(400, 200)
    widget.show()
    app.exec_()


