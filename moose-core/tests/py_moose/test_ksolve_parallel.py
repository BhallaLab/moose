import os
import numpy as np
import moose
print( 'Using moose from %s' % moose.__file__ )
import time
os.environ['MOOSE_SHOW_PROFILING_INFO'] = '1'

def test_ksolver_parallel( nthreads = 4 ):
    """
    This example implements a reaction-diffusion like system which is
    bistable and propagates losslessly. It is based on the NEURON example 
    rxdrun.py, but incorporates more compartments and runs for a longer time.
    The system is implemented as a hybrid of a reaction and a function which
    sets its rates. Please see rxdFuncDiffusion.py for a variant that uses
    just a function object to set up the system.
    """

    dt = 0.1

    # define the geometry
    compt = moose.CylMesh( '/cylinder' )
    compt.r0 = compt.r1 = 1
    #  compt.diffLength = 1e-6
    compt.x1 = 1e-2
    assert compt.numDiffCompts == int(compt.x1/compt.diffLength), ( 
            compt.numDiffCompts, compt.x1 / compt.diffLength )
    print( 'No of compartment %d' % compt.numDiffCompts )

    #define the molecule. Its geometry is defined by its parent volume, cylinder
    c = moose.Pool( '/cylinder/pool' )
    c.diffConst = 1 # define diffusion constant
    # There is an implicit reaction substrate/product. MOOSE makes it explicit.
    buf = moose.BufPool( '/cylinder/buf' )
    buf.nInit = 1

    # The reaction is something entirely peculiar, not a chemical thing.
    reaction = moose.Reac( '/cylinder/reac' )
    reaction.Kb = 0

    # so here we set up a function calculation to do the same thing.
    func = moose.Function( '/cylinder/reac/func' )
    func.expr = "(1 - x0) * (0.3 - x0)"
    func.x.num = 1 #specify number of input variables.

    #Connect the reaction to the pools
    moose.connect( reaction, 'sub', c, 'reac' )
    moose.connect( reaction, 'prd', buf, 'reac' )

    #Connect the function to the reaction
    moose.connect( func, 'valueOut', reaction, 'setNumKf' )

    #Connect the molecules to the func
    moose.connect( c, 'nOut', func.x[0], 'input' )

    #Set up solvers
    ksolve = moose.Ksolve( '/cylinder/ksolve' )
    ksolve.numThreads = nthreads
    dsolve = moose.Dsolve( '/cylinder/dsolve' )
    stoich = moose.Stoich( '/cylinder/stoich' )
    stoich.compartment = compt
    stoich.ksolve = ksolve
    stoich.dsolve = dsolve
    stoich.path = '/cylinder/##'
    for i in range( 10, 18 ):
        moose.setClock( i, dt )

    #initialize
    x = np.arange( 0, compt.x1, compt.diffLength )
    c.vec.nInit = [ (q < 0.2 * compt.x1) for q in x ]

    expected = [ (0.2, 0.40000000000000013)
            , (2.6704795776286974e-07, 1.2678976830753021e-17)
            , (8.167639617309419e-14, 3.8777269301457245e-24)
            , (2.498062905267963e-20, 1.1860363878961374e-30)
            , (7.64029581501609e-27, 3.6273808003690943e-37)
            ]

    # Run and plot it.
    moose.reinit()
    updateDt = 50
    runtime = updateDt * 4
    yvec = c.vec.n
    u1, m1 = np.mean( yvec ), np.std( yvec )
    print( u1, m1 )
    assert np.isclose( (u1, m1), expected[0], atol=1e-5).all(), expected[0]
    t1 = time.time()
    for i, t in enumerate(range( 0, runtime-1, updateDt)):
        moose.start( updateDt )
        yvec = c.vec.n
        u1, m1 = np.mean( yvec ), np.std( yvec )
        print( u1, m1 )
        np.isclose( (u1,m1), expected[i+1], atol=1e-5 ).all(), expected[i+1]
    return time.time() - t1

def main(nT):
    return test_ksolver_parallel(nT)

if __name__ == '__main__':
    import multiprocessing
    import sys
    nT = max(1, int(multiprocessing.cpu_count()))
    if len(sys.argv) > 1:
        nT = int(sys.argv[1])
    t2 = main( nT )
    print( 'With threads=%d, Time=%g' % (nT, t2))
