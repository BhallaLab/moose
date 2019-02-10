########################################################################
# This example demonstrates synaptic summation for sequential input on a 
# long cable
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from matplotlib.widgets import Slider, Button
import numpy as np
import warnings
import moose
import rdesigneur as rd

numDendSeg = 45
lines = []
tplot = []
RM = 1.0
RA = 1.0
CM = 0.01
length = 0.001
dia = 1e-6
runtime = 0.06
interval1 = 0.02
interval2 = 0.005
elecPlotDt = 0.001
inputDuration = 0.01
#onset = [ 10.0, 15.0, 20.0, 25.0 ] # onset delay in ms
rec = []
gluGbar = 10.0
nmdaGbar = 0.0001
inputFreq = 100.0

class RecInfo():
    def __init__(self, name, onset ):
        self.name = name
        self.onset = onset
    def setOnset( self, val ):
        self.onset = val/1000.0
        updateDisplay()

def makeModel():
    segLen = length / numDendSeg
    rdes = rd.rdesigneur(
        stealCellFromLibrary = True,
        elecPlotDt = elecPlotDt,
        verbose = False,
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
        # The numerical arguments are all optional
        cellProto = 
            [['ballAndStick', 'soma', dia, segLen, dia, length,numDendSeg]],
        chanProto = [['make_glu()', 'glu'],['make_NMDA()', 'nmda']],
        passiveDistrib = [[ '#', 'RM', str(RM), 'CM', str(CM), 'RA', str(RA) ]],
        chanDistrib = [
            ['glu', 'dend10,dend20,dend30,dend40', 'Gbar', str(gluGbar)],
            ['nmda', 'dend10,dend20,dend30,dend40', 'Gbar', str(nmdaGbar)],
        ],
        stimList = [
            ['dend10', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( inputFreq, rec[0].onset, rec[0].onset + inputDuration) ],
            ['dend20', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( inputFreq, rec[1].onset, rec[1].onset + inputDuration) ],
            ['dend30', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( inputFreq, rec[2].onset, rec[2].onset + inputDuration) ],
            ['dend40', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( inputFreq, rec[3].onset, rec[3].onset + inputDuration) ],
        ],
        plotList = [['soma,dend10,dend20,dend30,dend40', '1', '.', 'Vm' ]],
    )
    rdes.buildModel()
    #for i in moose.wildcardFind( '/model/elec/dend#/glu/sh/synapse/synInput_rs' ):
        #i.refractT = 0.0
    moose.connect( '/model/elec/dend10/glu/sh/synapse/synInput_rs', 'spikeOut', '/model/elec/dend10/nmda/sh/synapse', 'addSpike' )
    moose.connect( '/model/elec/dend20/glu/sh/synapse/synInput_rs', 'spikeOut', '/model/elec/dend20/nmda/sh/synapse', 'addSpike' )
    moose.connect( '/model/elec/dend30/glu/sh/synapse/synInput_rs', 'spikeOut', '/model/elec/dend30/nmda/sh/synapse', 'addSpike' )
    moose.connect( '/model/elec/dend40/glu/sh/synapse/synInput_rs', 'spikeOut', '/model/elec/dend40/nmda/sh/synapse', 'addSpike' )

def main():
    warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")
    makeDisplay()
    quit()

def updateLen(val): # This does the length of the entire cell.
    global length
    length = val * 0.001
    updateDisplay()

def updateDia(val):
    global dia
    dia = val * 1e-6
    updateDisplay()

class Toggle():
    def __init__( self, toggle, ax ):
        self.state = False
        self.toggle = toggle
        self.ax = ax

    def click( self, event ):
        global nmdaGbar
        self.state = not( self.state )
        if self.state:
            self.toggle.label.set_text( "NMDA On" )
            self.toggle.color = "green"
            self.toggle.hovercolor = "green"
            nmdaGbar = 20.0
        else:
            self.toggle.label.set_text( "NMDA Off" )
            self.toggle.color = "yellow"
            self.toggle.hovercolor = "yellow"
            nmdaGbar = 0.0001
        updateDisplay()

