# compartment_net_no_array.py --- 
# 
# Filename: compartment_net_no_array.py
# Description: 
# Author:Subhasis Ray 
# Maintainer: 
# Created: Sat Aug 11 14:30:21 2012 (+0530)
# Version: 
# Last-Updated: Tue May  7 18:26:26 2013 (+0530)
#           By: subha
#     Update #: 974
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# A demo to create a network of single compartmental neurons connected
# via alpha synapses. This is same as compartment_net.py except that
# we avoid ematrix and use single melements.
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:

import sys
sys.path.append('../../python')
import os
os.environ['NUMPTHREADS'] = '1'
import numpy as np
import matplotlib.pyplot as plt

import moose
from moose import utils

EREST_ACT = -70e-3

# Gate equations have the form:
#
# y(x) = (A + B * x) / (C + exp((x + D) / F))
# 
# where x is membrane voltage and y is the rate constant for gate
# closing or opening

Na_m_params = [1e5 * (25e-3 + EREST_ACT),   # 'A_A':
                -1e5,                       # 'A_B':
                -1.0,                       # 'A_C':
                -25e-3 - EREST_ACT,         # 'A_D':
               -10e-3,                      # 'A_F':
                4e3,                     # 'B_A':
                0.0,                        # 'B_B':
                0.0,                        # 'B_C':
                0.0 - EREST_ACT,            # 'B_D':
                18e-3                       # 'B_F':    
               ]
Na_h_params = [ 70.0,                        # 'A_A':
                0.0,                       # 'A_B':
                0.0,                       # 'A_C':
                0.0 - EREST_ACT,           # 'A_D':
                0.02,                     # 'A_F':
                1000.0,                       # 'B_A':
                0.0,                       # 'B_B':
                1.0,                       # 'B_C':
                -30e-3 - EREST_ACT,        # 'B_D':
                -0.01                    # 'B_F':       
                ]        
K_n_params = [ 1e4 * (10e-3 + EREST_ACT),   #  'A_A':
               -1e4,                      #  'A_B':
               -1.0,                       #  'A_C':
               -10e-3 - EREST_ACT,         #  'A_D':
               -10e-3,                     #  'A_F':
               0.125e3,                   #  'B_A':
               0.0,                        #  'B_B':
               0.0,                        #  'B_C':
               0.0 - EREST_ACT,            #  'B_D':
               80e-3                       #  'B_F':  
               ]
VMIN = -40e-3 + EREST_ACT
VMAX = 120e-3 + EREST_ACT
VDIVS = 30000

soma_dia = 30e-6

def create_na_chan(path):
    na = moose.HHChannel('%s/na' % (path))
    na.Xpower = 3
    xGate = moose.HHGate(na.path + '/gateX')    
    xGate.setupAlpha(Na_m_params +
                      [VDIVS, VMIN, VMAX])
    na.Ypower = 1
    yGate = moose.HHGate(na.path + '/gateY')
    yGate.setupAlpha(Na_h_params + 
                      [VDIVS, VMIN, VMAX])
    na.Ek = 115e-3 + EREST_ACT
    return na

def create_k_chan(path):
    k = moose.HHChannel('%s/k' % (path))
    k.Xpower = 4.0
    xGate = moose.HHGate(k.path + '/gateX')    
    xGate.setupAlpha(K_n_params +
                      [VDIVS, VMIN, VMAX])
    k.Ek = -12e-3 + EREST_ACT
    return k

