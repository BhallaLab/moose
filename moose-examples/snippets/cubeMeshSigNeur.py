# HsolveInstability.py --- 

# Commentary: 
# 
# A toy compartmental neuronal + chemical model in just a cubic volume
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.

# Code:

import sys
sys.path.append('../../python')
import os
os.environ['NUMPTHREADS'] = '1'
import math
import numpy
import pylab

import moose

EREST_ACT = -70e-3

# Gate equations have the form:
#
# y(x) = (A + B * x) / (C + exp((x + D) / F))
# 
# where x is membrane voltage and y is the rate constant for gate
# closing or opening

Na_m_params = [1e5 * (25e-3 + EREST_ACT),   # 'A_A':
                -1e5,                       # 'A_B':
                -1.0,                       # 'A_C':
                -25e-3 - EREST_ACT,         # 'A_D':
               -10e-3,                      # 'A_F':
                4e3,                     # 'B_A':
                0.0,                        # 'B_B':
                0.0,                        # 'B_C':
                0.0 - EREST_ACT,            # 'B_D':
                18e-3                       # 'B_F':    
               ]
Na_h_params = [ 70.0,                        # 'A_A':
                0.0,                       # 'A_B':
                0.0,                       # 'A_C':
                0.0 - EREST_ACT,           # 'A_D':
                0.02,                     # 'A_F':
                1000.0,                       # 'B_A':
                0.0,                       # 'B_B':
                1.0,                       # 'B_C':
                -30e-3 - EREST_ACT,        # 'B_D':
                -0.01                    # 'B_F':       
                ]        
K_n_params = [ 1e4 * (10e-3 + EREST_ACT),   #  'A_A':
               -1e4,                      #  'A_B':
               -1.0,                       #  'A_C':
               -10e-3 - EREST_ACT,         #  'A_D':
               -10e-3,                     #  'A_F':
               0.125e3,                   #  'B_A':
               0.0,                        #  'B_B':
               0.0,                        #  'B_C':
               0.0 - EREST_ACT,            #  'B_D':
               80e-3                       #  'B_F':  
               ]
VMIN = -30e-3 + EREST_ACT
VMAX = 120e-3 + EREST_ACT
VDIVS = 3000

def createSquid():
    """Create a single compartment squid model."""
    parent = moose.Neutral ('/n' )
    elec = moose.Neutral ('/n/elec' )
    compt = moose.SymCompartment( '/n/elec/compt' )
    Em = EREST_ACT + 10.613e-3
    compt.Em = Em
    compt.initVm = EREST_ACT
    compt.Cm = 7.85e-9 * 0.5
    compt.Rm = 4.2e5 * 5.0
    compt.Ra = 7639.44e3
    compt.length = 100e-6
    compt.diameter = 4e-6
    nachan = moose.HHChannel( '/n/elec/compt/Na' )
    nachan.Xpower = 3
    xGate = moose.HHGate(nachan.path + '/gateX')    
    xGate.setupAlpha(Na_m_params + [VDIVS, VMIN, VMAX])
    xGate.useInterpolation = 1
    nachan.Ypower = 1
    yGate = moose.HHGate(nachan.path + '/gateY')
    yGate.setupAlpha(Na_h_params + [VDIVS, VMIN, VMAX])
    yGate.useInterpolation = 1
    nachan.Gbar = 0.942e-3
    nachan.Ek = 115e-3+EREST_ACT
    moose.connect(nachan, 'channel', compt, 'channel', 'OneToOne')

    kchan = moose.HHChannel( '/n/elec/compt/K' )
    kchan.Xpower = 4.0
    xGate = moose.HHGate(kchan.path + '/gateX')    
    xGate.setupAlpha(K_n_params + [VDIVS, VMIN, VMAX])
    xGate.useInterpolation = 1
    kchan.Gbar = 0.2836e-3
    kchan.Ek = -12e-3+EREST_ACT
    moose.connect(kchan, 'channel', compt, 'channel', 'OneToOne')
    return compt

