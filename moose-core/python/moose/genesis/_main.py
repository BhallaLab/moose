""" Chemical Signalling model loaded into moose can be save into Genesis-Kkit format """

__author__           = "Harsha Rani"
__copyright__        = "Copyright 2015, Harsha Rani and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Harsha Rani"
__email__            = "hrani@ncbs.res.in"
__status__           = "Development"

import sys
import random
from moose import wildcardFind, element, loadModel, ChemCompt, exists, Annotator, Pool, ZombiePool,PoolBase,CplxEnzBase,Function,ZombieFunction
import numpy as np
import re

GENESIS_COLOR_SEQUENCE = ((248, 0, 255), (240, 0, 255), (232, 0, 255), (224, 0, 255), (216, 0, 255), (208, 0, 255),
 (200, 0, 255), (192, 0, 255), (184, 0, 255), (176, 0, 255), (168, 0, 255), (160, 0, 255), (152, 0, 255), (144, 0, 255),
 (136, 0, 255), (128, 0, 255), (120, 0, 255), (112, 0, 255), (104, 0, 255), (96, 0, 255), (88, 0, 255), (80, 0, 255),
 (72, 0, 255),  (64, 0, 255), (56, 0, 255), (48, 0, 255), (40, 0, 255), (32, 0, 255), (24, 0, 255), (16, 0, 255),
 (8, 0, 255),  (0, 0, 255), (0, 8, 248), (0, 16, 240), (0, 24, 232), (0, 32, 224), (0, 40, 216), (0, 48, 208),
 (0, 56, 200),  (0, 64, 192), (0, 72, 184), (0, 80, 176), (0, 88, 168), (0, 96, 160), (0, 104, 152), (0, 112, 144),
 (0, 120, 136),  (0, 128, 128), (0, 136, 120), (0, 144, 112), (0, 152, 104), (0, 160, 96), (0, 168, 88), (0, 176, 80),
 (0, 184, 72), (0, 192, 64), (0, 200, 56), (0, 208, 48), (0, 216, 40), (0, 224, 32), (0, 232, 24), (0, 240, 16), (0, 248, 8),
 (0, 255, 0), (8, 255, 0), (16, 255, 0), (24, 255, 0), (32, 255, 0), (40, 255, 0), (48, 255, 0), (56, 255, 0), (64, 255, 0),
 (72, 255, 0), (80, 255, 0), (88, 255, 0), (96, 255, 0), (104, 255, 0), (112, 255, 0), (120, 255, 0), (128, 255, 0),
 (136, 255, 0), (144, 255, 0), (152, 255, 0), (160, 255, 0), (168, 255, 0), (176, 255, 0), (184, 255, 0), (192, 255, 0),
 (200, 255, 0), (208, 255, 0), (216, 255, 0), (224, 255, 0), (232, 255, 0), (240, 255, 0), (248, 255, 0), (255, 255, 0),
 (255, 248, 0), (255, 240, 0), (255, 232, 0), (255, 224, 0), (255, 216, 0), (255, 208, 0), (255, 200, 0), (255, 192, 0),
 (255, 184, 0),  (255, 176, 0), (255, 168, 0), (255, 160, 0), (255, 152, 0), (255, 144, 0), (255, 136, 0), (255, 128, 0),
 (255, 120, 0), (255, 112, 0),  (255, 104, 0), (255, 96, 0), (255, 88, 0), (255, 80, 0), (255, 72, 0), (255, 64, 0),
 (255, 56, 0), (255, 48, 0), (255, 40, 0), (255, 32, 0), (255, 24, 0), (255, 16, 0), (255, 8, 0), (255, 0, 0))

#Todo : To be written
#               --Notes
#               --StimulusTable

