########################################################################
# This example demonstrates random (Poisson) synaptic input to a cell.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3. No warranty.
########################################################################
import moose
import rdesigneur as rd
rdes = rd.rdesigneur(
    cellProto = [['somaProto', 'soma', 20e-6, 200e-6]],
    chanProto = [['make_glu()', 'glu']],
    chanDistrib = [['glu', 'soma', 'Gbar', '1' ]],
    stimList = [['soma', '0.5', 'glu', 'randsyn', '50' ]],
    # Deliver stimulus to glu synapse on soma, at mean 50 Hz Poisson.
    plotList = [['soma', '1', '.', 'Vm', 'Soma membrane potential']]
)
rdes.buildModel()
moose.reinit()
moose.start( 0.3 )

rdes.display()
