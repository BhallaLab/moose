# Generates time-series response to a Ca pulse for each of the models. No
# diffusion involved.

import sys
import numpy as np
import pylab
import matplotlib.pyplot as plt
import moose
import abstrModelEqns2
import rdesigneur as rd
import time


def singleCompt( name, params ):
    mod = moose.copy( '/library/' + name + '/' + name, '/model' )
    A = moose.element( mod.path + '/A' )
    Z = moose.element( mod.path + '/Z' )
    Z.nInit = 1
    Ca = moose.element( mod.path + '/Ca' )
    CaStim = moose.element( Ca.path + '/CaStim' )
    runtime = params['preStimTime'] + params['stimWidth'] + params['postStimTime'] 
    steptime = 100

    CaStim.expr += ' + x2 * (t > ' + str( runtime ) + ' ) * ( t < ' + str( runtime + steptime ) +  ' )'
    print(CaStim.expr)
    tab = moose.Table2( '/model/' + name + '/Atab' )
    #for i in range( 10, 19 ):
        #moose.setClock( i, 0.01 )
    ampl = moose.element( mod.path + '/ampl' )
    phase = moose.element( mod.path + '/phase' )
    moose.connect( tab, 'requestOut', A, 'getN' )
    ampl.nInit = params['stimAmplitude'] * 1
    phase.nInit = params['preStimTime']

    ksolve = moose.Ksolve( mod.path + '/ksolve' )
    stoich = moose.Stoich( mod.path + '/stoich' )
    stoich.compartment = mod
    stoich.ksolve = ksolve
    stoich.path = mod.path + '/##'
    runtime += 2 * steptime

    moose.reinit()
    moose.start( runtime )
    t = np.arange( 0, runtime + 1e-9, tab.dt )
    return name, t, tab.vector
    #pylab.plot( t, tab.vector, label='[A] (mM)' )
    
    #pylab.show()

def plotBoilerplate( panelTitle, plotPos, xlabel = ''):
    ax = plt.subplot( 8,4,plotPos )
    #ax.xaxis.set_ticks( i[1] )
    #ax.locator_params( 
    ax.spines['top'].set_visible( False )
    ax.spines['right'].set_visible( False )
    ax.tick_params( direction = 'out' )
    if (((plotPos -1)/4) % 2) == 0:
        ax.set_xticklabels([])
    else:
        ax.set_xlabel( xlabel )
    for tick in ax.xaxis.get_major_ticks():
        tick.tick2On = False
    for tick in ax.yaxis.get_major_ticks():
        tick.tick2On = False
    
    if (plotPos % 4) == 1:
        plt.ylabel( 'conc', fontsize = 16 )
        # alternate way of doing this separately.
        #plt.yaxis.label.size_size(16)
        #plt.title( 'B' )
        ax.text( -0.3, 1, panelTitle, fontsize = 18, weight = 'bold',
        transform=ax.transAxes )
    return ax

def plotPanelB():
    panelB = []
    panelBticks = []
    panelB.append( singleCompt( 'fhn', abstrModelEqns2.makeFHN() ) )
    panelB.append( singleCompt( 'bis', abstrModelEqns2.makeBis() ) )
    panelB.append( singleCompt( 'negFB', abstrModelEqns2.makeNegFB() ) )
    panelB.append( singleCompt( 'negFF', abstrModelEqns2.makeNegFF() ) )

    panelBticks.append( np.arange( 0, 4.00001, 1 ) )
    panelBticks.append( np.arange( 0, 4.00001, 1 ) )
    panelBticks.append( np.arange( 0, 15.00001, 5 ) )
    panelBticks.append( np.arange( 0, 6.00001, 2 ) )
    moose.delete( '/model' )

    for i in zip( panelB, panelBticks, list(range( len( panelB ))) ):
        plotPos = i[2] + 5
        ax = plotBoilerplate( 'B', plotPos )
        plt.plot( i[0][1], i[0][2] )
        xmax = ax.get_xlim()[1]
        #ax.xaxis.set_ticks( np.arange( 0, xmax, 50 ) )
        ax.xaxis.set_ticks( np.arange( 0, 200.001, 50 ) )
        ax.yaxis.set_ticks( i[1] )

def runPanelCDEF( name, dist, seqDt, numSpine, seq, stimAmpl ):
    preStim = 10.0
    blanks = 20
    rdes = rd.rdesigneur(
        useGssa = False,
        turnOffElec = True,
        chemPlotDt = 0.1,
        #diffusionLength = params['diffusionLength'],
        diffusionLength = 1e-6,
        cellProto = [['cell', 'soma']],
        chemProto = [['dend', name]],
        chemDistrib = [['dend', 'soma', 'install', '1' ]],
        plotList = [['soma', '1', 'dend' + '/A', 'n', '# of A']],
    )
    rdes.buildModel()
    #for i in range( 20 ):
        #moose.setClock( i, 0.02 )
    A = moose.vec( '/model/chem/dend/A' )
    Z = moose.vec( '/model/chem/dend/Z' )
    print(moose.element( '/model/chem/dend/A/Adot' ).expr)
    print(moose.element( '/model/chem/dend/B/Bdot' ).expr)
    print(moose.element( '/model/chem/dend/Ca/CaStim' ).expr)
    phase = moose.vec( '/model/chem/dend/phase' )
    ampl = moose.vec( '/model/chem/dend/ampl' )
    vel = moose.vec( '/model/chem/dend/vel' )
    vel.nInit = 1e-6 * seqDt
    ampl.nInit = stimAmpl
    stride = int( dist ) / numSpine
    phase.nInit = 10000
    Z.nInit = 0
    for j in range( numSpine ):
        k = int(blanks + j * stride)
        Z[k].nInit = 1
        phase[k].nInit = preStim + seq[j] * seqDt
    moose.reinit()
    runtime = 50
    snapshot = preStim + seqDt * (numSpine - 0.8)
    print(snapshot)
    #snapshot = 26
    moose.start( snapshot )
    avec = moose.vec( '/model/chem/dend/A' ).n
    moose.start( runtime - snapshot )
    tvec = []
    for i in range( 5 ):
        tab = moose.element( '/model/graphs/plot0[' + str( blanks + i * stride ) + ']' )
        dt = tab.dt
        tvec.append( tab.vector )
    moose.delete( '/model' )
    return dt, tvec, avec

