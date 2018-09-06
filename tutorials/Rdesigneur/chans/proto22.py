#/* FILE INFORMATION
#** Based mostly on the traub91proto.g by Dave Beeman
#** Main difference is addition of Glu and NMDA channels
#** The 1991 Traub set of voltage and concentration dependent channels
#** Implemented as tabchannels by : Dave Beeman
#**      R.D.Traub, R. K. S. Wong, R. Miles, and H. Michelson
#**    Journal of Neurophysiology, Vol. 66, p. 635 (1991)
#**
#** This file depends on functions and constants defined in defaults.g
#** As it is also intended as an example of the use of the tabchannel
#** object to implement concentration dependent channels, it has extensive
#** comments.  Note that the original units used in the paper have been
#** converted to SI (MKS) units.  Also, we define the ionic equilibrium 
#** potentials relative to the resting potential, EREST_ACT.  In the
#** paper, this was defined to be zero.  Here, we use -0.060 volts, the
#** measured value relative to the outside of the cell.
#*/

#/* November 1999 update for GENESIS 2.2: Previous versions of this file used
#   a combination of a table, tabgate, and vdep_channel to implement the
#   Ca-dependent K Channel - K(C).  This new version uses the new tabchannel
#   "instant" field, introduced in GENESIS 2.2, to implement an
#   "instantaneous" gate for the multiplicative Ca-dependent factor in the
#   conductance.   This allows these channels to be used with the fast
#   hsolve chanmodes > 1.
#*/

# Apr 2012 update for pymoose. Converted to equivalent MOOSE funcs.

import moose
import numpy as np
import math

#CONSTANTS
global EK
global SOMA_A
global EREST_ACT
global ENA
global ECA
EREST_ACT = -0.060 #/* hippocampal cell resting potl */
ENA = 0.115 + EREST_ACT #// 0.055
EK = -0.015 + EREST_ACT #// -0.075
ECA = 0.140 + EREST_ACT #// 0.080
SOMA_A = 3.320e-9       #// soma area in square meters
CA_SCALE = 25000        # Ratio of Traub units to mM. 250::0.01
							

#/*
#For these channels, the maximum channel conductance (Gbar) has been
#calculated using the CA3 soma channel conductance densities and soma
#area.  Typically, the functions which create these channels will be used
#to create a library of prototype channels.  When the cell reader creates
#copies of these channels in various compartments, it will set the actual
#value of Gbar by calculating it from the cell parameter file.
#*/

#//========================================================================
#//                      Tabulated Ca Channel
#//========================================================================

def make_Ca( name ):
    if moose.exists( '/library/' + name):
        return
    Ca = moose.HHChannel( '/library/' + name )
    Ca.Ek = ECA
    Ca.Gbar = 40 * SOMA_A
    Ca.Gk = 0
    Ca.Xpower = 2
    Ca.Ypower = 1
    Ca.Zpower = 0

    xgate = moose.element( Ca.path + '/gateX' )
    xA = np.array( [ 1.6e3, 0, 1.0, -1.0 * (0.065 + EREST_ACT), -0.01389, -20e3 * (0.0511 + EREST_ACT), 20e3, -1.0, -1.0 * (0.0511 + EREST_ACT), 5.0e-3, 3000, -0.1, 0.05 ] )
#    xgate.min = -0.1
#    xgate.max = 0.05
#    xgate.divs = 3000
#// Converting Traub's expressions for the gCa/s alpha and beta functions
#// to SI units and entering the A, B, C, D and F parameters, we get:
#    xgate.alpha( 1.6e3, 0, 1.0, -1.0 * (0.065 + EREST_ACT), -0.01389 )
#    xgate.beta( -20e3 * (0.0511 + EREST_ACT), 20e3, -1.0, -1.0 * (0.0511 + EREST_ACT), 5.0e-3 )
    #xgate.setupAlpha( xA )
    xgate.alphaParms = xA


#  The Y gate (gCa/r) is not quite of this form.  For V > EREST_ACT, alpha =
#  5*{exp({-50*(V - EREST_ACT)})}.  Otherwise, alpha = 5.  Over the entire
#  range, alpha + beta = 5.  To create the Y_A and Y_B tables, we use some
#  of the pieces of the setupalpha function.
    ygate = moose.element( Ca.path + '/gateY' )
    ygate.min = -0.1
    ygate.max = 0.05
    ygate.divs = 3000
    yA = np.zeros( (ygate.divs + 1), dtype=float)
    yB = np.zeros( (ygate.divs + 1), dtype=float)


