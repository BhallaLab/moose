__author__      =   "HarshaRani"
__credits__     =   ["Upi Lab"]
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "HarshaRani"
__email__       =   "hrani@ncbs.res.in"
__status__      =   "Development"
__updated__     =   "Feb 14 2017"

from moose import *
import numpy as np
from moose import wildcardFind,element,PoolBase,CplxEnzBase,Annotator,exists
import numpy as np
import networkx as nx
from networkx.drawing.nx_agraph import graphviz_layout


def setupMeshObj(modelRoot):
    ''' Setup compartment and its members pool,reaction,enz cplx under self.meshEntry dictionaries \ 
    self.meshEntry with "key" as compartment, 
    value is key2:list where key2 represents moose object type,list of objects of a perticular type
    e.g self.meshEntry[meshEnt] = { 'reaction': reaction_list,'enzyme':enzyme_list,'pool':poollist,'cplx': cplxlist }
    '''
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
                    print "Substrate Product is empty ",path, " ",items
                    
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
    print " kkit Ordinatesutil autoCoordinates "
    
    G = nx.Graph()
    #G = pgv.AGraph()
    positionInfo = {}
    for cmpt,memb in meshEntry.items():
        for enzObj in find_index(memb,'enzyme'):
            #G.add_node(enzObj.path)
            G.add_node(enzObj.path,label='',shape='ellipse',color='',style='filled',fontname='Helvetica',fontsize=12,fontcolor='blue')
    for cmpt,memb in meshEntry.items():
        for poolObj in find_index(memb,'pool'):
            #G.add_node(poolObj.path)
            G.add_node(poolObj.path,label = poolObj.name,shape = 'box',color = '',style = 'filled',fontname = 'Helvetica',fontsize = 12,fontcolor = 'blue')
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
                print inn.className + ':' +inn.name + "  doesn't have input message"
            else:
                for items in (items for items in out[0] ):
                    G.add_edge(element(items[0]).path,inn.path)
            if len(out[1]) == 0:
                print inn.className + ':' + inn.name + "doesn't have output mssg"
            else:
                for items in (items for items in out[1] ):
                    G.add_edge(inn.path,element(items[0]).path)
        elif isinstance(out,list):
            if len(out) == 0:
                print "Func pool doesn't have sumtotal"
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
        anno.x = round(Ax,1)
        anno.y = round(Ay,1)

def find_index(value, key):
    """ Value.get(key) to avoid expection which would raise if empty value in dictionary for a given key """
    if value.get(key) != None:
        return value.get(key)
    else:
        raise ValueError('no dict with the key found')