def makePassiveSoma( name, length, diameter ):
    elecid = moose.Neuron( '/library/' + name )
    dend = moose.Compartment( elecid.path + '/soma' )
    dend.diameter = diameter
    dend.length = length
    dend.x = length
    return elecid

## NOTE: In this function, `start` is missing. Disabling it else pylint gonna fail.
#def plotOnePanel( tLabel, dt, tplot, numSyn, plotRange, tick ):
#    t = np.arange( 0, len( tplot[0] ), 1.0 ) * dt
#    ax = plotBoilerplate( tLabel, 1 + start )
#    for i in range( 5 ):
#        plt.plot( t, tplot[i] )
#    ax.yaxis.set_ticks( np.arange( 0, plotRange, tick ) )


def plotPanelCDEF( seq, row ):
    makePassiveSoma( 'cell', 100e-6, 10e-6 )
    start = (row -1) * 4
    tLabel = chr( ord( 'A' ) + row - 1 )
    xLabel = chr( ord( 'C' ) + row - 1 )
    xplot = []
    #dt, tplot, avec = runPanelCDEF( 'fhn', 15.0, 3.0, 5, seq, 0.4 )
    dt, tplot, avec = runPanelCDEF( 'fhn', 5.0, 0.5, 5, seq, 0.4 )
    xplot.append( avec )
    #plotOnePanel( dt, 'B', tplot, 5, 1.5, 0.5 )
    t = np.arange( 0, len( tplot[0] ), 1.0 ) * dt
    #ax = plotBoilerplate( tLabel, 1 + start )
    ax = plotBoilerplate( tLabel, 1 + start, 'Time (s)')
    for i in range( 5 ):
        plt.plot( t, tplot[i] )
    yl = ax.get_ylim()[1] 
    ax.yaxis.set_ticks( np.arange( 0, 4.0001, 1.0 ) )

    #dt, tplot, avec = runPanelCDEF( 'bis', 5.0, 4.0, 5, seq, 1.0 )
    dt, tplot, avec = runPanelCDEF( 'bis', 15.0, 2.0, 5, seq, 1.0 )
    xplot.append( avec )
    t = np.arange( 0, len( tplot[0] ), 1.0 ) * dt
    ax = plotBoilerplate( tLabel, 2 + start, 'Time (s)' )
    for i in range( 5 ):
        plt.plot( t, tplot[i] )
    yl = ax.get_ylim()[1] 
    ax.yaxis.set_ticks( np.arange( 0, 3.0001, 1.0 ) )

    dt, tplot, avec = runPanelCDEF( 'negFB', 5.0, 2.0, 5, seq, 1.0 )
    xplot.append( avec )
    t = np.arange( 0, len( tplot[0] ), 1.0 ) * dt
    ax = plotBoilerplate( tLabel, 3 + start, 'Time (s)')
    for i in range( 5 ):
        plt.plot( t, tplot[i] )
    ax.yaxis.set_ticks( np.arange( 0, 10.00001, 5.0 ) )

    dt, tplot, avec = runPanelCDEF( 'negFF', 5.0, 4.0, 5, seq, 1.0 )
    xplot.append( avec )
    t = np.arange( 0, len( tplot[0] ), 1.0 ) * dt
    ax = plotBoilerplate( tLabel, 4 + start, 'Time (s)')
    for i in range( 5 ):
        plt.plot( t, tplot[i] )
    ax.yaxis.set_ticks( np.arange( 0, 5.00001, 2.0 ) )

    for i in zip( list(range(4)), (4.0, 3.0, 10, 4 ), (1, 1, 5, 2) ):
        ax = plotBoilerplate( xLabel, 9 + start + i[0], 'Position( um)' )
        plt.plot( xplot[i[0]][:50] )
        ax.yaxis.set_ticks( np.arange( 0, i[1] * 1.0000001, i[2] ) )

##################################################################
if __name__ == '__main__':
    moose.Neutral( '/library' )
    moose.Neutral( '/model' )

    t1 = time.time()
    fig = plt.figure( figsize = (10,10), facecolor='white' )
    fig.subplots_adjust( left = 0.18 )
    plotPanelB()
    plotPanelCDEF( [0,1,2,3,4], 3 )
    plotPanelCDEF( [4,1,0,3,2], 4 )
    print(("Time taken = ", time.time() - t1))
    plt.tight_layout()
        
    plt.show()
        
        