#Fill the Y_A table with alpha values and the Y_B table with (alpha+beta)
    dx = (ygate.max - ygate.min)/ygate.divs
    x = ygate.min
    for i in range( ygate.divs + 1 ):
        if ( x > EREST_ACT):
            yA[i] = 5.0 * math.exp( -50 * (x - EREST_ACT) )
        else:
            yA[i] = 0.0
        #yB[i] = 6.0 - yA[i]
        yB[i] = 5.0
        x += dx
    ygate.tableA = yA
    ygate.tableB = yB
# Tell the cell reader that the current from this channel must be fed into
# the Ca_conc pool of calcium.
    addmsg1 = moose.Mstring( Ca.path + '/addmsg1' )
    addmsg1.value = '.    IkOut    ../Ca_conc    current'
# in some compartments, whe have an NMDA_Ca_conc object to put the current
# into.
    addmsg2 = moose.Mstring( Ca.path + '/addmsg2' )
    addmsg2.value = '.    IkOut    ../NMDA_Ca_conc    current'
        # Here we put in an addmsg command for nernst objects, if any.
    addmsg3 = moose.Mstring( Ca.path + '/addmsg3' )
    addmsg3.value = '../Ca_conc/nernst  Eout    .   setEk'
# As we typically use the cell reader to create copies of these prototype
#elements in one or more compartments, we need some way to be sure that the
#needed messages are established.  Although the cell reader has enough
#information to create the messages which link compartments to their channels
#and to other adjacent compartments, it most be provided with the information
#needed to establish additional messages.  This is done by placing the
#message string in a user-defined field of one of the elements which is
#involved in the message.  The cell reader recognizes the added object names
#"addmsg1", "addmsg2", etc. as indicating that they are to be
#evaluated and used to set up messages.  The paths are relative to the
#element which contains the message string in its added field.  Thus,
#"../Ca_conc" refers to the sibling element Ca_conc and "."
#refers to the Ca element itself.


#/*************************************************************************
#Next, we need an element to take the Calcium current calculated by the Ca
#channel and convert it to the Ca concentration.  The "Ca_concen" object
#solves the equation dC/dt = B*I_Ca - C/tau, and sets Ca = Ca_base + C.  As
#it is easy to make mistakes in units when using this Calcium diffusion
#equation, the units used here merit some discussion.

#With Ca_base = 0, this corresponds to Traub's diffusion equation for
#concentration, except that the sign of the current term here is positive, as
#GENESIS uses the convention that I_Ca is the current flowing INTO the
#compartment through the channel.  In SI units, the concentration is usually
#expressed in moles/m^3 (which equals millimoles/liter), and the units of B
#are chosen so that B = 1/(ion_charge * Faraday * volume). Current is
#expressed in amperes and one Faraday = 96487 coulombs.  However, in this
#case, Traub expresses the concentration in arbitrary units, current in
#microamps and uses tau = 13.33 msec.  If we use the same concentration units,
#but express current in amperes and tau in seconds, our B constant is then
#10^12 times the constant (called "phi") used in the paper.  The actual value
#used will be typically be determined by the cell reader from the cell
#parameter file.  However, for the prototype channel we wlll use Traub's
#corrected value for the soma.  (An error in the paper gives it as 17,402
#rather than 17.402.)  In our units, this will be 17.402e12.

#*************************************************************************/


#//========================================================================
#//                      Ca conc
#//========================================================================

def make_Ca_conc( name ):
    if moose.exists( '/library/' + name ):
        return
    conc = moose.CaConc( '/library/tempName' )
    conc.name = name
    conc.tau = 0.013333  # sec
    conc.B  = 17.402e12 # Curr to conc conversion for soma
    conc.Ca_base = 0.00000

#This Ca_concen element should receive a message from any calcium channels
# with the current going through the channel. Here we have this specified
# in the Ca channel, with the idea that more than one channel might
# contribute Ca ions to this calcium pool. In the original GENESIS file
# this was specified here in make_Ca_conc.

