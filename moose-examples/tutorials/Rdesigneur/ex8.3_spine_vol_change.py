########################################################################
# This example demonstrates volume scaling of spine as part of a 
# multiscale computation. As in ex8.2, we have the reactions:
#
#   Ca+CaM <===> Ca_CaM;    Ca_CaM + CaMKII <===> Ca_CaM_CaMKII (all in 
#       spine head, except that the Ca_CaM_CaMKII translocates to the PSD)
#   chan ------Ca_CaM_CaMKII-----> chan_p; chan_p ------> chan  (all in PSD)
# 
# Here, chan_p controls area of the PSD, and lots of things scale from that.
# 
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import moose
import rdesigneur as rd
rdes = rd.rdesigneur(
    elecDt = 50e-6,
    chemDt = 0.002,
    diffDt = 0.002,
    chemPlotDt = 0.02,
    useGssa = False,
    stealCellFromLibrary = True, # Simply move library model to use for sim
    cellProto = [['ballAndStick', 'soma', 12e-6, 12e-6, 4e-6, 100e-6, 2 ]],
    chemProto = [['./chem/chanPhosph3compt.g', 'chem']],
    spineProto = [['makeActiveSpine()', 'spine']],
    chanProto = [
        ['make_Na()', 'Na'], 
        ['make_K_DR()', 'K_DR'], 
        ['make_K_A()', 'K_A' ],
        ['make_Ca()', 'Ca' ],
        ['make_Ca_conc()', 'Ca_conc' ]
    ],
    passiveDistrib = [['soma', 'CM', '0.01', 'Em', '-0.06']],
    spineDistrib = [['spine', '#dend#', '50e-6', '1e-6']],
    chemDistrib = [['chem', '#', 'install', '1' ]],
    chanDistrib = [
        ['Na', 'soma', 'Gbar', '300' ],
        ['K_DR', 'soma', 'Gbar', '250' ],
        ['K_A', 'soma', 'Gbar', '200' ],
        ['Ca_conc', 'soma', 'tau', '0.0333' ],
        ['Ca', 'soma', 'Gbar', '40' ]
    ],
    adaptorList = [
        # This scales the psdArea of the spine by # of chan_p. Note that 
        # the cross-section area of the spine head is identical to psdArea.
        [ 'psd/chan_p', 'n', 'spine', 'psdArea', 0.1e-12, 0.01e-12 ],
        [ 'Ca_conc', 'Ca', 'spine/Ca', 'conc', 0.00008, 8 ]
    ],
    # Syn input basline 1 Hz, and 40Hz burst for 1 sec at t=20. Syn wt=10
    stimList = [['head#', '10','glu', 'periodicsyn', '1 + 40*(t>10 && t<11)']],
    plotList = [
        ['soma', '1', '.', 'Vm', 'Membrane potential'],
        ['#', '1', 'spine/Ca', 'conc', 'Ca in Spine'],
        ['#', '1', 'dend/DEND/Ca', 'conc', 'Ca in Dend'],
        ['head#', '1', 'psd/chan_p', 'n', 'Amount of Phospho-chan'],
        ['head#', '1', 'spine/CaMKII', 'conc', 'Conc of CaMKII in spine'],
        ['head#', '1', '.', 'Cm', 'Capacitance of spine head'],
        ['head#', '1', '.', 'Rm', 'Membrane res of spine head'],
        ['head#', '1', '.', 'Ra', 'Axial res of spine head'],
        ['head#', '1', 'glu', 'Gbar', 'Conductance of gluR'],
        ['head#', '1', 'NMDA', 'Gbar', 'Conductance of NMDAR'],
    ]
)
moose.seed(123)
rdes.buildModel()
moose.reinit()
moose.start( 25 )
rdes.display()
