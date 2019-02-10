# -*- coding: utf-8 -*-
# rdesignerProtos.py ---
#
# Filename: rdesignerProtos.py
# Description:
# Author: Subhasis Ray, Upi Bhalla
# Maintainer:
# Created: Tue May  7 12:11:22 2013 (+0530)
# Version:
# Last-Updated: Wed Dec 30 13:01:00 2015 (+0530)
#           By: Upi
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
#
#
#

# Change log:
#
#
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
#

# Code:
import numpy as np
import moose
import math
from moose import utils

EREST_ACT    = -0.060
EREST_ACT_HH = -0.070 # A different value is used for the HH squid params
ECA          = 0.080
EK           = -0.075
SOMA_A       = 3.32e-9
per_ms       = 1e3
PI           = 3.14159265359
FaradayConst = 96485.3365 # Coulomb/mol
#CA_SCALE     = 25000 # Ratio of Traub units to mM. 250::0.01
CA_SCALE     = 1.0 # I have now set sensible ranges in the KCA and KAHP



def make_HH_Na(name = 'HH_Na', parent='/library', vmin=-110e-3, vmax=50e-3, vdivs=3000):
    """Create a Hodhkin-Huxley Na channel under `parent`.

    vmin, vmax, vdivs: voltage range and number of divisions for gate tables

    """
    na = moose.HHChannel('%s/%s' % (parent, name))
    na.Ek = 50e-3
    na.Xpower = 3
    na.Ypower = 1
    v = np.linspace(vmin, vmax, vdivs+1) - EREST_ACT_HH
    m_alpha = per_ms * (25 - v * 1e3) / (10 * (np.exp((25 - v * 1e3) / 10) - 1))
    m_beta = per_ms * 4 * np.exp(- v * 1e3/ 18)
    m_gate = moose.element('%s/gateX' % (na.path))
    m_gate.min = vmin
    m_gate.max = vmax
    m_gate.divs = vdivs
    m_gate.tableA = m_alpha
    m_gate.tableB = m_alpha + m_beta
    h_alpha = per_ms * 0.07 * np.exp(-v / 20e-3)
    h_beta = per_ms * 1/(np.exp((30e-3 - v) / 10e-3) + 1)
    h_gate = moose.element('%s/gateY' % (na.path))
    h_gate.min = vmin
    h_gate.max = vmax
    h_gate.divs = vdivs
    h_gate.tableA = h_alpha
    h_gate.tableB = h_alpha + h_beta
    na.tick = -1
    return na

def make_HH_K(name = 'HH_K', parent='/library', vmin=-120e-3, vmax=40e-3, vdivs=3000):
    """Create a Hodhkin-Huxley K channel under `parent`.

    vmin, vmax, vdivs: voltage range and number of divisions for gate tables

    """
    k = moose.HHChannel('%s/%s' % (parent, name))
    k.Ek = -77e-3
    k.Xpower = 4
    v = np.linspace(vmin, vmax, vdivs+1) - EREST_ACT_HH
    n_alpha = per_ms * (10 - v * 1e3)/(100 * (np.exp((10 - v * 1e3)/10) - 1))
    n_beta = per_ms * 0.125 * np.exp(- v * 1e3 / 80)
    n_gate = moose.element('%s/gateX' % (k.path))
    n_gate.min = vmin
    n_gate.max = vmax
    n_gate.divs = vdivs
    n_gate.tableA = n_alpha
    n_gate.tableB = n_alpha + n_beta
    k.tick = -1
    return k

#/========================================================================
#/                Tabchannel Na Hippocampal cell channel
#/========================================================================
def make_Na( name ):
    if moose.exists( '/library/' + name ):
        return
    Na = moose.HHChannel( '/library/' + name )
    Na.Ek = 0.055             #    V
    Na.Gbar = 300 * SOMA_A    #    S
    Na.Gk = 0                 #    S
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

    ygate = moose.element( Na.path + '/gateY' )
    yA = np.array( [ 128.0, 0.0, 0.0, -1.0 * (0.017 + EREST_ACT), 0.018,
        4.0e3, 0.0, 1.0, -1.0 * (0.040 + EREST_ACT), -5.0e-3, 
        3000, -0.1, 0.05 ] )
    ygate.alphaParms = yA
    return Na

