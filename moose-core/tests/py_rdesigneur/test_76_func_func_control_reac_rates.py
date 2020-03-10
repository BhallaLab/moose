########################################################################
# This example illustrates how a function can be used to control a reaction
# rate. This kind of calculation is appropriate when we need to link 
# different kinds of physical processses with chemical reactions, for  
# example, membrane curvature with molecule accumulation. The use of
# functions to modify reaction rates should be avoided in purely chemical 
# systems since they obscure the underlying chemistry, and do not map
# cleanly to stochastic calculations.
#
# In this example we simply have a molecule C that controls the forward
# rate of a reaction that converts A to B. C is a function of location
# on the cylinder, and is fixed. In more elaborate computations we could
# have a function of multiple molecules, some of which could be changing and
# others could be buffered.
#  
#  Copyright (C) Upinder S. Bhalla NCBS 2018
#  Released under the terms of the GNU Public License V3.
########################################################################

import matplotlib
matplotlib.use('Agg')
import numpy as np
import moose
import rdesigneur as rd

def makeFuncRate():
    model = moose.Neutral( '/library' )
    model = moose.Neutral( '/library/chem' )
    compt = moose.CubeMesh( '/library/chem/compt' )
    compt.volume = 1e-15
    A = moose.Pool( '/library/chem/compt/A' )
    B = moose.Pool( '/library/chem/compt/B' )
    C = moose.Pool( '/library/chem/compt/C' )
    reac = moose.Reac( '/library/chem/compt/reac' )
    func = moose.Function( '/library/chem/compt/reac/func' )
    func.x.num = 1
    func.expr = "(x0/1e8)^2"
    moose.connect( C, 'nOut', func.x[0], 'input' )
    moose.connect( func, 'valueOut', reac, 'setNumKf' )
    moose.connect( reac, 'sub', A, 'reac' )
    moose.connect( reac, 'prd', B, 'reac' )

    A.concInit = 1
    B.concInit = 0
    C.concInit = 0
    reac.Kb = 1

def test_reac_rates():
    makeFuncRate()
    rdes = rd.rdesigneur(
        turnOffElec = True,
        #This subdivides the 50-micron cylinder into 2 micron voxels
        diffusionLength = 2e-6,
        cellProto = [['somaProto', 'soma', 5e-6, 50e-6]],
        chemProto = [['chem', 'chem']],
        chemDistrib = [['chem', 'soma', 'install', '1' ]],
        plotList = [['soma', '1', 'dend/A', 'conc', 'A conc', 'wave'],
            ['soma', '1', 'dend/C', 'conc', 'C conc', 'wave']],
    )
    rdes.buildModel()

    C = moose.element( '/model/chem/dend/C' )
    C.vec.concInit = [ 1+np.sin(x/5.0) for x in range( len(C.vec) ) ]
    moose.reinit()
    moose.start(10)

    E = (np.array([0.76793869, 0.69093771, 0.61740932, 0.55290337, 0.50043755,
       0.46090277, 0.43395074, 0.41882627, 0.41492281, 0.42206285,
       0.4405646 , 0.47111808, 0.51443061, 0.57058404, 0.63814939,
       0.71337226, 0.79003606, 0.86055299, 0.91814872, 0.95908718,
       0.9836508 , 0.99540289, 0.99934529, 0.99998887, 0.99999589,
       1.        , 1.19866933, 1.38941834, 1.56464247, 1.71735609,
       1.84147098, 1.93203909, 1.98544973, 1.9995736 , 1.97384763,
       1.90929743, 1.8084964 , 1.67546318, 1.51550137, 1.33498815,
       1.14112001, 0.94162586, 0.7444589 , 0.55747956, 0.38814211,
       0.2431975 , 0.12842423, 0.04839793, 0.006309  , 0.00383539]),
       np.array([1.35368806e-01, 1.74391515e-01, 2.08263083e-01, 2.34835055e-01,
       2.53957994e-01, 2.66695461e-01, 2.74465660e-01, 2.78476408e-01,
       2.79468906e-01, 2.77640055e-01, 2.72631277e-01, 2.63551810e-01,
       2.49094224e-01, 2.27869755e-01, 1.99072915e-01, 1.63375068e-01,
       1.23566311e-01, 8.42635843e-02, 5.04491469e-02, 2.55522217e-02,
       1.02890632e-02, 2.90344842e-03, 4.13990858e-04, 7.03942852e-06,
       2.60159221e-06, 0.00000000e+00, 0.00000000e+00, 2.22044605e-16,
       0.00000000e+00, 0.00000000e+00, 2.22044605e-16, 2.22044605e-16,
       2.22044605e-16, 2.22044605e-16, 4.44089210e-16, 0.00000000e+00,
       2.22044605e-16, 2.22044605e-16, 0.00000000e+00, 2.22044605e-16,
       2.22044605e-16, 0.00000000e+00, 1.11022302e-16, 0.00000000e+00,
       0.00000000e+00, 2.77555756e-17, 0.00000000e+00, 0.00000000e+00,
       0.00000000e+00, 0.00000000e+00]))



    A = []
    for t in moose.wildcardFind('/##[TYPE=Table2]'):
        A.append(t.vector)
    m = np.mean(A, axis=1)
    u = np.std(A, axis=1)

    # multithreaded version given different results.
    assert np.allclose(m, E[0], rtol=1e-3), (m-E[0])
    assert np.allclose(u, E[1], rtol=1e-3), (u-E[1])
    print('all done')

if __name__ == '__main__':
    test_reac_rates()
