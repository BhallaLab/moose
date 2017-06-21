
__author__      =   "HarshaRani"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "HarshaRani"
__email__       =   "hrani@ncbs.res.in"
__status__      =   "Development"
__updated__     =   "Mar 7 2017"

#import sys
#sys.path.insert(0, '/home/harsha/trunk/gui')
import config
from PyQt4 import QtGui, QtCore, Qt
from moose import *
from PyQt4.QtGui import QPixmap, QImage, QGraphicsPixmapItem
from constants import *
from os import path

class KineticsDisplayItem(QtGui.QGraphicsWidget):
    """Base class for display elemenets in kinetics layout"""
    name = ITEM
    defaultFontName = "Helvetica"
    defaultFontSize = 10
    def __init__(self, mooseObject, parent=None):
        QtGui.QGraphicsObject.__init__(self,parent)
        self.mobj = mooseObject
        self.gobj = None
        self.pressed = False
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable,True)
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable, True)
        self.setAcceptHoverEvents(True)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        if QtCore.QT_VERSION >= 0x040600:
            self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges,1)
        self.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, False)

    def getParentMooseObject(self):
        return moose.element(self.mobj).parent

    def getParentGraphicsObject(self):
        return self.parentItem()

    def setDisplayProperties(self, dinfo):
        self.setGeometry(dinfo.x, dinfo.y)
   
    def paint(self, painter=None, option=None, widget = None):
        #If item is selected
        if self.hasFocus() or self.isSelected():
            painter.setPen(QtGui.QPen(QtGui.QPen(QtCore.Qt.black, 1.8,Qt.Qt.DashLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin)))
            painter.drawRect(self.boundingRect())
    # def mouseDoubleClickEvent(self, event):
    #     self.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),element(self.mobj))
            
    def itemChange(self,change,value):
        #pass
        #if change == QtGui.QGraphicsItem.ItemPositionChange:
            #print "itemChange ---->",self.mobj
        #    self.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.mobj)
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
           self.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),element(self.mobj))
        return QtGui.QGraphicsItem.itemChange(self,change,value)

class FuncItem(KineticsDisplayItem):
    name = ITEM
    """Class for displaying Functions"""    
    #fontMetrics = None
    font =QtGui.QFont(KineticsDisplayItem.defaultFontName)
    font.setPointSize(KineticsDisplayItem.defaultFontSize)
    fontMetrics = QtGui.QFontMetrics(font)
    def __init__(self, mobj, parent):
        super(FuncItem, self).__init__(mobj, parent)
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable, True)
        #self.funcImage = QImage('icons/classIcon/Function.png').scaled(15,33)
        iconmap_file = (path.join(config.settings[config.KEY_ICON_DIR], 'classIcon/Function.png'))
        self.funcImage = QImage(iconmap_file).scaled(15,33)
        self.bg = QtGui.QGraphicsRectItem(self)
        self.bg.setAcceptHoverEvents(True)
        self.gobj = QtGui.QGraphicsPixmapItem(QtGui.QPixmap.fromImage(self.funcImage),self.bg)
        #self.gobj = QtGui.QGraphicsPixmapItem(QtGui.QPixmap('../icons/classIcon/Function.png'))
        self.gobj.setAcceptHoverEvents(True)
        self.gobj.mobj = self.mobj
        classname = self.mobj.className
        funcdoc = (element(self.mobj.path)).expr
        self.gobj.setToolTip(funcdoc)
        #self.bg.setPen(Qt.QColor(100,100,10,10))
        
    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        #With Respect to BufPool (as parent which is in old genesis) then function are placed at 0,30 (which is below the BufPool)
        #else if droped from the GUI then placed wrt position
        #self.setGeometry(0, 30,self.gobj.boundingRect().width(),self.gobj.boundingRect().height())
        #self.setGeometry(x,y)
        poolt = ["ZombieBufPool","BufPool","ZombiePool","Pool"]
        if self.gobj.mobj.parent.className in poolt:
            self.setGeometry(0, 30,self.gobj.boundingRect().width(),self.gobj.boundingRect().height())
        else:
            self.setGeometry(x,y,self.gobj.boundingRect().width(),self.gobj.boundingRect().height())
        self.bg.setBrush(QtGui.QBrush(bgcolor))
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable,False)
    def refresh(self,scale):
    #     fontsize = PoolItem.defaultFontsize*scale
    #     font =QtGui.QFont("Helvetica")
    #     font.setPointSize(fontsize)
    #     self.gobj.setFont(font)
        pass
    def boundingRect(self):
        ''' reimplimenting boundingRect for redrawning '''
        return QtCore.QRectF(0,0,self.gobj.boundingRect().width(),self.gobj.boundingRect().height())

    def updateSlot(self):
        pass
        # #This func will adjust the background color with respect to text size
        # self.gobj.setText(self.mobj.name)
        # self.bg.setRect(0, 0, self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '), self.gobj.boundingRect().height())
    
    def updateColor(self,bgcolor):
        #self.bg.setBrush(QtGui.QBrush(QtGui.QColor(bgcolor)))
        pass

    def updateRect(self,ratio):
        pass
        # width = self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  ')
        # height = self.gobj.boundingRect().height()
        # adjustw = width*ratio
        # adjusth = height*ratio
        # self.bgColor.setRect(width/2-abs(adjustw/2),height/2-abs(adjusth/2),adjustw, adjusth)
        #self.bg.setRect(0,0,self.gobj.boundingRect().width()*ratio+PoolItem.fontMetrics.width('  '), self.gobj.boundingRect().height()*ratio)
    def returnColor(self):
        return (self.bg.brush().color())

    def updateValue(self,gobj):
        self.gobj.setToolTip(funcdoc)
        # self._gobj = gobj
        # #if type(self._gobj) is moose.ZombiePool:
        # if (isinstance(self._gobj,PoolBase)):
        #     self._conc = self.mobj.conc
        #     self._n    = self.mobj.n
        #     doc = "Conc\t: "+str(self._conc)+"\nn\t: "+str(self._n)
        #     self.gobj.setToolTip(doc)


