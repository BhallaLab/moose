# -*- coding: utf-8 -*-
# test_synchan.py ---

import moose
print( 'Using moose from %s' % moose.__file__ )

def make_synapse(path):
    """Create a synapse with two time constants. Connect a spikegen to the
    synapse. Create a pulsegen to drive the spikegen."""

    syn = moose.SynChan(path)
    syn.tau1 = 5.0 # ms
    syn.tau2 = 1.0 # ms
    syn.Gk = 1.0 # mS
    syn.Ek = 0.0

    ## NOTE: This is old implementation.
    #syn.synapse.num = 1
    ## syn.bufferTime = 1.0 # ms
    #syn.synapse.delay = 1.0
    #syn.synapse.weight = 1.0
    #print 'Synapses:', len(syn.synapse), 'w=', syn.synapse[0].weight

    # IN new implementation, there is SimpleSynHandler class which takes cares
    # of multiple synapses. Class SynChan does not have any .synapse field.
    synH = moose.SimpleSynHandler( '%s/SynHandler' % path)
    synH.synapse.num = 1
    ## syn.bufferTime = 1.0 # ms
    synH.synapse.delay = 1.0
    synH.synapse.weight = 1.0
    synH.connect('activationOut', syn, 'activation')
    print(('Synapses:', len(synH.synapse), 'w=', synH.synapse[0].weight ))

    spikegen = moose.SpikeGen('%s/spike' % (syn.parent.path))
    spikegen.edgeTriggered = False # Make it fire continuously when input is high
    spikegen.refractT = 10.0 # With this setting it will fire at 1 s / 10 ms = 100 Hz
    spikegen.threshold = 0.5
    # This will send alternatind -1 and +1 to SpikeGen to make it fire
    spike_stim = moose.PulseGen('%s/spike_stim' % (syn.parent.path))
    spike_stim.delay[0] = 1.0
    spike_stim.level[0] = 1.0
    spike_stim.width[0] = 100.0
    moose.connect(spike_stim, 'output', spikegen, 'Vm')
    m = moose.connect(spikegen, 'spikeOut', synH.synapse.vec, 'addSpike', 'Sparse')
    m.setRandomConnectivity(1.0, 1)
    m = moose.connect(spikegen, 'spikeOut', synH.synapse[0], 'addSpike') # this causes segfault
    return syn, spikegen

if __name__ == '__main__':
    model = moose.Neutral('/model')
    syn, spikegen = make_synapse('/model/synchan')
    moose.setClock(0, 0.01)
    moose.useClock(0, '/model/##', 'process')
    moose.reinit()
    moose.start(100)


#
# test_synchan.py ends here
