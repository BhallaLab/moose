## Aditya Gilra, NCBS, Bangalore, 2013

"""
Stomatogastric ganglion Central Pattern Generator:
 generates pyloric rhythm of the lobster

Network model translated from:
Prinz, Bucher, Marder, Nature Neuroscience, 2004;
STG neuron models translated from:
Prinz, Billimoria, Marder, J.Neurophys., 2003.

Translated into MOOSE by Aditya Gilra, Bangalore, 2013, revised 2014.
"""

#import os
#os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.extend(['../../../python','synapses'])

import moose
from moose.utils import *
from moose.neuroml.NeuroML import NeuroML

import matplotlib.pyplot as plt
import numpy as np

simdt = 25e-6 # s
plotdt = 25e-6 # s
runtime = 10.0 # s
cells_path = '/cells' # neuromlR.readNeuroMLFromFile creates cells in '/cells'

def loadRunSTGNeuroML_L123(filename):
    'Loads and runs the pyloric rhythm generator from NeuroML files.'
    # for graded synapses, else NeuroML event-based are used
    from load_synapses import load_synapses
    moose.Neutral('/library')
    # set graded to False to use event based synapses
    #  if False, neuroml event-based synapses get searched for and loaded
    # True to load graded synapses
    graded_syn = True
    #graded_syn = False
    if graded_syn:
        load_synapses()

    neuromlR = NeuroML()
    ## readNeuroMLFromFile below returns:
    # This returns
    # populationDict = {
    #     'populationname1':('cellName',{('instanceid1'):moosecell, ... }) 
    #     , ... 
    #     }
    # (cellName and instanceid are strings, mooosecell is a moose.Neuron object instance)
    # and
    # projectionDict = { 
    #     'projName1':('source','target',[('syn_name1','pre_seg_path','post_seg_path')
    #     ,...]) 
    #     , ... 
    #     }
    populationDict, projectionDict = \
        neuromlR.readNeuroMLFromFile(filename)
    soma1_path = populationDict['AB_PD'][1][0].path+'/Soma_0'
    soma1Vm = setupTable('somaVm',moose.Compartment(soma1_path),'Vm')
    soma2_path = populationDict['LP'][1][0].path+'/Soma_0'
    soma2Vm = setupTable('somaVm',moose.Compartment(soma2_path),'Vm')
    soma3_path = populationDict['PY'][1][0].path+'/Soma_0'
    soma3Vm = setupTable('somaVm',moose.Compartment(soma3_path),'Vm')

    # monitor channel current
    channel_path = soma1_path + '/KCa_STG'
    channel_Ik = setupTable('KCa_Ik',moose.element(channel_path),'Ik')
    # monitor Ca
    capool_path = soma1_path + '/CaPool_STG'
    capool_Ca = setupTable('CaPool_Ca',moose.element(capool_path),'Ca')

    # monitor synaptic current
    soma2 = moose.element(soma2_path)
    print(("Children of",soma2_path,"are:"))
    for child in soma2.children:
        print((child.className, child.path))
    if graded_syn:
        syn_path = soma2_path+'/DoubExpSyn_Ach__cells-0-_AB_PD_0-0-_Soma_0'
        syn = moose.element(syn_path)
    else:
        syn_path = soma2_path+'/DoubExpSyn_Ach'
        syn = moose.element(syn_path)
    syn_Ik = setupTable('DoubExpSyn_Ach_Ik',syn,'Ik')

    print("Reinit MOOSE ... ")
    resetSim(['/elec',cells_path], simdt, plotdt, simmethod='hsolve')

    print(("Using graded synapses? = ", graded_syn))
    print(("Running model filename = ",filename," ... "))
    moose.start(runtime)
    tvec = np.arange(0.0,runtime+2*plotdt,plotdt)
    tvec = tvec[ : soma1Vm.vector.size ]
    
    fig = plt.figure(facecolor='w',figsize=(10,6))
    axA = plt.subplot2grid((3,2),(0,0),rowspan=3,colspan=1,frameon=False)
    img = plt.imread( 'STG.png' )
    imgplot = axA.imshow( img )
    for tick in axA.get_xticklines():
        tick.set_visible(False)
    for tick in axA.get_yticklines():
        tick.set_visible(False)
    axA.set_xticklabels([])
    axA.set_yticklabels([])
    ax = plt.subplot2grid((3,2),(0,1),rowspan=1,colspan=1)
    ax.plot(tvec,soma1Vm.vector*1000,label='AB_PD',color='g',linestyle='solid')
    ax.set_xticklabels([])
    ax.set_ylabel('AB_PD (mV)')
    ax = plt.subplot2grid((3,2),(1,1),rowspan=1,colspan=1)
    ax.plot(tvec,soma2Vm.vector*1000,label='LP',color='r',linestyle='solid')
    ax.set_xticklabels([])
    ax.set_ylabel('LP (mV)')
    ax = plt.subplot2grid((3,2),(2,1),rowspan=1,colspan=1)
    ax.plot(tvec,soma3Vm.vector*1000,label='PY',color='b',linestyle='solid')
    ax.set_ylabel('PY (mV)')
    ax.set_xlabel('time (s)')
    fig.tight_layout()

    fig = plt.figure(facecolor='w')
    plt.plot(tvec,soma2Vm.vector*1000,label='LP',color='r',linestyle='solid')
    plt.plot(tvec,soma3Vm.vector*1000,label='PY',color='b',linestyle='solid')
    plt.legend()
    plt.xlabel('time (s)')
    plt.ylabel('Soma Vm (mV)')

    plt.figure(facecolor='w')
    plt.plot(tvec,channel_Ik.vector,color='b',linestyle='solid')
    plt.title('KCa current; Ca conc')
    plt.xlabel('time (s)')
    plt.ylabel('Ik (Amp)')
    plt.twinx()
    plt.plot(tvec,capool_Ca.vector,color='r',linestyle='solid')
    plt.ylabel('Ca (mol/m^3)')

    plt.figure(facecolor='w')
    plt.plot(tvec,syn_Ik.vector,color='b',linestyle='solid')    
    plt.title('Ach syn current in '+soma2_path)
    plt.xlabel('time (s)')
    plt.ylabel('Isyn (S)')
    print("Showing plots ...")
    
    plt.show()

filename = "Generated.net.xml"
if __name__ == "__main__":
    '''
    Inside the moose-examples/neuroml/lobster_ploric/ directory supplied with MOOSE, run
    ``python STG_net.py``
    (other channels and morph xml files are already present in this same directory).
    read the pdf documentation for a tutorial by Aditya Gilra.
    '''
    if len(sys.argv)>=2:
        filename = sys.argv[1]
    loadRunSTGNeuroML_L123(filename)
