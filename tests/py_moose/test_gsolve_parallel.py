import numpy as np
import moose
print( '[INFO] Using moose from %s' % moose.__file__ )
import time

# Does not guarantee thread determinism in multithreaded Gsolve/Ksolve.
moose.seed( 10 )

def printCompt(compt):
    print( 'x0=%s, x1=%s, diffLength=%s, numDiffCompt=%d' % (compt.x0, compt.x1,
        compt.diffLength, compt.numDiffCompts))

def test_gsolve_paralllel(nT=4):
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
    compt.x1 = 200e-09
    compt.diffLength = 0.2e-9
    assert( compt.numDiffCompts == compt.x1/compt.diffLength)

    #define the molecule. Its geometry is defined by its parent volume, cylinder
    c = moose.Pool( '/cylinder/pool' )
    c.diffConst = 1e-13 # define diffusion constant

    # Here we set up a function calculation
    func = moose.Function( '/cylinder/pool/func' )
    func.expr = "(-x0*(30e-9-x0)*(100e-9-x0))*0.0001"
    #  func.x.num = 1 #specify number of input variables.

    #Connect the molecules to the func
    moose.connect( c, 'nOut', func.x[0], 'input' )
    #Connect the function to the pool
    moose.connect( func, 'valueOut', c, 'increment' )

    #Set up solvers
    ksolve = moose.Gsolve( '/cylinder/Gsolve' )
    ksolve.numThreads = nT

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
    clk = moose.element( '/clock' )
    for t in range( 0, runtime-1, updateDt ):
        y = c.vec.n
        s = np.sum(y)
        v = (np.mean(y), np.max(y), np.min(y), s)
        print(v)
        res.append(v)
        moose.start( updateDt )
        currTime = clk.currentTime

    # One molecule here and there because thread launching has undeterministic
    # characteristics. Even after setting moose.seed; we may not see same
    # numbers on all platfroms.
    expected = [
            (1000.0, 1000.0, 1000.0, 1000000.0)
            , (9.908, 10.0, 8.0, 9908.0)
            , (6.869, 7.0, 6.0, 6869.0)
            , (5.354, 6.0, 5.0, 5354.0)
            , (4.562, 5.0, 4.0, 4562.0)
            , (3.483, 4.0, 3.0, 3483.0)
            , (3.043, 4.0, 3.0, 3043.0)
            , (2.261, 3.0, 2.0, 2261.0)
            , (1.967, 2.0, 1.0, 1967.0)
            , (1.997, 2.0, 1.0, 1997.0) ]
    print("Time = ", time.time() - t1)
    assert np.isclose(res, expected, atol=1, rtol=1).all(), "Got %s, expected %s" % (res, expected)

def main(nT):
    test_gsolve_paralllel(nT)

if __name__ == '__main__':
    import sys
    import multiprocessing
    nT = max(1, int(multiprocessing.cpu_count()))
    if len(sys.argv) > 1:
        nT = int(sys.argv[1])
    main( nT )

