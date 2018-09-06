# -*- coding: utf-8 -*-

#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

from __future__ import print_function, division

import os
import random
import time
import pylab
from numpy import random as nprand
import sys
import moose

def make_network():
    """
    This snippet sets up a recurrent network of IntFire objects, using
    SimpleSynHandlers to deal with spiking events. 
    It isn't very satisfactory as activity runs down after a while.
    It is a good example for using the IntFire, setting up random
    connectivity, and using SynHandlers.

    """
    size = 1024
    dt = 0.2
    runsteps = 50
    delayMin = 0
    delayMax = 4
    weightMax = 1
    Vmax = 1.0
    thresh = 0.4
    refractoryPeriod = 0.4
    tau = 0.5
    connectionProbability = 0.01
    random.seed( 123 )
    nprand.seed( 456 )
    t0 = time.time()

    network = moose.IntFire( 'network', size );
    syns = moose.SimpleSynHandler( '/network/syns', size );
    moose.connect( syns, 'activationOut', network, 'activation', 'OneToOne' )
    moose.le( '/network' )
    syns.vec.numSynapses = [1] * size
    sv = moose.vec( '/network/syns/synapse' )
    print(('before connect t = ', time.time() - t0))
    mid = moose.connect( network, 'spikeOut', sv, 'addSpike', 'Sparse')
    print(('after connect t = ', time.time() - t0))
    #print mid.destFields
    m2 = moose.element( mid )
    m2.setRandomConnectivity( connectionProbability, 5489 )
    print(('after setting connectivity, t = ', time.time() - t0))
    #network.vec.Vm = [(Vmax*random.random()) for r in range(size)]
    network.vec.Vm = nprand.rand( size ) * Vmax
    network.vec.thresh = thresh
    network.vec.refractoryPeriod = refractoryPeriod
    network.vec.tau = tau
    numSynVec = syns.vec.numSynapses
    print(('Middle of setup, t = ', time.time() - t0))
    numTotSyn = sum( numSynVec )
    print((numSynVec.size, ', tot = ', numTotSyn,  ', numSynVec = ', numSynVec))
    for item in syns.vec:
            sh = moose.element( item )
            sh.synapse.delay = delayMin +  (delayMax - delayMin ) * nprand.rand( len( sh.synapse ) )
            #sh.synapse.delay = [ (delayMin + random.random() * (delayMax - delayMin ) for r in range( len( sh.synapse ) ) ] 
            sh.synapse.weight = nprand.rand( len( sh.synapse ) ) * weightMax
    print(('after setup, t = ', time.time() - t0))

    numStats = 100
    stats = moose.SpikeStats( '/stats', numStats )
    stats.vec.windowLength = 1 # timesteps to put together.
    plots = moose.Table( '/plot', numStats )
    convergence = size // numStats
    for i in range( numStats ):
        for j in range( size//numStats ):
            k = i * convergence + j
            moose.connect( network.vec[k], 'spikeOut', stats.vec[i], 'addSpike' )
    moose.connect( plots, 'requestOut', stats, 'getMean', 'OneToOne' )

    #moose.useClock( 0, '/network/syns,/network', 'process' )
    moose.useClock( 0, '/network/syns', 'process' )
    moose.useClock( 1, '/network', 'process' )
    moose.useClock( 2, '/stats', 'process' )
    moose.useClock( 3, '/plot', 'process' )
    moose.setClock( 0, dt )
    moose.setClock( 1, dt )
    moose.setClock( 2, dt )
    moose.setClock( 3, dt )
    moose.setClock( 9, dt )
    t1 = time.time()
    moose.reinit()
    print(('reinit time t = ', time.time() - t1))
    network.vec.Vm = nprand.rand( size ) * Vmax
    print(('setting Vm , t = ', time.time() - t1))
    t1 = time.time()
    print('starting')
    moose.start(runsteps * dt)
    print(('runtime, t = ', time.time() - t1))
    print((network.vec.Vm[99:103], network.vec.Vm[900:903]))
    t = [i * dt for i in range( plots.vec[0].vector.size )]
    i = 0
    for p in plots.vec:
        pylab.plot( t, p.vector, label=str( i) )
        i += 1
    pylab.xlabel( "Time (s)" )
    pylab.ylabel( "Vm (mV)" )
    pylab.legend()
    pylab.show()

if __name__ == '__main__':
    make_network()
