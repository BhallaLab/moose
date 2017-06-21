from __future__ import print_function

import random
import time
from numpy import random as nprand

import moose

def make_network():
	size = 1024
	timestep = 0.2
	runtime = 100.0
	delayMin = timestep
	delayMax = 4
	weightMax = 0.02
	Vmax = 1.0
	thresh = 0.2
	tau = 1  # Range of tau
	tau0 = 0.5 # minimum tau
	refr = 0.3
	refr0 = 0.2
	connectionProbability = 0.1
	random.seed( 123 )
	nprand.seed( 456 )
	t0 = time.time()

	clock = moose.element( '/clock' )
	network = moose.IntFire( 'network', size, 1 );
	network.vec.bufferTime = [delayMax * 2] * size
	moose.le( '/network' )
	network.vec.numSynapses = [1] * size
	# Interesting. This fails because we haven't yet allocated
	# the synapses. I guess it is fair to avoid instances of objects that
	# don't have allocations.
	#synapse = moose.element( '/network/synapse' )
	sv = moose.vec( '/network/synapse' )
	print('before connect t = ', time.time() - t0)
	mid = moose.connect( network, 'spikeOut', sv, 'addSpike', 'Sparse')
	print('after connect t = ', time.time() - t0)
	#print mid.destFields
	m2 = moose.element( mid )
	m2.setRandomConnectivity( connectionProbability, 5489 )
	print('after setting connectivity, t = ', time.time() - t0)
	network.vec.Vm = [(Vmax*random.random()) for r in range(size)]
	network.vec.thresh = thresh
	network.vec.refractoryPeriod = [( refr0 + refr * random.random()) for r in range( size) ]
	network.vec.tau = [(tau0 + tau*random.random()) for r in range(size)]
	numSynVec = network.vec.numSynapses
	print('Middle of setup, t = ', time.time() - t0)
	numTotSyn = sum( numSynVec )
	for item in network.vec:
		neuron = moose.element( item )
		neuron.synapse.delay = [ (delayMin + random.random() * delayMax) for r in range( len( neuron.synapse ) ) ] 
		neuron.synapse.weight = nprand.rand( len( neuron.synapse ) ) * weightMax
	print('after setup, t = ', time.time() - t0, ", numTotSyn = ", numTotSyn)

	"""

	netvec = network.vec
	for i in range( size ):
		synvec = netvec[i].synapse.vec
		synvec.weight = [ (random.random() * weightMax) for r in range( synvec.len )] 
		synvec.delay = [ (delayMin + random.random() * delayMax) for r in range( synvec.len )] 
	"""

	#moose.useClock( 9, '/postmaster', 'process' )
	moose.useClock( 0, '/network', 'process' )
	moose.setClock( 0, timestep )
	moose.setClock( 9, timestep )
	t1 = time.time()
	moose.reinit()
	print('reinit time t = ', time.time() - t1)
	network.vec.Vm = [(Vmax*random.random()) for r in range(size)]
	print('setting Vm , t = ', time.time() - t1)
	t1 = time.time()
	print('starting')
	moose.start(runtime)
	print('runtime, t = ', time.time() - t1)
	print('Vm100:103', network.vec.Vm[100:103])
	print('Vm900:903', network.vec.Vm[900:903])
	print('weights 100:', network.vec[100].synapse.delay[0:5])
	print('weights 900:', network.vec[900].synapse.delay[0:5])

make_network()
