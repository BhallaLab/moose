########################################################################
# This example demonstrates divisive normalization
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from matplotlib.widgets import Slider, Button, RadioButtons
import numpy as np
import warnings
import moose
import rdesigneur as rd

lines = []
tplot = ""
axes = []
sliders = []
spikingDistrib = []

RM = 1.0
RA = 1.0
CM = 0.01
dia = 10e-6
runtime = 0.08
elecPlotDt = 0.0002
sliderMode = "Gbar"
gluGbar = 1.0
gabaGbar = 0.5
K_A_Gbar = 2.0
gluOnset = 20.0e-3
gabaOnset = 40.0e-3
inputFreq = 100.0
inputDuration = 0.01
printOutput = False
makeMovie = False
frameNum = 0
fname = "movie/frame"
ttext = ""

rec = []


def setGluGbar( val ):
    global gluGbar
    gluGbar = val
    updateDisplay()

def setGabaGbar( val ):
    global gabaGbar
    gabaGbar = val
    updateDisplay()

def setK_A_Gbar( val ):
    global K_A_Gbar
    K_A_Gbar = val
    updateDisplay()

def setGabaOnset( val ):
    global gabaOnset
    gabaOnset = val/1000.0
    updateDisplay()

def setRM( val ):
    global RM
    RM = val
    updateDisplay()

def setCM( val ):
    global CM
    CM = val
    updateDisplay()

def makeModel():
    cd = [
            ['glu', 'soma', 'Gbar', str(gluGbar)],
            ['GABA', 'soma', 'Gbar', str(gabaGbar)],
            ['K_A', 'soma', 'Gbar', str(K_A_Gbar)]
    ]
    cd.extend( spikingDistrib )
     
    rdes = rd.rdesigneur(
        elecPlotDt = elecPlotDt,
        stealCellFromLibrary = True,
        verbose = False,
        chanProto = [
            ['make_glu()', 'glu'],['make_GABA()', 'GABA'],
            ['make_K_A()','K_A'],
            ['make_Na()', 'Na'],['make_K_DR()', 'K_DR'],
        ],
        cellProto = [['somaProto', 'cellBase', dia, dia]],
        passiveDistrib = [[ '#', 'RM', str(RM), 'CM', str(CM), 'RA', str(RA) ]],
        chanDistrib = cd,
        stimList = [
            ['soma', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( inputFreq, gluOnset, gluOnset + inputDuration) ],
            ['soma', '1','GABA', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( inputFreq, gabaOnset, gabaOnset + inputDuration) ],
        ],
        plotList = [['soma', '1','.', 'Vm']],
    )
    moose.element( '/library/GABA' ).Ek = -0.07
    rdes.buildModel()

def main():
    warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")

    makeDisplay()
    quit()

class stimToggle():
    def __init__( self, toggle, ax ):
        self.duration = 1
        self.toggle = toggle
        self.ax = ax

    def click( self, event ):
        global spikingDistrib
        if self.duration < 0.5:
            self.duration = 1.0
            self.toggle.label.set_text( "Spiking off" )
            self.toggle.color = "yellow"
            self.toggle.hovercolor = "yellow"
            spikingDistrib = []
        else:
            self.duration = 0.001
            self.toggle.label.set_text( "Spiking on" )
            self.toggle.color = "orange"
            self.toggle.hovercolor = "orange"
            spikingDistrib = [['Na', 'soma', 'Gbar', '200' ],['K_DR', 'soma', 'Gbar', '250' ]]
        updateDisplay()

def printSomaVm():
    print("This is somaVm" )

