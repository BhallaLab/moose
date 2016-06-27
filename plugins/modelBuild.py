import moose
from kkitQGraphics import * 
from kkitOrdinateUtil import *
from kkitUtil import *
import PyQt4
from setsolver import *

def updateCompartmentSize(qGraCompt):
    #childBoundingRect = qGraCompt.childrenBoundingRect()
    childBoundingRect = calculateChildBoundingRect(qGraCompt)
    comptBoundingRect = qGraCompt.boundingRect()
    rectcompt = comptBoundingRect.united(childBoundingRect)
    comptPen = qGraCompt.pen()
    comptWidth =  1
    comptPen.setWidth(comptWidth)
    qGraCompt.setPen(comptPen)
    if not comptBoundingRect.contains(childBoundingRect):
        qGraCompt.setRect(rectcompt.x()-comptWidth,rectcompt.y()-comptWidth,rectcompt.width()+(comptWidth*2),rectcompt.height()+(comptWidth*2))
    
# def checkCreate(string,num,itemAt,qGraCompt,modelRoot,scene,pos,posf,view,qGIMob):
def checkCreate(scene,view,modelpath,mobj,string,ret_string,num,event_pos,layoutPt):
    # The variable 'compt' will be empty when dropping cubeMesh,cyclMesh, but rest it shd be
    # compartment
    # if modelpath.find('/',1) > -1:
    #     modelRoot = modelpath[0:modelpath.find('/',1)]
    # else:
    #     modelRoot = modelpath
    if moose.exists(modelpath+'/info'):
        mType = moose.Annotator((moose.element(modelpath+'/info'))).modeltype
    
    itemAtView = view.sceneContainerPt.itemAt(view.mapToScene(event_pos))
    pos = view.mapToScene(event_pos)
    modelpath = moose.element(modelpath)

    if num:
        string_num = ret_string+str(num)
    else:
        string_num = ret_string
    if string == "CubeMesh" or string == "CylMesh":
        if string == "CylMesh":
            mobj = moose.CylMesh(modelpath.path+'/'+string_num)
        else:    
            mobj = moose.CubeMesh(modelpath.path+'/'+string_num)
        
        mobj.volume = 1e-15
        mesh = moose.element(mobj.path+'/mesh')
        qGItem = ComptItem(scene,pos.toPoint().x(),pos.toPoint().y(),500,500,mobj)
        qGItem.setPen(QtGui.QPen(Qt.QColor(66,66,66,100), 1, Qt.Qt.SolidLine, Qt.Qt.RoundCap, Qt.Qt.RoundJoin))
        view.sceneContainerPt.addItem(qGItem)
        qGItem.cmptEmitter.connect(qGItem.cmptEmitter,QtCore.SIGNAL("qgtextPositionChange(PyQt_PyObject)"),layoutPt.positionChange1)
        qGItem.cmptEmitter.connect(qGItem.cmptEmitter,QtCore.SIGNAL("qgtextItemSelectedChange(PyQt_PyObject)"),layoutPt.objectEditSlot)
        compartment = qGItem
        layoutPt.qGraCompt[mobj]= qGItem
        view.emit(QtCore.SIGNAL("dropped"),mobj)
    
    elif string == "Pool" or string == "BufPool":
        #getting pos with respect to compartment otherwise if compartment is moved then pos would be wrong
        posWrtComp = (itemAtView.mapFromScene(pos)).toPoint()
        if string == "Pool":
            poolObj = moose.Pool(mobj.path+'/'+string_num)
        else:
            poolObj = moose.BufPool(mobj.path+'/'+string_num)    
        
        poolinfo = moose.Annotator(poolObj.path+'/info')
        #Compartment's one Pool object is picked to get the font size
        
        qGItem = PoolItem(poolObj,itemAtView)
        layoutPt.mooseId_GObj[poolObj] = qGItem
        posWrtComp = (itemAtView.mapFromScene(pos)).toPoint()
        bgcolor = getRandColor()
        qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y(),QtGui.QColor('green'),bgcolor)
        poolinfo.color = str(bgcolor.getRgb())
        #if mType == "new_kkit":
        poolinfo.x = posWrtComp.x()
        poolinfo.y = posWrtComp.y()
        view.emit(QtCore.SIGNAL("dropped"),poolObj)
        setupItem(modelpath.path,layoutPt.srcdesConnection)
        layoutPt.drawLine_arrow(False)
        poolinfo.x = posWrtComp.x()
        poolinfo.y = posWrtComp.y()
        
        #Dropping is on compartment then update Compart size
        if isinstance(mobj,moose.ChemCompt):
            compt = layoutPt.qGraCompt[moose.element(mobj)]
            updateCompartmentSize(compt)
        
    elif  string == "Reac":
        posWrtComp = (itemAtView.mapFromScene(pos)).toPoint()
        reacObj = moose.Reac(mobj.path+'/'+string_num)
        reacinfo = moose.Annotator(reacObj.path+'/info')
        qGItem = ReacItem(reacObj,itemAtView)
        qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y(),"white", "white")
        #if mType == "new_kkit":
        reacinfo.x = posWrtComp.x()
        reacinfo.y = posWrtComp.y()
        layoutPt.mooseId_GObj[reacObj] = qGItem
        view.emit(QtCore.SIGNAL("dropped"),reacObj)
        setupItem(modelpath.path,layoutPt.srcdesConnection)
        layoutPt.drawLine_arrow(False)
        #Dropping is on compartment then update Compart size
        if isinstance(mobj,moose.ChemCompt):
            compt = layoutPt.qGraCompt[moose.element(mobj)]
            updateCompartmentSize(compt)

    elif  string == "StimulusTable":
        posWrtComp = (itemAtView.mapFromScene(pos)).toPoint()
        tabObj = moose.StimulusTable(mobj.path+'/'+string_num)
        tabinfo = moose.Annotator(tabObj.path+'/info')
        qGItem = TableItem(tabObj,itemAtView)
        qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y(),QtGui.QColor('white'),QtGui.QColor('white'))
        #if mType == "new_kkit":
        tabinfo.x = posWrtComp.x()
        tabinfo.y = posWrtComp.y()
        layoutPt.mooseId_GObj[tabObj] = qGItem
        view.emit(QtCore.SIGNAL("dropped"),tabObj)
        setupItem(modelpath.path,layoutPt.srcdesConnection)
        layoutPt.drawLine_arrow(False)
        #Dropping is on compartment then update Compart size
        if isinstance(mobj,moose.ChemCompt):
            compt = layoutPt.qGraCompt[moose.element(mobj)]
            updateCompartmentSize(compt)
    elif string == "Function":
        posWrtComp = (itemAtView.mapFromScene(pos)).toPoint()

        funcObj = moose.Function(mobj.path+'/'+string_num)
        funcinfo = moose.Annotator(funcObj.path+'/info')
        moose.connect( funcObj, 'valueOut', mobj ,'setN' )
        funcParent = layoutPt.mooseId_GObj[element(mobj.path)]
        qGItem = FuncItem(funcObj,funcParent)
        #print " function ", posWrtComp.x(),posWrtComp.y()
        qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y(),QtGui.QColor('red'),QtGui.QColor('green'))
        layoutPt.mooseId_GObj[funcObj] = qGItem
        #if mType == "new_kkit":
        funcinfo.x = posWrtComp.x()
        funcinfo.y = posWrtComp.y()
        view.emit(QtCore.SIGNAL("dropped"),funcObj)
        setupItem(modelpath.path,layoutPt.srcdesConnection)
        layoutPt.drawLine_arrow(False)
        #Dropping is on compartment then update Compart size
        mooseCmpt = findCompartment(mobj)
        if isinstance(mooseCmpt,moose.ChemCompt):
            compt = layoutPt.qGraCompt[moose.element(mooseCmpt)]
            updateCompartmentSize(compt)

    elif  string == "Enz" or string == "MMenz":
        #If 2 enz has same pool parent, then pos of the 2nd enz shd be displaced by some position, need to check how to deal with it
        posWrtComp = pos
        enzPool = layoutPt.mooseId_GObj[mobj]
        if ((mobj.parent).className == "Enz"):
            QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' has to have Pool as its parent and not Enzyme Complex'.format(newString =string),QtGui.QMessageBox.Ok)
            return
        else:
            enzparent = findCompartment(mobj)
            parentcompt = layoutPt.qGraCompt[enzparent]
        if string == "Enz":
            enzObj = moose.Enz(moose.element(mobj).path+'/'+string_num)
            enzinfo = moose.Annotator(enzObj.path+'/info')
            moose.connect( enzObj, 'enz', mobj, 'reac' )
            qGItem = EnzItem(enzObj,parentcompt)
            layoutPt.mooseId_GObj[enzObj] = qGItem
            posWrtComp = pos
            bgcolor = getRandColor()
            qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y()-40,QtGui.QColor('green'),bgcolor)
            #if mType == "new_kkit":
            enzinfo.x = posWrtComp.x()
            enzinfo.y = posWrtComp.y()
        
            enzinfo.color = str(bgcolor.name())
            e = moose.Annotator(enzinfo)
            #e.x = posWrtComp.x()
            #e.y = posWrtComp.y()
            Enz_cplx = enzObj.path+'/'+string_num+'_cplx';
            cplxItem = moose.Pool(Enz_cplx)
            cplxinfo = moose.Annotator(cplxItem.path+'/info')
            qGEnz = layoutPt.mooseId_GObj[enzObj]
            qGItem = CplxItem(cplxItem,qGEnz)
            layoutPt.mooseId_GObj[cplxItem] = qGItem
            enzboundingRect = qGEnz.boundingRect()
            moose.connect( enzObj, 'cplx', cplxItem, 'reac' )
            qGItem.setDisplayProperties(enzboundingRect.height()/2,enzboundingRect.height()-40,QtGui.QColor('white'),QtGui.QColor('white'))
            cplxinfo.x = enzboundingRect.height()/2
            cplxinfo.y = enzboundingRect.height()-60
            view.emit(QtCore.SIGNAL("dropped"),enzObj)

        else:
            enzObj = moose.MMenz(mobj.path+'/'+string_num)
            enzinfo = moose.Annotator(enzObj.path+'/info')
            moose.connect(mobj,"nOut",enzObj,"enzDest")
            qGItem = MMEnzItem(enzObj,parentcompt)
            posWrtComp = pos
            bgcolor = getRandColor()
            qGItem.setDisplayProperties(posWrtComp.x(),posWrtComp.y()-30,QtGui.QColor('green'),bgcolor)
            enzinfo.x = posWrtComp.x()
            enzinfo.y = posWrtComp.y()
            enzinfo.color = str(bgcolor.name())
            layoutPt.mooseId_GObj[enzObj] = qGItem
            view.emit(QtCore.SIGNAL("dropped"),enzObj)
        setupItem(modelpath.path,layoutPt.srcdesConnection)
        layoutPt.drawLine_arrow(False)
        #Dropping is on compartment then update Compart size
        if isinstance(enzparent,moose.ChemCompt):
            updateCompartmentSize(parentcompt)
    if view.iconScale != 1:
        view.updateScale(view.iconScale)

