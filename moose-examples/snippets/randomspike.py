# randomspike.py --- 
#
# Filename: randomspike.py
# Author: Subhasis Ray
# Maintainer:
# Created: Tue Sep 30 10:58:09 2014 (+0530)
# Version:
# Last-Updated:
#           By:
#     Update #: 0
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
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

SIMTIME = 5.0

def create_cell():
    """Create a single-compartment Hodgking-Huxley neuron with a
    synaptic channel.

    This uses the :func:`ionchannel.create_1comp_neuron` function for
    model creation.

    Returns a dict containing the neuron, the synchan and the
    synhandler for accessing the synapse,
    """
    neuron = create_1comp_neuron('/neuron')
    #: SynChan for post synaptic neuron
    synchan = moose.SynChan('/neuron/synchan')
    synchan.Gbar = 1e-8
    synchan.tau1 = 2e-3
    synchan.tau2 = 2e-3
    msg = moose.connect(neuron, 'channel', synchan, 'channel')
    #: Create SynHandler to handle spike event input and set the
    #: activation input of synchan
    synhandler = moose.SimpleSynHandler('/neuron/synhandler')
    synhandler.synapse.num = 1
    synhandler.synapse[0].delay = 5e-3
    moose.connect(synhandler, 'activationOut', synchan, 'activation')
    return {'neuron': neuron,
            'synchan': synchan,
            'synhandler': synhandler}


def example():
    """
    The RandSpike class generates spike events from a Poisson process
    and sends out a trigger via its `spikeOut` message. It is very
    common to approximate the spiking in many neurons as a Poisson
    process, i.e., the probability of `k` spikes in any interval `t`
    is given by the Poisson distribution:

        exp(-ut)(ut)^k/k!

    for k = 0, 1, 2, ... u is the rate of spiking (the mean of the
    Poisson distribution). See `wikipedia
    <http://en.wikipedia.org/wiki/Poisson_process>`__ for details.

    Many cortical neuron types spontaneously fire action
    potentials. These are called ectopic spikes. In this example we
    simulate this with a RandSpike object with rate 10 spikes/s and
    send this to a single compartmental neuron via a synapse.

    In this model the synaptic conductance is set so high that each
    incoming spike evokes an action potential.
    """
    ectopic = moose.RandSpike('ectopic_input')
    ectopic.rate = 10.0
    cellmodel = create_cell()
    moose.connect(ectopic, 'spikeOut',
                  cellmodel['synhandler'].synapse[0], 'addSpike')
    tab_vm = moose.Table('/Vm')
    moose.connect(tab_vm, 'requestOut', cellmodel['neuron'], 'getVm')
    moose.reinit()
    moose.start(SIMTIME)
    return tab_vm

def main():

	"""This is an example of simulating random events from a Poisson
	process and applying the event as spike input to a
	single-compartmental Hodgekin-Huxley type neuron model."""

	tab_vm = example()
	ts = np.linspace(0, SIMTIME, len(tab_vm.vector))
	plt.plot(ts, tab_vm.vector)
	plt.ylabel('Vm (Volt)')
	plt.xlabel('Time (s)')
	plt.show()

if __name__ == '__main__':
	main()

#
# randomspike.py ends here
