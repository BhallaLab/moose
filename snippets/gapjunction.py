# gapjunction.py ---
#
# Filename: gapjunction.py
# Description:
# Author:Subhasis Ray
# Maintainer:
# Created: Tue Jul  2 14:28:35 2013 (+0530)
# Version:
# Last-Updated: Tue Jul 23 21:28:45 2013 (+0530)
#           By: subha
#     Update #: 57
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
from moose import utils
import pylab

simtime = 100e-3
simdt = 1e-6

def make_compartment(path):
    comp = moose.Compartment(path)
    comp.Em = -70e-3
    comp.initVm = -70e-3
    comp.Cm = 1e-12
    comp.Rm = 1e9
    return comp

def gapjunction_demo():

    """
    Demonstration of medelling gap junction using MOOSE.
    """

    model = moose.Neutral('model')
    data = moose.Neutral('data')
    comps = []
    comp1 = make_compartment('%s/comp1' % (model.path))
    comp2 = make_compartment('%s/comp2' % (model.path))
    pulse = moose.PulseGen('%s/pulse' % (model.path))
    pulse.level[0] = 1e-9
    pulse.delay[0] = 50e-3
    pulse.width[0] = 20e-3
    pulse.delay[1] = 1e9
    moose.connect(pulse, 'output', comp1, 'injectMsg')
    gj = moose.GapJunction('%s/gj' % (model.path))
    gj.Gk = 1e-6
    moose.connect(gj, 'channel1', comp1, 'channel')
    moose.connect(gj, 'channel2', comp2, 'channel')
    vm1_tab = moose.Table('%s/Vm1' % (data.path))
    moose.connect(vm1_tab, 'requestOut', comp1, 'getVm')
    vm2_tab = moose.Table('%s/Vm2' % (data.path))
    moose.connect(vm2_tab, 'requestOut', comp2, 'getVm')
    pulse_tab = moose.Table('%s/inject' % (data.path))
    moose.connect(pulse_tab, 'requestOut', pulse, 'getOutputValue')
    utils.setDefaultDt(elecdt=simdt, plotdt2=simdt)
    utils.assignDefaultTicks()
    utils.stepRun(simtime, 10000*simdt)
    # print len(vm1_tab.vector), len(vm2_tab.vector), len(pulse_tab.vector)
    # moose.showmsg(comp1)
    # moose.showmsg(comp2)
    # moose.showmsg(pulse)
    t = pylab.linspace(0, simtime, len(vm1_tab.vector))
    pylab.plot(t, vm1_tab.vector*1000, label='Vm1 (mV)')
    pylab.plot(t, vm2_tab.vector*1000, label='Vm2 (mV)')
    pylab.plot(t, pulse_tab.vector*1e9, label='inject (nA)')
    pylab.legend()
    pylab.show()


if __name__ == '__main__':
    gapjunction_demo()

def main():
	"""
This example is to demonstrate, how gap junction can be modeled using MOOSE.
	"""
	gapjunction_demo()


#
# gapjunction.py ends here