def write( modelpath, filename,sceneitems=None):
        if filename.rfind('.') != -1:
            filename = filename[:filename.rfind('.')]
        else:
            filename = filename[:len(filename)]
        filename = filename+'.g'
        global NA
        NA = 6.0221415e23
        global xmin,xmax,ymin,ymax
        global cord
        global multi
        xmin = ymin = 0
        xmax = ymax = 1
        multi = 50
        cord = {}
        compt = wildcardFind(modelpath+'/##[ISA=ChemCompt]')
        maxVol = estimateDefaultVol(compt)
        f = open(filename, 'w')
        writeHeader (f,maxVol)
        if (compt > 0):
                if sceneitems == None:
                        #if sceneitems is none (loaded from script) then check x,y cord exists
                        xmin,ymin,xmax,ymax,positionInfoExist = getCor(modelpath,sceneitems)
                        if not positionInfoExist:
                                #incase of SBML or cspace or python Annotator is not populated then positionInfoExist= False
                                #print " x and y cordinates doesn't exist so auto cordinates"
                                print(" auto co-ordinates needs to be applied")
                                pass
                else:
                        #This is when it comes from Gui where the objects are already layout on to scene
                        # so using thoes co-ordinates
                        xmin,ymin,xmax,ymax,positionInfoExist = getCor(modelpath,sceneitems)
                gtId_vol = writeCompartment(modelpath,compt,f)
                writePool(modelpath,f,gtId_vol)
                reacList = writeReac(modelpath,f)
                enzList = writeEnz(modelpath,f)
                writeSumtotal(modelpath,f)
                storeReacMsg(reacList,f)
                storeEnzMsg(enzList,f)
                writeGui(f)
                tgraphs = wildcardFind(modelpath+'/##[ISA=Table2]')
                if tgraphs:
                        writeplot(tgraphs,f)
                        storePlotMsgs(tgraphs,f)
                writeFooter1(f)
                writeNotes(modelpath,f)
                writeFooter2(f)
                return True
        else:
                print("Warning: writeKkit:: No model found on " , modelpath)
                return False

def storeCplxEnzMsgs( enz, f ):
        for sub in enz.neighbors["subOut"]:
                s = "addmsg /kinetics/" + trimPath( sub ) + " /kinetics/" + trimPath(enz) + " SUBSTRATE n \n";
                s = s+ "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath( sub ) +        " REAC sA B \n";
                f.write(s)
        for prd in enz.neighbors["prd"]:
                s = "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath(prd) + " MM_PRD pA\n";
                f.write( s )
        for enzOut in enz.neighbors["enzOut"]:
                s = "addmsg /kinetics/" + trimPath( enzOut ) + " /kinetics/" + trimPath(enz) + " ENZYME n\n";
                s = s+ "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath(enzOut) + " REAC eA B\n";
                f.write( s )

def storeMMenzMsgs( enz, f):
        subList = enz.neighbors["subOut"]
        prdList = enz.neighbors["prd"]
        enzDestList = enz.neighbors["enzDest"]
        for esub in subList:
                es = "addmsg /kinetics/" + trimPath(element(esub)) + " /kinetics/" + trimPath(enz) + " SUBSTRATE n \n";
                es = es+"addmsg /kinetics/" + trimPath(enz) + " /kinetics/" + trimPath(element(esub)) + " REAC sA B \n";
                f.write(es)

        for eprd in prdList:
                es = "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath( element(eprd)) + " MM_PRD pA \n";
                f.write(es)
        for eenzDest in enzDestList:
                enzDest = "addmsg /kinetics/" + trimPath( element(eenzDest)) + " /kinetics/" + trimPath( enz ) + " ENZYME n \n";
                f.write(enzDest)

def storeEnzMsg( enzList, f):
        for enz in enzList:
                enzClass = enz.className
                if (enzClass == "ZombieMMenz" or enzClass == "MMenz"):
                        storeMMenzMsgs(enz, f)
                else:
                        storeCplxEnzMsgs( enz, f )