def createSynapseOnCompartment( compt ):
    FaradayConst = 96485.3415    # s A / mol
    length = compt.length
    dia = compt.diameter

    gluR = moose.SynChan( compt.path + '/gluR' )
    gluR.tau1 = 4e-3
    gluR.tau2 = 4e-3
    gluR.Gbar = 1e-6
    gluR.Ek= 10.0e-3
    moose.connect( compt, 'channel', gluR, 'channel', 'Single' )
    gluSyn = moose.SimpleSynHandler( compt.path + '/gluR/sh' )
    moose.connect( gluSyn, 'activationOut', gluR, 'activation' )
    gluSyn.synapse.num = 1
    # Ca comes in through this channel, at least for this example.
    caPool = moose.CaConc( compt.path + '/ca' )
    caPool.CaBasal = 1e-4       # 0.1 micromolar
    caPool.tau = 0.01
    B = 1.0 / ( FaradayConst * length * dia * dia * math.pi / 4)
    B = B / 20.0                # scaling factor for Ca buffering
    caPool.B = B
    moose.connect( gluR, 'IkOut', caPool, 'current', 'Single' )
    # Provide a regular synaptic input.
    synInput = moose.SpikeGen( '/n/elec/compt/synInput' )
    synInput.refractT = 47e-3
    synInput.threshold = -1.0
    synInput.edgeTriggered = 0
    synInput.Vm( 0 )
    syn = moose.element( gluSyn.path + '/synapse' )
    moose.connect( synInput, 'spikeOut', syn, 'addSpike', 'Single' )
    syn.weight = 0.2
    syn.delay = 1.0e-3
    return gluR

def createPool( compt, name, concInit ):
    pool = moose.Pool( compt.path + '/' + name )
    pool.concInit = concInit
    pool.diffConst = 5e-11
    return pool

# This is a Ca-activated enzyme that phosphorylates and inactivates kChan
# as per the following scheme:
# 		Ca + inact_kinase <===> Ca.kinase
# 		kChan ----- Ca.kinase -----> kChan_p
# 		kChan_p -------> kChan
def createChemModel( neuroCompt ):
    dendCa = createPool( neuroCompt, 'Ca', 1e-4 )
    dendKinaseInact = createPool( neuroCompt, 'inact_kinase', 1e-4 )
    dendKinase = createPool( neuroCompt, 'Ca.kinase', 0.0 )
    dendTurnOnKinase = moose.Reac( neuroCompt.path + '/turnOnKinase' )
    moose.connect( dendTurnOnKinase, 'sub', dendCa, 'reac' )
    moose.connect( dendTurnOnKinase, 'sub', dendKinaseInact, 'reac' )
    moose.connect( dendTurnOnKinase, 'prd', dendKinase, 'reac' )
    dendTurnOnKinase.Kf = 50000
    dendTurnOnKinase.Kb = 1
    dendKinaseEnz = moose.Enz( dendKinase.path + '/enz' )
    dendKinaseEnzCplx = moose.Pool( dendKinase.path + '/enz/cplx' )
    kChan = createPool( neuroCompt, 'kChan', 1e-3 )
    kChan_p = createPool( neuroCompt, 'kChan_p', 0.0 )
    moose.connect( dendKinaseEnz, 'enz', dendKinase, 'reac', 'OneToOne' )
    moose.connect( dendKinaseEnz, 'sub', kChan, 'reac', 'OneToOne' )
    moose.connect( dendKinaseEnz, 'prd', kChan_p, 'reac', 'OneToOne' )
    moose.connect( dendKinaseEnz, 'cplx', dendKinaseEnzCplx, 'reac', 'OneToOne' )
    dendKinaseEnz.Km = 1e-4
    dendKinaseEnz.kcat = 20
    dendPhosphatase = moose.Reac( neuroCompt.path + '/phosphatase' )
    moose.connect( dendPhosphatase, 'sub', kChan_p, 'reac' )
    moose.connect( dendPhosphatase, 'prd', kChan, 'reac' )
    dendPhosphatase.Kf = 1
    dendPhosphatase.Kb = 0.0

def makeModelInCubeMesh():
    compt = createSquid()
    createSynapseOnCompartment( compt )
    chem = moose.Neutral( '/n/chem' )
    neuroMesh = moose.CubeMesh( '/n/chem/neuroMesh' )
    coords = [0] * 9
    coords[3] = compt.length
    coords[4] = compt.diameter
    coords[5] = compt.diameter
    coords[6] = compt.length
    coords[7] = compt.diameter
    coords[8] = compt.diameter
    neuroMesh.coords = coords
    neuroMesh.preserveNumEntries = 1
    createChemModel( neuroMesh )
    dendCa = moose.element( '/n/chem/neuroMesh/Ca' )
    assert dendCa.volume == compt.length * compt.diameter * compt.diameter
    dendKinaseEnzCplx = moose.element( '/n/chem/neuroMesh/Ca.kinase/enz/cplx' )
    assert dendKinaseEnzCplx.volume == dendCa.volume
    # Make adaptors
    # Note that we can do this two ways: We can use an existing output
    # msg from the object, which will come whenever the object processes,
    # or the adapator can request the object for the field, which happens 
    # whenever the adaptor processes. Here we illustrate both alternatives.
    adaptK = moose.Adaptor( '/n/chem/neuroMesh/adaptK' )
    chemK = moose.element( '/n/chem/neuroMesh/kChan' )
    elecK = moose.element( '/n/elec/compt/K' )
    moose.connect( adaptK, 'requestOut', chemK, 'getConc', 'OneToAll' )
    moose.connect( adaptK, 'output', elecK, 'setGbar', 'OneToAll' )
    adaptK.scale = 0.3               # from mM to Siemens

    adaptCa = moose.Adaptor( '/n/chem/neuroMesh/adaptCa' )
    chemCa = moose.element( '/n/chem/neuroMesh/Ca' )
    elecCa = moose.element( '/n/elec/compt/ca' )
    moose.connect( elecCa, 'concOut', adaptCa, 'input', 'OneToAll' )
    moose.connect( adaptCa, 'output', chemCa, 'setConc', 'OneToAll' )
    adaptCa.outputOffset = 0.0001      # 100 nM offset in chem conc
    adaptCa.scale = 0.05               # Empirical: 0.06 max to 0.003 mM

