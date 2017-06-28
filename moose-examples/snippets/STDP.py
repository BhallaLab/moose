
#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

##### Author: Aditya Gilra, NCBS, Bangalore, October, 2014.  
##### Fixed numpy imports and global variables: Subhasis Ray, Fri Jul 10 19:34:53 IST 2015

import moose
import matplotlib.pyplot as plt
from numpy import arange, array

##### Author: Aditya Gilra, NCBS, Bangalore, October, 2014.
##### Fixed numpy imports and global variables: Subhasis Ray, Fri Jul 10 19:34:53 IST 2015

# ###########################################
# Neuron models
# ###########################################

## Leaky integrate and fire neuron
Vrest = -65e-3 # V      # resting potential
Vt_base = -45e-3 # V    # threshold
Vreset = -55e-3 # V     # in current steps, Vreset is same as pedestal
R = 1e8 # Ohm
tau = 10e-3 # s
refrT = 2e-3 # s
network = None
syn = None
Vms = None
weight = 0.0
spikes = None
dt = 1e-6

def setupModel():
    """
Set up two LIF neurons and connect them by an STDPSynHandler.
Set up some tables, and reinit MOOSE before simulation.

    """
    global network, syn, Vms, weight, spikes, dt
    # ###########################################
    # Initialize neuron group
    # ###########################################

    ## two neurons: index 0 will be presynaptic, 1 will be postsynaptic
    network = moose.LIF( 'network', 2 );
    moose.le( '/network' )
    network.vec.Em = Vrest
    network.vec.thresh = Vt_base
    network.vec.refractoryPeriod = refrT
    network.vec.Rm = R
    network.vec.vReset = Vreset
    network.vec.Cm = tau/R
    network.vec.inject = 0.
    network.vec.initVm = Vrest

    # ###########################################
    # Synaptic model: STDP at each pre and post spike
    # ###########################################

    # Values approx from figure in Scholarpedia article (following Bi and Poo 1998):
    # Jesper Sjoestroem and Wulfram Gerstner (2010) Spike-timing dependent plasticity.
    # Scholarpedia, 5(2):1362., revision #137369
    tauPlus = 10e-3 # s         # Apre time constant
    tauMinus = 10e-3 # s        # Apost time constant
    aPlus0 = 1.0                # at pre, Apre += Apre0
    aMinus0 = 0.25              # at post, Apost += Apost0
    weight = 5e-3 # V           # delta function synapse, adds to Vm

    syn = moose.STDPSynHandler( '/network/syn' )
    syn.numSynapses = 1                         # 1 synapse
                                                # many pre-synaptic inputs can connect to a synapse
    # synapse onto postsynaptic neuron
    moose.connect( syn, 'activationOut', network.vec[1], 'activation' )

    # synapse from presynaptic neuron
    moose.connect( network.vec[0],'spikeOut', syn.synapse[0], 'addSpike')

    # post-synaptic spikes also needed for STDP
    moose.connect( network.vec[1], 'spikeOut', syn, 'addPostSpike')

    syn.synapse[0].delay = 0.0
    syn.synapse[0].weight = weight # V
    syn.aPlus0 = aPlus0*weight      # on every pre-spike, aPlus gets this jump
                                    # aMinus0 includes learning rate
                                    # on every pre-spike, aMinus is added to weight
    syn.tauPlus = tauPlus
    syn.aMinus0 = -aMinus0*weight   # on every post-spike, aMinus gets this jump
                                    # aMinus0 includes learning rate
                                    # on every post-spike, aPlus is added to weight
    syn.tauMinus = tauMinus
    syn.weightMax = 2*weight        # bounds on the weight
    syn.weightMin = 0.

    # ###########################################
    # Setting up tables
    # ###########################################

    Vms = moose.Table( '/plotVms', 2 )
    moose.connect( network, 'VmOut', Vms, 'input', 'OneToOne')
    spikes = moose.Table( '/plotSpikes', 2 )
    moose.connect( network, 'spikeOut', spikes, 'input', 'OneToOne')

    # ###########################################
    # Simulate the STDP curve with spaced pre-post spike pairs
    # ###########################################

    dt = 0.5e-5 # s
    # moose simulation
    moose.useClock( 0, '/network/syn', 'process' )
    moose.useClock( 1, '/network', 'process' )
    moose.useClock( 2, '/plotSpikes', 'process' )
    moose.useClock( 3, '/plotVms', 'process' )
    moose.setClock( 0, dt )
    moose.setClock( 1, dt )
    moose.setClock( 2, dt )
    moose.setClock( 3, dt )
    moose.setClock( 9, dt )
    moose.reinit()


