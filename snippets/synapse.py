# synapse.py ---
#
# Filename: synapse.py
# Description:
# Author:Upi Bhalla
# Maintainer:
# Created: Sat Jun 29 11:32:29 2013 (+0530)
# Version:
# Last-Updated: Thu Oct  2 2014 (+0530)
#           By: Upi
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

from __future__ import print_function


import moose

def many_ematrix_to_one_element():
    model = moose.Neutral('/model')
    # data = moose.Neutral('/data')
    synchan = moose.SynChan('/model/synchan')
    synh = moose.SimpleSynHandler( '/model/synchan/synh' )
    moose.connect( synh, 'activationOut', synchan, 'activation' )
    synh.synapse.num = 2
    num_spikegen = 5
    spikegens = [moose.SpikeGen('/model/spikegen_%d' % (ii)) for ii in range(num_spikegen)]
    for ii in range(num_spikegen):
        msg = moose.connect(spikegens[ii], 'spikeOut', synh.synapse[ii%2], 'addSpike')
        # synchan.synapse[ii].delay = ii  * 1e-3
        # synchan.synapse[ii].weight = (ii+1) * 0.1
    for sg in spikegens:
        print(sg.path, '-->', end=' ')
        for m in sg.msgOut:
            print(moose.element(m).adjacent[sg].path)

def main():
    """
    This is an example of event messages from multiple SpikeGen objects
    into a synchan.

    Create a SynChan element with 2 elements in synapse field.

    Create 5 SpikeGen elements.

    Connect alternet SpikeGen elements to synapse[0] and synapse[1]

    ... This is a minimal example. In real simulations the SpikeGens
    will be embedded in compartments representing axon terminals and
    the SynChans will be embedded in somatic/dendritic compartments.

    """
    many_ematrix_to_one_element()

if __name__ == '__main__':
    main()
#
# synapse.py ends here
