# multicomp_lif.py --- 
# 
# Filename: multicomp_lif.py
# Description: Leaky Integrate and Fire using regular neuronal compartment
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri Feb  7 16:26:05 2014 (+0530)
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

"""This is an example of how you can create a Leaky Integrate and Fire
compartment using regular compartment and Func to check for thresold
crossing and resetting the Vm."""


import sys
sys.path.append('../../python')

import moose
from moose import utils
from pylab import *

simtime = 500e-3 # Total simulation time
stepsize = 100e-3 # Time step for pauses between runs
simdt = 1e-4 # time step for numerical integration
plotdt = 0.25e-3 # time step for plotting

delayMax = 5e-3 # Maximum synaptic delay 

class LIFComp(moose.Compartment):
    """Leaky integrate and fire neuron using regular compartments,
    spikegen and Func."""
    def __init__(self, *args):
        moose.Compartment.__init__(self, *args)
        self.spikegen = moose.SpikeGen('%s/spike' % (self.path))
        self.spikegen.edgeTriggered = 1 # This ensures that spike is generated only on leading edge.
        self.dynamics = moose.Func('%s/dynamics' % (self.path))
        self.initVm = 0.0
        self.Rm = 10e6
        self.Ra = 1e4
        self.Cm = 100e-9
        self.Em = 0 #-65e-3
        self.initVm = 0 #self.Em
        
        # Note that the result is dependent on exact order of
        # execution of SpikeGen and Func. If Func gets executed first
        # SpikeGen will never cross threshold.
        self.dynamics.expr = 'x >= y? z: x'
        moose.connect(self, 'VmOut', self.dynamics, 'xIn')
        moose.connect(self.dynamics, 'valueOut', self, 'setVm')
        moose.connect(self, 'VmOut', self.spikegen, 'Vm')

    @property
    def Vreset(self):
        """Reset voltage. The cell's membrane potential is set to this value
        after spiking."""
        return self.dynamics.z

    @Vreset.setter
    def Vreset(self, value):
        self.dynamics.z = value

    @property
    def Vthreshold(self):
        """Threshold voltage. The cell spikes if its membrane potential goes
        above this value."""
        return self.dynamics.y

    @Vthreshold.setter
    def Vthreshold(self, value):
        self.dynamics.y = value
        self.spikegen.threshold = value

def setup_two_cells():
    """Create two cells with leaky integrate and fire compartments. The
    first cell is composed of two compartments a1 and a2 and the
    second cell is composed of compartments b1 and b2. Each pair is
    connected via raxial message so that the voltage of one
    compartment influences the other through axial resistance Ra. 

    The compartment a1 of the first neuron is connected to the
    compartment b2 of the second neuron through a synaptic channel.

    """
    model = moose.Neutral('/model')
    data = moose.Neutral('/data')
    a1 = LIFComp('/model/a1')
    a2 = LIFComp('/model/a2')
    moose.connect(a1, 'raxial', a2, 'axial')
    b1 = LIFComp('/model/b1')
    b2 = LIFComp('/model/b2')
    moose.connect(b1, 'raxial', b2, 'axial')
    a1.Vthreshold = 10e-3
    a1.Vreset = 0
    a2.Vthreshold = 10e-3
    a2.Vreset = 0
    b1.Vthreshold = 10e-3
    b1.Vreset = 0
    b2.Vthreshold = 10e-3
    b2.Vreset = 0
    syn = moose.SynChan('%s/syn' % (b2.path))
    syn.tau1 = 1e-3
    syn.tau2 = 5e-3
    syn.Ek = 90e-3
    synh = moose.SimpleSynHandler( syn.path + "/synh" )
    moose.connect( synh, "activationOut", syn, "activation" )
    synh.numSynapses = 1
    synh.synapse.delay = delayMax
    moose.connect(b2, 'channel', syn, 'channel')
    ## Single message works most of the time but occassionally gives a
    ## core dump
    
    # m = moose.connect(a1.spikegen, 'spikeOut',
    #                   syn.synapse.vec, 'addSpike')

    ## With Sparse message and random connectivity I did not get core
    ## dump.
    m = moose.connect(a1.spikegen, 'spikeOut',
                      synh.synapse.vec, 'addSpike', 'Sparse')
    m.setRandomConnectivity(1.0, 1)
    stim = moose.PulseGen('/model/stim')
    stim.delay[0] = 100e-3
    stim.width[0] = 1e3
    stim.level[0] = 11e-9
    moose.connect(stim, 'output', a1, 'injectMsg')
    tables = []
    data = moose.Neutral('/data')    
    for c in moose.wildcardFind('/##[ISA=Compartment]'):
        tab = moose.Table('%s/%s' % (data.path, c.name))
        moose.connect(tab, 'requestOut', c, 'getVm')
        tables.append(tab)
        # t1 = moose.Table('%s/%s' % (data.path, c.name))
        # moose.connect(t1, 'requestOut', moose.element('%s/dynamics' % (c.path)), 'getX')
        # tables.append(t1)
    syntab = moose.Table('%s/%s' % (data.path, 'Gk'))
    moose.connect(syntab, 'requestOut', syn, 'getGk')
    tables.append(syntab)
    synh.synapse[0].delay = 1e-3
    syn.Gbar = 1e-6
    return tables

if __name__ == '__main__':
    tables = setup_two_cells()
    
    utils.setDefaultDt(elecdt=simdt, plotdt2=plotdt)
    utils.assignDefaultTicks(modelRoot='/model', dataRoot='/data', solver='ee')
    moose.reinit()
    utils.stepRun(simtime, stepsize)
    for ii, tab in enumerate(tables):
        subplot(len(tables), 1, ii+1)
        t = np.linspace(0, simtime, len(tab.vector))*1e3
        plot(t, tab.vector*1e3, label=tab.name)
        legend()
    show()
# 
# multicomp_lif.py ends here
