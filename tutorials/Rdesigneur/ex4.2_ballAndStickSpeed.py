########################################################################
# This example demonstrates a speed test on a ball-and-stick model of 
# a neuron with 10 compartments in the dendrite, that support propagating
# action potentials. It also shows how to deliver an interesting stimulus.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import moose
import pylab
import rdesigneur as rd
import time
rdes = rd.rdesigneur(
    cellProto = [['ballAndStick', 'soma', 20e-6, 20e-6, 4e-6, 500e-6, 10]],
    chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
    chanDistrib = [
        ['Na', 'soma', 'Gbar', '1200' ],
        ['K', 'soma', 'Gbar', '360' ],
        ['Na', 'dend#', 'Gbar', '400' ],
        ['K', 'dend#', 'Gbar', '120' ]
    ],
    stimList = [['soma', '1', '.', 'inject', '(1+cos(t/10))*(t>31.4 && t<94) * 0.5e-9' ]],
    plotList = [
        ['soma', '1', '.', 'Vm', 'Membrane potential'],
        ['soma', '1', '.', 'inject', 'Stimulus current']
    ],
)

rdes.buildModel()


runtime = 100
moose.reinit()
t0= time.time()
moose.start( runtime )
print( "Real time to run {} simulated seconds = {} seconds".format( runtime,
    time.time() - t0 ) )

rdes.display()
