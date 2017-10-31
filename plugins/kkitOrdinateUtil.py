__author__      =   "HarshaRani"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "HarshaRani"
__email__       =   "hrani@ncbs.res.in"
__status__      =   "Development"
__updated__     =   "Oct 18 2017"

'''
Oct 18: moved some function to kkitUtil
getxyCord, etc function are added
'''
import collections
from moose import *
import numpy as np
from moose import wildcardFind,element,PoolBase,CplxEnzBase,Annotator,exists
from networkx.drawing.nx_agraph import graphviz_layout
import numpy as np
import networkx as nx
from kkitUtil import getRandColor,colorCheck,findCompartment, findGroup, findGroup_compt, mooseIsInstance
from PyQt4.QtGui import QColor

def getxyCord(xcord,ycord,list1):
    for item in list1:
        # if isinstance(item,Function):
        #     objInfo = element(item.parent).path+'/info'
        # else:
        #     objInfo = item.path+'/info'
        if not isinstance(item,Function):
            objInfo = item.path+'/info'
            xcord.append(xyPosition(objInfo,'x'))
            ycord.append(xyPosition(objInfo,'y'))

def xyPosition(objInfo,xory):
    try:
        return(float(element(objInfo).getField(xory)))
    except ValueError:
        return (float(0))
'''
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
'''
def populateMeshEntry(meshEntry,parent,types,obj):
    #print " parent ",parent, "types ",types, " obj ",obj
    try:
        value = meshEntry[element(parent.path)][types]
    except KeyError:
        # Key is not present
        meshEntry[element(parent.path)].update({types :[element(obj)]})
    else:
        mlist = meshEntry[element(parent.path)][types]
        mlist.append(element(obj))
