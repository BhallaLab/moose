
"""This is to show a _raw_ way of traversing messages."""
import sys
sys.path.append('../../python')

import moose

connectionProbability = 0.5
net = moose.IntFire('/net1', 10)
syn = moose.SimpleSynHandler( '/net1/sh', 10 )
moose.connect( syn, 'activationOut', net, 'activation', 'OneToOne' )
synapse = syn.synapse.vec
mid = moose.connect(net, 'spikeOut', synapse, 'addSpike', 'Sparse') # This creates a `Sparse` message from `spikeOut` source of net to `addSpike` destination on synapse.
msg = moose.element(mid)
msg.setRandomConnectivity(connectionProbability, 5)
for n in net.vec:
    print 'Messages from %s.spikeOut' % (n.path)
    node = moose.element(n)
    for dest, df in zip(node.msgDests['spikeOut'], node.msgDestFunctions['spikeOut']):
        print '\t--> %s.%s' % (dest.path, df)