#========================================================================
#             Calcium channel including Nernst potential and calcium pool
#========================================================================
def make_Ca_conc_with_Nernst( name ):
	if moose.exists( '/library/' + name ):
		return
	make_Ca_conc( name )
	Ca_conc = moose.element( '/library/' + name )
	Ca_conc.Ca_base = 0.0001
	nernst = moose.Nernst( '/library/' + name + '/nernst' )
	nernst.Temperature = 300
	nernst.valence = 2
	nernst.Cout = 1.5   # 1.5 mM
	moose.connect( Ca_conc, "concOut", nernst, 'ci' )

	#addmsg1 = moose.Mstring( Ca_conc.path + '/addmsg1' )
	#addmsg1.value = '.  concOut nernst  ci'

	#moose.connect( nernst, "Eout", VGCC, "setEk" )
	#moose.connect( Ca_conc, "concOut", nernst, 'ci' )

#========================================================================
#             Tabulated Ca-dependent K AHP Channel
#========================================================================

# This is a tabchannel which gets the calcium concentration from Ca_conc
#  in order to calculate the activation of its Z gate.  It is set up much
#  like the Ca channel, except that the A and B tables have values which are
#  functions of concentration, instead of voltage.

def make_K_AHP( name ):
    if moose.exists( '/library/' + name ):
        return
    K_AHP = moose.HHChannel( '/library/' + name )
    K_AHP.Ek = EK    #            V
    K_AHP.Gbar = 8 * SOMA_A #    S
    K_AHP.Gk = 0    #    S
    K_AHP.Xpower = 0
    K_AHP.Ypower = 0
    K_AHP.Zpower = 1

    zgate = moose.element( K_AHP.path + '/gateZ' )
    xmax = 500.0
    zgate.min = 0
    zgate.max = xmax
    zgate.divs = 3000
    zA = np.zeros( (zgate.divs + 1), dtype=float)
    zB = np.zeros( (zgate.divs + 1), dtype=float)
    dx = (zgate.max - zgate.min)/zgate.divs
    x = zgate.min
    for i in range( zgate.divs + 1 ):
            zA[i] = min( 0.02 * CA_SCALE * x, 10 )
            zB[i] = 1.0
            x = x + dx

    zgate.tableA = zA
    zgate.tableB = zB
    addmsg1 = moose.Mstring( K_AHP.path + '/addmsg1' )
    addmsg1.value = '../Ca_conc    concOut    . concen'
# Use an added field to tell the cell reader to set up a message from the
# Ca_Conc with concentration info, to the current K_AHP object.


#//========================================================================
#//  Ca-dependent K Channel - K(C) - (vdep_channel with table and tabgate)
#//========================================================================

#The expression for the conductance of the potassium C-current channel has a
#typical voltage and time dependent activation gate, where the time dependence
#arises from the solution of a differential equation containing the rate
#parameters alpha and beta.  It is multiplied by a function of calcium
#concentration that is given explicitly rather than being obtained from a
#differential equation.  Therefore, we need a way to multiply the activation
#by a concentration dependent value which is determined from a lookup table.
#This is accomplished by using the Z gate with the new tabchannel "instant"
#field, introduced in GENESIS 2.2, to implement an "instantaneous" gate for
#the multiplicative Ca-dependent factor in the conductance.

def make_K_C( name ):
    if moose.exists( '/library/' + name ):
        return
    K_C = moose.HHChannel( '/library/' + name )
    K_C.Ek = EK                    #    V
    K_C.Gbar = 100.0 * SOMA_A     #    S
    K_C.Gk = 0                    #    S
    K_C.Xpower = 1
    K_C.Zpower = 1
    K_C.instant = 4                # Flag: 0x100 means Z gate is instant.
    K_C.useConcentration = 1

    # Now make a X-table for the voltage-dependent activation parameter.
    xgate = moose.element( K_C.path + '/gateX' )
    xgate.min = -0.1
    xgate.max = 0.05
    xgate.divs = 3000
    xA = np.zeros( (xgate.divs + 1), dtype=float)
    xB = np.zeros( (xgate.divs + 1), dtype=float)
    dx = (xgate.max - xgate.min)/xgate.divs
    x = xgate.min
    for i in range( xgate.divs + 1 ):
        alpha = 0.0
        beta = 0.0
        if (x < EREST_ACT + 0.05):
            alpha = math.exp( 53.872 * (x - EREST_ACT) - 0.66835 ) / 0.018975
            beta = 2000* (math.exp ( (EREST_ACT + 0.0065 - x)/0.027)) - alpha
        else:
            alpha = 2000 * math.exp( ( EREST_ACT + 0.0065 - x)/0.027 )
            beta = 0.0
        xA[i] = alpha
        xB[i] = alpha + beta
        x = x + dx
    xgate.tableA = xA
    xgate.tableB = xB

