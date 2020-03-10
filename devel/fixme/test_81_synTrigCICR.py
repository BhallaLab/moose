# This example demonstrates synaptic triggering of a wave of calcium
# release (CICR) from the endoplasmic reticulum. The wave is confined to 
# a subset of a dendrite where there is elevated IP3 present.
# 
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
# Converted to doctest by Dilawar Singh

import os
import moose
import numpy as np

import rdesigneur as rd

sdir_ = os.path.dirname(os.path.realpath(__file__))

expected = np.array([1.340e-05, 1.123e-16, 8.595e-09, 4.092e-01, 8.740e-05, 8.000e-05,
    3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05,
    3.007e-05, 3.008e-05, 3.011e-05, 3.026e-05, 3.091e-05, 3.106e-05,
    3.110e-05, 3.111e-05, 3.113e-05, 3.118e-05, 3.135e-05, 3.118e-05,
    3.113e-05, 3.112e-05, 3.111e-05, 3.111e-05, 3.111e-05, 3.111e-05,
    3.111e-05, 3.111e-05, 3.111e-05, 3.110e-05, 3.106e-05, 3.091e-05,
    3.026e-05, 3.011e-05, 3.008e-05, 3.007e-05, 3.007e-05, 3.007e-05,
    3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05,
    3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.008e-05,
    3.011e-05, 3.026e-05, 3.091e-05, 3.106e-05, 3.110e-05, 3.111e-05,
    3.113e-05, 3.118e-05, 3.135e-05, 3.118e-05, 3.113e-05, 3.112e-05,
    3.111e-05, 3.111e-05, 3.111e-05, 3.111e-05, 3.111e-05, 3.111e-05,
    3.111e-05, 3.110e-05, 3.106e-05, 3.091e-05, 3.026e-05, 3.011e-05,
    3.008e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05, 3.007e-05,
    3.007e-05, 3.007e-05, 4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01,
    4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.091e-01,
    4.089e-01, 4.089e-01, 4.089e-01, 4.090e-01, 4.090e-01, 4.090e-01,
    4.093e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01,
    4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.089e-01,
    4.089e-01, 4.089e-01, 4.091e-01, 4.090e-01, 4.090e-01, 4.090e-01,
    4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01, 4.090e-01])

def test():
    """
    Test CICR.

    >>> test() # doctest: +NORMALIZE_WHITESPACE
    Rdesigneur: Elec model has 5 compartments and 2 spines on 4 compartments.
        Chem part of model has the following compartments:
        | In dend, 40 voxels X 19 pools
        | In spine, 2 voxels X 1 pools
        | In psd, 2 voxels X 1 pools
        | In dend_endo, 40 voxels X 2 pools
    True
    """
    rdes = rd.rdesigneur(
        turnOffElec = False,
        chemDt = 0.002,
        chemPlotDt = 0.02,
        diffusionLength = 1e-6,
        numWaveFrames = 50,
        useGssa = False,
        addSomaChemCompt = False,
        addEndoChemCompt = True,
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSeg]
        cellProto = [['ballAndStick', 'soma', 10e-6, 10e-6, 2e-6, 40e-6, 4]],
        spineProto = [['makeActiveSpine()', 'spine']],
        chemProto = [[os.path.join(sdir_,'../py_rdesigneur/chem/CICRspineDend.g'), 'chem']],
        spineDistrib = [['spine', '#dend#', '10e-6', '0.1e-6']],
        chemDistrib = [['chem', 'dend#,spine#,head#', 'install', '1' ]],
        adaptorList = [
            [ 'Ca_conc', 'Ca', 'spine/Ca', 'conc', 0.00008, 8 ]
        ],
        stimList = [
            ['head0', '0.5', 'glu', 'periodicsyn', '1 + 40*(t>5 && t<6)'],
            ['head0', '0.5', 'NMDA', 'periodicsyn', '1 + 40*(t>5 && t<6)'],
            ['dend#',  'g>10e-6 && g<=31e-6', 'dend/IP3', 'conc', '0.0006' ],
            ],
        plotList = [
            ['head#', '1', 'spine/Ca', 'conc', 'Spine Ca conc'],
            ['dend#', '1', 'dend/Ca', 'conc', 'Dend Ca conc'],
            ['dend#', '1', 'dend/Ca', 'conc', 'Dend Ca conc', 'wave'],
            ['dend#', '1', 'dend_endo/CaER', 'conc', 'ER Ca conc', 'wave'],
            ['soma', '1', '.', 'Vm', 'Memb potl'],
        ],
    )
    moose.seed( 1234 )
    rdes.buildModel()
    moose.reinit()
    moose.start(2)
    data = []
    for t in moose.wildcardFind('/##[TYPE=Table2]'):
        data.append(t.vector[::20])
    res = np.mean(data, axis=1)
    assert np.allclose(res, expected, rtol=1e-4, atol=1e-4), \
            "Total error %s" % np.sum(res - expected)
    return True

if __name__ == '__main__':
    test()
    print('All done')
