# -*- coding: utf-8 -*-
## all SI units
########################################################################################
## Plot the membrane potential for a leaky integrate and fire neuron with current injection
## Author: Aditya Gilra
## Creation Date: 2012-06-08
## Modification Date: 2012-06-08
########################################################################################

#import os
#os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python')

## simulation parameters
SIMDT = 5e-5 # seconds
PLOTDT = 5e-5 # seconds
RUNTIME = 2.0 # seconds
injectI = 1e-8#2.5e-12 # Amperes

## moose imports
import moose
from moose.neuroml import *
from moose.utils import * # has setupTable(), resetSim() etc
import math

## import numpy and matplotlib in matlab style commands
from pylab import *

def create_twoLIFs():
    NML = NetworkML({'temperature':37.0,'model_dir':'.'})
    ## Below returns populationDict = { 'populationname1':(cellname,{instanceid1:moosecell, ... }) , ... }
    ## and projectionDict = { 'projectionname1':(source,target,[(syn_name1,pre_seg_path,post_seg_path),...]) , ... }
    (populationDict,projectionDict) = \
        NML.readNetworkMLFromFile('twoLIFs.net.xml', {}, params={})
    return populationDict,projectionDict

def run_twoLIFs():
	## reset and run the simulation
	print("Reinit MOOSE.")
	## from moose_utils.py sets clocks and resets
	resetSim(['/cells[0]'], SIMDT, PLOTDT, simmethod='ee')
	print("Running now...")
	moose.start(RUNTIME)

if __name__ == '__main__':
    populationDict,projectionDict = create_twoLIFs()
    ## element returns the right element and error if not present
    IF1Soma = moose.element(populationDict['LIFs'][1][0].path+'/soma_0')
    IF1Soma.inject = injectI
    IF2Soma = moose.element(populationDict['LIFs'][1][1].path+'/soma_0')
    IF2Soma.inject = 0.0#injectI*2.0
    #IF2Soma.inject = injectI
    IF1vmTable = setupTable("vmTableIF1",IF1Soma,'Vm')
    IF2vmTable = setupTable("vmTableIF2",IF2Soma,'Vm')

    table_path = moose.element(IF1Soma.path+'/data').path
    IF1spikesTable = moose.Table(table_path+'/spikesTable')
    moose.connect(IF1Soma,'spikeOut',IF1spikesTable,'input') ## spikeGen gives spiketimes

    ## record Gk of the synapse on IF2
    #print IF2Soma.children
    IF2SynChanTable = moose.Table(table_path+'/synChanTable')
    moose.connect(IF2SynChanTable,'requestOut',IF2Soma.path+'/exc_syn','getIk')

    run_twoLIFs()
    print(("Spiketimes :",IF1spikesTable.vector))
    ## plot the membrane potential of the neuron
    timevec = arange(0.0,RUNTIME+PLOTDT/2.0,PLOTDT)
    figure(facecolor='w')
    plot(timevec, IF1vmTable.vector*1000,'r-')
    xlabel('time(s)')
    ylabel('Vm (mV)')
    title('Vm of presynaptic IntFire')
    figure(facecolor='w')
    plot(timevec, IF2vmTable.vector*1000,'b-')
    xlabel('time(s)')
    ylabel('Vm (mV)')
    title('Vm of postsynaptic IntFire')
    figure(facecolor='w')
    plot(timevec, IF2SynChanTable.vector*1e12,'b-')
    xlabel('time(s)')
    ylabel('Ik (pA)')
    title('Ik entering postsynaptic IntFire')
    show()
