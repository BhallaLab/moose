########################################################################
# This example demonstrates the behaviour of various voltage and calcium-
# gated channels.
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
tplot = []
axes = []
sliders = []
fields = []

RM = 1.0
RA = 1.0
CM = 0.01
diameter = 10e-6
runtime = 0.05
elecPlotDt = 0.0001
sliderMode = "Gbar"
currentChanPath = '/model/elec/soma/Na'
currentChanMod = 1.0
preStimTime = 0.05
injectTime = 0.5
postStimTime = 0.05
runtime = preStimTime + injectTime + postStimTime
#inject = 20e-12
helpText = """
Channel Mixer demo help.\n
This demo illustrates how different classes  of ion channels affect neuronal spiking behaviour. The model consists of a single compartment spherical soma, with Na and K_DR set at a default level to get spiking.  The top graph shows the membrane potential, and the second graph shows the calcium concentration at the soma. The current injection begins at 50 ms, and lasts for 500 ms.
In each graph the solid blue plot indicates the current calculated response of the model to the specified channel modulation and current injection.  The red dotted plot indicates what would have happened had the modulated channel not been there at all.

The default channel density for all channels is 100 Mho/m^2, except K_AHP where the default is 10 Mho/m^2. There are sliders for modulating the levels of each of the channels.  The slider lets you modulate this channel density in a range from zero to 10x the default density.
There are two terms to modulate for the Calcium dynamics: the Ca_tau is the time-course for the pump to expel calcium influx, and the Ca_thickness is the thickness of the calcium shell. The maximum thickness is, of course, the radius of the soma, which is 5 microns. If we have a smaller value than this it means that the calcium flux only comes into a cylindrical shell and therefore gives rise to higher concentrations.

The simulation starts with nominal values for the Na and K_DR channels of 100 Mho/m^2, and all other channels modulated down to zero.

Things to do:
    - Examine what each channel does on its own
    - Examine how to control the calcium influx
    - Once calcium influx is present, examine how it affects the K_Ca and K_AHP channels, and how in turn the cell dynamics are affected by calcium dynamics.
    - Can you get it to fire a burst of APs starting around 400ms?
    - Can you get it to fire just one action potential around 100 ms?
    - Can you get it to fire just one action potential around 400 ms?
    - Can you get it to fire with adaptation, that is, rate gets slower?
    - Can you get it to fire with facilitation, that is, rate gets faster?
    - It is a bit fiddly, as the simulation only runs 500 ms, but can you get it to do a couple of bursts?
"""

class SlideField():
    def __init__(self, name, field = 'modulation', initVal = 0.0, suffix = "Modulation", scale = 1.0, valmax = 10.0 ):
        self.name = name
        self.path = '/model/elec/soma/' + name
        self.field = field
        assert( moose.exists( self.path ) )
        self.val = initVal
        self.suffix = suffix
        self.scale = scale
        self.valmax = valmax

    def setVal( self, val ):
        moose.element( self.path ).setField( self.field, val * self.scale )
        self.val = val
        updateDisplay()

    def setChanMod( self, val ):
        global currentChanPath
        global currentChanMod
        currentChanPath = self.path
        currentChanMod = val
        self.val = val
        updateDisplay()

def makeModel():
    rdes = rd.rdesigneur(
        elecPlotDt = elecPlotDt,
        stealCellFromLibrary = True,
        verbose = False,
        #chanProto = [['make_glu()', 'glu'],['make_GABA()', 'GABA']],
        chanProto = [
            ['make_Na()', 'Na'],
            ['make_K_DR()', 'K_DR'],
            ['make_K_A()', 'K_A'],
            ['make_Ca()', 'Ca'],
            ['make_Ca_conc()', 'Ca_conc'],
            ['make_K_AHP()', 'K_AHP'],
            ['make_K_C()', 'K_Ca'],
        ],
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
        # The numerical arguments are all optional
        cellProto = 
            [['somaProto', 'cellBase', diameter, diameter]],
        passiveDistrib = [[ '#', 'RM', str(RM), 'CM', str(CM), 'RA', str(RA) ]],
        chanDistrib = [
            ['Ca_conc', 'soma', 'tau', '0.01333' ],
            ['Na', 'soma', 'Gbar', '100' ],
            ['K_DR', 'soma', 'Gbar', '100' ],
            ['K_A', 'soma', 'Gbar', '100' ],
            ['Ca', 'soma', 'Gbar', '100' ],
            ['K_Ca', 'soma', 'Gbar', '100' ],
            ['K_AHP', 'soma', 'Gbar', '10' ],
        ],
        plotList = [['soma', '1','.', 'Vm'], ['soma', '1', 'Ca_conc', 'Ca']]
    )
    moose.element( '/library/Ca_conc' ).CaBasal=0.08e-3
    rdes.buildModel()
    for i in moose.wildcardFind( '/model/elec/soma/#[ISA=ChanBase]' ):
        i.modulation = 1e-6
    moose.element( '/model/elec/soma/Na' ).modulation = 1
    moose.element( '/model/elec/soma/K_DR' ).modulation = 1
    #moose.showfield( '/model/elec/soma/Ca_conc' )
    #moose.element( '/model/elec/soma/Ca_conc' ).B *= 1000

