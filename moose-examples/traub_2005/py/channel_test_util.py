# channel_test_util.py
# 
# Filename: channel_test_util.py
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
# Utility functions for testing single channel models
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
import moose
import channelbase
import testutils

def run_single_channel(channelname, Gbar, simtime, simdt=testutils.SIMDT, plotdt=testutils.PLOTDT):
    testId = uuid.uuid4().int
    container = moose.Neutral('test%d' % (testId))
    model_container = moose.Neutral('%s/model' % (container.path))
    data_container = moose.Neutral('%s/data' % (container.path))
    params = testutils.setup_single_compartment(
        model_container, data_container,
        channelbase.prototypes[channelname],
        Gbar)
    vm_data = params['Vm']
    gk_data = params['Gk']
    ik_data = params['Ik']
    testutils.setup_clocks(simdt, plotdt)
    testutils.assign_clocks(model_container, data_container)
    moose.reinit()
    print 'Starting simulation', testId, 'for', simtime, 's'
    moose.start(simtime)
    print 'Finished simulation'
    vm_file = 'data/%s_Vm.dat' % (channelname)
    gk_file = 'data/%s_Gk.dat' % (channelname)
    ik_file = 'data/%s_Ik.dat' % (channelname)
    tseries = np.array(range(len(vm_data.vector))) * simdt
    print 'Vm:', len(vm_data.vector), 'Gk', len(gk_data.vector), 'Ik', len(ik_data.vector)
    data = np.c_[tseries, vm_data.vector]
    np.savetxt(vm_file, data)
    print 'Saved Vm in', vm_file
    print len(gk_data.vector), len(vm_data.vector)
    data = np.c_[tseries, gk_data.vector]
    np.savetxt(gk_file, data)
    print 'Saved Gk in', gk_file
    data = np.c_[tseries, ik_data.vector]
    np.savetxt(ik_file, data)
    print 'Saved Gk in', ik_file
    return params

def compare_channel_data(series, channelname, param, simulator, x_range=None, plot=False):
    if simulator == 'moose':
        ref_file = 'testdata/%s_%s.dat.gz' % (channelname, param)
    elif simulator == 'neuron':
        ref_file = '../nrn/data/%s_%s.dat.gz' % (channelname, param)
    else:
        raise ValueError('Unrecognised simulator: %s' % (simulator))
    try:
        ref_series = np.loadtxt(ref_file)
    except IOError, e:
        print e
        return -1.0
    if plot:
        plt.figure()
        plt.title(channelname)
    return testutils.compare_data_arrays(ref_series, series, relative='meany', x_range=x_range, plot=plot)

class ChannelTestBase(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        unittest.TestCase.__init__(self, *args, **kwargs)

# 
# channel_test_util.py ends here