def updateDisplay():
    global frameNum
    makeModel()
    moose.reinit()
    moose.start( runtime )
    tabvec = moose.element( '/model/graphs/plot0' ).vector
    #print "############## len tabvec = ", len(tabvec)
    maxval = max(tabvec)
    imaxval = list(tabvec).index( maxval )
    maxt = imaxval * elecPlotDt * 1000
    pk = (maxval - min( tabvec[:imaxval+1] )) * 1000
    ttext.set_text( "Peak=({:.1f}, {:.1f})".format( maxt, pk ) )
    tplot.set_ydata( tabvec * 1000 )
    if printOutput:
        print( "{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}".format( maxval*1000, pk,maxt, gluGbar, gabaGbar, K_A_Gbar, gabaOnset*1000, RM, CM ) )
    if makeMovie:
        plt.savefig( "{}_{:03d}.png".format(fname, frameNum) )
        frameNum += 1
    
    moose.delete( '/model' )
    moose.delete( '/library' )

def doQuit( event ):
    quit()

def makeDisplay():
    global lines
    global tplot
    global axes
    global sliders
    global ttext
    img = mpimg.imread( 'EI_input.png' )
    fig = plt.figure( figsize=(10,12) )
    png = fig.add_subplot(311)
    imgplot = plt.imshow( img )
    plt.axis('off')
    ax1 = fig.add_subplot(312)
    ttext = plt.text( 0, -35, "Peak=(0,0)", alpha = 0.9 )
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -80, -30 )
    plt.xlabel( 'time (ms)' )
    plt.title( "Membrane potential vs time at soma." )
    t = np.arange( 0.0, runtime + elecPlotDt / 2.0, elecPlotDt ) * 1000 #ms
    #print "############## len t = ", len(t)
    tplot, = ax1.plot( t, np.zeros(len(t)), 'b-' )

    ax = fig.add_subplot(313)
    plt.axis('off')
    axcolor = 'palegreen'
    axStim = plt.axes( [0.02,0.005, 0.20,0.03], facecolor='green' )
    axReset = plt.axes( [0.25,0.005, 0.30,0.03], facecolor='blue' )
    axQuit = plt.axes( [0.60,0.005, 0.30,0.03], facecolor='blue' )

    for x in np.arange( 0.05, 0.31, 0.05 ):
        axes.append( plt.axes( [0.25, x, 0.65, 0.03], facecolor=axcolor ) )

    stim = Button( axStim, 'Spiking off', color = 'yellow' )
    stimObj = stimToggle( stim, axStim )
    
    reset = Button( axReset, 'Reset', color = 'cyan' )
    q = Button( axQuit, 'Quit', color = 'pink' )

    sliders.append( Slider( axes[0], "gluGbar (Mho/m^2)", 0.01, 2, valinit = gluGbar) )
    sliders[-1].on_changed( setGluGbar )
    sliders.append( Slider( axes[1], "gabaGbar (Mho/m^2)", 0.01, 2, valinit = gabaGbar) )
    sliders[-1].on_changed( setGabaGbar )
    sliders.append( Slider( axes[2], "K_A_Gbar (Mho/m^2)", 0.01, 50, valinit = K_A_Gbar) )
    sliders[-1].on_changed( setK_A_Gbar )
    sliders.append( Slider( axes[3], "GABA Onset time (ms)", 10, 50, valinit = gabaOnset * 1000) )
    sliders[-1].on_changed( setGabaOnset )
    sliders.append( Slider( axes[4], "RM (Ohm.m^2)", 0.1, 10, valinit = RM))
    sliders[-1].on_changed( setRM )
    sliders.append( Slider( axes[5], "CM (Farads/m^2)", 0.001, 0.1, valinit = CM, valfmt='%0.3f'))
    sliders[-1].on_changed( setCM )

    def resetParms( event ):
        for i in sliders:
            i.reset()

    stim.on_clicked( stimObj.click )
    reset.on_clicked( resetParms )
    q.on_clicked( doQuit )

    if printOutput:
        print( "maxval\tpk\tmaxt\tgluG\tgabaG\tK_A_G\tgabaon\tRM\tCM" )
    updateDisplay()

    plt.show()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()

