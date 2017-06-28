# symcompartment.py ---
#
# Filename: symcompartment.py
# Description:
# Author:Subhasis Ray
# Maintainer:
# Created: Thu Jun 20 17:47:10 2013 (+0530)
# Version:
# Last-Updated: Fri Jul 12 12:10:52 2013 (+0530)
#           By: subha
#     Update #: 71
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

import numpy as np
import pylab

import moose

simdt = 1e-6
simtime = 100e-3

def test_symcompartment():
    """This example demonstrates the use of SymCompartment class of MOOSE."""
    model = moose.Neutral('model')
    soma = moose.SymCompartment('%s/soma' % (model.path))
    soma.Em = -60e-3
    soma.Rm = 1000402560
    soma.Cm = 2.375043912e-11
    soma.Ra = 233957.7812
    d1 = moose.SymCompartment('%s/d1' % (model.path))
    d1.Rm = 397887392
    d1.Cm = 2.261946489e-12
    d1.Ra = 24867960
    d2 = moose.SymCompartment('%s/d2' % (model.path))
    d2.Rm = 2.877870285e+10
    d2.Cm = 8.256105218e-13
    d2.Ra = 20906072
    moose.connect(d1, 'proximal', soma, 'distal')
    moose.connect(d2, 'proximal', soma, 'distal')
    moose.connect(d1, 'sibling', d2, 'sibling')
    pg = moose.PulseGen('/model/pulse')
    pg.delay[0] = 10e-3
    pg.width[0] = 20e-3
    pg.level[0] = 1e-6
    pg.delay[1] = 1e9
    moose.connect(pg, 'output', d1, 'injectMsg')
    data = moose.Neutral('/data')
    tab_soma = moose.Table('%s/soma_Vm' % (data.path))
    tab_d1 = moose.Table('%s/d1_Vm' % (data.path))
    tab_d2 = moose.Table('%s/d2_Vm' % (data.path))
    moose.connect(tab_soma, 'requestOut', soma, 'getVm')
    moose.connect(tab_d1, 'requestOut', d1, 'getVm')
    moose.connect(tab_d2, 'requestOut', d2, 'getVm')
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
    np.savetxt('symcompartment.txt', data_matrix.transpose())
    pylab.plot(t, tab_soma.vector, label='Vm_soma')
    pylab.plot(t, tab_d1.vector, label='Vm_d1')
    pylab.plot(t, tab_d2.vector, label='Vm_d2')
    pylab.show()

def main():
    """
    This example demonstrats the use of SymCompartment class of MOOSE.
    """
    test_symcompartment()

if __name__ == '__main__':
    main()


#
# symcompartment.py ends here
