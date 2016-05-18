#!/usr/bin/env python

"""moose_methods.py:  Some helper function related with moose to do multiscale
modelling.

Last modified: Thu Jun 05, 2014  01:20AM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import re
import os
import moose
import debug.debug as debug

nameSep = '_'

def toFloat(string):
    if type(string) == float:
        return string 
    elif type(string) == str:
        return stringToFloat(string)
    else:
        raise RuntimeError("Converting type %s to float" % type(str))

def commonPath(pathA, pathB):
    ''' Find common path at the beginning of two paths. '''
    a = pathA.split('/')
    b = pathB.split('/')
    common = []
    for (i, p) in enumerate(a):
        if a[i] == b[i]: 
            common.append(p)
        else: 
            return '/'.join(common)
    return '/'.join(common)

def moosePath(baseName, append):
    """ 
    Append instance index to basename

    TODO: Ideally - should be replace with [ and ]

    """
    if append.isdigit():
        if len(nameSep) == 1:
            return baseName + nameSep + append 
        elif len(nameSep) == 2:
            return baseName + nameSep[0] + append + nameSep[1]
        else:
            raise UserWarning, "Not more than 2 characters are not supported"
    else:
        return "{}/{}".format(baseName, append)


def splitComparmentExpr(expr):
    """ Breaks compartment expression into name and id.
    """
    if len(nameSep) == 1:
        p = re.compile(r'(?P<name>[\w\/\d]+)\{0}(?P<id>\d+)'.format(nameSep[0]))
    else:
        # We have already verified that nameSep is no longer than 2 characters.
        a, b = nameSep 
        p = re.compile(r'(?P<name>[\w\/\d]+)\{0}(?P<id>\d+)\{1}'.format(a, b))
    m = p.match(expr)
    assert m.group('id').isdigit() == True
    return m.group('name'), m.group('id')



def getCompartmentId(compExpr):
    """Get the id of compartment.
    """
    return splitComparmentExpr(compExpr)[1]

def getCompName(compExpr):
    """Get the name of compartment.
    """
    return splitComparmentExpr(compExpr)[0]

def stringToFloat(text):
    text = text.strip()
    if not text:
        return 0.0
    try:
        val = float(text)
        return val
    except Exception:
        raise UserWarning, "Failed to convert {0} to float".format(text)


def dumpMoosePaths(pat, isRoot=True):
    ''' Path is pattern '''
    moose_paths = getMoosePaths(pat, isRoot)
    return "\n\t{0}".format(moose_paths)

def getMoosePaths(pat, isRoot=True):
    ''' Return a list of paths for a given pattern. '''
    if type(pat) != str:
        pat = pat.path
        assert type(pat) == str
    moose_paths = [x.path for x in moose.wildcardFind(pat)]
    return moose_paths

def dumpMatchingPaths(path, pat='/##'):
    ''' return the name of path which the closely matched with given path 
    pattern pat is optional.
    '''
    a = path.split('/')
    start = a.pop(0)
    p = moose.wildcardFind(start+'/##')
    common = []
    while len(p) > 0:
        common.append(p)
        start = start+'/'+a.pop(0)
        p = moose.wildcardFind(start+'/##')
        
    if len(common) > 1:
        matchedPaths = [x.getPath() for x in common[-1]]
    else:
        matchedPaths = []
    return '\n\t'+('\n\t'.join(matchedPaths))


def dumpFieldName(path, whichInfo='valueF'):
    print(path.getFieldNames(whichInfo+'info'))

def writeGraphviz(pat='/##', filename=None, filterList=[]):
    '''This is  a generic function. It takes the the pattern, search for paths
    and write a graphviz file.
    '''
    def ignore(line):
        for f in filterList:
            if f in line:
                return True
        return False

    pathList = getMoosePaths(pat)
    dot = []
    dot.append("digraph G {")
    dot.append("\tconcentrate=true")
    for p in pathList:
        if ignore(p):
            continue
        else:
            p = re.sub('[]()]', '', p)
            dot.append('\t'+' -> '.join(filter(None, p.split('/'))))
    dot.append('}')
    dot = '\n'.join(dot)
    if not filename:
        print(dot)
    else:
        with open(filename, 'w') as graphviz:
            debug.printDebug("INFO"
                    , "Writing topology to file {}".format(filename)
                    )
            graphviz.write(dot)
    return 

def setupTable(name, obj, qtyname, tablePath=None, threshold=None):
    '''This is replacement function for moose.utils.setupTable

    It stores qtyname from obj.
    '''
    assert qtyname[0].isupper(), "First character must be uppercase character"
    debug.printDebug("DEBUG"
            , "Setting up table for: {} -> get{}".format(obj.path, qtyname)
            )
    if tablePath is None:
        tablePath = '{}/{}'.format(obj.path, 'data')
        debug.printDebug("WARN"
                , "Using default table path: {}".format(tablePath)
                , frame = inspect.currentframe()
                )
    if not moose.exists(obj.path):
        raise RuntimeError("Unknown path {}".format(obj.path))

    moose.Neutral(tablePath)
    table = moose.Table('{}/{}'.format(tablePath, name))
    if threshold is None:
        moose.connect(table, "requestOut", obj, "get{}".format(qtyname))
    else:
        raise UserWarning("TODO: Table with threshold is not implemented yet")
    return table
