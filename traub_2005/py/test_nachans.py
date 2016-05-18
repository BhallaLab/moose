# test_nachans.py --- 
# 
# Filename: test_nachans.py
# Description: 
# Author: 
# Maintainer: 
# Created: Sat May 26 10:29:41 2012 (+0530)
# Version: 
# Last-Updated: Wed May 30 21:47:11 2012 (+0530)
#           By: subha
#     Update #: 349
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Unit tests for single Na channels
# 
# 

# Change log:
# 
# 
# 
# 

# Code:

import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python')
import numpy as np
import testutils
from testutils import *
from channel_test_util import ChannelTestBase, run_single_channel, compare_channel_data
import nachans

simtime = 350e-3
simdt = testutils.SIMDT
plotdt = testutils.PLOTDT
        
        
class TestNaF(ChannelTestBase):
    channelname = 'NaF'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt

    def testNaF_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaF.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaF_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaF.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaF_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaF_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

class TestNaF_TCR(ChannelTestBase):
    channelname = 'NaF_TCR'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt
    def testNaF_TCR_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaF_TCR.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaF_TCR_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaF_TCR.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaF_TCR_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaF_TCR_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.05)
        print 'OK'


class TestNaF2(ChannelTestBase):
    channelname = 'NaF2'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt
    def testNaF2_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaF2.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaF2_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaF2.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaF2_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaF2_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.05)
        print 'OK'


class TestNaF2_nRT(ChannelTestBase):
    channelname = 'NaF2_nRT'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt
    def testNaF2_nRT_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaF2_nRT.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaF2_nRT_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaF2_nRT.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaF2_nRT_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaF2_nRT_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.05)
        print 'OK'

class TestNaP(ChannelTestBase):
    channelname = 'NaP'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt
    def testNaP_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaP.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaP_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaP.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaP_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaP_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.05)
        print 'OK'


class TestNaPF(ChannelTestBase):
    channelname = 'NaPF'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt
    def testNaPF_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaPF.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaPF_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaPF.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaPF_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaPF_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.05)
        print 'OK'


class TestNaPF_SS(ChannelTestBase):
    channelname = 'NaPF_SS'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt
    def testNaPF_SS_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaPF_SS.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaPF_SS_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaPF_SS.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaPF_SS_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaPF_SS_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.05)
        print 'OK'


class TestNaPF_TCR(ChannelTestBase):
    channelname = 'NaPF_TCR'
    params = run_single_channel(channelname, 1e-9, simtime)
    vm = np.asarray(params['Vm'].vector)        
    gk = np.asarray(params['Gk'].vector)
    tseries = np.array(range(0, len(params['Vm'].vector))) * simdt
    def testNaPF_TCR_Vm_Moose(self):
        print 'Testing MOOSE Vm  ...',
        err = compare_channel_data(self.vm, TestNaPF_TCR.channelname, 'Vm', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)
        print 'OK'
        
    def testNaPF_TCR_Gk_Moose(self):
        print 'Testing MOOSE Gk  ...',
        err = compare_channel_data(self.gk, TestNaPF_TCR.channelname, 'Gk', 'moose', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.05)
        print 'OK'

    def testNaPF_TCR_Vm_Neuron(self):
        print 'Testing NEURON Vm  ...',
        data = np.c_[self.tseries, self.vm]
        err = compare_channel_data(data, self.channelname, 'Vm', 'neuron', x_range=(simtime/10.0, simtime))
        self.assertLess(err, 0.01)        
        print 'OK'

    def testNaPF_TCR_Gk_Neuron(self):
        print 'Testing NEURON Gk  ...',
        data = np.c_[self.tseries, self.gk]
        err = compare_channel_data(data, self.channelname, 'Gk', 'neuron', x_range=(simtime/10.0, simtime), plot=True)
        self.assertLess(err, 0.05)
        print 'OK'


if __name__ == '__main__':
    unittest.main()
    # suite = unittest.TestLoader().loadTestsFromTestCase(TestNaPF)
    # unittest.TextTestRunner(verbosity=2).run(suite)
    
#
# test_nachans.py ends here
