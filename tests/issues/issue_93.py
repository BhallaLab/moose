# Issue #93 on moose-core 

import numpy as np
import pylab as pl
import moose
import os
import sys

dt = 10e-6

d = os.path.split( os.path.abspath( __file__ ) )[0]

def loadAndRun(solver=True):
    simtime = 500e-3
    model = moose.loadModel('../data/h10.CNG.swc', '/cell')
    comp = moose.element('/cell/apical_e_177_0')
    soma = moose.element('/cell/soma')
    for i in range(10):
        moose.setClock(i, dt)
    if solver:
        solver = moose.HSolve('/cell/solver')
        solver.target = soma.path
        solver.dt = dt
    stim = moose.PulseGen('/cell/stim')
    stim.delay[0] = 50e-3
    stim.delay[1] = 1e9
    stim.level[0] = 1e-9
    stim.width[0] = 2e-3    
    moose.connect(stim, 'output', comp, 'injectMsg')
    tab = moose.Table('/cell/Vm')
    moose.connect(tab, 'requestOut', comp, 'getVm')
    tab_soma = moose.Table('/cell/Vm_soma')
    moose.connect(tab_soma, 'requestOut', soma, 'getVm')
    moose.reinit()
    print('[INFO] Running for %s' % simtime)
    moose.start(simtime )
    vec = tab_soma.vector 
    moose.delete( '/cell' )
    return vec

def main( ):
    print( '[INFO] See the detailed issue in %s/hsolve' % d )
    quit()
    eeVec = loadAndRun( False )
    hsolveVec = loadAndRun( True )
    clk = moose.Clock( '/clock' )
    print( '[DEBUG] Total entries %s' % len( eeVec ))
    t = pl.linspace(0, clk.currentTime, len( eeVec )) 
    pl.plot(t, eeVec, label = 'ee' )
    pl.plot( t, hsolveVec, label = 'hsolve' )
    outfile = '%s.png' % sys.argv[0] 
    pl.legend( )
    pl.savefig( outfile )
    print( '[INFO] Wrote results to %s' % outfile )
    pl.show()

if __name__ == '__main__':
    main()
