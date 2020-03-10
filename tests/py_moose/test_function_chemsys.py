# -*- coding: utf-8 -*-
"""test_muparser.py:

odified from https://elifesciences.org/articles/25827 
Fig4.py
"""
import sys
import re
import numpy as np
import moose
import rdesigneur as rd
print( "[INFO ] Using moose from %s" % moose.__file__ )

def parseExpr( expr, params, hasCa ):
    if hasCa:
        expr = expr.replace( 'Ca', 'x0' )
        expr = expr.replace( 'A', 'x1' )
        expr = expr.replace( 'B', 'x2' )
    else:
        expr = expr.replace( 'Ca', 'x0' ) # happens in the negFF model
        expr = expr.replace( 'A', 'x0' ) # This is the usual case.
        expr = expr.replace( 'B', 'x1' )

    parts = re.split( 'k', expr )
    ret = parts[0]
    for i in parts[1:]:
        ret += str( params[ 'k' + i[:2] ] )
        ret += i[2:]

    if hasCa:
        return 'x3*( ' + ret + ')'
    else:
        return 'x2*( ' + ret + ')'

def makeChemProto( name, Aexpr, Bexpr, params ):
    sw = params['stimWidth']
    diffLength = params['diffusionLength']
    dca = params['diffConstA'] * diffLength * diffLength
    dcb = params['diffConstB'] * diffLength * diffLength

    # Objects
    chem = moose.Neutral( '/library/' + name )
    compt = moose.CubeMesh( '/library/' + name + '/' + name )
    A = moose.Pool( compt.path + '/A' )
    B = moose.Pool( compt.path + '/B' )
    Z = moose.BufPool( compt.path + '/Z' )
    Ca = moose.BufPool( compt.path + '/Ca' )
    phase = moose.BufPool( compt.path + '/phase' )
    vel = moose.BufPool( compt.path + '/vel' )
    ampl = moose.BufPool( compt.path + '/ampl' )
    Adot = moose.Function( A.path + '/Adot' )
    Bdot = moose.Function( B.path + '/Bdot' )
    CaStim = moose.Function( Ca.path + '/CaStim' )
    A.diffConst = dca
    B.diffConst = dcb

    # Equations

    Adot.expr = parseExpr( Aexpr, params, True )
    Bdot.expr = parseExpr( Bexpr, params, False )
    CaStim.expr = 'x2 * exp( -((x0 - t)^2)/(2* ' + str(sw*sw) + ') )'

    #print Adot.expr
    #print Bdot.expr
    #print CaStim.expr

    # Connections
    Adot.x.num = 4
    moose.connect( Ca, 'nOut', Adot.x[0], 'input' )
    moose.connect( A, 'nOut', Adot.x[1], 'input' )
    moose.connect( B, 'nOut', Adot.x[2], 'input' )
    moose.connect( Z, 'nOut', Adot.x[3], 'input' )
    moose.connect( Adot, 'valueOut', A, 'increment' )

    Bdot.x.num = 3
    if name[:5] == 'negFF':
        moose.connect( Ca, 'nOut', Bdot.x[0], 'input' )
        print('Doing special msg')
    else:
        moose.connect( A, 'nOut', Bdot.x[0], 'input' )
    moose.connect( B, 'nOut', Bdot.x[1], 'input' )
    moose.connect( Z, 'nOut', Bdot.x[2], 'input' )
    moose.connect( Bdot, 'valueOut', B, 'increment' )

    CaStim.x.num = 3
    moose.connect( phase, 'nOut', CaStim.x[0], 'input' )
    moose.connect( vel, 'nOut', CaStim.x[1], 'input' )
    moose.connect( ampl, 'nOut', CaStim.x[2], 'input' )
    moose.connect( CaStim, 'valueOut', Ca, 'setN' )

    return compt


