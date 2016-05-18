# symcomp_readcell.py --- 
# 
# Filename: symcomp_readcell.py
# Description: 
# Author: 
# Maintainer: 
# Created: Fri Jun 21 16:38:36 2013 (+0530)
# Version: 
# Last-Updated: Fri Jun 21 20:33:11 2013 (+0530)
#           By: subha
#     Update #: 20
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
import  pylab 
import numpy as np

simdt = 1e-6
simtime = 100e-3

def test_symcomp_readcell():
    model = moose.Neutral('/model')
    cell = moose.loadModel('symcomp.p', '%s/cell' % (model.path))
    pg = moose.PulseGen('/model/pulse')
    pg.delay[0] = 10e-3
    pg.width[0] = 20e-3
    pg.level[0] = 1e-6
    pg.delay[1] = 1e9
    moose.connect(pg, 'output', moose.element('/model/cell/d1'), 'injectMsg')
    data = moose.Neutral('/data')
    tab_soma = moose.Table('%s/soma_Vm' % (data.path))
    tab_d1 = moose.Table('%s/d1_Vm' % (data.path))
    tab_d2 = moose.Table('%s/d2_Vm' % (data.path))
    moose.connect(tab_soma, 'requestOut', moose.element('/model/cell/soma'), 'getVm')
    moose.connect(tab_d1, 'requestOut', moose.element('/model/cell/d1'), 'getVm')
    moose.connect(tab_d2, 'requestOut', moose.element('/model/cell/d2'), 'getVm')
    moose.setClock(0, simdt)
    moose.setClock(1, simdt)
    moose.setClock(2, simdt)
    moose.useClock(0, '/model/##[ISA=Compartment]', 'init') # This is allowed because SymCompartment is a subclass of Compartment
    moose.useClock(1, '/model/##', 'process')
    moose.useClock(2, '/data/##[ISA=Table]', 'process')
    moose.reinit()
    moose.start(simtime)
    t = np.linspace(0, simtime, len(tab_soma.vector))
    data_matrix = np.vstack((t, tab_soma.vector, tab_d1.vector, tab_d2.vector))
    np.savetxt('symcompartment_readcell.txt', data_matrix.transpose())
    pylab.plot(t, tab_soma.vector, label='Vm_soma')
    pylab.plot(t, tab_d1.vector, label='Vm_d1')
    pylab.plot(t, tab_d2.vector, label='Vm_d2')
    pylab.show()

if __name__ == '__main__':
    test_symcomp_readcell()


# 
# symcomp_readcell.py ends here