def writeEnz( modelpath,f):
        enzList = wildcardFind(modelpath+'/##[ISA=EnzBase]')
        for enz in enzList:
                x = random.randrange(0,10)
                y = random.randrange(0,10)
                textcolor = "green"
                color = "red"
                k1 = 0;
                k2 = 0;
                k3 = 0;
                nInit = 0;
                concInit = 0;
                n = 0;
                conc = 0;
                enzParent = enz.parent
                if (isinstance(enzParent.className,Pool)) or (isinstance(enzParent.className,ZombiePool)):
                        print(" raise exception enz doesn't have pool as parent")
                        return False
                else:
                        vol = enzParent.volume * NA * 1e-3;
                        isMichaelisMenten = 0;
                        enzClass = enz.className
                        if (enzClass == "ZombieMMenz" or enzClass == "MMenz"):
                                k1 = enz.numKm
                                k3 = enz.kcat
                                k2 = 4.0*k3;
                                k1 = (k2 + k3) / k1;
                                isMichaelisMenten = 1;

                        elif (enzClass == "ZombieEnz" or enzClass == "Enz"):
                                k1 = enz.k1
                                k2 = enz.k2
                                k3 = enz.k3
                                cplx = enz.neighbors['cplx'][0]
                                nInit = cplx.nInit[0];

                        xe = cord[enz]['x']
                        ye = cord[enz]['y']
                        x = ((xe-xmin)/(xmax-xmin))*multi
                        y = ((ye-ymin)/(ymax-ymin))*multi
                        #y = ((ymax-ye)/(ymax-ymin))*multi
                        einfo = enz.path+'/info'
                        if exists(einfo):
                                color = Annotator(einfo).getField('color')
                                color = getColorCheck(color,GENESIS_COLOR_SEQUENCE)

                                textcolor = Annotator(einfo).getField('textColor')
                                textcolor = getColorCheck(textcolor,GENESIS_COLOR_SEQUENCE)

                        f.write("simundump kenz /kinetics/" + trimPath(enz) + " " + str(0)+  " " +
                                        str(concInit) + " " +
                                        str(conc) + " " +
                                        str(nInit) + " " +
                                        str(n) + " " +
                                        str(vol) + " " +
                                        str(k1) + " " +
                                        str(k2) + " " +
                                        str(k3) + " " +
                                        str(0) + " " +
                                        str(isMichaelisMenten) + " " +
                                        "\"\"" + " " +
                                        str(color) + " " + str(textcolor) + " \"\"" +
                                        " " + str(int(x)) + " " + str(int(y)) + " "+str(0)+"\n")
        return enzList

def nearestColorIndex(color, color_sequence):
        #Trying to find the index to closest color map from the rainbow pickle file for matching the Genesis color map
        distance = [ (color[0] - temp[0]) ** 2 + (color[1] - temp[1]) ** 2 + (color[2] - temp[2]) ** 2
                     for temp in color_sequence]

        minindex = 0

        for i in range(1, len(distance)):
                if distance[minindex] > distance[i] : minindex = i

        return minindex

def storeReacMsg(reacList,f):
        for reac in reacList:
                reacPath = trimPath( reac);
                sublist = reac.neighbors["subOut"]
                prdlist = reac.neighbors["prd"]
                for sub in sublist:
                        s = "addmsg /kinetics/" + trimPath( sub ) + " /kinetics/" + reacPath +  " SUBSTRATE n \n";
                        s =  s + "addmsg /kinetics/" + reacPath + " /kinetics/" + trimPath( sub ) +  " REAC A B \n";
                        f.write(s)

                for prd in prdlist:
                        s = "addmsg /kinetics/" + trimPath( prd ) + " /kinetics/" + reacPath + " PRODUCT n \n";
                        s = s + "addmsg /kinetics/" + reacPath + " /kinetics/" + trimPath( prd ) +  " REAC B A\n";
                        f.write( s)

def writeReac(modelpath,f):
        reacList = wildcardFind(modelpath+'/##[ISA=ReacBase]')
        for reac in reacList :
                color = "blue"
                textcolor = "red"
                kf = reac.numKf
                kb = reac.numKb
                xr = cord[reac]['x']
                yr = cord[reac]['y']
                x = ((xr-xmin)/(xmax-xmin))*multi
                y = ((yr-ymin)/(ymax-ymin))*multi
                #y = ((ymax-yr)/(ymax-ymin))*multi
                rinfo = reac.path+'/info'
                if exists(rinfo):
                        color = Annotator(rinfo).getField('color')
                        color = getColorCheck(color,GENESIS_COLOR_SEQUENCE)

                        textcolor = Annotator(rinfo).getField('textColor')
                        textcolor = getColorCheck(textcolor,GENESIS_COLOR_SEQUENCE)

                f.write("simundump kreac /kinetics/" + trimPath(reac) + " " +str(0) +" "+ str(kf) + " " + str(kb) + " \"\" " +
                        str(color) + " " + str(textcolor) + " " + str(int(x)) + " " + str(int(y)) + " 0\n")
        return reacList

