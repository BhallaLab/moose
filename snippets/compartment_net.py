# compartment_net.py --- 
# 
# Filename: compartment_net.py
# Description: 
# Author:Subhasis Ray 
# Maintainer: 
# Created: Sat Aug 11 14:30:21 2012 (+0530)
# Version: 
# Last-Updated: Sun Aug 12 15:45:38 2012 (+0530)
#           By: subha
#     Update #: 521
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# A demo to create a network of single compartmental neurons connected
# via alpha synapses.
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:
"""This script demonstrates the use of SynChan class to create a
network of single-compartment neurons connected by synapse."""
import sys
sys.path.append('../../python')
import os
from pylab import *
import numpy as np
import matplotlib.pyplot as plt

import moose
from ionchannel import create_1comp_neuron
    
def create_population(container, size):
    """Create a population of `size` single compartmental neurons with Na+
    and K+ channels. Also create SpikeGen objects and SynChan objects
    connected to these which can act as plug points for setting up
    synapses later.

    This uses ..ref::`ionchannel.create_1comp_neuron`.

    """
    path = container.path
    print path, size, type(path)
    comps = create_1comp_neuron('{}/neuron'.format(path), number=size)
    synpath = path+'/synchan'
    print synpath, size, type(size)
    synchan = moose.vec(synpath, n=size, dtype='SynChan')
    synchan.Gbar = 1e-8
    synchan.tau1 = 2e-3
    synchan.tau2 = 2e-3
    m = moose.connect(comps, 'channel', synchan, 'channel', 'OneToOne')
    synhandler = moose.vec('{}/synhandler'.format(path), n=size,
                           dtype='SimpleSynHandler')
    moose.connect(synhandler, 'activationOut', synchan, 'activation', 'OneToOne')
    spikegen = moose.vec('{}/spikegen'.format(path), n=size, dtype='SpikeGen')
    spikegen.threshold = 0.0
    m = moose.connect(comps, 'VmOut', spikegen, 'Vm', 'OneToOne')
    return {'compartment': comps, 'spikegen': spikegen, 'synchan':
            synchan, 'synhandler': synhandler}

def make_synapses(spikegen, synhandler, connprob=1.0, delay=5e-3):
    """Create synapses from spikegen array to synchan array.

    Parameters
    ----------
    spikegen : vec of SpikGen elements
        Spike generators from neurons.

    synhandler : vec of SynHandler elements
        Handles presynaptic spike event inputs to synchans.
    
    connprob: float in range (0, 1]
        connection probability between any two neurons

    delay: float
        mean delay of synaptic transmission. Individual delays are
        normally distributed with sd=0.1*mean.  
    """
    for sh in synhandler:
        scount = len(spikegen)
        sh.synapse.num = scount
        sh.synapse.vec.delay = 5e-3
        for ii, syn in enumerate(sh.synapse):
            msg = moose.connect(spikegen[ii], 'spikeOut', syn, 'addSpike')
            print 'Connected', spikegen[ii].path, 'to', syn.path, 'on', sh.path
            
def create_network(size=2):
    """Create a network containing two neuronal populations, pop_A and
    pop_B and connect them up"""
    net = moose.Neutral('network')
    pop_a = create_population(moose.Neutral('/network/pop_A'), size)
    print pop_a
    pop_b = create_population(moose.Neutral('/network/pop_B'), size)
    make_synapses(pop_a['spikegen'], pop_b['synhandler'])
    pulse = moose.PulseGen('pulse')
    pulse.level[0] = 1e-9
    pulse.delay[0] = 0.02 # disable the pulsegen
    pulse.width[0] = 40e-3
    pulse.delay[1] = 1e9
    data = moose.Neutral('/data')
    vm_a = moose.Table('/data/Vm_A', n=size)
    moose.connect(pulse, 'output', pop_a['compartment'], 'injectMsg', 'OneToAll')
    moose.connect(vm_a, 'requestOut', pop_a['compartment'], 'getVm', 'OneToOne')
    vm_b = moose.Table('/data/Vm_B', size)
    moose.connect(vm_b, 'requestOut', pop_b['compartment'], 'getVm', 'OneToOne')
    gksyn_b = moose.Table('/data/Gk_syn_b', n=size)
    moose.connect(gksyn_b, 'requestOut', pop_b['synchan'], 'getGk', 'OneToOne')
    pulsetable = moose.Table('/data/pulse')
    pulsetable.connect('requestOut', pulse, 'getOutputValue')
    return {'A': pop_a,
            'B': pop_b,
            'Vm_A': vm_a,
            'Vm_B': vm_b,
            'Gsyn_B': gksyn_b
        }

if __name__ == '__main__':
    simtime = 0.1
    simdt = 0.25e-5
    plotdt = 0.25e-3
    netinfo = create_network(size=2)
    vm_a = netinfo['Vm_A']
    vm_b = netinfo['Vm_B']
    gksyn_b = netinfo['Gsyn_B']
    for ii in range(10):
        moose.setClock(ii, simdt)
    moose.setClock(18, plotdt)
    moose.reinit()
    moose.start(simtime)
    plt.subplot(221)
    for oid in vm_a.vec:
        print oid, oid.vector.shape
        plt.plot(oid.vector, label=oid.path)
    plt.legend()
    plt.subplot(223)
    for oid in vm_b.vec:
        plt.plot(oid.vector, label=oid.path)
    plt.legend()
    plt.subplot(224)
    for oid in gksyn_b.vec:
        plt.plot(oid.vector, label=oid.path)
    plt.legend()
    plt.show()
    

# 
# compartment_net.py ends here
