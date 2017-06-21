""" passive_soma.py: 

In this script, we simulate a single compartment soma in MOOSE.

This soma does not have any ion-channels, only passive properties. It should
behave like a RC circuit. A current is injected into soma.

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2015, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

   
import moose
import pylab


model = None
soma = None 
vmtab = None

def buildModel():
    global model 
    global soma
    model = moose.Neutral('/model')
    soma = moose.Compartment('/model/soma')
    soma.Em = -60e-3
    soma.Rm = 1e10
    soma.Cm = 1e-10
    return model

def stimulus():
    global soma
    global vmtab
    pulse = moose.PulseGen('/model/pulse')
    pulse.delay[0] = 50e-3
    pulse.width[0] = 100e-3
    pulse.level[0] = 1e-9
    pulse.delay[1] = 1e9
    vmtab = moose.Table('/soma_Vm')
    moose.connect(pulse, 'output', soma, 'injectMsg')
    moose.connect(vmtab, 'requestOut', soma , 'getVm')

def main():
    global vmtab
    buildModel()
    stimulus()
    moose.reinit()
    t = 500e-2
    moose.start(t)
    time_vector = pylab.linspace(0, t, len(vmtab.vector))
    pylab.plot(time_vector, vmtab.vector)
    pylab.show( )
    # pylab.savefig('soma_passive.png')

if __name__ == '__main__':
    main()
