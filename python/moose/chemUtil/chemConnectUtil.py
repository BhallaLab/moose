# -*- coding: utf-8 -*-
""" ChemconnectUtil.py Some of the command function are written """

__author__           = "Harsha Rani"
__copyright__        = "Copyright 2017, Harsha Rani and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Harsha Rani"
__email__            = "hrani@ncbs.res.in"
__status__           = "Development"
__updated__          = "Mar 11 2019"

import moose
import numpy as np
import pickle
import matplotlib
import os
import random


direct = os.path.dirname(__file__)
colormap_file = open(os.path.join(direct, "rainbow2.pkl"), "r")  
colorMap = pickle.loads(colormap_file.read().encode('utf8'))
colormap_file.close()
ignoreColor= ["mistyrose","antiquewhite","aliceblue","azure","bisque","black","blanchedalmond","blue","cornsilk","darkolivegreen","darkslategray","dimgray","floralwhite","gainsboro","ghostwhite","honeydew","ivory","lavender","lavenderblush","lemonchiffon","lightcyan","lightgoldenrodyellow","lightgray","lightyellow","linen","mediumblue","mintcream","navy","oldlace","papayawhip","saddlebrown","seashell","snow","wheat","white","whitesmoke","aquamarine","lightsalmon","moccasin","limegreen","snow","sienna","beige","dimgrey","lightsage"]
matplotcolor = {}
for name,hexno in matplotlib.colors.cnames.items():
    matplotcolor[name]=hexno

def getRandColor():
    k = random.choice(matplotcolor.keys())
    if k in ignoreColor:
        return getRandColor()
    else:
        #return matplotcolor[k]
        return str(matplotlib.colors.cnames[k])

def getColor(iteminfo):
    """ Getting a textcolor and background color for the given  mooseObject \
        If textcolor is empty replaced with green \
           background color is empty replaced with blue
           if textcolor and background is same as it happend in kkit files \
           replacing textcolor with random color\
           The colors are not valid there are siliently replaced with some values \
           but while model building can raise an exception
    """
    textcolor = moose.element(iteminfo).textColor
    bgcolor   = moose.element(iteminfo).color
    if(textcolor == ''): textcolor = 'green'
    
    if(bgcolor == ''): bgcolor = 'orange'

    if(textcolor == bgcolor):
        textcolor = getRandColor()

    textcolor = colorCheck(textcolor)
    bgcolor = colorCheck(bgcolor)
    # if moose.exists(iteminfo):
    #     moose.element(iteminfo).textColor = textcolor
    # moose.element(iteminfo).color = bgcolor
    return(textcolor,bgcolor)

def colorCheck(fc_bgcolor):
    """ textColor or background can be anything like string or tuple or list \
        if string its taken as colorname further down in validColorcheck checked for valid color, \
        but for tuple and list its taken as r,g,b value.
    """
    if isinstance(fc_bgcolor,str):
        if fc_bgcolor.startswith("#"):
            fc_bgcolor = fc_bgcolor
        elif fc_bgcolor.isdigit():
            """ color is int  a map from int to r,g,b triplets from pickled color map file """
            tc = (int(fc_bgcolor))*2
            if tc < len(colorMap):
                pickledColor = colorMap[tc]
            else:
                pickledColor = (255, 0, 0)
            fc_bgcolor = '#%02x%02x%02x' % (pickledColor)

        elif fc_bgcolor.isalpha() or fc_bgcolor.isalnum():
            fc_bgcolor = validColorcheck(fc_bgcolor)
            
        else:
            for r in ['[',']','(',')']:
                fc_bgcolor = fc_bgcolor.replace(r,"")
            fc_bgcolor = fc_bgcolor.split(",")
            c = 0
            hexlist ="#"
            for n in fc_bgcolor:
                if c < 3:
                    hexlist = hexlist+str("%02x" % int(n))
                    c = c+1;
            fc_bgcolor = hexlist
        return(fc_bgcolor)

