# This example demonstrates a multiscale model with synaptic input, Ca
# entry to the spine, receptor modulation following phosphorylation and 
# Ca diffusion from spine to the dendrite. Lots going on. 
# System switches to a potentiated state after a 1s strong synaptic input.
#
#   Ca+CaM <===> Ca_CaM;    Ca_CaM + CaMKII <===> Ca_CaM_CaMKII (all in 
#       spine head, except that the Ca_CaM_CaMKII translocates to the PSD)
#   chan ------Ca_CaM_CaMKII-----> chan_p; chan_p ------> chan  (all in PSD)
# 
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
# Convered to doctest by Dilawar Singh

import os
import moose
import numpy as np
import rdesigneur as rd

sdir_ = os.path.dirname(os.path.realpath(__file__))

A = np.array([3.522e-05, 3.298e-04, 1.752e-05, 1.879e-02, 1.629e-02, 1.533e-04,
       1.538e-04, 1.546e-04, 1.559e-04, 1.576e-04, 1.597e-04, 1.623e-04,
       1.655e-04, 1.693e-04, 1.738e-04, 1.791e-04, 1.852e-04, 1.922e-04,
       2.002e-04, 2.094e-04, 2.197e-04, 2.314e-04, 2.446e-04, 2.596e-04,
       2.765e-04, 2.955e-04, 3.171e-04, 3.415e-04, 3.693e-04, 4.008e-04,
       4.369e-04, 4.783e-04, 5.259e-04, 5.811e-04, 6.455e-04, 7.212e-04,
       8.076e-04, 8.993e-04, 9.039e-04, 9.105e-04, 9.189e-04, 9.293e-04,
       9.417e-04, 9.562e-04, 9.729e-04, 9.918e-04, 1.013e-03, 1.037e-03,
       1.063e-03, 1.092e-03, 1.123e-03, 1.157e-03, 1.193e-03, 1.233e-03,
       1.275e-03, 1.320e-03, 1.305e-03, 1.293e-03, 1.285e-03, 1.279e-03,
       1.276e-03, 1.029e-03, 1.035e-03, 3.114e-02, 2.892e-02, 4.087e-03,
       3.402e-03])

B = np.array([6.761e-06, 1.049e-04, 4.196e-06, 7.230e-02, 6.142e-02, 8.928e-05,
       8.956e-05, 9.012e-05, 9.096e-05, 9.209e-05, 9.350e-05, 9.524e-05,
       9.735e-05, 9.990e-05, 1.029e-04, 1.065e-04, 1.106e-04, 1.154e-04,
       1.208e-04, 1.271e-04, 1.342e-04, 1.422e-04, 1.514e-04, 1.618e-04,
       1.735e-04, 1.869e-04, 2.021e-04, 2.195e-04, 2.394e-04, 2.625e-04,
       2.894e-04, 3.210e-04, 3.586e-04, 4.041e-04, 4.601e-04, 5.305e-04,
       6.183e-04, 7.234e-04, 6.871e-04, 6.725e-04, 6.715e-04, 6.796e-04,
       6.937e-04, 7.114e-04, 7.310e-04, 7.515e-04, 7.720e-04, 7.922e-04,
       8.118e-04, 8.311e-04, 8.504e-04, 8.702e-04, 8.913e-04, 9.150e-04,
       9.429e-04, 9.780e-04, 9.673e-04, 9.636e-04, 9.632e-04, 9.640e-04,
       9.647e-04, 2.049e-03, 1.984e-03, 1.499e-02, 1.382e-02, 2.415e-03,
       1.918e-03])
def test():
    """Test 
    """
    rdes = rd.rdesigneur(
        elecDt = 50e-6,
        chemDt = 0.002,
        diffDt = 0.002,
        chemPlotDt = 0.02,
        useGssa = False,
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
        cellProto = [['ballAndStick', 'soma', 12e-6, 12e-6, 4e-6, 100e-6, 2 ]],
        chemProto = [[os.path.join(sdir_,
            '../py_rdesigneur/chem/chanPhosph3compt.g'), 'chem']],
        spineProto = [['makeActiveSpine()', 'spine']],
        chanProto = [
            ['make_Na()', 'Na'], 
            ['make_K_DR()', 'K_DR'], 
            ['make_K_A()', 'K_A' ],
            ['make_Ca()', 'Ca' ],
            ['make_Ca_conc()', 'Ca_conc' ]
        ],
        passiveDistrib = [['soma', 'CM', '0.01', 'Em', '-0.06']],
        spineDistrib = [['spine', '#dend#', '50e-6', '1e-6']],
        chemDistrib = [['chem', '#', 'install', '1' ]],
        chanDistrib = [
            ['Na', 'soma', 'Gbar', '300' ],
            ['K_DR', 'soma', 'Gbar', '250' ],
            ['K_A', 'soma', 'Gbar', '200' ],
            ['Ca_conc', 'soma', 'tau', '0.0333' ],
            ['Ca', 'soma', 'Gbar', '40' ]
        ],
        adaptorList = [
            [ 'psd/chan_p', 'n', 'glu', 'modulation', 0.1, 1.0 ],
            [ 'Ca_conc', 'Ca', 'spine/Ca', 'conc', 0.00008, 8 ]
        ],
        # Syn input basline 1 Hz, and 40Hz burst for 1 sec at t=20. Syn weight
        # is 0.5, specified in 2nd argument as a special case stimLists. 
        stimList = [['head#', '0.5','glu', 'periodicsyn', '1 + 40*(t>10 && t<11)']],
        plotList = [
            ['soma', '1', '.', 'Vm', 'Membrane potential'],
            ['#', '1', 'spine/Ca', 'conc', 'Ca in Spine'],
            ['#', '1', 'dend/DEND/Ca', 'conc', 'Ca in Dend'],
            ['#', '1', 'spine/Ca_CaM', 'conc', 'Ca_CaM'],
            ['head#', '1', 'psd/chan_p', 'conc', 'Phosph gluR'],
            ['head#', '1', 'psd/Ca_CaM_CaMKII', 'conc', 'Active CaMKII'],
        ]
    )
    moose.seed(123)
    rdes.buildModel()
    moose.reinit()
    moose.start(25)
    data = []
    v = moose.wildcardFind('/##[TYPE=Table]')[0].vector
    assert np.allclose( (v.mean(), v.std()), (-0.06777396715033643,
        0.008550767915889)), (v.mean(), v.std())
    for t in moose.wildcardFind('/##[TYPE=Table2]'):
        data.append(t.vector)

    m = np.mean(data, axis=1)
    u = np.std(data, axis=1)
    assert m.shape[0] == 67
    assert np.allclose(m, A, atol=1e-5), m - A
    assert np.allclose(u, B, atol=1e-5), u - B

if __name__ == '__main__':
    test()
