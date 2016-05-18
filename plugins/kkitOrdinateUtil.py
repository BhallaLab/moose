from moose import *
import numpy as np
import networkx as nx
from collections import Counter

def xyPosition(objInfo,xory):
    try:
        return(float(element(objInfo).getField(xory)))
    except ValueError:
        return (float(0))

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
                    objInfo = m.parent.path+'/info'
                elif isinstance(element(m),moose.PoolBase):
                    mollist.append(m)
                    objInfo =m.path+'/info'
                xcord.append(xyPosition(objInfo,'x'))
                ycord.append(xyPosition(objInfo,'y')) 

            getxyCord(xcord,ycord,funclist)
            getxyCord(xcord,ycord,enzlist)
            getxyCord(xcord,ycord,realist)
            getxyCord(xcord,ycord,tablist)

            meshEntry[meshEnt] = {'enzyme':enzlist,
                                  'reaction':realist,
                                  'pool':mollist,
                                  'cplx':cplxlist,
                                  'table':tablist,
                                  'function':funclist
                                  }
            xmin = min(xcord)
            xmax = max(xcord)
            ymin = min(ycord)
            ymax = max(ycord)
            positionInfoExist = not(len(np.nonzero(xcord)[0]) == 0 \
                and len(np.nonzero(ycord)[0]) == 0)
    return(meshEntry,xmin,xmax,ymin,ymax,positionInfoExist)

def sizeHint(self):
    return QtCore.QSize(800,400)
def getxyCord(xcord,ycord,list1):
    for item in list1:
        # if isinstance(item,Function):
        #     objInfo = moose.element(item.parent).path+'/info'
        # else:
        #     objInfo = item.path+'/info'
        if not isinstance(item,Function):
            objInfo = item.path+'/info'
            xcord.append(xyPosition(objInfo,'x'))
            ycord.append(xyPosition(objInfo,'y'))

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

        # elif baseObj == 'Function':
        #     #ZombieSumFunc adding inputs
        #     inputlist = []
        #     outputlist = []
        #     funplist = []
        #     nfunplist = []
        #     for items in wildcardFind(path):
        #         for funplist in moose.element(items).neighbors['valueOut']:
        #             for func in funplist:
        #                 funcx = moose.element(items.path+'/x[0]')
        #                 uniqItem,countuniqItem = countitems(funcx,'input')
        #                 for inPut in uniqItem:
        #                     inputlist.append((inPut,'st',countuniqItem[inPut]))
        #             cntDict[func] = inputlist
        else:
            for tab in wildcardFind(path):
                tablist = []
                uniqItem,countuniqItem = countitems(tab,'output')
                for tabconnect in uniqItem:
                    tablist.append((element(tabconnect),'tab',countuniqItem[tabconnect]))
                cntDict[tab] = tablist

def countitems(mitems,objtype):
    items = []
    #print "mitems in countitems ",mitems,objtype
    items = element(mitems).neighbors[objtype]
    uniqItems = set(items)
    countuniqItems = Counter(items)
    return(uniqItems,countuniqItems)

def autoCoordinates(meshEntry,srcdesConnection):
    #for cmpt,memb in meshEntry.items():
    #    print memb
    xmin = 0.0
    xmax = 1.0
    ymin = 0.0
    ymax = 1.0
    G = nx.Graph()
    for cmpt,memb in meshEntry.items():
        for enzObj in find_index(memb,'enzyme'):
            G.add_node(enzObj.path)
    for cmpt,memb in meshEntry.items():
        for poolObj in find_index(memb,'pool'):
            G.add_node(poolObj.path)
        for cplxObj in find_index(memb,'cplx'):
            G.add_node(cplxObj.path)
            G.add_edge((cplxObj.parent).path,cplxObj.path)
        for reaObj in find_index(memb,'reaction'):
            G.add_node(reaObj.path)
        
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
    
    nx.draw(G,pos=nx.spring_layout(G))
    #plt.savefig('/home/harsha/Desktop/netwrokXtest.png')
    xcord = []
    ycord = []
    position = nx.spring_layout(G)
    for item in position.items():
        xy = item[1]
        ann = moose.Annotator(item[0]+'/info')
        ann.x = xy[0]
        xcord.append(xy[0])
        ann.y = xy[1]
        ycord.append(xy[1])
    # for y in position.values():
    #     xcord.append(y[0])
    #     ycord.append(y[1])
    if xcord and ycord:
        xmin = min(xcord)
        xmax = max(xcord)
        ymin = min(ycord)
        ymax = max(ycord)    	    
    return(xmin,xmax,ymin,ymax,position)

def find_index(value, key):
    """ Value.get(key) to avoid expection which would raise if empty value in dictionary for a given key """
    if value.get(key) != None:
        return value.get(key)
    else:
        raise ValueError('no dict with the key found')