def validColorcheck(color):
    ''' 
        Both in Qt4.7 and 4.8 if not a valid color it makes it as back but in 4.7 there will be a warning mssg which is taken here
        checking if textcolor or backgroundcolor is valid color, if 'No' making white color as default
        where I have not taken care for checking what will be backgroundcolor for textcolor or textcolor for backgroundcolor 
    '''
    #if QColor(color).isValid():
    if matplotlib.colors.is_color_like(color):
        if color == "blue":
            color = "orange"
        color =  matplotlib.colors.cnames[color.lower()]
        return color
    else:
        return(matplotlib.colors.cnames["orange"])

def xyPosition(objInfo,xory):
    try:
        return float(moose.element(objInfo).getField(xory))
    except ValueError as e:
        return float(0)

def setupMeshObj(modelRoot):
    ''' Setup compartment and its members pool,reaction,enz cplx under self.meshEntry dictionaries \
    self.meshEntry with "key" as compartment,
    value is key2:list where key2 represents moose object type,list of objects of a perticular type
    e.g self.meshEntry[meshEnt] = { 'reaction': reaction_list,'enzyme':enzyme_list,'pool':poollist,'cplx': cplxlist }
    '''
    xmin = 0.0
    xmax = 1.0
    ymin = 0.0
    ymax = 1.0
    listOfitems = {}
    positionInfoExist = True
    meshEntry = {}
    if meshEntry:
        meshEntry.clear()
    else:
        meshEntry = {}
    xcord = []
    ycord = []
    meshEntryWildcard = '/##[ISA=ChemCompt]'
    if modelRoot != '/':
        meshEntryWildcard = modelRoot+meshEntryWildcard
    for meshEnt in moose.wildcardFind(meshEntryWildcard):
        mollist  = []
        realist  = []
        enzlist  = []
        cplxlist = []
        tablist  = []
        funclist = []

        mol_cpl  = moose.wildcardFind(meshEnt.path+'/##[ISA=PoolBase]')
        funclist = moose.wildcardFind(meshEnt.path+'/##[ISA=Function]')
        enzlist  = moose.wildcardFind(meshEnt.path+'/##[ISA=EnzBase]')
        realist  = moose.wildcardFind(meshEnt.path+'/##[ISA=ReacBase]')
        tablist  = moose.wildcardFind(meshEnt.path+'/##[ISA=StimulusTable]')
        if mol_cpl or funclist or enzlist or realist or tablist:
            for m in mol_cpl:
                if isinstance(moose.element(m.parent),moose.CplxEnzBase):
                    cplxlist.append(m)
                    objInfo = m.parent.path+'/info'
                elif isinstance(moose.element(m),moose.PoolBase):
                    mollist.append(m)
                    objInfo =m.path+'/info'
                if moose.exists(objInfo):
                    listOfitems[moose.element(moose.element(objInfo).parent)]={'x':xyPosition(objInfo,'x'),'y':xyPosition(objInfo,'y')}

                xcord.append(xyPosition(objInfo,'x'))
                ycord.append(xyPosition(objInfo,'y'))

            getxyCord(xcord,ycord,funclist,listOfitems)
            getxyCord(xcord,ycord,enzlist,listOfitems)
            getxyCord(xcord,ycord,realist,listOfitems)
            getxyCord(xcord,ycord,tablist,listOfitems)

            meshEntry[meshEnt] = {'enzyme':enzlist,
                                  'reaction':realist,
                                  'pool':mollist,
                                  'cplx':cplxlist,
                                  'table':tablist,
                                  'function':funclist
                                  }
            positionInfoExist = not(len(np.nonzero(xcord)[0]) == 0 \
                and len(np.nonzero(ycord)[0]) == 0)
            if positionInfoExist:
                xmin = min(xcord)
                xmax = max(xcord)
                ymin = min(ycord)
                ymax = max(ycord)
    return meshEntry,xmin,xmax,ymin,ymax,positionInfoExist,listOfitems
            
def getxyCord(xcord,ycord,list1,listOfitems):
    for item in list1:
        # if isinstance(item,Function):
        #     objInfo = moose.element(item.parent).path+'/info'
        # else:
        #     objInfo = item.path+'/info'
        if not isinstance(item,moose.Function):
            objInfo = item.path+'/info'
            xcord.append(xyPosition(objInfo,'x'))
            ycord.append(xyPosition(objInfo,'y'))
            if moose.exists(objInfo):
                listOfitems[moose.element(moose.element(objInfo).parent)]={'x':xyPosition(objInfo,'x'),'y':xyPosition(objInfo,'y')}

