# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

# Some network analysis utilities.

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2018-19, NCBS Bangalore"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"

import sys
import hashlib
import moose._moose as _moose
import re

import logging
logger_ = logging.getLogger('moose.utils.graph')

pathPat_ = re.compile(r'.+?\[\d+\]$')

def morphologyToGraphviz(filename=None, pat='/##[TYPE=Compartment]'):
    '''Write Electrical network to a dot graph.

    Params:

    :filename: Default None. Write graphviz file to this path. If None, write to
        stdout.
    :pat: Compartment path. By default, search for all moose.Compartment.
    '''

    def fix(path):
        '''Fix a given path so it can be written to a graphviz file'''
        # If no [0] is at end of the path then append it.
        global pathPat_
        if not pathPat_.match(path):
            path = path + '[0]'
        return path

    compList = _moose.wildcardFind(pat)
    if not compList:
        logger_.warn("No compartment found")

    dot = []
    dot.append("digraph G {")
    dot.append("\tconcentrate=true;")
    for c in compList:
        if c.neighbors['raxial']:
            for n in c.neighbors['raxial']:
                lhs = fix(c.path)
                rhs = fix(n.path)
                dot.append('\t"{}" -> "{}";'.format(lhs, rhs))
        elif c.neighbors['axial']:
            for n in c.neighbors['axial']:
                lhs = fix(c.path)
                rhs = fix(n.path)
                dot.append('\t"{}" -> "{}" [dir=back];'.format(lhs, rhs))
        else:
            p = fix(c.path)
            dot.append('\t"{}"'.format(p))

    dot.append('}')
    dot = '\n'.join(dot)
    if not filename:
        print(dot)
    else:
        with open(filename, 'w') as graphviz:
            logger_.info("Writing compartment topology to file {}".format(filename))
            graphviz.write(dot)
    return True


def chemicalReactionNetworkToGraphviz(compt, path=None):
    """Write chemical reaction network to a graphviz file.

    :param compt: Given compartment.
    :param filepath: Save to this filepath. If None, write to stdout.
    """
    dot = _crn(compt)
    if path is None:
        print(dot, file=sys.stdout)
        return
    with open(path, 'w') as f:
        f.write(dot)

# aliases
crnToDot = chemicalReactionNetworkToGraphviz
crnToGraphviz = chemicalReactionNetworkToGraphviz

def _fixLabel(name):
    name = name.replace('*', 'star')
    name = name.replace('.', '_')
    return "\"{}\"".format(name)

def _addNode(n, nodes, dot):
    node = hashlib.sha224(n.path.encode()).hexdigest()
    nodeType = 'pool'
    if isinstance(n, _moose.Reac) or isinstance(n, _moose.ZombieReac):
        node = 'r'+node
        nodeType = 'reac'
    else:
        node = 'p'+node
    if node in nodes:
        return node

    nLabel = n.name
    if nodeType == 'reac':
        nLabel = "kf=%g kb=%g"%(n.Kf, n.Kb)
        dot.append('\t%s [label="%s", kf=%g, kb=%g, shape=rect]' % (
            node, nLabel, n.Kf, n.Kb))
    else:
        dot.append('\t%s [label="%s", concInit=%g]' % (
            node, nLabel, n.concInit))
    return node

def _crn(compt):
    nodes = {}
    reacs = _moose.wildcardFind(compt.path+'/##[TYPE=Reac]')
    reacs += _moose.wildcardFind(compt.path+'/##[TYPE=ZombieReac]')
    dot = ['digraph %s {\n\t overlap=false' % compt.name ]
    for r in reacs:
        rNode = _addNode(r, nodes, dot)
        for s in r.neighbors['sub']:
            sNode = _addNode(s, nodes, dot)
            dot.append('\t%s -> %s' % (sNode, rNode))
        for p in r.neighbors['prd']:
            pNode = _addNode(p, nodes, dot)
            dot.append('\t%s -> %s' % (rNode, pNode))
    dot.append('}')
    return '\n'.join(dot)
