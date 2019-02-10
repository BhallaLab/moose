########################################################################
# This example demonstrates synaptic summation in a branched neuron.
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

numDendSeg = 10 # Applies both to dend and to branches.
interval1 = 0.015
interval2 = 0.010
lines = []
tplot = []
axes = []
sliders = []
spikingProto = []
spikingDistrib = []

RM = 1.0
RA = 1.0
CM = 0.01
#length = 0.0005
#dia = 2e-6
runtime = 0.05
elecPlotDt = 0.0005
sliderMode = "Gbar"

rec = []

class lineWrapper():
    def __init__( self ):
        self.YdendLines = 0
        self.Ybranch1Lines = 0
        self.Ybranch2Lines = 0


class RecInfo():
    def __init__(self, name, tau1 = 2e-3, inputFreq = 100.0, Ek = 0.0, geom = 500e-6 ):
        self.name = name
        self.Gbar = 10.0
        self.tau1 = tau1
        self.tau2 = 9e-3
        self.Ek = Ek   # Reversal potential
        self.onset = 0.01
        self.inputFreq = inputFreq
        self.inputDuration = 0.01
        self.geom = geom

        self.setFunc = {'Gbar': self.setGbar, 'Onset': self.setOnset, 
            'Tau': self.setTau, 'Freq': self.setFreq,
            'Duration': self.setDuration, 'Elec': self.setElec
            }
        self.getFunc = {'Gbar': (lambda x: x.Gbar), 
                'Onset': lambda x: 1000*x.onset,
                'Tau': self.getTau,
                'Freq': lambda x: x.inputFreq,
                'Duration': lambda x: 1000*x.inputDuration,
                'Elec': lambda x: x.geom*1e6 }

    def setGbar( self, val ):
        self.Gbar = val
        #print "########### GBAR = ", val
        updateDisplay()

    def setOnset( self, val ):
        self.onset = val/1000.0
        updateDisplay()

    def setFreq( self, val ):
        self.inputFreq = val
        updateDisplay()

    def setDuration( self, val ):
        self.inputDuration = val/1000.0
        updateDisplay()

    def setElec( self, val ): #messy because we set geom to handle both
        # length and dia. The correct value are picked up based on rec index
        # in the field assingment during construction
        self.geom = val/1e6
        updateDisplay()

    def setTau( self, val ): #messy because we set taus and Ek both.
        #print self.name, val
        if self.name == 'glu_Jn':
            self.tau1 = val / 1000.0
        elif self.name == 'glu_Br1':
            self.tau2 = val / 1000.0
        elif self.name == 'glu_Br2':
            self.Ek = val / 1000.0
        elif self.name == 'gaba_Jn':
            self.tau1 = val / 1000.0
        elif self.name == 'gaba_Br1':
            self.tau2 = val / 1000.0
        elif self.name == 'gaba_Br2':
            self.Ek = val / 1000.0
        updateDisplay()

    def getTau( self, dummy ): #messy because we set taus and Ek both.
        #print self.name
        if self.name == 'glu_Jn':
            return self.tau1 * 1000
        elif self.name == 'glu_Br1':
            return self.tau2 * 1000
        elif self.name == 'glu_Br2':
            return self.Ek * 1000
        elif self.name == 'gaba_Jn':
            return self.tau1 * 1000
        elif self.name == 'gaba_Br1':
            return self.tau2 * 1000
        elif self.name == 'gaba_Br2':
            return self.Ek * 1000


    def set( self, val ):
        if sliderMode in self.setFunc:
            self.setFunc[ sliderMode ]( val )

    def get( self ):
        if sliderMode in self.getFunc:
            return self.getFunc[ sliderMode ](self)
        return 0.0