def setupItem(modelPath,cntDict):
    '''This function collects information of what is connected to what. \
    eg. substrate and product connectivity to reaction's and enzyme's \
    sumtotal connectivity to its pool are collected '''
    #print " setupItem"
    sublist = []
    prdlist = []
    zombieType = ['ReacBase','EnzBase','Function','StimulusTable']
    for baseObj in zombieType:
        path = '/##[ISA='+baseObj+']'
        if modelPath != '/':
            path = modelPath+path
        if ( (baseObj == 'ReacBase') or (baseObj == 'EnzBase')):
            for items in moose.wildcardFind(path):
                sublist = []
                prdlist = []
                uniqItem,countuniqItem = countitems(items,'subOut')
                subNo = uniqItem
                for sub in uniqItem:
                    sublist.append((moose.element(sub),'s',countuniqItem[sub]))

                uniqItem,countuniqItem = countitems(items,'prd')
                prdNo = uniqItem
                if (len(subNo) == 0 or len(prdNo) == 0):
                    print ("\nSubstrate Product is empty for "+items.path)

                for prd in uniqItem:
                    prdlist.append((moose.element(prd),'p',countuniqItem[prd]))

                if (baseObj == 'CplxEnzBase') :
                    uniqItem,countuniqItem = countitems(items,'toEnz')
                    for enzpar in uniqItem:
                        sublist.append((moose.element(enzpar),'t',countuniqItem[enzpar]))

                    uniqItem,countuniqItem = countitems(items,'cplxDest')
                    for cplx in uniqItem:
                        prdlist.append((moose.element(cplx),'cplx',countuniqItem[cplx]))

                if (baseObj == 'EnzBase'):
                    uniqItem,countuniqItem = countitems(items,'enzDest')
                    for enzpar in uniqItem:
                        sublist.append((moose.element(enzpar),'t',countuniqItem[enzpar]))
                cntDict[items] = sublist,prdlist
        elif baseObj == 'Function':
            for items in moose.wildcardFind(path):
                sublist = []
                prdlist = []
                item = items.path+'/x[0]'
                uniqItem,countuniqItem = countitems(item,'input')
                for funcpar in uniqItem:
                    sublist.append((moose.element(funcpar),'sts',countuniqItem[funcpar]))

                uniqItem,countuniqItem = countitems(items,'valueOut')
                for funcpar in uniqItem:
                    prdlist.append((moose.element(funcpar),'stp',countuniqItem[funcpar]))
                cntDict[items] = sublist,prdlist

        # elif baseObj == 'Function':
        #     #ZombieSumFunc adding inputs
        #     inputlist = []
        #     outputlist = []
        #     funplist = []
        #     nfunplist = []
        #     for items in moose.wildcardFind(path):
        #         for funplist in moose.element(items).neighbors['valueOut']:
        #             for func in funplist:
        #                 funcx = moose.element(items.path+'/x[0]')
        #                 uniqItem,countuniqItem = countitems(funcx,'input')
        #                 for inPut in uniqItem:
        #                     inputlist.append((inPut,'st',countuniqItem[inPut]))
        #             cntDict[func] = inputlist
        else:
            for tab in moose.wildcardFind(path):
                tablist = []
                uniqItem,countuniqItem = countitems(tab,'output')
                for tabconnect in uniqItem:
                    tablist.append((moose.element(tabconnect),'tab',countuniqItem[tabconnect]))
                cntDict[tab] = tablist

def countitems(mitems,objtype):
    items = []
    items = moose.element(mitems).neighbors[objtype]
    uniqItems = set(items)
    #countuniqItems = Counter(items)
    countuniqItems = dict((i, items.count(i)) for i in items)
    return(uniqItems,countuniqItems)

def findCompartment(element):
    if element.path == '/':
        return moose.element('/')
    elif mooseIsInstance(element, ["CubeMesh", "CylMesh","EndoMesh","NeuroMesh"]):
        return (element)
    else:
        return findCompartment(moose.element(element.parent))
    

def mooseIsInstance(element, classNames):
    return moose.element(element).__class__.__name__ in classNames