def main():
    global fields
    makeModel()
    fields.append( SlideField( 'Na', initVal = 1.0 ) )
    fields.append( SlideField( 'K_DR', initVal = 1.0 ) )
    fields.append( SlideField( 'K_A' ) )
    fields.append( SlideField( 'Ca' ) )
    fields.append( SlideField( 'K_AHP' ) )
    fields.append( SlideField( 'K_Ca' ) )
    fields.append( SlideField( 'Ca_conc', field = 'tau', initVal = 13.3, suffix = "(ms)" , scale = 0.001, valmax = 500.0 ) )
    fields[-1].name = 'Ca_tau'
    fields.append( SlideField( 'Ca_conc', field = 'thick', initVal = 1e6*diameter/2.0, suffix = "(microns)", scale = 1e-6, valmax = 1e6*diameter / 2.0) )
    fields[-1].name = 'Ca_thickness'
    fields.append( SlideField( '.', field = 'inject', initVal = 10, suffix = "(pA)", scale = 1e-12, valmax = 20 ) )
    fields[-1].name = 'Inject'
    warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")

    makeDisplay()
    quit()

def printSomaVm():
    print("This is somaVm" )

def runMod( mod ):
    moose.element( currentChanPath ).modulation = mod
    #print( 'path = {}, mod = {}'.format( currentChanPath, mod ) )
    moose.reinit()
    moose.start( preStimTime )
    moose.element( '/model/elec/soma' ).inject = fields[-1].val * fields[-1].scale
    moose.start( injectTime )
    moose.element( '/model/elec/soma' ).inject = 0
    moose.start( postStimTime )
    Vm = moose.element( '/model/graphs/plot0' ).vector
    Ca = moose.element( '/model/graphs/plot1' ).vector
    return Vm * 1000, Ca * 1e3

def updateDisplay():
    Vm0, Ca0 = runMod( 1.0e-9 )
    Vm1, Ca1 = runMod( currentChanMod )
    #print len(Vm0), len(Ca0), len(Vm1), len( Ca1)
    tplot[0].set_ydata( Vm0 )
    tplot[1].set_ydata( Vm1 )
    tplot[2].set_ydata( Ca0 )
    tplot[3].set_ydata( Ca1 )

def doQuit( event ):
    quit()

def makeDisplay():
    global tplot
    global axes
    global sliders

    fig = plt.figure( figsize=(10,12) )
    t = np.arange( 0.0, runtime + elecPlotDt / 2.0, elecPlotDt ) * 1000 #ms
    ax0 = fig.add_subplot(311)
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -80, +40 )
    plt.xlabel( 'time (ms)' )
    plt.title( "Membrane potential vs. time." )
    ln, = ax0.plot( t, np.zeros(len(t)), 'r:', label='chan absent' )
    tplot.append(ln)
    ln, = ax0.plot( t, np.zeros(len(t)), 'b-', label='chan present' )
    tplot.append(ln)
    plt.legend()
    ####################################
    ax1 = fig.add_subplot(312)
    plt.ylabel( 'Ca (uM)' )
    plt.ylim( 0, 10 )
    plt.xlabel( 'time (ms)' )
    ln, = ax1.plot( t, np.zeros(len(t)), 'r:', label='chan absent' )
    tplot.append(ln)
    ln, = ax1.plot( t, np.zeros(len(t)), 'b-', label='chan present' )
    tplot.append(ln)
    plt.title( "Calcium concentration vs. time." )
    plt.legend()
    helpTextBox = plt.text( -0.14,-0.1, helpText, fontsize = 14, wrap = True, transform=ax1.transAxes, bbox=dict(facecolor='white', alpha=0.9), zorder = 10 )
    ####################################
    ax2 = fig.add_subplot(313)
    plt.axis('off')
    axcolor = 'palegreen'
    axStim = plt.axes( [0.02,0.005, 0.20,0.03], facecolor='green' )
    axReset = plt.axes( [0.25,0.005, 0.30,0.03], facecolor='blue' )
    axQuit = plt.axes( [0.60,0.005, 0.30,0.03], facecolor='blue' )

    for x in np.arange( 0.05, 0.34, 0.034 ):
        axes.append( plt.axes( [0.25, x, 0.65, 0.015], facecolor=axcolor ) )
    #aInit = Slider( axAinit, 'A init conc', 0, 10, valinit=1.0, valstep=0.2)
    #rax = plt.axes([0.02, 0.05, 0.10, 0.28], facecolor="#EEEFFF")
    #mode = RadioButtons(rax, ('Channels', 'Other Parms'))

    stim = Button( axStim, 'View help', color = 'yellow' )
    
    reset = Button( axReset, 'Reset', color = 'cyan' )
    q = Button( axQuit, 'Quit', color = 'pink' )

    for i in range( len( axes ) ):
        if i < 2: # Na and K
            valinit = 1.0
        else:
            valinit = 0.0
        sliders.append( Slider( axes[i], fields[i].name+ " " + fields[i].suffix, 0.0, fields[i].valmax, valinit = fields[i].val) )
        if fields[i].field == 'modulation':
            sliders[-1].on_changed( fields[i].setChanMod )
        else:
            sliders[-1].on_changed( fields[i].setVal )

    def resetParms( event ):
        for i in sliders:
            i.reset()

    def toggleHelp( event ):
        if helpTextBox.get_visible():
            helpTextBox.set_visible( False )
            stim.label.set_text( "View Help" )
        else:
            helpTextBox.set_visible( True )
            stim.label.set_text( "Hide Help" )
        #plt.draw()
    #mh = modeHandler()

    #mode.on_clicked( mh.setMode )
    stim.on_clicked( toggleHelp )
    reset.on_clicked( resetParms )
    q.on_clicked( doQuit )

    updateDisplay()

    plt.show()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()