def trimPath(mobj):
        original = mobj
        mobj = element(mobj)
        found = False
        while not isinstance(mobj,ChemCompt) and mobj.path != "/":
                mobj = element(mobj.parent)
                found = True
        if mobj.path == "/":
                print("compartment is not found with the given path and the path has reached root ",original)
                return
        #other than the kinetics compartment, all the othername are converted to group in Genesis which are place under /kinetics
        # Any moose object comes under /kinetics then one level down the path is taken.
        # e.g /group/poolObject or /Reac
        if found:
                if mobj.name != "kinetics":
                        splitpath = original.path[(original.path.find(mobj.name)):len(original.path)]
                else:

                        pos = original.path.find(mobj.name)
                        slash = original.path.find('/',pos+1)
                        splitpath = original.path[slash+1:len(original.path)]
                splitpath = re.sub("\[[0-9]+\]", "", splitpath)
                s = splitpath.replace("_dash_",'-')
                return s

def writeSumtotal( modelpath,f):
        funclist = wildcardFind(modelpath+'/##[ISA=Function]')
        for func in funclist:
                funcInputs = element(func.path+'/x[0]')
                s = ""
                for funcInput in funcInputs.neighbors["input"]:
                        s = s+ "addmsg /kinetics/" + trimPath(funcInput)+ " /kinetics/" + trimPath(element(func.parent)) + " SUMTOTAL n nInit\n"
                f.write(s)

def storePlotMsgs( tgraphs,f):
        s = ""
        if tgraphs:
                for graph in tgraphs:
                        slash = graph.path.find('graphs')
                        if not slash > -1:
                                slash = graph.path.find('graph_0')
                        if slash > -1:
                                conc = graph.path.find('conc')
                                if conc > -1 :
                                        tabPath = graph.path[slash:len(graph.path)]
                                else:
                                        slash1 = graph.path.find('/',slash)
                                        tabPath = "/graphs/conc1" +graph.path[slash1:len(graph.path)]

                                if len(element(graph).msgOut):
                                        poolPath = (element(graph).msgOut)[0].e2.path
                                        poolEle = element(poolPath)
                                        poolName = poolEle.name
                                        bgPath = (poolEle.path+'/info')
                                        bg = Annotator(bgPath).color
                                        bg = getColorCheck(bg,GENESIS_COLOR_SEQUENCE)
                                        tabPath = re.sub("\[[0-9]+\]", "", tabPath)
                                        s = s+"addmsg /kinetics/" + trimPath( poolEle ) + " " + tabPath + \
                                                " PLOT Co *" + poolName + " *" + str(bg) +"\n";
        f.write(s)

def writeplot( tgraphs,f ):
        if tgraphs:
                for graphs in tgraphs:
                        slash = graphs.path.find('graphs')
                        if not slash > -1:
                                slash = graphs.path.find('graph_0')
                        if slash > -1:
                                conc = graphs.path.find('conc')
                                if conc > -1 :
                                        tabPath = "/"+graphs.path[slash:len(graphs.path)]
                                else:
                                        slash1 = graphs.path.find('/',slash)
                                        tabPath = "/graphs/conc1" +graphs.path[slash1:len(graphs.path)]

                                if len(element(graphs).msgOut):
                                        poolPath = (element(graphs).msgOut)[0].e2.path
                                        poolEle = element(poolPath)
                                        poolAnno = (poolEle.path+'/info')
                                        fg = Annotator(poolAnno).textColor
                                        fg = getColorCheck(fg,GENESIS_COLOR_SEQUENCE)
                                        tabPath = re.sub("\[[0-9]+\]", "", tabPath)
                                        f.write("simundump xplot " + tabPath + " 3 524288 \\\n" + "\"delete_plot.w <s> <d>; edit_plot.D <w>\" " + fg + " 0 0 1\n")

