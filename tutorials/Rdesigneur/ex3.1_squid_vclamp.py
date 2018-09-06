########################################################################
# This demo illustrates voltage clamp on a soma model.
# Copyright (C) Upinder S. Bhalla, NCBS, 2018.
# This program is made available under the terms of GPL3.0. No warranty.
########################################################################
import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
    chanDistrib = [
        ['Na', 'soma', 'Gbar', '1200' ],
        ['K', 'soma', 'Gbar', '360' ]],
    stimList = [['soma', '1', '.', 'vclamp', '-0.065 + (t>0.1 && t<0.2) * 0.02' ]],
    plotList = [
        ['soma', '1', '.', 'Vm', 'Membrane potential'],
        ['soma', '1', 'vclamp', 'current', 'Soma holding current']
    ]
)

rdes.buildModel()
moose.reinit()
moose.start( 0.3 )

rdes.display()