def create_compartment(path):
    comp = moose.Compartment(path)
    comp.diameter = soma_dia
    comp.Em = EREST_ACT + 10.613e-3
    comp.initVm = EREST_ACT
    sarea = np.pi * soma_dia * soma_dia
    comp.Rm = 1/(0.3e-3 * 1e4 * sarea)
    comp.Cm = 1e-6 * 1e4 * sarea
    if moose.exists('/library/na'):
        nachan = moose.element(moose.copy('/library/na', comp, 'na'))
    else:
        nachan = create_na_chan(comp.path)
    nachan.Gbar = 120e-3 * sarea * 1e4
    moose.showfield(nachan)
    moose.connect(nachan, 'channel', comp, 'channel')
    if moose.exists('/library/k'):
        kchan = moose.element(moose.copy('/library/k', comp, 'k'))
    else:
        kchan = create_k_chan(comp.path)
    kchan.Gbar = 36e-3 * sarea * 1e4
    moose.connect(kchan, 'channel', comp, 'channel')
    synchan = moose.SynChan(comp.path + '/synchan')
    synchan.Gbar = 1e-8
    synchan.tau1 = 2e-3
    synchan.tau2 = 2e-3        
    synchan.Ek = 0.0
    m = moose.connect(comp, 'channel', synchan, 'channel')
    spikegen = moose.SpikeGen(comp.path + '/spikegen')
    spikegen.threshold = 0.0
    m = moose.connect(comp, 'VmOut', spikegen, 'Vm')
    return comp

def test_compartment():
    n = moose.Neutral('/model')
    lib = moose.Neutral('/library')
    create_na_chan(lib.path)
    create_k_chan(lib.path)
    comp = create_compartment('/model/soma')
    pg = moose.PulseGen('/model/pulse')
    pg.firstDelay = 50e-3
    pg.firstWidth = 40e-3
    pg.firstLevel = 1e-9
    moose.connect(pg, 'output', comp, 'injectMsg')
    d = moose.Neutral('/data')
    vm = moose.Table('/data/Vm')
    moose.connect(vm, 'requestOut', comp, 'getVm')
    gK = moose.Table('/data/gK')
    moose.connect(gK, 'requestOut', moose.element('%s/k' % (comp.path)), 'getGk')
    gNa = moose.Table('/data/gNa')
    moose.connect(gNa, 'requestOut', moose.element('%s/na' % (comp.path)), 'getGk')
    # utils.resetSim(['/model', '/data'], 1e-6, 1e-4, simmethod='ee')
    assign_clocks(['/model'], 1e-6, 1e-4)
    simtime = 100e-3
    moose.start(simtime)
    t = np.linspace(0, simtime, len(vm.vector))
    plt.subplot(221)
    plt.title('Vm')
    plt.plot(t, vm.vector)
    plt.subplot(222)
    plt.title('Conductance')
    plt.plot(t, gK.vector, label='GK')
    plt.plot(t, gNa.vector, label='GNa')
    plt.legend()
    plt.subplot(223)
    ma = moose.element('%s/na/gateX' % (comp.path)).tableA
    mb = moose.element('%s/na/gateX' % (comp.path)).tableB
    ha = moose.element('%s/na/gateY' % (comp.path)).tableA
    hb = moose.element('%s/na/gateY' % (comp.path)).tableB
    na = moose.element('%s/k/gateX' % (comp.path)).tableA
    nb = moose.element('%s/k/gateX' % (comp.path)).tableB
    plt.plot(1/mb, label='tau_m')
    plt.plot(1/hb, label='tau_h')
    plt.plot(1/nb, label='tau_n')
    plt.legend()
    plt.subplot(224)
    plt.plot(ma/mb, label='m_inf')
    plt.plot(ha/hb, label='h_inf')
    plt.plot(na/nb, label='n_inf')
    plt.legend()
    plt.show()
    plt.close()
    
def create_population(container, size):
    """Create a population of `size` single compartmental neurons with
    Na and K channels. Also create SpikeGen objects and SynChan
    objects connected to these which can act as plug points for
    setting up synapses later."""
    path = container.path
    # Contrast this with     
    # comps = moose.vec(path+'/soma', size, 'Compartment')    
    comps = [create_compartment(path+'/soma_%d' % (ii)) for ii in range(size)]
    spikegens = []
    synchans = []
    Em = EREST_ACT + 10.613e-3
    initVm_array = [EREST_ACT] * size
    Em_array = [Em] * size
    # initVm_array = np.random.normal(EREST_ACT, np.abs(EREST_ACT) * 0.1, size)
    # Em_array = np.random.normal(Em, np.abs(Em) * 0.1, size)
    for comp, initVm, Em in zip(comps, initVm_array, Em_array):
        comp.Em = Em
        comp.initVm = initVm
        synchan = moose.element(comp.path + '/synchan')
        synchans.append(synchan)
        spikegen = moose.element(comp.path + '/spikegen')
        spikegens.append(spikegen)
    
    return {'compartment': comps,
            'spikegen': spikegens,
            'synchan': synchans}

