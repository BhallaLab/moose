# __BROKEN__
#
# A demo of a simple hopfield network using leaky integrate and fire neurons
# memory1.csv and memory2.csv have the memory being saved, the synaptic weights
# are set at start according to this memory memory must be a square matrix of
# 0's and 1's only input.csv has partial input given as input to the cells.  -
# By C.H.Chaitanya This code is available under GPL3 or greater GNU license 

import moose
import math
import csv
import numpy as np
import matplotlib.pyplot as plt

class HopfieldNetwork():
    def __init__(self,numberOfNeurons):
        self.cells = []
        #self.Vms   = []
        self.allSpikes = []
        #self.inTables = []
        self.inSpike = []
        self.numNeurons = numberOfNeurons
        self.synWeights = [0]*self.numNeurons*self.numNeurons
        self.numMemories = 0
        self.createNetwork()

    def mooseReinit(self):
        moose.reinit()
        
    def clearAllMemory(self):
        self.synWeights = [0]*self.numNeurons*self.numNeurons
        self.numMemories = 0
        self.clearAllSynapticWeights()
        
    def readMemoryFile(self, memoryFile):
        '''read a memory pattern to save from a file'''
        f = open(memoryFile,'r')
        testLine = f.readline()
        dialect = csv.Sniffer().sniff(testLine) #to get the format of the csv
        f.close()
        f = open(memoryFile, 'r')
        reader = csv.reader(f,dialect)
        memory = []
        for row in reader:
            for i in row[0:]:
                memory.append(int(i))
        f.close()

        return memory

    def updateWeights(self, memory):
        for i in range(self.numNeurons):
            for j in range(self.numNeurons):
                if i != j:
                    if memory[i] == 0:
                        memory[i] = -1
                    if memory[j] == 0:
                        memory[j] = -1
                    self.synWeights[i*len(memory)+j] += memory[i]*memory[j]

        print((self.synWeights[0:100]))
        self.numMemories += 1
        print((self.numMemories, '#number Of saved memories'))

    def createNetwork(self):
        '''setting up of the cells and their connections'''
        hopfield = moose.Neutral('/hopfield')
        pg = moose.PulseGen('/hopfield/inPulGen')

        pgTable = moose.Table('/hopfield/inPulGen/pgTable')
        moose.connect(pgTable, 'requestOut', pg, 'getOutputValue')

        pg.firstDelay = 10e-3
        pg.firstWidth = 2e-03
        pg.firstLevel = 3.0
        pg.secondDelay = 1.0
    
        for i in range(self.numNeurons):
            cellPath = '/hopfield/cell_'+str(i)
            cell = moose.IntFire(cellPath) 
            cell.setField('tau',10e-3)
            cell.setField('refractoryPeriod', 5e-3)
            cell.setField('thresh', 0.99)
            cell.synapse.num = self.numNeurons
            #definite firing everytime ip is given
            cell.synapse[i].weight = 1.00 #synapse i = input synapse
            cell.synapse[i].delay = 0.0 #1e-3 #instantaneous

            #VmVals = moose.Table(cellPath+'/Vm_cell_'+str(i))
            #moose.connect(VmVals, 'requestOut', cell, 'getVm')
            spikeVals = moose.Table(cellPath+'/spike_cell_'+str(i))
            moose.connect(cell, 'spike', spikeVals, 'input')

            inSpkGen = moose.SpikeGen(cellPath+'/inSpkGen')
            inSpkGen.setField('edgeTriggered', True)
            inSpkGen.setField('threshold', 2.0)
            moose.connect(pg, 'output', inSpkGen, 'Vm')
            #inTable = moose.Table(cellPath+'/inSpkGen/inTable')
            #moose.connect(inTable, 'requestOut', inSpkGen, 'getHasFired')
            moose.connect(inSpkGen, 'spikeOut', cell.synapse[i] ,'addSpike') #self connection is the input 
            self.inSpike.append(inSpkGen)
            #self.inTables.append(inTable)
            #self.Vms.append(VmVals)
            self.cells.append(cell)
            self.allSpikes.append(spikeVals)
        
        for ii in range(self.numNeurons):
            for jj in range(self.numNeurons):
                if ii == jj:
                    continue
                self.cells[jj].synapse[ii].weight = 0
                self.cells[jj].synapse[ii].delay = 20e-3
                moose.connect(self.cells[ii], 'spike', self.cells[jj].synapse[ii], 'addSpike')

    def updateInputs(self,inputGiven):
        '''assign inputs here'''
        for i in range(self.numNeurons):
            if inputGiven[i] == 1: 
                self.inSpike[i].setField('threshold',2.0)
            else:
                self.inSpike[i].setField('threshold',4.0)

    def clearAllSynapticWeights(self):
        for ii in range(self.numNeurons):
            for jj in range(self.numNeurons):
                self.cells[jj].synapse[ii].weight = 0.0

    def assignAllSynapticWeights(self):
        '''always call before runnning model'''
        for ii in range(self.numNeurons):
            for jj in range(self.numNeurons):
                if (ii <= jj):
                    continue
                self.cells[jj].synapse[ii].weight = float(self.synWeights[(jj*self.numNeurons)+ii]/self.numMemories)
                self.cells[ii].synapse[jj].weight = float(self.synWeights[(ii*self.numNeurons)+jj]/self.numMemories)


    def setClocksHopfield(self):
        moose.setClock(0, 1e-4)
        moose.useClock(0, '/hopfield/##', 'process')

    def runMooseHopfield(self,runtime):
        moose.start(runtime)

def test(runtime=0.2):
    hopF = HopfieldNetwork(100)

    pattern1 = hopF.readMemoryFile('memory1.csv')
    hopF.updateWeights(pattern1)
    pattern2 = hopF.readMemoryFile('memory2.csv')
    hopF.updateWeights(pattern2)

    #hopF.createNetwork()
    hopF.assignAllSynapticWeights()

    initialInputs = hopF.readMemoryFile('input.csv')
    hopF.updateInputs(initialInputs)
    hopF.setClocksHopfield()

    moose.reinit()
    moose.start(runtime)

    for ySet,Vm in enumerate(hopF.allSpikes):
        ySpike = ySet
        plt.subplot(211)    
        if pattern1[ySet] == 1:
            plt.scatter(Vm.vector[1:],ySpike*np.ones(len(Vm.vector[1:])),color='blue')
        else:
            plt.scatter(Vm.vector[1:],ySpike*np.ones(len(Vm.vector[1:])),color='red')
        plt.subplot(212)
        if pattern2[ySet] == 1:
            plt.scatter(Vm.vector[1:],ySpike*np.ones(len(Vm.vector[1:])),color='blue')
        else:
            plt.scatter(Vm.vector[1:],ySpike*np.ones(len(Vm.vector[1:])),color='red')
    
    plt.subplot(211)
    plt.xlim(0,0.25)
    plt.ylim(0,100)
    plt.xlabel('time')
    plt.ylabel('neuron#')
    plt.title('spikes:pattern1- red:misfire & blue:correct')
    plt.subplot(212)
    plt.xlim(0,0.25)
    plt.ylim(0,100)
    plt.xlabel('time')
    plt.ylabel('neuron#')
    plt.title('spikes:pattern2- red:misfire & blue:correct')

    plt.show()

if __name__ == '__main__':
    test()

