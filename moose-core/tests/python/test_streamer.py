# -*- coding: utf-8 -*-
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

import os
import sys
import time
import moose
import numpy as np
print(( '[INFO] Using moose form %s' % moose.__file__ ))

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

    tabA = moose.Table2( '/compt/a/tab' )
    tabB = moose.Table2( '/compt/tabB' )
    tabC = moose.Table2( '/compt/tabB/tabC' )
    print(tabA, tabB, tabC)

    moose.connect( tabA, 'requestOut', a, 'getConc' )
    moose.connect( tabB, 'requestOut', b, 'getConc' )
    moose.connect( tabC, 'requestOut', c, 'getConc' )

    # Now create a streamer and use it to write to a stream
    st = moose.Streamer( '/compt/streamer' )
    st.outfile = os.path.join( os.getcwd(), 'temp.npy' )
    print(("outfile set to: %s " % st.outfile ))
    assert st.outfile  == os.path.join( os.getcwd(), 'temp.npy' ), st.outfile

    st.addTable( tabA )
    st.addTables( [ tabB, tabC ] )

    assert st.numTables == 3

    moose.reinit( )
    print( '[INFO] Running for 57 seconds' )
    moose.start( 57 )
    outfile = st.outfile
    moose.quit() # Otherwise Streamer won't flush the rest of entries.

    # Now read the table and verify that we have written
    print( '[INFO] Reading file %s' % outfile )
    if 'csv' in outfile:
        data = np.loadtxt(outfile, skiprows=1 )
    else:
        data = np.load( outfile )
    # Total rows should be 58 (counting zero as well).
    # print(data)
    # print( data.dtype )
    time = data['time']
    print( time )
    assert data.shape >= (58,), data.shape
    print( '[INFO] Test 2 passed' )
    return 0

def main( ):
    sanity_test( )
    test( )
    print( '[INFO] All tests passed' )


if __name__ == '__main__':
    main()
