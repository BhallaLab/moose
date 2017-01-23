## __BROKEN__
## all SI units
########################################################################################
## Plot the membrane potential for a leaky integrate and fire neuron with current injection
## Author: Aditya Gilra
## Creation Date: 2012-06-08
## Modification Date: 2012-06-08
########################################################################################

import os

SIMDT = 5e-5 # seconds
PLOTDT = 5e-5 # seconds
RUNTIME = 2.0 # seconds
injectI = 2.5e-12 # Amperes

## moose imports
import moose
print(( '[DBEUG] Using moose from %s' % moose.__file__ ))

from moose.neuroml import *
from moose.utils import * # has setupTable(), resetSim() etc
import math

## import numpy and matplotlib in matlab style commands
from pylab import *

def create_LIF():
    neuromlR = NeuroML()
    neuromlR.readNeuroMLFromFile('cells_channels/LIF.morph.xml')
    libcell = moose.Neuron('/library/LIF')
    LIFCellid = moose.copy(libcell,moose.Neutral('/cells'),'IF1')
    # FIXME: No LeakyIaF is found in MOOSE.
    LIFCell = moose.LeakyIaF(LIFCellid)
    return LIFCell

def run_LIF():
    cells_path = '/cells'

    ## reset and run the simulation
    print("Reinit MOOSE.")
    ## from moose_utils.py sets clocks and resets
    resetSim(['/cells'], SIMDT, PLOTDT, simmethod='hsolve')
    print("Running now...")
    moose.start(RUNTIME)

if __name__ == '__main__':
    IF1 = create_LIF()
    IF1.inject = injectI
    IF1vmTable = setupTable("vmTableIF1",IF1,'Vm')

    ## edge-detect the spikes using spike-gen (table does not have edge detect)
    spikeGen = moose.SpikeGen(IF1.path+'/spikeGen')
    spikeGen.threshold = IF1.Vthreshold
    moose.connect(IF1,'VmOut',spikeGen,'Vm')
    ## save spikes in table
    table_path = moose.Neutral(IF1.path+'/data').path
    IF1spikesTable = moose.Table(table_path+'/spikesTable')
    moose.connect(spikeGen,'event',IF1spikesTable,'input')

    run_LIF()
    print(("Spiketimes :",IF1spikesTable.vector))
    ## plot the membrane potential of the neuron
    timevec = arange(0.0,RUNTIME+PLOTDT/2.0,PLOTDT)
    figure(facecolor='w')
    plot(timevec, IF1vmTable.vector)
    show()
