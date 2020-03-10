# -*- coding: utf-8 -*-
# Test scripts are always called from BUILD directory by cmake.

import os
import sys
import numpy as np
import time
import test_difshells as td
import chan_proto
import param_chan

import moose
print('Using moose from %s' % moose.__file__ )

difshell_no = 3
difbuf_no = 0

script_dir_ = os.path.dirname( os.path.abspath( __file__ ) )

p_file = os.path.join( script_dir_, "soma.p" )

cond = {'CaL12':30*0.35e-5, 'SK':0.5*0.35e-6}

def assert_stat( vec, expected ):
    min_, max_ = np.min( vec ), np.max( vec )
    mean, std = np.mean( vec ), np.std( vec )
    computed = [ min_, max_, mean, std ]
    assert np.allclose( computed, expected ),  \
        "Got %s expected %s" % (computed, expected)

def test_hsolve_calcium():
    for tick in range(0, 7):
        moose.setClock(tick,10e-6)
    moose.setClock(8, 0.005)

    lib = moose.Neutral('/library')
    model = moose.loadModel(p_file,'neuron')
    pulse = moose.PulseGen('/neuron/pulse')
    inject = 100e-10
    chan_proto.chan_proto('/library/SK',param_chan.SK)
    chan_proto.chan_proto('/library/CaL12',param_chan.Cal)
    pulse.delay[0] = 8.
    pulse.width[0] = 500e-12
    pulse.level[0] = inject
    pulse.delay[1] = 1e9

    for comp in moose.wildcardFind('/neuron/#[TYPE=Compartment]'):
        new_comp = moose.element(comp)
        new_comp.initVm = -.08
        difs, difb = td.add_difshells_and_buffers(new_comp,difshell_no,difbuf_no)
        for name in cond:
            chan = td.addOneChan(name,cond[name],new_comp)
            if 'Ca' in name:
                moose.connect(chan, "IkOut", difs[0], "influx")

            if 'SK' in name:
                moose.connect(difs[0], 'concentrationOut', chan, 'concen')

    data = moose.Neutral('/data')
    vmtab = moose.Table('/data/Vm')
    shelltab = moose.Table('/data/Ca')
    caltab = moose.Table('/data/CaL_Gk')
    sktab = moose.Table('/data/SK_Gk')
    moose.connect(vmtab, 'requestOut',moose.element('/neuron/soma') , 'getVm')
    moose.connect(shelltab, 'requestOut', difs[0], 'getC')
    moose.connect(caltab,'requestOut',moose.element('/neuron/soma/CaL12') ,'getGk')
    moose.connect(sktab,'requestOut', moose.element('/neuron/soma/SK'),'getGk')

    hsolve = moose.HSolve('/neuron/hsolve')
    hsolve.dt = 10e-6
    hsolve.target = ('/neuron/soma')
    t_stop = 10.
    moose.reinit()
    moose.start(t_stop)
    vec1 = sktab.vector
    vec2 = shelltab.vector
    assert_stat( vec1, [ 0.0, 5.102834e-22, 4.79066e-22, 2.08408e-23 ] )
    assert_stat( vec2, [ 5.0e-5, 5.075007e-5, 5.036985e-5, 2.1950117e-7] )
    assert len(np.where(sktab.vector<1e-19)[0]) == 2001
    assert len(np.where(shelltab.vector>50e-6)[0]) == 2000

def main():
    test_hsolve_calcium()

if __name__ == '__main__':
    main()