def makeYmodel():
    length = rec[0].geom
    dia = rec[3].geom
    segLen = length / numDendSeg
    cp = [['make_glu()', 'glu'],['make_GABA()', 'GABA']]
    cp.extend( spikingProto )
    cd = [
        ['glu', 'dend9', 'Gbar', str(rec[0].Gbar)],
        ['glu', 'branch1_9', 'Gbar', str(rec[1].Gbar)],
        ['glu', 'branch2_9', 'Gbar', str(rec[2].Gbar)],
        ['GABA', 'dend9', 'Gbar', str(rec[3].Gbar)],
        ['GABA', 'branch1_9', 'Gbar', str(rec[4].Gbar)],
        ['GABA', 'branch2_9', 'Gbar', str(rec[5].Gbar)]
    ]
    cd.extend( spikingDistrib )
     
    rdes = rd.rdesigneur(
        elecPlotDt = elecPlotDt,
        stealCellFromLibrary = True,
        verbose = False,
        #chanProto = [['make_glu()', 'glu'],['make_GABA()', 'GABA']],
        chanProto = cp,
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
        # The numerical arguments are all optional
        cellProto = 
            [['ballAndStick', 'cellBase', dia, segLen, dia, length, numDendSeg]],
        passiveDistrib = [[ '#', 'RM', str(RM), 'CM', str(CM), 'RA', str(RA) ]],
        chanDistrib = cd,
        #chanDistrib = [
            #['glu', 'dend9', 'Gbar', str(rec[0].Gbar)],
            #['glu', 'branch1_9', 'Gbar', str(rec[1].Gbar)],
            #['glu', 'branch2_9', 'Gbar', str(rec[2].Gbar)],
            #['GABA', 'dend9', 'Gbar', str(rec[3].Gbar)],
            #['GABA', 'branch1_9', 'Gbar', str(rec[4].Gbar)],
            #['GABA', 'branch2_9', 'Gbar', str(rec[5].Gbar)],
        #],
        stimList = [
            ['dend9', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( rec[0].inputFreq, rec[0].onset, rec[0].onset + rec[0].inputDuration) ],
            ['branch1_9', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( rec[1].inputFreq, rec[1].onset, rec[1].onset + rec[1].inputDuration) ],
            ['branch2_9', '1','glu', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( rec[2].inputFreq, rec[2].onset, rec[2].onset + rec[2].inputDuration) ],
            ['dend9', '1','GABA', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( rec[3].inputFreq, rec[3].onset, rec[3].onset + rec[3].inputDuration) ],
            ['branch1_9', '1','GABA', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( rec[4].inputFreq, rec[4].onset, rec[4].onset + rec[4].inputDuration) ],
            ['branch2_9', '1','GABA', 'periodicsyn', '{}*(t>{:.3f} && t<{:.3f})'.format( rec[5].inputFreq, rec[5].onset, rec[5].onset + rec[5].inputDuration) ],
            #['dend9,branch1_9,branch2_9', '1','glu', 'periodicsyn', '100*(t>0.01 && t<0.02)'],
        ],
        plotList = [
            ['soma,dend9,branch1_9,branch2_9', '1','.', 'Vm'],
        ],
    )
    # Modify some library values based on the slider controls
    glu = moose.element( '/library/glu' )
    gaba = moose.element( '/library/GABA' )
    glu.tau1 = rec[0].tau1
    glu.tau2 = rec[1].tau2
    glu.Ek = rec[2].Ek
    gaba.tau1 = rec[3].tau1
    gaba.tau2 = rec[4].tau2
    gaba.Ek = rec[5].Ek
    # Build the arms of the Y for a branching cell.
    pa = moose.element( '/library/cellBase' )
    x1 = length
    x2 = length
    y1 = 0.0
    y2 = 0.0
    dx1 = rec[1].geom / ( numDendSeg * np.sqrt(2.0) )
    dx2 = rec[2].geom / ( numDendSeg * np.sqrt(2.0) )
    dia1 = rec[4].geom
    dia2 = rec[5].geom
    dy1 = dx1
    dy2 = -dx2 
    prevc1 = moose.element( '/library/cellBase/dend{}'.format( numDendSeg-1 ) )
    prevc2 = prevc1
    for i in range( numDendSeg ):
        c1 = rd.buildCompt( pa, 'branch1_{}'.format(i), RM = RM, CM = CM, RA = RA, dia = dia1, x=x1, y=y1, dx = dx1, dy = dy1 )
        c2 = rd.buildCompt( pa, 'branch2_{}'.format(i), RM = RM, CM = CM, RA = RA, dia = dia2, x=x2, y=y2, dx = dx2, dy = dy2 )
        moose.connect( prevc1, 'axial', c1, 'raxial' )
        moose.connect( prevc2, 'axial', c2, 'raxial' )
        prevc1 = c1
        prevc2 = c2
        x1 += dx1
        y1 += dy1
        x2 += dx2
        y2 += dy2
    rdes.elecid.buildSegmentTree() # rebuild it as we've added the branches

    rdes.buildModel()
    # Permit fast spiking input.
    #for i in moose.wildcardFind( '/model/##[ISA=RandSpike]' ):
    for i in moose.wildcardFind( '/model/elec/#/#/#/#/synInput_rs' ):
        #print i.path, i.refractT
        i.refractT = 0.002
    '''
    moose.le( '/model/elec/dend9/glu/sh/synapse/synInput_rs' )
    moose.showmsg( '/model/elec/dend9/glu/sh/synapse/synInput_rs' )
    moose.showfield( '/model/stims/stim0' )
    '''

