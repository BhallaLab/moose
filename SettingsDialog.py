
# -*- coding: utf-8 -*-
from __future__ import print_function

"""Dialog for settings. Currently only plot settings are supported
"""

__author__      =   "Aviral Goel"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "Aviral Goel"
__email__       =   "goel.aviral@gmail.com"
__status__      =   "Development"


import sys
import os
from PyQt4 import QtGui, Qt
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QLabel
from PyQt4.QtGui import QComboBox
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QTabWidget

class SettingsWidget(QTabWidget):
    def __init__( self
                , plotFieldMap
                , parent        = None
                ):
        super(SettingsWidget, self).__init__(parent)

        self.plotFieldMap = plotFieldMap

        self.addTab(self.plotSettingsPage(),"Plot Settings");
        self.addTab(self.plotSettingsPage(),"Other Settings");

    def plotSettingsPage(self):
        page = QWidget()
        layout = QGridLayout()
        page.setLayout(layout)
        index = 0
        for key, values in self.plotFieldMap.iteritems() :
            label = QLabel(key, page)
            combo = QComboBox(page)
            for value in values:
                combo.addItem(value)
            layout.addWidget(label,index,0, Qt.Qt.AlignRight)
            layout.addWidget(combo,index,1, Qt.Qt.AlignLeft)
            index += 1
        return page

        # combo.move(50, 50)
        # self.lbl.move(50, 150)

        # combo.activated[str].connect(self.onActivated)

        # self.setGeometry(300, 300, 300, 200)
        # self.setWindowTitle('QtGui.QComboBox')
        # self.show()


def main():
    app = QtGui.QApplication(sys.argv)
    window = QtGui.QMainWindow()
    dialog = SettingsWidget({
                            'LeakyIaF':['Vm'],
                            'Compartment':['Vm','Im'],
                            'HHChannel':['Ik','Gk'],
                            'ZombiePool':['n','conc'],
                            'ZombieBufPool':['n','conc'],
                            'HHChannel2D':['Ik','Gk'],
                            'CaConc':['Ca']
                            }
                           )
    dialog.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
