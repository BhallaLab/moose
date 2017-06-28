# test_singlecomp.py --- 
# 
# Filename: test_singlecomp.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Tue Jul 17 21:01:14 2012 (+0530)
# Version: 
# Last-Updated: Sun Jun 25 15:37:21 2017 (-0400)
#           By: subha
#     Update #: 320
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Test the ion channels with a single compartment.
# 
# 

# Change log:
# 
# 2012-07-17 22:22:23 (+0530) Tested NaF2 and NaPF_SS against neuron
# test case.
# 
#

# Code:


import os
os.environ['NUMPTHREADS'] = '1'
import uuid
import unittest
from datetime import datetime
import sys
sys.path.append('../../../python')

import numpy as np
from matplotlib import pyplot as plt


import moose
from testutils import *
from nachans import *
from kchans import *
from archan import *
from cachans import *
from capool import *

simdt = 0.25e-4
plotdt = 0.25e-4
simtime = 350e-3

erev = {
    'K': -100e-3,
    'Na': 50e-3,
    'Ca': 125e-3,
    'AR': -40e-3
    }

channel_density = {
    'NaF2':     1500.0,
    'NaPF_SS':  1.5,
    'KDR_FS':   1000.0,
    'KC_FAST':  100.0,
    'KA':       300.0,
    'KM':       37.5,
    'K2':       1.0,
    'KAHP_SLOWER':      1.0,
    'CaL':      5.0,
    'CaT_A':    1.0,
    'AR':       2.5
}

compartment_propeties = {
    'length': 20e-6,
    'diameter': 2e-6 * 7.5,
    'initVm': -65e-3,
    'Em': -65e-3,
    'Rm': 5.0,
    'Cm': 9e-3,
    'Ra': 1.0,
    'specific': True}

stimulus = [[100e-3, 50e-3, 3e-10], # delay[0], width[0], level[0]
            [1e9, 0, 0]]

def create_compartment(path, length, diameter, initVm, Em, Rm, Cm, Ra, specific=False):
    comp = moose.Compartment(path)
    comp.length = length
    comp.diameter = diameter
    comp.initVm = initVm
    comp.Em = Em
    if not specific:
        comp.Rm = Rm
        comp.Cm = Cm
        comp.Ra = Ra
    else:
        sarea = np.pi * length * diameter
        comp.Rm = Rm / sarea
        comp.Cm = Cm * sarea
        comp.Ra = 4.0 * Ra * length / (np.pi * diameter * diameter)
    return comp

def insert_channel(compartment, channeclass, gbar, density=False):
    channel = moose.copy(channeclass.prototype, compartment)[0]
    if not density:
        channel.Gbar = gbar
    else:
        channel.Gbar = gbar * np.pi * compartment.length * compartment.diameter
    moose.connect(channel, 'channel', compartment, 'channel')
    return channel

def insert_ca(compartment, phi, tau):
    ca = moose.copy(CaPool.prototype, compartment)[0]
    ca.B = phi / (np.pi * compartment.length * compartment.diameter)
    ca.tau = tau
    print( ca.path, ca.B, ca.tau)
    for chan in moose.wildcardFind('%s/#[TYPE=HHChannel]' % (compartment.path)):
        if chan.name.startswith('KC') or chan.name.startswith('KAHP'):
            moose.connect(ca, 'concOut', chan, 'concen')
        elif chan.name.startswith('CaL'):
            moose.connect(chan, 'IkOut', ca, 'current')
        else:
            continue
        moose.showfield(chan)
    return ca

