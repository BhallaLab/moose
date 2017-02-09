# __DEPRECATED__ __BROKEN__

# testSigNeur.py ---
# Upi Bhalla, NCBS Bangalore 2013.
#
# Commentary:
#
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
sys.path.append('../../python')
import os
os.environ['NUMPTHREADS'] = '1'
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

    caPool = moose.CaConc( head.path + '/ca' )
    caPool.CaBasal = 1e-4       # 0.1 micromolar
    caPool.tau = 0.01
    B = 1.0 / ( FaradayConst * spineLength * spineDia * spineDia *math.pi/4)
    B = B / 20.0                # scaling factor for Ca buffering
    caPool.B = B
    moose.connect( gluR, 'IkOut', caPool, 'current', 'Single' )

    return gluR

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
    addPlot( '/model/elec/head0', 'getVm', 'elec/head0Vm' )
    addPlot( '/model/elec/head2', 'getVm', 'elec/head2Vm' )
    addPlot( '/model/elec/head0/ca', 'getCa', 'elec/head0Ca' )
    addPlot( '/model/elec/head1/ca', 'getCa', 'elec/head1Ca' )
    addPlot( '/model/elec/head2/ca', 'getCa', 'elec/head2Ca' )
    addPlot( '/model/elec/head0/gluR', 'getIk', 'elec/head0Ik' )
    addPlot( '/model/elec/head1/gluR', 'getIk', 'elec/head1Ik' )
    addPlot( '/model/elec/head2/gluR', 'getIk', 'elec/head2Ik' )
    addPlot( '/model/elec/head1/gluR', 'getGbar', 'elec/head1Gbar' )
    addPlot( '/model/elec/head2/gluR', 'getGbar', 'elec/head2Gbar' )

def dumpPlots( fname ):
    if ( os.path.exists( fname ) ):
        os.remove( fname )
    for x in moose.wildcardFind( '/graphs/##[ISA=Table]' ):
        moose.element( x[0] ).xplot( fname, x[0].name )

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
    moose.connect( pool, 'mesh', meshEntries, 'mesh', 'Single' )
    pool.concInit = concInit
    pool.diffConst = 1e-11
    return pool


def createChemModel( neuroCompt, spineCompt, psdCompt ):
    # Stuff in spine + psd
    # The psdCa pool is an unfortunate necessity because of limitations in
    # the solver setup that require molecules to diffuse through all
    # compartments, at least as of the July 2013 version.
    psdCa = createPool( psdCompt, 'Ca', 0.0001 )
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
    mesh = moose.element( spineCompt.path + '/mesh' )
    moose.connect( toPsdEnzCplx, 'mesh', mesh, 'mesh' )
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
    mesh = moose.element( neuroCompt.path + '/mesh' )
    moose.connect( mesh, 'mesh', bufCa, 'mesh', 'Single' )
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
    moose.connect( dendKinaseEnzCplx, 'mesh', mesh, 'mesh' )
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