# Create a table for the function of concentration, allowing a
# concentration range of 0 to 200, with 3000 divisions.  This is done
# using the Z gate, which can receive a CONCEN message.  By using
# the "instant" flag, the A and B tables are evaluated as lookup tables,
#  rather than being used in a differential equation.
    zgate = moose.element( K_C.path + '/gateZ' )
    zgate.min = 0.0
    xmax = 150.0
    zgate.max = xmax
    zgate.divs = 3000
    zA = np.zeros( (zgate.divs + 1), dtype=float)
    zB = np.zeros( (zgate.divs + 1), dtype=float)
    dx = ( zgate.max -  zgate.min)/ zgate.divs
    x = zgate.min
    #CaScale = 100000.0 / 250.0e-3
    for i in range( zgate.divs + 1 ):
        zA[i] = min( 1000.0, x * CA_SCALE / (250 * xmax ) )
        zB[i] = 1000.0
        x += dx
    zgate.tableA = zA
    zgate.tableB = zB

# Now we need to provide for messages that link to external elements.
# The message that sends the Ca concentration to the Z gate tables is stored
# in an added field of the channel, so that it may be found by the cell
# reader.
    addmsg1 = moose.Mstring( K_C.path + '/addmsg1' )
    addmsg1.value = '../Ca_conc    concOut    . concen'


# The remaining channels are straightforward tabchannel implementations

#/========================================================================
#/                Tabchannel Na Hippocampal cell channel
#/========================================================================
def make_Na( name ):
    if moose.exists( '/library/' + name ):
        return
    Na = moose.HHChannel( '/library/' + name )
    Na.Ek = ENA                #    V
    Na.Gbar = 300 * SOMA_A    #    S
    Na.Gk = 0                #    S
    Na.Xpower = 2
    Na.Ypower = 1
    Na.Zpower = 0

    xgate = moose.element( Na.path + '/gateX' )
    xA = np.array( [ 320e3 * (0.0131 + EREST_ACT),
        -320e3, -1.0, -1.0 * (0.0131 + EREST_ACT), -0.004, 
        -280e3 * (0.0401 + EREST_ACT), 280e3, -1.0, 
        -1.0 * (0.0401 + EREST_ACT), 5.0e-3, 
        3000, -0.1, 0.05 ] )
    xgate.alphaParms = xA


    #xgate.alpha( 320e3 * (0.0131 + EREST_ACT), -320e3, -1.0, -1.0 * (0.0131 + EREST_ACT), -0.004 )
    #xgate.beta( -280e3 * (0.0401 + EREST_ACT), 280e3, -1.0, -1.0 * (0.0401 + EREST_ACT), 5.0e-3 )

    ygate = moose.element( Na.path + '/gateY' )
    yA = np.array( [ 128.0, 0.0, 0.0, -1.0 * (0.017 + EREST_ACT), 0.018,
        4.0e3, 0.0, 1.0, -1.0 * (0.040 + EREST_ACT), -5.0e-3, 
        3000, -0.1, 0.05 ] )
    ygate.alphaParms = yA

    #ygate.alpha( 128.0, 0.0, 0.0, -1.0 * (0.017 + EREST_ACT), 0.018 )
    #ygate.beta( 4.0e3, 0.0, 1.0, -1.0 * (0.040 + EREST_ACT), -5.0e-3 )

#========================================================================
#                Tabchannel K(DR) Hippocampal cell channel
#========================================================================
def make_K_DR( name ):
    if moose.exists( '/library/' + name ):
        return
    K_DR = moose.HHChannel( '/library/' + name )
    K_DR.Ek = EK                #    V
    K_DR.Gbar = 150 * SOMA_A    #    S
    K_DR.Gk = 0                #    S
    K_DR.Xpower = 1
    K_DR.Ypower = 0
    K_DR.Zpower = 0

    xgate = moose.element( K_DR.path + '/gateX' )
    xA = np.array( [ 16e3 * (0.0351 + EREST_ACT), 
        -16e3, -1.0, -1.0 * (0.0351 + EREST_ACT), -0.005,
        250, 0.0, 0.0, -1.0 * (0.02 + EREST_ACT), 0.04,
        3000, -0.1, 0.05 ] )
    xgate.alphaParms = xA
    #xgate.alpha( 16e3 * (0.0351 + EREST_ACT), -16e3, -1.0, -1.0 * (0.0351 + EREST_ACT), -0.005 )
    #xgate.beta( 250, 0.0, 0.0, -1.0 * (0.02 + EREST_ACT), 0.04 )

