# -*- coding: utf-8 -*-
from __future__ import print_function

"""test_streamer.py:
Test script for Streamer class.
"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2016, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import moose
import threading
import numpy as np
import time
import os
import sys
print('[INFO] Using moose form %s' % moose.__file__)

all_done_ = False

# Poll the file to see that we are really writing to it.
def sanity_test( ):
    a = moose.Table( '/t1' )
    b = moose.Table( '/t1/t1' )
    c = moose.Table( '/t1/t1/t1' )
    print(a)
    print(b)
    print(c)

    st = moose.Streamer( '/s' )

    st.outfile = 'a.txt'
    assert st.outfile == 'a.txt'

    st.addTable( a )
    assert( st.numTables == 1 )
    st.addTable( b )
    assert( st.numTables == 2 )
    st.addTable( c )
    assert( st.numTables == 3 )
    st.addTable( c )
    assert( st.numTables == 3 )
    st.addTable( c )
    assert( st.numTables == 3 )

    st.removeTable( c )
    assert( st.numTables == 2 )
    st.removeTable( c )
    assert( st.numTables == 2 )
    st.removeTable( a )
    assert( st.numTables == 1 )
    st.removeTable( b )
    assert( st.numTables == 0 )
    st.removeTable( b )
    assert( st.numTables == 0 )
    print( 'Sanity test passed' )

    st.addTables( [a, b, c ])
    assert st.numTables == 3
    st.removeTables( [a, a, c] )
    assert st.numTables == 1

def test( ):
    compt = moose.CubeMesh( '/compt' )
    assert compt
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

    outfile = 'streamer_test.csv'
    if os.path.exists(outfile):
        os.remove(outfile)

    tabA = moose.Table2( '/compt/a/tab' )
    tabB = moose.Table2( '/compt/tabB' )
    tabC = moose.Table2( '/compt/tabB/tabC' )
    print(tabA, tabB, tabC)

    moose.connect( tabA, 'requestOut', a, 'getConc' )
    moose.connect( tabB, 'requestOut', b, 'getConc' )
    moose.connect( tabC, 'requestOut', c, 'getConc' )

    # Now create a streamer and use it to write to a stream
    st = moose.Streamer( '/compt/streamer' )
    st.outfile = outfile

    print("outfile set to: %s " % st.outfile )
    st.addTable( tabA )
    st.addTables( [ tabB, tabC ] )
    assert st.numTables == 3

    moose.reinit( )
    t = 100
    print( '[INFO] Running for %d seconds' % t )
    moose.start(t)
    outfile = st.outfile
    moose.quit() # Otherwise Streamer won't flush the rest of entries.

    print('Moose is done. Waiting for monitor to shut down...')

    # Now read the table and verify that we have written
    print( '[INFO] Reading file %s' % outfile )
    if 'csv' in outfile:
        data = np.loadtxt(outfile, skiprows=1 )
    else:
        data = np.load( outfile )
    # Total rows should be 58 (counting zero as well).
    #  print(data)
    # print( data.dtype )
    assert data.shape >= (101,), data.shape
    print( '[INFO] Test 2 passed' )
    return 0

def main( ):
    sanity_test( )
    test( )
    print( '[INFO] All tests passed' )


if __name__ == '__main__':
    main()
