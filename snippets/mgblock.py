# mgblock.py ---
#
# Filename: mgblock.py
# Description:
# Author:Upi Bhalla
# Maintainer:
# Created: Wed Jul  3 09:36:06 2013 (+0530)
# Version:
# Last-Updated: Fri Sep 12 10:56:18 2014 (+0530)
#           By: Upi
#     Update #:
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
plotdt = 1e-4

def test_mgblock():
    model = moose.Neutral('/model')
    data = moose.Neutral('/data')
    soma = moose.Compartment('/model/soma')
    soma.Em = -60e-3
    soma.Rm = 1e7
    soma.Cm = 1e-9

    ###################################################
    # This is where we create the synapse with MgBlock
    #--------------------------------------------------
    nmda = moose.SynChan('/model/soma/nmda')
    nmda.Gbar = 1e-9
    mgblock = moose.MgBlock('/model/soma/mgblock')
    mgblock.CMg = 2.0
    mgblock.KMg_A = 1/0.33
    mgblock.KMg_B = 1/60.0

    # The synHandler manages the synapses and their learning rules if any.
    synHandler = moose.SimpleSynHandler( '/model/soma/nmda/handler' )
    synHandler.synapse.num = 1
    moose.connect( synHandler, 'activationOut', nmda, 'activation' )



    # MgBlock sits between original channel nmda and the
    # compartment. The origChannel receives the channel message from
    # the nmda SynChan.
    moose.connect(soma, 'VmOut', nmda, 'Vm' )
    moose.connect(nmda, 'channelOut', mgblock, 'origChannel')
    moose.connect(mgblock, 'channel', soma, 'channel')
    # This is for comparing with MgBlock
    nmda_noMg = moose.copy(nmda, soma, 'nmda_noMg')
    moose.connect( nmda_noMg, 'channel', soma, 'channel')
    moose.le( nmda_noMg )

    #########################################
    # The rest is for experiment setup
    spikegen = moose.SpikeGen('/model/spike')
    pulse = moose.PulseGen('/model/input')
    pulse.delay[0] = 10e-3
    pulse.level[0] = 1.0
    pulse.width[0] = 50e-3
    moose.connect(pulse, 'output', spikegen, 'Vm')
    moose.le( synHandler )
    #syn = moose.element(synHandler.path + '/synapse' )
    syn = synHandler.synapse[0]
    syn.delay = simdt * 2
    syn.weight = 10
    moose.connect(spikegen, 'spikeOut', synHandler.synapse[0], 'addSpike')
    moose.le( nmda_noMg )
    noMgSyn = moose.element(nmda_noMg.path + '/handler/synapse' )
    noMgSyn.delay = 0.01
    noMgSyn.weight = 1
    moose.connect(spikegen, 'spikeOut', noMgSyn, 'addSpike')
    moose.showfields( syn )
    moose.showfields( noMgSyn )
    Gnmda = moose.Table('/data/Gnmda')
    moose.connect(Gnmda, 'requestOut', mgblock, 'getGk')
    Gnmda_noMg = moose.Table('/data/Gnmda_noMg')
    moose.connect(Gnmda_noMg, 'requestOut', nmda_noMg, 'getGk')
    Vm = moose.Table('/data/Vm')
    moose.connect(Vm, 'requestOut', soma, 'getVm')
    for i in range( 10 ):
        moose.setClock( i, simdt )
    moose.setClock( Gnmda.tick, plotdt )
    print((spikegen.dt, Gnmda.dt))
    moose.reinit()
    moose.start( simtime )
    t = pylab.linspace(0, simtime*1e3, len(Vm.vector))
    pylab.plot(t, (Vm.vector + 0.06) * 1000, label='Vm (mV)')
    pylab.plot(t, Gnmda.vector * 1e9, label='Gnmda (nS)')
    pylab.plot(t, Gnmda_noMg.vector * 1e9, label='Gnmda no Mg (nS)')
    pylab.legend()
    #data = pylab.vstack((t, Gnmda.vector, Gnmda_noMg.vector)).transpose()
    #pylab.savetxt('mgblock.dat', data)
    pylab.show()

def main():
    """
    Demonstrates the use of MgBlock.
    Creates an NMDA channel with MgBlock and another without.
    Connects them up to the compartment on one hand, and to a
    SynHandler on the other, so as to receive synaptic input.
    Delivers two pulses to each receptor, with a small delay in between.

    Plots out the conductance change at each receptor and the reslting
    membrane potential rise at the compartment.

    Note that these NMDA channels do NOT separate out the contributions
    due to calcium and other ions. To do this correctly one should use
    the GHK object.
    """
    test_mgblock()

if __name__ == '__main__':
    main()
#
# mgblock.py ends here
