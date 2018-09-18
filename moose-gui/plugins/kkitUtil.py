__author__      =   "HarshaRani"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "HarshaRani"
__email__       =   "hrani@ncbs.res.in"
__status__      =   "Development"
__updated__     =   "Sep 17 2018"

'''
2018
Sep 17: when vertical or horizontal layout is applied for group, compartment size is recalculated
Sep 11: group size is calculated based on sceneBoundingRect for compartment size
2017
Oct 18  some of the function moved to this file from kkitOrdinateUtils
'''
from moose import Annotator,element,ChemCompt
from kkitQGraphics import PoolItem, ReacItem,EnzItem,CplxItem,GRPItem,ComptItem
from PyQt4 import QtCore,QtGui,QtSvg
from PyQt4.QtGui import QColor
import numpy as np
import os
import config
import pickle
import random
import matplotlib

colormap_file = open(os.path.join(config.settings[config.KEY_COLORMAP_DIR], 'rainbow2.pkl'),'rb')
colorMap = pickle.load(colormap_file)
colormap_file.close()

ignoreColor= ["mistyrose","antiquewhite","aliceblue","azure","bisque","black","blanchedalmond","blue","cornsilk","darkolivegreen","darkslategray","dimgray","floralwhite","gainsboro","ghostwhite","honeydew","ivory","lavender","lavenderblush","lemonchiffon","lightcyan","lightgoldenrodyellow","lightgray","lightyellow","linen","mediumblue","mintcream","navy","oldlace","papayawhip","saddlebrown","seashell","snow","wheat","white","whitesmoke","aquamarine","lightsalmon","moccasin","limegreen","snow","sienna","beige","dimgrey","lightsage"]
matplotcolor = {}
for name,hexno in matplotlib.colors.cnames.iteritems():
    matplotcolor[name]=hexno

def getRandColor():
    k = random.choice(matplotcolor.keys())
    if k in ignoreColor:
        return getRandColor()
    else:
        return QColor(matplotcolor[k])

def getRandColor1():
    color = (np.random.randint(low=0, high=255, size=3)).tolist()
    if not all((x <= 65 or x >= 105) for x in (color[0],color[1],color[2])):
        return QColor(color[0],color[1],color[2])
    else:
        return getRandColor()
    

def getColor(iteminfo):
    """ Getting a textcolor and background color for the given  mooseObject \
        If textcolor is empty replaced with green \
           background color is empty replaced with blue
           if textcolor and background is same as it happend in kkit files \
           replacing textcolor with random color\
           The colors are not valid there are siliently replaced with some values \
           but while model building can raise an exception
    """
    textcolor = Annotator(iteminfo).getField('textColor')
    bgcolor = Annotator(iteminfo).getField('color')
    if(textcolor == ''): textcolor = 'green'
    if(bgcolor == ''): bgcolor = 'blue'
    if(textcolor == bgcolor):textcolor = getRandColor()
    textcolor = colorCheck(textcolor,"fc")
    bgcolor = colorCheck(bgcolor,"bg")
    return(textcolor,bgcolor)

def colorCheck(fc_bgcolor,fcbg):
    """ textColor or background can be anything like string or tuple or list \
        if string its taken as colorname further down in validColorcheck checked for valid color, \
        but for tuple and list its taken as r,g,b value.
    """
    #import re
    #fc_bgcolor = re.sub('[^a-zA-Z0-9-_*.]', '', fc_bgcolor)
    if isinstance(fc_bgcolor,str):
        if fc_bgcolor.startswith("#"):
            fc_bgcolor = QColor(fc_bgcolor)
        elif fc_bgcolor.isdigit():
            """ color is int  a map from int to r,g,b triplets from pickled color map file """
            tc = int(fc_bgcolor)
            tc = tc*2
            if tc < len(colorMap):
                pickledColor = colorMap[tc]
            else:
                pickledColor = (255, 0, 0)
            fc_bgcolor = QColor(*pickledColor)

        elif fc_bgcolor.isalpha() or fc_bgcolor.isalnum():
            fc_bgcolor = validColorcheck(fc_bgcolor)
        else:
            fc_bgcolor = QColor(*eval(fc_bgcolor))
            # fc_bgcolor = validColorcheck(fc_bgcolor)
    return(fc_bgcolor)

def validColorcheck(color):
	''' 
        Both in Qt4.7 and 4.8 if not a valid color it makes it as back but in 4.7 there will be a warning mssg which is taken here
        checking if textcolor or backgroundcolor is valid color, if 'No' making white color as default
        where I have not taken care for checking what will be backgroundcolor for textcolor or textcolor for backgroundcolor 
    '''
        if QColor(color).isValid():
            return (QColor(color))
        else:
            return(QColor("white"))


