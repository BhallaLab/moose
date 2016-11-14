# display_morphology.py --- 
# 
# Filename: display_morphology.py
# Description: 
# Author: 
# Maintainer: 
# Created: Fri Mar  8 11:26:13 2013 (+0530)
# Version: 
# Last-Updated: Thu Aug 11 11:29:36 2016 (-0400)
#           By: Subhasis Ray
#     Update #: 389
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Draw the schematic diagram of cells using networkx
# 
# 

# Change log:
# 
# 
# 
# 

# Code:
from __future__ import print_function

"""
Display/save the topology of one or all cells in traub_2005 demo.

command line options (all are optional):
-c celltype    : display topology of cell type 'celltype'. If unspecified, all cell types are displayed
-p filename    : save output to fiel specified by 'filename'
-l             : show labels of the compartments
-h,--help      : show this help
"""
import sys
sys.path.append('../../../python')
import os
import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
import moose
import cells

def node_sizes(g):
    """Calculate the 2D projection area of each compartment.
    
    g: graph whose nodes are moose Compartment objects.
    
    return a numpy array with compartment areas in 2D projection
    normalized by the maximum.

    """
    sizes = []
    comps = [moose.Compartment(n) for n in g.nodes()]
    sizes = np.array([c.length * c.diameter for c in comps])
    soma_i = [ii for ii in range(len(comps)) if comps[ii].path.endswith('comp_1')]
    sizes[soma_i] *= np.pi/4 # for soma, length=diameter. So area is dimater^2 * pi / 4
    return sizes / max(sizes)
    
def cell_to_graph(cell, label=False):
    """Convert a MOOSE compartmental neuron into a graph describing
    the topology of the compartments

    """
    soma = moose.element('%s/comp_1' % (cell.path))
    if len(soma.neighbors['axialOut']) > 0:
        msg = 'raxialOut'
    elif len(soma.neighbors['distalOut']) > 0:
        msg = 'distalOut'
    else:
        raise Exception('No neighbors on raxial or distal')
    es = [(c1.path, c2[0].path, {'weight': 2/ (c1.Ra + c2[0].Ra)}) \
              for c1 in moose.wildcardFind('%s/##[ISA=CompartmentBase]' % (cell.path)) \
              for c2 in moose.Compartment(c1).neighbors[msg]]
    g = nx.Graph()
    g.add_edges_from(es)
    if label:
        for v in g.nodes():
            g.node[v]['label'] = v.rpartition('/')[-1]
    return g

def axon_dendrites(g):
    """Get a 2-tuple with list of nodes representing axon and list of
    nodes representing dendrites.

    g: graph whose nodes are compartments 

    """
    axon = []
    soma_dendrites = []
    for n in g.nodes():
        if moose.exists('%s/CaPool' % (n)):
            soma_dendrites.append(n)
        else:
            axon.append(n)
    return (axon, soma_dendrites)

def plot_cell_topology(cell, label=False):
    g = cell_to_graph(cell, label=label)
    axon, sd = axon_dendrites(g)
    node_size = node_sizes(g)
    weights = np.array([g.edge[e[0]][e[1]]['weight'] for e in g.edges()])
    try:
        pos = nx.graphviz_layout(g,prog='twopi',root=cell.path + '/comp_1')
    except (NameError, AttributeError) as e:
        # this is the best networkx can do by itself. Its Furchtman
        # Reingold layout ends up with overlapping edges even for a
        # tree. igraph does much better.
        pos = nx.spectral_layout(g)
    nx.draw_networkx_edges(g, pos, width=10*weights/max(weights), edge_color='gray', alpha=0.8)
    nx.draw_networkx_nodes(g, pos, with_labels=False,
                           nnode_size=node_size * 500, 
                           node_color=map(lambda x: 'k' if x in axon else 'gray', g.nodes()), 
                           linewidths=[1 if n.endswith('comp_1') else 0 for n in g.nodes()], 
                           alpha=0.8)
    if label:
        labels = dict([(n, g.node[n]['label']) for n in g.nodes()])
        nx.draw_networkx_labels(g, pos, labels=labels)
    plt.title(cell.__class__.__name__)

from matplotlib.backends.backend_pdf import PdfPages

import sys
from getopt import getopt

if __name__ == '__main__':
    optlist, args = getopt(sys.argv[1:], 'lhp:c:', ['help'])
    celltype = ''
    pdf = ''
    label = False
    for arg in optlist:
        if arg[0] == '-c':
            celltype = arg[1]
        elif arg[0] == '-p':
            pdf = arg[1]
        elif arg[0] == '-l':
            label = True
        elif arg[0] == '-h' or arg[0] == '--help':
            print('Usage: %s [-c CellType [-p filename]]' % (sys.argv[0]))
            print('Display/save the morphology of cell type "CellType".')
            print('Options:')
            print('-c celltype (optional) display only an instance of the specified cell type. If CellType is empty or not specified, all prototype cells are displayed.')
            print('-l label the compartments')
            print('-p  filename (optional) save outputin a pdf file named "filename".')
            print('-h,--help print this help')
            sys.exit(0)
    figures = []
    if len(celltype) > 0:
        try:
            fig = plt.figure()
            figures.append(fig)
            cell = cells.init_prototypes()[celltype]
            # print 'Label', label
            plot_cell_topology(cell, label=label)
        except KeyError:
            print( '%s: no such cell type. Available are:' % (celltype))
            for ii in cells.init_prototypes().keys():
                print( ii)
            sys.exit(1)    
    else:
        for cell, proto in cells.init_prototypes().items():
            figures.append(plt.figure())
            plot_cell_topology(proto, label=label)
    plt.axis('off')
    if len(pdf) > 0:
        pdfout = PdfPages(pdf)
        for fig in figures:
            pdfout.savefig(fig)
        pdfout.close()
    else:
        plt.show()

# 
# display_morphology.py ends here
