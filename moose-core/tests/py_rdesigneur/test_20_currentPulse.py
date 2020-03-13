import matplotlib as mpl
mpl.use('Agg')
import numpy as np
import matplotlib.pyplot as plt
import moose
print("[INFO ] MOOSE version=%s, loc=%s" % (moose.version(), moose.__file__))
import rdesigneur as rd

expected = (-0.051693861353435865, 0.004062563722923687 
        , np.array([7, 8, 11, 17, 34, 1852, 49, 31, 34, 958])
        , np.array([-0.065, -0.06309117, -0.06118235, -0.05927352, -0.05736469,
                -0.05545587, -0.05354704, -0.05163822, -0.04972939, -0.04782056,
                -0.04591174]))

def test_msgs():
    if moose.exists('/model'):
        moose.delete('/model')
    rdes = rd.rdesigneur(
        stimList = [['soma', '1', '.', 'inject', '(t>0.1 && t<0.2) * 2e-8' ]],
        plotList = [['soma', '1', '.', 'Vm', 'Soma membrane potential']]
    )
    rdes.buildModel()
    msgs = moose.listmsg(rdes.soma)
    assert len(msgs) == 3, msgs

def test_current_pulse():
    """Test current pulse.
    """
    rdes = rd.rdesigneur(
        stimList = [['soma', '1', '.', 'inject', '(t>0.1 && t<0.2) * 2e-8' ]],
        plotList = [['soma', '1', '.', 'Vm', 'Soma membrane potential']]
    )
    rdes.buildModel()
    moose.reinit()
    moose.start(0.3)
    rdes.display(block=False)
    y = moose.wildcardFind('/##[TYPE=Table]')[0].vector
    ym, ys = y.mean(), y.std()
    X = np.histogram(y)
    assert np.allclose(ym, expected[0])
    assert np.allclose(ys, expected[1])
    assert np.allclose(X[0], expected[2])
    assert np.allclose(X[1], expected[3])


if __name__ == '__main__':
    test_current_pulse()
    test_msgs()
