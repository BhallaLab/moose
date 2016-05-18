# test_kchans.py --- 
# 
# Filename: test_kchans.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed May 30 23:51:58 2012 (+0530)
# Version: 
# Last-Updated: Sun Jun  3 20:07:30 2012 (+0530)
#           By: subha
#     Update #: 115
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 

# Code:

import uuid
import unittest
import numpy as np
import testutils 
import channelbase
from testutils import setup_single_compartment
from channel_test_util import compare_channel_data, run_single_channel, ChannelTestBase
from kchans import *

simtime = 350e-3
simdt = testutils.SIMDT
plotdt = testutils.PLOTDT


class TestKDR(ChannelTestBase):
    channelname = 'KDR'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKDR_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)

    def testKDR_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)

        
class TestKDR_FS(ChannelTestBase):
    channelname = 'KDR_FS'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKDR_FS_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testKDR_FS_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)
        

        
class TestKA(ChannelTestBase):
    channelname = 'KA'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKA_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testKA_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', plot=True)
        self.assertLess(err, 0.01)
        

class TestKA_IB(ChannelTestBase):
    channelname = 'KA_IB'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKA_IB_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        

    def testKA_IB_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', plot=True)
        self.assertLess(err, 0.01)
        
        
class TestK2(ChannelTestBase):
    channelname = 'K2'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testK2_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testK2_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', plot=True)
        self.assertLess(err, 0.01)
        
        
class TestKM(ChannelTestBase):
    channelname = 'KM'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKM_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)

    def testKM_Gk_Neuron(self):
        
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)


def setup_cadep_channel(model_container, data_container, channel_proto, Gbar, ca_start, ca_stop):
    """Setup a test compartment with [Ca2+] dependent channel."""
    params = setup_single_compartment(model_container, data_container, channel_proto, Gbar)
    ca_table = moose.StimulusTable(model_container.path + '/CaStim')    
    ca_table.vector = np.linspace(ca_start, ca_stop, 1000)
    ca_table.doLoop = True
    ca_recorder = moose.Table(data_container.path + '/Ca')
    moose.connect(ca_table, 'output', ca_recorder, 'input')
    moose.connect(ca_table, 'output', params['channel'], 'concen')
    params['Ca'] = ca_recorder
    params['CaStim'] = ca_table
    testutils.setup_clocks(simdt, plotdt)
    testutils.assign_clocks(model_container, data_container)    
    moose.useClock(1, '%s,%s' % (ca_recorder.path, ca_table.path), 'process')
    return params

def run_cadep_channel(channelname, Gbar, simtime):
    testId = uuid.uuid4().int
    container = moose.Neutral('test%d' % (testId))
    model_container = moose.Neutral('%s/model' % (container.path)) 
    data_container = moose.Neutral('%s/data' % (container.path))
    params = setup_cadep_channel(
        model_container, data_container,
        channelbase.prototypes[channelname],
        Gbar,
        0,
        500.0)
    ca_table = params['CaStim']
    ca_table.startTime = 0.0
    ca_table.stopTime = 175e-3
    vm_data = params['Vm']
    gk_data = params['Gk']
    ik_data = params['Ik']
    ca_data = params['Ca']
    moose.reinit()
    print 'Starting simulation', testId, 'for', simtime, 's'
    moose.start(simtime)
    print 'Finished simulation'
    vm_file = 'data/%s_Vm.dat' % (channelname)
    gk_file = 'data/%s_Gk.dat' % (channelname)
    ik_file = 'data/%s_Ik.dat' % (channelname)
    ca_file = 'data/%s_Ca.dat' % (channelname)
    tseries = np.array(range(len(vm_data.vector))) * simdt
    print 'Vm:', len(vm_data.vector), 'Gk', len(gk_data.vector), 'Ik', len(ik_data.vector)
    data = np.c_[tseries, vm_data.vector]
    np.savetxt(vm_file, data)
    print 'Saved Vm in', vm_file
    data = np.c_[tseries, gk_data.vector]
    np.savetxt(gk_file, data)
    print 'Saved Gk in', gk_file
    data = np.c_[tseries, ik_data.vector]
    np.savetxt(ik_file, data)
    print 'Saved Ik in', ik_file
    np.savetxt(ca_file, data)
    print 'Saved [Ca2+] in', ca_file
    return params

        
class TestKAHP(ChannelTestBase):
    channelname = 'KAHP'
    params = run_cadep_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKAHP_Vm_Neuron(self):        
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testKAHP_Gk_Neuron(self):        
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)
        
    
class TestKAHP_SLOWER(ChannelTestBase):
    channelname = 'KAHP_SLOWER'
    params = run_cadep_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKAHP_SLOWER_Vm_Neuron(self):        
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testKAHP_SLOWER_Gk_Neuron(self):        
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)
        

class TestKAHP_DP(ChannelTestBase):
    channelname = 'KAHP_DP'
    params = run_cadep_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKAHP_DP_Vm_Neuron(self):        
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testKAHP_DP_Gk_Neuron(self):        
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)

        
class TestKC(ChannelTestBase):
    channelname = 'KC'
    params = run_cadep_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKC_Vm_Neuron(self):        
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testKC_Gk_Neuron(self):        
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)


class TestKC_FAST(ChannelTestBase):
    channelname = 'KC_FAST'
    params = run_cadep_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testKC_FAST_Vm_Neuron(self):        
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        
    def testKC_FAST_Gk_Neuron(self):        
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)
        

if __name__ == '__main__':
    unittest.main()
    
# 
# test_kchans.py ends here
