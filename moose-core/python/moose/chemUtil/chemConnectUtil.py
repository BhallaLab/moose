import moose
import numpy as np
from collections import Counter

def xyPosition(objInfo,xory):
    try:
        return(float(moose.element(objInfo).getField(xory)))
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

def sizeHint(self):
    return QtCore.QSize(800,400)

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
                    print ("Substrate Product is empty ",path, " ",items)
                    
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
    countuniqItems = Counter(items)
    return(uniqItems,countuniqItems)