def updateDisplay():
    makeModel()
    vec = moose.wildcardFind( '/model/elec/#[ISA=CompartmentBase]' )
    tabvec = moose.wildcardFind( '/model/graphs/plot#' )
    #vec[0].inject = 1e-10
    moose.reinit()
    dt = interval1
    currtime = 0.0
    for i in lines:
        moose.start( dt )
        currtime += dt
        i.set_ydata( [v.Vm * 1000 for v in vec] )
        dt = interval2
        #print( "inRunSim v0={}, v10={}".format( vec[0].Vm, vec[10].Vm ) )
    moose.start( runtime - currtime )
    for i,tab in zip( tplot, tabvec ):
        i.set_ydata( tab.vector * 1000 )

    moose.delete( '/model' )
    moose.delete( '/library' )
    # Put in something here for the time-series on soma

def doQuit( event ):
    quit()

def makeDisplay():
    global tplot
    global lines
    global rec
    axOnset = []
    sliders = []
    img = mpimg.imread( 'SeqSummation.png' )
    fig = plt.figure( figsize=(10,12) )
    png = fig.add_subplot(321)
    imgplot = plt.imshow( img )
    plt.axis('off')
    ax1 = fig.add_subplot(322)
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -70, -20 )
    plt.xlabel( 'time (ms)' )
    plt.title( "Membrane potential vs time at 5 positions." )
    t = np.arange( 0.0, runtime + elecPlotDt / 2.0, elecPlotDt ) * 1000 #ms
    for i,col,name in zip( range( 5 ), ['k', 'b:', 'g:', 'r:', 'm:' ], ['soma', 'a', 'b', 'c', 'd'] ):
        ln, = ax1.plot( t, np.zeros(len(t)), col, label='pos= ' + name )
        tplot.append(ln)
    plt.legend()

    ax2 = fig.add_subplot(312)
    #ax2.margins( 0.05 )
    #ax.set_ylim( 0, 0.1 )
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -70, -20 )
    plt.xlabel( 'Position (microns)' )
    #ax2.autoscale( enable = True, axis = 'y' )
    plt.title( "Membrane potential vs position, at 5 times." )
    t = np.arange( 0, numDendSeg+1, 1 ) #sec
    for i,col in zip( range( 5 ), ['m-', 'y-', 'g-', 'b-', 'k-' ] ):
        ln, = ax2.plot( t, np.zeros(numDendSeg+1), col, label="t={} ms".format( 1000*(i*interval2 + interval1) ) )
        lines.append(ln)
    plt.legend()
    ax = fig.add_subplot(313)
    #ax.margins( 0.05 )
    plt.axis('off')
    axcolor = 'palegreen'
    axToggle = plt.axes( [0.02,0.005, 0.20,0.03], facecolor='green' )
    axReset = plt.axes( [0.25,0.005, 0.30,0.03], facecolor='blue' )
    axQuit = plt.axes( [0.60,0.005, 0.30,0.03], facecolor='blue' )
    for y in np.arange( 0.05, 0.21, 0.05 ):
        axOnset.append(plt.axes( [0.25,y, 0.65,0.03], facecolor=axcolor))
    axLen = plt.axes( [0.25,0.25, 0.65,0.03], facecolor=axcolor )
    axDia = plt.axes( [0.25,0.30, 0.65,0.03], facecolor=axcolor )
    #aInit = Slider( axAinit, 'A init conc', 0, 10, valinit=1.0, valstep=0.2)
    toggle = Button( axToggle, 'NMDA Off', color = 'yellow' )
    toggleObj = Toggle( toggle, axToggle )
    
    reset = Button( axReset, 'Reset', color = 'cyan' )
    q = Button( axQuit, 'Quit', color = 'pink' )
    for i, j in zip( range( len( axOnset ) ), ['a', 'b', 'c', 'd'] ): 
        rec.append( RecInfo( "Onset{}".format(i), (6 - i) * 5e-3 ) )
        sliders.append( Slider( axOnset[i], "Onset {} (ms)".format(j), 0.01, 50, valinit = rec[i].onset * 1000.0) )
        sliders[-1].on_changed( rec[-1].setOnset )

    length = Slider( axLen, 'Total length of cell (mm)', 0.1, 4, valinit=1.0 )
    dia = Slider( axDia, 'Diameter of cell (um)', 0.1, 5, valinit=1.0 )

    def resetParms( event ):
        for i in sliders:
            i.reset()
        length.reset()
        dia.reset()


    toggle.on_clicked( toggleObj.click )
    reset.on_clicked( resetParms )
    q.on_clicked( doQuit )
    length.on_changed( updateLen )
    dia.on_changed( updateDia )
    plt.tight_layout()

    updateDisplay()
    plt.show()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()