def writePool(modelpath,f,volIndex):
        for p in wildcardFind(modelpath+'/##[ISA=PoolBase]'):
                slave_enable = 0
                if (p.className == "BufPool" or p.className == "ZombieBufPool"):
                        pool_children = p.children
                        if pool_children== 0:
                                slave_enable = 4
                        else:
                                for pchild in pool_children:
                                        if not(pchild.className == "ZombieFunction") and not(pchild.className == "Function"):
                                                slave_enable = 4
                                        else:
                                                slave_enable = 0
                                                break
                #Eliminated enzyme complex pool
                if ((p.parent).className != "Enz" and (p.parent).className != "ZombieEnz"):
                    xp = cord[p]['x']
                    yp = cord[p]['y']
                    x = ((xp-xmin)/(xmax-xmin))*multi
                    y = ((yp-ymin)/(ymax-ymin))*multi
                    #y = ((ymax-yp)/(ymax-ymin))*multi
                    pinfo = p.path+'/info'
                    if exists(pinfo):
                            color = Annotator(pinfo).getField('color')
                            color = getColorCheck(color,GENESIS_COLOR_SEQUENCE)

                            textcolor = Annotator(pinfo).getField('textColor')
                            textcolor = getColorCheck(textcolor,GENESIS_COLOR_SEQUENCE)
                    geometryName = volIndex[p.volume]
                    volume = p.volume * NA * 1e-3
                    f.write("simundump kpool /kinetics/" + trimPath(p) + " 0 " +
                            str(p.diffConst) + " " +
                            str(0) + " " +
                            str(0) + " " +
                            str(0) + " " +
                            str(p.nInit) + " " +
                            str(0) + " " + str(0) + " " +
                            str(volume)+ " " +
                            str(slave_enable) +
                            " /kinetics"+ geometryName + " " +
                            str(color) +" " + str(textcolor) + " " + str(int(x)) + " " + str(int(y)) + " "+ str(0)+"\n")
                # print " notes ",notes
                # return notes

def getColorCheck(color,GENESIS_COLOR_SEQUENCE):
        if isinstance(color, str):
                if color.startswith("#"):
                        color = ( int(color[1:3], 16)
                                            , int(color[3:5], 16)
                                            , int(color[5:7], 16)
                                            )
                        index = nearestColorIndex(color, GENESIS_COLOR_SEQUENCE)
                        return index
                elif color.startswith("("):
                        color = eval(color)[0:3]
                        index = nearestColorIndex(color, GENESIS_COLOR_SEQUENCE)
                        return index
                else:
                        index = color
                        return index
        elif isinstance(color, tuple):
                color = map(int, color)[0:3]
                index = nearestColorIndex(color, GENESIS_COLOR_SEQUENCE)
                return index
        elif isinstance(color, int):
                index = color
                return index
        else:
                raise Exception("Invalid Color Value!")

def getxyCord(xcord,ycord,list1,sceneitems):
        for item in list1:
                if not ( isinstance(item,Function) and isinstance(item,ZombieFunction) ):
                        if sceneitems == None:
                                objInfo = item.path+'/info'
                                xpos = xyPosition(objInfo,'x')
                                ypos = xyPosition(objInfo,'y')
                        else:
                                co = sceneitems[item]
                                xpos = co.scenePos().x()
                                ypos =-co.scenePos().y()
                                #xpos = co['x']
                                #ypos = co['y']

                        cord[item] ={ 'x': xpos,'y':ypos}
                        xcord.append(xpos)
                        ycord.append(ypos)

def xyPosition(objInfo,xory):
    try:
        return(float(element(objInfo).getField(xory)))
    except ValueError:
        return (float(0))
def getCor(modelRoot,sceneitems):
        xmin = ymin = 0.0
        xmax = ymax = 1.0
        positionInfoExist = False
        xcord = []
        ycord = []
        mollist = realist = enzlist = cplxlist = tablist = funclist = []
        meshEntryWildcard = '/##[ISA=ChemCompt]'
        if modelRoot != '/':
                meshEntryWildcard = modelRoot+meshEntryWildcard
        for meshEnt in wildcardFind(meshEntryWildcard):
                mol_cpl  = wildcardFind(meshEnt.path+'/##[ISA=PoolBase]')
                realist  = wildcardFind(meshEnt.path+'/##[ISA=ReacBase]')
                enzlist  = wildcardFind(meshEnt.path+'/##[ISA=EnzBase]')
                funclist = wildcardFind(meshEnt.path+'/##[ISA=Function]')
                tablist  = wildcardFind(meshEnt.path+'/##[ISA=StimulusTable]')
                if mol_cpl or funclist or enzlist or realist or tablist:
                        for m in mol_cpl:
                                if isinstance(element(m.parent),CplxEnzBase):
                                        cplxlist.append(m)
                                        objInfo = m.parent.path+'/info'
                                elif isinstance(element(m),PoolBase):
                                        mollist.append(m)
                                        objInfo =m.path+'/info'
                                        #xx = xyPosition(objInfo,'x')
                                        #yy = xyPosition(objInfo,'y')
                                        
                                
                                if sceneitems == None:
                                        xx = xyPosition(objInfo,'x')
                                        yy = xyPosition(objInfo,'y')
                                else:
                                    c = sceneitems[m]
                                    xx = c.scenePos().x()
                                    yy =-c.scenePos().y()
                                    #listq = sceneitems[m]
                                    #xx = listq['x']
                                    #yy = listq['y']
                                    
                                cord[m] ={ 'x': xx,'y':yy}
                                xcord.append(xx)
                                ycord.append(yy)
                                
                        getxyCord(xcord,ycord,realist,sceneitems)
                        getxyCord(xcord,ycord,enzlist,sceneitems)
                        getxyCord(xcord,ycord,funclist,sceneitems)
                        getxyCord(xcord,ycord,tablist,sceneitems)
        xmin = min(xcord)
        xmax = max(xcord)
        ymin = min(ycord)
        ymax = max(ycord)
        positionInfoExist = not(len(np.nonzero(xcord)[0]) == 0 \
                and len(np.nonzero(ycord)[0]) == 0)

        return(xmin,ymin,xmax,ymax,positionInfoExist)

