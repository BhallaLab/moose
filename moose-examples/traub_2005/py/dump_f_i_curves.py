# dump_f_i_curves.py --- 
# 
# Filename: dump_f_i_curves.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Tue Dec  8 13:11:01 2015 (-0500)
# Version: 
# Package-Requires: ()
# Last-Updated: Wed Dec  9 12:28:06 2015 (-0500)
#           By: Subhasis Ray
#     Update #: 79
# URL: 
# Doc URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change Log:
# 
# 
# 
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
# 
# 

# Code:

"""Do a series of current steps on each celltype"""

import numpy as np
try:
    import h5py as h5
except ImportError as e:
    print( "[INFO ] h5py is not found. Quitting ..." )
    quit()
    
from collections import defaultdict

import moose
from moose import utils as mutils

from cells import SpinyStellate
from cells import DeepBasket
from cells import DeepLTS


simdt = 2.5e-6
plotdt = 1e-4

amps = np.array([-0.1, 0.1, 0.5, 1.0, 1.5])*1e-9

def run_current_pulse(amps, delay=100e-3, dur=100e-3, trail=100e-3, outfile='f_i_curves_data.h5'):
    models = []
    model = moose.Neutral('/model')
    data = moose.Neutral('/data')
    ddict = defaultdict(list)
    for ii, amp in enumerate(amps):
        mc = moose.Neutral('{}/mc_{}'.format(model.path, ii))
        models.append(mc)
        stim = moose.PulseGen('{}/stim_{}'.format(mc.path, ii))
        stim.delay[0] = delay
        stim.width[0] = dur
        stim.level[0] = amp
        stim.delay[1] = 1e9 # make delay so large that it does not activate again
        for celltype in [SpinyStellate, DeepBasket, DeepLTS]:
            cell = celltype('{}/{}_{}'.format(mc.path, celltype.__name__, ii))
            solver = moose.element('{}/solver'.format(cell.path))
            solver.dt = simdt
            solver.target = cell.path
            stim.connect('output', cell.soma, 'injectMsg')
            tab = moose.Table('/data/Vm_{}'.format(cell.name))
            ddict[ii].append(tab)
            tab.connect('requestOut', cell.soma, 'getVm')
    mutils.setDefaultDt(elecdt=simdt, plotdt2=plotdt)
    mutils.assignDefaultTicks(modelRoot='/model', dataRoot='/data', solver='hsolve')
    moose.reinit()
    print('Finished scheduling')
    moose.start(delay + dur + trail)
    print('Finished simulation')
    # Save data
    fd = h5.File(outfile, 'w')    
    for ii, tabs in list(ddict.items()):
        for tab in tabs:
            print(('Table', tab.name))
            node = fd.create_dataset(tab.name, data=tab.vector)
            node.attrs['current'] = amps[ii]
            node.attrs['delay'] = delay
            node.attrs['width'] = dur
    fd.close()
    print(('Finished saving data in file', outfile))


if __name__ == '__main__':
    run_current_pulse(amps)

# 
# dump_f_i_curves.py ends here