# function to make the aPlus and aMinus settle to equilibrium values
def reset_settle():
    """
Call this between every pre-post pair
to reset the neurons and make them settle to rest.

    """
    settletime = 100e-3 # s
    syn.synapse[0].weight = weight # V
    moose.start(settletime)

# function to inject a sharp current pulse to make neuron spike
# immediately at a given time step
def make_neuron_spike(nrnidx,I=1e-7,duration=1e-3):
    """
Inject a brief current pulse to
make a neuron spike

    """
    network.vec[nrnidx].inject = I
    moose.start(duration)
    network.vec[nrnidx].inject = 0.

def main():
    """
    Connect two cells via a plastic synapse (STDPSynHandler).  
    Induce spikes spearated by varying intervals, in the pre and post synaptic cells.  
    Plot the synaptic weight change for different intervals between the spike-pairs.  
    This ia a pseudo-STDP protocol and we get the STDP rule.  

    On the command-line, in moose-examples/snippets directory, run ``python STDP.py``
    """
    setupModel()
    dwlist_neg = []
    ddt = 2e-3 # s
    t_extent = 20e-3 # s
    # dt = tpost - tpre
    # negative dt corresponds to post before pre
    print('-----------------------------------------------')
    for deltat in arange(t_extent,0.0,-ddt):
        reset_settle()
        # post neuron spike
        make_neuron_spike(1)
        moose.start(deltat)
        # pre neuron spike after deltat
        make_neuron_spike(0)
        moose.start(1e-3)
        dw = ( syn.synapse[0].weight - weight ) / weight
        print(('post before pre, dt = %1.3f s, dw/w = %1.3f'%(-deltat,dw)))
        dwlist_neg.append(dw)
    print('-----------------------------------------------')
    # positive dt corresponds to pre before post
    dwlist_pos = []
    for deltat in arange(ddt,t_extent+ddt,ddt):
        reset_settle()
        # pre neuron spike
        make_neuron_spike(0)
        moose.start(deltat)
        # post neuron spike after deltat
        make_neuron_spike(1)
        moose.start(1e-3)
        dw = ( syn.synapse[0].weight - weight ) / weight
        print(('pre before post, dt = %1.3f s, dw/w = %1.3f'%(deltat,dw)))
        dwlist_pos.append(dw)
    print('-----------------------------------------------')

    # ###########################################
    # Plot the simulated Vm-s and STDP curve
    # ###########################################

    # insert spikes so that Vm reset doesn't look weird
    Vmseries0 = list(Vms.vec[0].vector)
    numsteps = len(Vmseries0)
    for t in spikes.vec[0].vector:
        Vmseries0[int(t/dt)-1] = 30e-3 # V
    Vmseries1 = list(Vms.vec[1].vector)
    for t in spikes.vec[1].vector:
        Vmseries1[int(t/dt)-1] = 30e-3 # V

    # Voltage plots
    plt.figure(facecolor='w')
    timeseries = arange(0.,1000*numsteps*dt,dt*1000)
    plt.plot(timeseries,Vmseries0,color='r') # pre neuron's vm
    plt.plot(timeseries,Vmseries1,color='b') # post neuron's vm
    plt.xlabel('time (ms)')
    plt.ylabel('Vm (V)')
    plt.title("pre (r) and post (b) neurons' Vm")

    # STDP curve
    fig = plt.figure(facecolor='w')
    ax = fig.add_subplot(111)
    ax.plot(arange(-t_extent,0,ddt)*1000,array(dwlist_neg),'.-r')
    ax.plot(arange(ddt,(t_extent+ddt),ddt)*1000,array(dwlist_pos),'.-b')
    xmin,xmax = ax.get_xlim()
    ymin,ymax = ax.get_ylim()
    ax.set_xticks([xmin,0,xmax])
    ax.set_yticks([ymin,0,ymax])
    ax.plot((0,0),(ymin,ymax),linestyle='dashed',color='k')
    ax.plot((xmin,xmax),(0,0),linestyle='dashed',color='k')
    ax.set_xlabel('$t_{post}-t_{pre}$ (ms)')
    ax.set_ylabel('$\Delta w / w$')
    fig.tight_layout()
    #fig.subplots_adjust(hspace=0.3,wspace=0.5) # has to be after tight_layout()

    plt.show()

# Run below if script is executed standalone.
if __name__ == '__main__':
    main()