def makeNeuroMeshModel():
    makeSpinyCompt()
    diffLength = moose.element( '/n/compt' ).length
    diffLength = diffLength / 10.0
    elec = moose.element( '/n' )
    elec.name = 'elec'
    model = moose.Neutral( '/model' )
    moose.move( elec, model )
    synInput = moose.element( '/model/elec/compt/synInput' )
    synInput.refractT = 47e-3

    chem = moose.Neutral( '/model/chem' )
    neuroCompt = moose.NeuroMesh( '/model/chem/neuroMesh' )
    neuroCompt.separateSpines = 1
    neuroCompt.diffLength = diffLength
    neuroCompt.geometryPolicy = 'cylinder'
    spineCompt = moose.SpineMesh( '/model/chem/spineMesh' )
    moose.connect( neuroCompt, 'spineListOut', spineCompt, 'spineList', 'OneToOne' )
    psdCompt = moose.PsdMesh( '/model/chem/psdMesh' )
    moose.connect( neuroCompt, 'psdListOut', psdCompt, 'psdList', 'OneToOne' )

    createChemModel( neuroCompt, spineCompt, psdCompt )

    # Put in the solvers, see how they fare.
    nmksolve = moose.GslStoich( '/model/chem/neuroMesh/ksolve' )
    nmksolve.path = '/model/chem/neuroMesh/##'
    nmksolve.compartment = moose.element( '/model/chem/neuroMesh' )
    nmksolve.method = 'rk5'
    nm = moose.element( '/model/chem/neuroMesh/mesh' )
    moose.connect( nm, 'remesh', nmksolve, 'remesh' )
    #print "neuron: nv=", nmksolve.numLocalVoxels, ", nav=", nmksolve.numAllVoxels, nmksolve.numVarPools, nmksolve.numAllPools

    #print 'setting up smksolve'
    smksolve = moose.GslStoich( '/model/chem/spineMesh/ksolve' )
    smksolve.path = '/model/chem/spineMesh/##'
    smksolve.compartment = moose.element( '/model/chem/spineMesh' )
    smksolve.method = 'rk5'
    sm = moose.element( '/model/chem/spineMesh/mesh' )
    moose.connect( sm, 'remesh', smksolve, 'remesh' )
    #print "spine: nv=", smksolve.numLocalVoxels, ", nav=", smksolve.numAllVoxels, smksolve.numVarPools, smksolve.numAllPools
    #
    #print 'setting up pmksolve'
    pmksolve = moose.GslStoich( '/model/chem/psdMesh/ksolve' )
    pmksolve.path = '/model/chem/psdMesh/##'
    pmksolve.compartment = moose.element( '/model/chem/psdMesh' )
    pmksolve.method = 'rk5'
    pm = moose.element( '/model/chem/psdMesh/mesh' )
    moose.connect( pm, 'remesh', pmksolve, 'remesh' )
    #print "psd: nv=", pmksolve.numLocalVoxels, ", nav=", pmksolve.numAllVoxels, pmksolve.numVarPools, pmksolve.numAllPools
    #

    #print 'Assigning the cell model'
    # Now to set up the model.
    neuroCompt.cell = elec
    ns = neuroCompt.numSegments
    #assert( ns == 11 ) # dend, 5x (shaft+head)
    ndc = neuroCompt.numDiffCompts
    assert( ndc == 10 )
    ndc = neuroCompt.mesh.num
    assert( ndc == 10 )
    sdc = spineCompt.mesh.num
    assert( sdc == 5 )
    pdc = psdCompt.mesh.num
    assert( pdc == 5 )
    #
    # We need to use the spine solver as the master for the purposes of
        # these calculations. This will handle the diffusion calculations
        # between head and dendrite, and between head and PSD.
    smksolve.addJunction( nmksolve )
    #print "spine: nv=", smksolve.numLocalVoxels, ", nav=", smksolve.numAllVoxels, smksolve.numVarPools, smksolve.numAllPools
    smksolve.addJunction( pmksolve )
    #print "psd: nv=", pmksolve.numLocalVoxels, ", nav=", pmksolve.numAllVoxels, pmksolve.numVarPools, pmksolve.numAllPools
    # Have to pass a message between the various solvers.
    foo = moose.vec( '/model/chem/spineMesh/headGluR' )

    # oddly, numLocalFields does not work.
    ca = moose.element( '/model/chem/neuroMesh/Ca' )
    assert( ca.lastDimension == ndc )

    moose.vec( '/model/chem/spineMesh/headGluR' ).nInit = 100
    moose.vec( '/model/chem/psdMesh/psdGluR' ).nInit = 0

    # set up adaptors
    aCa = moose.Adaptor( '/model/chem/spineMesh/adaptCa', 5 )
    adaptCa = moose.vec( '/model/chem/spineMesh/adaptCa' )
    chemCa = moose.vec( '/model/chem/spineMesh/Ca' )
    assert( len( adaptCa ) == 5 )
    for i in range( 5 ):
        path = '/model/elec/head' + str( i ) + '/ca'
        elecCa = moose.element( path )
        moose.connect( elecCa, 'concOut', adaptCa[i], 'input', 'Single' )
    moose.connect( adaptCa, 'outputSrc', chemCa, 'setConc', 'OneToOne' )
    adaptCa.outputOffset = 0.0001    # 100 nM offset in chem.
    adaptCa.scale = 0.05             # 0.06 to 0.003 mM

    aGluR = moose.Adaptor( '/model/chem/psdMesh/adaptGluR', 5 )
    adaptGluR = moose.vec( '/model/chem/psdMesh/adaptGluR' )
    chemR = moose.vec( '/model/chem/psdMesh/psdGluR' )
    assert( len( adaptGluR ) == 5 )
    for i in range( 5 ):
        path = '/model/elec/head' + str( i ) + '/gluR'
        elecR = moose.element( path )
        moose.connect( adaptGluR[i], 'outputSrc', elecR, 'setGbar', 'Single' )
    #moose.connect( chemR, 'nOut', adaptGluR, 'input', 'OneToOne' )
        # Ksolve isn't sending nOut. Not good. So have to use requestOut.
    moose.connect( adaptGluR, 'requestOut', chemR, 'getN', 'OneToOne' )
    adaptGluR.outputOffset = 1e-7    # pS
    adaptGluR.scale = 1e-6 / 100     # from n to pS

    adaptK = moose.Adaptor( '/model/chem/neuroMesh/adaptK' )
    chemK = moose.element( '/model/chem/neuroMesh/kChan' )
    elecK = moose.element( '/model/elec/compt/K' )
    moose.connect( adaptK, 'requestOut', chemK, 'getConc', 'OneToAll' )
    moose.connect( adaptK, 'outputSrc', elecK, 'setGbar', 'Single' )
    adaptK.scale = 0.3               # from mM to Siemens
    """
    """