def updateMeshObj(modelRoot):
    print " updateMeshObj "
    meshEntry = {}
    if meshEntry:
        meshEntry.clear()
    else:
        meshEntry = {}

    objPar = collections.OrderedDict()
    for compt in wildcardFind(modelRoot+'/##[ISA=ChemCompt]'):
        groupColor = []
        try:
            value = meshEntry[element(compt)]
        except KeyError:
            # Compt  is not present
            meshEntry[element(compt)] = {}
            objPar[element(compt)] = element('/')

        for grp in wildcardFind(compt.path+'/##[TYPE=Neutral]'):
            test = [x for x in wildcardFind(element(grp).path+'/#') if x.className in ["Pool","Reac","Enz"]]
            grp_cmpt = findGroup_compt(grp.parent)    

            try:
                value = meshEntry[element(grp)]
            except KeyError:
                # Grp is not present
                meshEntry[element(grp)] = {}
                objPar[element(grp)] = element(grp_cmpt)

    for compt in wildcardFind(modelRoot+'/##[ISA=ChemCompt]'):
        for m in wildcardFind(compt.path+'/##[ISA=PoolBase]'):
            grp_cmpt = findGroup_compt(m)
            if isinstance(element(grp_cmpt),Neutral):
                if isinstance(element(m.parent),EnzBase):
                    populateMeshEntry(meshEntry,grp_cmpt,"cplx",m)
                else:
                    populateMeshEntry(meshEntry,grp_cmpt,"pool",m)
            else:
                if isinstance(element(m.parent),EnzBase):
                    populateMeshEntry(meshEntry,compt,"cplx",m)
                else:
                    populateMeshEntry(meshEntry,compt,"pool",m)
        
        for r in wildcardFind(compt.path+'/##[ISA=ReacBase]'):
            rgrp_cmpt = findGroup_compt(r)
            if isinstance(element(rgrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,rgrp_cmpt,"reaction",r)
            else:
                populateMeshEntry(meshEntry,compt,"reaction",r)
        
        for e in wildcardFind(compt.path+'/##[ISA=EnzBase]'):
            egrp_cmpt = findGroup_compt(e)
            if isinstance(element(egrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,egrp_cmpt,"enzyme",e)
            else:
                populateMeshEntry(meshEntry,compt,"enzyme",e)

        for f in wildcardFind(compt.path+'/##[ISA=Function]'):
            fgrp_cmpt = findGroup_compt(f)
            if isinstance(element(fgrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,fgrp_cmpt,"function",f)
            else:
                populateMeshEntry(meshEntry,compt,"function",f)

        for t in wildcardFind(compt.path+'/##[ISA=StimulusTable]'):
            tgrp_cmpt = findGroup_compt(t)
            if isinstance(element(tgrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,tgrp_cmpt,"stimTab",t)
            else:
                populateMeshEntry(meshEntry,compt,"stimTab",t)
    return(objPar,meshEntry)

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
    positionInfoExist = True
    meshEntry = {}
    if meshEntry:
        meshEntry.clear()
    else:
        meshEntry = {}
    xcord = []
    ycord = []
    n = 1
    objPar = collections.OrderedDict()
    
    for compt in wildcardFind(modelRoot+'/##[ISA=ChemCompt]'):
        groupColor = []
        try:
            value = meshEntry[element(compt)]
        except KeyError:
            # Compt  is not present
            meshEntry[element(compt)] = {}
            objPar[element(compt)] = element('/')

        for grp in wildcardFind(compt.path+'/##[TYPE=Neutral]'):
            test = [x for x in wildcardFind(element(grp).path+'/#') if x.className in ["Pool","Reac","Enz"]]
            #if len(test) >1:
            grpinfo = Annotator(element(grp).path+'/info')
            validatecolor = colorCheck(grpinfo.color,"bg")
            validatedgrpcolor = str(QColor(validatecolor).name())

            groupColor.append(validatedgrpcolor)
            grp_cmpt = findGroup_compt(grp.parent)    

            try:
                value = meshEntry[element(grp)]
            except KeyError:
                # Grp is not present
                meshEntry[element(grp)] = {}
                objPar[element(grp)] = element(grp_cmpt)
                # if n > 1:
                #     validatecolor = colorCheck(grpinfo.color,"bg")
                #     validatedgrpcolor = str(QColor(validatecolor).name())
                #     if validatedgrpcolor in groupColor:
                #         print " inside "
                #         c = getRandColor()
                #         print " c ",c, c.name()
                #         grpinfo.color = str(c.name())
                #         groupColor.append(str(c.name()))
                # print " groupColor ",grpinfo,grpinfo.color, groupColor
                # n =n +1
    for compt in wildcardFind(modelRoot+'/##[ISA=ChemCompt]'):
        for m in wildcardFind(compt.path+'/##[ISA=PoolBase]'):
            grp_cmpt = findGroup_compt(m)

            xcord.append(xyPosition(m.path+'/info','x'))
            ycord.append(xyPosition(m.path+'/info','y')) 
            if isinstance(element(grp_cmpt),Neutral):
                if isinstance(element(m.parent),EnzBase):
                    populateMeshEntry(meshEntry,grp_cmpt,"cplx",m)
                else:
                    populateMeshEntry(meshEntry,grp_cmpt,"pool",m)
            else:
                if isinstance(element(m.parent),EnzBase):
                    populateMeshEntry(meshEntry,compt,"cplx",m)
                else:
                    populateMeshEntry(meshEntry,compt,"pool",m)
        
        for r in wildcardFind(compt.path+'/##[ISA=ReacBase]'):
            rgrp_cmpt = findGroup_compt(r)
            xcord.append(xyPosition(r.path+'/info','x'))
            ycord.append(xyPosition(r.path+'/info','y'))
            if isinstance(element(rgrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,rgrp_cmpt,"reaction",r)
            else:
                populateMeshEntry(meshEntry,compt,"reaction",r)
        
        for e in wildcardFind(compt.path+'/##[ISA=EnzBase]'):
            egrp_cmpt = findGroup_compt(e)
            xcord.append(xyPosition(e.path+'/info','x'))
            ycord.append(xyPosition(e.path+'/info','y'))

            if isinstance(element(egrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,egrp_cmpt,"enzyme",e)
            else:
                populateMeshEntry(meshEntry,compt,"enzyme",e)

        for f in wildcardFind(compt.path+'/##[ISA=Function]'):
            fgrp_cmpt = findGroup_compt(f)
            if isinstance(element(fgrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,fgrp_cmpt,"function",f)
            else:
                populateMeshEntry(meshEntry,compt,"function",f)

        for t in wildcardFind(compt.path+'/##[ISA=StimulusTable]'):
            tgrp_cmpt = findGroup_compt(t)
            xcord.append(xyPosition(t.path+'/info','x'))
            ycord.append(xyPosition(t.path+'/info','y'))
            if isinstance(element(tgrp_cmpt),Neutral):
                populateMeshEntry(meshEntry,tgrp_cmpt,"stimTab",t)
            else:
                populateMeshEntry(meshEntry,compt,"stimTab",t)
    
    xmin = min(xcord)
    xmax = max(xcord)
    ymin = min(ycord)
    ymax = max(ycord)
    positionInfoExist = not(len(np.nonzero(xcord)[0]) == 0 and len(np.nonzero(ycord)[0]) == 0)
    return(objPar,meshEntry,xmin,xmax,ymin,ymax,positionInfoExist)
'''
def setupMeshObj(modelRoot):
    # Setup compartment and its members pool,reaction,enz cplx under self.meshEntry dictionaries \ 
    # self.meshEntry with "key" as compartment, 
    # value is key2:list where key2 represents moose object type,list of objects of a perticular type
    # e.g self.meshEntry[meshEnt] = { 'reaction': reaction_list,'enzyme':enzyme_list,'pool':poollist,'cplx': cplxlist }
    
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
    for meshEnt in wildcardFind(meshEntryWildcard):
        mollist  = []
        realist  = []
        enzlist  = []
        cplxlist = []
        tablist  = []
        funclist = []

        mol_cpl  = wildcardFind(meshEnt.path+'/##[ISA=PoolBase]')
        funclist = wildcardFind(meshEnt.path+'/##[ISA=Function]')
        enzlist  = wildcardFind(meshEnt.path+'/##[ISA=EnzBase]')
        realist  = wildcardFind(meshEnt.path+'/##[ISA=ReacBase]')
        tablist  = wildcardFind(meshEnt.path+'/##[ISA=StimulusTable]')
        if mol_cpl or funclist or enzlist or realist or tablist:
            for m in mol_cpl:
                if isinstance(element(m.parent),CplxEnzBase):
                    cplxlist.append(m)
                elif isinstance(element(m),moose.PoolBase):
                    mollist.append(m)

            meshEntry[meshEnt] = {'enzyme':enzlist,
                                  'reaction':realist,
                                  'pool':mollist,
                                  'cplx':cplxlist,
                                  'table':tablist,
                                  'function':funclist
                                  }

        for mert in [mollist,enzlist,realist,tablist]:
            for merts in mert:
                objInfo = merts.path+'/info'
                if exists(objInfo):
                    xcord.append(element(objInfo).x)
                    ycord.append(element(objInfo).y)
    return(meshEntry,xcord,ycord)

def sizeHint(self):
    return QtCore.QSize(800,400)
'''
def setupItem(modelPath,cntDict):
    # This function collects information of what is connected to what. \
    # eg. substrate and product connectivity to reaction's and enzyme's \
    # sumtotal connectivity to its pool are collected 
    #print " setupItem"
    sublist = []
    prdlist = []
    zombieType = ['ReacBase','EnzBase','Function','StimulusTable']
    for baseObj in zombieType:
        path = '/##[ISA='+baseObj+']'
        if modelPath != '/':
            path = modelPath+path
        if ( (baseObj == 'ReacBase') or (baseObj == 'EnzBase')):
            for items in wildcardFind(path):
                sublist = []
                prdlist = []
                uniqItem,countuniqItem = countitems(items,'subOut')
                subNo = uniqItem
                for sub in uniqItem: 
                    sublist.append((element(sub),'s',countuniqItem[sub]))

                uniqItem,countuniqItem = countitems(items,'prd')
                prdNo = uniqItem
                if (len(subNo) == 0 or len(prdNo) == 0):
                    print ("Substrate Product is empty ",path, " ",items)
                    
                for prd in uniqItem:
                    prdlist.append((element(prd),'p',countuniqItem[prd]))
                
                if (baseObj == 'CplxEnzBase') :
                    uniqItem,countuniqItem = countitems(items,'toEnz')
                    for enzpar in uniqItem:
                        sublist.append((element(enzpar),'t',countuniqItem[enzpar]))
                    
                    uniqItem,countuniqItem = countitems(items,'cplxDest')
                    for cplx in uniqItem:
                        prdlist.append((element(cplx),'cplx',countuniqItem[cplx]))

                if (baseObj == 'EnzBase'):
                    uniqItem,countuniqItem = countitems(items,'enzDest')
                    for enzpar in uniqItem:
                        sublist.append((element(enzpar),'t',countuniqItem[enzpar]))
                cntDict[items] = sublist,prdlist
        elif baseObj == 'Function':
            for items in wildcardFind(path):
                sublist = []
                prdlist = []
                item = items.path+'/x[0]'
                uniqItem,countuniqItem = countitems(item,'input')
                for funcpar in uniqItem:
                    sublist.append((element(funcpar),'sts',countuniqItem[funcpar]))
                
                uniqItem,countuniqItem = countitems(items,'valueOut')
                for funcpar in uniqItem:
                    prdlist.append((element(funcpar),'stp',countuniqItem[funcpar]))
                cntDict[items] = sublist,prdlist

        else:
            for tab in wildcardFind(path):
                tablist = []
                uniqItem,countuniqItem = countitems(tab,'output')
                for tabconnect in uniqItem:
                    tablist.append((element(tabconnect),'tab',countuniqItem[tabconnect]))
                cntDict[tab] = tablist

def countitems(mitems,objtype):
    items = []
    items = element(mitems).neighbors[objtype]
    uniqItems = set(items)
    #countuniqItemsauto = Counter(items)
    countuniqItems = dict((i, items.count(i)) for i in items)
    return(uniqItems,countuniqItems)

def recalculatecoordinatesforKkit(mObjlist,xcord,ycord):
    
    positionInfoExist = not(len(np.nonzero(xcord)[0]) == 0 \
                        and len(np.nonzero(ycord)[0]) == 0)

    if positionInfoExist:
        #Here all the object has been taken now recalculate and reassign back x and y co-ordinates
        xmin = min(xcord)
        xmax = max(xcord)
        ymin = min(ycord)
        ymax = max(ycord)
        for merts in mObjlist:
            objInfo = merts.path+'/info'
            if moose.exists(objInfo):
                Ix = (xyPosition(objInfo,'x')-xmin)/(xmax-xmin)
                Iy = (ymin-xyPosition(objInfo,'y'))/(ymax-ymin)
                element(objInfo).x = Ix
                element(objInfo).y = Iy  
        
def xyPosition(objInfo,xory):
    try:
        return(float(element(objInfo).getField(xory)))
    except ValueError:
        return (float(0))

                                    
def autoCoordinates(meshEntry,srcdesConnection):
   
    G = nx.Graph()
    for cmpt,memb in meshEntry.items():
        if memb in ["enzyme"]:
            for enzObj in find_index(memb,'enzyme'):
                #G.add_node(enzObj.path)
                G.add_node(enzObj.path,label='',shape='ellipse',color='',style='filled',fontname='Helvetica',fontsize=12,fontcolor='blue')
    for cmpt,memb in meshEntry.items():
        #if memb.has_key
        if memb in ["pool","cplx","reaction"]:
            for poolObj in find_index(memb,'pool'):
                #G.add_node(poolObj.path)
                G.add_node(poolObj.path,label = poolObj.name,shape = 'box',color = '',style = 'filled',fontname = 'Helvetica',fontsize = 9,fontcolor = 'blue')
            for cplxObj in find_index(memb,'cplx'):
                G.add_node(cplxObj.path)
                G.add_node(cplxObj.path,label = cplxObj.name,shape = 'box',color = '',style = 'filled',fontname = 'Helvetica',fontsize = 12,fontcolor = 'blue')
                #G.add_edge((cplxObj.parent).path,cplxObj.path)
            for reaObj in find_index(memb,'reaction'):
                #G.add_node(reaObj.path)
                G.add_node(reaObj.path,label='',shape='circle',color='')
        
    for inn,out in srcdesConnection.items():
        if (inn.className =='ZombieReac'): arrowcolor = 'green'
        elif(inn.className =='ZombieEnz'): arrowcolor = 'red'
        else: arrowcolor = 'blue'
        if isinstance(out,tuple):
            if len(out[0])== 0:
                print (inn.className + ':' +inn.name + "  doesn't have input message")
            else:
                for items in (items for items in out[0] ):
                    G.add_edge(element(items[0]).path,inn.path)
            if len(out[1]) == 0:
                print (inn.className + ':' + inn.name + "doesn't have output mssg")
            else:
                for items in (items for items in out[1] ):
                    G.add_edge(inn.path,element(items[0]).path)
        elif isinstance(out,list):
            if len(out) == 0:
                print ("Func pool doesn't have sumtotal")
            else:
                for items in (items for items in out ):
                    G.add_edge(element(items[0]).path,inn.path)
    position = graphviz_layout(G)
    xcord, ycord = [],[]
    for item in position.items():
        xy = item[1]
        xroundoff = round(xy[0],0)
        yroundoff = round(xy[1],0)
        xcord.append(xroundoff)
        ycord.append(yroundoff)
    
    xmin = min(xcord)
    xmax = max(xcord)
    ymin = min(ycord)
    ymax = max(ycord)
    for item in position.items():
        xy = item[1]
        anno = Annotator(item[0]+'/info')
        Ax = (xy[0]-xmin)/(xmax-xmin)
        Ay = (xy[1]-ymin)/(ymax-ymin)
        #anno.x = round(Ax,1)
        #anno.y = round(Ay,1)
        #not roundingoff to max and min the co-ordinates for bigger model would overlay the co-ordinates
        anno.x = xy[0]
        anno.y = xy[1]
def find_index(value, key):
    """ Value.get(key) to avoid expection which would raise if empty value in dictionary for a given key """
    if value.get(key) != None:
        return value.get(key)
    else:
        raise ValueError('no dict with the key found')
