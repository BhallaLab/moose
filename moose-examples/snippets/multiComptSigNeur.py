# multiComptSigNeur.py --- 
# Upi Bhalla NCBS Bangalore 2013.
# Commentary: 

# A toy compartmental neuronal + chemical model. The neuronal model is in
# a dendrite and five dendritic spines. The chemical model is in three
# compartments: one for the dendrite,
# one for the spine head, and one for the postsynaptic density. However,
# the spatial geometry of the neuronal model is ignored and the chemical
# model just has three cubic volumes for each compartment. So there
# is a functional mapping but spatial considerations are lost.
# The electrical model contributes the incoming calcium flux to the
# chemical model. This comes from the synaptic channels.
# The signalling here does two things to the electrical model. First, the
# amount of receptor in the chemical model controls the amount of glutamate
# receptor in the PSD. Second, there is a small kinase reaction that 
# phosphorylates and inactivates the dendritic potassium channel.
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
# 

# Code:

import sys

import os
import math

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
    compt = moose.SymCompartment( '/n/compt' )
    Em = EREST_ACT + 10.613e-3
    compt.Em = Em
    compt.initVm = EREST_ACT
    compt.Cm = 7.85e-9 * 0.5
    compt.Rm = 4.2e5 * 5.0
    compt.Ra = 7639.44e3
    nachan = moose.HHChannel( '/n/compt/Na' )
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

    kchan = moose.HHChannel( '/n/compt/K' )
    kchan.Xpower = 4.0
    xGate = moose.HHGate(kchan.path + '/gateX')    
    xGate.setupAlpha(K_n_params + [VDIVS, VMIN, VMAX])
    xGate.useInterpolation = 1
    kchan.Gbar = 0.2836e-3
    kchan.Ek = -12e-3+EREST_ACT
    moose.connect(kchan, 'channel', compt, 'channel', 'OneToOne')
    return compt

def createSpine( parentCompt, parentObj, index, frac, length, dia, theta ):
    """Create spine of specified dimensions and index"""
    RA = 1.0
    RM = 1.0
    CM = 0.01
    shaftDia = dia / 5.0
    sname = 'shaft' + str(index)
    hname = 'head' + str(index)
    shaft = moose.SymCompartment( parentObj.path + '/' + sname )
    moose.connect( parentCompt, 'cylinder', shaft, 'proximalOnly','Single' )
    x = parentCompt.x0 + frac * ( parentCompt.x - parentCompt.x0 )
    y = parentCompt.y0 + frac * ( parentCompt.y - parentCompt.y0 )
    z = parentCompt.z0 + frac * ( parentCompt.z - parentCompt.z0 )
    shaft.x0 = x
    shaft.y0 = y
    shaft.z0 = z
    sy = y + length * math.cos( theta * math.pi / 180.0 )
    sz = z + length * math.sin( theta * math.pi / 180.0 )
    shaft.x = x
    shaft.y = sy
    shaft.z = sz
    shaft.diameter = dia / 2.0
    shaft.length = length
    xa = math.pi * shaftDia * shaftDia / 4
    circumference = math.pi * shaftDia
    shaft.Ra = RA * length / xa
    shaft.Rm = RM / ( length * circumference )
    shaft.Cm = CM * length * circumference
    shaft.Em = EREST_ACT
    shaft.initVm = EREST_ACT

    head = moose.SymCompartment( parentObj.path + '/' + hname )
    moose.connect( shaft, 'distal', head, 'proximal', 'Single' )
    head.x0 = x
    head.y0 = sy
    head.z0 = sz
    hy = sy + length * math.cos ( theta * math.pi / 180.0 )
    hz = sz + length * math.sin ( theta * math.pi / 180.0 )
    head.x = x
    head.y = hy
    head.z = hz
    head.diameter = dia
    head.length = length
    xa = math.pi * dia * dia / 4.0
    circumference = math.pi * dia
    head.Ra = RA * length / xa
    head.Rm = RM / ( length * circumference )
    head.Cm = CM * length * circumference
    head.Em = EREST_ACT
    head.initVm = EREST_ACT
    #print head.Rm, head.Ra, head.Cm, head.diameter, head.length
    #print shaft.Rm, shaft.Ra, shaft.Cm, shaft.diameter, shaft.length
    return head

