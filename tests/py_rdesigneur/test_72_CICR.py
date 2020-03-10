# This example demonstrates insertion of endo-compartments into the
# dendrite. Here endo_compartments are used for the endoplasmic reticulum
# (ER) in a model of Calcium Induced Calcium Release through the
# IP3 receptor. It generates a series of propagating waves of calcium.
# Note that units of permeability in the ConcChan are 1/(millimolar.sec)
#
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
# Converted to a test by Dilawar Singh, 2020

import matplotlib as mpl
mpl.use('Agg')

import os
import moose
print("[INFO ] Using moose from %s (%s)" % (moose.__file__, moose.version()))

import numpy as np
import rdesigneur as rd

np.set_printoptions(precision=3)

sdir_ = os.path.dirname(os.path.realpath(__file__))

E = (np.array([1.09014453e-07, 7.28082797e-13, 2.75389935e-08, 4.09373273e-01,
    5.13839676e-04, 5.04392239e-04, 5.18535951e-04, 5.20332653e-04,
    5.20319412e-04, 5.20315927e-04, 5.20315785e-04, 5.20315780e-04,
    5.20315780e-04, 5.20315780e-04, 5.13839676e-04, 5.04392239e-04,
    5.18535951e-04, 5.20332653e-04, 5.20319412e-04, 5.20315927e-04,
    5.20315785e-04, 5.20315780e-04, 5.20315780e-04, 5.20315780e-04,
    4.03334121e-01, 4.04616316e-01, 4.03839819e-01, 4.03873596e-01,
    4.03877574e-01, 4.03877276e-01, 4.03877250e-01, 4.03877249e-01,
    4.03877249e-01, 4.03877249e-01, 1.08136177e-06, 1.03726538e-06,
    1.04624969e-06, 1.04989891e-06, 1.05005782e-06, 1.05006129e-06,
    1.05006147e-06, 1.05006148e-06, 1.05006148e-06, 1.05006148e-06]), 
    np.array([2.64763531e-06, 3.53901405e-12, 1.06297817e-07, 2.59647692e-05,
        1.50771752e-03, 1.44372345e-03, 1.46452771e-03, 1.46445738e-03,
        1.46426743e-03, 1.46425938e-03, 1.46425914e-03, 1.46425913e-03,
        1.46425913e-03, 1.46425913e-03, 1.50771752e-03, 1.44372345e-03,
        1.46452771e-03, 1.46445738e-03, 1.46426743e-03, 1.46425938e-03,
        1.46425914e-03, 1.46425913e-03, 1.46425913e-03, 1.46425913e-03,
        1.26799318e-02, 1.15981501e-02, 1.19280784e-02, 1.20059244e-02,
        1.20092971e-02, 1.20092807e-02, 1.20092772e-02, 1.20092772e-02,
        1.20092772e-02, 1.20092772e-02, 2.11602709e-06, 2.06303080e-06,
        2.08117025e-06, 2.08584557e-06, 2.08603181e-06, 2.08603541e-06,
        2.08603560e-06, 2.08603562e-06, 2.08603562e-06, 2.08603562e-06])
    )


def test_CICR():
    """Test CICR
    """
    rdes = rd.rdesigneur(
        turnOffElec=True,
        chemDt=0.005,
        chemPlotDt=0.02,
        numWaveFrames=200,
        diffusionLength=1e-6,
        useGssa=False,
        addSomaChemCompt=False,
        addEndoChemCompt=True,
        # cellProto syntax: ['somaProto', 'name', dia, length]
        cellProto=[['somaProto', 'soma', 2e-6, 10e-6]],
        chemProto=[[os.path.join(sdir_, 'chem', 'CICRwithConcChan.g'),
                    'chem']],
        chemDistrib=[['chem', 'soma', 'install', '1']],
        plotList=[
            ['soma', '1', 'dend/CaCyt', 'conc', 'Dendritic Ca'],
            ['soma', '1', 'dend/CaCyt', 'conc', 'Dendritic Ca', 'wave'],
            ['soma', '1', 'dend_endo/CaER', 'conc', 'ER Ca'],
            ['soma', '1', 'dend/ActIP3R', 'conc', 'active IP3R'],
        ],
    )
    rdes.buildModel()
    IP3 = moose.element('/model/chem/dend/IP3')
    IP3.vec.concInit = 0.004
    IP3.vec[0].concInit = 0.02
    moose.reinit()

    moose.start(20)
    data = [t.vector for t in moose.wildcardFind('/##[TYPE=Table2]')]
    m, s = np.mean(data, axis=1), np.std(data, axis=1)
    # print(np.array_repr(m))
    # print(np.array_repr(s))
    # In multithreaded mode, the numers are not exactly the same as in
    # expected.
    assert np.allclose(m, E[0], rtol=1e-2, atol=1e-4), (m - E[0])
    # standard deviation could be very low in some cases.
    print(np.sum(abs(s-E[1])) )
    assert np.sum(abs(s-E[1])) < 1e-2, "Got %s" % np.sum(abs(s-E[1]))
    print('done')

if __name__ == '__main__':
    test_CICR()
