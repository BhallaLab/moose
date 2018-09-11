__author__      =   "HarshaRani"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "HarshaRani"
__email__       =   "hrani@ncbs.res.in"
__status__      =   "Development"
__updated__     =   "Sep 7 2018"

'''
2018

Sep 07  : when object qgraphicalparent is changed then connecting arrow's parent also need to be changed 
Jun 08  : If object is moved from one group or compartment to another group or with in same Compartment, 
           then both at moose level (group or compartment path is updated ) and qt level the setParentItem is set
          -If object is moved to Empty place or not allowed place in the GUI its moved back to origin position  
          -also some clean up when object is just clicked in QsvgItem and v/s clicked and some action done
          -with Rubber selection if object are moved then group size is updated
2017
Oct 03  : At mousePressEvent, a clean way of checking on what object mouse press Event happened is checked.
            This is after group is added where Group Interior and Boundary is checked, with in groupInterior if  click in
            on COMPARTMENT BOUNDARY is clicked then COMPARTMENT_BOUNDARY is return, else top most group object is returned.
Sep 20  : Group related function added
          -resolveGroupInteriorAndBoundary, findGraphic_groupcompt, graphicsIsInstance
          -@resolveItem,editorMousePressEvent,editorMouseMoveEvent,editorMouseReleaseEvent checks made for group

'''
import sys
from modelBuild import *
from constants import *
from PyQt4.QtGui import QPixmap, QImage, QPen, QGraphicsPixmapItem, QGraphicsLineItem
from PyQt4.QtCore import pyqtSignal
from kkitUtil import  *
#from setsolver import *
from PyQt4 import QtSvg
from moose import utils

