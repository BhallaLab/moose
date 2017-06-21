# rall64.py --- 
# 
# Filename: rall64.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri May 23 16:33:43 2014 (+0530)
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



"""
Implementation of Rall 1964 model of dendritic branching. All units
SI.

"""
from __future__ import print_function

import sys
import math
import pylab
import moose
from moose import utils as mu

RM = 0.4 # specific membrane resistivity unit: Ohm m2
RA = 1.0 # specific cytoplasmic resistivity unit: Ohm m
CM = 0.01 # specific membrane capacitance unit: Farad/m2
diameter = 1e-6

lambda_ = math.sqrt(0.25 * diameter * RM/RA)    # electrotonic length
tau = RM * CM                   # Membrane time constant

length = 0.2 * lambda_ # Rall used 10 compartments of `0.2 * lambda` length each

Em = -65e-3             # Leak reversal potential
initVm = Em         # Initial membrane potential
Rm = RM / (math.pi * diameter * length)         # Total membrane resistance
Cm = CM * (math.pi * diameter * length)         # Total membrane capacitance
# Total axial (cytoplasmic) resistance between compartments
Ra = RA * length / (math.pi * diameter * diameter / 4.0)        

Ek = Em + 1e-3

print('tau =', tau, 'lambda =', lambda_)

# Rall used `0.05 * tau`, but that does not reproduce in our
# case. Why? Sym vs Asym comp??
dt = 0.01 * tau 
simtime = 2.5 * tau
inject_time = 0.25 * tau

model = moose.Neutral('/model')
data = moose.Neutral('/data')

num_comp = 10

def make_compartment(path):
    """Create a compartment at `path` with standard property values."""
    comp = moose.Compartment(path)
    comp.Em = Em
    comp.Rm = Rm
    comp.Cm = Cm
    comp.Ra = Ra
    comp.initVm = initVm
    comp.diameter = diameter
    comp.length = length
    return comp
    
def make_cable(path, num_comp):
    """Create a cable made of `nump_comp` identical compartments under
    `path`.

    The compartments will be named c0 to c{num_comp} and returned in a
    list.

    """
    cable_container = moose.Neutral(path)
    comp_list = []
    for ii in range(num_comp):
        comp_path = '%s/c%d' % (path, ii)
        comp = make_compartment(comp_path)
        if len(comp_list) > 0:
            # connect to the last compartment via axial resistance
            moose.connect(comp_list[-1], 'raxial', comp, 'axial') 
        comp_list.append(comp)
    return comp_list

def insert_channel(comp, ek=Ek):
    """Insert a constant conductance channel on compartment `comp`. Set
    the channel reversal potential to `Ek`

    """
    chan = moose.Leakage('%s/chan' % (comp.path))
    chan.Ek = ek
    moose.connect(chan, 'channel', comp, 'channel')
    return chan

def insert_Vm_probe(probename, comp):
    """Insert a recorder for Vm in compartment `comp`."""
    probe = moose.Table('/data/%s' % (probename))
    moose.connect(probe, 'requestOut', comp, 'getVm')
    return probe

# Scheduling
def schedule():
    moose.setClock(0, dt)
    moose.setClock(1, dt)
    moose.setClock(2, dt)
    moose.setClock(3, dt)
    moose.useClock(0, '/model/##[ISA=Compartment]', 'init')
    moose.useClock(1, '/model/##', 'process')
    moose.useClock(3, '/data/##', 'process')
    moose.reinit()

def setup_model_fig6():
    """Setup the model and recording for fig 6 experiment. This will
    insert channels in specific compartments for each cable."""
    # Figure 6A 
    cable_6a = make_cable('/model/cable_6a', num_comp)
    chan_6a_1 = insert_channel(cable_6a[1])
    chan_6a_2 = insert_channel(cable_6a[2])
    soma_Vm_6a = insert_Vm_probe('soma_Vm_6a', cable_6a[0])    

    # Figure 6B
    cable_6b = make_cable('/model/cable_6b', num_comp)
    chan_6b_1 = insert_channel(cable_6b[3])
    chan_6b_2 = insert_channel(cable_6b[4])
    soma_Vm_6b = insert_Vm_probe('soma_Vm_6b', cable_6b[0])

    # Figure 6C
    cable_6c = make_cable('/model/cable_6c', num_comp)
    chan_6c_1 = insert_channel(cable_6c[5])
    chan_6c_2 = insert_channel(cable_6c[6])
    soma_Vm_6c = insert_Vm_probe('soma_Vm_6c', cable_6c[0])

    # Figure 6D
    cable_6d = make_cable('/model/cable_6d', num_comp)
    chan_6d_1 = insert_channel(cable_6d[7])
    chan_6d_2 = insert_channel(cable_6d[8])
    soma_Vm_6d = insert_Vm_probe('soma_Vm_6d', cable_6d[0])
    return [soma_Vm_6a, soma_Vm_6b, soma_Vm_6c, soma_Vm_6d]            