class PoolItem(KineticsDisplayItem):
    name = ITEM
    """Class for displaying pools. Uses a QGraphicsSimpleTextItem to
    display the name."""    
    #fontMetrics = None
    font = QtGui.QFont(KineticsDisplayItem.defaultFontName)
    font.setPointSize(KineticsDisplayItem.defaultFontSize)
    fontMetrics = QtGui.QFontMetrics(font)
    def __init__(self, mobj, parent):
        KineticsDisplayItem.__init__(self, mobj, parent)
        self.bg = QtGui.QGraphicsRectItem(self)
        self.bg.setAcceptHoverEvents(True)
        self.gobj = QtGui.QGraphicsSimpleTextItem(self.mobj.name, self.bg)
        self.gobj.mobj = self.mobj
        classname = self.mobj.className
        # classname = 'PoolBase'
        # doc = moose.element('/classes/%s' % (classname)).docs
        # print "docs ",self.gobj.mobj, " ",doc
        # doc = doc.split('Description:')[-1].split('Name:')[0].strip()
        self._conc = self.mobj.conc
        self._n    = self.mobj.n
        doc = "Conc\t: "+str(self._conc)+"\nn\t: "+str(self._n)
        self.gobj.setToolTip(doc)
        self.gobj.setFont(PoolItem.font)
        if not PoolItem.fontMetrics:
            PoolItem.fontMetrics = QtGui.QFontMetrics(self.gobj.font())
        self.bg.setRect(0, 
                        0, 
                        self.gobj.boundingRect().width()
                        +PoolItem.fontMetrics.width('  '), 
                        self.gobj.boundingRect().height())
        self.bg.setPen(Qt.QColor(0,0,0,0))
        self.gobj.setPos(PoolItem.fontMetrics.width(' '), 0)
    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(x, y,self.gobj.boundingRect().width()
                        +PoolItem.fontMetrics.width(''), 
                        self.gobj.boundingRect().height())
        self.bg.setBrush(QtGui.QBrush(bgcolor))
    
    def refresh(self,scale):
        fontsize = KineticsDisplayItem.defaultFontSize*scale
        font = QtGui.QFont(KineticsDisplayItem.defaultFontName)
        if (fontsize < 1):
            fontsize = self.gobj.font().pointSize()
        font.setPointSize(fontsize)
        #self.gobj.setFont(PoolItem.font)
        self.gobj.setFont(font)

    def boundingRect(self):
        ''' reimplimenting boundingRect for redrawning '''
        return QtCore.QRectF(0,0,self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '),self.gobj.boundingRect().height())

    def updateSlot(self):
        #This func will adjust the background color with respect to text size
        self.gobj.setText(self.mobj.name)
        self.bg.setRect(0, 0, self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '), self.gobj.boundingRect().height())
        self.setGeometry(self.pos().x(),self.pos().y(),self.gobj.boundingRect().width()
                        +PoolItem.fontMetrics.width(''), 
                        self.gobj.boundingRect().height())
    
    def updateColor(self,bgcolor):
        self.bg.setBrush(QtGui.QBrush(QtGui.QColor(bgcolor)))
        #pass

    def updateRect(self,ratio=1.0):
        width = self.gobj.boundingRect().width()+PoolItem.fontMetrics.width(' ')
        height = self.gobj.boundingRect().height()
        adjustw = width*ratio
        adjusth = height*ratio
        self.bg.setRect(width/2-abs(adjustw/2),height/2-abs(adjusth/2),adjustw, adjusth)
        #self.bg.setRect(0,0,self.gobj.boundingRect().width()*ratio+PoolItem.fontMetrics.width('  '), self.gobj.boundingRect().height()*ratio)
    
    def returnColor(self):
        return (self.bg.brush().color())

    def updateValue(self,gobj):
        self._gobj = gobj
        #if type(self._gobj) is moose.ZombiePool:
        if (isinstance(self._gobj,PoolBase)):
            self._conc = self.mobj.conc
            self._n    = self.mobj.n
            doc = "Conc\t: "+str(self._conc)+"\nn\t: "+str(self._n)
            self.gobj.setToolTip(doc)

class PoolItemCircle(PoolItem):
    name = ITEM
    def __init__(self,*args,**kwargs):
        PoolItem.__init__(self, *args, **kwargs)
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable,True)
        self.bgColor = QtGui.QGraphicsEllipseItem(self)
        self.bgColor.setFlag(QtGui.QGraphicsItem.ItemStacksBehindParent,True)
        self.bgColor.setRect(((self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '))/2)-5,self.gobj.boundingRect().height()/2-5,10,10)
    
    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        self.setGeometry(x, y,self.gobj.boundingRect().width()
                        +PoolItem.fontMetrics.width('  '), 
                        self.gobj.boundingRect().height())
        self.bgColor.setBrush(QtGui.QBrush(QtGui.QColor(bgcolor.red(),bgcolor.green(),bgcolor.blue(),255)))
        
    def updateRect(self,ratio):
        width = self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  ')
        height = self.gobj.boundingRect().height()
        adjustw = width*ratio
        adjusth = height*ratio
        self.bgColor.setRect(width/2-abs(adjustw/2),height/2-abs(adjusth/2),adjustw, adjusth)
        self.updateValue(self.gobj)
     
    def returnEllispeSize(self):
        self.bgColor.setRect(((self.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '))/2)-5,self.gobj.boundingRect().
            height()/2-5,10,10)
    
    def refresh(self,scale):
        fontsize = KineticsDisplayItem.defaultFontSize*scale
        font =QtGui.QFont(KineticsDisplayItem.defaultFontName)
        if (fontsize < 1):
            fontsize = self.gobj.font().pointSize()
        font.setPointSize(fontsize)
        self.gobj.setFont(font)    

    def MooseRef(self):
        return self.gobj.mobj

class TableItem(KineticsDisplayItem):
    defaultWidth = 30
    defaultHeight = 30
    defaultPenWidth = 2
    name = ITEM
    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)

        points = [QtCore.QPointF(0,TableItem.defaultWidth/2),
                  QtCore.QPointF(TableItem.defaultHeight/2-2,0),
                  QtCore.QPointF(TableItem.defaultWidth/2+2,0),
                  QtCore.QPointF(TableItem.defaultWidth,TableItem.defaultHeight/2),
                  ]

        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
        path.moveTo(0,0)
        path.lineTo(TableItem.defaultWidth,0)
        path.moveTo(-(TableItem.defaultWidth/3),TableItem.defaultHeight/4)
        path.lineTo((TableItem.defaultWidth+10),TableItem.defaultHeight/4)

        self.gobj = QtGui.QGraphicsPathItem(path, self)
        #self.gobj.setToolTip("Need to see what to show unlike conc/nint for pool")
        tabledoc = (element(self.mobj.path)).outputValue
        self.gobj.setToolTip(str(tabledoc))
        self.gobj.setPen(QtGui.QPen(QtCore.Qt.black, 2,Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
        self.gobj.mobj = self.mobj

    def refresh( self,scale):
        defaultWidth = TableItem.defaultWidth*scale
        defaultHeight = TableItem.defaultHeight*scale
        points = [QtCore.QPointF(0,defaultWidth/2),
                  QtCore.QPointF(defaultHeight/2-2,0),
                  QtCore.QPointF(defaultWidth/2+2,0),
                  QtCore.QPointF(defaultWidth,defaultHeight/2)
                  ]
        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
        path.moveTo(0,0)
        path.lineTo(defaultWidth,0)
        path.moveTo(-(defaultWidth/3),defaultHeight/4)
        path.lineTo((defaultWidth+10),defaultHeight/4)
        self.gobj.setPath(path)
        TablePen = self.gobj.pen()
        defaultpenwidth = TableItem.defaultPenWidth
        tableWidth =  TableItem.defaultPenWidth*scale
        TablePen.setWidth(tableWidth)
        self.gobj.setPen(TablePen)

    def setDisplayProperties(self, x,y,textcolor,bgcolor):
        #TODO check the table bounding reactangle b'cos selection looks ugly
        """Set the display properties of this item."""
        self.setGeometry(x,y, 
                         self.gobj.boundingRect().width(), 
                         self.gobj.boundingRect().height())
    
class ReacItem(KineticsDisplayItem):
    defaultWidth = 30
    defaultHeight = 30
    defaultPenWidth = 2
    name = ITEM
    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        points = [QtCore.QPointF(ReacItem.defaultWidth/4, 0),
                  QtCore.QPointF(0, ReacItem.defaultHeight/4),
                  QtCore.QPointF(ReacItem.defaultWidth, ReacItem.defaultHeight/4),
                  QtCore.QPointF(3*ReacItem.defaultWidth/4, ReacItem.defaultHeight/2)]
        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
        self.gobj = QtGui.QGraphicsPathItem(path, self)
        self.gobj.setPen(QtGui.QPen(QtCore.Qt.black, 2,Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
        self.gobj.mobj = self.mobj
        #classname = self.mobj.className
        # classname = 'ReacBase'
        # doc = moose.element('/classes/%s' % (classname)).docs
        # print "docs ",self.gobj.mobj, " ",doc
        # doc = doc.split('Description:')[-1].split('Name:')[0].strip()
        self._Kf = self.gobj.mobj.Kf
        self._Kb = self.gobj.mobj.Kb
        doc = "Kf\t: "+str(self._Kf)+"\nKb\t: "+str(self._Kb) 
        self.gobj.setToolTip(doc)

    def updateValue(self,gobj):
        self._gobj = gobj
        #if ( type(self._gobj) is moose.ZombieReac or type(self_gobj) is moose.Reac):
        if (isinstance(self._gobj,ReacBase)):
            self._Kf = self._gobj.Kf
            self._Kb = self._gobj.Kb
            doc = "Kf\t: "+str(self._Kf)+"\nKb\t: "+str(self._Kb)
            self.gobj.setToolTip(doc)

    def refresh( self,scale):
        defaultWidth = ReacItem.defaultWidth*scale
        defaultHeight = ReacItem.defaultHeight*scale
        points = [QtCore.QPointF(defaultWidth/4, 0),
                          QtCore.QPointF(0,defaultHeight/4),
                          QtCore.QPointF(defaultWidth, defaultHeight/4),
                          QtCore.QPointF(3*defaultWidth/4,defaultHeight/2)]
        path = QtGui.QPainterPath()
        path.moveTo(points[0])
        for p in points[1:]:
            path.lineTo(p)
            path.moveTo(p)
                
        self.gobj.setPath(path)
        ReacPen = self.gobj.pen()
        defaultpenwidth = ReacItem.defaultPenWidth
        reacWidth =  defaultpenwidth*scale
        ReacPen.setWidth(reacWidth)
        self.gobj.setPen(ReacPen)

    def setDisplayProperties(self, x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(x,y, 
                         self.gobj.boundingRect().width(), 
                         self.gobj.boundingRect().height())

class EnzItem(KineticsDisplayItem):
    defaultWidth = 20
    defaultHeight = 10 
    name = ITEM   
    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        self.gobj = QtGui.QGraphicsEllipseItem(0, 0, 
                                            EnzItem.defaultWidth, 
                                            EnzItem.defaultHeight, self)
        self.gobj.mobj = self.mobj
        # classname = 'EnzBase'
        # doc = moose.element('/classes/%s' % (classname)).docs
        # doc = doc.split('Description:')[-1].split('Name:')[0].strip()
        self._Km   = self.gobj.mobj.Km
        self._Kcat = self.gobj.mobj.kcat
        doc = "Km\t: "+str(self._Km)+"\nKcat\t: "+str(self._Kcat) 
        self.gobj.setToolTip(doc)

    def updateValue(self,gobj):
        self._gobj = gobj
        if ( isinstance(self.gobj,EnzBase)):
            self._Km = self._gobj.Km
            self._Kcat = self._gobj.kcat
            doc = "Km\t: "+str(self._Km)+"\nKcat\t: "+str(self._Kcat)
            self.gobj.setToolTip(doc)

    def updateColor(self,bgcolor):
        self.gobj.setBrush(QtGui.QBrush(QtGui.QColor(bgcolor)))        

    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(x,y, 
                          self.gobj.boundingRect().width(), 
                          self.gobj.boundingRect().height())
        self.gobj.setBrush(QtGui.QBrush(bgcolor))

    def refresh(self,scale):
        defaultWidth = EnzItem.defaultWidth*scale
        defaultHeight = EnzItem.defaultHeight*scale
        self.gobj.setRect(0,0,defaultWidth,defaultHeight)

class MMEnzItem(EnzItem):
    name = ITEM
    def __init__(self,*args, **kwargs):
        EnzItem.__init__(self,*args, **kwargs)

class CplxItem(KineticsDisplayItem):
    defaultWidth = 10
    defaultHeight = 10
    name = ITEM    
    def __init__(self, *args, **kwargs):
        KineticsDisplayItem.__init__(self, *args, **kwargs)
        self.gobj = QtGui.QGraphicsRectItem(0,0, CplxItem.defaultWidth, CplxItem.defaultHeight, self)
        self.gobj.mobj = self.mobj
        self._conc = self.mobj.conc
        self._n    = self.mobj.n
        doc = "Conc\t: "+str(self._conc)+"\nn\t: "+str(self._n)
        self.gobj.setToolTip(doc)

    def updateValue(self,gobj):
        self._gobj = gobj
        if (isinstance(self._gobj,PoolBase)):
            self._conc = self.mobj.conc
            self._n    = self.mobj.n
            doc = "Conc\t: "+str(self._conc)+"\nn\t: "+str(self._n)
            self.gobj.setToolTip(doc)

    def setDisplayProperties(self,x,y,textcolor,bgcolor):
        """Set the display properties of this item."""
        self.setGeometry(self.gobj.boundingRect().width()/2,self.gobj.boundingRect().height(), 
                         self.gobj.boundingRect().width(), 
                         self.gobj.boundingRect().height())
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable,False)

    def refresh(self,scale):
        defaultWidth = CplxItem.defaultWidth*scale
        defaultHeight = CplxItem.defaultHeight*scale
    
        self.gobj.setRect(0,0,defaultWidth,defaultHeight)

class ComptItem(QtGui.QGraphicsRectItem):
    name = COMPARTMENT
    def __init__(self,parent,x,y,w,h,item):
        self.cmptEmitter = QtCore.QObject()
        iParent = item
        '''
        self._rect = QtCore.QRectF(x,y,w,h)
        self._scene = parent
        print "self comptItem",self._scene.sceneContainer ," and",self._scene.sceneContainer.graphicsView
        self.mouseOver = False
        self.resizeHandleSize = 4.0
        self.mousePressPos = None
        self.mouseMovePos = None
        self.mouseIsPressed = False
        #self.setAcceptsHoverEvents(True)
        self.updateResizeHandles()
        '''
        if hasattr(iParent, "__iter__"):
            self.mobj = iParent[0]
        else:
            self.mobj = iParent
        self.layoutWidgetPt = parent
        QtGui.QGraphicsRectItem.__init__(self,x,y,w,h)

        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable, True);
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
        #self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1)
        QT_VERSION = str(QtCore.QT_VERSION_STR).split('.')
        QT_MINOR_VERSION = int(QT_VERSION[1])
        if config.QT_MINOR_VERSION >= 6:
            self.setFlag(QtGui.QGraphicsItem.ItemSendsGeometryChanges, 1) 
    '''
    def hoverEnterEvent(self, event):
        self.updateResizeHandles()
        self.mouseOver = True
        self.prepareGeometryChange()

    def hoverLeaveEvent(self, event):
        self.mouseOver = False
        self.prepareGeometryChange()

    def hoverMoveEvent(self, event):

        if self.topLeft.contains(event.scenePos()) or self.bottomRight.contains(event.scenePos()):
            self.setCursor(QtCore.Qt.SizeFDiagCursor)
        elif self.topRight.contains(event.scenePos()) or self.bottomLeft.contains(event.scenePos()):
            self.setCursor(QtCore.Qt.SizeBDiagCursor)
        else:
            self.setCursor(QtCore.Qt.SizeAllCursor)

        QtGui.QGraphicsRectItem.hoverMoveEvent(self, event)

    def mousePressEvent(self, event):
        """
        Capture mouse press events and find where the mosue was pressed on the object
        """
        self.mousePressPos = event.scenePos()
        self.mouseIsPressed = True
        self.rectPress = copy.deepcopy(self._rect)

        # Top left corner
        if self.topLeft.contains(event.scenePos()):
            self.mousePressArea = 'topleft'
        # top right corner            
        elif self.topRight.contains(event.scenePos()):
            self.mousePressArea = 'topright'
        #  bottom left corner            
        elif self.bottomLeft.contains(event.scenePos()):
            self.mousePressArea = 'bottomleft'
        # bottom right corner            
        elif self.bottomRight.contains(event.scenePos()):
            self.mousePressArea = 'bottomright'
        # entire rectangle
        else:
            self.mousePressArea = None

        QtGui.QGraphicsRectItem.mousePressEvent(self, event)

    def mouseReleaseEvent(self, event):
        """
        Capture nmouse press events.
        """
        self.mouseIsPressed = False

        self.updateResizeHandles()
        self.prepareGeometryChange()

        QtGui.QGraphicsRectItem.mouseReleaseEvent(self, event)

    def mouseMoveEvent(self, event):
        """
        Handle mouse move events.
        """
        self.mouseMovePos = event.scenePos()

        if self.mouseIsPressed:
            # Move top left corner
            if self.mousePressArea=='topleft':
                self._rect.setTopLeft(self.rectPress.topLeft()-(self.mousePressPos-self.mouseMovePos))
            # Move top right corner            
            elif  self.mousePressArea=='topright':
                self._rect.setTopRight(self.rectPress.topRight()-(self.mousePressPos-self.mouseMovePos))
            # Move bottom left corner            
            elif  self.mousePressArea=='bottomleft':
                self._rect.setBottomLeft(self.rectPress.bottomLeft()-(self.mousePressPos-self.mouseMovePos))
            # Move bottom right corner            
            elif  self.mousePressArea=='bottomright':
                self._rect.setBottomRight(self.rectPress.bottomRight()-(self.mousePressPos-self.mouseMovePos))
            # Move entire rectangle, don't resize
            else:
                self._rect.moveCenter(self.rectPress.center()-(self.mousePressPos-self.mouseMovePos))

            self.updateResizeHandles()
            self.prepareGeometryChange()

        QtGui.QGraphicsRectItem.mousePressEvent(self, event)

    def boundingRect(self):
        """
        Return bounding rectangle
        """
        return self._boundingRect
    
    def updateResizeHandles(self):
        """
        Update bounding rectangle and resize handles
        """
        self.offset = self.resizeHandleSize*(self._scene.sceneContainer.graphicsView.mapToScene(1,0)-self._scene.sceneContainer.graphicsView.mapToScene(0,1)).x()        

        self._boundingRect = self._rect.adjusted(-self.offset, self.offset, self.offset, -self.offset)

        # Note: this draws correctly on a view with an inverted y axes. i.e. QGraphicsView.scale(1,-1)
        self.topLeft = QtCore.QRectF(self._boundingRect.topLeft().x(), self._boundingRect.topLeft().y() - 2*self.offset,
                                     2*self.offset, 2*self.offset)
        self.topRight = QtCore.QRectF(self._boundingRect.topRight().x() - 2*self.offset, self._boundingRect.topRight().y() - 2*self.offset,
                                     2*self.offset, 2*self.offset)
        self.bottomLeft = QtCore.QRectF(self._boundingRect.bottomLeft().x(), self._boundingRect.bottomLeft().y(),
                                     2*self.offset, 2*self.offset)
        self.bottomRight = QtCore.QRectF(self._boundingRect.bottomRight().x() - 2*self.offset, self._boundingRect.bottomRight().y(),
                                     2*self.offset, 2*self.offset)
    
    def paint(self, painter, option, widget):
        """
        Paint Widget
        """

        # show boundingRect for debug purposes
        painter.setPen(QtGui.QPen(QtCore.Qt.red, 0, QtCore.Qt.DashLine))
        painter.drawRect(self._boundingRect)

        # Paint rectangle
        painter.setPen(QtGui.QPen(QtCore.Qt.black, 0, QtCore.Qt.SolidLine))
        painter.drawRect(self._rect)

        # If mouse is over, draw handles
        if self.mouseOver:
            # if rect selected, fill in handles
            if self.isSelected():
                painter.setBrush(QtGui.QBrush(QtGui.QColor(0,0,0)))
            painter.drawRect(self.topLeft)
            painter.drawRect(self.topRight)
            painter.drawRect(self.bottomLeft)
            painter.drawRect(self.bottomRight)
    '''
    def pointerLayoutpt(self):
        return (self.layoutWidgetPt)

    # def mouseDoubleClickEvent(self, event):
    #     self.cmptEmitter.emit(QtCore.SIGNAL("qgtextDoubleClick(PyQt_PyObject)"),element(self.mooseObj_))
    
    def itemChange(self,change,value):
        if change == QtGui.QGraphicsItem.ItemPositionChange:
            self.cmptEmitter.emit(QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),self.mobj)
        if change == QtGui.QGraphicsItem.ItemSelectedChange and value == True:
           self.cmptEmitter.emit(QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),self.mobj)
        return QtGui.QGraphicsItem.itemChange(self,change,value)


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    a = moose.Pool('pool')
    b = moose.Reac('reac')
    c = moose.Enz('enzyme')
    gview = QtGui.QGraphicsView()
    scene = QtGui.QGraphicsScene(gview)
    #item = PoolItem(a)
    #dinfo = (5, 5, QtGui.QColor('red'), QtGui.QColor('blue'))
    #item.setDisplayProperties(dinfo)
    reacItem = ReacItem(b)
    reacItem.setDisplayProperties(50, 5, QtGui.QColor('yellow'), QtGui.QColor('green'))
    enzItem = EnzItem(c)
    enzItem.setDisplayProperties(100, 10, QtGui.QColor('blue'), QtGui.QColor('yellow'))
    #scene.addItem(item)
    scene.addItem(reacItem)
    scene.addItem(enzItem)
    gview.setScene(scene)
    #print 'Position', reacItem.pos()
    gview.show()
    sys.exit(app.exec_())
