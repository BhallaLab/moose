########################################################################
# This example illustrates how a function can be used to control a reaction
# rate. This kind of calculation is appropriate when we need to link 
# different kinds of physical processses with chemical reactions, for  
# example, membrane curvature with molecule accumulation. The use of
# functions to modify reaction rates should be avoided in purely chemical 
# systems since they obscure the underlying chemistry, and do not map
# cleanly to stochastic calculations.
#
# In this example we simply have a molecule C that controls the forward
# rate of a reaction that converts A to B. C is a function of location
# on the cylinder, and is fixed. In more elaborate computations we could
# have a function of multiple molecules, some of which could be changing and
# others could be buffered.
#  
#  Copyright (C) Upinder S. Bhalla NCBS 2018
#  Released under the terms of the GNU Public License V3.
########################################################################

import numpy as np
import moose
import pylab
import rdesigneur as rd


def makeFuncRate():
    model = moose.Neutral( '/library' )
    model = moose.Neutral( '/library/chem' )
    compt = moose.CubeMesh( '/library/chem/compt' )
    compt.volume = 1e-15
    A = moose.Pool( '/library/chem/compt/A' )
    B = moose.Pool( '/library/chem/compt/B' )
    C = moose.Pool( '/library/chem/compt/C' )
    reac = moose.Reac( '/library/chem/compt/reac' )
    func = moose.Function( '/library/chem/compt/reac/func' )
    func.x.num = 1
    func.expr = "(x0/1e8)^2"
    moose.connect( C, 'nOut', func.x[0], 'input' )
    moose.connect( func, 'valueOut', reac, 'setNumKf' )
    moose.connect( reac, 'sub', A, 'reac' )
    moose.connect( reac, 'prd', B, 'reac' )

    A.concInit = 1
    B.concInit = 0
    C.concInit = 0
    reac.Kb = 1


makeFuncRate()

rdes = rd.rdesigneur(
        turnOffElec = True,
        #This subdivides the 50-micron cylinder into 2 micron voxels
        diffusionLength = 2e-6,
        cellProto = [['somaProto', 'soma', 5e-6, 50e-6]],
        chemProto = [['chem', 'chem']],
        chemDistrib = [['chem', 'soma', 'install', '1' ]],
        plotList = [['soma', '1', 'dend/A', 'conc', 'A conc', 'wave'],
            ['soma', '1', 'dend/C', 'conc', 'C conc', 'wave']],
)
rdes.buildModel()

C = moose.element( '/model/chem/dend/C' )
C.vec.concInit = [ 1+np.sin(x/5.0) for x in range( len(C.vec) ) ]
moose.reinit()
moose.start(10)
rdes.display()
