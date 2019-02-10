########################################################################
# This example demonstrates a cable
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

numDendSeg = 50
interval = 0.005
lines = []
tplot = []
RM = 1.0
RA = 1.0
CM = 0.01
length = 0.002
dia = 1e-6
stimStr = "2e-10"
runtime = 50e-3
elecPlotDt = 0.001

def makeModel():
    segLen = length / numDendSeg
    rdes = rd.rdesigneur(
        stealCellFromLibrary = True,
        elecPlotDt = elecPlotDt,
        verbose = False,
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
        # The numerical arguments are all optional
        cellProto = 
            [['ballAndStick', 'soma', dia, segLen, dia, length, numDendSeg]],
        passiveDistrib = [[ '#', 'RM', str(RM), 'CM', str(CM), 'RA', str(RA) ]],
        stimList = [['soma', '1', '.', 'inject', stimStr ]],
        plotList = [['dend3,dend18,dend33,dend49', '1', '.', 'Vm' ]],
    )
    rdes.buildModel()

def main():
    global vec
    warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")
    makeDisplay()
    quit()

def updateRM(RM_):
    global RM
    RM = RM_
    updateDisplay()

def updateCM(CM_):
    global CM
    CM = CM_
    updateDisplay()

def updateRA(RA_):
    global RA
    RA = RA_
    updateDisplay()

def updateLen(val): # This does the length of the entire cell.
    global length
    length = val * 0.001
    updateDisplay()

def updateDia(val):
    global dia
    dia = val * 1e-6
    updateDisplay()

class stimToggle():
    def __init__( self, toggle, ax ):
        self.duration = 1
        self.toggle = toggle
        self.ax = ax

    def click( self, event ):
        global stimStr
        if self.duration < 0.5:
            self.duration = 1.0
            self.toggle.label.set_text( "Long Stim" )
            self.toggle.color = "yellow"
            self.toggle.hovercolor = "yellow"
            stimStr = "2e-10"
        else:
            self.duration = 0.001
            self.toggle.label.set_text( "Short Stim" )
            self.toggle.color = "orange"
            self.toggle.hovercolor = "orange"
            stimStr = "1e-9*(t<0.001)"
            #stimStr = "10e-9*(t<0.001)-9e-9*(t>0.001&&t<0.002)"
        updateDisplay()

def updateDisplay():
    makeModel()
    vec = moose.wildcardFind( '/model/elec/#[ISA=CompartmentBase]' )
    tabvec = moose.wildcardFind( '/model/graphs/plot#' )
    #vec[0].inject = 1e-10
    moose.reinit()
    initdt = 0.0001
    dt = initdt
    currtime = 0.0
    for i in lines:
        moose.start( dt )
        currtime += dt
        i.set_ydata( [v.Vm * 1000 for v in vec] )
        dt = interval
        #print( "inRunSim v0={}, v10={}".format( vec[0].Vm, vec[10].Vm ) )
    moose.start( runtime - currtime )
    for i,tab in zip( tplot, tabvec ):
        i.set_ydata( tab.vector * 1000 )

    moose.delete( '/model' )
    moose.delete( '/library' )
    # Put in something here for the time-series on soma

def doQuit( event ):
    tab = []
    makeModel()
    soma = moose.element( '/model/elec/soma' )
    moose.reinit()
    with open('output.txt', 'w') as file: 
        file.write( "0.000  {:.2f}\n".format( soma.Vm * 1000 ) )
        for t in np.arange( 0, 0.1, 0.001 ):
            moose.start(0.001)
            file.write( "{:.3f} {:.2f}\n".format( t+0.001, soma.Vm*1000 ) )
    quit()

def makeDisplay():
    global tplot
    global lines
    #img = mpimg.imread( 'CableEquivCkt.png' )
    img = mpimg.imread( 'CableInjectEquivCkt.png' )
    #plt.ion()
    fig = plt.figure( figsize=(10,12) )
    png = fig.add_subplot(321)
    imgplot = plt.imshow( img )
    plt.axis('off')
    ax1 = fig.add_subplot(322)
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -80, 40 )
    plt.xlabel( 'time (ms)' )
    plt.title( "Membrane potential vs time at 4 positions." )
    t = np.arange( 0.0, runtime + elecPlotDt / 2.0, elecPlotDt ) * 1000 #ms
    for i,col,name in zip( range( 4 ), ['b-', 'g-', 'r-', 'm-' ], ['a', 'b', 'c', 'd'] ):
        ln, = ax1.plot( t, np.zeros(len(t)), col, label='pos= ' + name )
        tplot.append(ln)
    plt.legend()

    ax2 = fig.add_subplot(312)
    #ax2.margins( 0.05 )
    #ax.set_ylim( 0, 0.1 )
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -80, 50 )
    plt.xlabel( 'Position (microns)' )
    #ax2.autoscale( enable = True, axis = 'y' )
    plt.title( "Membrane potential vs position, at 5 times." )
    t = np.arange( 0, numDendSeg+1, 1 ) #sec
    for i,col in zip( range( 5 ), ['k-', 'b-', 'g-', 'y-', 'm-' ] ):
        ln, = ax2.plot( t, np.zeros(numDendSeg+1), col, label="t={}".format(i*interval) )
        lines.append(ln)
    plt.legend()
    ax = fig.add_subplot(313)
    #ax.margins( 0.05 )
    plt.axis('off')
    axcolor = 'palegreen'
    axStim = plt.axes( [0.02,0.05, 0.20,0.03], facecolor='green' )
    axReset = plt.axes( [0.25,0.05, 0.30,0.03], facecolor='blue' )
    axQuit = plt.axes( [0.60,0.05, 0.30,0.03], facecolor='blue' )
    axRM = plt.axes( [0.25,0.1, 0.65,0.03], facecolor=axcolor )
    axCM = plt.axes( [0.25,0.15, 0.65,0.03], facecolor=axcolor )
    axRA = plt.axes( [0.25,0.20, 0.65,0.03], facecolor=axcolor )
    axLen = plt.axes( [0.25,0.25, 0.65,0.03], facecolor=axcolor )
    axDia = plt.axes( [0.25,0.30, 0.65,0.03], facecolor=axcolor )
    #aInit = Slider( axAinit, 'A init conc', 0, 10, valinit=1.0, valstep=0.2)
    stim = Button( axStim, 'Long Stim', color = 'yellow' )
    stimObj = stimToggle( stim, axStim )
    
    reset = Button( axReset, 'Reset', color = 'cyan' )
    q = Button( axQuit, 'Quit', color = 'pink' )
    RM = Slider( axRM, 'RM ( ohm.m^2 )', 0.1, 10, valinit=1.0 )
    CM = Slider( axCM, 'CM ( Farad/m^2)', 0.001, 0.1, valinit=0.01, valfmt = '%0.3f' )
    RA = Slider( axRA, 'RA ( ohm.m', 0.1, 10, valinit=1.0 )
    length = Slider( axLen, 'Total length of cell (mm)', 0.1, 10, valinit=2.0 )
    dia = Slider( axDia, 'Diameter of cell (um)', 0.1, 10, valinit=1.0 )
    def resetParms( event ):
        RM.reset()
        CM.reset()
        RA.reset()
        length.reset()
        dia.reset()


    stim.on_clicked( stimObj.click )
    reset.on_clicked( resetParms )
    q.on_clicked( doQuit )
    RM.on_changed( updateRM )
    CM.on_changed( updateCM )
    RA.on_changed( updateRA )
    length.on_changed( updateLen )
    dia.on_changed( updateDia )
    plt.tight_layout()

    updateDisplay()
    plt.show()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()
