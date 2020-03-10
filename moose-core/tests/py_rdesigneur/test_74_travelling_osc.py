# This example illustrates molecular transport of an oscillatory reaction
# system, along a closed-end cylinder. The concentrations along the entire 
# length of the cylinder start out uniform. It shows an interesting 
# combination of propagating oscillations and buildup of reagents.
# Author: Upinder Bhalla 
# Converted to pydoc test by Dilawar Singh

import matplotlib
matplotlib.use('Agg')
import moose
import numpy as np
import pylab
import rdesigneur as rd

expected = (np.array([0.1, 0.15516149, 0.1403528 , 0.12798643, 0.14763476,
    0.13274035, 0.13840812, 0.14143158, 0.13480444, 0.14342124,
    0.13690342, 0.14346931, 0.13888074, 0.14376058, 0.14016138,
    0.14403992, 0.1415519 , 0.14333911, 0.143152  , 0.14287822,
    0.14605064, 0.14336306, 0.15619844, 0.14403064, 0.15615248,
    0.14539543, 0.1517716 , 0.14729328, 0.14838669, 0.14997354,
    0.14767709, 0.16223196, 0.14865582, 0.16134809, 0.14992121,
    0.15467856, 0.15189257, 0.15189602, 0.19084929, 0.15356592,
    0.15523634]), np.array([1.46079786e-17, 4.78646136e-02, 3.27393913e-02, 2.49512647e-02,
        3.63456677e-02, 3.83510871e-02, 3.26338079e-02, 6.95644575e-02,
        3.46265152e-02, 9.46756756e-02, 4.61280054e-02, 1.05823511e-01,
        5.86208446e-02, 1.05951587e-01, 7.15086316e-02, 9.68416361e-02,
        8.33557080e-02, 8.79500371e-02, 1.01320736e-01, 8.94334644e-02,
        1.37529592e-01, 9.51733672e-02, 2.21766095e-01, 1.01452560e-01,
        2.12351153e-01, 1.09061426e-01, 1.47326940e-01, 1.19309887e-01,
        1.19308264e-01, 1.48644433e-01, 1.21409122e-01, 2.55315182e-01,
        1.26555965e-01, 2.30541272e-01, 1.32675123e-01, 1.52134329e-01,
        1.45874039e-01, 1.37777639e-01, 6.86047595e-01, 1.43783302e-01,
        1.82237898e-01]))


def test_travelling_osc():
    """Test travelling wave.
    """
    rdes = rd.rdesigneur(
        turnOffElec = True,
        diffusionLength = 2e-6, 
        chemProto = [['makeChemOscillator()', 'osc']],
        chemDistrib = [['osc', 'soma', 'install', '1' ]],
        plotList = [
            ['soma', '1', 'dend/a', 'conc', 'Concentration of a'],
            ['soma', '1', 'dend/b', 'conc', 'Concentration of b'],
            ['soma', '1', 'dend/a', 'conc', 'Concentration of a', 'wave'],
        ],
        moogList = [['soma', '1', 'dend/a', 'conc', 'a Conc', 0, 360 ]]
    )
    a = moose.element( '/library/osc/kinetics/a' )
    b = moose.element( '/library/osc/kinetics/b' )
    s = moose.element( '/library/osc/kinetics/s' )
    a.diffConst = 0
    b.diffConst = 0
    a.motorConst = 1e-6
    rdes.buildModel()
    moose.reinit()
    #  rdes.displayMoogli( 1, 400, rotation = 0, azim = -np.pi/2, elev = 0.0 )
    moose.start(400)
    data = []
    for t in moose.wildcardFind('/##[TYPE=Table2]'): 
        data.append(t.vector[::10])
    m = np.mean(data, axis=0)
    u = np.std(data, axis=0)
    # The expected values are computed with GSL solver. These values will vary
    # a little with BOOST solver. Give some leeway for these tests.
    assert np.allclose(expected[0], m, rtol=1e-2), "Error is %s " % (expected[0]-m)
    A, B = expected[1], u
    assert np.allclose(B, A, rtol=1e-1), "Error is %s, base=%s" % ((A-B), B)
    print('done')
            

if __name__ == '__main__':
    test_travelling_osc()
