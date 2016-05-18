# vclamp.py --- 
# 
# Filename: vclamp.py
# Description: 
# Author:Subhasis Ray 
# Maintainer: 
# Created: Sat Feb  2 19:16:54 2013 (+0530)
# Version: 
# Last-Updated: Tue Jun 11 17:35:20 2013 (+0530)
#           By: subha
#     Update #: 178
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

import sys
sys.path.append('../../python')
import moose
sys.path.append('../squid')
from squid import SquidAxon
from pylab import *

def vclamp_demo(simtime=50.0, dt=1e-2):
    ## It is good practice to modularize test elements inside a
    ## container
    container = moose.Neutral('/vClampDemo')
    ## Create a compartment with properties of a squid giant axon
    comp = SquidAxon('/vClampDemo/axon')
    # Create and setup the voltage clamp object
    clamp = moose.VClamp('/vClampDemo/vclamp')
    ## The defaults should work fine
    # clamp.mode = 2
    # clamp.tau = 10*dt
    # clamp.ti = dt
    # clamp.td = 0
    # clamp.gain = comp.Cm / dt
    ## Setup command voltage time course
    command = moose.PulseGen('/vClampDemo/command')
    command.delay[0] = 10.0
    command.width[0] = 20.0
    command.level[0] = 50.0
    command.delay[1] = 1e9
    moose.connect(command, 'output', clamp, 'commandIn')
    ## Connect the Voltage Clamp to the compartemnt
    moose.connect(clamp, 'currentOut', comp, 'injectMsg')
    moose.connect(comp, 'VmOut', clamp, 'sensedIn')
    ## setup stimulus recroding - this is the command pulse
    stimtab = moose.Table('/vClampDemo/vclamp_command')
    moose.connect(stimtab, 'requestOut', command, 'getOutputValue')
    ## Set up Vm recording
    vmtab = moose.Table('/vClampDemo/vclamp_Vm')
    moose.connect(vmtab, 'requestOut', comp, 'getVm')
    ## setup command potential recording - this is the filtered input
    ## to PID controller
    commandtab = moose.Table('/vClampDemo/vclamp_filteredcommand')
    moose.connect(commandtab, 'requestOut', clamp, 'getCommand')
    ## setup current recording
    Imtab = moose.Table('/vClampDemo/vclamp_inject')
    moose.connect(Imtab, 'requestOut', clamp, 'getCurrent')
    # Scheduling
    moose.setClock(0, dt)
    moose.setClock(1, dt)    
    moose.setClock(2, dt)
    moose.setClock(3, dt)
    moose.useClock(0, '%s/##[TYPE=Compartment]' % (container.path), 'init')
    moose.useClock(0, '%s/##[TYPE=PulseGen]' % (container.path), 'process')
    moose.useClock(1, '%s/##[TYPE=Compartment]' % (container.path), 'process')
    moose.useClock(2, '%s/##[TYPE=HHChannel]' % (container.path), 'process')
    moose.useClock(2, '%s/##[TYPE=VClamp]' % (container.path), 'process')
    moose.useClock(3, '%s/##[TYPE=Table]' % (container.path), 'process')
    moose.reinit()
    print 'RC filter in VClamp:: tau:', clamp.tau
    print 'PID controller in VClamp:: ti:', clamp.ti, 'td:', clamp.td, 'gain:', clamp.gain
    moose.start(simtime)
    print 'Finished simulation for %g seconds' % (simtime)
    tseries = linspace(0, simtime, len(vmtab.vector))
    subplot(211)
    title('Membrane potential and clamp voltage')
    plot(tseries, vmtab.vector, 'g-', label='Vm (mV)')
    plot(tseries, commandtab.vector, 'b-', label='Filtered command (mV)')
    plot(tseries, stimtab.vector, 'r-', label='Command (mV)')
    xlabel('Time (ms)')
    ylabel('Voltage (mV)')
    legend()
    # print len(commandtab.vector)
    subplot(212)
    title('Current through clamp circuit')
    # plot(tseries, stimtab.vector, label='stimulus (uA)')
    plot(tseries, Imtab.vector, label='injected current (uA)')
    xlabel('Time (ms)')
    ylabel('Current (uA)')
    legend()
    show()

if __name__ == '__main__':
    vclamp_demo()
    

# 
# vclamp.py ends here