#========================================================================
#                Tabchannel K(A) Hippocampal cell channel
#========================================================================
def make_K_A( name ):
    if moose.exists( '/library/' + name ):
        return
    K_A = moose.HHChannel( '/library/' + name )
    K_A.Ek = EK                #    V
    K_A.Gbar = 50 * SOMA_A    #    S
    K_A.Gk = 0                #    S
    K_A.Xpower = 1
    K_A.Ypower = 1
    K_A.Zpower = 0

    xgate = moose.element( K_A.path + '/gateX' )
    xA = np.array( [ 20e3 * (0.0131 + EREST_ACT), 
        -20e3, -1.0, -1.0 * (0.0131 + EREST_ACT), -0.01,
        -17.5e3 * (0.0401 + EREST_ACT), 
        17.5e3, -1.0, -1.0 * (0.0401 + EREST_ACT), 0.01,
        3000, -0.1, 0.05 ] )
    xgate.alphaParms = xA
    # xgate.alpha( 20e3 * (0.0131 + EREST_ACT), -20e3, -1.0, -1.0 * (0.0131 + EREST_ACT), -0.01 )
    # xgate.beta( -17.5e3 * (0.0401 + EREST_ACT), 17.5e3, -1.0, -1.0 * (0.0401 + EREST_ACT), 0.01 )

    ygate = moose.element( K_A.path + '/gateY' )
    yA = np.array( [ 1.6, 0.0, 0.0, 0.013 - EREST_ACT, 0.018,
        50.0, 0.0, 1.0, -1.0 * (0.0101 + EREST_ACT), -0.005,
        3000, -0.1, 0.05 ] )
    ygate.alphaParms = yA
    # ygate.alpha( 1.6, 0.0, 0.0, 0.013 - EREST_ACT, 0.018 )
    # ygate.beta( 50.0, 0.0, 1.0, -1.0 * (0.0101 + EREST_ACT), -0.005 )
#========================================================================
#                SynChan: Glu receptor
#========================================================================