def make_synapses(spikegen, synchan, delay=5e-3):
    """Create synapses from spikegens to synchans in a manner similar to
    OneToAll connection.

    spikegen: list of spikegen objects - these are sources of synaptic
    event messages.

    synchan: list of synchan objects - these are the targets of the
    synaptic event messages.

    delay: mean delay of synaptic transmission. Individual delays are
    normally distributed with sd=0.1*mean.

    """
    scount = len(spikegen)
    for ii, sid in enumerate(synchan): 
        s = moose.SynChan(sid)
        sh = moose.SimpleSynHandler( sid.path + "/synh" )
        moose.connect( sh, "activationOut", s, "activation" )
        sh.synapse.num = scount
        delay_list = np.random.normal(delay, delay*0.1, scount)
        # print delay_list
        for jj in range(scount): 
            sh.synapse[jj].delay = delay_list[jj]
            # Connect all spikegens to this synchan except that from
            # same compartment - we assume if parents are same the two belong to the same compartment
            if s.parent.path != spikegen[jj].parent.path:
                m = moose.connect(spikegen[jj], 'spikeOut', moose.element(sh.path + '/synapse'),  'addSpike')
            
def two_populations(size=2):
    """An example with two population connected via synapses."""
    net = moose.Neutral('network2')
    pop_a = create_population(moose.Neutral('/network2/pop_A'), size)
    pop_b = create_population(moose.Neutral('/network2/pop_B'), size)
    make_synapses(pop_a['spikegen'], pop_b['synchan'])
    make_synapses(pop_b['spikegen'], pop_a['synchan'])
    pulse = moose.PulseGen('/network2/net2_pulse')
    pulse.firstLevel = 1e-9
    pulse.firstDelay = 0.05 # disable the pulsegen
    pulse.firstWidth = 0.02
    moose.connect(pulse, 'output', pop_a['compartment'][0], 'injectMsg')
    data = moose.Neutral('/data')
    vm_a = [moose.Table('/data/net2_Vm_A_%d' % (ii)) for ii in range(size)]
    for tab, comp in zip(vm_a, pop_a['compartment']):
        moose.connect(tab, 'requestOut', comp, 'getVm')
    vm_b = [moose.Table('/data/net2_Vm_B_%d' % (ii)) for ii in range(size)]
    for tab, comp in zip(vm_b, pop_b['compartment']):
        moose.connect(tab, 'requestOut', comp, 'getVm')
    gksyn_a = [moose.Table('/data/net2_Gk_syn_a_%d' % (ii)) for ii in range(size)]
    for tab, synchan in zip(gksyn_a, pop_a['synchan']):
        moose.connect(tab, 'requestOut', synchan, 'getGk')
    gksyn_b = [moose.Table('/data/net2_Gk_syn_b_%d' % (ii)) for ii in range(size)]
    for tab, synchan in zip(gksyn_b, pop_b['synchan']):
        moose.connect(tab, 'requestOut', synchan, 'getGk')
    pulsetable = moose.Table('/data/net2_pulse')
    pulsetable.connect('requestOut', pulse, 'getOutputValue')
    return {'vm_a': vm_a,
            'vm_b': vm_b,
            'gksyn_a': gksyn_a,
            'gksyn_b': gksyn_b,
            'pulse': pulsetable,}

