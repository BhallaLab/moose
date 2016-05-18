#########################################################################
## diffEqSolution.py
##
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

import numpy
import moose
import pylab

runtime = 10
chemdt = 0.05
tgtCaInitConc = 50e-6

def makeReacs():
    # Parameters
    volume = 1e-15
    CaInitConc = 60e-6
    NA = 6.022e23
    tauI = 1
    tauG = 0.1

    model = moose.Neutral( '/cells' )
    compartment = moose.CubeMesh( '/cells/compartment' )
    compartment.volume = volume

    # Make pools
    Ca = moose.BufPool( '/cells/compartment/Ca' )
    tgtCa = moose.BufPool( '/cells/compartment/tgtCa' )
    m = moose.Pool( '/cells/compartment/m' )
    chan = moose.Pool( '/cells/compartment/chan' )

    # Make Funcs
    f1 = moose.Func( '/cells/compartment/f1' )
    f2 = moose.Func( '/cells/compartment/f2' )

    # connect up
    moose.connect( f1, 'valueOut', m, 'increment' )
    moose.connect( f2, 'valueOut', chan, 'increment' )

    moose.connect( Ca, 'nOut', f1, 'xIn' )
    moose.connect( tgtCa, 'nOut', f1, 'yIn' )

    moose.connect( m, 'nOut', f2, 'xIn' )
    moose.connect( chan, 'nOut', f2, 'yIn' )

    # Set params
    Ca.concInit = CaInitConc
    tgtCa.concInit = tgtCaInitConc
    m.concInit = 0.0
    chan.concInit = 0.0
    volscale = 1.0

    f1.expr = str( volscale / tauI ) + " * (x-y)"
    f2.expr = str( volscale / tauG ) + " * (x-y)"

    #Plotting
    channelPlot = makePlot( 'channelConc', chan, 'Conc', 18 )
    mPlot = makePlot( 'mConc', m, 'Conc', 18 )
    caPlot = makePlot( 'Ca', Ca, 'Conc', 18 )
    targetPlot = makePlot( 'tgtCa', tgtCa, 'Conc', 18 )
    return (channelPlot, mPlot, caPlot )

def makePlot( name, src, field, tick ):
    tab = moose.Table( '/graphs/' + name )
    moose.connect( tab, 'requestOut', src, 'get' + field )
    tab.tick = tick
    return tab

def main():
    """
    This snippet illustrates the solution of an arbitrary set of 
    differential equations using the **Func** class and the **Pool** 
    class. The equations solved here are::

        tauI.m' = Ca - Ca_tgt
        tauG.chan' = m - chan

    These equations are taken from: O'Leary et al *Neuron* 2014.
    
    **Func** evaluates an arbitrary function each timestep.
    Here this function is the rate of change from the equations above.
    The rate of change is passed to the *increment* message of the 
    **Pool**. The numerical integration method is the Exponential
    Euler method but this will work fine if the rates are slow compared to
    the simulation timestep.
    
    Conceptually, the idea is that if Ca is greater than the target level,
    then more mRNA is made, which makes more channels.
    Although the equations have no upper or lower bounds on **m** or 
    **chan**, MOOSE is sensible about preventing the molecular pools from
    having negative concentrations. This does mean that the solution method 
    employed here won't work for the general solution of differential 
    equations in non-chemical systems.
    """
    elecdt = 25e-6
    eplotdt = 0.5e-3
    plotdt = 0.1 # s
    graphs = moose.Neutral( '/graphs' )
    plots = makeReacs()
    for i in range( 0, 18 ):
        moose.setClock( i, chemdt )
    moose.setClock( 18, plotdt )
    moose.setClock( 8, eplotdt )

    moose.reinit()
    moose.start( runtime )
    moose.element( '/cells/compartment/Ca' ).concInit = tgtCaInitConc/2
    moose.start( runtime )
    tvec = [ i * plotdt for i in range( plots[0].vector.size ) ]

    for x in moose.wildcardFind( '/graphs/#' ):
        pylab.plot(tvec, x.vector, label = x.name )
    pylab.xlabel('time (s)')
    pylab.ylabel('voltage (v)')
    pylab.legend()
    pylab.show()


if __name__ == "__main__":
    main()