def createObj(scene,view,modelpath,string,pos,layoutPt):
    event_pos = pos
    num = 0
    ret_string = " "
    pos = view.mapToScene(event_pos)
    itemAt = view.sceneContainerPt.itemAt(pos)
    chemMesh = moose.wildcardFind(modelpath+'/##[ISA=ChemCompt]')
    deleteSolver(modelpath)
    mobj = ""

    if itemAt != None:
        itemAtView = view.sceneContainerPt.itemAt(view.mapToScene(event_pos))
        itemClass = type(itemAtView).__name__
        if ( itemClass == 'QGraphicsRectItem'):
            mobj = itemAtView.parentItem().mobj
        elif(itemClass == 'QGraphicsSvgItem'):
            mobj = itemAtView.parent().mobj
        else:
            mobj = itemAtView.mobj
    
    if string == "CubeMesh" or string == "CylMesh":
        ret_string,num = findUniqId(moose.element(modelpath),"Compartment",0)
        comptexist = moose.wildcardFind(modelpath+'/##[ISA=ChemCompt]')
        if not len(comptexist):
            if itemAt != None:
                QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' currently single compartment model building is allowed'.format(newString =string),QtGui.QMessageBox.Ok)
                return
            else:
                mobj = moose.element(modelpath)
        else:
            QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' currently single compartment model building is allowed'.format(newString =string),QtGui.QMessageBox.Ok)
            return
    
    elif string == "Pool" or string == "BufPool" or string == "Reac" or string == "StimulusTable":
        if itemAt == None:
            QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' has to have compartment as its parent'.format(newString =string),QtGui.QMessageBox.Ok)
            return
        else:
            mobj = findCompartment(mobj)
            ret_string,num = findUniqId(mobj,string,num)

    elif string == "Function":
        if itemAt != None:
            if ((mobj).className != "BufPool"):    
                QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' has to have BufPool as its parent'.format(newString =string),QtGui.QMessageBox.Ok)
                return
            else:
                ret_string,num = findUniqId(mobj,string,num)
        else:
            QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' has to have BufPool as its parent'.format(newString =string),QtGui.QMessageBox.Ok)
            return

    elif string == "Enz" or string == "MMenz":
        if itemAt != None:
            if ((mobj).className != "Pool" and (mobj).className != "BufPool"):    
                QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' has to have Pool as its parent'.format(newString =string),QtGui.QMessageBox.Ok)
                return
            else:
                ret_string,num = findUniqId(mobj,string,num)
        else:
            QtGui.QMessageBox.information(None,'Drop Not possible','\'{newString}\' has to have Pool as its parent'.format(newString =string),QtGui.QMessageBox.Ok)
            return
    
    if ret_string != " ":
        checkCreate(scene,view,modelpath,mobj,string,ret_string,num,event_pos,layoutPt)

def findUniqId(mobj,string,num):
    if num == 0:
        path = mobj.path+'/'+string;
    else:
        path = mobj.path+'/'+string+str(num);
    if not moose.exists(path):
        return(string,num)
    else:
        num +=1;
        return(findUniqId(mobj,string,num))

def findCompartment(mooseObj):
    if mooseObj.path == '/':
        return None
    elif isinstance(mooseObj,ChemCompt):
        return (mooseObj)
    else:
        return findCompartment(moose.element(mooseObj.parent))