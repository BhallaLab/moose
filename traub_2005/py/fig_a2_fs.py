# fig)a2.py --- 
# 
# Filename: fig)a2.py
# Description: 
# Author: 
# Maintainer: 
# Created: Thu Jun 27 12:06:52 2013 (+0530)
# Version: 
# Last-Updated: Sat Aug  6 15:28:07 2016 (-0400)
#           By: subha
#     Update #: 30
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
"""
Reproduce the experiment for Fig A2 in Traub et al 2005. 

FS (deep basket cell) with 0.5 nA current pusle simulated for 1 s
"""
from __future__ import print_function

import numpy as np
import pylab

import moose
from moose import utils
from cells import DeepBasket
import config

simtime = 200e-3
simdt = 2e-5
plotdt=1e-4

def setup_model(root='/', hsolve=True):
    moose.ce(root)
    model = moose.Neutral('model')
    data = moose.Neutral('data')
    cell = DeepBasket('%s/deepbasket' % (model.path))
    soma = moose.Compartment('%s/comp_1' % (cell.path))
    if hsolve:
        solver = moose.HSolve('%s/solve' % (cell.path))
        solver.dt = simdt
        solver.target = cell.path
    pulse = moose.PulseGen('%s/stimulus' % (model.path))
    moose.connect(pulse, 'output', soma, 'injectMsg')
    tab_vm = moose.Table('%s/spinystellate_soma_Vm' % (data.path))
    moose.connect(tab_vm, 'requestOut', soma, 'getVm')
    tab_stim = moose.Table('%s/spinystellate_soma_inject' % (data.path))
    moose.connect(tab_stim, 'requestOut', pulse, 'getOutputValue')
    utils.setDefaultDt(elecdt=simdt, plotdt2=plotdt)
    utils.assignDefaultTicks(model, data)
    return {'stimulus': pulse,
            'tab_vm': tab_vm,
            'tab_stim': tab_stim}

def do_sim(pulsegen, amp):
    pulsegen.level[0] = amp 
    pulsegen.delay[0] = 50e-3
    pulsegen.width[0] = 100e-3
    moose.reinit()
    utils.stepRun(simtime, 10000*simdt, logger=config.logger)
    

def main():
    amp = 0.4e-9
    model_dict = setup_model()
    do_sim(model_dict['stimulus'], amp)   
    config.logger.info('##### %d' % (model_dict['tab_vm'].size))
    vm = model_dict['tab_vm'].vector * 1e3
    inject = model_dict['tab_stim'].vector.copy()
    t = np.linspace(0, simtime, len(vm))
    fname = 'data_fig_a2_FS.txt'
    np.savetxt(fname,
               np.vstack((t, inject, vm)).transpose())
    msg = 'Saved data for %g A current pulse in %s' % (amp, fname)
    config.logger.info(msg)
    print(msg)
    pylab.title('%g nA' % (amp*1e9))
    pylab.plot(t, vm, label='soma-Vm (mV)')
    stim_boundary = np.flatnonzero(np.diff(inject))
    pylab.plot((t[stim_boundary[0]]), (vm.min()), 'r^', label='stimulus start')       
    pylab.plot((t[stim_boundary[-1]]), (vm.min()), 'gv', label='stimulus end')        
    pylab.legend()    
    pylab.savefig('fig_a2_fs.png')
    pylab.show()

if __name__ == '__main__':
    main()




# 
# fig)a2.py ends here
