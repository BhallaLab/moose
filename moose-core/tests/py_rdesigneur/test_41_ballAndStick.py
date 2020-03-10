# This example demonstrates loading a ball-and-stick model of a neuron
# with 10 compartments in the dendrite, that support propagating
# action potentials.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
#
# Turned to a test by Dilawar Singh

import matplotlib
matplotlib.use('Agg')
import moose
import numpy as np
import rdesigneur as rd

expected =  np.array([-0.065, -0.068, -0.07 , -0.071, -0.072, -0.072, -0.072, -0.072,
    -0.072, -0.072, -0.072, -0.048,  0.03 , -0.019, -0.054, -0.07 ,
    -0.07 , -0.067, -0.065, -0.062, -0.06 , -0.056, -0.048, -0.005,
    -0.024, -0.055, -0.069, -0.069, -0.067, -0.064, -0.062, -0.059,
    -0.055, -0.046, -0.005, -0.028, -0.057, -0.069, -0.068, -0.066,
    -0.064, -0.061, -0.058, -0.054, -0.044, -0.004, -0.031, -0.059,
    -0.069, -0.068, -0.066, -0.075, -0.076, -0.075, -0.075, -0.075,
    -0.075, -0.074, -0.074, -0.073, -0.073])

def test_ball_and_stick():
    """Test 41 Ball and Stick model.
    """
    rdes = rd.rdesigneur(
        cellProto = [['ballAndStick', 'soma', 20e-6, 20e-6, 4e-6, 500e-6, 10]],
        chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
        chanDistrib = [
            ['Na', 'soma', 'Gbar', '1200' ],
            ['K', 'soma', 'Gbar', '360' ],
            ['Na', 'dend#', 'Gbar', '400' ],
            ['K', 'dend#', 'Gbar', '120' ]
            ],
        stimList = [['soma', '1', '.', 'inject', '(t>0.01 && t<0.05) * 1e-9' ]],
        plotList = [['soma', '1', '.', 'Vm', 'Membrane potential']],
        moogList = [['#', '1', '.', 'Vm', 'Vm (mV)']]
    )
    rdes.buildModel()
    soma = moose.element( '/model/elec/soma' )
    moose.reinit()
    moose.start(0.06)
    t = [x.vector for x in moose.wildcardFind('/##[TYPE=Table]')]
    m, u = np.mean(t), np.std(t)
    assert np.allclose([m, u], [-0.05820780543933818, 0.02098328548146809]), \
            (m, u)
    print('done')
    return True

if __name__ == '__main__':
    test_ball_and_stick()
