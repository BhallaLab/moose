########################################################################
# This example illustrates molecular transport of an oscillatory reaction
# system, along a closed-end cylinder. The concentrations along the entire 
# length of the cylinder start out uniform. It shows an interesting 
# combination of propagating oscillations and buildup of reagents.
########################################################################
import moose
import numpy as np
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    turnOffElec = True,
    diffusionLength = 2e-6, 
    chemProto = [['makeChemOscillator()', 'osc']],
    chemDistrib = [['osc', 'soma', 'install', '1' ]],
    plotList = [
        ['soma', '1', 'dend/a', 'conc', 'Concentration of a'],
        ['soma', '1', 'dend/b', 'conc', 'Concentration of b'],
        ['soma', '1', 'dend/a', 'conc', 'Concentration of a', 'wave'],
    ],
    moogList = [['soma', '1', 'dend/a', 'conc', 'a Conc', 0, 360 ]]
)
a = moose.element( '/library/osc/kinetics/a' )
b = moose.element( '/library/osc/kinetics/b' )
s = moose.element( '/library/osc/kinetics/s' )
a.diffConst = 0
b.diffConst = 0
a.motorConst = 1e-6

rdes.buildModel()
moose.reinit()

rdes.displayMoogli( 1, 400, rotation = 0, azim = -np.pi/2, elev = 0.0 )
