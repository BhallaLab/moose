import moose
import numpy as np
import rdesigneur as rd

def test_chem_osc():
    """Test chemical oscillation.
    """
    rdes = rd.rdesigneur(
        turnOffElec = True,
        diffusionLength = 1e-3, # Default diffusion length is 2 microns
        chemProto = [['makeChemOscillator()', 'osc']],
        chemDistrib = [['osc', 'soma', 'install', '1' ]],
        plotList = [['soma', '1', 'dend/a', 'conc', 'a Conc'],
            ['soma', '1', 'dend/b', 'conc', 'b Conc']]
    )
    rdes.buildModel()
    b = moose.element( '/model/chem/dend/b' )
    b.concInit *= 5
    moose.reinit()
    moose.start(200)
    res = [t.vector for t in moose.wildcardFind('/##[TYPE=Table2]')]
    m = np.mean(res, axis=1)
    s = np.std(res, axis=1)
    expected = ([0.13622532, 0.13261175], [0.08518292, 0.06766806])
    assert np.allclose(expected, [m,s]), (m,s)

if __name__ == '__main__':
    test_chem_osc()
