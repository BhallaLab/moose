# mtoolbutton.py --- 
# 
# Filename: mtoolbutton.py
# Description: Subclass of QToolButton to allow drag and drop
# Author: 
# Maintainer: 
# Created: Fri Jun 14 14:24:11 2013 (+0530)
# Version: 
# Last-Updated: Fri Jun 14 16:28:51 2013 (+0530)
#           By: subha
#     Update #: 89
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
from PyQt4 import QtGui, QtCore
from PyQt4.Qt import Qt

class MToolButton(QtGui.QToolButton):
    """QToolButton subclass with dragEvent reimplemented. It sends the
    text of the ToolButton as the mimedata.

    """
    def __init__(self, *args):
        QtGui.QToolButton.__init__(self, *args)
        self.dragStartPosition = QtCore.QPoint(0,0)

    def mousePressEvent(self, event):
        if event.buttons() & Qt.LeftButton:
            self.dragStartPosition = event.pos()

    def mouseMoveEvent(self, event):        
        if not (event.buttons() & Qt.LeftButton):
            return 
        if (event.pos() - self.dragStartPosition).manhattanLength() < QtGui.QApplication.startDragDistance():
            return
        drag = QtGui.QDrag(self)
        mimeData = QtCore.QMimeData()
        mimeData.setText(self.text())
        drag.setMimeData(mimeData)
        # print '1111', mimeData.text()
        dropAction = drag.exec_(Qt.CopyAction)


class MyWidget(QtGui.QTextBrowser):
    """Class for testing the drag and drop ability of MToolButton"""
    def __init__(self, *args):
        QtGui.QTextBrowser.__init__(self, *args)
        self.dropCount = 0
        self.setPlainText('Drops: %d' % (self.dropCount))
        self.setAcceptDrops(True)
        

    def dragEnterEvent(self, event):
        print '2222', event.mimeData().text()
        if event.mimeData().hasFormat('text/plain'):
            event.acceptProposedAction()
            print '3333 accepted '

    def dragMoveEvent(self, event):
        """This must be reimplemented to accept the event in case of
        QTextBrowser. Not needed in QWidgets in general."""
        print '4444'
        event.acceptProposedAction()

    def dropEvent(self, event):
        print '5555', event.mimeData().text()
        self.dropCount += 1
        self.setPlainText('`%s` dropped: %d times' % (event.mimeData().text(), self.dropCount))
        event.acceptProposedAction()
        QtGui.QTextBrowser.dropEvent(self, event)

def test_main():
    """Test main: see if drag and drop is working"""
    app = QtGui.QApplication(sys.argv)
    mainwin = QtGui.QMainWindow()
    mainwin.setWindowTitle('MTooButton')
    toolbar = QtGui.QToolBar()
    mainwin.addToolBar(toolbar)
    button = MToolButton()
    button.setText('test')
    toolbar.addWidget(button)
    browser = MyWidget(mainwin)
    print browser.acceptDrops()
    mainwin.setCentralWidget(browser)
    mainwin.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    test_main()

# 
# mtoolbutton.py ends here
