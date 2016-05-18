"""graph_utils.py: Graph related utilties. It does not require networkx library.
It writes files to be used with graphviz.

Last modified: Sat Jan 18, 2014  05:01PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
from . import _moose
import inspect
from . import print_utils as debug
import re

pathPat = re.compile(r'.+?\[\d+\]$')

def getMoosePaths(pat, isRoot=True):
    ''' Return a list of paths for a given pattern. '''
    if type(pat) != str:
        pat = pat.path
        assert type(pat) == str
    moose_paths = [x.path for x in _moose.wildcardFind(pat)]
    return moose_paths

def writeGraphviz(filename=None, pat='/##[TYPE=Compartment]'):
    '''This is  a generic function. It takes the the pattern, search for paths
    and write a graphviz file.
    '''

    def fix(path):
        '''Fix a given path so it can be written to a graphviz file'''
        # If no [0] is at end of the path then append it.
        global pathPat
        if not pathPat.match(path):
            path = path + '[0]'
        return path

        
    pathList = getMoosePaths(pat)
    compList = _moose.wildcardFind(pat)
    if not compList:
        debug.dump("WARN"
                , "No compartment found"
                , frame = inspect.currentframe()
                )

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
            debug.dump("INFO"
                    , "Writing compartment topology to file {}".format(filename)
                    )
            graphviz.write(dot)
    return True

