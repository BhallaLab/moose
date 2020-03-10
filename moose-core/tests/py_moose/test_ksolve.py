# -*- coding: utf-8 -*-
import sys
import numpy as np
import time
import moose

print('Using moose from %s' % moose.__file__ )

def test_ksolve():
    compt = moose.CubeMesh( '/compt' )
    compt.volume = 1e-20

    pools = []
    for r in range( 10 ):
        a1 = moose.Pool( '/compt/a1%s' % r )
        a1.concInit = 10
        a2 = moose.Pool( '/compt/a2%s' % r )
        a2.concInit = 5
        b1 = moose.Pool( '/compt/b1%s' % r )
        b1.concInit = 0.054
        b2 = moose.Pool( '/compt/b2%s' % r )
        b2.concInit = 3.9
        r = moose.Reac( '/compt/reac%s'% r )
        moose.connect( r, 'sub', a1, 'reac' )
        moose.connect( r, 'sub', a2, 'reac' )
        moose.connect( r, 'prd', b1, 'reac' )
        moose.connect( r, 'prd', b2, 'reac' )
        r.Kf = 2.9
        r.Kb = 4.5
        pools += [ a1, a2, b1, b2 ]

    ksolve = moose.Ksolve( '/compt/ksolve' )
    stoich = moose.Stoich( '/compt/stoich' )
    stoich.compartment = compt
    stoich.ksolve = ksolve
    ksolve.numThreads = 2
    stoich.path = '/compt/##'
    moose.reinit()
    print( '[INFO] Using method = %s' % ksolve.method )
    t1 = time.time()
    moose.start( 100 )
    print('[INFO] Time taken %s' % (time.time() - t1 ))
    expected = [ 7.77859 , 2.77858 , 2.27541 , 6.12141 , 7.77858 , 2.77858
            , 2.27541 , 6.12141 , 7.77858 , 2.77858 , 2.27541 , 6.12141 , 7.77858
            , 2.77858 , 2.27541 , 6.12141 , 7.77858 , 2.77858 , 2.27541 , 6.12141
            , 7.77858 , 2.77858 , 2.27541 , 6.12141 , 7.77858 , 2.77858 , 2.27541
            , 6.12141 , 7.77858 , 2.77858 , 2.27541 , 6.12141 , 7.77858 , 2.77858
            , 2.27541 , 6.12141 , 7.77858 , 2.77858 , 2.27541 , 6.12141
            ]
    concs = [ p.conc for p in pools ]
    if(not np.isclose( concs, expected ).all() ):
        print( " Expected %s" % expected )
        print( " Got %s" % concs )
        quit(1)
    print( 'Test passed' )

if __name__ == '__main__':
    test_ksolve()