#========================================================================
#                Tabchannel K(DR) Hippocampal cell channel
#========================================================================
def make_K_DR( name ):
    if moose.exists( '/library/' + name ):
        return
    K_DR = moose.HHChannel( '/library/' + name )
    K_DR.Ek = EK                #    V
    K_DR.Gbar = 150 * SOMA_A    #    S
    K_DR.Gk = 0                 #    S
    K_DR.Xpower = 1
    K_DR.Ypower = 0
    K_DR.Zpower = 0

    xgate = moose.element( K_DR.path + '/gateX' )
    xA = np.array( [ 16e3 * (0.0351 + EREST_ACT), 
        -16e3, -1.0, -1.0 * (0.0351 + EREST_ACT), -0.005,
        250, 0.0, 0.0, -1.0 * (0.02 + EREST_ACT), 0.04,
        3000, -0.1, 0.05 ] )
    xgate.alphaParms = xA
    return K_DR

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

    ygate = moose.element( K_A.path + '/gateY' )
    yA = np.array( [ 1.6, 0.0, 0.0, 0.013 - EREST_ACT, 0.018,
        50.0, 0.0, 1.0, -1.0 * (0.0101 + EREST_ACT), -0.005,
        3000, -0.1, 0.05 ] )
    ygate.alphaParms = yA

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
            yA[i] = 5.0
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
#             Tabulated Ca-dependent K AHP Channel: Traub 1991
#========================================================================

# This is a tabchannel which gets the calcium concentration from Ca_conc
#  in order to calculate the activation of its Z gate.  It is set up much
#  like the Ca channel, except that the A and B tables have values which are
#  functions of concentration, instead of voltage.
# Traub's original equation is min(0.2e-4 Xi, 0.01) which suggests a max
# of 500 for the Xi (calcium) in his system. For dendritic calcium say 10uM
# or 0.01 mM. Elsewhere we estimate that the ratio of Traub units to mM is
# 250:0.01. Using this ratio, we should have xmax = 0.02.

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
    K_AHP.useConcentration = 1

    zgate = moose.element( K_AHP.path + '/gateZ' )
    xmax = 0.02 # 20 micromolar.
    zgate.min = 0
    zgate.max = xmax
    zgate.divs = 3000
    zA = np.zeros( (zgate.divs + 1), dtype=float)
    zB = np.zeros( (zgate.divs + 1), dtype=float)
    dx = (zgate.max - zgate.min)/zgate.divs
    x = zgate.min
    for i in range( zgate.divs + 1 ):
            zA[i] = min( 250.00 * CA_SCALE * x, 10 )
            zB[i] = 1.0 + zA[i]
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
# concentration range of 0 to 100 uM, with 3000 divisions.  This is done
# using the Z gate, which can receive a CONCEN message.  By using
# the "instant" flag, the A and B tables are evaluated as lookup tables,
#  rather than being used in a differential equation.
    zgate = moose.element( K_C.path + '/gateZ' )
    zgate.min = 0.0
    xmax = 0.02  # Max Ca conc is likely to be 100 uM even in spine, but
        # based on estimates above let's keep it at 20uM.
    zgate.max = xmax
    zgate.divs = 3000
    zA = np.zeros( (zgate.divs + 1), dtype=float)
    zB = np.zeros( (zgate.divs + 1), dtype=float)
    dx = ( zgate.max -  zgate.min)/ zgate.divs
    x = zgate.min
    #CaScale = 100000.0 / 250.0e-3
    for i in range( zgate.divs + 1 ):
        zA[i] = min( 1.0, x * CA_SCALE / xmax)
        zB[i] = 1.0
        x += dx
    zgate.tableA = zA
    zgate.tableB = zB

