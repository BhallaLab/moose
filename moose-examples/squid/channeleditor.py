# channeleditor.py --- 
# 
# Filename: channeleditor.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Jul 18 19:06:39 2012 (+0530)
# Version: 
# Last-Updated: Wed Aug  8 16:36:10 2012 (+0530)
#           By: Subhasis Ray
#     Update #: 179
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# A widget for editing Hodgkin-Huxley type ion channels.
# 
# 

# Change log:
# 
# 
# 

# Code:

import sys
from PyQt4 import QtGui, QtCore
from numpy import *
import ast

class GateEditor(QtGui.QWidget):
    """Utility to edit gate equations.
    
    It provides two line edits to enter the alpha and beta equations
    directly.
    """
    def __init__(self, *args):
        QtGui.QWidget.__init__(self, *args)
        self.useVButton = QtGui.QRadioButton('Use V', self)
        self.useVButton.setChecked(True)
        self.useCaButton = QtGui.QRadioButton('Use Ca', self)
        self.useCaButton.setChecked(False)
        self.symbolGroup = QtGui.QGroupBox(self)
        layout = QtGui.QHBoxLayout()        
        # self.symbolGroup.setExclusive(True)
        layout.addWidget(self.useVButton)
        layout.addWidget(self.useCaButton)
        self.symbolGroup.setLayout(layout)
        self.inputPanel = QtGui.QFrame(self)
        self.minVLabel = QtGui.QLabel('Minimum', self)
        self.maxVLabel = QtGui.QLabel('Maximum', self)
        self.divsVLabel = QtGui.QLabel('Number of divisions', self)
        self.minVEdit = QtGui.QLineEdit(self)
        self.maxVEdit = QtGui.QLineEdit(self)
        self.divsVEdit = QtGui.QLineEdit(self)
        self.equation = '(A + B * V) / (C + exp((V + D)/F))'
        self.alphaText = u'Equation for forward rate  \u03B1 '
        self.betaText = u'Equation for backward rate \u03B2 '
        self.minfText = u'Equation for m\u221E '
        self.taumText = u'Equation for \u03C4m '
        self.alphaLabel = QtGui.QLabel(self.minfText, self)
        self.betaLabel = QtGui.QLabel(self.taumText, self)
        self.alphaEdit = QtGui.QLineEdit(self)
        self.betaEdit =  QtGui.QLineEdit(self)
        self.formCombo = QtGui.QComboBox(self)
        self.formCombo.addItem(u'm\u221E - \u03C4m')
        self.formCombo.addItem(u'\u03B1 - \u03B2')
        self.okButton = QtGui.QPushButton('OK', self)
        layout = QtGui.QGridLayout(self.inputPanel)
        layout.addWidget(self.minVLabel, 0, 0)
        layout.addWidget(self.minVEdit, 0, 1)
        layout.addWidget(self.maxVLabel, 0, 3)
        layout.addWidget(self.maxVEdit, 0, 4)
        layout.addWidget(self.divsVLabel, 0, 6)
        layout.addWidget(self.divsVEdit, 0, 7)
        layout.addWidget(self.formCombo, 1, 0, 1, 4)
        layout.addWidget(self.alphaLabel, 2, 0, 1, 2)
        layout.addWidget(self.alphaEdit, 2, 2, 1, 7)
        layout.addWidget(self.betaLabel, 3, 0, 1, 2)
        layout.addWidget(self.betaEdit, 3, 2, 1, 7)
        self.inputPanel.setLayout(layout)
        layout = QtGui.QVBoxLayout(self)
        layout.addWidget(self.symbolGroup)
        layout.addWidget(self.inputPanel)
        layout.addWidget(self.okButton)
        self.setLayout(layout)
        self.connect(self.okButton, QtCore.SIGNAL('clicked()'), self.evalGateExpressions)
        # self.connect(self.useVButton, QtCore.SIGNAL('toggled(bool)'), self.toggleInputPanel)
        self.connect(self.formCombo, QtCore.SIGNAL('currentIndexChanged(int)'), self.switchEquationForm)
        
    def toggleInputPanel(self, on):
        self.inputPanel.setVisible(on)
        self.adjustSize()

    def switchEquationForm(self, index):
        if index == 1:
            self.alphaLabel.setText(self.alphaText)
            self.betaLabel.setText(self.betaText)
        elif index == 0:
            self.alphaLabel.setText(self.minfText)
            self.betaLabel.setText(self.taumText)

    def evalGateExpressions(self):
        """Evaluate the expressions supplied by user to obtain the
        arrays for the gate tables."""
        vmin = float(str(self.minVEdit.text()))
        vmax = float(str(self.maxVEdit.text()))
        vdivs = float(str(self.divsVEdit.text()))
        vrange = linspace(vmin, vmax, vdivs+1)
        a_expr = str(self.alphaEdit.text())
        b_expr = str(self.betaEdit.text())
        a = zeros(vdivs+1)
        b = zeros(vdivs+1)
        if self.useVButton.isChecked():
            symbol = 'V'
        elif self.useCaButton.isChecked():
            symbol = 'Ca'
        code = """for ii in range(len(vrange)):
    %s = vrange[ii]
    a[ii] = %s
    b[ii] = %s
        """ % (symbol, a_expr, b_expr)            
        print code
        tree = compile(code, '<string>', 'exec')
        exec(tree)
        print a, b
        return {'min': vmin,
                'max': vmax,
                'divs': vdivs,
                'a': a,
                'b': b}

        # 0.5 * (1-exp(-(V-10)/10))
if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    QtGui.qApp = app
    editor = GateEditor()
    editor.show()
    sys.exit(app.exec_())

# 
# channeleditor.py ends here
