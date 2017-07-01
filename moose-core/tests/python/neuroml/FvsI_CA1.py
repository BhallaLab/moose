#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
All SI units
## Plot the firing rate vs current injection curve for a Cerebellar Granule Cell neuron

## Author: Aditya Gilra
## Creation Date: 2012-07-12
## Modification Date: 2012-07-12

                    Wednesday 15 July 2015 09:46:36 AM IST
                    Added unittest
                    Modified for testing with ctest.
"""
from __future__ import print_function

import os
os.environ['NUMPTHREADS'] = '1'

import moose
from moose.neuroml.NeuroML import NeuroML

import numpy as np
import unittest

soma_ = None
cellSpikeTable_ = None

def loadModel(filename):
    global soma_, cellSpikeTable_
    neuromlR = NeuroML()
    neuromlR.readNeuroMLFromFile(filename)
    libcell = moose.Neuron('/library/CA1soma')
    CA1Cellid = moose.copy(libcell,moose.Neutral('/cells'),'CA1')
    CA1Cell = moose.Neuron(CA1Cellid)

    spikeGen = moose.SpikeGen(CA1Cell.path+'/spikeGen')
    spikeGen.threshold = -30e-3 # V
    soma_ = moose.Compartment(CA1Cell.path+'/Seg0_soma_0_0')
    soma_.inject = 0 # by default the cell has a current injection
    moose.connect(soma_,'VmOut',spikeGen,'Vm')
    table_path = moose.Neutral(CA1Cell.path+'/data').path
    cellSpikeTable_ = moose.Table(table_path+'/spikesTable')
    moose.connect(spikeGen,'spikeOut', cellSpikeTable_,'input')

def applyCurrent(currenti):
    global soma_, cellSpikeTable_
    moose.reinit()
    soma_.inject = currenti
    moose.start(1.0)
    spikesList = np.array(cellSpikeTable_.vector)
    if len(spikesList)>0:
        spikesList = spikesList[np.where(spikesList>0.0)[0]]
        spikesNow = len(spikesList)
    else:
        spikesNow = 0.0
    print("For injected current {0}, no of spikes in {1} second: {2}".format(
        currenti, 1.0, spikesNow )
        )
    return spikesNow

def ca1_main( currenti):
    return applyCurrent(currenti)
