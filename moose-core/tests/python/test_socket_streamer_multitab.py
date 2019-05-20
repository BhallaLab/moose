# -*- coding: utf-8 -*-
'''
Here is some docstring.
'''
from __future__ import print_function, division

import os
import random
import time
import multiprocessing as mp
import socket
import moose
import json
import struct
import numpy as np
import moose.utils as mu


print('Using from %s. VERSION: %s' % (moose.__file__, moose.__version__))

sockPath = '/tmp/MOOSE'
os.environ['MOOSE_STREAMER_ADDRESS'] = 'file://%s' % sockPath


def streamer_handler(done, q):
    # Streamer handler
    global sockPath
    global all_done_
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    while not os.path.exists(sockPath):
        time.sleep(0.01)
        if done.value == 1:
            break
    s.connect( sockPath )
    print( "[INFO ] Connected" )
    s.settimeout(1e-5)
    data = b''
    while True:
        if done.value == 1:
            break
        try:
            d = s.recv(10240)
            if d.strip():
                data += d
        except socket.timeout:
            pass

    res = mu.decode_data(data)
    q.put(res)

def make_network():
    """
    This snippet sets up a recurrent network of IntFire objects, using
    SimpleSynHandlers to deal with spiking events.
    It isn't very satisfactory as activity runs down after a while.
    It is a good example for using the IntFire, setting up random
    connectivity, and using SynHandlers.
    """
    global all_done_
    done = mp.Value('i', 0)
    q = mp.Queue()
    th = mp.Process(target=streamer_handler, args=(done, q))
    th.start()

    size = 1024
    dt = 0.2
    runsteps = 10
    delayMin = 0
    delayMax = 4
    weightMax = 1
    Vmax = 1.0
    thresh = 0.4
    refractoryPeriod = 0.4
    tau = 0.5
    connectionProbability = 0.01
    random.seed( 123 )
    np.random.seed( 456 )
    t0 = time.time()

    network = moose.IntFire( 'network', size );
    syns = moose.SimpleSynHandler( '/network/syns', size );
    moose.connect( syns, 'activationOut', network, 'activation', 'OneToOne' )
    moose.le( '/network' )
    syns.vec.numSynapses = [1] * size
    sv = moose.vec( '/network/syns/synapse' )
    print('before connect t = %.3f'%(time.time() - t0))
    mid = moose.connect( network, 'spikeOut', sv, 'addSpike', 'Sparse')
    print('after connect t = %.3f'%(time.time() - t0))
    #print mid.destFields
    m2 = moose.element( mid )
    m2.setRandomConnectivity( connectionProbability, 5489 )
    print('after setting connectivity, t=%.3f'%(time.time() - t0))
    #network.vec.Vm = [(Vmax*random.random()) for r in range(size)]
    network.vec.Vm = np.random.rand( size ) * Vmax
    network.vec.thresh = thresh
    network.vec.refractoryPeriod = refractoryPeriod
    network.vec.tau = tau
    numSynVec = syns.vec.numSynapses
    print('Middle of setup, t = %.3f'%(time.time() - t0))
    numTotSyn = sum( numSynVec )
    print((numSynVec.size, ', tot = ', numTotSyn,  ', numSynVec = ', numSynVec))
    for item in syns.vec:
        h = moose.element(item)
        h.synapse.delay = delayMin + (delayMax-delayMin) * np.random.rand(len(h.synapse))
        h.synapse.weight = np.random.rand(len(h.synapse)) * weightMax
    print('After setup, t = %.3f'%(time.time()-t0))

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

    t1 = time.time()
    moose.reinit()
    print('reinit time t = %.3f'%(time.time() - t1))
    network.vec.Vm = np.random.rand( size ) * Vmax
    print('setting Vm , t = %.3f'%(time.time() - t1))
    t1 = time.time()
    moose.start(runsteps * dt, 1)
    time.sleep(0.1)
    done.value = 1
    print('runtime, t = %.3f'%(time.time() - t1))
    print(network.vec.Vm[99:103], network.vec.Vm[900:903])
    res = q.get()

    for tabPath in res:
        aWithTime = res[tabPath]
        a = aWithTime[1::2]
        b = moose.element(tabPath).vector
        print( tabPath, len(a), len(b) )
        if len(a) == len(b):
            assert np.equal(a, b).all()
        else:
            print( "Table did not equal size. The last table is allowed to "
                    " have fewer entries." )


    th.join()
    print( 'All done' )

if __name__ == '__main__':
    make_network()
