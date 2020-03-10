# -*- coding: utf-8 -*-
from __future__ import print_function, division

# This example demonstrates random (Poisson) synaptic input to a cell.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3. No warranty.
# Changelog:
# Thursday 20 September 2018 09:53:27 AM IST
# - Turned into a test script. Dilawar Singh <dilawars@ncbs.res.in>

import moose
import numpy as np

moose.seed( 100 )

try:
    import matplotlib 
except Exception as e:
    print( "[INFO ] matplotlib is not found. This test wont run." )
    quit()

import rdesigneur as rd

def test_rdes():
    rdes = rd.rdesigneur(
        cellProto = [['somaProto', 'soma', 20e-6, 200e-6]],
        chanProto = [['make_glu()', 'glu']],
        chanDistrib = [['glu', 'soma', 'Gbar', '1' ]],
        stimList = [['soma', '0.5', 'glu', 'randsyn', '50' ]],
        # Deliver stimulus to glu synapse on soma, at mean 50 Hz Poisson.
        plotList = [['soma', '1', '.', 'Vm', 'Soma membrane potential']]
    )
    rdes.buildModel()
    moose.reinit()
    moose.start( 0.3 )
    tables = moose.wildcardFind( '/##[TYPE=Table]' )
    res = [ ]
    for t in tables:
        y = t.vector
        u, s = np.mean(y), np.std(y)
        res.append( (u,s) )


    # Got these values from version compiled on Sep 20, 2018 with moose.seed
    # set to 100.
    expected = [(-0.051218660048699974, 0.01028490481294165)]
    assert np.isclose( expected, res, atol=1e-5).all(), "Expected %s, got %s" %(expected,res)

def main():
    test_rdes()

if __name__ == '__main__':
    main()