def createSpineWithReceptor( compt, cell, index, frac ):
    FaradayConst = 96485.3415    # s A / mol
    spineLength = 5.0e-6
    spineDia = 4.0e-6
    head = createSpine( compt, cell, index, frac, spineLength, spineDia, 0.0 )
    gluR = moose.SynChan( head.path + '/gluR' )
    gluR.tau1 = 4e-3
    gluR.tau2 = 4e-3
    gluR.Gbar = 1e-6
    gluR.Ek= 10.0e-3
    moose.connect( head, 'channel', gluR, 'channel', 'Single' )
    synh = moose.SimpleSynHandler( gluR.path + '/synh' )
    moose.connect( synh, 'activationOut', gluR, 'activation' )

    caPool = moose.CaConc( head.path + '/ca' )
    caPool.CaBasal = 1e-4       # 0.1 micromolar
    caPool.tau = 0.01
    B = 1.0 / ( FaradayConst * spineLength * spineDia * spineDia *math.pi/4)
    B = B / 20.0                # scaling factor for Ca buffering
    caPool.B = B
    moose.connect( gluR, 'IkOut', caPool, 'current', 'Single' )

    return synh

def addPlot( objpath, field, plot ):
    assert moose.exists( objpath )
    tab = moose.Table( '/graphs/' + plot )
    obj = moose.element( objpath )
    moose.connect( tab, 'requestOut', obj, field )
    return tab

def makeElecPlots():
    graphs = moose.Neutral( '/graphs' )
    elec = moose.Neutral( '/graphs/elec' )
    addPlot( '/model/elec/compt', 'getVm', 'elec/dendVm' )
    #addPlot( '/n/compt/Na', 'getGbar', 'elec/NaGbar' )
    addPlot( '/model/elec/compt/K', 'getGbar', 'elec/KGbar' )
    #addPlot( '/n/compt/Na', 'getIk', 'elec/NaIk' )
    #addPlot( '/n/compt/K', 'getIk', 'elec/KIk' )
    #addPlot( '/n/compt/Na', 'getEk', 'elec/NaEk' )
    #addPlot( '/n/compt/K', 'getEk', 'elec/KEk' )
    addPlot( '/model/elec/head0', 'getVm', 'elec/head0Vm' )
    addPlot( '/model/elec/head2', 'getVm', 'elec/head2Vm' )
    #addPlot( '/n/head2', 'getIm', 'elec/head2Im' )
    addPlot( '/model/elec/head0/ca', 'getCa', 'elec/head0Ca' )
    addPlot( '/model/elec/head1/ca', 'getCa', 'elec/head1Ca' )
    addPlot( '/model/elec/head2/ca', 'getCa', 'elec/head2Ca' )
    addPlot( '/model/elec/head0/gluR', 'getIk', 'elec/head0Ik' )
    addPlot( '/model/elec/head1/gluR', 'getIk', 'elec/head1Ik' )
    addPlot( '/model/elec/head2/gluR', 'getIk', 'elec/head2Ik' )
    addPlot( '/model/elec/head1/gluR', 'getGbar', 'elec/head1Gbar' )
    addPlot( '/model/elec/head2/gluR', 'getGbar', 'elec/head2Gbar' )
    #addPlot( '/n/head0/gluR', 'getGk', 'elec/head0Gk' )
    #addPlot( '/n/head2/gluR', 'getGk', 'elec/head2Gk' )

def dumpPlots( fname ):
    if ( os.path.exists( fname ) ):
        os.remove( fname )
    for x in moose.wildcardFind( '/graphs/##[ISA=Table]' ):
        x.xplot( fname, x.name )

def makeSpinyCompt():
    comptLength = 30e-6
    comptDia = 6e-6
    numSpines = 5
    compt = createSquid()
    compt.inject = 0
    compt.x0 = 0
    compt.y0 = 0
    compt.z0 = 0
    compt.x = comptLength
    compt.y = 0
    compt.z = 0
    compt.length = comptLength
    compt.diameter = comptDia
    #kchan = moose.element( '/n/compt/K' )
    #kchan.Gbar = 0.2e-3
    synInput = moose.SpikeGen( '/n/compt/synInput' )
    synInput.refractT = 47e-3
    synInput.threshold = -1.0
    synInput.edgeTriggered = 0
    synInput.Vm( 0 )
    cell = moose.element( '/n' )
    for i in range( numSpines ):
        r = createSpineWithReceptor( compt, cell, i, i/float(numSpines) )
        r.synapse.num = 1
        syn = moose.element( r.path + '/synapse' )
        moose.connect( synInput, 'spikeOut', syn, 'addSpike', 'Single' )
        syn.weight = 0.2 * i * ( 4 - i )
        syn.delay = i * 1.0e-3

