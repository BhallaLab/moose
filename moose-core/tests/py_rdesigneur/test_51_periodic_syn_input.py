# This example demonstrates periodic synaptic input to a cell.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3. No warranty.
# Doctest by Dilawar Singh

import moose
import rdesigneur as rd
import numpy as np


def test_periodic_syninput():
    rdes = rd.rdesigneur(
        cellProto = [['somaProto', 'soma', 20e-6, 200e-6]],
        chanProto = [['make_glu()', 'glu']],
        chanDistrib = [['glu', 'soma', 'Gbar', '1' ]],

        # Deliver stimulus to glu synapse on soma, periodically at 50 Hz.
        stimList = [['soma', '0.5', 'glu', 'periodicsyn', '50' ]],
        plotList = [['soma', '1', '.', 'Vm', 'Soma membrane potential']]
    )
    rdes.buildModel()
    moose.reinit()
    moose.start( 0.3 )
    t = moose.wildcardFind('/##[TYPE=Table]')[0].vector
    expected = np.array([-0.04995514162861773, 0.004795008283676097])
    got = np.array([t.mean(), t.std()])
    assert np.allclose(expected, got, rtol=1e-3), (expected, got)
    print('ok')
    return t

if __name__ == '__main__':
    test_periodic_syninput()
