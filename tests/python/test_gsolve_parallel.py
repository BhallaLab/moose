import numpy as np
import moose
print( '[INFO] Using moose from %s' % moose.__file__ )
import time
moose.seed( 10 )

def main( nT ):
    """
    This example implements a reaction-diffusion like system which is
    bistable and propagates losslessly. It is based on the NEURON example 
    rxdrun.py, but incorporates more compartments and runs for a longer time.
    The system is implemented in a function rather than as a proper system
    of chemical reactions. Please see rxdReacDiffusion.py for a variant that 
    uses a reaction plus a function object to control its rates.
    """
    print( 'Using %d threads' % nT )

    dt = 0.1

    # define the geometry
    compt = moose.CylMesh( '/cylinder' )
    compt.r0 = compt.r1 = 100e-9
    compt.x1 = 200e-9
    compt.diffLength = 0.2e-9
    assert( compt.numDiffCompts == compt.x1/compt.diffLength )

    #define the molecule. Its geometry is defined by its parent volume, cylinder
    c = moose.Pool( '/cylinder/pool' )
    c.diffConst = 1e-13 # define diffusion constant


    # Here we set up a function calculation
    func = moose.Function( '/cylinder/pool/func' )
    func.expr = "(-x0 * (30e-9 - x0) * (100e-9 - x0))*0.0001"
    func.x.num = 1 #specify number of input variables.

    #Connect the molecules to the func
    moose.connect( c, 'nOut', func.x[0], 'input' )
    #Connect the function to the pool
    moose.connect( func, 'valueOut', c, 'increment' )

    #Set up solvers
    ksolve = moose.Gsolve( '/cylinder/Gsolve' )
    try:
        ksolve.numThreads = nT
    except Exception as e:
        print( 'OLD MOOSE. Does not support multithreading' )
    dsolve = moose.Dsolve( '/cylinder/dsolve' )
    stoich = moose.Stoich( '/cylinder/stoich' )
    stoich.compartment = compt
    stoich.ksolve = ksolve
    stoich.dsolve = dsolve
    stoich.path = '/cylinder/##'

    #initialize
    x = np.arange( 0, compt.x1, compt.diffLength )
    c.vec.nInit = [ 1000.0 for q in x ]

    # Run and plot it.
    moose.reinit()
    updateDt = 50
    runtime = updateDt * 10
    t1 = time.time()
    res = []
    for t in range( 0, runtime-1, updateDt ):
        moose.start( updateDt )
        y = c.vec.n
        res.append( (np.mean(y), np.std(y)) )

    expected = [(9.0, 0.0), (6.0, 0.0), (5.0, 0.0), (3.0, 0.0), (2.0, 0.0),
        (2.0, 0.0), (2.0, 0.0), (1.0, 0.0), (1.0, 0.0), (1.0, 0.0)]
    print(("Time = ", time.time() - t1))
    print( res )
    assert res == expected

if __name__ == '__main__':
    import sys
    import multiprocessing
    nT = int(multiprocessing.cpu_count() / 2)
    if len(sys.argv) > 1:
        nT = int(sys.argv[1])
    main( nT )

