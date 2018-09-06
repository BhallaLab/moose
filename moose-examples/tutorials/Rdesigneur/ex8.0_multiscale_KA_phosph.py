########################################################################
# This example demonstrates a multiscale bistable model. 
# In the low state there is no spiking, no Ca entry, and the KA channel 
# is unophosporylated and hence active. This keeps the potential low.
# In the high state, there is sustained spiking leading to Ca entry, setting
# off reactions that dephosphorylate and turn off the KA and so make the
# cell more excitable. Thus the activity continues.
# Brief current pulses cause the cell to toggle between the states.
# 
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    elecDt = 50e-6,
    chemDt = 0.002,
    chemPlotDt = 0.002,
    # cellProto syntax: ['somaProto', 'name', dia, length]
    cellProto = [['somaProto', 'soma', 12e-6, 12e-6]],
    chemProto = [['./chem/chanPhosphByCaMKII.g', 'chem']],
    chanProto = [
        ['make_Na()', 'Na'], 
        ['make_K_DR()', 'K_DR'], 
        ['make_K_A()', 'K_A' ],
        ['make_Ca()', 'Ca' ],
        ['make_Ca_conc()', 'Ca_conc' ]
    ],
    # Some changes to the default passive properties of the cell.
    passiveDistrib = [['soma', 'CM', '0.03', 'Em', '-0.06']],
    chemDistrib = [['chem', 'soma', 'install', '1' ]],
    chanDistrib = [
        ['Na', 'soma', 'Gbar', '300' ],
        ['K_DR', 'soma', 'Gbar', '250' ],
        ['K_A', 'soma', 'Gbar', '200' ],
        ['Ca_conc', 'soma', 'tau', '0.0333' ],
        ['Ca', 'soma', 'Gbar', '40' ]
    ],
    adaptorList = [
        [ 'dend/chan', 'conc', 'K_A', 'modulation', 0.0, 70 ],
        [ 'Ca_conc', 'Ca', 'dend/Ca', 'conc', 0.00008, 2 ]
    ],
    # Give a + pulse from 5 to 7s, and a - pulse from 20 to 21.
    stimList = [['soma', '1', '.', 'inject', '((t>5 && t<7) - (t>20 && t<21)) * 1.0e-12' ]],
    plotList = [
        ['soma', '1', '.', 'Vm', 'Membrane potential'],
        ['soma', '1', '.', 'inject', 'current inj'],
        ['soma', '1', 'K_A', 'Ik', 'K_A current'],
        ['soma', '1', 'dend/chan', 'conc', 'Unphosph K_A conc'],
        ['soma', '1', 'dend/Ca', 'conc', 'Chem Ca'],
    ],
)

rdes.buildModel()
moose.reinit()
moose.start( 30 )

rdes.display()