def makeBis( args = None ):
    params = {
        'k0a':0.1,  # Constant
        'k1a':-5.0,  # Coeff for A
        'k2a':5.0,  # Coeff for A^2
        'k3a':-1.0,  # Coeff for A^3
        'k4a':10.0,  # turnon of A by A and Ca
        'k5a':-5.0,  # Turnoff of A by B
        'k1b':0.01,  # turnon of B by A
        'k2b':-0.01,   # Decay rate of B
        'diffusionLength':1.0e-6,  # Diffusion characteristic length, used as voxel length too.
        'dendDiameter': 10e-6,  # Diameter of section of dendrite in model
        'dendLength': 100e-6,   # Length of section of dendrite in model
        'diffConstA':5.0,       # Diffusion constant of A
        'diffConstB':2.0,       # Diffusion constant of B
        'stimWidth' :1.0,        # Stimulus width in seconds
        'stimAmplitude':1.0,    # Stimulus amplitude, arb units. From FHN review
        'blankVoxelsAtEnd':10,  # of voxels to leave blank at end of cylinder
        'preStimTime':10.0,     # Time to run before turning on stimulus.
        'postStimTime':40.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus.
                              # To include in analysis of total response over
                              # entire dendrite.
        'fnumber':1,          # Number to append to fname
    }

    for i in args:
        params[i] = args[i]

    makeChemProto( 'bis',
        'k0a + k1a*A + k2a*A*A + k3a*A*A*A + k4a*Ca*A/(1+A+10*B) + k5a*A*B',
        'k1b*A*A + k2b*B',
        params )
    return params

def makeFHN( args = None ):
    params = {
        'k_t':2.5,  # Time-const.
        'k_a':0.7,  # Coeff1
        'k_b':0.8,  # Coeff2
        'kxa': 2.0, # Offset for V for FHN eqns.
        'kxb': 0.8, # Offset for W for FHN eqns.
        'diffusionLength':1.0e-6,  # Diffusion characteristic length, used as voxel length too.
        'dendDiameter': 10e-6,  # Diameter of section of dendrite in model
        'dendLength': 100e-6,   # Length of section of dendrite in model
        'diffConstA':7.5,      # Diffusion constant of A
        'diffConstB':5.0,       # Diffusion constant of B
        'stimWidth' :1.0,        # Stimulus width in seconds
        'stimAmplitude':0.4,    # Stimulus amplitude, arb units. From FHN review
        'blankVoxelsAtEnd':10,  # of voxels to leave blank at end of cylinder
        'preStimTime':10.0,     # Time to run before turning on stimulus.
        'postStimTime':40.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus.
                              # To include in analysis of total response over
                              # entire dendrite.
        'fnumber': 1,         # Number to append to fname
    }

    for i in args:
        params[i] = args[i]

    makeChemProto( 'fhn',
        '5.0*(A - kxa - (A-kxa)^3/3 - (B-kxb) + Ca)',
        '(A-kxa + k_a - k_b*(B-kxb))/k_t',
        params )
    # We do this to get the system to settle at the start.
    B = moose.element( '/library/fhn/fhn/B' )
    B.nInit = 0.2
    return params


def makeNegFB( args = None ):
    params = {
        'k1a':-0.1,  # Coeff for decay of A, slow.
        'k2a':-0.2,  # Coeff for turnoff of A by B, medium.
        'k3a':10.0,  # turnon of A by Ca, fast.
        'k1b':0.2,  # turnon of B by A, medium
        'k2b':-0.1,   # Decay rate of B, slow
        'diffusionLength':1.0e-6,  # Diffusion characteristic length, used as voxel length too.
        'dendDiameter': 10e-6,  # Diameter of section of dendrite in model
        'dendLength': 100e-6,   # Length of section of dendrite in model
        'diffConstA':0.5,      # Diffusion constant of A
        'diffConstB':1.0,       # Diffusion constant of B
        'stimWidth' :1.0,        # Stimulus width in seconds
        'stimAmplitude':1.0,    # Stimulus amplitude, arb units. From FHN review
        'blankVoxelsAtEnd':10,  #of voxels to leave blank at end of cylinder
        'preStimTime':10.0,     # Time to run before turning on stimulus.
        'postStimTime':40.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus.
                              # To include in analysis of total response over
                              # entire dendrite.
        'fnumber':1,          # Number to append to fname
    }

    for i in args:
        params[i] = args[i]

    makeChemProto( 'negFB',
        'k1a*A + k2a*A*B + k3a*Ca',
        'k1b*A + k2b*B',
        params )
    return params

