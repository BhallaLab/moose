from PyQt4.QtGui import QPolygonF
from PyQt4.QtCore import QLineF,QPointF
import math
from kkitQGraphics import PoolItem #, ReacItem,EnzItem,CplxItem,ComptItem

''' One to need to pass the source, destination,endtype and order for drawing the arrow between 2 object \
    endtype is to check if needs arrow head (arrowhead for product and sumtotal) \
    order for higher order reaction
'''
def calcArrow(srcdes_list,itemignoreZooming,iconScale):
    ''' if PoolItem then boundingrect should be background rather than graphicsobject '''
    src = srcdes_list[0]
    des = srcdes_list[1]
    endtype = srcdes_list[2]
    order = srcdes_list[3]
    # print("Source => ", src)
    compartment = src.parentItem()
    srcobj = src.gobj
    desobj = des.gobj
    if isinstance(src,PoolItem):
        srcobj = src.bg
    if isinstance(des,PoolItem):
        desobj = des.bg
            
    # if itemignoreZooming:
    #     srcRect = self.recalcSceneBoundingRect(srcobj)
    #     desRect = self.recalcSceneBoundingRect(desobj)
    # else:
    srcRect = compartment.mapFromScene(srcobj.sceneBoundingRect()).boundingRect()
    desRect = compartment.mapFromScene(desobj.sceneBoundingRect()).boundingRect()
    arrow = QPolygonF()
    if srcRect.intersects(desRect):                
        ''' This is created for getting a emptyline reference \
            because 'lineCord' function keeps a reference between qgraphicsline and its src and des
        '''
        arrow.append(QPointF(0,0))
        arrow.append(QPointF(0,0))
        return arrow
    if (order == 0):
        tmpLine = QLineF(srcRect.center().x(),
                                srcRect.center().y(),
                                desRect.center().x(),
                                desRect.center().y())
    elif(order > 0):
        dx = desRect.center().x()- srcRect.center().x()
        dy = desRect.center().y()- srcRect.center().y()
        dx0 = dy
        dy0 = -dx
        tetha1 = (math.atan2(dy0,dx0))
        a0 = 4 *(math.cos(tetha1))
        b0 = 4 *(math.sin(tetha1))
        ''' Higher order ( > 4) connectivity will not be done'''
        if ((order == 3) or (order == 4)):
            a0 = a0*2
            b0 = b0*2
        if(order %2 == 0):
            srcCentera0 = srcRect.center().x()-a0
            srcCenterb0 = srcRect.center().y()-b0
            desCentera0 = desRect.center().x()-a0
            desCenterb0 = desRect.center().y()-b0
        else:
            srcCentera0 = srcRect.center().x()+a0
            srcCenterb0 = srcRect.center().y()+b0
            desCentera0 = desRect.center().x()+a0
            desCenterb0 = desRect.center().y()+b0
        pointa = QPointF(srcCentera0,srcCenterb0)
        pointb = QPointF(desCentera0,desCenterb0)
        tmpLine = QLineF(srcCentera0,srcCenterb0,desCentera0,desCenterb0)

    srcIntersects, lineSrcPoint = calcLineRectIntersection(srcRect, tmpLine)
    destIntersects, lineDestPoint = calcLineRectIntersection(desRect, tmpLine)

    if not srcIntersects:
        print 'Source does not intersect line. Arrow points:',lineSrcPoint,src.mobj.name, src.mobj.className
    if not destIntersects:
        print 'Dest does not intersect line. Arrow points:', lineDestPoint,  des.mobj.name, des.mobj.className

    '''src and des are connected with line co-ordinates
       Arrow head is drawned if the distance between src and des line is >8 just for clean appeareance
    '''
    if (abs(lineSrcPoint.x()-lineDestPoint.x()) > 8 or abs(lineSrcPoint.y()-lineDestPoint.y())>8):
        srcAngle = tmpLine.angle()
        if endtype == 'p' or endtype == 'stp':
            ''' Arrow head for Destination is calculated'''
            arrow.append(lineSrcPoint)
            arrow.append(lineDestPoint)
            degree = -60
            srcXArr1,srcYArr1= arrowHead(srcAngle,degree,lineDestPoint,iconScale)
            arrow.append(QPointF(srcXArr1,srcYArr1))
            arrow.append(QPointF(lineDestPoint.x(),lineDestPoint.y()))
                
            degree = -120
            srcXArr2,srcYArr2 = arrowHead(srcAngle,degree,lineDestPoint,iconScale)
            arrow.append(QPointF(srcXArr2,srcYArr2))                    
            arrow.append(QPointF(lineDestPoint.x(),lineDestPoint.y()))
 
        elif endtype == 'st':
            ''' Arrow head for Source is calculated'''
            arrow.append(lineDestPoint)
            arrow.append(lineSrcPoint)
            degree = 60
            srcXArr2,srcYArr2 = arrowHead(srcAngle,degree,lineSrcPoint,iconScale)
            arrow.append(QPointF(srcXArr2,srcYArr2))                    
            arrow.append(QPointF(lineSrcPoint.x(),lineSrcPoint.y()))

            degree = 120
            srcXArr1,srcYArr1= arrowHead(srcAngle,degree,lineSrcPoint,iconScale)
            arrow.append(QPointF(srcXArr1,srcYArr1))
            arrow.append(QPointF(lineSrcPoint.x(),lineSrcPoint.y()))

        elif endtype == 's' or endtype == 'sts':
            arrow.append(lineDestPoint)
            arrow.append(lineSrcPoint)
            
            degree = 60
            srcXArr2,srcYArr2 = arrowHead(srcAngle,degree,lineSrcPoint,iconScale)
            arrow.append(QPointF(srcXArr2,srcYArr2))                    
            arrow.append(QPointF(lineSrcPoint.x(),lineSrcPoint.y()))

            degree = 120
            srcXArr1,srcYArr1= arrowHead(srcAngle,degree,lineSrcPoint,iconScale)
            arrow.append(QPointF(srcXArr1,srcYArr1))
            arrow.append(QPointF(lineSrcPoint.x(),lineSrcPoint.y()))
        else:
            arrow.append(lineSrcPoint)
            arrow.append(lineDestPoint)
    return arrow

def calcLineRectIntersection(rect, centerLine):
    '''      checking which side of rectangle intersect with centerLine \
        Here the 1. a. intersect point between center and 4 sides of src and \
                    b. intersect point between center and 4 sides of des and \
                     to draw a line connecting for src & des
                 2. angle for src for the arrow head calculation is returned
    '''
    x = rect.x()
    y = rect.y()
    w = rect.width()
    h = rect.height()
    borders = [(x,y,x+w,y),
                   (x+w,y,x+w,y+h),
                   (x+w,y+h,x,y+h),
                   (x,y+h,x,y)]
    intersectionPoint = QPointF()
    intersects = False
    for lineEnds in borders:
        line = QLineF(*lineEnds)
        intersectType = centerLine.intersect(line, intersectionPoint)
        if intersectType == centerLine.BoundedIntersection:
            intersects = True
            break
    return (intersects, intersectionPoint)

def arrowHead(srcAngle,degree,lineSpoint,iconScale):
    '''  arrow head is calculated '''
    r = 8*iconScale
    delta = math.radians(srcAngle) + math.radians(degree)
    width = math.sin(delta)*r
    height = math.cos(delta)*r
    srcXArr = (lineSpoint.x() + width)
    srcYArr = (lineSpoint.y() + height)
    return srcXArr,srcYArr