def main():
    global rec
    rec.append( RecInfo( 'glu_Jn' ) )
    rec.append( RecInfo( 'glu_Br1' ) )
    rec.append( RecInfo( 'glu_Br2' ) )
    rec.append( RecInfo( 'gaba_Jn', tau1 = 4e-3, inputFreq =100.0, Ek=-0.07, geom = 2e-6 ) )
    rec.append( RecInfo( 'gaba_Br1', tau1 = 4e-3, inputFreq=100.0, Ek=-0.07, geom = 2e-6 ) )
    rec.append( RecInfo( 'gaba_Br2', tau1 = 4e-3, inputFreq=100.0, Ek=-0.07, geom = 2e-6 ) )

    warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")

    makeDisplay()
    quit()

class stimToggle():
    def __init__( self, toggle, ax ):
        self.duration = 1
        self.toggle = toggle
        self.ax = ax

    def click( self, event ):
        global spikingProto
        global spikingDistrib
        if self.duration < 0.5:
            self.duration = 1.0
            self.toggle.label.set_text( "Spiking off" )
            self.toggle.color = "yellow"
            self.toggle.hovercolor = "yellow"
            spikingProto = []
            spikingDistrib = []
        else:
            self.duration = 0.001
            self.toggle.label.set_text( "Spiking on" )
            self.toggle.color = "orange"
            self.toggle.hovercolor = "orange"
            spikingProto = [['make_Na()', 'Na'], ['make_K_DR()', 'K_DR']]
            spikingDistrib = [['Na', 'soma', 'Gbar', '300' ],['K_DR', 'soma', 'Gbar', '250' ]]
        updateDisplay()

def printSomaVm():
    print("This is somaVm" )

def updateDisplay():
    makeYmodel()
    tabvec = moose.wildcardFind( '/model/graphs/plot#' )
    moose.element( '/model/elec/' ).name = 'Y'
    vecYdend = moose.wildcardFind( '/model/Y/soma,/model/Y/dend#' )
    vecYbranch1 = moose.wildcardFind( '/model/Y/branch1#' )
    vecYbranch2 = moose.wildcardFind( '/model/Y/branch2#' )
    moose.reinit()
    dt = interval1
    currtime = 0.0
    for i in lines:
        moose.start( dt )
        currtime += dt
        #print "############## NumDendData = ", len( vecYdend )
        i.YdendLines.set_ydata( [v.Vm*1000 for v in vecYdend] )
        i.Ybranch1Lines.set_ydata( [v.Vm*1000 for v in vecYbranch1] )
        i.Ybranch2Lines.set_ydata( [v.Vm*1000 for v in vecYbranch2] )
        dt = interval2

    moose.start( runtime - currtime )

    #print "############## len (tabvec)  = ", len( tabvec[0].vector )
    for i, tab in zip( tplot, tabvec ):
        i.set_ydata( tab.vector * 1000 )
    
    moose.delete( '/model' )
    moose.delete( '/library' )