def createPool( compt, name, concInit ):
    meshEntries = moose.element( compt.path + '/mesh' )
    pool = moose.Pool( compt.path + '/' + name )
    pool.concInit = concInit
    pool.diffConst = 1e-11
    return pool


def createChemModel( neuroCompt, spineCompt, psdCompt ):
    # Stuff in spine + psd
    #psdCa = createPool( psdCompt, 'Ca', 0.0001 )
    psdGluR = createPool( psdCompt, 'psdGluR', 1 )
    headCa = createPool( spineCompt, 'Ca', 1e-4 )
    headGluR = createPool( spineCompt, 'headGluR', 2 )
    toPsd = createPool( spineCompt, 'toPsd', 0 )
    toPsdInact = createPool( spineCompt, 'toPsdInact', 1e-3 )
    turnOnPsd = moose.Reac( spineCompt.path +  '/turnOnPsd' )
    moose.connect( turnOnPsd, 'sub', headCa, 'reac', 'OneToOne' )
    moose.connect( turnOnPsd, 'sub', toPsdInact, 'reac', 'OneToOne' )
    moose.connect( turnOnPsd, 'prd', toPsd, 'reac', 'OneToOne' )
    turnOnPsd.Kf = 1e3
    turnOnPsd.Kb = 1
    toPsdEnz = moose.Enz( toPsd.path + '/enz' )
    toPsdEnzCplx = moose.Pool( toPsdEnz.path + '/cplx' )
    toPsdEnzCplx.concInit = 0
    moose.connect( toPsdEnz, 'enz', toPsd, 'reac', 'OneToOne' )
    moose.connect( toPsdEnz, 'sub', headGluR, 'reac', 'OneToOne' )
    moose.connect( toPsdEnz, 'prd', psdGluR, 'reac', 'OneToOne' )
    moose.connect( toPsdEnz, 'cplx', toPsdEnzCplx, 'reac', 'OneToOne' )
    toPsdEnz.Km = 1.0e-3
    toPsdEnz.kcat = 10.0
    fromPsd = moose.Reac( psdCompt.path + '/fromPsd' )
    moose.connect( fromPsd, 'sub', psdGluR, 'reac', 'OneToOne' )
    moose.connect( fromPsd, 'prd', headGluR, 'reac', 'OneToOne' )
    fromPsd.Kf = 0.5
    fromPsd.Kb = 0.0
    # Stuff in dendrite
    dendCa = createPool( neuroCompt, 'Ca', 1e-4 )
    bufCa = moose.Pool( neuroCompt.path + '/bufCa' )
    bufCa.concInit = 1e-4
    pumpCa = moose.Reac( neuroCompt.path + '/pumpCa' )
    moose.connect( pumpCa, 'sub', dendCa, 'reac', 'OneToOne' )
    moose.connect( pumpCa, 'prd', bufCa, 'reac', 'OneToOne' )
    pumpCa.Kf = 1
    pumpCa.Kb = 1
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


# Just for printf debugging
def printMolVecs( title ):
    print(title)
    """    
    nCa = moose.vec( '/model/chem/neuroMesh/Ca' )
    sCa = moose.vec( '/model/chem/spineMesh/Ca' )
    sR = moose.vec( '/model/chem/spineMesh/headGluR' )
    pR = moose.vec( '/model/chem/psdMesh/psdGluR' )
    print 'sizes: nCa, sCa, sR, pR = ', len(nCa), len(sCa), len(sR), len(pR) 
    #print 'nCa=', nCa.conc, ', sCa=', sCa.conc, ', sR=', sR.n, ', pR=', pR.n
    print 'nCaConcInit=', nCa.concInit, ', sCa=', sCa.concInit
    #print 'sRnInit=', sR.nInit, ', pR=', pR.nInit
    print 'sRconcInit=', sR.concInit, ', pR=', pR.concInit

    #print 'nCaSize=', nCa.volume, ', sCa=', sCa.volume, ', sR=', sR.n, ', pR=', pR.n
    """    

