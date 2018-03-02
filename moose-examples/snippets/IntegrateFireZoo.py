
# -*- coding: utf-8 -*-
#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

#### Author: Aditya Gilra, NCBS, Bangalore, October, 2014.

"""
Simulate current injection into various Integrate and Fire neurons.

All integrate and fire (IF) neurons are subclasses of compartment,
so they have all the fields of a passive compartment. Multicompartmental neurons can be created.
Even ion channels and synaptic channels can be added to them, say for sub-threshold behaviour.

The key addition is that they have a reset mechanism when the membrane potential Vm crosses a threshold.
On each reset, a spikeOut message is generated, and the membrane potential is reset to Vreset.
The threshold may be the spike generation threshold as for LIF and AdThreshIF,
or it may be the peak of the spike as for QIF, ExIF, AdExIF, and IzhIF.
The adaptive IFs have an extra adapting variable apart from membrane potential Vm.

Details of the IFs are given below.
The fields of the MOOSE objects are named exactly as the parameters in the equations below.

 LIF:      Leaky Integrate and Fire:
           Rm*Cm * dVm/dt = -(Vm-Em) + Rm*I

 QIF:      Quadratic LIF:
           Rm*Cm * dVm/dt = a0*(Vm-Em)*(Vm-vCritical) + Rm*I

 ExIF:     Exponential leaky integrate and fire:
           Rm*Cm * dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I

 AdExIF:   Adaptive Exponential LIF:
           Rm*Cm * dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I - w,

           tau_w * dw/dt = a0*(Vm-Em) - w,

           At each spike, w -> w + b0 "

 AdThreshIF: Adaptive threshold LIF:
           Rm*Cm * dVm/dt = -(Vm-Em) + Rm*I,

           tauThresh * d threshAdaptive / dt = a0*(Vm-Em) - threshAdaptive,

           At each spike, threshAdaptive is increased by threshJump
           the spiking threshold adapts as thresh + threshAdaptive

 IzhIF:    Izhikevich:
           d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm,

           d u / dt = a * ( b * Vm - u ),

           At each spike, u -> u + d,

           By default, a0 = 0.04e6/V/s, b0 = 5e3/s, c0 = 140 V/s are set to SI units,
           so use SI consistently, or change a0, b0, c0 also if you wish to use other units.
           Rm from Compartment is not used here, vReset is same as c in the usual formalism.
           At rest, u0 = b V0, and V0 = ( -(-b0-b) +/- sqrt((b0-b)^2 - 4*a0*c0)) / (2*a0).

"""

import moose
import matplotlib.pyplot as plt
import numpy as np

# ###########################################
# Neuron model parameters (definitions above)
# ###########################################

# Leaky Integrate and Fire neuron (common to all IFs)
Vrest = -65e-3 # V      # resting potential
Vt_base = -45e-3 # V    # threshold
Vreset = -55e-3 # V     # Vreset need not be same as Vrest
R = 1e8 # Ohm
tau = 10e-3 # s
refrT = 1e-3 # s

# for QIF
vCritical = -54e-3 # V  # critical voltage above
                        # which Vm rises fast quadratically
a0 = 1e3 # V^-1         # parameter in equation

# for ExIF
deltaThresh = 10e-3 # V
vPeak = 30e-3 # V       # for ExpIF & IzhIF, reset is from vPeak, not thresh.
                        # I also use vPeak for adding spikes post-simulation
                        # to LIF, QIF, etc.
# for AdExIF
a0AdEx = 0.0 # unitless # voltage-dependent adaptation factor
b0 = 5e-10 # Amp        # current step added to the adaptation current at each spike
tauW = 20e-3 # s        # decay time constant of the adaptation current

# for AdThreshIF
a0AdTh = 0.0 # unitless # voltage-dependent adaptation factor
threshJump = 5e-3 # V   # voltage step added to the threshold at each spike
tauThresh = 20e-3 # s   # decay time constant of the adaptative threshold

# for IzhIF
a = 50.0 # s^-1         # d u / dt = a * ( b * Vm - u )
b = 246.15 # s^-1       # to obtain Em = -65mV at rest, set b = (a0*Em^2 + b0*Em + c0)/Em
                        # b = (4×10⁴×(−65×10⁻³)^2 + 5×10³×(−65×10⁻³) + 140) ÷ (−65×10⁻³)
uRest = -16 # V/s       # uRest = b*Em = -16 V/s
d = 10 # V/s            # d is added to u at every spike

