import sys
import os
import pylab
import numpy
import math
import moose
print( '[INFO] Using moose from %s' % moose.__file__ )
import moose.utils

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

def create_squid():
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
    xGate = moose.element(nachan.path + '/gateX')
    xGate.setupAlpha(Na_m_params + [VDIVS, VMIN, VMAX])
        #This is important: one can run without it but the output will diverge.
    xGate.useInterpolation = 1
    nachan.Ypower = 1
    yGate = moose.element(nachan.path + '/gateY')
    yGate.setupAlpha(Na_h_params + [VDIVS, VMIN, VMAX])
    yGate.useInterpolation = 1
    nachan.Gbar = 0.942e-3
    nachan.Ek = 115e-3+EREST_ACT
    moose.connect(nachan, 'channel', compt, 'channel', 'OneToOne')

    kchan = moose.HHChannel( '/n/compt/K' )
    kchan.Xpower = 4.0
    xGate = moose.element(kchan.path + '/gateX')
    xGate.setupAlpha(K_n_params + [VDIVS, VMIN, VMAX])
    xGate.useInterpolation = 1
    kchan.Gbar = 0.2836e-3
    kchan.Ek = -12e-3+EREST_ACT
    moose.connect(kchan, 'channel', compt, 'channel', 'OneToOne')
    return compt

def create_spine( parentCompt, parentObj, index, frac, length, dia, theta ):
    """Create spine of specified dimensions and index"""
    RA = 1.0
    RM = 1.0
    CM = 0.01
    sname = 'shaft' + str(index)
    hname = 'head' + str(index)
    shaft = moose.SymCompartment( parentObj.path + '/' + sname )
    #moose.connect( parentCompt, 'cylinder', shaft, 'proximalOnly','Single')
    #moose.connect( parentCompt, 'distal', shaft, 'proximal','Single' )
    moose.connect( parentCompt, 'sphere', shaft, 'proximalOnly','Single' )
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
    xa = math.pi * dia * dia / 400.0
    circumference = math.pi * dia / 10.0
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
    return head

def create_spine_with_receptor( compt, cell, index, frac ):
    FaradayConst = 96485.3415    # s A / mol
    spineLength = 5.0e-6
    spineDia = 4.0e-6
    head = create_spine( compt, cell, index, frac, spineLength, spineDia, 0.0 )
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

    synHandler = moose.SimpleSynHandler( head.path + '/gluR/handler' )
    synHandler.synapse.num = 1
    moose.connect( synHandler, 'activationOut', gluR, 'activation', 'Single' )

    return gluR

def add_plot( objpath, field, plot ):
    assert moose.exists( objpath )
    tab = moose.Table( '/graphs/' + plot )
    obj = moose.element( objpath )
    moose.connect( tab, 'requestOut', obj, field )
    return tab

def make_elec_plots():
    graphs = moose.Neutral( '/graphs' )
    elec = moose.Neutral( '/graphs/elec' )
    add_plot( '/n/compt', 'getVm', 'elec/dendVm' )
    add_plot( '/n/head0', 'getVm', 'elec/head0Vm' )
    add_plot( '/n/head2', 'getVm', 'elec/head2Vm' )
    add_plot( '/n/head2/ca', 'getCa', 'elec/head2Ca' )

def dump_plots( fname ):
    if ( os.path.exists( fname ) ):
        os.remove( fname )
    for x in moose.wildcardFind( '/graphs/##[ISA=Table]' ):
        t = numpy.arange( 0, x.vector.size, 1 )
        pylab.plot( t, x.vector, label=x.name )
    pylab.legend()
    pylab.savefig( fname )
    print('[INFO] Saved to %s' % fname )
    pylab.close()

def make_spiny_compt():
    comptLength = 100e-6
    comptDia = 4e-6
    numSpines = 5
    compt = create_squid()
    compt.inject = 1e-7
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
        r = create_spine_with_receptor( compt, cell, i, i/float(numSpines) )
        #r.synapse.num = 1
        syn = moose.element( r.path + '/handler/synapse' )
        moose.connect( synInput, 'spikeOut', syn, 'addSpike', 'Single' )
        syn.weight = 0.2
        syn.delay = i * 1.0e-4
        """
path = '/n/head' + str(i)
sib1 = moose.element( path )
for j in range( i - 1 ):
     sib2 = moose.element( '/n/head' + str(j) )
     moose.connect( sib1, 'sibling', sib2, 'sibling', 'Single' )
        """

def create_pool( compt, name, concInit ):
    meshEntries = moose.element( compt.path + '/mesh' )
    pool = moose.Pool( compt.path + '/' + name )
    moose.connect( pool, 'mesh', meshEntries, 'mesh', 'Single' )
    pool.concInit = concInit
    return pool

def test_elec_alone():
    eeDt = 2e-6
    hSolveDt = 2e-5
    runTime = 0.02

    make_spiny_compt()
    make_elec_plots()
    head2 = moose.element( '/n/head2' )
    moose.setClock( 0, 2e-6 )
    moose.setClock( 1, 2e-6 )
    moose.setClock( 2, 2e-6 )
    moose.setClock( 8, 0.1e-3 )
    moose.useClock( 0, '/n/##[ISA=Compartment]', 'init' )
    moose.useClock( 1, '/n/##[ISA=Compartment]', 'process' )
    moose.useClock( 2, '/n/##[ISA=ChanBase],/n/##[ISA=SynBase],/n/##[ISA=CaConc],/n/##[ISA=SpikeGen]','process')
    moose.useClock( 8, '/graphs/elec/#', 'process' )
    moose.reinit()
    moose.start( runTime )
    dump_plots( 'instab.png' )
    # make Hsolver and rerun
    hsolve = moose.HSolve( '/n/hsolve' )
    moose.useClock( 1, '/n/hsolve', 'process' )
    hsolve.dt = 20e-6
    hsolve.target = '/n/compt'
    moose.le( '/n' )
    for dt in ( 20e-6, 50e-6, 100e-6 ):
        print(('running at dt =', dt))
        moose.setClock( 0, dt )
        moose.setClock( 1, dt )
        moose.setClock( 2, dt )
        hsolve.dt = dt
        moose.reinit()
        moose.start( runTime )
        dump_plots( 'h_instab' + str( dt ) + '.png' )

def main():
    """
A small compartmental model that demonstrates ::
    a) how to set up a multicompartmental model using SymCompartments
    b) Solving this with the default Exponential Euler (EE) method
    c) Solving this with the Hsolver.
    d) What happens at different timesteps.
    """
    test_elec_alone()

if __name__ == '__main__':
    main()

#
# testHsolve.py ends here
