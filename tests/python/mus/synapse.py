# -*- coding: utf-8 -*-
# synapse.py ---
#
# Filename: synapse.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Mon May 26 11:29:54 2014 (+0530)
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

import moose

def make_NMDA(comp):
    """This function is modified from proto18.py by Upi, which is again
    based on Dave Beeman's implementation.

    This implementation of NMDA uses the formulation by Zador, Koch
    and Brown (1990) where

    Gk = Gbar * (exp(- t / tau1) - exp(-t / tau2)) / (1 + eta * [Mg2+] * exp(- gmma * Vm))

    The MgBlock object sits between the SynChan and teh Compartment
    and scales the Gk of regular SynChan Gk by [Mg2+] such that the
    resulting conductance is as above formula, where the fields are :

    CMg = [Mg2+]
    KMg_A = 1 / eta
    KMg_B = 1 / gamma

    """
    nmda = moose.SynChan('%s/nmda' % (comp.path))
    nmda.synapse.num = 1
    nmda.Ek = 0.0
    nmda.tau1 = 20e-3
    nmda.tau2 = 20e-3
    nmda.Gbar = 0.2 * 8e-9 # channel open probability is 0.2
    nmda.bufferTime = 0.0001
    mgblock = moose.MgBlock('%s/mgblock' % (nmda.path))
    mgblock.CMg = 1.2		#	[Mg] in mM
    mgblock.Zk = 2
    mgblock.KMg_A = 1.0/0.28
    mgblock.KMg_B = 1.0/62
    moose.connect( nmda, 'channelOut', mgblock, 'origChannel', 'OneToOne' )
    moose.connect(mgblock, 'channel', comp, 'channel')
    return nmda, mgblock

def make_AMPA(comp):
    ampa = moose.SynChan('%s/ampa' % (comp.path))
    ampa.synapse.num = 1
    ampa.tau1 = 2e-3
    ampa.Gbar = 0.5e-9
    ampa.bufferTime = 0.0001
    moose.connect(ampa, 'channel', comp, 'channel')
    return ampa

def make_synapse(comp):
    nmda, mgblock = make_NMDA(comp)
    ampa = make_AMPA(comp)
    spikegen = moose.SpikeGen('%s/spike' % (comp.path))
    moose.connect(spikegen, 'spikeOut', ampa.synapse[0], 'addSpike')
    moose.connect(spikegen, 'spikeOut', nmda.synapse[0], 'addSpike')
    return {'ampa': ampa,
            'nmda': nmda,
            'mgblock': mgblock,
            'spike': spikegen
    }



#
# synapse.py ends here