class TestSingleComp(unittest.TestCase):
    def setUp(self):
        self.testId = uuid.uuid4().int
        self.container = moose.Neutral('test%d' % (self.testId))
        self.model = moose.Neutral('%s/model' % (self.container.path))
        self.data = moose.Neutral('%s/data' % (self.container.path))
        self.soma = create_compartment('%s/soma' % (self.model.path),
                                       **compartment_propeties)
        self.tables = {}
        tab = moose.Table('%s/Vm' % (self.data.path))
        self.tables['Vm'] = tab
        moose.connect(tab, 'requestOut', self.soma, 'getVm')
        for channelname, conductance in list(channel_density.items()):
            chanclass = eval(channelname)
            channel = insert_channel(self.soma, chanclass, conductance, density=True)
            if issubclass(chanclass, KChannel):
                channel.Ek = erev['K']
            elif issubclass(chanclass, NaChannel):
                channel.Ek = erev['Na']
            elif issubclass(chanclass, CaChannel):
                channel.Ek = erev['Ca']
            elif issubclass(chanclass, AR):
                channel.Ek = erev['AR']
            tab = moose.Table('%s/%s' % (self.data.path, channelname))
            moose.connect(tab, 'requestOut', channel, 'getGk')
            self.tables['Gk_'+channel.name] = tab
        archan = moose.HHChannel(self.soma.path + '/AR')
        archan.X = 0.0
        ca = insert_ca(self.soma, 2.6e7, 50e-3)
        tab = moose.Table('%s/Ca' % (self.data.path))
        self.tables['Ca'] = tab
        moose.connect(tab, 'requestOut', ca, 'getCa')
        self.pulsegen = moose.PulseGen('%s/inject' % (self.model.path))
        moose.connect(self.pulsegen, 'output', self.soma, 'injectMsg')
        tab = moose.Table('%s/injection' % (self.data.path))
        moose.connect(tab, 'requestOut', self.pulsegen, 'getOutputValue')
        self.tables['pulsegen'] = tab
        self.pulsegen.count = len(stimulus)
        for ii in range(len(stimulus)):
            self.pulsegen.delay[ii] = stimulus[ii][0]
            self.pulsegen.width[ii] = stimulus[ii][1]
            self.pulsegen.level[ii] = stimulus[ii][2]
        setup_clocks(simdt, plotdt)
        assign_clocks(self.model, self.data)        
        moose.reinit()
        start = datetime.now()
        moose.start(simtime)
        end = datetime.now()
        delta = end - start
        print( 'Simulation of %g s finished in %g s' % (simtime, delta.seconds + delta.microseconds*1e-6))


    def testDefault(self):
        vm_axis = plt.subplot(2,1,1)
        ca_axis = plt.subplot(2,1,2)
        try:
            fname = os.path.join(config.mydir, 'nrn', 'data', 'singlecomp_Vm.dat')
            nrndata = np.loadtxt(fname)
            vm_axis.plot(nrndata[:,0], nrndata[:,1], label='Vm (mV) - nrn')
            ca_axis.plot(nrndata[:,0], nrndata[:,2], label='Ca (mM) - nrn')
        except IOError as e:
            print(e)
        tseries = np.linspace(0, simtime, len(self.tables['Vm'].vector)) * 1e3
        # plotcount = len(channel_density) + 1
        # rows = int(np.sqrt(plotcount) + 0.5)
        # columns = int(plotcount * 1.0/rows + 0.5)
        # print plotcount, rows, columns
        # plt.subplot(rows, columns, 1)
        vm_axis.plot(tseries, self.tables['Vm'].vector * 1e3, label='Vm (mV) - moose')
        vm_axis.plot(tseries, self.tables['pulsegen'].vector * 1e12, label='inject (pA)')
        ca_axis.plot(tseries, self.tables['Ca'].vector, label='Ca (mM) - moose')
        vm_axis.legend()
        ca_axis.legend()
        # ii = 2
        # for key, value in self.tables.items():
        #     if key.startswith('Gk'):
        #         plt.subplot(rows, columns, ii)
        #         plt.plot(tseries, value.vector, label=key)                
        #         ii += 1
        #         plt.legend()
        plt.show()
        data = np.vstack((tseries*1e-3, 
                          self.tables['Vm'].vector, 
                          self.tables['Ca'].vector))
        np.savetxt(os.path.join(config.data_dir, 'singlecomp_Vm.dat'), 
                   np.transpose(data))

if __name__ == '__main__':
    unittest.main()
    
# 
# test_singlecomp.py ends here
