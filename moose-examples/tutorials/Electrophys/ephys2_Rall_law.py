########################################################################
# This example demonstrates Rall's Law. 
# In memory of Will Rall, 1922-2018.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# Released under the terms of the GNU Public License V3.
########################################################################
import moose
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from matplotlib.widgets import Slider, Button
import numpy as np
import rdesigneur as rd

numDendSeg = 10 # Applies both to dend and to branches.
interval1 = 0.010
interval2 = 0.05 - interval1
lines = []
# These 5 params vary only for the branches.
RM = 1.0
RA = 1.0
CM = 0.01
length = 0.001
dia = 2e-6
# The params below are fixed, apply to the soma and dend.
dendRM = 2.0
dendRA = 1.5
dendCM = 0.02
dendLength = 0.001
dendDia = 2e-6

# Stimulus in Amps applied to soma.
stimStr = "2e-10"

class lineWrapper():
    def __init__( self ):
        self.YdendLines = 0
        self.Ybranch1Lines = 0
        self.Ybranch2Lines = 0
        self.CylLines = 0

def makeYmodel():
    segLen = dendLength / numDendSeg
    rdes = rd.rdesigneur(
        stealCellFromLibrary = True,
        verbose = False,
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
        # The numerical arguments are all optional
        cellProto = 
            [['ballAndStick', 'cellBase', dendDia, segLen, dendDia, dendLength, numDendSeg]],
        passiveDistrib = [[ '#', 'RM', str(dendRM), 'CM', str(dendCM), 'RA', str(dendRA) ]],
        stimList = [['soma', '1', '.', 'inject', stimStr ]],
    )
    # Build the arms of the Y for a branching cell.
    pa = moose.element( '/library/cellBase' )
    x = length
    y = 0.0
    dx = length / ( numDendSeg * np.sqrt(2.0) )
    dy = dx
    prevc1 = moose.element( '/library/cellBase/dend{}'.format( numDendSeg-1 ) )
    prevc2 = prevc1
    for i in range( numDendSeg ):
        c1 = rd.buildCompt( pa, 'branch1_{}'.format(i), RM = RM, CM = CM, RA = RA, dia = dia, x=x, y=y, dx = dx, dy = dy )
        c2 = rd.buildCompt( pa, 'branch2_{}'.format(i), RM = RM, CM = CM, RA = RA, dia = dia, x=x, y=-y, dx = dx, dy = -dy )
        moose.connect( prevc1, 'axial', c1, 'raxial' )
        moose.connect( prevc2, 'axial', c2, 'raxial' )
        prevc1 = c1
        prevc2 = c2
        x += dx
        y += dy
    rdes.buildModel()

def makeCylModel():
    segLen = dendLength / numDendSeg
    rdes = rd.rdesigneur(
        stealCellFromLibrary = True,
        verbose = False,
        # cellProto syntax: ['ballAndStick', 'name', somaDia, somaLength, dendDia, dendLength, numDendSegments ]
        # The numerical arguments are all optional
        cellProto = 
            [['ballAndStick', 'soma', dendDia, segLen, dendDia, 2*dendLength, 2*numDendSeg]],
        passiveDistrib = [[ '#', 'RM', str(dendRM), 'CM', str(dendCM), 'RA', str(dendRA) ]],
        stimList = [['soma', '1', '.', 'inject', stimStr ]],
    )
    rdes.buildModel()

def main():
    global vec
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
            stimStr = "40e-9*(t<0.001)-36e-9*(t>0.001&&t<0.002)"
        updateDisplay()
        #self.ax.update()
        #self.ax.redraw_in_frame()
        #self.ax.draw()

def printSomaVm():
    print("This is somaVm" )