def single_population(size=2):
    """Example of a single population where each cell is connected to
    every other cell.

    Creates a network of single compartmental cells under /network1 and a pulse generaor

    """
    net = moose.Neutral('network1')
    pop = create_population(moose.Neutral('/network1'), size)
    make_synapses(pop['spikegen'], pop['synchan'])
    pulse = moose.PulseGen('/network1/net1_pulse')
    pulse.firstLevel = 1e-9
    pulse.firstDelay = 0.05
    pulse.firstWidth = 0.02
    moose.connect(pulse, 'output', pop['compartment'][0], 'injectMsg')
    data = moose.Neutral('/data')
    vm = [moose.Table('/data/net1_Vm_%d' % (ii)) for ii in range(size)]
    for tab, comp in zip(vm, pop['compartment']):
        moose.connect(tab, 'requestOut', comp, 'getVm')
    gksyn = [moose.Table('/data/net1_Gk_syn_%d' % (ii)) for ii in range(size)]
    for tab, synchan in zip(gksyn, pop['synchan']):
        moose.connect(tab, 'requestOut', synchan, 'getGk')
    pulsetable = moose.Table('/data/net1_pulse')
    pulsetable.connect('requestOut', pulse, 'getOutputValue')
    return {'vm': vm,
            'gksyn': gksyn,
            'pulse': pulsetable,}

inited = False
def assign_clocks(model_container_list, simdt, plotdt):
    """Assign clocks to elements under the listed paths.
    
    This should be called only after all model components have been
    created. Anything created after this will not be scheduled.

    """
    global inited
    # `inited` is for avoiding double scheduling of the same object
    if not inited:
        print 'SimDt=%g, PlotDt=%g' % (simdt, plotdt)
        moose.setClock(0, simdt)
        moose.setClock(1, simdt)
        moose.setClock(2, simdt)
        moose.setClock(3, simdt)
        moose.setClock(4, plotdt)
        for path in model_container_list:
            print 'Scheduling elements under:', path
            moose.useClock(0, '%s/##[TYPE=Compartment]' % (path), 'init')
            moose.useClock(1, '%s/##[TYPE=Compartment]' % (path), 'process')
            moose.useClock(2, '%s/##[TYPE=SynChan],%s/##[TYPE=HHChannel]' % (path, path), 'process')
            moose.useClock(3, '%s/##[TYPE=SpikeGen],%s/##[TYPE=PulseGen]' % (path, path), 'process')
        moose.useClock(4, '/data/##[TYPE=Table]', 'process')
        inited = True
    moose.reinit()

if __name__ == '__main__':
    # test_compartment() # this calls assign_clocks - after which nothing else will be scheduled.
    simtime = 0.1
    simdt = 0.25e-5
    plotdt = 0.25e-3
    size = 2
    data1 = single_population(size=size)
    data2 = two_populations(size=size)
    assign_clocks(['/network1', '/network2'], simdt, plotdt)
    # assign_clocks(['/network1'], simdt, plotdt)
    moose.start(simtime)
    plt.figure(1)
    plt.suptitle('Single population')
    plt.subplot(211)
    for vm in data1['vm']:
        t = np.linspace(0, simtime, len(vm.vector))
        plt.plot(t, vm.vector, label=vm.path)
    plt.plot(np.linspace(0, simtime, len(data1['pulse'].vector)), data1['pulse'].vector * 1e6, label='Inject(uA)')
    plt.legend()
    plt.subplot(212)    
    for gk in data1['gksyn']:
        t = np.linspace(0, simtime, len(gk.vector))
        plt.plot(t, gk.vector, label=gk.path)
    plt.legend()
    plt.figure(2)
    plt.suptitle('Two populations')
    plt.subplot(221)
    for vm in data2['vm_a']:
        t = np.linspace(0, simtime, len(vm.vector))
        plt.plot(t, vm.vector, label=vm.path)
    plt.plot(np.linspace(0, simtime, len(data2['pulse'].vector)), data2['pulse'].vector*1e6, label='Inject(uA)')
    plt.legend()
    plt.subplot(223)
    for vm in data2['vm_b']:
        t = np.linspace(0, simtime, len(vm.vector))
        plt.plot(t, vm.vector, label=vm.path)
    plt.legend()
    plt.subplot(222)
    for gk in data2['gksyn_a']:
        t = np.linspace(0, simtime, len(gk.vector))
        plt.plot(t, gk.vector, label=gk.path)
    plt.legend()
    plt.subplot(224)
    for gk in data2['gksyn_b']:
        t = np.linspace(0, simtime, len(gk.vector))
        plt.plot(t, gk.vector, label=gk.path)
    plt.legend()
    plt.show()
    

# 
# compartment_net_no_array.py ends here
