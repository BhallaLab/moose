import moose
import numpy as np
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    turnOffElec = True,
    #This subdivides the length of the soma into 2 micron voxels
    diffusionLength = 2e-6, 
    chemProto = [['makeChemOscillator()', 'osc']],
    chemDistrib = [['osc', 'soma', 'install', '1' ]],
    plotList = [['soma', '1', 'dend/a', 'conc', 'Concentration of a'],
        ['soma', '1', 'dend/b', 'conc', 'Concentration of b']],
    moogList = [['soma', '1', 'dend/a', 'conc', 'a Conc', 0, 360 ]]
)

rdes.buildModel()
bv = moose.vec( '/model/chem/dend/b' )
bv[0].concInit *= 2
bv[-1].concInit *= 2
moose.reinit()

rdes.displayMoogli( 1, 400, rotation = 0, azim = np.pi/2, elev = 0.0 )