def updateDisplay():
    makeCylModel()
    moose.element( '/model/elec/' ).name = 'Cyl'
    moose.element( '/model' ).name = 'model1'
    makeYmodel()
    moose.element( '/model/elec/' ).name = 'Y'
    moose.move( '/model1/Cyl', '/model' )
    #moose.le( '/model/Y' )
    #print "################################################"
    #moose.le( '/model/Cyl' )
    vecYdend = moose.wildcardFind( '/model/Y/soma,/model/Y/dend#' )
    vecYbranch1 = moose.wildcardFind( '/model/Y/branch1#' )
    vecYbranch2 = moose.wildcardFind( '/model/Y/branch2#' )
    vecCyl = moose.wildcardFind( '/model/Cyl/#[ISA=CompartmentBase]' )
    #vec[0].inject = 1e-10
    moose.reinit()
    dt = interval1
    for i in lines:
        moose.start( dt )
        #print( len(vecCyl), len(vecYdend), len(vecYbranch1), len(vecYbranch2) )
        i.CylLines.set_ydata( [v.Vm*1000 for v in vecCyl] )
        i.YdendLines.set_ydata( [v.Vm*1000 for v in vecYdend] )
        i.Ybranch1Lines.set_ydata( [v.Vm*1000 for v in vecYbranch1] )
        #i.Ybranch2Lines.set_ydata( [v.Vm*1000 for v in vecYbranch2] )
        dt = interval2

    moose.delete( '/model' )
    moose.delete( '/model1' )
    moose.delete( '/library' )
    # Put in something here for the time-series on soma

def doQuit( event ):
    cylLength = 2*dendLength*float(2*numDendSeg+1)/(2*numDendSeg)
    print( "The cylinder parameters were:\n"
    "Length = {} microns\n"
    "Diameter = {} microns\n"
    "RM = {} Ohms.m^2\n"
    "RA = {} Ohms.m\n"
    "CM = {} Farads/m^2\n"
    "Your branch parameters were:\n"
    "Length = {:.2f} microns\n"
    "Diameter = {:.2f} microns\n"
    "RM = {:.2f} Ohms.m^2\n"
    "RA = {:.2f} Ohms.m\n"
    "CM = {:.3f} Farads/m^2\n".format( 
        cylLength*1e6, dendDia*1e6, dendRM, dendRA, dendCM,
        length*1e6, dia*1e6, RM, RA, CM ) )
    print( "The branch point was at 1100 microns from the left" )

    quit()

def makeDisplay():
    global lines
    #img = mpimg.imread( 'CableEquivCkt.png' )
    img = mpimg.imread( 'RallsLaw.png' )
    #plt.ion()
    fig = plt.figure( figsize=(10,12) )
    png = fig.add_subplot(311)
    imgplot = plt.imshow( img )
    plt.axis('off')
    ax2 = fig.add_subplot(312)
    #ax.set_ylim( 0, 0.1 )
    plt.ylabel( 'Vm (mV)' )
    plt.ylim( -70, 0.0 )
    plt.xlabel( 'Position (segment #)' )
    #ax2.autoscale( enable = True, axis = 'y' )
    plt.title( "Membrane potential as a function of position along cell." )
    #for i,col in zip( range( 5 ), ['k', 'b', 'g', 'y', 'm' ] ):
    time = interval1
    for i,col,cylcol in zip( range( 2 ), ['b', 'g' ], ['r', 'm'] ):
        lw = lineWrapper()
        lw.YdendLines, = ax2.plot( np.arange(0, numDendSeg+1, 1 ),
                np.zeros(numDendSeg+1), col + '.', label = "Primary Dend, t={}".format( time * 1e3 ) )
        lw.Ybranch1Lines, = ax2.plot( np.arange(0, numDendSeg, 1) + numDendSeg + 1, 
            np.zeros(numDendSeg), col + ':', label = "Branch, t={}".format(time*1e3) )
        #lw.Ybranch2Lines, = ax2.plot( np.arange(0, numDendSeg, 1) + numDendSeg + 1, 
            #np.zeros(numDendSeg) + numDendSeg + 1, col + '--' )
        lw.CylLines, = ax2.plot( np.arange(0, numDendSeg*2+1, 1), 
            np.zeros(numDendSeg*2+1), cylcol + '-', label = "Cyl, t={}".format(time*1e3) )
        time += interval2
        lines.append( lw )
    plt.legend()

    ax = fig.add_subplot(313)
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
    CM = Slider( axCM, 'CM ( Farad/m^2)', 0.001, 0.05, valinit=0.01, valfmt = '%0.3f' )
    RA = Slider( axRA, 'RA ( ohm.m', 0.1, 10, valinit=1.0 )
    length = Slider( axLen, 'Length of branches (mm)', 0.1, 10, valinit=2.0 )
    dia = Slider( axDia, 'Diameter of branches (um)', 0.1, 10, valinit=1.0 )
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

    updateDisplay()

    plt.show()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()