def make_glu( name ):
    if moose.exists( '/library/' + name ):
        return
    glu = moose.SynChan( '/library/' + name )
    glu.Ek = 0.0
    glu.tau1 = 2.0e-3
    glu.tau2 = 9.0e-3
    glu.Gbar = 40 * SOMA_A
    sh = moose.SimpleSynHandler( glu.path + '/sh' )
    moose.connect( sh, 'activationOut', glu, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1

#========================================================================
# SynChan: Glu receptor
#========================================================================
def make_GABA( name ):
    if moose.exists( '/library/' + name ):
        return
    GABA = moose.SynChan( '/library/' + name )
    GABA.Ek = EK + 10.0e-3
    GABA.tau1 = 4.0e-3
    GABA.tau2 = 9.0e-3
    GABA.Gbar = 40 * SOMA_A
    sh = moose.SimpleSynHandler( GABA.path + '/sh' )
    moose.connect( sh, 'activationOut', GABA, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1


#========================================================================
#  SynChan: NMDA receptor
#========================================================================
def make_NMDA( name ):
    if moose.exists( '/library/' + name ):
        return
    NMDA = moose.NMDAChan( '/library/' + name )
    NMDA.Ek = 0.0
    NMDA.tau1 = 20.0e-3
    NMDA.tau2 = 20.0e-3
    NMDA.Gbar = 5 * SOMA_A
    NMDA.CMg = 1.2        #    [Mg]ext in mM
    NMDA.KMg_A = 1.0/0.28
    NMDA.KMg_B = 1.0/62
    NMDA.temperature = 300  # Temperature in Kelvin.
    NMDA.extCa = 1.5        # [Ca]ext in mM
    NMDA.intCa = 0.00008        # [Ca]int in mM
    NMDA.intCaScale = 1         # Scale factor from elec Ca units to mM
    NMDA.intCaOffset = 0.00008  # Basal [Ca]int in mM
    NMDA.condFraction = 0.02  # Fraction of conductance due to Ca

    addmsg1 = moose.Mstring( NMDA.path + '/addmsg1' )
    addmsg1.value = '.    ICaOut ../Ca_conc current'
    addmsg2 = moose.Mstring( NMDA.path + '/addmsg2' )
    addmsg2.value = '../Ca_conc    concOut . assignIntCa'

    sh = moose.SimpleSynHandler( NMDA.path + '/sh' )
    moose.connect( sh, 'activationOut', NMDA, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1

#========================================================================
# The Ca_NMDA channel is a subset of the NMDA channel that carries Ca.
# It is identical to above, except that the Ek for Ca is much higher:
# 0.08 V from the consts at the top of this file.
# This is about the reversal potl for 1 uM Ca_in, 2 mM out.
# Also we do not want this channel to contribute to the current,
# which is already accounted for in the main channel. So there is
# no CHANNEL message to the parent compartment.
# I would like to have used the Nernst to do the Ca potential, and
# Synchans now take Ek messages but I haven't yet used this.
#========================================================================

def make_Ca_NMDA( name ):
    if moose.exists( '/library/' + name ):
        return
    Ca_NMDA = moose.NMDAChan( '/library/' + name )
    Ca_NMDA.Ek = 0.0
    Ca_NMDA.tau1 = 20.0e-3
    Ca_NMDA.tau2 = 20.0e-3
    Ca_NMDA.Gbar = 5 * SOMA_A
    Ca_NMDA.CMg = 1.2        #    [Mg]ext in mM
    Ca_NMDA.KMg_A = 1.0/0.28
    Ca_NMDA.KMg_B = 1.0/62
    Ca_NMDA.temperature = 300  # Temperature in Kelvin.
    Ca_NMDA.extCa = 1.5        # [Ca]ext in mM
    Ca_NMDA.intCa = 0.00008        # [Ca]int in mM
    Ca_NMDA.intCaScale = 1         # Scale factor from elec Ca units to mM
    Ca_NMDA.intCaOffset = 0.00008  # Basal [Ca]int in mM
    Ca_NMDA.condFraction = 0.02  # Fraction of conductance due to Ca

    addmsg1 = moose.Mstring( Ca_NMDA.path + '/addmsg1' )
    addmsg1.value = '.    ICaOut ../Ca_conc current'
    addmsg2 = moose.Mstring( Ca_NMDA.path + '/addmsg2' )
    addmsg2.value = '../Ca_conc    concOut . assignIntCa'

    sh = moose.SimpleSynHandler( Ca_NMDA.path + '/sh' )
    moose.connect( sh, 'activationOut', Ca_NMDA, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1



    '''
    if moose.exists( 'Ca_NMDA' ):
        return
    Ca_NMDA = moose.SynChan( 'Ca_NMDA' )
    Ca_NMDA.Ek = ECA
    Ca_NMDA.tau1 = 20.0e-3
    Ca_NMDA.tau2 = 20.0e-3
    Ca_NMDA.Gbar = 5 * SOMA_A

    block = moose.MgBlock( '/library/Ca_NMDA/block' )
    block.CMg = 1.2        #    [Mg] in mM
    block.Zk = 2
    block.KMg_A = 1.0/0.28
    block.KMg_B = 1.0/62

    moose.connect( Ca_NMDA, 'channelOut', block, 'origChannel', 'OneToOne' )
    addmsg1 = moose.Mstring( '/library/Ca_NMDA/addmsg1' )
    addmsg1.value = '.. VmOut    ./block    Vm'
    addmsg2 = moose.Mstring( '/library/Ca_NMDA/addmsg2' )
    addmsg2.value = './block    IkOut ../NMDA_Ca_conc current'
    # The original model has the Ca current also coming here.

    sh = moose.SimpleSynHandler( 'Ca_NMDA/sh' )
    moose.connect( sh, 'activationOut', Ca_NMDA, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1
    '''

#=====================================================================
#  SPIKE DETECTOR
#=====================================================================

#//addmsg axon/spike axon BUFFER name
def make_axon( name ):
    if moose.exists( '/library/' + name ):
        return
    axon = moose.SpikeGen( '/library/' + name )
    axon.threshold = -40e-3             #    V
    axon.abs_refract = 10e-3            #     sec

