# rc19.py --- 
# 
# Filename: rc19.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Sat May 24 14:10:22 2014 (+0530)
# Version: 
# Last-Updated: 
#           By: 
#     Update #: 0
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
"""Cell morphology and passive properties from Branco et al 2010."""
from __future__ import print_function

__author__ = 'Subhasis Ray'

import sys

import moose
from moose import utils as mutils
from synapse import *
from matplotlib import pyplot as plt
import numpy as np
from settings import *

from nachannel import *
from kchannel import *
from cachannel import *
from hchannel import *

# locations of the synapses
# synapse_locations = [#(9, 2), # this one seems garbage : loc should be within 0-1
#                      (13, 1),
#                      (13, 0.875),
#                      (13, 0.75),
#                      (13, 0.625),
#                      (13, 0.5),
#                      (13, 0.375),
#                      (13, 0.25),
#                      (13, 0.125),
#                      (13, 0)]

# This has been generated and edited from dumping the locations in the
# neuron model
synloc = [
    'dend_13_0',
    'dend_13_1',
    'dend_13_2',
    'dend_13_3',
    'dend_13_4',
    'dend_13_5',
    'dend_13_6',
    'dend_13_7',
    'dend_13_8',]

# synapse stimulation order
# stim_order = [# [2, 9], # What is this?
#               [0, 1, 2, 3, 4, 5, 6, 7, 8],
#               [8, 7, 6, 5, 4, 3, 2, 1, 0]]
stim_order = [[8]]

color = {0: 'darkcyan',
         1: 'darkmagenta'}

library = moose.Neutral('/library')
model_container = moose.Neutral('/model')
data_container = moose.Neutral('/data')

simdt = 5e-6

def make_prototype(passive=True):
    path = '%s/rc19' % (library.path)
    pfile = 'rc19.p'
    try:
        return moose.element(path)
    except ValueError:
        pass
    if not passive:
        make_na()
        make_kv()
        make_km()
        make_kca()
        make_cat()
        make_cahva()
        make_h()
    try:        
        proto = moose.element(path)
    except ValueError:
        print('Loading model %s to %s' % (pfile, path))
        proto = moose.loadModel(pfile, path, 'ee') # hsolve is not functional yet
        for comp in proto[0].children:
            comp.initVm = -75e-3
        for chan in moose.wildcardFind('%s/##[ISA=HHChannel]'):
            chan.Gbar *= tadj
    return proto

def setup_model(model_path, synapse_locations, passive=False, solver='hsolve'):
    """Set up a single cell model under `model_path` with synapses
    created in the compartments listed in `synapse_locations`.


    `model_path` - location where the model should be created.
    
    `synapse_locations`: compartment names for the synapses.

    """
    cell = moose.copy(make_prototype(passive), model_path)
    if solver.lower() == 'hsolve':
        hsolve = moose.HSolve( '%s/solve' % (cell.path))
        hsolve.dt = simdt
        hsolve.target = cell.path
    syninfo_list = []
    for compname in synapse_locations:
        comppath = '%s/%s' % (cell.path, compname)
        print('1111 Creating synapse in', comppath)
        compartment = moose.element(comppath)
        syninfo = make_synapse(compartment)
        syninfo_list.append(syninfo)
        # connect  pulse stimulus
        stim_path = '%s/%s/stim' % (cell.path, compname)
        print('2222 Creating stimuls in', stim_path)
        stim = moose.PulseGen(stim_path)
        moose.connect(stim, 'output', syninfo['spike'], 'Vm')
        syninfo['stimulus'] = stim
    return {'neuron': cell,
            'syninfo': syninfo_list}

def setup_recording(data_path, neuron, syninfo_list):
    """Record Vm from soma and synaptic conductances from synapses in
    syninfo_list

    """
    neuron_path = neuron.path
    data_container = moose.Neutral(data_path)
    soma_vm = moose.Table('%s/Vm_soma' % (data_path))
    soma_path = '%s/soma_1' % (neuron_path)
    print('5555 Soma path', soma_path)
    soma = moose.element(soma_path)
    moose.connect(soma_vm, 'requestOut', soma, 'getVm')
    ampa_data = moose.Neutral('%s/G_AMPA' % (data_path))
    nmda_data = moose.Neutral('%s/G_NMDA' % (data_path))
    ampa_gk = []
    nmda_gk = []
    
    # Record synaptic conductances
    for syninfo in syninfo_list:
        compname = syninfo['spike'].parent.name
        tab = moose.Table('%s/Gk_nmda_%s' % (nmda_data.path, compname))
        moose.connect(tab, 'requestOut', syninfo['nmda'], 'getGk')
        nmda_gk.append(tab)
        tab = moose.Table('%s/Gk_ampa_%s' % (ampa_data.path, compname))
        moose.connect(tab, 'requestOut', syninfo['ampa'], 'getGk')
        ampa_gk.append(tab)
    return {'ampa_gk': ampa_gk,
            'nmda_gk': nmda_gk,
            'soma_vm': soma_vm,
            'data': data_container}

