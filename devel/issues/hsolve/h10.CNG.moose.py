# -*- coding: utf-8 -*-
# h10.CNG.moose.py ---
#
# Filename: h10.CNG.moose.py
# Description:
# Author: subha
# Maintainer:
# Created: Sat Apr 30 02:10:09 2016 (-0400)
# Version:
# Last-Updated: Sat Apr 30 03:12:18 2016 (-0400)
#           By: subha
#     Update #: 95
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
from __future__ import print_function
import numpy as np
import moose
import rdesigneur as rd

# Reduce dt to get more accurate result from EE solver
dt = 25e-6

model = moose.Neutral('/model')
data = moose.Neutral('/data')
cell = moose.loadModel('h10.CNG.swc', '/model/h10')
RA = 1.0    # Ohm m - nrn 100 Ohm cm
RM = 0.1    # Ohm m2 - nrn 0.001 S/cm2
CM = 0.01   # F/m2  - nrn 1uF/cm2

for comp in moose.wildcardFind('{}/##[ISA=CompartmentBase]'.format(cell.path)):
    sarea = np.pi * comp.diameter * comp.length
    xarea = np.pi * comp.diameter * comp.diameter / 4
    comp.Ra = RA * comp.length / xarea
    comp.Rm = RM / sarea
    comp.Cm = CM * sarea
    comp.initVm = -70e-3
    comp.Em = -70e-3
    print(comp.name, comp.x0, comp.y0, comp.z0, comp.x, comp.y, comp.z)
# Last apical compartment is apical_e_177 between nodes 200 (-3.061,
# 56.39, -3.12) and 204 (-32.910 128.5 -3.12) in the swc file- same as
# 90 in NEURON.

stim = moose.PulseGen('/model/stim')
stim.delay[0] = 20e-3
stim.width[0] = 10e-3
stim.level[0] = 1e-12
stim.delay[1] = 1e9
moose.connect(stim, 'output', moose.element('/model/h10/apical_e_177_0'), 'injectMsg')
tab_v_soma = moose.Table('/data/soma_v')
moose.connect(tab_v_soma, 'requestOut', moose.element('/model/h10/soma'), 'getVm')
tab_v_apical = moose.Table('/data/apical_v')
moose.connect(tab_v_apical, 'requestOut', moose.element('/model/h10/apical_e_177_0'), 'getVm')

import sys

if __name__ == '__main__':
    if len(sys.argv) > 1:
        solver = moose.HSolve('/model/h10/solver')
        solver.target = '/model/h10/soma'
        solver.dt = dt
        outfile = 'h10.CNG.moose.hsolve.txt'
    else:
        outfile = 'h10.CNG.moose.ee.txt'
        for ii in range(20):
            moose.setClock(ii, dt)

    simtime = 100e-3
    moose.reinit()
    moose.start(simtime)
    t = np.arange(len(tab_v_soma.vector)) * simtime / len(tab_v_soma.vector)
    np.savetxt(outfile, np.vstack([t, tab_v_soma.vector, tab_v_apical.vector]).T)



#
# h10.CNG.moose.py ends here