def makeChemInCubeMesh():
    dendSide = 10.8e-6
    spineSide = 6.8e-6
    psdSide = 8.565e-7
    parent = moose.Neutral ('/model/chem' )
    neuroMesh = moose.CubeMesh( '/model/chem/neuroMesh' )
    spineMesh = moose.CubeMesh( '/model/chem/spineMesh' )
    psdMesh = moose.CubeMesh( '/model/chem/psdMesh' )
    coords = [dendSide] * 9
    coords[0] = 0
    coords[1] = 0
    coords[2] = 0
    neuroMesh.coords = coords
    neuroMesh.preserveNumEntries = 1

    coords = [spineSide] * 9
    coords[0] = dendSide
    coords[1] = 0
    coords[2] = 0
    coords[3] = spineSide + dendSide
    spineMesh.coords = coords
    spineMesh.preserveNumEntries = 1

    coords = [psdSide] * 9
    coords[0] = dendSide + spineSide
    coords[1] = 0
    coords[2] = 0
    coords[3] = psdSide + spineSide + dendSide
    psdMesh.coords = coords
    psdMesh.preserveNumEntries = 1

    createChemModel( neuroMesh, spineMesh, psdMesh )
    dendCa = moose.element( '/model/chem/neuroMesh/Ca' )
    assert dendCa.volume == dendSide * dendSide * dendSide
    spineCa = moose.element( '/model/chem/spineMesh/Ca' )
    assert spineCa.volume == spineSide * spineSide * spineSide
    psdGluR = moose.element( '/model/chem/psdMesh/psdGluR' )
    assert psdGluR.volume == psdSide * psdSide * psdSide
    dendKinaseEnzCplx = moose.element( '/model/chem/neuroMesh/Ca.kinase/enz/cplx' )
    assert dendKinaseEnzCplx.volume == dendSide * dendSide * dendSide

def makeSolvers( elecDt ):
        # Put in the solvers, see how they fare.
        # Here we kludge in a single chem solver for the whole system.
        ksolve = moose.Ksolve( '/model/ksolve' )
        stoich = moose.Stoich( '/model/stoich' )
        stoich.compartment = moose.element( '/model/chem/neuroMesh' )
        stoich.ksolve = ksolve
        stoich.path = '/model/chem/##'
        #stoich.method = 'rk5'
        moose.useClock( 5, '/model/ksolve', 'init' )
        moose.useClock( 6, '/model/ksolve', 'process' )
        # Here is the elec solver
        hsolve = moose.HSolve( '/model/hsolve' )
        moose.useClock( 1, '/model/hsolve', 'process' )
        hsolve.dt = elecDt
        hsolve.target = '/model/elec/compt'

def makeCubeMultiscale():
    makeSpinyCompt()
    model = moose.Neutral( '/model' )
    elec = moose.element( '/n' )
    elec.name = 'elec'
    moose.move( elec, model )
    synInput = moose.element( '/model/elec/compt/synInput' )
    synInput.refractT = 47e-3
    makeChemInCubeMesh()
    # set up a reaction to fake diffusion between compts.
    headCa = moose.element( '/model/chem/spineMesh/Ca' )
    dendCa = moose.element( '/model/chem/neuroMesh/Ca' )
    diffReac = moose.Reac( '/model/chem/spineMesh/diff' )
    moose.connect( diffReac, 'sub', headCa, 'reac' )
    moose.connect( diffReac, 'prd', dendCa, 'reac' )
    diffReac.Kf = 1 
    diffReac.Kb = headCa.volume / dendCa.volume

    # set up adaptors
    headCa = moose.element( '/model/chem/spineMesh/Ca' )
    dendCa = moose.element( '/model/chem/neuroMesh/Ca' )
    adaptCa = moose.Adaptor( '/model/chem/adaptCa' )
    elecCa = moose.element( '/model/elec/head2/ca' )
    # There are 5 spine heads in the electrical model. Average their input.
    for i in range( 5 ):
        path = '/model/elec/head' + str( i ) + '/ca'
        elecCa = moose.element( path )
        moose.connect( elecCa, 'concOut', adaptCa, 'input', 'Single' )
    moose.connect( adaptCa, 'output', headCa, 'setConc' )
    adaptCa.outputOffset = 0.0001    # 100 nM offset in chem.
    adaptCa.scale = 0.05             # 0.06 to 0.003 mM

    adaptGluR = moose.Adaptor( '/model/chem/psdMesh/adaptGluR' )
    chemR = moose.element( '/model/chem/psdMesh/psdGluR' )
    # Here we connect up the chem adaptors to only 3 of the spine
    # heads in the elec model, just to make it interesting.
    elec1R = moose.element( '/model/elec/head1/gluR' )
    elec2R = moose.element( '/model/elec/head2/gluR' )
    elec3R = moose.element( '/model/elec/head3/gluR' )
    moose.connect( adaptGluR, 'requestOut', chemR, 'getN', 'OneToAll' )
    moose.connect( adaptGluR, 'output', elec1R, 'setGbar', 'OneToAll' )
    moose.connect( adaptGluR, 'output', elec2R, 'setGbar', 'OneToAll' )
    moose.connect( adaptGluR, 'output', elec3R, 'setGbar', 'OneToAll' )
    adaptGluR.outputOffset = 1e-9    # pS
    adaptGluR.scale = 1e-8 / 100    # from n to pS

    adaptK = moose.Adaptor( '/model/chem/neuroMesh/adaptK' )
    chemK = moose.element( '/model/chem/neuroMesh/kChan' )
    elecK = moose.element( '/model/elec/compt/K' )
    moose.connect( adaptK, 'requestOut', chemK, 'getConc', 'OneToAll' )
    moose.connect( adaptK, 'output', elecK, 'setGbar', 'OneToAll' )
    adaptK.scale = 0.3               # from mM to Siemens


