########################################################################
# This example demonstrates insertion of endo-compartments into the
# dendrite. Here endo_compartments are used for the endoplasmic reticulum 
# (ER) in a model of Calcium Induced Calcium Release through the 
# IP3 receptor. It generates a series of propagating waves of calcium.
# Note that units of permeability in the ConcChan are 1/(millimolar.sec)
# 
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    turnOffElec = True,
    chemDt = 0.005,
    chemPlotDt = 0.02,
    numWaveFrames = 200,
    diffusionLength = 1e-6,
    useGssa = False,
    addSomaChemCompt = False,
    addEndoChemCompt = True,
    # cellProto syntax: ['somaProto', 'name', dia, length]
    cellProto = [['somaProto', 'soma', 2e-6, 10e-6]],
    chemProto = [['./chem/CICRwithConcChan.g', 'chem']],
    chemDistrib = [['chem', 'soma', 'install', '1' ]],
    plotList = [
        ['soma', '1', 'dend/CaCyt', 'conc', 'Dendritic Ca'],
        ['soma', '1', 'dend/CaCyt', 'conc', 'Dendritic Ca', 'wave'],
        ['soma', '1', 'dend_endo/CaER', 'conc', 'ER Ca'],
        ['soma', '1', 'dend/ActIP3R', 'conc', 'active IP3R'],
    ],
)

rdes.buildModel()
IP3 = moose.element( '/model/chem/dend/IP3' )
IP3.vec.concInit = 0.004
IP3.vec[0].concInit = 0.02
moose.reinit()
moose.start( 20 )

rdes.display()
