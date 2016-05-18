from PyQt4 import Qt, QtCore, QtGui
from PyQt4.QtGui import *
import os

APPLICATION_BACKGROUND_PATH = os.path.join( os.path.dirname(os.path.realpath(__file__))
                                          , "icons/moose_icon_large.png"
                                          )


class MdiArea(QMdiArea):
    def __init__(self):
        super(MdiArea, self).__init__()
        self.backgroundImage = QImage(APPLICATION_BACKGROUND_PATH)
        self.background      = None

    def resizeEvent(self,event):
        # http://qt-project.org/faq/answer/when_setting_a_background_pixmap_for_a_widget_it_is_tiled_if_the_pixmap_is_
        self.background = QImage(event.size(), QImage.Format_ARGB32_Premultiplied)
        painter = QPainter(self.background)
        painter.fillRect(self.background.rect(), QColor(243, 239, 238, 255))
        scaled = self.backgroundImage.scaled(event.size() , QtCore.Qt.KeepAspectRatio)
        scaledRect = scaled.rect()
        scaledRect.moveCenter(self.background.rect().center())
        painter.drawImage(scaledRect, scaled)
        self.setBackground(QBrush(self.background))
        super(MdiArea, self).resizeEvent(event)
