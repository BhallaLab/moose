## Aditya Gilra, NCBS, Bangalore, 2012

"""
Inside the .../moose-examples/GranuleCell/ directory supplied with MOOSE, run
python testNeuroML_Gran98.py
(other channels and morph xml files are already present in this same directory).
The soma name below is hard coded for gran98, else any other file can be used by modifying this script.
"""
import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python')
import moose
from moose.utils import *

from moose.neuroml.NeuroML import NeuroML

from pylab import *

simdt = 1e-6 # s
plotdt = 10e-6 # s
runtime = 0.7 # s

def loadGran98NeuroML_L123(filename):
    neuromlR = NeuroML()
    populationDict, projectionDict = \
        neuromlR.readNeuroMLFromFile(filename)
    soma_path = populationDict['Gran'][1][0].path+'/Soma_0'
    somaVm = setupTable('somaVm',moose.Compartment(soma_path),'Vm')
    somaCa = setupTable('somaCa',moose.CaConc(soma_path+'/Gran_CaPool_98'),'Ca')
    somaIKCa = setupTable('somaIKCa',moose.element(soma_path+'/Gran_KCa_98'),'Gk')
    #KDrX = setupTable('ChanX',moose.element(soma_path+'/Gran_KDr_98'),'X')
    soma = moose.Compartment(soma_path)
    print("Reinit MOOSE ... ")
    resetSim(['/elec','/cells'],simdt,plotdt,simmethod='ee') # from moose.utils
    print("Running ... ")
    moose.start(runtime)
    tvec = arange(0.0,runtime,plotdt)
    plot(tvec,somaVm.vector[1:])
    title('Soma Vm')
    xlabel('time (s)')
    ylabel('Voltage (V)')
    figure()
    plot(tvec,somaCa.vector[1:])
    title('Soma Ca')
    xlabel('time (s)')
    ylabel('Ca conc (mol/m^3)')
    figure()
    plot(tvec,somaIKCa.vector[1:])
    title('KCa current (A)')
    xlabel('time (s)')
    ylabel('')
    print("Showing plots ...")
    show()

filename = "GranuleCell.net.xml"
if __name__ == "__main__":
    if len(sys.argv)<2:
        filename = "GranuleCell.net.xml"
    else:
        filename = sys.argv[1]
    loadGran98NeuroML_L123(filename)
