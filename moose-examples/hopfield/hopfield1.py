# __BROKEN__
# PARTIAL DEMO
# demo of a simple hopfield network using leaky integrate and fire neurons
# memory.csv has the memory being saved, the synaptic weights are set at start according to this memory
# memory must be a square matrix of 0's and 1's only
# input.csv has partial input given as input to the cells.
# - By C.H.Chaitanya
# This code is available under GPL3 or greater GNU license 
import sys
sys.path.append('../../python')

import moose
import math
from moose.utils import *
import csv
import numpy as np
#import matplotlib.pyplot as plt
from pylab import *
#from datetime import datetime

cell = []
spikegen = []
vmtable = []
intable = {}

def updateWeights(old_weights, training_data):
    new_weights = np.array(
            [x * y for x in training_data[:,0] for y in training_data[:,1] ]
            )
    new_weights = new_weights.reshape(old_weights.shape)
    old_weights += new_weights
    return old_weights

def createNetwork(synWeights,inputGiven):
    """synWeights : 2D array. synWeights[ii][jj] is the weight of
    connection from cell [ii] to cell [jj]."""
    global cell
    global vmtable
    global spikegen
    global intable 
#    start = datetime.now()
    numberOfCells = synWeights.shape[0]
    hopfield = moose.Neutral('/hopfield')
    data = moose.Neutral('/data')
    pg = moose.PulseGen('/hopfield/inPulGen')
    pgTable = moose.Table('/hopfield/inPulGen/pgTable')
    moose.connect(pgTable, 'requestOut', pg, 'getOutputValue')
#    pg.firstDelay = 10e-3
#    pg.firstWidth = 2e-03
#    pg.firstLevel = 3
#    pg.secondDelay = 1.0
    pg.count = 1
    pg.level[0] = 3
    pg.width[0] = 2e-03
    pg.delay[0] = 50e-03 #every 50ms
#    start1 = datetime.now()
    for ii in range(numberOfCells):
        cell.append(moose.IntFire('/hopfield/cell_%d' % (ii)))
        cell[ii].tau = 10e-3
        cell[ii].Vm = -0.07
        cell[ii].synapse.num = numberOfCells
        cell[ii].synapse[ii].delay = 1e-3
        cell[ii].synapse[ii].weight = 1.0
        cell[ii].setField('thresh', 0.98)
        vmtable.append(moose.Table('/data/Vm_%d' % (ii)))
        moose.connect(vmtable[ii], 'requestOut', cell[ii], 'getVm')
        spikegen.append(moose.SpikeGen('/hopfield/inSpkGen_%d' % (ii)))
        if inputGiven[ii] == 0:
            spikegen[ii].threshold = 4.0
        else:
            spikegen[ii].threshold = 2.0
        spikegen[ii].edgeTriggered = True # Redundant
        intable[ii] = moose.Table('/data/inTable_%d' % (ii))
        moose.connect(pg, 'output', spikegen[ii], 'Vm')
        # No self connection - so we can use the synapse [ii] for input delivery
        moose.connect(spikegen[ii], 'spikeOut', cell[ii].synapse[ii], 'addSpike')
        moose.connect(intable[ii], 'requestOut', spikegen[ii], 'getHasFired')
#    end1 = datetime.now()
    for ii in range(numberOfCells):
        for jj in range(numberOfCells):
            if ii == jj:
                continue
            cell[jj].synapse[ii].weight = float(synWeights[ii,jj]/2.0)
            cell[jj].synapse[ii].delay = 20e-3
            moose.connect(cell[ii], 'spike', cell[jj].synapse[ii], 'addSpike')
#    end2 = datetime.now()
#    delta = end2 - start
#    print 'createNetwork: Total time:', delta.seconds + delta.microseconds * 1e-6
#    delta = end1 - start1
#    print 'createNetwork: create cells:', delta.seconds + delta.microseconds * 1e-6
#    delta = end2 - end1
#    print 'createNetwork: connect cells:', delta.seconds + delta.microseconds * 1e-6
    return (cell,vmtable,pgTable, spikegen, intable)
    
def update_conn(inputFileName):
    inputdata = np.loadtxt(inputFileName)
    for ii in range(len(spikegen)):
        if inputGiven[ii] == 0:
            spikegen[ii].threshold = 4.0
        else:
            spiksegen[ii].threshold = 2.0
                          
if __name__ == '__main__':
    memoryFile1 = "memory1.csv"
    memory = np.loadtxt(memoryFile1)
    synWeights = np.zeros((len(memory), len(memory)))
    synWeights = updateWeights(synWeights, memory)

    memoryFile2 = "memory2.csv"
    memory2 = np.loadtxt(memoryFile2)
    synWeights = updateWeights(synWeights, memory2)

    inputFile = "input.csv"
    inputData = np.loadtxt(inputFile)
    #cells,Vms,pgTable,inTables = 
    cell,vmtable,pgTable,spikegen,intable = createNetwork(synWeights,inputData)
    moose.setClock(0, 1e-4)
    moose.useClock(0, '/hopfield/##,/data/##','process')
    moose.reinit()
    moose.start(0.2)
    ii = 0
    for vm in vmtable:
        plot(np.linspace(0, 0.2, len(vm.vector)), vm.vector + ii*1.5e-7, label=vm.name)
        ii += 1
    show()
# #plot(pgTable.vector[1:])
# #for yset,inTable in enumerate(inTables):
# #    plot(float(yset)+inTable.vector[1:])
# for ySet,Vm in enumerate(Vms):
#     plot(float(2*ySet)/(1e+7)+Vm.vector[1:])
# #plot(Vms[0].vector[1:])
# show()
