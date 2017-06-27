#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
# This snippet shows the use of several objects.
# This snippet sets up a StimulusTable to control a RandSpike which
# sends its outputs to two places: to a SimpleSynHandler on an IntFire,
# which is used to monitor spike arrival, and to various Stats objects.
# I record and plot each of these.
# The StimulusTable has a sine-wave waveform
#
import numpy
import pylab
import moose

dt = 0.01
runtime = 100

def make_model():
    sinePeriod = 50
    maxFiringRate = 10
    refractT = 0.05

    for i in range( 20 ):
        moose.setClock( i, dt )

    ############### Create objects ###############
    stim = moose.StimulusTable( 'stim' )
    spike = moose.RandSpike( 'spike' )
    syn = moose.SimpleSynHandler( 'syn' )
    fire = moose.IntFire( 'fire' )
    stats1 = moose.SpikeStats( 'stats1' )
    stats2 = moose.Stats( 'stats2' )
    plots = moose.Table( 'plots' )
    plot1 = moose.Table( 'plot1' )
    plot2 = moose.Table( 'plot2' )
    plotf = moose.Table( 'plotf' )

    ############### Set up parameters ###############
    stim.vector = [ maxFiringRate *
            numpy.sin(x * 2 * numpy.pi / sinePeriod)
            for x in range( sinePeriod )]
    stim.startTime = 0
    stim.stopTime = sinePeriod
    stim.loopTime = sinePeriod
    stim.stepSize = 0
    stim.stepPosition = 0
    stim.doLoop = 1

    spike.refractT = refractT
    syn.synapse.num = 1
    syn.synapse[0].weight = 1
    syn.synapse[0].delay = 0

    fire.thresh = 100 # Don't want it to spike, just to integrate
    fire.tau = 1.0 / maxFiringRate

    stats1.windowLength = int( 1/dt )
    stats2.windowLength = int( 1/dt )

    ############### Connect up circuit ###############
    moose.connect( stim, 'output', spike, 'setRate' )
    moose.connect( spike, 'spikeOut', syn.synapse[0], 'addSpike' )
    moose.connect( spike, 'spikeOut', stats1, 'addSpike' )
    moose.connect( syn, 'activationOut', fire, 'activation' )
    moose.connect( stats2, 'requestOut', fire, 'getVm' )
    moose.connect( plots, 'requestOut', stim, 'getOutputValue' )
    moose.connect( plot1, 'requestOut', stats1, 'getWmean' )
    moose.connect( plot2, 'requestOut', stats2, 'getWmean' )
    moose.connect( plotf, 'requestOut', fire, 'getVm' )

def main():
    """
    This snippet shows the use of several objects.
    This snippet sets up a StimulusTable to control a RandSpike which
    sends its outputs to two places:

    to a SimpleSynHandler on an IntFire, which is used to monitor spike arrival,
    and to various Stats objects.

    I record and plot each of these.
    The StimulusTable has a sine-wave waveform
    """
    make_model()

    moose.reinit()
    moose.start( runtime )
    plots = moose.element( '/plots' )
    plot1 = moose.element( '/plot1' )
    plot2 = moose.element( '/plot2' )
    plotf = moose.element( '/plotf' )
    t = [i * dt for i in range( plot1.vector.size )]
    pylab.plot( t, plots.vector, label='stimulus' )
    pylab.plot( t, plot1.vector, label='spike rate mean' )
    pylab.plot( t, plot2.vector, label='Vm mean' )
    pylab.plot( t, plotf.vector, label='Vm' )
    pylab.legend()
    pylab.show()


    '''
    moose.useClock( 0, '/stim', 'process' )
    moose.useClock( 1, '/spike', 'process' )
    moose.useClock( 2, '/syn', 'process' )
    moose.useClock( 3, '/fire', 'process' )
    moose.useClock( 4, '/stats#', 'process' )
    moose.useClock( 8, '/plot#', 'process' )
    for i in range (10):
        moose.setClock( i, dt )
    moose.useClock( 8, '/plot#', 'process' )
    '''
# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()