def run_model_fig6():
    """Do a simulation for fig6 and plot data."""
    for ch in moose.wildcardFind('/model/##[ISA=ChanBase]'):
        ch.Gk = 1.0/Rm
    print('Starting for', inject_time)
    moose.start(inject_time)
    for ch in moose.wildcardFind('/model/##[ISA=ChanBase]'):
        ch.Gk = 0.0
    moose.start(simtime - inject_time)

def plot_fig6(vm):
    ax_6a = pylab.subplot(411)
    ax_6a.plot(pylab.linspace(0, simtime / tau,
                              len(vm[0].vector)),
               (vm[0].vector - Em)/(Ek - Em))
    ax_6a.set_title('6A')
    ax_6b = pylab.subplot(412, sharex=ax_6a, sharey=ax_6a)
    ax_6b.plot(pylab.linspace(0, simtime / tau, len(vm[1].vector)),
               (vm[1].vector - Em)/(Ek - Em))
    ax_6b.set_title('6B')
    ax_6c = pylab.subplot(413, sharex=ax_6a, sharey=ax_6a)
    ax_6c.plot(pylab.linspace(0, simtime / tau, len(vm[2].vector)),
               (vm[2].vector - Em)/(Ek - Em))
    ax_6c.set_title('6C')
    ax_6d = pylab.subplot(414, sharex=ax_6a, sharey=ax_6a)
    ax_6d.plot(pylab.linspace(0, simtime / tau, len(vm[3].vector)),
               (vm[3].vector - Em)/(Ek - Em))
    ax_6d.set_title('6D')
    pylab.tight_layout()
    pylab.show()

def simulate_fig6():
    vm_tables = setup_model_fig6()
    schedule()
    run_model_fig6()
    plot_fig6(vm_tables)

def setup_model_fig7():
    cable_1 = make_cable('/model/cable_7_1', num_comp)
    chans_1 = []
    for ii in range(1, 9):
        chan = insert_channel(cable_1[ii])
        chans_1.append(chan)
    cable_2 = make_cable('/model/cable_7_2', num_comp)
    chans_2 = []
    for ii in range(1, 9):
        chan = insert_channel(cable_2[ii])
        chans_2.append(chan)
    cable_3 = make_cable('/model/cable_7_3', num_comp)
    chans_3 = []
    for ii in range(1, 9):
        chan = insert_channel(cable_3[ii])
        chans_3.append(chan)
        # chan.Gk = 0.25 / Rm
    soma_Vm_1 = insert_Vm_probe('ABCD', cable_1[0])
    soma_Vm_2 = insert_Vm_probe('DCBA', cable_2[0])
    soma_Vm_3 = insert_Vm_probe('control', cable_3[0])
    return (chans_1, chans_2, chans_3), (soma_Vm_1, soma_Vm_2, soma_Vm_3)

def run_model_fig7(chans_1, chans_2, chans_3):
    to_run = simtime
    delta_t = 0.25 * tau
    for ii in range(0, len(chans_1), 2):
        print(ii)
        print('-----------------')
        chans_1[ii].Gk = 1 / Rm
        chans_1[ii+1].Gk = 1 / Rm
        chans_2[-ii-1].Gk = 1 / Rm
        chans_2[-ii-2].Gk = 1 / Rm
        for chan in chans_3:
            chan.Gk = 0.25 / Rm
        
        for chan in chans_1:
            print(chan.Gk, end=' ')
        print()
        for chan in chans_2:
            print(chan.Gk, end=' ')
        print()
        moose.start(delta_t)
        for chan in chans_1:
            chan.Gk = 0.0
        for chan in chans_2:
            chan.Gk = 0.0
        to_run = to_run - delta_t
    for chan in chans_3:
        chan.Gk = 0.0
    moose.start(to_run)
    
def plot_fig7(vm):
    ax_7 = pylab.subplot(111)
    ax_7.plot(pylab.linspace(0, simtime/tau, len(vm[0].vector)),
              (vm[0].vector-Em)/(Ek - Em), label='(1,2)->(3,4)->(5,6)->(7,8)')
    ax_7.plot(pylab.linspace(0, simtime/tau, len(vm[1].vector)),
              (vm[1].vector-Em)/(Ek - Em), label='(7,8)->(5,6)->(3,4)->(1,2)')
    ax_7.plot(pylab.linspace(0, simtime/tau, len(vm[2].vector)),
              (vm[2].vector-Em)/(Ek - Em), label='control')
    pylab.legend()
    pylab.show()

def simulate_fig7():
    chans_list, vm_list = setup_model_fig7()
    schedule()
    run_model_fig7(*chans_list) # * converts list to args
    plot_fig7(vm_list)

if __name__ == '__main__':
    # simulate_fig6()
    simulate_fig7()

# 
# rall64.py ends here
