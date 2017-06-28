# intfire.py ---
#
# Filename: intfire.py
# Description:
# Author:Subhasis Ray
# Maintainer:
# Created: Thu Jun 21 16:40:25 2012 (+0530)
# Version:
# Last-Updated: Sat Jun 23 13:44:10 2012 (+0530)
#           By: subha
#     Update #: 35
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Code snippet to show some operations on IntFire.
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
    Demonstrates connection between 2 IntFire neurons to observe spike generation.
"""
import moose

def connect_two_intfires():
    """
    Connect two IntFire neurons so that spike events in one gets
    transmitted to synapse of the other.
    """
    if1 = moose.IntFire('/if1')
    if2 = moose.IntFire('/if2')
    sf1 = moose.SimpleSynHandler( '/if1/sh' )
    moose.connect( sf1, 'activationOut', if1, 'activation' )
    sf1.synapse.num = 1
    syn1 = moose.element(sf1.synapse)
    # Connect the spike message of if2 to the first synapse on if1
    moose.connect(if2, 'spikeOut', syn1, 'addSpike')

def connect_spikegen():
    """
    Connect a SpikeGen object to an IntFire neuron such that spike
    events in spikegen get transmitted to the synapse of the IntFire
    neuron.
    """
    if3 = moose.IntFire('/if3')
    sf3 = moose.SimpleSynHandler( '/if3/sh' )
    moose.connect( sf3, 'activationOut', if3, 'activation' )
    sf3.synapse.num = 1
    sg = moose.SpikeGen('sg')
    syn = moose.element(sf3.synapse)
    moose.connect(sg, 'spikeOut', syn, 'addSpike')

def setup_synapse():
    """
    Create an intfire object and create two synapses on it.

    """
    if4 = moose.IntFire('if4')
    sf4 = moose.SimpleSynHandler( 'if4/sh' )
    sg1 = moose.SpikeGen('sg1')
    sg2 = moose.SpikeGen('sg2')
    sf4.synapse.num = 2 # set synapse count to 2
    sf4.synapse[0].weight = 0.5
    sf4.synapse[0].delay = 1e-3
    sf4.synapse[1].weight = 2.0
    sf4.synapse[1].delay = 2e-3
    moose.connect(sg1, 'spikeOut', sf4.synapse[0], 'addSpike')
    moose.connect(sg2, 'spikeOut', sf4.synapse[1], 'addSpike')

def main():
    """
    Demonstrates connection between 2 IntFire neurons to observe
    spike generation.

    """
    connect_two_intfires()
    connect_spikegen()
    setup_synapse()

#
# intfire.py ends here
