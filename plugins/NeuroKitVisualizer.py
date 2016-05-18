from PyQt4 import QtGui, Qt, QtCore
from PyQt4.QtGui import QLabel
from PyQt4.QtGui import QWidget
from PyQt4.QtGui import QToolBar
from PyQt4.QtGui import QPushButton
from PyQt4.QtGui import QGridLayout
from PyQt4.QtGui import QTextEdit
from PyQt4.QtCore import QMimeData
from PyQt4.QtGui import QDrag
from PyQt4.QtGui import QPixmap
from PyQt4.QtGui import QPainter
from PyQt4.QtGui import QFont
from PyQt4.QtCore import QPoint
import moose
import default
import moogli

class MorphologyEditor(moogli.MorphologyViewer):

    def __init__(self, morphology, width, height, plugin):
        super(MorphologyEditor, self).__init__( morphology
                                              , width
                                              , height
                                              )
        self._timer = QtCore.QTimer(self)
        self.plugin = plugin

    def start(self):
        self._timer.timeout.connect(self.next)
        self._timer.start(0)

    @QtCore.pyqtSlot()
    def show(self):
        self._timer.timeout.connect(self.next)
        super(MorphologyEditor, self).show()
        self._timer.start(0)

    @QtCore.pyqtSlot()
    def hide(self):
        self._timer.stop()
        super(MorphologyEditor, self).hide()

    def next(self):
        self.frame()
        info_id = self.select_info.get_id()
        info_event = self.select_info.get_event_type()
        if info_event == 1:
            self.plugin.mainWindow.objectEditSlot(info_id)
            self.select_info.set_event_type(0)

class MorphologySimulator(moogli.MorphologyViewer):

    def __init__(self, morphology, width, height, plugin):
        super(MorphologySimulator, self).__init__( morphology
                                                 , width
                                                 , height
                                                 )
        self._timer = QtCore.QTimer(self)
        self.plugin = plugin

    def start(self):
        self._timer.timeout.connect(self.next)
        self._timer.start(0)

    @QtCore.pyqtSlot()
    def show(self):
        self._timer.timeout.connect(self.next)
        super(MorphologySimulator, self).show()
        self._timer.start(0)

    @QtCore.pyqtSlot()
    def hide(self):
        self._timer.stop()
        super(MorphologySimulator, self).hide()

    def start_drag(self, info_id):
        # pixmap = QPixmap()
        # painter = QPainter( pixmap )
        # painter.setFont( QFont("Arial") );
        # painter.drawText( QPoint(100, 100), info_id );
        mimeData = QMimeData()
        mimeData.data =("/" + info_id.partition("/")[2].partition("/")[0], moose.element(info_id))
        mimeData.setText(info_id)
        drag = QDrag(self)
        drag.setMimeData(mimeData)
        pixmap = QPixmap("")

        drag.setPixmap(pixmap)
        # drag.setHotSpot(e.pos() - self.rect().topLeft())
        dropAction = drag.start(QtCore.Qt.MoveAction)
        print(" => ", dropAction)
        self.select_info.set_event_type(0)
        self._timer.start(0)
        return

    def next(self):
        self.frame()
        info_id = self.select_info.get_id()
        info_event = self.select_info.get_event_type()
        # print("id => ", info_id)
        # print("type => ", info_event)
        if info_event == 2:
            # self.select_info.set_event_type(-1)
            self._timer.stop()
            self.start_drag(info_id)



class NeuroKitVisualizer(moogli.MorphologyViewer):

    def __init__(self, modelRoot):
        super(NeuroKitVisualizer, self).__init__()
        layout = QGridLayout()
        self.toolbar = self.createToolbar()
        self.textEdit = self.createTextEdit()
        layout.addWidget(self.toolbar, 0, 0, 1, -1)
        layout.addWidget(self.textEdit, 1, 0, -1, -1)

    def slot_showPointMorphology(self):
        """
        This function is called on pressing the detailsButton.
        This function will work iff it is invoked by a signal.
        The sender() function returns None iff the function is
        called explicitly.
        """
        detailsButton = self.sender()
        detailsButton.clicked.disconnect()  # Disconnect all functions
        detailsButton.setText("Detailed")
        detailsButton.clicked.connect(self.slot_showDetailedMorphology)
        print("Write code to show point morphology.")

    def slot_showDetailedMorphology(self):
        """
        This function is called on pressing the detailsButton.
        This function will work iff it is invoked by a signal.
        The sender() function returns None iff the function is
        called explicitly.
        """
        detailsButton = self.sender()
        detailsButton.clicked.disconnect()  # Disconnect all functions
        detailsButton.setText("Point")
        detailsButton.clicked.connect(self.slot_showPointMorphology)
        print("Write code to show detailed morphology.")


    def createTextEdit(self):
        textEdit = QTextEdit(self)
        textEdit.insertPlainText("Aviral Goel")

    def createToolbar(self):
        toolBar = QtGui.QToolBar('Model Settings')

        detailsButton = QPushButton("Detailed")
        detailsButton.clicked.connect(self.slot_showDetailedMorphology)

        propertyButton = QPushButton("Property")
        # propertyButton.clicked.connect(self._propertyTable.show)

        toolBar.addWidget(detailsButton)
        toolBar.addWidget(propertyButton)
        return toolBar
