# startstop.py --- 
# 
# Filename: startstop.py
# Description: 
# Author:Subhasis Ray 
# Maintainer: 
# Created: Sun Jul  1 13:42:28 2012 (+0530)
# Version: 
# Last-Updated: Sun Jul  1 14:41:34 2012 (+0530)
#           By: subha
#     Update #: 62
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

import pylab
import moose
            
def main():
    """
    This demo shows how to start, stop, and continue a simulation. 
    This is commonly done when we want to run a model till settling, then
    change a parameter or deliver a stimulus, and then continue the
    simulation.

    Here, the model is just the output of a PulseGen object which 
    generates periodic pulses.
    The demo shows how to start the simulation. using the 
    *moose.reinit* command to reset the model to its initial state,
    and *moose.start* command to run the model for the specified duration. 
    We issue multiple *moose.start* commands and do different things to
    the model between them. First, we change the delay of the pulseGen.
    Then we show a number of ways to assign the timestep (dt) to the
    table object in the simulation.
    Note that throughout this simulation the pulsegen is going at a 
    uniform rate, it is just being sampled by the output table at
    different intervals.
    """

    dt = 0.1
    steps = 100
    simtime = dt * steps
    # Pulsegen is on tick 0, we can pre-emptively set its dt.
    moose.setClock(0, dt) 
    table = setup_model()
    pulse = moose.element( '/model/pulse' )
    # The 'tick' field is on every object, we can use this to set its dt.
    moose.setClock( table.tick, dt )
    moose.reinit()
    clock = moose.element('/clock')
    print dt
    print 'dt = ', dt, ', Total simulation time = ', simtime
    print 'Running simulation for', simtime, 'seconds'
    moose.start( simtime )
    print 'Simulator time:', clock.currentTime
    # Here we change the pulse delay and then run again.
    pulse.delay[0] = 1.0
    moose.start( simtime )

    # We change the table tick and use a different dt for it:
    table.tick = 2
    moose.setClock( table.tick, dt * 2 )
    moose.start( simtime )

    # Here is yet another way to change clocks used by the table
    moose.useClock( 9, '/model/pulse/tab', 'process' )
    print table.tick
    moose.setClock( 9, dt / 2.0 )
    moose.start( simtime )

    # Finally, here we change the pulse delay to 1 second and run again.

    print 'Simulator time at end of simulation', clock.currentTime
    pylab.plot(pylab.linspace(0, clock.currentTime, len(table.vector)), table.vector)    
    pylab.show()

def setup_model():
    model_container = moose.Neutral('/model')    
    pulse = moose.PulseGen('/model/pulse')
    pulse.level[0] = 1.0
    pulse.delay[0] = 0.5
    pulse.width[0] = 0.5
    table = moose.Table('%s/tab' % (pulse.path))
    moose.connect(table, 'requestOut', pulse, 'getOutputValue')
    return table

if __name__ == '__main__':
    main()
