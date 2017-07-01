# -*- coding: utf-8 -*-
# graphutil.py ---
#
# Filename: graphutil.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Sun Mar 18 13:42:28 2012 (+0530)
# Version:
# Last-Updated: Wed Mar 28 18:27:26 2012 (+0530)
#           By: subha
#     Update #: 103
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
#
#
#

# Change log:
#
#

# Code:

from collections import defaultdict
import re
import networkx as nx
import numpy as np
from . import moose

def moosegraph(element, ies=['childMsg'], ied=['parentMsg'], iv=[], keep_solitary=False):
    """Create a graph out of all objects under teh element
    specified. Ignore edges going outside the subtree rooted at
    element.

    If element is a string, it can be a wildcard path in MOOSE

    ies is a list of sourcefields to be ignored when constructing edges.

    ied is a list of destination fields to be ignored when
    constructing edges.

    iv is a list of paths, possibly wildcards, to be ignored when
    building vertex set.

    keep_solitary -- if True solitary nodes are not discarded

    Note that this is a temporary solution. We rely on neighbours
    field to create the graph. Ideally we should use the messages
    between objects. A bug in Messages does not allow us to access the
    fields on both sides properly in the general case. We shall switch
    to messgaes once that bug is fixed."""

    path = None
    if isinstance(element, str):
        path = element
    elif isinstance(element, moose.Id):
        path = element.getPath()
    elif isinstance(element, moose.ObjId):
        path = element.getField('path')
    elif isinstance(element, moose.Neutral):
        path = element.path
    else:
        raise TypeError('Require Id, ObjId, Neutral or string object for element')
    iv_re = [re.compile(pattern) for pattern in iv]
    ies_re = [re.compile(pattern) for pattern in ies]
    ied_re = [re.compile(pattern) for pattern in ied]
    if '#' in path:
        all_v = moose.wildcardFind(path)
    else:
        if path.endswith('/'):
            path = '%s##' % (path)
        else:
            path = '%s/##' % (path)
        all_v = moose.wildcardFind(path)
    valid_v = []
    # Collect all not-to-be-ignored vertices
    for vv in all_v:
        include = True
        for iv in iv_re:
            if iv.search(vv.path):
                include = False
                break
        if include:
            valid_v.append(vv)
    graph = nx.DiGraph()
    graph.add_nodes_from([vv.id_ for vv in valid_v])
    for vv in graph.nodes():
        for fname in vv[0].getFieldNames('srcFinfo'):
            matches = [True for regex in ies_re if regex.search(fname)]
            if matches:
                continue
            nlist = vv[0].getNeighbors(fname)
            for nn in nlist:
                if nn in graph.nodes():
                    try:
                        src = graph.edge[vv][nn]['src']
                        src.add(fname)
                    except KeyError:
                        graph.add_edge(vv, nn, src=set([fname]))

        for fname in vv[0].getFieldNames('destFinfo'):
            matches = [True for regex in ied_re if regex.search(fname)]
            if matches:
                continue
            nlist = vv[0].getNeighbors(fname)
            for nn in nlist:
                if nn in graph.nodes():
                    try:
                        dest = graph.edge[vv][nn]['dest']
                        dest.add(fname)
                    except KeyError:
                        graph.add_edge(nn, vv, dest=set([fname]))

        for fname in vv[0].getFieldNames('sharedFinfo'):
            nlist = vv[0].getNeighbors(fname)
            for nn in nlist:
                if nn in graph.nodes():
                    try:
                        src = graph.edge[vv][nn]['src']
                        src.add(fname)
                    except KeyError:
                        graph.add_edge(vv, nn, src=set([fname]))
                    try:
                        dest = graph.edge[nn][vv]['dest']
                        dest.add(fname)
                    except KeyError:
                        graph.add_edge(nn, vv, dest=set([fname]))
    if not keep_solitary:
        solitary=[ n for n,d in graph.degree_iter() if d==0 ]
        graph.remove_nodes_from(solitary)
    return graph

def draw_moosegraph(graph, pos=None, label_edges=True):
    if pos is None:
        pos = nx.spring_layout(graph)
    edge_labels = {}
    if label_edges:
        for ee in graph.edges():
            try:
                src = ','.join(graph.edge[ee[0]][ee[1]]['src'])
            except KeyError:
                src = '?'
            try:
                dest = ','.join(graph.edge[ee[0]][ee[1]]['dest'])
            except KeyError:
                dest = '?'
            label = '%s-%s' % (src, dest)
            edge_labels[ee] = label
    nx.draw(graph, pos)
    nx.draw_networkx_edge_labels(graph, pos, edge_labels)

import unittest
import pylab

class TestMooseGraph(unittest.TestCase):
    def setUp(self):
        cell = moose.Neutral('/n')
        comp1 = moose.Compartment('/n/c1')
        chan = moose.HHChannel('/n/c1/ch')
        moose.connect(comp1, 'channel', chan, 'channel')
        comp2 = moose.Compartment('/n/c2')
        comp2.connect('raxial', comp1, 'axial')
        self.graph = moosegraph('/n')

    def test_edgelabels(self):
        for ee in self.graph.edges():
            keys = self.graph.edge[ee[0]][ee[1]].keys()
            self.assertTrue('src' in keys)
            self.assertTrue('dest' in keys)

    def test_vertexid(self):
        for vv in self.graph.nodes():
            self.assertTrue(isinstance(vv, moose.Id))

    def test_plot(self):
        draw_moosegraph(self.graph)
        pylab.show()


if '__main__' == __name__:
    unittest.main()



#
# graphutil.py ends here
