#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################


import sys
import math
import numpy as np
import moose
print('[INFO] Using moose from %s' % moose.__file__ )
import os

moose.seed( 1 )

concA = 0.005 # millimolar
def makeModel():
    # create container for model
    r0 = 2e-6        # m
    r1 = 1e-6        # m
    num = 100
    diffLength = 1e-6 # m
    L = num * diffLength        # m
    diffConst = 10e-12
    #motorRate = 1e-6
    #diffConst = 0
    motorRate = 0

    model = moose.Neutral( 'model' )
    compartment = moose.CylMesh( '/model/compartment' )
    compartment.r0 = r0
    compartment.r1 = r1
    compartment.x0 = 0
    compartment.x1 = L
    compartment.diffLength = diffLength
    assert( compartment.numDiffCompts == num )

    # create molecules and reactions
    a = moose.Pool( '/model/compartment/a' )
    b = moose.Pool( '/model/compartment/b' )
    c = moose.Pool( '/model/compartment/c' )
    d = moose.Pool( '/model/compartment/d' )
    r1 = moose.Reac( '/model/compartment/r1' )
    moose.connect( r1, 'sub', b, 'reac' )
    moose.connect( r1, 'sub', d, 'reac' )
    moose.connect( r1, 'prd', c, 'reac' )
    r1.Kf = 1000.0 # 1/(mM.sec)
    r1.Kb = 1 # 1/sec

    # Assign parameters
    a.diffConst = diffConst
    b.diffConst = diffConst / 2.0
    b.motorConst = motorRate
    c.diffConst = diffConst
    d.diffConst = diffConst

    # Make solvers
    ksolve = moose.Gsolve( '/model/compartment/ksolve' )
    dsolve = moose.Dsolve( '/model/compartment/dsolve' )
    stoich = moose.Stoich( '/model/compartment/stoich' )
    stoich.compartment = compartment
    stoich.ksolve = ksolve
    stoich.dsolve = dsolve
    stoich.path = "/model/compartment/##"
    assert( dsolve.numPools == 4 )
    a.vec.concInit = concA
    b.vec.concInit = concA / 5.0
    c.vec.concInit = concA
    d.vec.concInit = concA / 5.0
    for i in range( num ):
        d.vec[i].concInit = concA * 2 * i / num

def test_diffusion_gsolve_dsolve():
    """
    This example illustrates how to set up a diffusion/transport model with
    a simple reaction-diffusion system in a tapering cylinder:

    | Molecule **a** diffuses with diffConst of 10e-12 m^2/s.
    | Molecule **b** diffuses with diffConst of 5e-12 m^2/s.
    | Molecule **b** also undergoes motor transport with a rate of 10e-6 m/s
    |   Thus it 'piles up' at the end of the cylinder.
    | Molecule **c** does not move: diffConst = 0.0
    | Molecule **d** does not move: diffConst = 10.0e-12 but it is buffered.
    |   Because it is buffered, it is treated as non-diffusing.

    All molecules other than **d** start out only in the leftmost (first)
    voxel, with a concentration of 1 mM. **d** is present throughout
    at 0.2 mM, except in the last voxel, where it is at 1.0 mM.

    The cylinder has a starting radius of 2 microns, and end radius of
    1 micron. So when the molecule undergoing motor transport gets to the
    narrower end, its concentration goes up.

    There is a little reaction in all compartments: ``b + d <===> c``

    As there is a high concentration of **d** in the last compartment,
    when the molecule **b** reaches the end of the cylinder, the reaction
    produces lots of **c**.

    Note that molecule **a** does not participate in this reaction.

    The concentrations of all molecules are displayed in an animation.
    """
    runtime = 20.0
    diffdt = 0.005
    plotdt = 0.1
    makeModel()
    # Set up clocks. The dsolver to know before assigning stoich
    moose.setClock( 10, diffdt ) # 10 is the standard clock for Dsolve.
    moose.setClock( 16, plotdt ) # 16 is the standard clock for Ksolve.

    a = moose.element( '/model/compartment/a' )
    b = moose.element( '/model/compartment/b' )
    c = moose.element( '/model/compartment/c' )
    d = moose.element( '/model/compartment/d' )

    moose.reinit()
    atot = sum( a.vec.n )
    btot = sum( b.vec.n )
    ctot = sum( c.vec.n )
    dtot = sum( d.vec.n )
    for t in np.arange( 0, runtime, plotdt ):
        moose.start( plotdt )

    atot2 = sum( a.vec.n )
    btot2 = sum( b.vec.n )
    ctot2 = sum( c.vec.n )
    dtot2 = sum( d.vec.n )

    msg = 'Ratio of initial to final total numbers of '
    got = np.array((atot2/atot, btot2/btot, ctot2/ctot, dtot2/dtot))
    msg += 'a=%f b=%f, c=%f, d=%f' % (tuple(got))
    print(msg)
    print('Initial to final (b+c)=%f' % (float(btot2 + ctot2) / (btot + ctot )))
    expected = np.array((1.00003087, 1.39036644, 0.92191184, 1.11427514))
    error = got - expected
    rerror = np.abs( error ) / expected
    assert np.allclose(got, expected, atol=1e-3), "Got %s, expected %s" % (got, expected)

def main():
    test_diffusion_gsolve_dsolve()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