class GraphicalView(QtGui.QGraphicsView):

    def __init__(self, modelRoot,parent,border,layoutPt,createdItem):
        QtGui.QGraphicsView.__init__(self,parent)
        
        self.state = None
        self.move  = False
        self.resetState()
        self.connectionSign          = None
        self.connectionSource        = None
        self.expectedConnection      = None
        self.selections              = []
        self.connector               = None
        self.connectionSignImagePath = "../gui/icons/connection.png"
        self.connectionSignImage     = QImage(self.connectionSignImagePath)

        # self.expectedConnectionPen   = QPen()
        self.setScene(parent)
        self.modelRoot = modelRoot
        self.sceneContainerPt = parent
        self.sceneContainerPt.setItemIndexMethod(QtGui.QGraphicsScene.NoIndex)
        self.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
        self.itemSelected = False
        self.customrubberBand = None
        self.rubberbandWidth = 0
        self.rubberbandHeight = 0
        self.moved = False
        self.showpopupmenu = False
        self.popupmenu4rlines = True
        self.border = 6
        self.setRenderHints(QtGui.QPainter.Antialiasing)
        self.layoutPt = layoutPt
        # All the object which are stacked on the scene are listed
        self.stackOrder = self.sceneContainerPt.items(Qt.Qt.DescendingOrder)
        #From stackOrder selecting only compartment
        self.cmptStackorder = [i for i in self.stackOrder if isinstance(i,ComptItem)]
        self.viewBaseType = " "
        self.iconScale = 1
        self.arrowsize = 2
        self.defaultComptsize = 5
        self.connectorlist = {"plot": None ,"clone": None,"move": None,"delete": None}
        self.setHorizontalScrollBarPolicy(PyQt4.QtCore.Qt.ScrollBarAlwaysOn)
        self.setVerticalScrollBarPolicy(PyQt4.QtCore.Qt.ScrollBarAlwaysOn)

    def setRefWidget(self,path):
        self.viewBaseType = path
    
    def resizeEvent(self, event):
        #print event.size().width(),event.size().height()
        self.fitInView(self.sceneContainerPt.itemsBoundingRect().x()-10,self.sceneContainerPt.itemsBoundingRect().y()-10,self.sceneContainerPt.itemsBoundingRect().width()+20,self.sceneContainerPt.itemsBoundingRect().height()+20,Qt.Qt.IgnoreAspectRatio)
        #print("Called =>", event)

        return
    
    def resolveCompartmentInteriorAndBoundary(self, item, position):
        bound = item.rect().adjusted(3,3,-3,-3)
        return COMPARTMENT_INTERIOR if bound.contains(item.mapFromScene(position)) else COMPARTMENT_BOUNDARY

    def resolveGroupInteriorAndBoundary(self, item, position):
        bound = item.rect().adjusted(3,3,-3,-3)
        return GROUP_INTERIOR if bound.contains(item.mapFromScene(position)) else GROUP_BOUNDARY

    def resetState(self):
        self.state = { "press"      :   { "mode"     :  INVALID
                                        , "item"     :  None
                                        , "sign"     :  None
                                        , "pos"      :  None
                                        , "scenepos" :  None
                                        }
                , "move"       :   { "happened": False
                                   }
                , "release"    :   { "mode"    : INVALID
                                   , "item"    : None
                                   , "sign"    : None
                                   }
                }


    def resolveItem(self, items, position):
        csolution = None
        gsolution = None
        groupInteriorfound = False
        groupList = []
        comptInteriorfound = False
        comptBoundary = []

        # If clicked , moved and dropped then drop object should not take polygonItem it shd take Goup or compartment 
        #(this is checked is self.state["move"]), else qpolygonItem then deleting the connection b/w 2 objects
        #move is True the
        for item in items:
            if isinstance(item, QtSvg.QGraphicsSvgItem):
                return (item, CONNECTOR)

        if not self.state["move"]["happened"]:
            for item in items:
                if isinstance(item, QtGui.QGraphicsPolygonItem):
                    return (item, CONNECTION)

        for item in items:
            if hasattr(item, "name"):
                if item.name == ITEM:
                    return (item, ITEM)
                if item.name == GROUP:
                    gsolution = (item, self.resolveGroupInteriorAndBoundary(item, position))
                    if gsolution[1] == GROUP_BOUNDARY:
                        return gsolution
                    elif gsolution[1] == GROUP_INTERIOR:
                        groupInteriorfound = True
                        groupList.append(gsolution)

                if item.name == COMPARTMENT:
                    csolution = (item, self.resolveCompartmentInteriorAndBoundary(item, position))
                    if csolution[1] == COMPARTMENT_BOUNDARY:
                        comptInteriorfound = True
                        comptBoundary.append(csolution)

        if groupInteriorfound:
            if comptInteriorfound:
                return comptBoundary[0]
            else:
                return groupList[0]
        else:
            if csolution is None:
                return (None, EMPTY)
            return csolution
        
    def findGraphic_groupcompt(self,gelement):
        while not (self.graphicsIsInstance(gelement, ["GRPItem","ComptItem"])):
            gelement = gelement.parentItem()
        return gelement
    
    def graphicsIsInstance(self, gelement, classNames):
        return gelement.__class__.__name__ in classNames

    def editorMousePressEvent(self, event):
        self.clickPosition  = self.mapToScene(event.pos())
        (item, itemType) = self.resolveItem(self.items(event.pos()), self.clickPosition)
        if event.buttons() == QtCore.Qt.LeftButton:
            self.state["press"]["mode"] = VALID
            self.state["press"]["item"] = item
            self.state["press"]["type"] = itemType
            self.state["press"]["pos"]  = event.pos()
            if  isinstance(item, QtSvg.QGraphicsSvgItem):
                ##This is kept for reference, so that if object (P,R,E,Tab,Fun) is moved outside the compartment,
                #then it need to be pull back to original position
                self.state["press"]["scenepos"]  = item.parent().scenePos() 
            
            if itemType == COMPARTMENT_INTERIOR or itemType == GROUP_BOUNDARY  or itemType == GROUP_INTERIOR:
                self.removeConnector()
                
            elif itemType == ITEM:
                if not self.move:
                    self.showConnector(self.state["press"]["item"])
        else:
            #If right button clicked
            self.resetState()
            if itemType == GROUP_BOUNDARY:
                popupmenu = QtGui.QMenu('PopupMenu', self)
                popupmenu.addAction("DeleteGroup", lambda : self.deleteGroup(item,self.layoutPt))
                #popupmenu.addAction("CloneGroup" ,lambda : handleCollisions(comptList, moveMin, self.layoutPt ))
                popupmenu.exec_(self.mapToGlobal(event.pos()))
            
            elif itemType == COMPARTMENT_BOUNDARY:
                if len(list(self.layoutPt.qGraCompt.values())) > 1:
                    popupmenu = QtGui.QMenu('PopupMenu', self)
                    #popupmenu.addAction("DeleteCmpt", lambda : self.deleteCmpt(item,self.layoutPt))
                    popupmenu.addAction("LinearLayout", lambda : handleCollisions(list(self.layoutPt.qGraCompt.values()), moveX, self.layoutPt))
                    popupmenu.addAction("VerticalLayout" ,lambda : handleCollisions(list(self.layoutPt.qGraCompt.values()), moveMin, self.layoutPt ))
                    popupmenu.exec_(self.mapToGlobal(event.pos()))

    
    def editorMouseMoveEvent(self,event):
        if self.state["press"]["mode"] == INVALID:
            self.state["move"]["happened"] = False
            return
        if self.move:
            #This part of the code is when rubberband selection is done and move option is selected
            initial = self.mapToScene(self.state["press"]["pos"])
            final = self.mapToScene(event.pos())
            displacement = final - initial
            for item in self.selectedItems:
                if isinstance(item, KineticsDisplayItem) and not isinstance(item,ComptItem) and not isinstance(item,CplxItem):
                    item.moveBy(displacement.x(), displacement.y())
                    self.layoutPt.positionChange(item.mobj.path)
            self.state["press"]["pos"] = event.pos()
            return
        
        self.state["move"]["happened"] = True
        itemType = self.state["press"]["type"]
        item     = self.state["press"]["item"]
        if itemType == CONNECTOR:
            actionType = str(item.data(0).toString())
            if actionType == "move":
                QtGui.QApplication.setOverrideCursor(QtGui.QCursor(QtCore.Qt.CrossCursor))
                initial = self.mapToScene(self.state["press"]["pos"])
                final = self.mapToScene(event.pos())
                displacement = final-initial
                self.removeConnector()
                item.parent().moveBy(displacement.x(), displacement.y())
                #self.layoutPt.updateArrow(item.parent())
                if isinstance(item.parent(),PoolItem):
                    for funcItem in item.parent().childItems():
                        if isinstance(funcItem,FuncItem):
                            self.layoutPt.updateArrow(funcItem)
                self.state["press"]["pos"] = event.pos()
                self.layoutPt.positionChange(item.parent().mobj)
            elif actionType == "clone":
                pixmap = QtGui.QPixmap(24, 24)
                pixmap.fill(QtCore.Qt.transparent)
                painter = QtGui.QPainter()
                painter.begin(pixmap)
                painter.setRenderHints(painter.Antialiasing)
                pen = QtGui.QPen(QtGui.QBrush(QtGui.QColor("black")), 1)
                pen.setWidthF(1.5)
                painter.setPen(pen)
                painter.drawLine(12,7,12,17)
                painter.drawLine(7,12,17,12)
                painter.end()
                QtGui.QApplication.setOverrideCursor(QtGui.QCursor(pixmap))

        if itemType == ITEM:
            self.drawExpectedConnection(event)

        if itemType == COMPARTMENT_BOUNDARY or itemType == GROUP_BOUNDARY:
            '''When Comparement or group is moved from boundary'''
            initial = self.mapToScene(self.state["press"]["pos"])
            final = self.mapToScene(event.pos())
            displacement = final - initial
            item.moveBy(displacement.x(), displacement.y())
            self.layoutPt.positionChange(item.mobj.path)
            self.state["press"]["pos"] = event.pos()

        if itemType == COMPARTMENT_INTERIOR or itemType == GROUP_INTERIOR:
            if self.customrubberBand == None:
                self.customrubberBand = QtGui.QRubberBand(QtGui.QRubberBand.Rectangle,self)
                self.customrubberBand.show()

            startingPosition = self.state["press"]["pos"]
            endingPosition = event.pos()
            displacement   = endingPosition - startingPosition

            x0 = startingPosition.x() 
            x1 = endingPosition.x()
            y0 = startingPosition.y() 
            y1 = endingPosition.y()

            if displacement.x() < 0 :
                x0,x1= x1,x0

            if displacement.y() < 0 :
                y0,y1= y1,y0

            self.customrubberBand.setGeometry(QtCore.QRect(QtCore.QPoint(x0, y0), QtCore.QSize(abs(displacement.x()), abs(displacement.y()))))
            
    def editorMouseReleaseEvent(self,event):
        if self.move:
            self.move = False
            self.setCursor(Qt.Qt.ArrowCursor)
    
        if self.state["press"]["mode"] == INVALID:
            self.state["release"]["mode"] = INVALID
            self.resetState()
            return

        self.clickPosition  = self.mapToScene(event.pos())
        (item, itemType) = self.resolveItem(self.items(event.pos()), self.clickPosition)
        self.state["release"]["mode"] = VALID
        self.state["release"]["item"] = item
        self.state["release"]["type"] = itemType
        clickedItemType = self.state["press"]["type"]
        if clickedItemType == ITEM:
            if not self.state["move"]["happened"]:
                if not self.move:
                    self.showConnector(self.state["press"]["item"])
                    self.layoutPt.plugin.mainWindow.objectEditSlot(self.state["press"]["item"].mobj, True)
                # compartment's rectangle size is calculated depending on children
                #self.layoutPt.comptChilrenBoundingRect()
                l = self.modelRoot
                if self.modelRoot.find('/',1) > 0:
                    l = self.modelRoot[0:self.modelRoot.find('/',1)]
                linfo = moose.Annotator(l+'/info')
                for k,v in self.layoutPt.qGraGrp.items():
                    rectgrp = calculateChildBoundingRect(v)
                    v.setRect(rectgrp.x()-10,rectgrp.y()-10,(rectgrp.width()+20),(rectgrp.height()+20))
                for k, v in self.layoutPt.qGraCompt.items():
                    rectcompt = v.childrenBoundingRect()
                    if linfo.modeltype == "new_kkit":
                        #if newly built model then compartment is size is fixed for some size.
                        comptBoundingRect = v.boundingRect()
                        if not comptBoundingRect.contains(rectcompt):
                            self.layoutPt.updateCompartmentSize(v)
                    else:
                        #if already built model then compartment size depends on max and min objects
                        rectcompt = calculateChildBoundingRect(v)
                        v.setRect(rectcompt.x()-10,rectcompt.y()-10,(rectcompt.width()+20),(rectcompt.height()+20))

            else:
                #When group is moved then compartment need to be update which is done here
                if isinstance(self.state["release"]["item"], KineticsDisplayItem):
                    if not moose.element(self.state["press"]["item"].mobj) == moose.element(self.state["release"]["item"].mobj):
                        self.populate_srcdes( self.state["press"]["item"].mobj
                                            , self.state["release"]["item"].mobj
                                            )
                    else:
                        pass
                self.removeExpectedConnection()
                self.removeConnector()
            self.move = False
        elif clickedItemType  == CONNECTOR:
            actionType = str(self.state["press"]["item"].data(0).toString())
            pressItem = self.state["press"]["item"]
            initscenepos = self.state["press"]["scenepos"]
            finialscenepos = pressItem.parent().scenePos()
            xx = finialscenepos.x()-initscenepos.x()
            yy = finialscenepos.y()-initscenepos.y()
            movedGraphObj = self.state["press"]["item"].parent()
                
            if actionType == "move":
                
                if itemType == EMPTY:
                    self.objectpullback("Empty",item,movedGraphObj,xx,yy)

                else:
                    grpCmpt = self.findGraphic_groupcompt(item)
                    if movedGraphObj.parentItem() != grpCmpt:
                        '''Not same compartment/group to which it belonged to '''
                        if isinstance(movedGraphObj,(EnzItem,MMEnzItem)):
                            parentPool = moose.element((movedGraphObj.mobj.neighbors['enzDest'])[0])
                            if isinstance(parentPool,PoolBase):
                                if moose.exists(grpCmpt.mobj.path+'/'+parentPool.name):
                                    if moose.exists(grpCmpt.mobj.path+'/'+parentPool.name+'/'+movedGraphObj.name):
                                        self.objectpullback("Enzyme",grpCmpt,movedGraphObj,xx,yy)
                                    else:
                                        self.moveObjSceneParent(grpCmpt,movedGraphObj,item.pos(),self.mapToScene(event.pos()))
                                else:
                                    self.objectpullback("Enzymeparent",grpCmpt,movedGraphObj,xx,yy)
                        else:
                            ''' All the other moose object '''
                            if moose.exists(grpCmpt.mobj.path+'/'+movedGraphObj.mobj.name):
                                self.objectpullback("All",grpCmpt,movedGraphObj,xx,yy)
                            else:
                                self.moveObjSceneParent(grpCmpt,movedGraphObj,item.pos(),self.mapToScene(event.pos()))
                    else:
                        '''Same compt/grp to which it was belong to '''
                        if isinstance(grpCmpt,GRPItem):
                            self.layoutPt.updateGrpSize(movedGraphObj.parentItem())
                        elif isinstance(grpCmpt,ComptItem):
                            self.layoutPt.updateCompartmentSize(movedGraphObj.parentItem())
                QtGui.QApplication.setOverrideCursor(QtGui.QCursor(Qt.Qt.ArrowCursor))
            if actionType == "delete":
                self.removeConnector()
                pixmap = QtGui.QPixmap(24, 24)
                pixmap.fill(QtCore.Qt.transparent)
                painter = QtGui.QPainter()
                painter.begin(pixmap)
                painter.setRenderHints(painter.Antialiasing)
                pen = QtGui.QPen(QtGui.QBrush(QtGui.QColor("black")), 1)
                pen.setWidthF(1.5)
                painter.setPen(pen)
                painter.drawLine(8,8,16,16)
                painter.drawLine(8,16,16,8)
                painter.end()
                QtGui.QApplication.setOverrideCursor(QtGui.QCursor(pixmap))
                reply = QtGui.QMessageBox.question(self, "Deleting Object","Do want to delete object and its connections",
                                                   QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
                if reply == QtGui.QMessageBox.Yes:
                    #delete solver first as topology is changing
                    mooseDeleteChemSolver(self.layoutPt.modelRoot)
                    self.deleteObj([item.parent()])
                    QtGui.QApplication.restoreOverrideCursor()
                else:
                    QtGui.QApplication.restoreOverrideCursor()

            elif actionType == "plot":
                element = moose.element(item.parent().mobj.path)
                if isinstance (element,moose.PoolBase):
                    if moose.exists(self.modelRoot+'/data/graph_0'):
                        self.graph = moose.element(self.modelRoot+'/data/graph_0')
                    else:
                        moose.Neutral(self.modelRoot+'/data')
                        moose.Neutral(self.modelRoot+'/data/graph_0')
                        self.graph = moose.element(self.modelRoot+'/data/graph_0')
                    plotType = "Conc"
                    msgBox = QtGui.QMessageBox()
                    msgBox.setText('What to plot?')
                    msgBox.addButton(QtGui.QPushButton('Number'), QtGui.QMessageBox.YesRole)
                    msgBox.addButton(QtGui.QPushButton('Concentration'), QtGui.QMessageBox.NoRole)
                    ret = msgBox.exec_()
                    if ret == 0:
                        plotType = "N"
                    tablePath = moose.utils.create_table_path(moose.element(self.modelRoot), self.graph, element, plotType)
                    table     = moose.utils.create_table(tablePath, element, plotType,"Table2")
                    '''
                    tablePath = utils.create_table_path(moose.element(self.modelRoot), self.graph, element, "Conc")
                    table     = utils.create_table(tablePath, element, "Conc","Table2")
                    '''
                    self.layoutPt.plugin.view.getCentralWidget().plotWidgetContainer.plotAllData()
                    reply = QtGui.QMessageBox.information(self, "plot Object","Plot is added to Graph1",
                                                   QtGui.QMessageBox.Ok)
            elif actionType == "clone":
                if self.state["move"]["happened"]:
                    QtGui.QApplication.setOverrideCursor(QtGui.QCursor(Qt.Qt.ArrowCursor))
                    self.state["press"]["item"].parent().mobj
                    cloneObj = self.state["press"]["item"]
                    posWrtComp = self.mapToScene(event.pos())
                    itemAtView = self.sceneContainerPt.itemAt(self.mapToScene(event.pos()))
                    self.removeConnector()
                    if isinstance(itemAtView,ComptItem) or isinstance(itemAtView, GRPItem):
                        #Solver should be deleted
                            ## if there is change in 'Topology' of the model
                            ## or if copy has to made then oject should be in unZombify mode
                        mooseDeleteChemSolver(self.layoutPt.modelRoot)
                        #As name is suggesting, if item is Compartment, then search in qGraCompt and if group then qGraGrp
                        if isinstance(itemAtView,ComptItem):
                            lKey = [key for key, value in self.layoutPt.qGraCompt.iteritems() if value == itemAtView][0]
                        if isinstance (itemAtView, GRPItem):
                            lKey = [key for key, value in self.layoutPt.qGraGrp.iteritems() if value == itemAtView][0]
                        iR = 0
                        iP = 0
                        t = moose.element(cloneObj.parent().mobj)
                        name = t.name
                        if isinstance(cloneObj.parent().mobj,PoolBase):
                            retValue = self.objExist(lKey.path,name,iP) 
                            if retValue != None:
                                name += retValue
                            pmooseCp = moose.copy(t,lKey.path,name,1)
                            if pmooseCp.path != '/':
                                ct = moose.element(pmooseCp)
                                concInit = pmooseCp.concInit[0]
                                #this is b'cos if pool copied across the comptartment,
                                #then it doesn't calculate nInit according but if one set 
                                #concInit then it would, just a hack
                                ct.concInit = concInit
                                #itemAtView = self.state["release"]["item"]
                                poolObj = moose.element(ct)
                                poolinfo = moose.element(poolObj.path+'/info')
                                qGItem = PoolItem(poolObj,itemAtView)
                                self.layoutPt.mooseId_GObj[poolObj] = qGItem
                                #bgcolor = getRandColor()
                                color,bgcolor = getColor(poolinfo)
                                qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y(),color,bgcolor)
                                self.emit(QtCore.SIGNAL("dropped"),poolObj)
                            
                        if isinstance(cloneObj.parent().mobj,ReacBase):
                            retValue = self.objExist(lKey.path,name,iR)
                            if retValue != None :
                                name += retValue
                            rmooseCp = moose.copy(t,lKey.path,name,1)
                            if rmooseCp.path != '/':
                                ct = moose.element(rmooseCp)
                                reacObj = moose.element(ct)
                                reacinfo = moose.Annotator(reacObj.path+'/info')
                                qGItem = ReacItem(reacObj,itemAtView)
                                self.layoutPt.mooseId_GObj[reacObj] = qGItem
                                posWrtComp = self.mapToScene(event.pos())
                                qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y(),"white", "white")
                                self.emit(QtCore.SIGNAL("dropped"),reacObj)
                        self.updateScale(self.iconScale)
                    else:
                        if itemAtView == None:
                            QtGui.QMessageBox.information(None,'Dropping Not possible ','Dropping not allowed outside the compartment',QtGui.QMessageBox.Ok)
                        else:
                            srcdesString = ((self.state["release"]["item"]).mobj).className
                            QtGui.QMessageBox.information(None,'Dropping Not possible','Dropping on \'{srcdesString}\' not allowed'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)

        if clickedItemType == CONNECTION:
            popupmenu = QtGui.QMenu('PopupMenu', self)
            popupmenu.addAction("Delete", lambda : self.deleteConnection(item))
            popupmenu.exec_(self.mapToGlobal(event.pos()))
        
        if clickedItemType == COMPARTMENT_BOUNDARY or clickedItemType == GROUP_BOUNDARY:
            if item != None:
                item.setSelected(True)
                if not self.state["move"]["happened"]:
                    self.layoutPt.plugin.mainWindow.objectEditSlot(self.state["press"]["item"].mobj, True)
                self.resetState()

        if clickedItemType == COMPARTMENT_INTERIOR or clickedItemType == GROUP_INTERIOR:
            if self.state["move"]["happened"]:
                startingPosition = self.state["press"]["pos"]
                endingPosition = event.pos()
                displacement   = endingPosition - startingPosition
                x0 = startingPosition.x() 
                x1 = endingPosition.x()
                y0 = startingPosition.y() 
                y1 = endingPosition.y()

                if displacement.x() < 0 :
                    x0,x1= x1,x0

                if displacement.y() < 0 :
                    y0,y1= y1,y0
    
                self.selectedItems = selectedItems = self.items(x0,y0,abs(displacement.x()), abs(displacement.y()))
                self.selectSelections(selectedItems)
                for item in selectedItems:
                    if isinstance(item, KineticsDisplayItem) and not isinstance(item,ComptItem):
                        item.setSelected(True)
                if self.customrubberBand != None:
                    self.customrubberBand.hide()
                    self.customrubberBand = None
                popupmenu = QtGui.QMenu('PopupMenu', self)

                popupmenu.addAction("Delete", lambda: self.deleteSelections(x0,y0,x1,y1))
                popupmenu.addAction("Zoom",   lambda: self.zoomSelections(x0,y0,x1,y1))
                popupmenu.addAction("Move",   lambda: self.moveSelections())
                popupmenu.exec_(self.mapToGlobal(event.pos()))        
        self.resetState()
    
    def objectpullback(self,messgtype,item,movedGraphObj,xx,yy):
        if messgtype.lower() != "empty":
            desObj = item.mobj.className
            if desObj == "CubeMesh" or desObj == "CyclMesh":
                desObj = "compartment"
            elif desObj == "Neutral":
                desObj = "group"
        
        movedGraphObj.moveBy(-xx,-yy)
        self.layoutPt.updateArrow(movedGraphObj)
        messgstr=""
        if messgtype.lower() == "all":
            messgstr = "The object name  \'{0}\' exist in \'{1}\' {2}".format(movedGraphObj.mobj.name,item.mobj.name,desObj)
        elif messgtype.lower() =="enzymeparent":
            messgstr = "The Enzyme parent  \'{0}\' doesn't exist in \'{2}\' {1} \n If you need to move the enzyme to {1} first parent pool needs to be moved".format(movedGraphObj.mobj.parent.name,desObj,item.mobj.name)
        elif messgtype.lower() == "enzyme":
            messgstr = "The Enzyme \'{0}\' already exist in \'{2}\' {1}".format(movedGraphObj.mobj.name,desObj,item.mobj.name)        
        elif messgtype.lower() == "empty":
            messgstr = "The object can't be moved to empty space"
        QtGui.QMessageBox.warning(None,'Could not move the object', messgstr )
        QtGui.QApplication.setOverrideCursor(QtGui.QCursor(Qt.Qt.ArrowCursor))

    def moveObjSceneParent(self,item,movedGraphObj,itempos,eventpos):
        ''' Scene parent object needs to be updated '''
        prevPar = movedGraphObj.parentItem()
        movedGraphObj.setParentItem(item)

        if isinstance(movedGraphObj,ReacItem):
            for ll in self.layoutPt.object2line[movedGraphObj]:
                ll[0].setParentItem(item)
        moose.move(movedGraphObj.mobj, item.mobj)
        self.layoutPt.plugin.mainWindow.objectEditSlot(movedGraphObj.mobj, True)

        if isinstance(movedGraphObj ,PoolItem):
            ''' if pool is moved, a check is made to see if this pool is parent of a enzyme
                if yes then Graphicalparent is changed, and 
                not moose path since it will be taken care while we move pool
            '''
            pl = (movedGraphObj.mobj).neighbors['nOut']
            for es in pl:
                if isinstance(moose.element(es), EnzBase):
                    if moose.element(moose.element(es).neighbors['enzDest'][0]) == movedGraphObj.mobj:
                        enzGrapObj = self.layoutPt.mooseId_GObj[moose.element(es)]
                        testx = enzGrapObj.scenePos().x()
                        testy = enzGrapObj.scenePos().y()
                        enzGrapObj.setParentItem(item)
                        enzGrapObj.setGeometry(testx,testy,
                                      enzGrapObj.gobj.boundingRect().width(),
                                      enzGrapObj.gobj.boundingRect().height())
                        for ll in self.layoutPt.object2line[enzGrapObj]:
                            ll[0].setParentItem(item)
                        self.layoutPt.updateArrow(enzGrapObj)
                        #enzGrapObj.setGeometry(testx,testy,
                        #              enzGrapObj.gobj.boundingRect().width(),
                        #              enzGrapObj.gobj.boundingRect().height())
        ''' Re-calculting the group size after the movement '''
        if isinstance(prevPar,GRPItem):
            if item != prevPar:
                self.layoutPt.updateGrpSize(prevPar)
                self.layoutPt.updateGrpSize(item)
        self.setnewPostion(movedGraphObj,itempos,eventpos)
        self.layoutPt.updateArrow(movedGraphObj)

    def setnewPostion(self,movedGraphObj,itempos,eventpos):
        if isinstance(movedGraphObj,KineticsDisplayItem):
            itemPath = movedGraphObj.mobj.path
            if moose.exists(itemPath):
                iInfo = itemPath+'/info'
                anno = moose.Annotator(iInfo)
                # eventpos = self.mapToScene(event.pos())
                # itempos = item.pos()
                x = eventpos.x()+(15/2)-itempos.x()
                y = eventpos.y()+(2/2)-itempos.y()
                if moose.Annotator(self.layoutPt.plugin.modelRoot+'/info').modeltype == 'kkit':
                    anno.x = x/self.layoutPt.defaultScenewidth
                    anno.y = y/self.layoutPt.defaultSceneheight
                else:
                    anno.x = x
                    anno.y = y
                #item = movedGraphObj
                if isinstance(movedGraphObj,ReacItem) or isinstance(movedGraphObj,EnzItem) or isinstance(movedGraphObj,MMEnzItem):
                    movedGraphObj.setGeometry(x,y,
                                 movedGraphObj.gobj.boundingRect().width(),
                                 movedGraphObj.gobj.boundingRect().height())
                elif isinstance(movedGraphObj,PoolItem):
                    movedGraphObj.setGeometry(x, y,movedGraphObj.gobj.boundingRect().width()
                                 +PoolItem.fontMetrics.width('  '),
                                 movedGraphObj.gobj.boundingRect().height())
                    movedGraphObj.bg.setRect(0, 0, movedGraphObj.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '), movedGraphObj.gobj.boundingRect().height())
                QtGui.QApplication.setOverrideCursor(QtGui.QCursor(Qt.Qt.ArrowCursor))

    def deleteGroup(self,item,layoutPt):
        reply = QtGui.QMessageBox.question(self, "Deleting Object",'Do want to delete group \'{groupname}\' and its children and connections'.format(groupname=item.mobj.name),
                                                   QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
        if reply == QtGui.QMessageBox.Yes:
            mooseDeleteChemSolver(self.layoutPt.modelRoot)
            key = [k for k,v in self.layoutPt.qGraGrp.items() if v == item]
            if key[0] in self.layoutPt.qGraGrp:
                self.layoutPt.qGraGrp.pop(key[0])
            self.groupItemlist1 = item.childItems()
            self.groupItemlist = [ i for i in self.groupItemlist1 if not isinstance(i,QtGui.QGraphicsPolygonItem)]
            self.deleteObj(self.groupItemlist)
            self.deleteItem(item)

    def drawExpectedConnection(self, event):
        self.connectionSource = self.state["press"]["item"]
        sourcePoint      = self.connectionSource.mapToScene(
            self.connectionSource.boundingRect().center()
                                          )
        destinationPoint = self.mapToScene(event.pos())
        if self.expectedConnection is None:
            self.expectedConnection = QGraphicsLineItem( sourcePoint.x()
                                                       , sourcePoint.y()
                                                       , destinationPoint.x()
                                                       , destinationPoint.y()
                                                       )
            self.expectedConnection.setPen(QPen(Qt.Qt.DashLine))

            self.sceneContainerPt.addItem(self.expectedConnection)
        else:
            self.expectedConnection.setLine( sourcePoint.x()
                                           , sourcePoint.y()
                                           , destinationPoint.x()
                                           , destinationPoint.y()
                                           )

    def removeExpectedConnection(self):
        self.sceneContainerPt.removeItem(self.expectedConnection)
        self.expectedConnection = None
        self.connectionSource   = None

    def removeConnector(self):
        try:
            for l,k in self.connectorlist.items():
                if k is not None:
                    self.sceneContainerPt.removeItem(k)
                    self.connectorlist[l] = None
        except:
            #print("Exception received!")
            pass

    def showConnector(self, item):
        self.removeConnector()
        self.connectionSource = item
        rectangle = item.boundingRect()

        for l in self.connectorlist.keys():
            self.xDisp = 0
            self.yDisp = 0
            self.connectionSign = None

            if isinstance(item.mobj,PoolBase) or isinstance(item.mobj,ReacBase):
                if l == "clone":
                    self.connectionSign = QtSvg.QGraphicsSvgItem('icons/clone.svg')
                    self.connectionSign.setData(0, QtCore.QVariant("clone"))
                    self.connectionSign.setParent(self.connectionSource)
                    self.connectionSign.setScale(
                        (1.0 * rectangle.height()) / self.connectionSign.boundingRect().height()
                                                )
                    position = item.mapToParent(rectangle.bottomLeft())
                    self.xDisp = 15
                    self.yDisp = 2
                    self.connectionSign.setZValue(1)
                    self.connectionSign.setToolTip("Click and drag to clone the object")
                    self.connectorlist["clone"] = self.connectionSign 
                    
            if isinstance(item.mobj,PoolBase):
                if l == "plot":
                    self.connectionSign = QtSvg.QGraphicsSvgItem('icons/plot.svg')
                    self.connectionSign.setData(0, QtCore.QVariant("plot"))
                    self.connectionSign.setParent(self.connectionSource)
                    self.connectionSign.setScale(
                        (1.0 * rectangle.height()) / self.connectionSign.boundingRect().height()
                                                )
                    position = item.mapToParent(rectangle.bottomRight())
                    #self.xDisp = 15
                    #self.yDisp = 2
                    self.connectionSign.setZValue(1)
                    self.connectionSign.setToolTip("plot the object")
                    self.connectorlist["plot"] = self.connectionSign

            if l == "move":
                if ((item.mobj.parent.className == "ZombieEnz") or (item.mobj.parent.className == "Enz")):
                    pass
                else:
                    self.connectionSign = QtSvg.QGraphicsSvgItem('icons/move.svg')
                    self.connectionSign.setData(0, QtCore.QVariant("move"))
                    self.connectionSign.setParent(self.connectionSource)
                    self.connectionSign.setToolTip("Drag to move.")
                    if ( item.mobj.className == "ZombieFunction" or item.mobj.className == "Function"):
                        self.connectionSign.setScale(
                        (0.75 * rectangle.height()) / self.connectionSign.boundingRect().height()
                                                )
                    else:
                        self.connectionSign.setScale(
                            (1 * rectangle.height()) / self.connectionSign.boundingRect().height()
                                                    )
                    position = item.mapToParent(rectangle.topLeft())
                    self.xDisp = 15
                    self.yDisp = 2
                    self.connectionSign.setZValue(1)
                    self.connectorlist["move"] = self.connectionSign
            elif l == "delete":
                if ((item.mobj.parent.className == "ZombieEnz") or (item.mobj.parent.className == "Enz")):
                    pass
                else:
                    self.connectionSign = QtSvg.QGraphicsSvgItem('icons/delete.svg')
                    self.connectionSign.setParent(self.connectionSource)
                    self.connectionSign.setData(0, QtCore.QVariant("delete"))
                    if ( item.mobj.className == "ZombieFunction" or item.mobj.className == "Function"):
                        self.connectionSign.setScale(
                        (0.75 * rectangle.height()) / self.connectionSign.boundingRect().height()
                                                )
                    else:
                        self.connectionSign.setScale(
                            (1.0 * rectangle.height()) / self.connectionSign.boundingRect().height()
                                                )
                    position = item.mapToParent(rectangle.topRight())
                    self.connectionSign.setZValue(1)
                    self.connectionSign.setToolTip("Delete the object")
                    self.connectorlist["delete"] = self.connectionSign

            if self.connectionSign != None:
                self.connectionSign.setFlag(QtGui.QGraphicsItem.ItemIsSelectable,True)
                self.connectionSign.setParentItem(item.parentItem())
                self.connectionSign.setPos(0.0,0.0)
                self.connectionSign.moveBy( position.x()-self.xDisp
                                          , position.y() +self.yDisp - rectangle.height() / 2.0
                                          )

    def objExist(self,path,name,index):
        if index == 0:
            fPath = path+'/'+name
        else:
            fPath = path+'/'+name+'_'+str(index)
        if moose.exists(fPath):
            index += 1
            return self.objExist(path,name,index)
        else:
            if index == 0:
                return
            else:
                return ('_'+str(index))

    def selectSelections(self, selections):
        for selection in selections :
            if isinstance(selection, KineticsDisplayItem):
                self.selections.append(selection)

    def deselectSelections(self):
        for selection in self.selections:
            selection.setSelected(False)
        self.selections  = []

    def mousePressEvent(self, event):
        selectedItem = None
        if self.viewBaseType == "editorView":
            return self.editorMousePressEvent(event)

        elif self.viewBaseType == "runView":
            pos = event.pos()
            item = self.itemAt(pos)
            if item:
                itemClass = type(item).__name__
                if ( itemClass!='ComptItem' and itemClass != 'QGraphicsPolygonItem' and 
                    itemClass != 'QGraphicsEllipseItem' and itemClass != 'QGraphicsRectItem'):
                    self.setCursor(Qt.Qt.CrossCursor)
                    mimeData = QtCore.QMimeData()
                    mimeData.setText(item.mobj.name)
                    mimeData.setData("text/plain", "")
                    mimeData.data =(self.modelRoot,item.mobj)
                    drag = QtGui.QDrag(self)
                    drag.setMimeData(mimeData)
                    dropAction = drag.start(QtCore.Qt.MoveAction)
                    self.setCursor(Qt.Qt.ArrowCursor)

    
    def mouseMoveEvent(self,event):
        if self.viewBaseType == "editorView":
            return self.editorMouseMoveEvent(event)
        

    def mouseReleaseEvent(self, event):
        if self.viewBaseType == "editorView":
            for preSelectedItem in self.sceneContainerPt.selectedItems():
                preSelectedItem.setSelected(False)
            return self.editorMouseReleaseEvent(event)

        return

        if self.state["press"]["mode"] == CONNECTION:
            desPos =self.mapToScene(event.pos())
            destination = self.items(event.pos())
            src = self.state["press"]["item"]
            des  = [j for j in destination if isinstance(j,KineticsDisplayItem)]
            if len(des):
                self.populate_srcdes(src.mobj,des[0].mobj)
        self.setCursor(Qt.Qt.ArrowCursor)
        QtGui.QGraphicsView.mouseReleaseEvent(self, event)
        
    def updateItemTransformationMode(self, on):
        for v in self.sceneContainerPt.items():
            #v.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations,on)
            if( not isinstance(v,ComptItem)):
                #if ( isinstance(v, PoolItem) or isinstance(v, ReacItem) or isinstance(v, EnzItem) or isinstance(v, CplxItem) ):
                if isinstance(v,KineticsDisplayItem):
                    v.setFlag(QtGui.QGraphicsItem.ItemIgnoresTransformations, on)
    def keyPressEvent(self,event):
        key = event.key()
        self.removeConnector()
        if (key ==  Qt.Qt.Key_A and (event.modifiers() & Qt.Qt.ShiftModifier)): # 'A' fits the view to iconScale factor
            itemignoreZooming = False
            self.updateItemTransformationMode(itemignoreZooming)
            #self.fitInView(self.sceneContainerPt.itemsBoundingRect().x()-10,self.sceneContainerPt.itemsBoundingRect().y()-10,self.sceneContainerPt.itemsBoundingRect().width()+20,self.sceneContainerPt.itemsBoundingRect().height()+20,Qt.Qt.IgnoreAspectRatio)
            self.layoutPt.drawLine_arrow(itemignoreZooming=False)

        elif (key == Qt.Qt.Key_Less or key == Qt.Qt.Key_Minus):# and (event.modifiers() & Qt.Qt.ShiftModifier)): # '<' key. zooms-in to iconScale factor
            self.iconScale *= 0.8
            self.updateScale( self.iconScale )

        elif (key == Qt.Qt.Key_Greater or key == Qt.Qt.Key_Plus):# and (event.modifiers() & Qt.Qt.ShiftModifier)): # '>' key. zooms-out to iconScale factor
            self.iconScale *= 1.25
            self.updateScale( self.iconScale )

        elif (key == Qt.Qt.Key_Period): # '.' key, lower case for '>' zooms in
            self.scale(1.1,1.1)

        elif (key == Qt.Qt.Key_Comma): # ',' key, lower case for '<' zooms in
            self.scale(1/1.1,1/1.1)

        elif (key == Qt.Qt.Key_A):  # 'a' fits the view to initial value where iconscale=1
            self.iconscale = 1
            self.updateScale( 1 )
            self.fitInView(self.sceneContainerPt.itemsBoundingRect().x()-10,self.sceneContainerPt.itemsBoundingRect().y()-10,self.sceneContainerPt.itemsBoundingRect().width()+20,self.sceneContainerPt.itemsBoundingRect().height()+20,Qt.Qt.IgnoreAspectRatio)

    def updateScale( self, scale ):
        for item in self.sceneContainerPt.items():
            if isinstance(item,KineticsDisplayItem):
                item.refresh(scale)
                xpos = item.pos().x()
                ypos = item.pos().y()
                if isinstance(item,ReacItem) or isinstance(item,EnzItem) or isinstance(item,MMEnzItem):
                    item.setGeometry(xpos,ypos,
                                     item.gobj.boundingRect().width(),
                                     item.gobj.boundingRect().height())
                elif isinstance(item,CplxItem):
                    item.setGeometry(item.gobj.boundingRect().width()/2,item.gobj.boundingRect().height(),
                                     item.gobj.boundingRect().width(),
                                     item.gobj.boundingRect().height())
                elif isinstance(item,PoolItem) or isinstance(item, PoolItemCircle):
                    item.setGeometry(xpos, ypos,item.gobj.boundingRect().width()
                                     +PoolItem.fontMetrics.width('  '),
                                     item.gobj.boundingRect().height())
                    item.bg.setRect(0, 0, item.gobj.boundingRect().width()+PoolItem.fontMetrics.width('  '), item.gobj.boundingRect().height())

        self.layoutPt.drawLine_arrow(itemignoreZooming=False)
        self.layoutPt.comptChildrenBoundingRect()
        
    def moveSelections(self):
        self.setCursor(Qt.Qt.CrossCursor)
        self.move = True
        return
      
    def GrVfitinView(self):
        itemignoreZooming = False
        self.layoutPt.updateItemTransformationMode(itemignoreZooming)
        self.fitInView(self.sceneContainerPt.itemsBoundingRect().x()-10,self.sceneContainerPt.itemsBoundingRect().y()-10,self.sceneContainerPt.itemsBoundingRect().width()+20,self.sceneContainerPt.itemsBoundingRect().height()+20,Qt.Qt.IgnoreAspectRatio)
        self.layoutPt.drawLine_arrow(itemignoreZooming=False)

    def deleteSelections(self,x0,y0,x1,y1):
        if( x1-x0 > 0  and y1-y0 >0):
            # self.rubberbandlist = self.sceneContainerPt.items(self.mapToScene(QtCore.QRect(x0, y0, x1 - x0, y1 - y0)).boundingRect(), Qt.Qt.IntersectsItemShape)
            # for unselectitem in self.rubberbandlist:
            self.rubberbandlist_qpolygon = []
            for unselectitem in self.rubberbandlist_qpolygon:
                if unselectitem.isSelected() == True:
                    unselectitem.setSelected(0)
            self.rubberbandlist_qpolygon = self.sceneContainerPt.items(self.mapToScene(QtCore.QRect(x0, y0, x1 - x0, y1 - y0)).boundingRect(), Qt.Qt.IntersectsItemShape)
            for item in self.rubberbandlist_qpolygon:
                ''' in RubberbandSelection if entire group object contains then group is removed,if partly selected then group is retained'''
                if isinstance(item,GRPItem):
                    if not (self.mapToScene(QtCore.QRect(x0, y0, x1 - x0, y1 - y0)).boundingRect()).contains(item.sceneBoundingRect()):
                        self.rubberbandlist_qpolygon.remove(item)
                        
            self.deleteObj(self.rubberbandlist_qpolygon)
        self.selections = []

    def deleteObj(self,item):
        self.rubberbandlist = item
        mooseDeleteChemSolver(self.layoutPt.modelRoot)
        self.list_EnzReccplx   = [ i for i in self.rubberbandlist if (isinstance(i,MMEnzItem) or isinstance(i,EnzItem) or isinstance(i,CplxItem) or isinstance(i,ReacItem) )] 
        self.list_PFS          = [ i for i in self.rubberbandlist if (isinstance(i,PoolItem) or isinstance(i,TableItem) or isinstance(i,FuncItem) )]
        self.grp               = [ i for i in self.rubberbandlist if isinstance(i,GRPItem)]
        for item in self.list_EnzReccplx:
            #First Loop to remove all the enz b'cos if parent (which is a Pool) is removed,
            #then it will created problem at qgraphicalitem not having parent.
            #So first delete enz, then Reac and then delete pool
            self.deleteItem(item)
        for item in self.list_PFS:
            if isinstance(item,PoolItem) or isinstance(item,BufPool):
                plot = moose.wildcardFind(self.layoutPt.modelRoot+'/data/graph#/#')
                for p in plot:
                    if len(p.neighbors['requestOut']):
                        if item.mobj.path == moose.element(p.neighbors['requestOut'][0]).path:
                            p.tick = -1
                            moose.delete(p)
                            self.layoutPt.plugin.view.getCentralWidget().plotWidgetContainer.plotAllData()
            self.deleteItem(item)
        for item in self.grp:
            key = [k for k,v in self.layoutPt.qGraGrp.items() if v == item]
            if key[0] in self.layoutPt.qGraGrp:
                self.layoutPt.qGraGrp.pop(key[0])
            self.groupItemlist1 = item.childItems()
            self.groupItemlist = [ i for i in self.groupItemlist1 if not isinstance(i,QtGui.QGraphicsPolygonItem)]
            self.deleteObj(self.groupItemlist)
            self.deleteItem(item)
        
    def deleteObject2line(self,qpolygonline,src,des,endt):
        object2lineInfo = self.layoutPt.object2line[des]
        if len(object2lineInfo) == 1:
            for polygon,objdes,endtype,numL in object2lineInfo:
                if polygon == qpolygonline and objdes == src and endtype == endt:
                    del(self.layoutPt.object2line[des])
                else:
                    print( " check this condition when is len is single and else condition",qpolygonline, objdes,endtype)
        else:
            n = 0
            for polygon,objdes,endtype,numL in object2lineInfo:
                if polygon == qpolygonline and objdes == src and endtype == endt:
                    tup = object2lineInfo[:n]+object2lineInfo[n+1:]
                    self.layoutPt.object2line[des] = tup
                    #d[keyNo].append((a,b,c))
                else:
                    n = n+1
        
    def deleteConnection(self,item):
        #Delete moose connection, i.e one can click on connection arrow and delete the connection
        reply = QtGui.QMessageBox.question(self, "Deleting Object","Do want to delete object and its connections",
                                                   QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
        if reply == QtGui.QMessageBox.Yes:
            mooseDeleteChemSolver(self.layoutPt.modelRoot)
            msgIdforDeleting = " "
            if isinstance(item,QtGui.QGraphicsPolygonItem):
                src = self.layoutPt.lineItem_dict[item]
                lineItem_value = self.layoutPt.lineItem_dict[item]
                i = iter(lineItem_value)
                source  = i.next()
                destination  = i.next()
                endt = i.next()
                numl = i.next()
                self.deleteObject2line(item,source,destination,endt)
                self.deleteObject2line(item,destination,source,endt)
                try:
                    del self.layoutPt.lineItem_dict[item]
                except KeyError:
                    pass
                srcZero = [k for k, v in self.layoutPt.mooseId_GObj.iteritems() if v == src[0]]
                srcOne = [k for k, v in self.layoutPt.mooseId_GObj.iteritems() if v == src[1]]
            
                if isinstance (moose.element(srcZero[0]),moose.MMenz):
                    gItem =self.layoutPt.mooseId_GObj[moose.element(srcZero[0])]
                    # This block is done b'cos for MMenz while loaded from ReadKKit, the msg
                    # from parent pool to Enz is different as compared to direct model building.
                    # if ReadKKit get the msg from parent Pool, else from MMenz itself.

                    # Rules: If some one tries to remove connection parent Pool to Enz
                    # then delete entire enz itself, this is True for enz and mmenz
                    for msg in srcZero[0].msgIn:
                        if moose.element(msg.e1.path) == moose.element(srcOne[0].path):
                            if src[2] == "t":
                                if msg.destFieldsOnE2[0] == "enzDest":
                                    # delete indivial msg if later adding parent is possible
                                    # msgIdforDeleting = msg  
                                    # moose.delete(msgIdforDeleting)
                                    # self.sceneContainerPt.removeItem(item)
                                    self.deleteItem(gItem)
                                    return
                            else:
                                self.getMsgId(src,srcZero,srcOne,item)
                                moose.delete(msgIdforDeleting)
                                self.sceneContainerPt.removeItem(item)
                                setupItem(self.modelRoot,self.layoutPt.srcdesConnection)
                    for msg in moose.element(srcZero[0].parent).msgIn:
                        if moose.element(msg.e2.path) == moose.element(srcZero[0].parent.path):
                            if src[2] == 't':
                                if len(msg.destFieldsOnE1) > 0:
                                    if msg.destFieldsOnE1[0] == "enzDest":
                                        # delete indivial msg if later adding parent is possible
                                        # msgIdforDeleting = msg  
                                        # moose.delete(msgIdforDeleting)
                                        # self.sceneContainerPt.removeItem(item)
                                        self.deleteItem(gItem)
                                    return
                            else:
                                self.getMsgId(src,srcZero,srcOne,item)
                                
                elif isinstance (moose.element(srcZero[0]),moose.Enz):
                    self.getMsgId(src,srcZero,srcOne,item)

                elif isinstance(moose.element(srcZero[0]),moose.Function):
                    v = moose.Variable(srcZero[0].path+'/x')
                    found = False
                    for msg in v.msgIn:
                        if moose.element(msg.e1.path) == moose.element(srcOne[0].path):
                            if src[2] == "sts":
                                if msg.destFieldsOnE2[0] == "input":
                                    msgIdforDeleting = msg
                                    self.deleteSceneObj(msgIdforDeleting,item)
                                    found = True
                    if not found:
                        for msg in srcZero[0].msgOut:
                            if moose.element(msg.e2.path) == moose.element(srcOne[0].path):
                                if src[2] == "stp":
                                    if msg.destFieldsOnE2[0] == "setN":
                                        gItem =self.layoutPt.mooseId_GObj[moose.element(srcZero[0])]
                                        self.deleteItem(gItem)
                                        self.deleteSceneObj(msg,item)
                                        return
                                    elif msg.destFieldsOnE2[0] == "setNumKf" or msg.destFieldsOnE2[0] == "setConcInit" or msg.destFieldsOnE2[0]=="increment":
                                        msgIdforDeleting = msg
                                        self.deleteSceneObj(msgIdforDeleting,item)
                    gItem =self.layoutPt.mooseId_GObj[moose.element(srcZero[0])]
                    self.deleteItem(gItem)
                    
                else:
                    self.getMsgId(src,srcZero,srcOne,item)
        else:
            pass

    def deleteSceneObj(self,msgIdforDeleting,item):
        moose.delete(msgIdforDeleting)
        self.sceneContainerPt.removeItem(item)
        setupItem(self.modelRoot,self.layoutPt.srcdesConnection)

    def getMsgId(self,src,srcZero,srcOne,item):
        for msg in srcZero[0].msgOut:
            msgIdforDeleting = " "
            if moose.element(msg.e2.path) == moose.element(srcOne[0].path):
                if src[2] == 's':
                    # substrate connection for R,E
                    if msg.srcFieldsOnE1[0] == "subOut":
                        msgIdforDeleting = msg
                        self.deleteSceneObj(msgIdforDeleting,item)
                        return
                elif src[2] == 'p':
                    # product connection for R,E
                    if msg.srcFieldsOnE1[0] == "prdOut":
                        msgIdforDeleting = msg
                        self.deleteSceneObj(msgIdforDeleting,item)
                        return 
                elif src[2] == 't':
                    if msg.srcFieldsOnE1[0] == "enzOut":
                        gItem =self.layoutPt.mooseId_GObj[moose.element(srcZero[0])]
                        self.deleteItem(gItem)
                        return
                elif src[2] == 'tab':
                    #stimulation Table connection
                    if msg.srcFieldsOnE1[0] == "output":
                        msgIdforDeleting = msg
                        self.deleteSceneObj(msgIdforDeleting,item)
        return 

    def deleteItem(self,item):
        #delete Items 

        self.layoutPt.plugin.mainWindow.objectEditSlot('/', False)
        if isinstance(item,KineticsDisplayItem)  or isinstance(item,GRPItem):
            if moose.exists(item.mobj.path):
                if isinstance(item,PoolItem) or isinstance(item,BufPool):
                    # pool is item is removed, then check is made if its a parent to any
                    # enz if 'yes', then enz and its connection are removed before
                    # removing Pool
                    for items in moose.element(item.mobj.path).children:
                        if isinstance(moose.element(items), EnzBase):
                            gItem = self.layoutPt.mooseId_GObj[moose.element(items)]
                            for l in self.layoutPt.object2line[gItem]:
                                # Need to check if the connection on the scene exist
                                # or its removed from some other means
                                # E.g Enz to pool and pool to Enz is connected, 
                                # when enz is removed the connection is removed, 
                                # but when pool tried to remove then qgraphicscene says 
                                # "item scene is different from this scene"
                                sceneItems = self.sceneContainerPt.items()
                                if l[0] in sceneItems:
                                    #deleting the connection which is connected to Enz
                                    self.sceneContainerPt.removeItem(l[0])
                            moose.delete(items)
                            self.sceneContainerPt.removeItem(gItem)
                    #If pool/bufpool is input to a function and if pool/bufpool is removed then function is also removed.
                    for msg in moose.element(item.mobj.path).msgOut:
                        if (moose.element(msg.e2.path).className == "Variable" and msg.destFieldsOnE2[0]=="input"):
                            funcp = moose.element(msg.e2.path).parent
                            pool = moose.element(funcp).parent
                            self.deleteItem(self.layoutPt.mooseId_GObj[funcp])

                for l in self.layoutPt.object2line[item]:
                    sceneItems = self.sceneContainerPt.items()
                    if l[0] in sceneItems:
                        self.sceneContainerPt.removeItem(l[0])
                self.sceneContainerPt.removeItem(item)
                moose.delete(item.mobj)
                for key, value in self.layoutPt.object2line.items():
                    self.layoutPt.object2line[key] = filter( lambda tup: tup[1] != item ,value)
                self.layoutPt.getMooseObj()
                setupItem(self.modelRoot,self.layoutPt.srcdesConnection) 

    def zoomSelections(self, x0, y0, x1, y1):
        p0 = self.mapToScene(x0, y0)
        p1 = self.mapToScene(x1, y1)
        self.fitInView(QtCore.QRectF(p0, p1), Qt.Qt.KeepAspectRatio)
        self.deselectSelections()
        return
        

    def wheelEvent(self,event):
        factor = 1.41 ** (event.delta() / 240.0)
        self.scale(factor, factor)


    def dragEnterEvent(self, event):
        if self.viewBaseType == "editorView":
            if event.mimeData().hasFormat('text/plain'):
                event.acceptProposedAction()
        else:
            pass

    def dragMoveEvent(self, event):
        if self.viewBaseType == "editorView":
            if event.mimeData().hasFormat('text/plain'):
                event.acceptProposedAction()
        else:
            pass

    def eventFilter(self, source, event):
        if self.viewBase == "editorView":
            if (event.type() == QtCore.QEvent.Drop):
                pass
        else:
            pass

    def dropEvent(self, event):
        """Insert an element of the specified class in drop location"""
        """ Pool and reaction should have compartment as parent, dropping outside the compartment is not allowed """
        """ Enz should be droped on the PoolItem which inturn will be under compartment"""
        if self.viewBaseType == "editorView":
            if not event.mimeData().hasFormat('text/plain'):
                return
            event_pos = event.pos()
            string = str(event.mimeData().text())
            createObj(self.viewBaseType,self,self.modelRoot,string,event_pos,self.layoutPt)

    def populate_srcdes(self,src,des):
        self.modelRoot = self.layoutPt.modelRoot
        callsetupItem = True
        #print " populate_srcdes ",src,des
        srcClass =  moose.element(src).className
        if 'Zombie' in srcClass:
            srcClass = srcClass.split('Zombie')[1]
        desClass = moose.element(des).className
        if 'Zombie' in desClass:
            desClass = desClass.split('Zombie')[1]
        if ( isinstance(moose.element(src),PoolBase) and ( (isinstance(moose.element(des),ReacBase) ) or isinstance(moose.element(des),EnzBase) )):
            #If one to tries to connect pool to Reac/Enz (substrate to Reac/Enz), check if already (product to Reac/Enz) exist.
            #If exist then connection not allowed one need to delete the msg and try connecting back.
            #And in moose Enzyme can't have 2nd order reaction.
            founds, foundp = False,False
            
            if isinstance(moose.element(des),EnzBase):
                if len(moose.element(des).neighbors["subOut"]) > 0:
                    founds = True
                
            for msg in des.msgOut:
                if moose.element(msg.e2.path) == src:
                    if msg.srcFieldsOnE1[0] == "prdOut":
                        foundp = True 
            
            if foundp == False and founds == False:
                # moose.connect(src, 'reac', des, 'sub', 'OneToOne')
                moose.connect(des, 'sub', src, 'reac', 'OneToOne')
            elif foundp:
                srcdesString = srcClass+' is already connected as '+ '\'Product\''+' to '+desClass +' \n \nIf you wish to connect this object then first delete the exist connection'
                QtGui.QMessageBox.information(None,'Connection Not possible','{srcdesString}'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)
            elif founds:
                srcdesString = desClass+' has already connected to a'+ '\'Substrate\''+' \n \nIn moose Enzyme\'s can not have second order reaction. If you wish to connect this object then first delete the exist connection'
                QtGui.QMessageBox.information(None,'Connection Not possible','{srcdesString}'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)    
                       
        elif (isinstance (moose.element(src),PoolBase) and (isinstance(moose.element(des),Function))):
            numVariables = des.numVars
            expr = ""
            expr = (des.expr+'+'+'x'+str(numVariables))
            expr = expr.lstrip("0 +")
            expr = expr.replace(" ","")
            des.expr = expr
            moose.connect( src, 'nOut', des.x[numVariables], 'input' )
            
        elif ( isinstance(moose.element(src),Function) and (moose.element(des).className=="Pool") or  
               isinstance(moose.element(src),ZombieFunction) and (moose.element(des).className=="ZombiePool")
            ):
                if ((element(des).parent).className != 'Enz'):
                    #moose.connect(src, 'valueOut', des, 'increment', 'OneToOne')
                    found = False
                    if len(moose.element(src).neighbors["valueOut"]):
                        for psl in moose.element(src).neighbors["valueOut"]:
                            if moose.element(psl) == moose.element(des):
                                found = True
                    if found == False:
                        moose.connect(src, 'valueOut', des, 'setN', 'OneToOne')
                    else:
                        srcdesString = '\"'+moose.element(src).name+'\" is already connected to \"'+ moose.element(des).name +'\" \n'
                        QtGui.QMessageBox.information(None,'Connection Not possible','{srcdesString}'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)

                    
                else:
                    srcdesString = element(src).className+'-- EnzCplx'
                    QtGui.QMessageBox.information(None,'Connection Not possible','\'{srcdesString}\' not allowed to connect'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)
                    callsetupItem = False
        elif ( isinstance(moose.element(src),Function) and (moose.element(des).className=="BufPool") or  
               isinstance(moose.element(src),ZombieFunction) and (moose.element(des).className=="ZombieBufPool")
            ):
                moose.connect(src, 'valueOut', des, 'setN', 'OneToOne')
        elif ( isinstance(moose.element(src),Function) and (isinstance(moose.element(des),ReacBase) ) or
               isinstance(moose.element(src),ZombieFunction) and (moose.element(des).className=="ZombieReac")
            ):
                moose.connect(src, 'valueOut', des, 'setNumKf', 'OneToOne')
        elif (((isinstance(moose.element(src),ReacBase))or (isinstance(moose.element(src),EnzBase))) and (isinstance(moose.element(des),PoolBase))):
            founds,foundp = False,False
            if isinstance(moose.element(src),EnzBase):
                if len(moose.element(src).neighbors["prdOut"]) > 0:
                    foundp = True
  
            for msg in src.msgOut:
                if moose.element(msg.e2.path) == des:
                    if msg.srcFieldsOnE1[0] == "subOut":
                        founds = True 
            if founds == False and foundp == False:
                #moose.connect(src, 'prd', des, 'reac', 'OneToOne')
                moose.connect(src, 'prd', des, 'reac', 'OneToOne')
            elif foundp:
                srcdesString = srcClass+' is already connected as '+ '\'Product\''+' to '+desClass +' \n \nIn moose Enzyme\'s can not have second order reaction. If you wish to connect this object then first delete the exist connection'
                QtGui.QMessageBox.information(None,'Connection Not possible','{srcdesString}'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)
            elif founds:
                srcdesString = desClass+' is already connected as '+'\'Substrate\''+' to '+srcClass +' \n \nIf you wish to connect this object then first delete the exist connection'
                QtGui.QMessageBox.information(None,'Connection Not possible','{srcdesString}'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)
        elif( isinstance(moose.element(src),StimulusTable) and (isinstance(moose.element(des),PoolBase) ) ):
            moose.connect(src, 'output', des, 'setConcInit', 'OneToOne')
        else:
            srcString = moose.element(src).className
            desString = moose.element(des).className
            srcdesString = srcString+'--'+desString
            QtGui.QMessageBox.information(None,'Connection Not possible','\'{srcdesString}\' not allowed to connect'.format(srcdesString = srcdesString),QtGui.QMessageBox.Ok)
            callsetupItem = False
            
        if callsetupItem:
            self.layoutPt.getMooseObj()
            setupItem(self.modelRoot,self.layoutPt.srcdesConnection)
            self.layoutPt.drawLine_arrow(False)
