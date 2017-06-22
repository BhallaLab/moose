
# -*- coding: utf-8 -*-
## all SI units
########################################################################################
## Plot the firing rate vs current injection curve for a Cerebellar Granule Cell neuron
## Author: Aditya Gilra
## Creation Date: 2012-07-12
## Modification Date: 2012-07-12
########################################################################################

import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('.')
import moose
from moose.utils import *

from moose.neuroml.NeuroML import NeuroML

from pylab import *

SIMDT = 25e-6 # s
PLOTDT = 25e-6 # s
RUNTIME = 1.0 # s

injectmax = 2e-12 # Amperes

neuromlR = NeuroML()
neuromlR.readNeuroMLFromFile('cells_channels/CA1soma.morph.xml')
libcell = moose.Neuron('/library/CA1soma')
CA1Cellid = moose.copy(libcell,moose.Neutral('/cells'),'CA1')
CA1Cell = moose.Neuron(CA1Cellid)
#printCellTree(CA1Cell)

## edge-detect the spikes using spike-gen (table does not have edge detect)
spikeGen = moose.SpikeGen(CA1Cell.path+'/spikeGen')
spikeGen.threshold = -30e-3 # V
CA1CellSoma = moose.Compartment(CA1Cell.path+'/Seg0_soma_0_0')
CA1CellSoma.inject = 0 # by default the cell has a current injection
moose.connect(CA1CellSoma,'VmOut',spikeGen,'Vm')
## save spikes in table
table_path = moose.Neutral(CA1Cell.path+'/data').path
CA1CellSpikesTable = moose.Table(table_path+'/spikesTable')
moose.connect(spikeGen,'spikeOut',CA1CellSpikesTable,'input')

#CA1CellVmTable = moose.Table(table_path+'/vmTable')
#moose.connect(CA1CellSoma,'VmOut',CA1CellVmTable,'input')

cells_path = '/cells'

## from moose_utils.py sets clocks and resets/reinits
resetSim(['/cells'],SIMDT,PLOTDT,simmethod='hsolve') # from moose.utils

## Loop through different current injections
freqList = []
currentvec = arange(0, injectmax, injectmax/50.0)
for currenti in currentvec:
    moose.reinit()
    CA1CellSoma.inject = currenti
    moose.start(RUNTIME)
    spikesList = array(CA1CellSpikesTable.vector)
    if len(spikesList)>0:
        spikesList = spikesList[where(spikesList>0.0)[0]]
        spikesNow = len(spikesList)
    else: spikesNow = 0.0
    print(("For injected current =",currenti,\
        "number of spikes in",RUNTIME,"seconds =",spikesNow))
    freqList.append( spikesNow/float(RUNTIME) )

## plot the F vs I curve of the neuron
figure(facecolor='w')
plot(currentvec, freqList,'o-')
xlabel('time (s)')
ylabel('frequency (Hz)')
title('HH single-compartment Cell')
show()