def makeChemPlots():
    graphs = moose.Neutral( '/graphs' )
    addPlot( '/model/chem/psdMesh/psdGluR[0]', 'getN', 'psd0R' )
    addPlot( '/model/chem/psdMesh/psdGluR[1]', 'getN', 'psd1R' )
    addPlot( '/model/chem/psdMesh/psdGluR[2]', 'getN', 'psd2R' )
    addPlot( '/model/chem/spineMesh/Ca[0]', 'getConc', 'spine0Ca' )
    addPlot( '/model/chem/spineMesh/Ca[1]', 'getConc', 'spine1Ca' )
    addPlot( '/model/chem/spineMesh/Ca[2]', 'getConc', 'spine2Ca' )
    addPlot( '/model/chem/neuroMesh/Ca[0]', 'getConc', 'dend0Ca' )
    addPlot( '/model/chem/neuroMesh/Ca[1]', 'getConc', 'dend1Ca' )
    addPlot( '/model/chem/neuroMesh/Ca[2]', 'getConc', 'dend2Ca' )
    addPlot( '/model/chem/neuroMesh/Ca[3]', 'getConc', 'dend3Ca' )
    addPlot( '/model/chem/neuroMesh/Ca[6]', 'getConc', 'dend6Ca' )
    addPlot( '/model/chem/neuroMesh/Ca[9]', 'getConc', 'dend9Ca' )
    addPlot( '/model/chem/neuroMesh/kChan_p[4]', 'getConc', 'kChan_p4' )
    addPlot( '/model/chem/neuroMesh/kChan[4]', 'getConc', 'kChan4' )
    addPlot( '/model/chem/neuroMesh/Ca.kinase[4]', 'getConc', 'dendKinase4' )
    addPlot( '/model/chem/spineMesh/toPsd[0]', 'getConc', 'toPsd0' )
    addPlot( '/model/chem/spineMesh/toPsd[2]', 'getConc', 'toPsd2' )
    addPlot( '/model/chem/spineMesh/toPsd[4]', 'getConc', 'toPsd4' )
    #addPlot( '/n/neuroMesh/Ca', 'getConc', 'dendCa' )
    #addPlot( '/n/neuroMesh/inact_kinase', 'getConc', 'inactDendKinase' )
    #addPlot( '/n/psdMesh/psdGluR', 'getN', 'psdGluR' )

def testNeuroMeshMultiscale():
    elecDt = 50e-6
    chemDt = 2e-3
    plotDt = 5e-4
    plotName = 'nm.plot'

    makeNeuroMeshModel()
    """
    moose.le( '/model/chem/spineMesh/ksolve' )
    print 'Neighbors:'
    for t in moose.element( '/model/chem/spineMesh/ksolve/junction' ).neighbors['masterJunction']:
        print 'masterJunction <-', t.path
    for t in moose.wildcardFind( '/model/chem/#Mesh/ksolve' ):
        k = moose.element( t[0] )
        print k.path + ' localVoxels=', k.numLocalVoxels, ', allVoxels= ', k.numAllVoxels
    """

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
    moose.useClock( 1, '/model/elec/##[ISA=SpikeGen]', 'process' )
    moose.useClock( 2, '/model/elec/##[ISA=ChanBase],/model/##[ISA=SynBase],/model/##[ISA=CaConc]','process')
    moose.useClock( 5, '/model/chem/##[ISA=PoolBase],/model/##[ISA=ReacBase],/model/##[ISA=EnzBase]', 'process' )
    moose.useClock( 6, '/model/chem/##[ISA=Adaptor]', 'process' )
    moose.useClock( 7, '/graphs/#', 'process' )
    moose.useClock( 8, '/graphs/elec/#', 'process' )
    moose.useClock( 5, '/model/chem/#Mesh/ksolve', 'init' )
    moose.useClock( 6, '/model/chem/#Mesh/ksolve', 'process' )
    hsolve = moose.HSolve( '/model/elec/hsolve' )
    moose.useClock( 1, '/model/elec/hsolve', 'process' )
    hsolve.dt = elecDt
    hsolve.target = '/model/elec/compt'
    moose.reinit()
    moose.reinit()

    moose.start( 1.0 )
    dumpPlots( plotName )


def main():
    """
    A toy compartmental neuronal + chemical model. The neuronal model
    geometry sets up the chemical volume to match the parent dendrite
    and five dendritic spines, each with a shaft and head. This volume
    mapping uses the NeuroMesh, SpineMesh and PsdMesh classes from MOOSE.
    There is a
    3-compartment chemical model to go with this: one for the dendrite,
    one for the spine head, and one for the postsynaptic density. Note
    that the three mesh classes distribute the chemical model appropriately
    to all the respective spines, and set up the diffusion to the dendrite.
    The electrical model contributes the incoming calcium flux to the
    chemical model. This comes from the synaptic channels.
    The signalling here does two things to the electrical model. First, the
    amount of receptor in the chemical model controls the amount of glutamate
    receptor in the PSD. Second, there is a small kinase reaction that
    phosphorylates and inactivates the dendritic potassium channel.
    """
    testNeuroMeshMultiscale()

if __name__ == '__main__':
    main()

#
# testSigNeur.py ends here.