# Now we need to provide for messages that link to external elements.
# The message that sends the Ca concentration to the Z gate tables is stored
# in an added field of the channel, so that it may be found by the cell
# reader.
    addmsg1 = moose.Mstring( K_C.path + '/addmsg1' )
    addmsg1.value = '../Ca_conc    concOut    . concen'


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
    sh = moose.SimpleSynHandler( glu.path + '/sh' )
    moose.connect( sh, 'activationOut', glu, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1
    return glu

#========================================================================
#                NMDAChan: NMDA receptor
#========================================================================

def make_NMDA( name ):
    if moose.exists( '/library/' + name ):
        return
    NMDA = moose.NMDAChan( '/library/' + name )
    NMDA.Ek = 0.0
    NMDA.tau1 = 20.0e-3
    NMDA.tau2 = 20.0e-3
    NMDA.Gbar = 5 * SOMA_A
    NMDA.CMg = 1.2		#	[Mg]ext in mM
    NMDA.KMg_A = 1.0/0.28
    NMDA.KMg_B = 1.0/62
    NMDA.temperature = 300  # Temperature in Kelvin.
    NMDA.extCa = 1.5        # [Ca]ext in mM
    NMDA.intCa = 0.00008        # [Ca]int in mM
    NMDA.intCaScale = 1         # Scale factor from elec Ca units to mM
    NMDA.intCaOffset = 0.00008  # Basal [Ca]int in mM
    NMDA.condFraction = 0.02  # Fraction of conductance due to Ca

    addmsg1 = moose.Mstring( NMDA.path + '/addmsg1' )
    addmsg1.value = '.	ICaOut ../Ca_conc current'
    addmsg2 = moose.Mstring( NMDA.path + '/addmsg2' )
    addmsg2.value = '../Ca_conc	concOut . assignIntCa'

    sh = moose.SimpleSynHandler( NMDA.path + '/sh' )
    moose.connect( sh, 'activationOut', NMDA, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1
    return NMDA

#========================================================================
#                SynChan: GABA receptor
#========================================================================

def make_GABA( name ):
    if moose.exists( '/library/' + name ):
        return
    GABA = moose.SynChan( '/library/' + name )
    GABA.Ek = -0.065            # V
    GABA.tau1 = 4.0e-3          # s
    GABA.tau2 = 9.0e-3          # s
    sh = moose.SimpleSynHandler( GABA.path + '/sh' )
    moose.connect( sh, 'activationOut', GABA, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1

#========================================================================

def makeChemOscillator( name = 'osc', parent = '/library' ):
    model = moose.Neutral( parent + '/' + name )
    compt = moose.CubeMesh( model.path + '/kinetics' )
    """
    This function sets up a simple oscillatory chemical system within
    the script. The reaction system is::

        s ---a---> a  // s goes to a, catalyzed by a.
        s ---a---> b  // s goes to b, catalyzed by a.
        a ---b---> s  // a goes to s, catalyzed by b.
        b -------> s  // b is degraded irreversibly to s.

    in sum, **a** has a positive feedback onto itself and also forms **b**.
    **b** has a negative feedback onto **a**.
    Finally, the diffusion constant for **a** is 1/10 that of **b**.
    """
    # create container for model
    diffConst = 10e-12 # m^2/sec
    motorRate = 1e-6 # m/sec
    concA = 1 # millimolar

    # create molecules and reactions
    a = moose.Pool( compt.path + '/a' )
    b = moose.Pool( compt.path + '/b' )
    s = moose.Pool( compt.path + '/s' )
    e1 = moose.MMenz( compt.path + '/e1' )
    e2 = moose.MMenz( compt.path + '/e2' )
    e3 = moose.MMenz( compt.path + '/e3' )
    r1 = moose.Reac( compt.path + '/r1' )

    a.concInit = 0.1
    b.concInit = 0.1
    s.concInit = 1

    moose.connect( e1, 'sub', s, 'reac' )
    moose.connect( e1, 'prd', a, 'reac' )
    moose.connect( a, 'nOut', e1, 'enzDest' )
    e1.Km = 1
    e1.kcat = 1

    moose.connect( e2, 'sub', s, 'reac' )
    moose.connect( e2, 'prd', b, 'reac' )
    moose.connect( a, 'nOut', e2, 'enzDest' )
    e2.Km = 1
    e2.kcat = 0.5

    moose.connect( e3, 'sub', a, 'reac' )
    moose.connect( e3, 'prd', s, 'reac' )
    moose.connect( b, 'nOut', e3, 'enzDest' )
    e3.Km = 0.1
    e3.kcat = 1

    moose.connect( r1, 'sub', b, 'reac' )
    moose.connect( r1, 'prd', s, 'reac' )
    r1.Kf = 0.3 # 1/sec
    r1.Kb = 0 # 1/sec

    # Assign parameters
    a.diffConst = diffConst/10
    b.diffConst = diffConst
    s.diffConst = 0
    return compt


    #################################################################
    # Here we have a series of utility functions for building cell
    # prototypes.
    #################################################################
def transformNMDAR( path ):
    for i in moose.wildcardFind( path + "/##/#NMDA#[ISA!=NMDAChan]" ):
        chanpath = i.path
        pa = i.parent
        i.name = '_temp'
        if ( chanpath[-3:] == "[0]" ):
            chanpath = chanpath[:-3]
        nmdar = moose.NMDAChan( chanpath )
        sh = moose.SimpleSynHandler( chanpath + '/sh' )
        moose.connect( sh, 'activationOut', nmdar, 'activation' )
        sh.numSynapses = 1
        sh.synapse[0].weight = 1
        nmdar.Ek = i.Ek
        nmdar.tau1 = i.tau1
        nmdar.tau2 = i.tau2
        nmdar.Gbar = i.Gbar
        nmdar.CMg = 12
        nmdar.KMg_A = 1.0 / 0.28
        nmdar.KMg_B = 1.0 / 62
        nmdar.temperature = 300
        nmdar.extCa = 1.5
        nmdar.intCa = 0.00008
        nmdar.intCaScale = 1
        nmdar.intCaOffset = 0.00008
        nmdar.condFraction = 0.02
        moose.delete( i )
        moose.connect( pa, 'channel', nmdar, 'channel' )
        caconc = moose.wildcardFind( pa.path + '/#[ISA=CaConcBase]' )
        if ( len( caconc ) < 1 ):
            print('no caconcs found on ', pa.path)
        else:
            moose.connect( nmdar, 'ICaOut', caconc[0], 'current' )
            moose.connect( caconc[0], 'concOut', nmdar, 'assignIntCa' )
    ################################################################
    # Utility function for building a compartment, used for spines.
    # Builds a compartment object downstream (further away from soma)
    # of the specfied previous compartment 'pa'. If 'pa' is not a
    # compartment, it builds it on 'pa'. It places the compartment
    # on the end of 'prev', and at 0,0,0 otherwise.

def buildCompt( pa, name, RM = 1.0, RA = 1.0, CM = 0.01, dia = 1.0e-6, x = 0.0, y = 0.0, z = 0.0, dx = 10e-6, dy = 0.0, dz = 0.0, Em = -0.065, initVm = -0.065 ):
    length = np.sqrt( dx * dx + dy * dy + dz * dz )
    compt = moose.Compartment( pa.path + '/' + name )
    compt.x0 = x
    compt.y0 = y
    compt.z0 = z
    compt.x = dx + x
    compt.y = dy + y
    compt.z = dz + z
    compt.diameter = dia
    compt.length = length
    xa = dia * dia * PI / 4.0
    sa = length * dia * PI
    compt.Ra = length * RA / xa
    compt.Rm = RM / sa
    compt.Cm = CM * sa
    compt.Em = Em
    compt.initVm = initVm
    return compt

def buildComptWrapper( pa, name, length, dia, xoffset, RM, RA, CM ):
    return buildCompt( pa, name, RM, RA, CM, dia = dia, x = xoffset, dx = length )

    ################################################################
    # Utility function for building a synapse, used for spines.
def buildSyn( name, compt, Ek, tau1, tau2, Gbar, CM ):
    syn = moose.SynChan( compt.path + '/' + name )
    syn.Ek = Ek
    syn.tau1 = tau1
    syn.tau2 = tau2
    syn.Gbar = Gbar * compt.Cm / CM
    #print "BUILD SYN: ", name, Gbar, syn.Gbar, CM
    moose.connect( compt, 'channel', syn, 'channel' )
    sh = moose.SimpleSynHandler( syn.path + '/sh' )
    moose.connect( sh, 'activationOut', syn, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1
    return syn

######################################################################
# Utility function, borrowed from proto18.py, for making an LCa channel.
# Based on Traub's 91 model, I believe.
def make_LCa( name = 'LCa', parent = '/library' ):
        if moose.exists( parent + '/' + name ):
                return
        Ca = moose.HHChannel( parent + '/' + name )
        Ca.Ek = ECA
        Ca.Gbar = 0
        Ca.Gk = 0
        Ca.Xpower = 2
        Ca.Ypower = 1
        Ca.Zpower = 0

        xgate = moose.element( parent + '/' + name + '/gateX' )
        xA = np.array( [ 1.6e3, 0, 1.0, -1.0 * (0.065 + EREST_ACT), -0.01389, -20e3 * (0.0511 + EREST_ACT), 20e3, -1.0, -1.0 * (0.0511 + EREST_ACT), 5.0e-3, 3000, -0.1, 0.05 ] )
        xgate.alphaParms = xA
        ygate = moose.element( parent + '/' + name + '/gateY' )
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
                        yA[i] = 5.0
                yB[i] = 5.0
                x += dx
        ygate.tableA = yA
        ygate.tableB = yB
        return Ca
######################################################################

def make_vclamp( name = 'Vclamp', parent = '/library' ):
    if moose.exists( '/library/' + name ):
        return
    vclamp = moose.VClamp( parent + '/' + name )
    vclamp.mode = 0     # Default. could try 1, 2 as well
    vclamp.tau = 0.2e-3 # lowpass filter for command voltage input
    vclamp.ti = 20e-6   # Integral time
    vclamp.td = 5e-6    # Differential time. Should it be >= dt?
    vclamp.gain = 0.00005   # Gain of vclamp ckt.

    # Connect voltage clamp circuitry
    addmsg1 = moose.Mstring( vclamp.path + '/addmsg1' )
    addmsg1.value = '.  currentOut  ..  injectMsg'
    addmsg2 = moose.Mstring( vclamp.path + '/addmsg2' )
    addmsg2.value = '.. VmOut . sensedIn'

    return vclamp

######################################################################

def make_synInput( name = 'RandSpike', parent = '/library' ):
    if moose.exists( '/library/' + name ):
        return
    rs = moose.RandSpike( parent + '/' + name + '_rs' )
    rs.rate = 0     # mean firing rate
    rs.refractT = 5e-3 # 5 ms.
    

    # Connect rand spike to channel that it is sitting on.
    addmsg1 = moose.Mstring( rs.path + '/addmsg1' )
    addmsg1.value = '.  spikeOut  ../sh/synapse[0]  addSpike'

    return rs

######################################################################

    ################################################################
    # API function for building spine prototypes. Here we put in the
    # spine dimensions, and options for standard channel types.
    # The synList tells it to create dual alpha function synchans:
    # [name, Erev, tau1, tau2, conductance_density, connectToCa]
    # The chanList tells it to copy over channels defined in /library
    # and assign the specified conductance density.
    # If caTau <= zero then there is no caConc created, otherwise it
    # creates one and assigns the desired tau in seconds.
    # With the default arguments here it will create a glu, NMDA and LCa,
    # and add a Ca_conc.
def addSpineProto( name = 'spine',
        parent = '/library',
        RM = 1.0, RA = 1.0, CM = 0.01,
        shaftLen = 1.e-6 , shaftDia = 0.2e-6,
        headLen = 0.5e-6, headDia = 0.5e-6,
        synList = (),
        chanList = (),
        caTau = 0.0
        ):
    assert( moose.exists( parent ) ), "%s must exist" % parent
    spine = moose.Neutral( parent + '/' + name )
    shaft = buildComptWrapper( spine, 'shaft', shaftLen, shaftDia, 0.0, RM, RA, CM )
    head = buildComptWrapper( spine, 'head', headLen, headDia, shaftLen, RM, RA, CM )
    moose.connect( shaft, 'axial', head, 'raxial' )

    if caTau > 0.0:
        conc = moose.CaConc( head.path + '/Ca_conc' )
        conc.tau = caTau
        conc.length = head.length
        conc.diameter = head.diameter
        conc.thick = 0.0
        # The 'B' field is deprecated.
        # B = 1/(ion_charge * Faraday * volume)
        #vol = head.length * head.diameter * head.diameter * PI / 4.0
        #conc.B = 1.0 / ( 2.0 * FaradayConst * vol )
        conc.Ca_base = 0.0
    for i in synList:
        syn = buildSyn( i[0], head, i[1], i[2], i[3], i[4], CM )
        if i[5] and caTau > 0.0:
            moose.connect( syn, 'IkOut', conc, 'current' )
    for i in chanList:
        if ( moose.exists( parent + '/' + i[0] ) ):
            chan = moose.copy( parent + '/' + i[0], head )
        else:
            moose.setCwe( head )
            chan = make_LCa()
            chan.name = i[0]
            moose.setCwe( '/' )
        chan.Gbar = i[1] * head.Cm / CM
        #print "CHAN = ", chan, chan.tick, chan.Gbar
        moose.connect( head, 'channel', chan, 'channel' )
        if i[2] and caTau > 0.0:
            moose.connect( chan, 'IkOut', conc, 'current' )
    transformNMDAR( parent + '/' + name )
    return spine

#######################################################################
# Here are some compartment related prototyping functions
def makePassiveHHsoma(name = 'passiveHHsoma', parent='/library'):
    ''' Make HH squid model sized compartment:
    len and dia 500 microns. CM = 0.01 F/m^2, RA =
    '''
    elecpath = parent + '/' + name
    if not moose.exists( elecpath ):
        elecid = moose.Neuron( elecpath )
        dia = 500e-6
        soma = buildCompt( elecid, 'soma', dx = dia, dia = dia, x = 0.0,
            RM = 0.33333333, RA = 3000, CM = 0.01 )
        soma.initVm = -65e-3 # Resting of -65, from HH
        soma.Em = -54.4e-3 # 10.6 mV above resting of -65, from HH
    else:
        elecid = moose.element( elecpath )
    return elecid

# Wrapper function. This is used by the proto builder from rdesigneur
def makeActiveSpine(name = 'active_spine', parent='/library'):
    return addSpineProto( name = name, parent = parent,
            synList = ( ['glu', 0.0, 2e-3, 9e-3, 200.0, False],
            ['NMDA', 0.0, 20e-3, 20e-3, 80.0, True] ),
            chanList = ( ['Ca', 10.0, True ], ),
            caTau = 13.333e-3
        )

# Wrapper function. This is used by the proto builder from rdesigneur
def makeExcSpine(name = 'exc_spine', parent='/library'):
    return addSpineProto( name = name, parent = parent,
            synList = ( ['glu', 0.0, 2e-3, 9e-3, 200.0, False],
            ['NMDA', 0.0, 20e-3, 20e-3, 80.0, True] ),
            caTau = 13.333e-3 )

# Wrapper function. This is used by the proto builder from rdesigneur
def makePassiveSpine(name = 'passive_spine', parent='/library'):
    return addSpineProto( name = name, parent = parent)


# legacy function. This is used by the proto builder from rdesigneur
def makeSpineProto( name ):
    addSpineProto( name = name, chanList = () )
