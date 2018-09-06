########################################################################
# This example illustrates molecular transport along a closed-end 
# cylinder. All the molecules start out in a single voxel A[0] on the left,
# and all end up at A[99], on the right.
########################################################################

import moose
import numpy as np
import pylab
import rdesigneur as rd

moose.Neutral( '/library' )
moose.Neutral( '/library/transp' )
moose.CubeMesh( '/library/transp/dend' )
A = moose.Pool( '/library/transp/dend/A' )
A.diffConst = 0
A.motorConst = 1e-6     # Metres/sec

rdes = rd.rdesigneur(
    turnOffElec = True,
    #This subdivides the length of the soma into 0.5 micron voxels
    diffusionLength = 0.5e-6, 
    cellProto = [['somaProto', 'soma', 2e-6, 50e-6]],
    chemProto = [['transp', 'transp']],
    chemDistrib = [['transp', 'soma', 'install', '1' ]],
    plotList = [
        ['soma', '1', 'dend/A', 'conc', 'Concentration of A'],
        ['soma', '1', 'dend/A', 'conc', 'Concentration of A', 'wave'],
    ],
    moogList = [['soma', '1', 'dend/A', 'conc', 'A Conc', 0, 20 ]]
)

rdes.buildModel()
moose.element( '/model/chem/dend/A[0]' ).concInit = 0.1
moose.reinit()

rdes.displayMoogli( 1, 80, rotation = 0, azim = -np.pi/2, elev = 0.0 )
