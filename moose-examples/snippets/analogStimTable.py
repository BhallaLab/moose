# analogStimTable.py --- 
# 
# Filename: analogStimTable.py
# Description: 
# Author: Upi Bhalla
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

import numpy as np
import pylab
import moose
from moose import utils

def analogStimTable():    
    """Example of using a StimulusTable as an analog signal source in
    a reaction system. It could be used similarly to give other 
    analog inputs to a model, such as a current or voltage clamp signal.

    This demo creates a StimulusTable and assigns it half a sine wave.
    Then we assign the start time and period over which to emit the wave.
    The output of the StimTable is sent to a pool **a**, which participates
    in a trivial reaction::

        table ----> a <===> b

    The output of **a** and **b** are recorded in a regular table 
    for plotting.
    """
    simtime = 150
    simdt = 0.1
    model = moose.Neutral('/model')
    data = moose.Neutral('/data')
    # This is the stimulus generator
    stimtable = moose.StimulusTable('/model/stim')
    a = moose.BufPool( '/model/a' )
    b = moose.Pool( '/model/b' )
    reac = moose.Reac( '/model/reac' )
    reac.Kf = 0.1
    reac.Kb = 0.1
    moose.connect( stimtable, 'output', a, 'setConcInit' )
    moose.connect( reac, 'sub', a, 'reac' )
    moose.connect( reac, 'prd', b, 'reac' )
    aPlot = moose.Table('/data/aPlot')
    moose.connect(aPlot, 'requestOut', a, 'getConc')
    bPlot = moose.Table('/data/bPlot')
    moose.connect(bPlot, 'requestOut', b, 'getConc')
    moose.setClock( stimtable.tick, simdt )
    moose.setClock( a.tick, simdt )
    moose.setClock( aPlot.tick, simdt )

    ####################################################
    # Here we set up the stimulus table. It is half a sine-wave.
    stim = [ np.sin(0.01 * float(i) ) for i in range( 314 )]
    stimtable.vector = stim
    stimtable.stepSize = 0 # This forces use of current time as x value

    # The table will interpolate its contents over the time start to stop:
    # At values less than startTime, it emits the first value in table
    stimtable.startTime = 5
    # At values more than stopTime, it emits the last value in table
    stimtable.stopTime = 60
    stimtable.doLoop = 1 # Enable repeat playbacks.
    stimtable.loopTime = 10 + simtime / 2.0 # Repeat playback over this time

    moose.reinit()
    moose.start(simtime)
    t = [ x * simdt for x in range( len( aPlot.vector ) )]
    pylab.plot( t, aPlot.vector, label='Stimulus waveform' )
    pylab.plot( t, bPlot.vector, label='Reaction product b' )
    pylab.legend()
    pylab.show()

if __name__ == '__main__':
    analogStimTable()
    

# 
# stimtable.py ends here
