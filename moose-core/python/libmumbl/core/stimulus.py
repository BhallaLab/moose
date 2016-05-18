"""
File: stimulus.py
Author: Dilawar Singh
Created on: 2013-11-25
Description: generates stimuous
"""

import random
import numpy as np

def countSpikes(spikeArray):
    count = 0
    for i in spikeArray:
        if i == True:
            count += 1
    return count

def generateSpikeTrainPoission(frequency, dt=1e-3, simTime=100e-3
                               , refractoryPeriod=1e-3
                               ):
    """
    Generate a Poission spike train.

    All units are in SI.

    @return:  A numpy array of time when there is a spike.

    @raise e:  Description
    """

    frequency = float(frequency)

    def checkIfRefractoryPeriodIsViolated(simArray):
        """
        Local function to check if refractory period is violated.
        """
        deadPeriod = int(refractoryPeriod / dt)
        indicesOfViolation = []
        for i in range(simArray.size - deadPeriod):
            if simArray[i] == True:
                for j in range(1, deadPeriod+1):
                    if simArray[i+j] == True:
                        indicesOfViolation.append((i, i+j))
        return indicesOfViolation


    def fixRefractoryViolation(simArray):
        """
        Fix violation of refractory period, if any. We are just deleting the
        spike. Can't think of a fast solution.
        """
        deadPeriod = int(refractoryPeriod / dt)
        for i in range(simArray.size - deadPeriod):
            if simArray[i] == True:
                for j in range(1, deadPeriod+1):
                    if simArray[i+j] == True:
                        simArray[i+j] = False
        return simArray


    arraySize = int(simTime/dt)
    # Initialize array to hold absense or presence of spike. By default there is
    # no spike.
    simArray = np.zeros(arraySize, dtype=np.bool_)
    stepSize = dt * frequency
    for i in range(arraySize):
        # generate a random number between 0 and 1 and check if it is larger
        # than (frequency*dt)/1000 (sec)
        n = random.random()
        if n < stepSize:
            simArray[i] = True
    # If refractory period is given then shift invalid firing to next location.
    simArray = fixRefractoryViolation(simArray)
    # We need to return only time on which there is a spike.
    eventArray = np.array([], dtype=np.float_)
    for i in range(simArray.size):
        if simArray[i] == True:
            eventArray = np.append(eventArray, i*dt)
    return eventArray


"""


Following functions create two time tables.  #1 is filled with entries in a
numpy array and #2 is filled from a text file containing the event times.

The `state` field of #1, which becomes 1 when an event occurs and 0
otherwise, is recorded.

On the other hand, #2 is connected to a synapse (in a SynChan element)
to demonstrate artificial spike event generation.

"""

import numpy as np

simdt = 1e-3
simtime = 100

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

def timetable_nparray(simtime=100):
    """Create a time table and populate it with numpy array. The `vec`
    field in a Table can be directly assigned a sequence to fill the
    table entries.

    """
    times = generate_poisson_times(rate=20, simtime=simtime, seed=1)
    model = moose.Neutral('/model')
    tt = moose.TimeTable('%s/tt_array' % (model.path))
    tt.vec = times
    return tt, times,

def timetable_file(filename='timetable.txt', simtime=100):
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
    print(sp_array)
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
    synchan.synapse.num = 1
    moose.connect(tt_file, 'event', moose.element(synchan.path + '/synapse'), 'addSpike')
    # Data recording: record the `state` of the time table filled
    # using array.
    data = moose.Neutral('/data')
    tab_array = moose.Table('/data/tab_array')
    moose.connect(tab_array, 'requestData', tt_array, 'get_state')
    # Record the synaptic conductance for the other time table, which
    # is filled from a text file and sends spike events to a synchan.
    tab_file = moose.Table('/data/tab_file')
    moose.connect(tab_file, 'requestData', synchan, 'get_Gk')

    # Scheduling
    moose.setClock(0, simdt)
    moose.setClock(1, simdt)

    # Using clocks
    moose.useClock(1, '/model/##[ISA=Compartment]', 'init')
    moose.useClock(1, '/model/##,/data/##', 'process')
    moose.useClock(0, '/neuroml/cells/##', 'process')
    moose.reinit()
    moose.start(simtime)

    # Plotting
    pylab.subplot(2,1,1)
    pylab.plot(sp_array, np.ones(len(sp_array)), 'rx', label='spike times from numpy array')
    pylab.plot(np.linspace(0, simtime, len(tab_array.vec)), tab_array.vec, 'b-', label='TimeTable state')
    pylab.legend()
    pylab.subplot(2,1,2)
    pylab.plot(sp_file, np.ones(len(sp_file)), 'rx', label='spike times from file')
    pylab.plot(np.linspace(0, simtime, len(tab_file.vec)), tab_file.vec*1e6, 'b-', label='Syn Gk (uS)')
    pylab.legend()
    pylab.show()

if __name__ == '__main__':
    import moose
    import os
    import pylab
    timetable_demo()


