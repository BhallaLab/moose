########################################################################
# This example demonstrates loading a ball-and-stick model of a neuron
# with 10 compartments in the dendrite, that support propagating
# action potentials.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
    # The numerical arguments are all optional
    cellProto = [['ballAndStick', 'soma', 20e-6, 20e-6, 4e-6, 500e-6, 10]],
    chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
    chanDistrib = [
        ['Na', 'soma', 'Gbar', '1200' ],
        ['K', 'soma', 'Gbar', '360' ],
        ['Na', 'dend#', 'Gbar', '400' ],
        ['K', 'dend#', 'Gbar', '120' ]
        ],
    stimList = [['soma', '1', '.', 'inject', '(t>0.01 && t<0.05) * 1e-9' ]],
    plotList = [['soma', '1', '.', 'Vm', 'Membrane potential']],
    moogList = [['#', '1', '.', 'Vm', 'Vm (mV)']]
)

rdes.buildModel()
soma = moose.element( '/model/elec/soma' )
moose.reinit()

rdes.displayMoogli( 0.0005, 0.06, 0.0 )