def doQuit( event ):
    quit()

def makeDisplay():
    global lines
    global tplot
    global axes
    global sliders
    img = mpimg.imread( 'synapticSummation.png' )
    fig = plt.figure( figsize=(10,12) )
    png = fig.add_subplot(321)
    imgplot = plt.imshow( img )
    plt.axis('off')
    ax1 = fig.add_subplot(322)
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -80, -20 )
    plt.xlabel( 'time (ms)' )
    plt.title( "Membrane potential vs time at 4 positions." )
    t = np.arange( 0.0, runtime + elecPlotDt / 2.0, elecPlotDt ) * 1000 #ms
    #print "############## len t = ", len(t), " numDendSeg = " , numDendSeg
    for i,col,name in zip( range( 4 ), ['b-', 'g-', 'r-', 'm-' ], ['soma', 'jn', 'br1', 'br2'] ):
        ln, = ax1.plot( t, np.zeros(len(t)), col, label='pos= ' + name )
        tplot.append(ln)
    plt.legend()
    ax2 = fig.add_subplot(312)
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -70, 0.0 )
    plt.xlabel( 'Position (microns)' )
    #ax2.autoscale( enable = True, axis = 'y' )
    plt.title( "Membrane potential as a function of position along cell." )
    #for i,col in zip( range( 5 ), ['k', 'b', 'g', 'y', 'm' ] ):
    lt = interval1
    for i,col in zip( range( 3 ), ['g', 'b', 'k' ] ):
        lw = lineWrapper()
        lw.YdendLines, = ax2.plot( np.arange(0, numDendSeg+1, 1 ),
                np.zeros(numDendSeg+1), col + '-', label = str(lt*1000) + " ms" )
        lw.Ybranch1Lines, = ax2.plot( np.arange(0, numDendSeg, 1) + numDendSeg + 1, 
                np.zeros(numDendSeg), col + ':' )
        lw.Ybranch2Lines, = ax2.plot( np.arange(0, numDendSeg, 1) + numDendSeg + 1, 
                np.zeros(numDendSeg) + numDendSeg + 1, col + '.' )
        lines.append( lw )
        lt += interval2
    plt.legend()

    ax = fig.add_subplot(313)
    plt.axis('off')
    axcolor = 'palegreen'
    axStim = plt.axes( [0.02,0.005, 0.20,0.03], facecolor='green' )
    axReset = plt.axes( [0.25,0.005, 0.30,0.03], facecolor='blue' )
    axQuit = plt.axes( [0.60,0.005, 0.30,0.03], facecolor='blue' )

    for x in np.arange( 0.05, 0.31, 0.05 ):
        axes.append( plt.axes( [0.25, x, 0.65, 0.03], facecolor=axcolor ) )
    #aInit = Slider( axAinit, 'A init conc', 0, 10, valinit=1.0, valstep=0.2)
    rax = plt.axes([0.02, 0.05, 0.10, 0.28], facecolor="#EEEFFF")
    mode = RadioButtons(rax, ('Gbar', 'Onset', 'Tau', 'Duration', 'Freq', 'Elec'))


    stim = Button( axStim, 'Spiking off', color = 'yellow' )
    stimObj = stimToggle( stim, axStim )
    
    reset = Button( axReset, 'Reset', color = 'cyan' )
    q = Button( axQuit, 'Quit', color = 'pink' )

    for i in range( len( axes ) ):
        sliders.append( Slider( axes[i], rec[i].name+" Gbar", 0.01, 100, valinit = 10) )
        sliders[-1].on_changed( rec[i].set )

    '''
    sliders[3].facecolor = "red"
    sliders[3].label.set_text( "abbaHippo") 
    print( "{}".format(  sliders[3].poly ) )
    sliders[3].poly.set_color( "red") 
    sliders[3].valmax = 1234
    sliders[3].ax.set_xlim( 0, 1234.0)
    '''

    def resetParms( event ):
        for i in sliders:
            i.reset()

    mh = modeHandler()

    mode.on_clicked( mh.setMode )
    stim.on_clicked( stimObj.click )
    reset.on_clicked( resetParms )
    q.on_clicked( doQuit )

    updateDisplay()

    plt.show()

