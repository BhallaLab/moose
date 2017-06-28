# test_capool.py --- 
# 
# Filename: test_capool.py
# Description: 
# Author: 
# Maintainer: 
# Created: Sun Jun  3 20:31:03 2012 (+0530)
# Version: 
# Last-Updated: Sun Jun 25 15:48:08 2017 (-0400)
#           By: subha
#     Update #: 76
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
# 

# Code:

import os
import uuid
import numpy as np
import testutils
from testutils import *
from cachans import *
from capool import *
from channel_test_util import compare_channel_data, run_single_channel, ChannelTestBase


simtime = 350e-3
simdt = testutils.SIMDT
plotdt = testutils.PLOTDT


def run_capool(poolname, Gbar, simtime):
    testId = uuid.uuid4().int
    container = moose.Neutral('test%d' % (testId))
    model = moose.Neutral('%s/model' % (container.path))
    data = moose.Neutral('%s/data' % (container.path))
    params = setup_single_compartment(
        model, data,
        channelbase.prototypes['CaL'],
        Gbar)
    channelname = 'CaL'
    capool = moose.copy(CaPool.prototype, params['compartment'], 'CaPool')[0]
    moose.connect(params['channel'], 'IkOut', capool, 'current')
    # The B is obtained from phi in NEURON by dividing it with
    # compartment area in cm2 and multiplying by 1e3 for /mA->/A and
    # by 1e3 for /ms->/S
    capool.B = 52000 * 1e6 / (3.141592 * 1e-4 * 1e-4)
    # beta = 1/tau (ms) = 0.02 => tau = 50 ms
    capool.tau = 50e-3
    ca_data = moose.Table('%s/Ca' % (data.path))
    moose.connect(ca_data, 'requestOut', capool, 'getCa')
    setup_clocks(simdt, plotdt)
    assign_clocks(model, data)
    vm_data = params['Vm']
    gk_data = params['Gk']
    ik_data = params['Ik']
    params['Ca'] = ca_data
    moose.reinit()
    print('Starting simulation', testId, 'for', simtime, 's')
    moose.start(simtime)
    print('Finished simulation')
    vm_file = os.path.join(config.data_dir, '%s_Vm.dat' % (poolname))
    gk_file = os.path.join(config.data_dir, '%s_Gk.dat' % (poolname))
    ik_file = os.path.join(config.data_dir, '%s_Ik.dat' % (poolname))
    ca_file = os.path.join(config.data_dir, '%s_Ca.dat' % (poolname))
    tseries = np.array(list(range(len(vm_data.vector)))) * simdt
    print(('Vm:', len(vm_data.vector), 'Gk', len(gk_data.vector), 'Ik', len(ik_data.vector)))
    data = np.c_[tseries, vm_data.vector]
    np.savetxt(vm_file, data)
    print(('Saved Vm in', vm_file))
    data = np.c_[tseries, gk_data.vector]
    np.savetxt(gk_file, data)
    print(('Saved Gk in', gk_file))
    data = np.c_[tseries, ik_data.vector]
    np.savetxt(ik_file, data)
    print(('Saved Ik in', ik_file))
    print(('>>', len(ca_data.vector)))
    data = np.c_[tseries, ca_data.vector]
    np.savetxt(ca_file, data)
    print(('Saved [Ca2+] in', ca_file))
    return params
    

class TestCaPool(ChannelTestBase):
    channelname = 'CaL'
    poolname = 'CaPool'    
    params = run_capool(poolname, 1e-9, 350e-3)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    ca = np.array(params['Ca'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testCaPool_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        try:
            err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
            self.assertLess(err, 0.01)
        except IOError:
            print('Could not find NRN data')


    def testCaPool_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        try:
            err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
            self.assertLess(err, 0.01)
        except IOError:
            print('Could not find NRN data')
        
    def testCaPool_Ca_Neuron(self):
        print(self.ca.shape)
        data = np.c_[self.tseries, self.ca]
        try:
            err = compare_channel_data(data, self.poolname, 'Ca', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
            self.assertLess(err, 0.01)
        except IOError:
            print('Could not find NRN data')

if __name__ == '__main__':
    unittest.main()

# 
# test_capool.py ends here
