########################################################################
# This example illustrates molecular transport of an oscillatory reaction
# system, along a closed-end cylinder. Here all the molecules are 
#, transported, a left to right and b and s right to left.
########################################################################
import moose
import numpy as np
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    turnOffElec = True,
    diffusionLength = 2e-6, 
    numWaveFrames = 50,
    chemProto = [['makeChemOscillator()', 'osc']],
    chemDistrib = [['osc', 'soma', 'install', '1' ]],
    plotList = [
        ['soma', '1', 'dend/a', 'conc', 'Concentration of a', 'wave', 0, 1800],
        ['soma', '1', 'dend/b', 'conc', 'Concentration of b', 'wave', 0, 500],
        ['soma', '1', 'dend/s', 'conc', 'Concentration of s', 'wave', 0, 1200],
    ],
    moogList = [['soma', '1', 'dend/a', 'conc', 'a Conc', 0, 600 ]]
)
a = moose.element( '/library/osc/kinetics/a' )
b = moose.element( '/library/osc/kinetics/b' )
s = moose.element( '/library/osc/kinetics/s' )
a.diffConst = 0
b.diffConst = 0
a.motorConst = 2e-6
b.motorConst = -2e-6
s.motorConst = -2e-6

rdes.buildModel()
moose.reinit()

rdes.displayMoogli( 1, 250, rotation = 0, azim = -np.pi/2, elev = 0.0 )
