# -*- coding: utf-8 -*-
"""test_table_streaming_support.py:

Test the streaming support in moose.Table.

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2016, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import os
import sys
import moose
import numpy as np
print( '[INFO] Using moose form %s' % moose.__file__ )

def print_table( table ):
    msg = ""
    msg += " outfile : %s" % table.outfile
    msg += " useStreamer: %s" % table.useStreamer
    msg += ' Path: %s' % table.path
    print( msg )

def test( ):
    compt = moose.CubeMesh( '/compt' )
    r = moose.Reac( '/compt/r' )
    a = moose.Pool( '/compt/a' )
    a.concInit = 1
    b = moose.Pool( '/compt/b' )
    b.concInit = 2
    c = moose.Pool( '/compt/c' )
    c.concInit = 0.5
    moose.connect( r, 'sub', a, 'reac' )
    moose.connect( r, 'prd', b, 'reac' )
    moose.connect( r, 'prd', c, 'reac' )
    r.Kf = 0.1
    r.Kb = 0.01

    tabA = moose.Table2( '/compt/a/tabA' )
    tabA.format = 'npy'
    tabA.useStreamer = 1   # Setting format alone is not good enough

    tabB = moose.Table2( '/compt/b/tabB' )
    tabB.outfile = 'table2.npy'

    tabC = moose.Table2( '/compt/c/tabC' )
    tabC.outfile = 'tablec.csv'

    moose.connect( tabA, 'requestOut', a, 'getConc' )
    moose.connect( tabB, 'requestOut', b, 'getConc' )
    moose.connect( tabC, 'requestOut', c, 'getConc' )

    moose.reinit( )
    [ print_table( x) for x in [tabA, tabB, tabC] ]
    runtime = 1000
    print( 'Starting moose for %d secs' % runtime )
    moose.start( runtime, 1 )
    print( ' MOOSE is done' )

    # Now read the numpy and csv and check the results.
    a = np.loadtxt( '_tables/compt/a/tabA.csv', skiprows=1 )
    b = np.load( 'table2.npy' )
    c = np.loadtxt( 'tablec.csv', skiprows=1 )
    assert (len(a) == len(b) == len(c))

def main( ):
    test( )
    print( '[INFO] All tests passed' )


if __name__ == '__main__':
    main()
