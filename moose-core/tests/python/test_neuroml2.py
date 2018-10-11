# -*- coding: utf-8 -*-
# test_neurom2.py, modified from run_cell.py
# Maintainer: P Gleeson, Dilawar Singh
# This test is not robust.
# Code:

from __future__ import absolute_import, print_function, division

# check if neuroml working properly.
# NOTE: This script does not work with python3
# See https://github.com/NeuroML/NeuroML2/issues/116 . If this bug is fixed then
# remove this code block.
import neuroml as nml
a = nml.nml.nml.IonChannel()
try:
    b = {a : 1 }
except TypeError as e:
    print( 'Failed due to https://github.com/NeuroML/NeuroML2/issues/116' )
    quit( 0 )

import moose
import moose.utils as mu
import sys
import os
import numpy as np

SCRIPT_DIR = os.path.dirname( os.path.realpath( __file__ ) )


def run( nogui = True ):
    global SCRIPT_DIR
    filename = os.path.join(SCRIPT_DIR, 'test_files/passiveCell.nml' )
    mu.info('Loading: %s' % filename )
    nml = moose.mooseReadNML2( filename )
    if not nml:
        mu.warn( "Failed to parse NML2 file" )
        return

    assert nml, "Expecting NML2 object"
    msoma = nml.getComp(nml.doc.networks[0].populations[0].id,0,0)
    data = moose.Neutral('/data')
    pg = nml.getInput('pulseGen1')

    inj = moose.Table('%s/pulse' % (data.path))
    moose.connect(inj, 'requestOut', pg, 'getOutputValue')

    vm = moose.Table('%s/Vm' % (data.path))
    moose.connect(vm, 'requestOut', msoma, 'getVm')

    simtime = 150e-3
    moose.reinit()
    moose.start(simtime)
    print("Finished simulation!")
    t = np.linspace(0, simtime, len(vm.vector))
    yvec = vm.vector
    injvec = inj.vector * 1e12
    m1, u1 = np.mean( yvec ), np.std( yvec )
    m2, u2 = np.mean( injvec ), np.std( injvec )
    assert np.isclose( m1, -0.0456943 ), m1
    assert np.isclose( u1, 0.0121968 ), u1
    assert np.isclose( m2, 26.64890 ), m2
    assert np.isclose( u2, 37.70607574 ), u2
    quit( 0 )

if __name__ == '__main__':
    run( )
