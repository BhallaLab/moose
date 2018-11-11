# -*- coding: utf-8 -*-
# run_cell.py ---
#
# Filename: run_cell.py
# Description:
# Author:
# Maintainer: P Gleeson
# Version:
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

import moose
import moose.utils as mu
import sys
from reader import NML2Reader
import numpy as np

    
def run(nogui):
    
    reader = NML2Reader(verbose=True)

    filename = 'test_files/passiveCell.nml'
    print('Loading: %s'%filename)
    reader.read(filename)
    
    
    msoma = reader.getComp(reader.doc.networks[0].populations[0].id,0,0)
    print(msoma)
    
    
    data = moose.Neutral('/data')
    
    pg = reader.getInput('pulseGen1')
    
    inj = moose.Table('%s/pulse' % (data.path))
    moose.connect(inj, 'requestOut', pg, 'getOutputValue')
    
    
    vm = moose.Table('%s/Vm' % (data.path))
    moose.connect(vm, 'requestOut', msoma, 'getVm')
    
    simdt = 1e-6
    plotdt = 1e-4
    simtime = 150e-3
    
    for i in range(8):
        moose.setClock( i, simdt )
    moose.setClock( 8, plotdt )
    moose.reinit()
    moose.start(simtime)
    
    print("Finished simulation!")
    
    t = np.linspace(0, simtime, len(vm.vector))
    
    if not nogui:
        import matplotlib.pyplot as plt

        plt.subplot(211)
        plt.plot(t, vm.vector * 1e3, label='Vm (mV)')
        plt.legend()
        plt.title('Vm')
        plt.subplot(212)
        plt.title('Input')
        plt.plot(t, inj.vector * 1e9, label='injected (nA)')
        #plt.plot(t, gK.vector * 1e6, label='K')
        #plt.plot(t, gNa.vector * 1e6, label='Na')
        plt.legend()
        plt.show()
        plt.close()

    
if __name__ == '__main__':
    
    nogui = '-nogui' in sys.argv
    
    run(nogui)