def writeCompartment(modelpath,compts,f):
        index = 0
        volIndex = {}
        for compt in compts:
                if compt.name != "kinetics":
                        xgrp = xmax -random.randrange(1,10)
                        ygrp = ymin +random.randrange(1,10)
                        x = ((xgrp-xmin)/(xmax-xmin))*multi
                        #y = ((ymax-ygrp)/(ymax-ymin))*multi
                        y = ((ygrp-ymin)/(ymax-ymin))*multi
                        f.write("simundump group /kinetics/" + compt.name + " 0 " + "blue" + " " + "green"       + " x 0 0 \"\" defaultfile \\\n" )
                        f.write( "  defaultfile.g 0 0 0 " + str(int(x)) + " " + str(int(y)) + " 0\n")
        i = 0
        l = len(compts)
        geometry = ""
        for compt in compts:
                size = compt.volume
                ndim = compt.numDimensions
                vecIndex = l-i-1
                #print vecIndex
                i = i+1
                xgeo = xmax -random.randrange(1,10)
                ygeo = ymin +random.randrange(1,10)
                x = ((xgeo-xmin)/(xmax-xmin))*multi
                #y = ((ymax-ygeo)/(ymax-ymin))*multi
                y = ((ygeo-ymin)/(ymax-ymin))*multi
                if vecIndex > 0:
                        geometry = geometry+"simundump geometry /kinetics" +  "/geometry[" + str(vecIndex) +"] 0 " + str(size) + " " + str(ndim) + " sphere " +" \"\" white black "+ str(int(x)) + " " +str(int(y)) +" 0\n";
                        volIndex[size] = "/geometry["+str(vecIndex)+"]"
                else:
                        geometry = geometry+"simundump geometry /kinetics"  +  "/geometry 0 " + str(size) + " " + str(ndim) + " sphere " +" \"\" white black " + str(int(x)) + " "+str(int(y))+ " 0\n";
                        volIndex[size] = "/geometry"
                f.write(geometry)
        writeGroup(modelpath,f,xmax,ymax)
        return volIndex

def writeGroup(modelpath,f,xmax,ymax):
        ignore = ["graphs","moregraphs","geometry","groups","conc1","conc2","conc3","conc4","model","data","graph_0","graph_1","graph_2","graph_3","graph_4","graph_5"]
        for g in wildcardFind(modelpath+'/##[TYPE=Neutral]'):
                if not g.name in ignore:
                        if trimPath(g) != None:
                                xgrp1 = xmax - random.randrange(1,10)
                                ygrp1 = ymin + random.randrange(1,10)
                                x = ((xgrp1-xmin)/(xmax-xmin))*multi
                                #y = ((ymax-ygrp1)/(ymax-ymin))*multi
                                y = ((ygrp1-ymin)/(ymax-ymin))*multi
                                f.write("simundump group /kinetics/" + trimPath(g) + " 0 " +    "blue" + " " + "green"   + " x 0 0 \"\" defaultfile \\\n")
                                f.write("  defaultfile.g 0 0 0 " + str(int(x)) + " " + str(int(y)) + " 0\n")