class modeInfo():
    def __init__( self, name = "glu_jn", fg = "blue", 
        vmin = 0.001, vmax = 100.0, default = 20.0 ):
        self.name = name
        self.fg = fg
        self.vmin = vmin
        self.vmax = vmax
        self.default = default
        self.current = default

class modeHandler():
    def __init__(self):
        gbarMode = [ modeInfo( name = i.name ) for i in rec ]
        onsetMode = [ modeInfo( name = i.name, fg = "cyan", 
            vmin = 0.0, vmax = 40.0, default = 10.0 ) for i in rec ]
        tauMode = [ modeInfo( name = 'Tau1 glu', fg = "yellow", 
            vmin = 0.1, vmax = 20.0, default = 2.0 ),
            modeInfo( name = 'Tau2 glu', fg = "yellow", 
            vmin = 0.1, vmax = 20.0, default = 9.0 ),
            modeInfo( name = 'Erev glu', fg = "yellow", 
            vmin = -40.0, vmax = 40.0, default = 0.0 ),
            modeInfo( name = 'Tau1 GABA', fg = "yellow", 
            vmin = 0.1, vmax = 20.0, default = 4.0 ),
            modeInfo( name = 'Tau2 GABA', fg = "yellow", 
            vmin = 0.1, vmax = 20.0, default = 9.0 ),
            modeInfo( name = 'Erev GABA', fg = "yellow", 
            vmin = -80.0, vmax = -40.0, default = -65.0 )
        ]
        freqMode = [ modeInfo( name = i.name, fg = "pink", vmin = 0.0, 
            vmax = 200.0, default = 100.0 ) for i in rec ]
        durationMode = [ modeInfo( name = i.name, fg = "green", vmin = 0.0, 
            vmax = 40.0, default = 10.0 ) for i in rec ]
        elecMode = [ 
            modeInfo( name = 'Dend Length', fg = "maroon", 
            vmin = 50, vmax = 2000, default = 500 ),
            modeInfo( name = 'Br1 Length', fg = "maroon", 
            vmin = 50, vmax = 2000, default = 500 ),
            modeInfo( name = 'Br2 Length', fg = "maroon", 
            vmin = 50, vmax = 2000, default = 500 ),
            modeInfo( name = 'Dend Dia', fg = "maroon", 
            vmin = 0.1, vmax = 10.0, default = 2.0 ),
            modeInfo( name = 'Br1 Dia', fg = "maroon", 
            vmin = 0.1, vmax = 10.0, default = 1.26 ),
            modeInfo( name = 'Br2 Dia', fg = "maroon", 
            vmin = 0.1, vmax = 10.0, default = 1.26 )
        ]

        self.labels = { "Gbar": gbarMode, "Onset": onsetMode, 
            "Freq": freqMode, "Duration": durationMode,
            "Tau": tauMode, "Elec": elecMode }


    def setMode( self, label ):
        global sliderMode
        if label in self.labels:
            modes = self.labels[label]
            sliderMode = label
            for s,m,r in zip( sliders, modes, rec ):
                if label == "Tau" or label == "Elec":
                    s.label.set_text( m.name )
                else:
                    s.label.set_text( label + " " + m.name )
                s.valmin = m.vmin
                s.valmax = m.vmax
                s.poly.set_color( m.fg )
                s.ax.set_xlim( m.vmin, m.vmax )
                #print m.current, r.get()
                s.set_val( r.get() )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()

