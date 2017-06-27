# timetable.py ---
#
# Filename: timetable.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Wed Jun 19 19:37:00 2013 (+0530)
# Version:
# Last-Updated: Thu Oct 02
#           By: Upi
#     Update #: ?
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
"""Demonstrates the use of TimeTable elements in MOOSE.

This script creates two time tables, #1 is filled with entries in a
numpy array and #2 is filled from a text file containing the event
times.

The `state` field of #1, which becomes 1 when an event occurs and 0
otherwise, is recorded.

On the other hand, #2 is connected to a synapse (in a SynChan element)
to demonstrate artificial spike event generation.

"""

import os
import numpy as np
import moose

import pylab

simtime = 100
simdt = 1e-3

def generate_poisson_times(rate=20, simtime=100, seed=1):
    """Generate Poisson spike times using `rate`. Use `seed` for seeding
    the numpy rng"""
    np.random.seed(seed) # ensure repeatability
    isi = np.random.exponential(rate, int(simtime/rate))
    # The stimulus times are the cumulative sum of the inter-stimulus intervals.
    stimtimes = np.cumsum(isi)
    # Select only stimulus times that are within simulation time -
    # this may leave out some possible stimuli at the end, but the
    # exoected number of Poisson events within simtime is
    # simtime/rate.
    stimtimes = stimtimes[stimtimes < simtime]
    return stimtimes

def timetable_nparray():
    """Create a time table and populate it with numpy array. The `vec`
    field in a Table can be directly assigned a sequence to fill the
    table entries.

    """
    times = generate_poisson_times(rate=20, simtime=simtime, seed=1)
    model = moose.Neutral('/model')
    tt = moose.TimeTable('%s/tt_array' % (model.path))
    tt.vector = times
    return tt, times,

def timetable_file(filename='timetable.txt'):
    """Create a TimeTable and populate it from file specified by
    `filename`. If `filename` does not exist, a file of the same name is
    created and a random series of spike times is saved in it"""
    if not os.access(filename, os.R_OK):
        times = generate_poisson_times(rate=10, simtime=simtime, seed=1)
        np.savetxt(filename, times)
    model = moose.Neutral('/model')
    tt = moose.TimeTable('%s/tt_file' % (model.path))
    tt.filename = filename
    return tt, np.loadtxt(filename),

def timetable_demo():
    tt_array, sp_array = timetable_nparray()
    tt_file, sp_file = timetable_file()
    # Create a synchan inside a compartment to demonstrate how to use
    # TimeTable to send artificial spike events to a synapse.
    comp = moose.Compartment('/model/comp')
    comp.Em = -60e-3
    comp.Rm = 1e9
    comp.Cm = 1e-12
    synchan = moose.SynChan('/model/comp/synchan')
    synchan.Gbar = 1e-6
    synchan.Ek = 0.0
    moose.connect(synchan, 'channel', comp, 'channel')
    synh = moose.SimpleSynHandler( '/model/comp/synchan/synh' )
    moose.connect( synh, 'activationOut', synchan, 'activation')
    synh.synapse.num = 1
    moose.connect(tt_file, 'eventOut', moose.element(synh.path + '/synapse'), 'addSpike')
    # Data recording: record the `state` of the time table filled
    # using array.
    data = moose.Neutral('/data')
    tab_array = moose.Table('/data/tab_array')
    moose.connect(tab_array, 'requestOut', tt_array, 'getState')
    # Record the synaptic conductance for the other time table, which
    # is filled from a text file and sends spike events to a synchan.
    tab_file = moose.Table('/data/tab_file')
    moose.connect(tab_file, 'requestOut', synchan, 'getGk')

    # Scheduling
    moose.setClock(0, simdt)
    moose.setClock(1, simdt)
    moose.useClock(1, '/model/##[ISA=Compartment]', 'init')
    moose.useClock(1, '/model/##,/data/##', 'process')
    moose.reinit()
    moose.start(simtime)

    # Plotting
    pylab.subplot(2,1,1)
    pylab.plot(sp_array, np.ones(len(sp_array)), 'rx', label='spike times from numpy array')
    pylab.plot(np.linspace(0, simtime, len(tab_array.vector)), tab_array.vector, 'b-', label='TimeTable state')
    pylab.legend()
    pylab.subplot(2,1,2)
    pylab.plot(sp_file, np.ones(len(sp_file)), 'rx', label='spike times from file')
    pylab.plot(np.linspace(0, simtime, len(tab_file.vector)), tab_file.vector*1e6, 'b-', label='Syn Gk (uS)')
    pylab.legend()
    pylab.show()

def main():
    """
    Demonstrates the use of TimeTable elements in MOOSE.

    This scripts creates two time tables, #1 is filled with entries in a
    numpy array and #2 is filled from a text file containing the event
    times.

    The `state` field of #1, which becomes 1 when an event occurs and 0
    otherwise, is recorded.

    On the other hand, #2 is connected to a synapse (in a SynChan element)
    to demonstrate artificial spike event generation.

    """
    timetable_demo()




#
# timetable.py ends here