def writeHeader(f,maxVol):
        simdt = 0.001
        plotdt = 0.1
        rawtime = 100
        maxtime = 100
        defaultVol = maxVol
        f.write("//genesis\n"
                        "// kkit Version 11 flat dumpfile\n\n"
                        "// Saved on " + str(rawtime)+"\n"
                        "include kkit {argv 1}\n"
                        "FASTDT = " + str(simdt)+"\n"
                        "SIMDT = " +str(simdt)+"\n"
                        "CONTROLDT = " +str(plotdt)+"\n"
                        "PLOTDT = " +str(plotdt)+"\n"
                        "MAXTIME = " +str(maxtime)+"\n"
                        "TRANSIENT_TIME = 2"+"\n"
                        "VARIABLE_DT_FLAG = 0"+"\n"
                        "DEFAULT_VOL = " +str(defaultVol)+"\n"
                        "VERSION = 11.0 \n"
                        "setfield /file/modpath value ~/scripts/modules\n"
                        "kparms\n\n"
                        )
        f.write( "//genesis\n"
                        "initdump -version 3 -ignoreorphans 1\n"
                        "simobjdump table input output alloced step_mode stepsize x y z\n"
                        "simobjdump xtree path script namemode sizescale\n"
                        "simobjdump xcoredraw xmin xmax ymin ymax\n"
                        "simobjdump xtext editable\n"
                        "simobjdump xgraph xmin xmax ymin ymax overlay\n"
                        "simobjdump xplot pixflags script fg ysquish do_slope wy\n"
                        "simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \\\n"
                                "  link savename file version md5sum mod_save_flag x y z\n"
                        "simobjdump geometry size dim shape outside xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable \\\n"
                                "  geomname xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \\\n"
                                "  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z\n"
                        "simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \\\n"
                                "  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z\n"
                        "simobjdump xtab input output alloced step_mode stepsize notes editfunc \\\n"
                                "  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z\n"
                        "simobjdump kchan perm gmax Vm is_active use_nernst notewriteReacs xtree_fg_req \\\n"
                                "  xtree_textfg_req x y z\n"
                        "simobjdump transport input output alloced step_mode stepsize dt delay clock \\\n"
                                "  kf xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump proto x y z\n"
                        )

def estimateDefaultVol(compts):
        maxVol = 0
        vol = []
        for compt in compts:
                vol.append(compt.volume)
        if len(vol) > 0:
                return max(vol)
        return maxVol

def writeGui( f ):
        f.write("simundump xgraph /graphs/conc1 0 0 99 0.001 0.999 0\n"
        "simundump xgraph /graphs/conc2 0 0 100 0 1 0\n"
        "simundump xgraph /moregraphs/conc3 0 0 100 0 1 0\n"
        "simundump xgraph /moregraphs/conc4 0 0 100 0 1 0\n"
        "simundump xcoredraw /edit/draw 0 -6 4 -2 6\n"
        "simundump xtree /edit/draw/tree 0 \\\n"
        "  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \"edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>\" auto 0.6\n"
        "simundump xtext /file/notes 0 1\n")
def writeNotes(modelpath,f):
    notes = ""
    items = []
    items = wildcardFind(modelpath+"/##[ISA=ChemCompt]") +\
            wildcardFind(modelpath+"/##[ISA=PoolBase]") +\
            wildcardFind(modelpath+"/##[ISA=ReacBase]") +\
            wildcardFind(modelpath+"/##[ISA=EnzBase]") +\
            wildcardFind(modelpath+"/##[ISA=Function]") +\
            wildcardFind(modelpath+"/##[ISA=StimulusTable]")
    for item in items:
        if exists(item.path+'/info'):
            info = item.path+'/info'
            notes = Annotator(info).getField('notes')
            if (notes):
                f.write("call /kinetics/"+ trimPath(item)+"/notes LOAD \ \n\""+Annotator(info).getField('notes')+"\"\n")
    
def writeFooter1(f):
    f.write("\nenddump\n // End of dump\n")
def writeFooter2(f):
    f.write("complete_loading\n")

if __name__ == "__main__":
        import sys

        filename = sys.argv[1]
        modelpath = filename[0:filename.find('.')]
        loadModel(filename,'/'+modelpath,"gsl")
        output = filename.g
        written = write('/'+modelpath,output)
        if written:
                print(" file written to ",output)
        else:
                print(" could be written to kkit format")