# Was negFF2 in earlier versions of abstrModelEqns
def makeNegFF( args = None ):
    params = {
        'k1a':-0.1,  # Coeff for decay of A, slow.
        'k2a':-0.01,  # Coeff for turnoff of A by B, medium.
        'k3a':10.0,  # turnon of A by Ca, fast.
        'k4a':40.0,   # amount of B inhibition of turnon of A by Ca.
        'k1b':2.0,  # turnon of B by Ca, medium
        'k2b':-0.05,   # Decay rate of B, slow
        'diffusionLength':1.0e-6,  # Diffusion characteristic length, used as voxel length too.
        'dendDiameter': 10e-6,  # Diameter of section of dendrite in model
        'dendLength': 100e-6,   # Length of section of dendrite in model
        'diffConstA':0.02,      # Diffusion constant of A
        'diffConstB':0.4,       # Diffusion constant of B
        'stimWidth' :1.0,        # Stimulus width in seconds
        'stimAmplitude':10.0,    # Stimulus amplitude, arb units. From FHN review
        'blankVoxelsAtEnd':10,  # of voxels to leave blank at end of cylinder
        'preStimTime':10.0,     # Time to run before turning on stimulus.
        'postStimTime':40.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus.
                              # To include in analysis of total response over
                              # entire dendrite.
        'fnumber':1,          # Number to append to fname
    }

    for i in args:
        params[i] = args[i]

    makeChemProto( 'negFF',
        'k1a*A + k2a*A*B + k3a*Ca/(1+k4a*B*B)',
        'k1b*Ca + k2b*B',
        params )

    return params


def singleCompt( name, params ):
    print('=============')
    print('[INFO] Making compartment %s' % name)
    mod = moose.copy( '/library/' + name + '/' + name, '/model' )
    A = moose.element( mod.path + '/A' )
    Z = moose.element( mod.path + '/Z' )
    Z.nInit = 1
    Ca = moose.element( mod.path + '/Ca' )
    CaStim = moose.element( Ca.path + '/CaStim' )

    print( '\n\n[INFO] CaStim %s' % CaStim.path )
    runtime = params['preStimTime'] + params['postStimTime']
    steptime = 50
    CaStim.expr += '+x2*(t>100+'+str(runtime)+')*(t<100+'+str(runtime+steptime)+ ')'
    print("[INFO] CaStim.expr = %s" % CaStim.expr)
    tab = moose.Table2( '/model/' + name + '/Atab' )
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

    print( 'REINIT AND START' )
    moose.reinit()
    runtime += 100 + steptime*2
    moose.start( runtime )
    t = np.arange( 0, runtime + 1e-9, tab.dt )
    return name, t, tab.vector


def runPanelDEFG( name, dist, seqDt, numSpine, seq, stimAmpl ):
    preStim = 10.0
    blanks = 20
    rdes = rd.rdesigneur(
        useGssa = False,
        turnOffElec = True,
        chemPlotDt = 0.1,
        diffusionLength = 1e-6,
        cellProto = [['cell', 'soma']],
        chemProto = [['dend', name]],
        chemDistrib = [['dend', 'soma', 'install', '1' ]],
        plotList = [['soma', '1', 'dend' + '/A', 'n', '# of A']],
    )
    rdes.buildModel()
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
        k = int( blanks + j * stride )
        Z[k].nInit = 1
        phase[k].nInit = preStim + seq[j] * seqDt

    moose.reinit()
    runtime = 50
    snapshot = preStim + seqDt * (numSpine - 0.8)
    print(snapshot)
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

def run():
    panelC = []
    panelCticks = []
    panelC.append( singleCompt( 'negFB', makeNegFB( [] ) ) )
    panelC.append( singleCompt( 'negFF', makeNegFF( [] ) ) )
    panelC.append( singleCompt( 'fhn', makeFHN( [] ) ) )
    panelC.append( singleCompt( 'bis', makeBis( [] ) ) )
    panelCticks.append( np.arange( 0, 15.00001, 5 ) )
    panelCticks.append( np.arange( 0, 1.50001, 0.5 ) )
    panelCticks.append( np.arange( 0, 5.00002, 1 ) )
    panelCticks.append( np.arange( 0, 5.00002, 1 ) )
    moose.delete( '/model' )
    for i in zip( panelC, panelCticks, list(range( len( panelC ))) ):
        plotPos = i[2] + 5
        doty = i[1][-1] * 0.95
        print('doty',  doty )
    print( '[INFO] Run is over')

def test_function_chemsys():
    moose.Neutral( '/library' )
    moose.Neutral( '/model' )
    run()
    print( 'All done' )

def main():
    test_function_chemsys()

if __name__ == '__main__':
    main()
