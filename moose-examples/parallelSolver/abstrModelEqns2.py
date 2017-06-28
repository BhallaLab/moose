import re
import moose
# Equations here are:
# Adot = 1 -6A + 5A^2 - A^3, or spread out as:
# Adot = k0a + k1a.A + k2a.A.A + k3a.A.A.A + k4a.Ca.A/(1+A+10*B) - k5a.A.B
# Bdot = k1b.A - k2b.B
#


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

    print(Adot.expr)
    print(Bdot.expr)
    print(CaStim.expr)

    # Connections
    Adot.x.num = 4
    moose.connect( Ca, 'nOut', Adot.x[0], 'input' )
    moose.connect( A, 'nOut', Adot.x[1], 'input' )
    moose.connect( B, 'nOut', Adot.x[2], 'input' )
    moose.connect( Z, 'nOut', Adot.x[3], 'input' )
    moose.connect( Adot, 'valueOut', A, 'increment' )

    Bdot.x.num = 3
    if name == 'negFF':
        moose.connect( Ca, 'nOut', Bdot.x[0], 'input' )
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


def makeBis():
    params = {
        'k0a':0.1,  # Constant
        'k1a':-5.0,  # Coeff for A
        'k2a':5.0,  # Coeff for A^2
        'k3a':-1.0,  # Coeff for A^3
        'k4a':10.0,  # turnon of A by A and Ca
        'k5a':-5.0,  # Turnoff of A by B
        'k1b':0.1,  # turnon of B by A
        'k2b':-0.01,   # Decay rate of B
        'diffusionLength':1.0e-6,  # Diffusion characteristic length, used as voxel length too.
        'dendDiameter': 10e-6,  # Diameter of section of dendrite in model
        'dendLength': 100e-6,   # Length of section of dendrite in model
        'diffConstA':1.0,       # Diffusion constant of A
        'diffConstB':1.0,       # Diffusion constant of B
        'stimWidth' :5.0,        # Stimulus width in seconds
        'stimAmplitude':1.0,    # Stimulus amplitude, arb units. From FHN review
        'blankVoxelsAtEnd':10,  # of voxels to leave blank at end of cylinder
        'preStimTime':40.0,     # Time to run before turning on stimulus.
        'postStimTime':80.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus. 
                              # To include in analysis of total response over 
                              # entire dendrite.
        'fnumber':1,          # Number to append to fname
    }

    makeChemProto( 'bis', 
        'k0a + k1a*A + k2a*A*A + k3a*A*A*A + k4a*Ca*A/(1+A+10*B) + k5a*A*B',
        'k1b*A*A + k2b*B',
        params )
    return params

def makeFHN():
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
        'preStimTime':30.0,     # Time to run before turning on stimulus.
        'postStimTime':40.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus. 
                              # To include in analysis of total response over 
                              # entire dendrite.
        'fnumber': 1,         # Number to append to fname
    }
    makeChemProto( 'fhn', 
        '5.0*(A - kxa - (A-kxa)^3/3 - (B-kxb) + Ca)',
        '(A-kxa + k_a - k_b*(B-kxb))/k_t',
        params )
    return params


def makeNegFB():
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
        'postStimTime':60.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus. 
                              # To include in analysis of total response over 
                              # entire dendrite.
        'fnumber':1,          # Number to append to fname
    }
    makeChemProto( 'negFB', 
        'k1a*A + k2a*A*B + k3a*Ca',
        'k1b*A + k2b*B',
        params )
    return params


def makeNegFF():
    params = {
        'k1a':-0.1,  # Coeff for decay of A, slow.
        'k2a':-1.0,  # Coeff for turnoff of A by B, medium.
        'k3a':10.0,  # turnon of A by Ca, fast.
        'k1b':2.0,  # turnon of B by Ca, medium
        'k2b':-0.1,   # Decay rate of B, slow
        'diffusionLength':1.0e-6,  # Diffusion characteristic length, used as voxel length too.
        'dendDiameter': 10e-6,  # Diameter of section of dendrite in model
        'dendLength': 100e-6,   # Length of section of dendrite in model
        'diffConstA':1.0,      # Diffusion constant of A
        'diffConstB':2.0,       # Diffusion constant of B
        'stimWidth' :1.0,        # Stimulus width in seconds
        'stimAmplitude':1.0,    # Stimulus amplitude, arb units. From FHN review
        'blankVoxelsAtEnd':10,  # of voxels to leave blank at end of cylinder
        'preStimTime':10.0,     # Time to run before turning on stimulus.
        'postStimTime':60.0,    # Time to run after stimulus. ~3x decay time
        'settleTime':20.0,    # Settling time of response, after stimulus. 
                              # To include in analysis of total response over 
                              # entire dendrite.
        'fnumber':1,          # Number to append to fname
    }
    makeChemProto( 'negFF', 
        'k1a*A + k2a*A*B + k3a*Ca',
        'k1b*Ca + k2b*B',
        params )

    return params

if __name__ == '__main__':
    moose.Neutral( '/library' )
    print("Making Bistable model")
    makeBis()
    print("Making FHN model")
    makeFHN()
    print("Making Negative Feedback model")
    makeNegFB()
    print("Making Negative Feedforward model")
    makeNegFF()


