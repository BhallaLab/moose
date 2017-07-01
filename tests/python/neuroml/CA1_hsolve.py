# -*- coding: utf-8 -*-
## Aditya Gilra, NCBS, Bangalore, 2012

"""
Inside the .../Demos/CA1PyramidalCell/ directory supplied with MOOSE, run
python testNeuroML_CA1.py
(other channels and morph xml files are already present in this same directory).
The soma name below is hard coded for CA1, else any other file can be used by modifying this script.
"""
from __future__ import print_function

import moose
from moose.utils import *

from moose.neuroml.NeuroML import NeuroML

simdt = 10e-6 # s
plotdt = 10e-6 # s
runtime = 0.2 # s
cells_path = '/cells' # neuromlR.readNeuroMLFromFile creates cells in '/cells'

def loadGran98NeuroML_L123(filename,params):
    neuromlR = NeuroML()
    populationDict, projectionDict = \
        neuromlR.readNeuroMLFromFile(filename,params=params)
    print("Number of compartments =",
          len(moose.Neuron(populationDict['CA1group'][1][0].path).children))
    soma_path = populationDict['CA1group'][1][0].path+'/Seg0_soma_0_0'
    somaVm = setupTable('somaVm',moose.Compartment(soma_path),'Vm')
    #somaCa = setupTable('somaCa',moose.CaConc(soma_path+'/Gran_CaPool_98'),'Ca')
    #somaIKCa = setupTable('somaIKCa',moose.HHChannel(soma_path+'/Gran_KCa_98'),'Gk')
    #KDrX = setupTable('ChanX',moose.HHChannel(soma_path+'/Gran_KDr_98'),'X')
    soma = moose.Compartment(soma_path)

    print("Reinit MOOSE ... ")
    resetSim(['/elec','/cells'],simdt,plotdt,simmethod='hsolve') # from moose.utils
    print("Running ... ")
    moose.start(runtime)
    tvec = arange(0.0,runtime,simdt)

if __name__ == "__main__":
    if len(sys.argv)<2:
        filename = "CA1soma.net.xml"
        params = {}
    else:
        filename = sys.argv[1]
        params = {}
        if len(sys.argv)>2:
            params = {'combineSegments':bool(sys.argv[2])}
            # sys.argv[2] should be True or False
    loadGran98NeuroML_L123(filename,params)