def addPlot( objpath, field, plot ):
    assert moose.exists( objpath )
    tab = moose.Table( '/graphs/' + plot )
    obj = moose.element( objpath )
    moose.connect( tab, 'requestOut', obj, field )
    return tab

def displayPlots():
    for x in moose.wildcardFind( '/graphs/##[ISA=Table]' ):
        t = numpy.arange( 0, x.vector.size, 1 ) * x.dt
        pylab.plot( t, x.vector, label=x.name )
    pylab.legend()
    pylab.show()

def makeElecPlots():
    graphs = moose.Neutral( '/graphs' )
    elec = moose.Neutral( '/graphs/elec' )
    addPlot( '/n/elec/compt', 'getVm', 'elec/Vm' )
    addPlot( '/n/elec/compt/ca', 'getCa', 'elec/Ca' )
    addPlot( '/n/elec/compt/K', 'getGk', 'elec/K_Gk' )

def makeChemPlots():
    graphs = moose.Neutral( '/graphs' )
    addPlot( '/n/chem/neuroMesh/Ca', 'getConc', 'chemCa' )
    addPlot( '/n/chem/neuroMesh/kChan_p', 'getConc', 'chemkChan_p' )
    addPlot( '/n/chem/neuroMesh/kChan', 'getConc', 'chemkChan' )
    addPlot( '/n/chem/neuroMesh/Ca.kinase', 'getConc', 'activeKinase' )

def testCubeMultiscale( useSolver ):
    elecDt = 10e-6
    chemDt = 1e-4
    plotDt = 5e-4
    plotName = 'cm.plot'
    if ( useSolver ):
        elecDt = 50e-6
        chemDt = 2e-3
        plotName = 'solve_cm.plot'

    makeModelInCubeMesh()
    makeChemPlots()
    makeElecPlots()
    '''
    moose.setClock( 0, elecDt )
    moose.setClock( 1, elecDt )
    moose.setClock( 2, elecDt )
    moose.setClock( 5, chemDt )
    moose.setClock( 6, chemDt )
    moose.setClock( 7, plotDt )
    moose.setClock( 8, plotDt )
    moose.useClock( 1, '/n/##[ISA=SpikeGen]', 'process' )
    moose.useClock( 2, '/n/##[ISA=SynBase]','process')
    moose.useClock( 6, '/n/##[ISA=Adaptor]', 'process' )
    moose.useClock( 7, '/graphs/#', 'process' )
    moose.useClock( 8, '/graphs/elec/#', 'process' )
    moose.useClock( 0, '/n/##[ISA=Compartment]', 'init' )
    moose.useClock( 1, '/n/##[ISA=Compartment]', 'process' )
    moose.useClock( 2, '/n/##[ISA=ChanBase],/n/##[ISA=SynBase],/n/##[ISA=CaConc]','process')
    moose.useClock( 5, '/n/##[ISA=PoolBase],/n/##[ISA=ReacBase],/n/##[ISA=EnzBase]', 'process' )
    '''
    if ( useSolver ):
        ksolve = moose.Ksolve( '/n/ksolve' )
        stoich = moose.Stoich( '/n/stoich' )
        stoich.compartment = moose.element( '/n/chem/neuroMesh' )
        stoich.ksolve = ksolve
        stoich.path = '/n/##'
        ksolve.method = 'rk5'
        #moose.useClock( 5, '/n/ksolve', 'process' )
        hsolve = moose.HSolve( '/n/hsolve' )
        #moose.useClock( 1, '/n/hsolve', 'process' )
        hsolve.dt = elecDt
        hsolve.target = '/n/compt'
    moose.reinit()
    moose.start( 1 )
    displayPlots()

def main():
    testCubeMultiscale( 1 ) # change argument to 0 to run without solver.

if __name__ == '__main__':
    main()

# cubeMeshSigNeur.py ends here
