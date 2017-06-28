"""diffusion_using_Gillespie.py: 

In this snippet, we show how to setup diffusion using Gillespie scheme.

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2016, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import math
import moose
import moose.utils as mu

tables_ = { }
pools_ = { }
D_ = 1e-12
A0_ = 1.0

compt_ = None

def diff_analytical( x = 1e-6, maxT = 10 ):
    global D_
    tvec = np.arange( 0.001, maxT, 0.1 )
    return [ A0_ / math.sqrt(4 * math.pi * D_ * t ) 
            * math.exp( - (x ** 2.0) / (4 * D_ * t ) ) for t in tvec ]


def create_model( compt, sec ):
    global pools_
    for s in [ 'A' ]:
        x = moose.Pool( '%s/%s.%s' % (compt.path, sec, s) )
        pools_[ x.name ] = x
        t = moose.Table2( '%s/tab%s.%s' % (compt.path, sec, s ))
        moose.connect( t, 'requestOut', x, 'getConc' )
        tables_[ x.name ] = t 

def enable_diffusion( s1, s2, pool, kfkb ):
    global compt_
    global pools_
    p1 = pools_[ '%s.%s' % ( s1, pool ) ]
    p2 = pools_[ '%s.%s' % ( s2, pool ) ]
    r = moose.Reac( '%s/%s_%s' % (compt_.path, s1, s2 ) )
    moose.connect( r, 'sub', p1, 'reac' )
    moose.connect( r, 'prd', p2, 'reac' )
    r.Kf = r.Kb = kfkb

def make_plot( tables ):
    clk = moose.Clock( '/clock' )
    t = clk.currentTime
    tvec = np.linspace( 0, t, len( tables.values()[0].vector ) )
    compts = sorted( tables.keys( ) )
    data = [ tables[ c ].vector for c in compts ]
    data = np.vstack( data )
    plt.plot( tvec, data[0,:] )
    plt.show( )

def setup_solvers( ):
    global compt_
    s = moose.Stoich( '%s/stoich' % compt_.path )
    k = moose.Ksolve( '%s/ksolve' % compt_.path )
    s.ksolve = k
    s.compartment = compt_
    s.path = '%s/##' % compt_.path 


def main( ):
    global compt_
    compt_ = moose.CylMesh( '/compt' )
    compt_.r0 = compt_.r1 = 1e-6
    compt_.x1 = 100e-6
    print( '[INFO] Compartment volume is %g' % compt_.volume )

    # Create n subcompartment in this compartment.
    subsecs = [ 'sec%d' % x for x in range( 10 ) ]
    for subsec in subsecs :
        create_model( compt_, subsec )

    # Put 1000 molecules of A in first subsection.
    pools_[ 'sec0.A' ].concInit = A0_

    # Putt diffusion between subsections.
    for i, s2 in enumerate( subsecs[1:] ):
        s1 = subsecs[i]
        kfkb = D_ / ( compt_.x1 / len( subsecs ) ) ** 2.0    # D/L^2
        enable_diffusion( s1, s2, 'A', kfkb )

    setup_solvers( )
    moose.reinit( )
    moose.start( 100, 1 )
    # make_plot( tables_ )
    concA = diff_analytical( )
    print( concA )

if __name__ == '__main__':
    main()
