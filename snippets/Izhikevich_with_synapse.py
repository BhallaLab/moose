# __BROKEN__
# Izhikevich_with_synapse.py ---
#
# Filename: Izhikevich_with_synapse.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Sat Apr 19 10:47:15 2014 (+0530)
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
import pylab
sys.path.append('../../python')
import moose
from moose import utils as mutils

def make_neuron(path):
    """Create a neuron with parameters set for tonic_bursting."""
    nrn = moose.IzhikevichNrn(path)
    # "tonic_bursting":   ['C', 0.02  ,    0.2  ,   -50.0,     2.0  ,      15.0,      -70.0,  220.0], # Fig. 1.C
    #print((path,dir(nrn)))
    nrn.alpha = 0.04
    nrn.beta = 5.0
    nrn.gamma = 140.0
    #nrn.Rm = 1.0           # FIXME: IzhikevichNrn does not have this field.
    nrn.a = 0.02
    nrn.b = 0.2
    nrn.c = -50.0
    nrn.d = 2.0
    nrn.initVm = -70.0
    nrn.Vmax = 40.0
    return nrn

def make_pulsegen(path):
    """Create a pulse generator to test tonic spiking in the neuron."""
    pulse = moose.PulseGen(path)
    pulse.delay[0] = 10.0 # ms
    pulse.width[0] = 200.0 # ms
    pulse.level[0] = 15e-3 # uA
    return pulse

def make_synapse(path):
    """Create a synapse with two time constants."""
    syn = moose.SynChan(path)
    syn.tau1 = 5.0 # ms
    syn.tau2 = 1.0 # ms
    syn.Gbar = 1.0 # mS
    syn.Ek = 0.0
    synsh = moose.SimpleSynHandler( path + '/sh' )
    synsh.synapse.num = 1
    # syn.bufferTime = 1.0 # ms
    synsh.synapse.delay = 1.0
    synsh.synapse.weight = 1.0
    print(('Synapses:', len(synsh.synapse), 'w=', synsh.synapse[0].weight))
    spikegen = moose.SpikeGen('%s/spike' % (syn.parent.path))
    spikegen.edgeTriggered = False # Make it fire continuously when input is high
    spikegen.refractT = 10.0 # With this setting it will fire at 1 s / 10 ms = 100 Hz
    spikegen.threshold = 0.5
    # This will send alternatind -1 and +1 to SpikeGen to make it fire
    spike_stim = moose.PulseGen('%s/spike_stim' % (syn.parent.path))
    spike_stim.delay[0] = 50.0
    spike_stim.level[0] = 1.0
    spike_stim.width[0] = 100.0
    moose.connect(spike_stim, 'output', spikegen, 'Vm')
    m = moose.connect(spikegen, 'spikeOut', synsh.synapse[0], 'addSpike')
    return syn, spikegen

def make_model():
    model = moose.Neutral('/model') # Just a container for other things
    neuron = make_neuron('/model/neuron')
    pulse = make_pulsegen('/model/pulse')
    synapse, spike_in = make_synapse('/model/synapse')
    # moose.connect(pulse, 'output', neuron, 'injectDest')
    moose.connect(neuron, 'channel', synapse, 'channel')
    return {'neuron': neuron,
            'pulse': pulse,
            'synapse': synapse,
            'spike_in': spike_in}

def setup_data_recording(neuron, pulse, synapse, spikegen):
    data = moose.Neutral('/data')
    vm_table = moose.Table('/data/Vm')
    moose.connect(vm_table, 'requestOut', neuron, 'getVm')
    inject_table = moose.Table('/data/Inject')
    moose.connect(inject_table, 'requestOut', pulse, 'getOutputValue')
    gk_table = moose.Table('/data/Gk')
    moose.connect(gk_table, 'requestOut', synapse, 'getGk')
    spike_in_table = moose.Table('/data/spike_in')
    moose.connect(spikegen, 'spikeOut', spike_in_table, 'spike')
    return [vm_table, inject_table, gk_table, spike_in_table]

def main():
    """
    This shows the use of SynChan with Izhikevich neuron. This can be
    used for creating a network of Izhikevich neurons.
    """
    
    simtime = 200.0
    stepsize = 10.0
    model_dict = make_model()
    vm, inject, gk, spike = setup_data_recording(model_dict['neuron'],
                                          model_dict['pulse'],
                                          model_dict['synapse'],
                                          model_dict['spike_in'])
    mutils.setDefaultDt(elecdt=0.01, plotdt2=0.25)
    mutils.assignDefaultTicks(solver='ee')
    moose.reinit()
    mutils.stepRun(simtime, stepsize)
    pylab.subplot(411)
    pylab.plot(pylab.linspace(0, simtime, len(vm.vector)), vm.vector, label='Vm (mV)')
    pylab.legend()
    pylab.subplot(412)
    pylab.plot(pylab.linspace(0, simtime, len(inject.vector)), inject.vector, label='Inject (uA)')
    pylab.legend()
    pylab.subplot(413)
    pylab.plot(spike.vector, pylab.ones(len(spike.vector)), '|', label='input spike times')
    pylab.legend()
    pylab.subplot(414)
    pylab.plot(pylab.linspace(0, simtime, len(gk.vector)), gk.vector, label='Gk (mS)')
    pylab.legend()
    pylab.show()

if __name__ == '__main__':
    main()


#
# Izhikevich_with_synapse.py ends here
