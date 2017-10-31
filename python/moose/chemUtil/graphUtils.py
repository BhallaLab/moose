# -*- coding: utf-8 -*-
'''
*******************************************************************
 * File:            chemConnectUtil.py
 * Description:
 * Author:          HarshaRani
 * E-mail:          hrani@ncbs.res.in
 ********************************************************************/
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2017 Upinder S. Bhalla. and NCBS
Created : Friday May 27 12:19:00 2016(+0530)
Version
Last-Updated: Mon Aug 7 17:02:00 2017(+0530)
          By: HarshaRani
**********************************************************************/
/****************************

Aug 7: cleaned up space
'''

import moose

pygraphvizFound_ = True
try:
    import pygraphviz as pgv
except Exception as e:
    pygraphvizFound_ = False

def autoCoordinates(meshEntry,srcdesConnection):
    global pygraphvizFound_
    positionInfo = {}

    if not pygraphvizFound_:
        print( '[warn] python-pygraphviz could not be found.' )
        print( '\tMOOSE Install pygraphviz to use this feature' )
        return positionInfo

    if meshEntry:
        #G = nx.Graph()
        G = pgv.AGraph()

        for cmpt,memb in list(meshEntry.items()):
            for enzObj in find_index(memb,'enzyme'):
                #G.add_node(enzObj.path)
                G.add_node(enzObj.path,label='',shape='ellipse',color='',style='filled',fontname='Helvetica',fontsize=12,fontcolor='blue')
        for cmpt,memb in list(meshEntry.items()):
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

        for inn,out in list(srcdesConnection.items()):
            if (inn.className =='ZombieReac'): arrowcolor = 'green'
            elif(inn.className =='ZombieEnz'): arrowcolor = 'red'
            else: arrowcolor = 'blue'
            if isinstance(out,tuple):
                if len(out[0])== 0:
                    print( inn.className + ':' +inn.name + "  doesn't have input message")
                else:
                    for items in (items for items in out[0] ):
                        G.add_edge(moose.element(items[0]).path,inn.path)
                if len(out[1]) == 0:
                    print(inn.className + ':' + inn.name + " doesn't have output mssg")
                else:
                    for items in (items for items in out[1] ):
                        G.add_edge(inn.path,moose.element(items[0]).path)
            elif isinstance(out,list):
                if len(out) == 0:
                    print ("Func pool doesn't have sumtotal")
                else:
                    for items in (items for items in out ):
                        G.add_edge(moose.element(items[0]).path,inn.path)

        # if int( nx.__version__.split( '.' )[-1] ) >= 11:
        #     position = nx.spring_layout( G )
        # else:
        #     position = nx.graphviz_layout(G, prog = 'dot')
        # for item in position.items():
        #     xy = item[1]
        #     ann = moose.Annotator(item[0]+'/info')
        #     ann.x = xy[0]
        #     ann.y = xy[1]
        #     positioninfo[(moose.element(item[0]))] ={'x':float(xy[0]),'y':float(xy[1])}

        G.layout()
        for n in G.nodes():
            value = str(n.attr['pos'])
            valuelist = (value.split(','))
            positionInfo[(moose.element(n))] ={'x':float(valuelist[0]),'y':float(valuelist[1])}
            ann = moose.Annotator(moose.element(n).path+'/info')
            ann.x = float(valuelist[0])
            ann.y = float(valuelist[1])

    return positionInfo

def find_index(value, key):
    """ Value.get(key) to avoid expection which would raise if empty value in dictionary for a given key """
    if value.get(key) != None:
        return value.get(key)
    else:
        raise ValueError('no dict with the key found')