def setup_experiment(name, stim_order, onset, interval, passive=False, solver='hsolve'):
    """Setup an experiment with specified stimulation order. `stim_order` is a
    series of integers specifying the compartment numbers along dendritic 
    branch dend_13. `onset` is time of onset of stimulation protocol. 
    `inteval` is the interval between stimulation time of 
    successive synapses."""    
    model_container = moose.Neutral('/model/%s' % (name))
    model_info = setup_model(model_container.path, synloc, passive=passive, solver=solver)
    data_container = moose.Neutral('/data/%s' % (name))
    data_info = setup_recording(data_container.path, model_info['neuron'], model_info['syninfo'])
    for ii, dend_no in enumerate(stim_order):
        comp_path = '%s/%s' % (model_info['neuron'].path, synloc[dend_no])
        stim = moose.PulseGen('%s/stim' % (comp_path))
        stim.delay[0] = onset + ii * interval
        stim.width[0] = 1e9 # The spike generator is edge triggered. A single level change will suffice.
        stim.level[0] = 1.0
    print('Experiment %s has been setup.' % (name))
    print('Stimulus order:', [synloc[ii] for ii in stim_order])
    print('Stimulus onset:', onset)
    print('Inter stimulus interval:', interval)
    return (data_info, model_info)
        

tstop = 200e-3
tonset = 50e-3
intervals = [1e-3]#[ii * 1e-3 for ii in range(1,11)]
def run_sim_parallel(passive=True, solver='hsolve'):
    data_info_list = []
    model_info_list = []
    for jj, ti in enumerate(intervals):
        for ii, st in enumerate(stim_order):
            experiment_name = 'expt_%d_%d' % (jj, ii)
            dinfo, minfo = setup_experiment(experiment_name, st, tonset, ti, passive=passive, solver=solver)
            data_info_list.append(dinfo)
            model_info_list.append(minfo)
    mutils.setDefaultDt(elecdt=simdt)
    mutils.assignDefaultTicks()
    moose.reinit()
    moose.start(tstop)
    print('$$$$$$$$$$$', moose.element('/clock'    ).currentTime)
    fig = plt.figure()
    axes_vm = fig.add_subplot(111)
    # axes_vm_out = fig.add_subplot(121)
    # axes_vm_in = fig.add_subplot(122, sharex=axes_vm_out, sharey=axes_vm_out)
    ################
    # axes_vm = fig.add_subplot(311)
    # axes_nmda = fig.add_subplot(312)
    # axes_ampa = fig.add_subplot(313)
    for jj, ti in enumerate(intervals):
        for ii, st in enumerate(stim_order):
            dinfo = data_info_list[jj * len(stim_order) + ii]
            print('Interval=', ti, 'Stim order=', st)
            print('dinfo:', dinfo)
            print(dinfo['soma_vm'])
            print(dinfo['soma_vm'].vector)
            v = dinfo['soma_vm'].vector
            t = np.linspace(0, tstop, len(v))
            print('num points=', len(t), 't0=', t[0], 't_last=', t[-1], 'v0=', v[0], 'v_last=', v[-1])
            axes_vm.plot(t, v)
            # if ii % 2 == 0:
            #     axes_vm_in.plot(t,
            #                     v,
            #                     color=color[ii])
            # else:
            #     axes_vm_out.plot(t,
            #                      v,
            #                      color=color[ii])
            # for tab in dinfo['nmda_gk']:
            #     axes_nmda.plot(np.linspace(0, tstop, len(tab.vector)),
            #                    tab.vector, color=color[ii])
            # # axes_nmda.legend()
            # for tab in dinfo['ampa_gk']:
            #     axes_ampa.plot(np.linspace(0, tstop, len(tab.vector)),
            #                    tab.vector, label='%s/%s' % (dinfo['data'].name, tab.name), color=color[ii])
    # axes_vm.legend([plt.Line2D([0], [0], color=color[ii]) for ii in range(len(stim_order))],
    #                [str(st) for st in stim_order])
    #axes_vm.legend()
    #axes_nmda.legend()
    #axes_ampa.legend()
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) > 1:
        passive = sys.argv[1].lower() == 'passive'
    else:
        passive = False
    if len(sys.argv) > 2:
        solver = sys.argv[2].lower()
    else:
        solver = 'hsolve'
    print('running simulation using: model with solver %s. Model is passive? %s' % (solver, passive))
    run_sim_parallel(passive=passive, solver=solver)


# 
# rc19.py ends here