def moveMin(reference, collider, layoutPt,margin):
    referenceRect = reference.sceneBoundingRect()
    colliderRect = collider.sceneBoundingRect()
    xDistance = referenceRect.x() + referenceRect.width() / 2.0 + colliderRect.width() / 2.0 + margin - colliderRect.x()
    yDistance = 0.0     
    if colliderRect.y() < referenceRect.y():
        yDistance = (referenceRect.y() - referenceRect.height() / 2.0 - colliderRect.height() / 2.0 - margin) - colliderRect.y()
    else:
        yDistance = referenceRect.y() + referenceRect.height() / 2.0 + colliderRect.height() / 2.0 + margin - colliderRect.y()

    #if xDistance > yDistance:
    collider.moveBy(xDistance, yDistance)
    #else:
    #   collider.moveBy(xDistance, 0.0)
    layoutPt.drawLine_arrow(itemignoreZooming=False)

def moveX(reference, collider, layoutPt, margin):
    referenceRect = reference.sceneBoundingRect()
    colliderRect = collider.sceneBoundingRect()
    xc = abs(referenceRect.topRight().x()) - abs(colliderRect.topLeft().x())+margin
    yc = 0.0
    collider.moveBy(xc,yc)
    layoutPt.drawLine_arrow(itemignoreZooming=False)

def handleCollisions(compartments, moveCallback, layoutPt,margin = 5.0):
    print " handelCollision"
    if len(compartments) is 0 : return
    compartments = sorted(compartments, key = lambda c: c.sceneBoundingRect().center().x())
    print " compartment ",compartments
    reference = compartments.pop(0);
    print (reference.name)
    referenceRect = reference.sceneBoundingRect()
    colliders = filter( lambda compartment : referenceRect.intersects(compartment.sceneBoundingRect())
                      , compartments
                      )
    for collider in colliders:
        moveCallback(reference, collider, layoutPt,margin)
    #print (reference.mobj).parent
    if isinstance(element(((reference.mobj).parent)),ChemCompt):
        v = layoutPt.qGraCompt[element(((reference.mobj).parent))]
        #layoutPt.updateCompartmentSize(x)
        rectcompt = calculateChildBoundingRect(v)
        comptBoundingRect = v.boundingRect()
        if not comptBoundingRect.contains(rectcompt):
            layoutPt.updateCompartmentSize(v)
                    
        else:
            rectcompt = calculateChildBoundingRect(v)
            v.setRect(rectcompt.x()-10,rectcompt.y()-10,(rectcompt.width()+20),(rectcompt.height()+20))
    return handleCollisions(compartments, moveCallback, layoutPt,margin)

def calculateChildBoundingRect(compt):
    ''' In this function I am trying to calculate BoundingRect of the compartments
        looking into children and its children which default "ChildrenBoundingRect"
        function was doing it but in multi-compartment cross-compartment reaction 
        the arrow width is taken into account which doesn't belong to this perticular compartment
    '''
    ypos = []
    xpos = []
    for l in compt.childItems():
        ''' All the children including pool,reac,enz,polygon(arrow),table '''
        if not isinstance(l,QtSvg.QGraphicsSvgItem):
            if (not isinstance(l,QtGui.QGraphicsPolygonItem)):
                if (not isinstance(l,GRPItem)):
                    xpos.append((l.pos().x())+(l.boundingRect().bottomRight().x()))
                    xpos.append(l.pos().x())
                    ypos.append(l.pos().y()+l.boundingRect().bottomRight().y())
                    ypos.append(l.pos().y())

                else:
                    xpos.append(l.sceneBoundingRect().x())
                    xpos.append(l.sceneBoundingRect().bottomRight().x())
                    ypos.append(l.sceneBoundingRect().y())
                    ypos.append(l.sceneBoundingRect().bottomRight().y())
                    '''
                    xpos.append(l.rect().x())
                    xpos.append(l.boundingRect().bottomRight().x())
                    ypos.append(l.rect().y())
                    ypos.append(l.boundingRect().bottomRight().y())
                    '''
        if (isinstance(l,PoolItem) or isinstance(l,EnzItem)):
            ''' For Enz cplx height and for pool function height needs to be taken'''
            for ll in l.childItems():
                ''' eleminating polygonItem (arrow) [This is happen in cross-compartment model that arrow from one compartment will be child]
                    pool's outboundary RectItem and Enz's outerboundary Ellipse is eleminating since its same 
                '''
                if ( (not isinstance(ll,QtGui.QGraphicsPolygonItem)) and 
                     (not isinstance(ll,QtGui.QGraphicsRectItem)) and 
                     (not isinstance(ll,QtGui.QGraphicsEllipseItem))
                    ):
                    ypos.append(l.pos().y()+ll.pos().y()+ll.boundingRect().bottomRight().y())
    if xpos and ypos:
        calculateRectcompt = QtCore.QRectF(min(xpos),min(ypos),(max(xpos)-min(xpos)),(max(ypos)-min(ypos)))
    else:
        calculateRectcompt = compt.rect()
        
    return calculateRectcompt

def mooseIsInstance(melement, classNames):
    return element(melement).__class__.__name__ in classNames

def findCompartment(melement):
    while not mooseIsInstance(melement, ["CubeMesh", "CyclMesh"]):
        melement = melement.parent
    return melement

def findGroup(melement):
    while not mooseIsInstance(melement, ["Neutral"]):
        melement = melement.parent
    return melement

def findGroup_compt(melement):
    while not (mooseIsInstance(melement, ["Neutral","CubeMesh", "CyclMesh"])):
        melement = melement.parent
    return melement