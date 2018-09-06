########################################################################
# This example demonstrates loading a soma with defined dimensions
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    # cellProto syntax: ['somaProto', 'name', dia, length]
    cellProto = [['somaProto', 'soma', 20e-6, 200e-6]],
    chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
    chanDistrib = [
        ['Na', 'soma', 'Gbar', '1200' ],
        ['K', 'soma', 'Gbar', '360' ]],
    stimList = [['soma', '1', '.', 'inject', '(t>0.01 && t<0.05) * 1e-9' ]],
    plotList = [['soma', '1', '.', 'Vm', 'Membrane potential']],
    moogList = [['#', '1', '.', 'Vm', 'Vm (mV)']]
)

rdes.buildModel()
soma = moose.element( '/model/elec/soma' )
print( 'Soma dia = {}, length = {}'.format( soma.diameter, soma.length ) )
moose.reinit()

rdes.displayMoogli( 0.0005, 0.06, 0.0 )
