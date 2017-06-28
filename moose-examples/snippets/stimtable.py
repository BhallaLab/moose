# stimtable.py ---
#
# Filename: stimtable.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Wed May  8 18:51:07 2013 (+0530)
# Version:
# Last-Updated: Mon May 27 21:15:36 2013 (+0530)
#           By: subha
#     Update #: 124
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

import numpy as np
from matplotlib import pyplot as plt
import moose
from moose import utils

def stimulus_table_demo():
    """
    Example of StimulusTable using Poisson random numbers.
    Creates a StimulusTable and assigns it signal representing events in a
    Poisson process. The output of the StimTable is sent to a DiffAmp
    object for buffering and then recorded in a regular table.
    """

    model = moose.Neutral('/model')
    data = moose.Neutral('/data')
    # This is the stimulus generator
    stimtable = moose.StimulusTable('/model/stim')
    recorded = moose.Table('/data/stim')
    moose.connect(recorded, 'requestOut', stimtable, 'getOutputValue')
    simtime = 100
    simdt = 1e-3
    # Inter-stimulus-intervals with rate=20/s
    rate = 20
    np.random.seed(1) # ensure repeatability
    isi = np.random.exponential(rate, int(simtime/rate))
    # The stimulus times are the cumulative sum of the inter-stimulus intervals.
    stimtimes = np.cumsum(isi)
    # Select only stimulus times that are within simulation time -
    # this may leave out some possible stimuli at the end, but the
    # exoected number of Poisson events within simtime is
    # simtime/rate.
    stimtimes = stimtimes[stimtimes < simtime]
    ts = np.arange(0, simtime, simdt)
    # Find the indices of table entries corresponding to time of stimulus
    stimidx = np.searchsorted(ts, stimtimes)
    stim = np.zeros(len(ts))
    # Since linear interpolation is forced, we need at least three
    # consecutive entries to have same value to get correct
    # magnitude. And still we shall be off by at least one time step.
    indices = np.concatenate((stimidx-1, stimidx, stimidx+1))
    stim[indices] = 1.0
    stimtable.vector = stim
    stimtable.stepSize = 0 # This forces use of current time as x value for interpolation
    stimtable.stopTime = simtime
    moose.setClock(0, simdt)
    moose.useClock(0, '/model/##,/data/##', 'process')
    moose.reinit()
    moose.start(simtime)
    plt.plot(np.linspace(0, simtime, len(recorded.vector)), recorded.vector, 'r-+', label='generated stimulus')
    plt.plot(ts, stim, 'b-x', label='originally assigned values')
    plt.ylim((-1, 2))
    plt.legend()
    plt.title('Exmaple of StimulusTable')
    plt.show()

def main():
	"""
Example of StimulusTable using Poisson random numbers.

Creates a StimulusTable and assigns it signal representing events in a
Poisson process. The output of the StimTable is sent to a DiffAmp
object for buffering and then recorded in a regular table.

	"""
	stimulus_table_demo()

if __name__ == '__main__':
	main()

# stimtable.py ends here
