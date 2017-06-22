# twocells.py ---
#
# Filename: twocells.py
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

import sys
sys.path.append('../../python')
import os
import numpy as np
import matplotlib.pyplot as plt

import moose
from ionchannel import create_1comp_neuron

def create_model():
    """
Create two single compartmental neurons, neuron_A is the
presynaptic neuron and neuron_B is the postsynaptic neuron.

    1. The presynaptic cell's Vm is monitored by a SpikeGen
    object. Whenever the Vm crosses the threshold of the spikegen, it
    sends out a spike event message.

    2. This is event message is received by a SynHandler, which
    passes the event as activation parameter to a SynChan object.

    3. The SynChan, which is connected to the postsynaptic neuron
    as a channel, updates its conductance based on the activation
    parameter.

    4. The change in conductance due to a spike may evoke an
    action potential in the post synaptic neuron.

    """
    model = moose.Neutral('/model')
    nrn_a = create_1comp_neuron('/model/neuron_A')[0]
    nrn_b = create_1comp_neuron('/model/neuron_B')[0]
    #: SynChan for post synaptic neuron
    synchan = moose.SynChan('/model/neuron_B/synchan')
    synchan.Gbar = 1e-8
    synchan.tau1 = 2e-3
    synchan.tau2 = 2e-3
    msg = moose.connect(nrn_b, 'channel', synchan, 'channel')
    #: Create SynHandler to handle spike event input and set the
    #: activation input of synchan
    synhandler = moose.SimpleSynHandler('/model/neuron_B/synhandler')
    synhandler.synapse.num = 1
    synhandler.synapse[0].delay = 5e-3
    moose.connect(synhandler, 'activationOut', synchan, 'activation')
    #: SpikeGen detects when presynaptic Vm crosses threshold and
    #: sends out a spike event
    spikegen = moose.SpikeGen('/model/neuron_A/spikegen')
    spikegen.threshold = 0.0
    msg = moose.connect(nrn_a, 'VmOut', spikegen, 'Vm')
    msg = moose.connect(spikegen, 'spikeOut', synhandler.synapse[0],
                        'addSpike')
    return {'presynaptic': nrn_a, 'postsynaptic': nrn_b, 'spikegen':
            spikegen, 'synchan': synchan, 'synhandler': synhandler}

def setup_experiment(presynaptic, postsynaptic, synchan):
    """
Setup step current stimulation of presynaptic neuron. Also setup
recording of pre and postsynaptic Vm, Gk of synchan.

    """
    pulse = moose.PulseGen('/model/pulse')
    pulse.level[0] = 1e-9
    pulse.delay[0] = 0.02 # disable the pulsegen
    pulse.width[0] = 40e-3
    pulse.delay[1] = 1e9
    moose.connect(pulse, 'output', presynaptic, 'injectMsg')
    data = moose.Neutral('/data')
    vm_a = moose.Table('/data/Vm_pre')
    moose.connect(vm_a, 'requestOut', presynaptic, 'getVm')
    vm_b = moose.Table('/data/Vm_post')
    moose.connect(vm_b, 'requestOut', postsynaptic, 'getVm')
    gksyn_b = moose.Table('/data/Gk_syn')
    moose.connect(gksyn_b, 'requestOut', synchan, 'getGk')
    pulsetable = moose.Table('/data/pulse')
    pulsetable.connect('requestOut', pulse, 'getOutputValue')
    return { 'stimulus': pulsetable,
            'Vm_pre': vm_a,
            'Vm_post': vm_b,
            'Gk_syn': gksyn_b
        }

def main():
    """
A demo to create a network of single compartmental neurons connected
via alpha synapses. Here SynChan class is used to setup synaptic
connection between two single-compartmental Hodgkin-Huxley type
neurons.

    """
    simtime = 0.1
    simdt = 0.25e-5
    plotdt = 0.25e-3
    netinfo = create_model()
    expinfo = setup_experiment(netinfo['presynaptic'],
                               netinfo['postsynaptic'],
                               netinfo['synchan'])
    vm_a = expinfo['Vm_pre']
    vm_b = expinfo['Vm_post']
    gksyn_b = expinfo['Gk_syn']
    for ii in range(10):
        moose.setClock(ii, simdt)
    moose.setClock(18, plotdt)
    moose.reinit()
    moose.start(simtime)
    plt.subplot(211)
    plt.plot(vm_a.vector*1e3, color='b', label='presynaptic Vm (mV)')
    plt.plot(vm_b.vector*1e3, color='g', label='postsynaptic Vm (mV)')
    plt.plot(expinfo['stimulus'].vector * 1e9, color='r', label='injected current (nA)')
    plt.legend()
    plt.subplot(212)
    plt.plot(expinfo['Gk_syn'].vector*1e9, color='orange', label='Gk_synapse (nS)')
    plt.legend()
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    main()

#
# twocells.py ends here