def main():
    """
    On the command-line, in moose-examples/snippets directory, run ``python IntegrateFireZoo.py``.
    The script will ask you which neuron you want to simulate and you can choose and run what you want.
    Play with the parameters of the IF neurons in the source code.
    """
    neuronChoices = {'LIF':moose.LIF, 'QIF':moose.QIF, 'ExIF':moose.ExIF, 'AdExIF':moose.AdExIF,
                    'AdThreshIF':moose.AdThreshIF, 'IzhIF':moose.IzhIF}
    #### CHOOSE ONE OF THE NEURON KEYS AS choiceKey FROM BELOW DICTIONARY ####
    #choiceKey = 'LIF'
    #### No need, am inputting it from the user on the terminal
    choiceKeys = list(neuronChoices.keys()) # keys() does not retain the order in dict defn above!

    try:
        v = raw_input('Choose a number corresponding to your desired neuron: '+str([(i,key) for (i,key) in enumerate(choiceKeys)])+' -- ')
    except NameError as e:
        v = input('Choose a number corresponding to your desired neuron: '+str([(i,key) for (i,key) in enumerate(choiceKeys)])+' -- ')

    choiceIndex = eval(v)
    choiceKey = choiceKeys[choiceIndex]
    neuronChoice = neuronChoices[choiceKey]

    # ###########################################
    # Initialize neuron group
    # ###########################################

    # neuron instantiation
    network = neuronChoice( 'network' ); # choose neuron type above
    moose.le( '/network' )
    network.vec.Em = Vrest
    network.vec.thresh = Vt_base
    network.vec.refractoryPeriod = refrT
    network.vec.Rm = R
    network.vec.vReset = Vreset
    network.vec.Cm = tau/R
    network.vec.initVm = Vrest

    # neuron specific parameters and current injected I
    if choiceKey == 'LIF':
        network.vec.inject = 5e-10 # Amp    # injected current I
    if choiceKey == 'QIF':
        network.vec.a0 = a0
        network.vec.vCritical = vCritical
        network.vec.inject = 5e-10 # Amp    # injected current I
    elif choiceKey == 'ExIF':
        network.vec.deltaThresh = deltaThresh
        network.vec.vPeak = vPeak           # reset at vPeak, not at thresh
        network.vec.inject = 5e-9  # Amp    # injected current I
    elif choiceKey == 'AdExIF':
        network.vec.deltaThresh = deltaThresh
        network.vec.vPeak = vPeak           # reset at vPeak, not at thresh
        network.vec.a0 = a0AdEx
        network.vec.b0 = b0
        network.vec.tauW = tauW
        network.vec.inject = 5e-9  # Amp    # injected current I
    elif choiceKey == 'AdThreshIF':
        network.vec.a0 = a0AdTh
        network.vec.threshJump = threshJump
        network.vec.tauThresh = tauThresh
        network.vec.inject = 1e-9  # Amp    # injected current I
    elif choiceKey == 'IzhIF':
        network.vec.a = a
        network.vec.b = b
        network.vec.d = d
        network.vec.uInit = uRest           # Just sets the initial value of u
        network.vec.vPeak = vPeak           # reset at vPeak, not at thresh
        network.vec.inject = 5e-9  # Amp    # injected current I

    print(("Injecting current =",network.vec[0].inject,"in",choiceKey,"neuron."))

    # ###########################################
    # Setting up table
    # ###########################################

    Vm = moose.Table( '/plotVm' )
    moose.connect( network, 'VmOut', Vm, 'input', 'OneToOne')
    spikes = moose.Table( '/plotSpikes' )
    moose.connect( network, 'spikeOut', spikes, 'input', 'OneToOne')

    # ###########################################
    # Simulate the current injection
    # ###########################################

    dt = 5e-6 # s
    runtime = 0.02 # s

    # moose simulation
    moose.useClock( 1, '/network', 'process' )
    moose.useClock( 2, '/plotSpikes', 'process' )
    moose.useClock( 3, '/plotVm', 'process' )
    moose.setClock( 0, dt )
    moose.setClock( 1, dt )
    moose.setClock( 2, dt )
    moose.setClock( 3, dt )
    moose.setClock( 9, dt )
    moose.reinit()
    moose.start(runtime)

    # ###########################################
    # Plot the simulated Vm-s and STDP curve
    # ###########################################

    # Voltage plots
    Vmseries = Vm.vector
    numsteps = len(Vmseries)
    if choiceKey not in ['ExIF','AdExIF','IzhIF']:
        # insert spikes so that Vm reset at thresh doesn't look weird
        # for ExIF, etc. reset is at vPeak, so not weird.
        for t in spikes.vector:
            Vmseries[int(t/dt)-1] = 30e-3 # V
    timeseries = np.arange(0.,1000*numsteps*dt-1e-10,dt*1000)
    plt.figure(facecolor='w')
    plt.plot(timeseries,Vmseries*1000,color='r') # neuron's Vm
    plt.xlabel('time (ms)')
    plt.ylabel('Vm (mV)')
    plt.title(choiceKey+"neuron, current="+str(network.vec[0].inject)+"A")

    plt.show()

# Run below if script is executed standalone.
if __name__ == '__main__':
    """The main function to simulate an integrate and fire neuron."""
    main()

# end of IntegrateFireZoo.py
