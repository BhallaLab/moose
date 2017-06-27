# pulsegen2.py ---
#
# Filename: pulsegen2.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Wed Jul  8 17:06:23 2015 (+0530)
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
# Recovered old demo from moose branch (beta 1.4) and converted to
# current API.
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
import pylab
import numpy
RUNTIME = 200.0
SIMDT = 1.0

def main():
    """
This example shows the full range of operations of PulseGen objects
with a reimplementation of corresponding GENESIS demo.

A PulseGen object can be run in three modes: free running
(trigMode=0), triggered (trigMode=1) and gated (trigMode=2).

In the free running mode it keeps repeating the pulse series
indefinitely.

In triggered mode, it generates a pulse series on the leading edge of
the trigger signal coming to its `input` field. The trigger can be the
`output` of another PulseGen as in this example.

In gated mode, the PulseGen acts as if it was free-running as long as
the `input` remains high.

    """

    # Pulse generator with trigger mode = 0 This is free running - and
    # creates a series of pulses
    pulse0 = moose.PulseGen("/pulse0")
    pulse0.level[0] = 50.0
    pulse0.width[0] = 3.0
    pulse0.delay[0] = 5.0
    pulse0.level[1] = -20.0
    pulse0.width[1] = 5.0
    pulse0.delay[1] = 8.0
    pulse0.baseLevel = 10.0
    pulse0.trigMode = 0

    trig = moose.PulseGen("/trig")
    trig.level[0] = 20.0
    trig.width[0] = 1.0
    trig.delay[0] = 5.0
    trig.width[1] = 30.0

    # Pulse generator with trigger mode = 1
    pulse1 = moose.PulseGen("/pulse1")
    pulse1.level[0] = 50.0
    pulse1.width[0] = 3.0
    pulse1.delay[0] = 5.0
    pulse1.level[1] = -20.0
    pulse1.width[1] = 5.0
    pulse1.delay[1] = 8.0
    pulse1.baseLevel = 10.0
    pulse1.trigMode = 1
    trig.connect("output", pulse1, "input")

    # Gated pulse
    gate = moose.PulseGen("/gate")
    gate.level[0] = 20.0
    gate.width[0] = 30.0
    gate.delay[0] = 15.0
    gate.width[1] = 30.0

    # Pulse generator with trigger mode = 2
    pulse2 = moose.PulseGen("/pulse2")
    pulse2.level[0] = 50.0
    pulse2.width[0] = 3.0
    pulse2.delay[0] = 5.0
    pulse2.level[1] = -20.0
    pulse2.width[1] = 5.0
    pulse2.delay[1] = 8.0
    pulse2.baseLevel = 10.0
    pulse2.trigMode = 2
    gate.connect("output", pulse2, "input")

    plot0 = moose.Table("/plot0")
    plot0.connect("requestOut", pulse0, "getOutputValue")

    plot1 = moose.Table("/plot1")
    plot1.connect("requestOut", pulse1, "getOutputValue")

    plot2 = moose.Table("/plot2")
    plot2.connect("requestOut", pulse2, "getOutputValue")

    plotGate = moose.Table("/plotGate")
    plotGate.connect("requestOut", gate, "getOutputValue")

    plotTrig = moose.Table("/plotTrig")
    plotTrig.connect("requestOut", trig, "getOutputValue")

    moose.useClock(0, "/#[TYPE=PulseGen]", 'process')
    moose.useClock(1, "/#[TYPE=Table]", 'process')
    moose.setClock(0, SIMDT)
    moose.setClock(1, SIMDT)
    moose.reinit()
    moose.start(RUNTIME)

    pylab.subplot(511)
    pylab.plot(plot0.vector)
    pylab.title('Free Run')
    pylab.subplot(512)
    pylab.plot(plot1.vector)
    pylab.title('Triggered (below)')
    pylab.subplot(513)
    pylab.plot(plotTrig.vector)
    pylab.title('Free Running Trigger')
    pylab.subplot(514)
    pylab.plot(plot2.vector)
    pylab.title('Gated (below)')
    pylab.subplot(515)
    pylab.plot(plotGate.vector)
    pylab.title('Free Running Gate')
    pylab.tight_layout()
    pylab.show()
    print("pulsegen.py: finished simulation")

if __name__ == '__main__':
    main()
#
# pulsegen2.py ends here
