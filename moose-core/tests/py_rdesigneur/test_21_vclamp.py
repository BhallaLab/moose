import moose
import numpy as np
import rdesigneur as rd

def test_21_vclamp():
    """Test vclamp.
    """
    rdes = rd.rdesigneur(
        stimList = [['soma', '1', '.', 'vclamp', '-0.065 + (t>0.1 && t<0.2) * 0.02' ]],
        plotList = [
            ['soma', '1', '.', 'Vm', 'Soma membrane potential'],
            ['soma', '1', 'vclamp', 'current', 'Soma holding current'],
        ]
    )
    rdes.buildModel()
    moose.reinit()
    moose.start( 0.3 )
    # rdes.display(block=False)
    data = []
    for t in moose.wildcardFind('/##[TYPE=Table]'):
        data.append(t.vector)
    mean = np.mean(data, axis=1)
    std = np.std(data, axis=1)
    assert np.allclose([-5.83422152e-02, -9.28563233e-09], mean), mean
    assert np.allclose([9.41512562e-03, 2.79081939e-08], std), std
    return data

if __name__ == '__main__':
    test_21_vclamp()