def makeChemPlots():
    graphs = moose.Neutral( '/graphs' )
    addPlot( '/model/chem/psdMesh/psdGluR', 'getN', 'psd0R' )
    addPlot( '/model/chem/spineMesh/Ca', 'getConc', 'spine0Ca' )
    addPlot( '/model/chem/neuroMesh/Ca', 'getConc', 'dend0Ca' )
    addPlot( '/model/chem/neuroMesh/kChan_p', 'getConc', 'kChan_p' )
    addPlot( '/model/chem/neuroMesh/kChan', 'getConc', 'kChan' )
    addPlot( '/model/chem/neuroMesh/Ca.kinase', 'getConc', 'dendKinase' )
    addPlot( '/model/chem/spineMesh/toPsd', 'getConc', 'toPsd0' )
    #addPlot( '/n/neuroMesh/Ca', 'getConc', 'dendCa' )
    #addPlot( '/n/neuroMesh/inact_kinase', 'getConc', 'inactDendKinase' )
    #addPlot( '/n/psdMesh/psdGluR', 'getN', 'psdGluR' )

def testCubeMultiscale( useSolver ):
    elecDt = 10e-6
    chemDt = 1e-4
    plotDt = 5e-4
    plotName = 'mc.plot'
    if ( useSolver ):
        elecDt = 50e-6
        chemDt = 2e-3
        plotName = 'mcs.plot'
    makeCubeMultiscale()

    makeChemPlots()
    makeElecPlots()
    moose.setClock( 0, elecDt )
    moose.setClock( 1, elecDt )
    moose.setClock( 2, elecDt )
    moose.setClock( 5, chemDt )
    moose.setClock( 6, chemDt )
    moose.setClock( 7, plotDt )
    moose.setClock( 8, plotDt )
    moose.useClock( 0, '/model/elec/##[ISA=Compartment]', 'init' )
    moose.useClock( 1, '/model/elec/##[ISA=Compartment],/model/elec/##[ISA=SpikeGen]', 'process' )
    moose.useClock( 2, '/model/elec/##[ISA=SynBase],/model/elec/##[ISA=ChanBase],/model/elec/##[ISA=CaConc]','process')
    moose.useClock( 5, '/model/##[ISA=ReacBase],/model/##[ISA=EnzBase]', 'process' )
    moose.useClock( 6, '/model/##[ISA=PoolBase],/model/chem/##[ISA=Adaptor]', 'process' )
    moose.useClock( 7, '/graphs/#', 'process' )
    moose.useClock( 8, '/graphs/elec/#', 'process' )
    if ( useSolver ):
        makeSolvers( elecDt )
    moose.reinit()
    moose.start( 1.0 )
    dumpPlots( plotName )

def main():
    testCubeMultiscale( 1 )

if __name__ == '__main__':
    main()

# 
# HsolveInstability.py ends here
