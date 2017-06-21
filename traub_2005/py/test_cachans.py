# test_cachans.py --- 
# 
# Filename: test_cachans.py
# Description: 
# Author: 
# Maintainer: 
# Created: Sat Jun  2 11:07:05 2012 (+0530)
# Version: 
# Last-Updated: Sat Jun  2 15:27:15 2012 (+0530)
#           By: subha
#     Update #: 9
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

import numpy as np
import testutils
from testutils import *
from cachans import *
from channel_test_util import ChannelTestBase, run_single_channel, compare_channel_data

simtime = 350e-3
simdt = testutils.SIMDT
plotdt = testutils.PLOTDT

class TestCaL(ChannelTestBase):
    channelname = 'CaL'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testCAL_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)

    def testCAL_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)


class TestCaT(ChannelTestBase):
    channelname = 'CaT'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testCaT_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)

    def testCaT_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)


class TestCaT_A(ChannelTestBase):
    channelname = 'CaT_A'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.array(params['Vm'].vector)
    gk = np.array(params['Gk'].vector)
    tseries = np.arange(0, len(vm), 1.0) * simdt
    
    def testCaT_Vm_Neuron(self):
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)

    def testCaT_Gk_Neuron(self):
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.01)



if __name__ == '__main__':
    unittest.main()


# 
# test_cachans.py ends here
