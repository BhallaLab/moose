######################################################################
# This example shows simple diffusion in 1 dimension. It plots
# snapshots of the conc profile at 2 second intervals.
######################################################################

import moose
import pylab
import re
import rdesigneur as rd
import matplotlib.pyplot as plt
import numpy as np

moose.Neutral( '/library' )
moose.Neutral( '/library/diffn' )
moose.CubeMesh( '/library/diffn/dend' )
A = moose.Pool( '/library/diffn/dend/A' )
A.diffConst = 1e-10

rdes = rd.rdesigneur(
    turnOffElec = True,
    diffusionLength = 1e-6,
    chemProto = [['diffn', 'diffn']],
    chemDistrib = [['diffn', 'soma', 'install', '1' ]],
    moogList = [
            ['soma', '1', 'dend/A', 'conc', 'A Conc', 0, 360 ]
    ]
)
rdes.buildModel()

rdes.displayMoogli( 1, 2, rotation = 0, azim = -np.pi/2, elev = 0.0, block = False )
av = moose.vec( '/model/chem/dend/A' )
for i in range(10):
    av[i].concInit = 1
moose.reinit()
plist = []
for i in range( 20 ):
    plist.append( av.conc[:200] )
    moose.start( 2 )
fig = plt.figure( figsize = ( 10, 12 ) )
plist = np.array( plist ).T
plt.plot( range( 0, 200 ), plist )
plt.xlabel( "position ( microns )" )
plt.ylabel( "concentration ( mM )" )
plt.show( block = True )
