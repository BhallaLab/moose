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

EREST_ACT = -70e-3
per_ms = 1e3
PI = 3.14159265359
FaradayConst = 96485.3365 # Coulomb/mol


def make_HH_Na(name = 'HH_Na', parent='/library', vmin=-110e-3, vmax=50e-3, vdivs=3000):
    """Create a Hodhkin-Huxley Na channel under `parent`.

    vmin, vmax, vdivs: voltage range and number of divisions for gate tables

    """
    na = moose.HHChannel('%s/%s' % (parent, name))
    na.Ek = 50e-3
    na.Xpower = 3
    na.Ypower = 1
    v = np.linspace(vmin, vmax, vdivs+1) - EREST_ACT
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
    v = np.linspace(vmin, vmax, vdivs+1) - EREST_ACT
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
#                SynChan: GABA receptor
#========================================================================

def make_GABA( name ):
    if moose.exists( '/library/' + name ):
        return
    GABA = moose.SynChan( '/library/' + name )
    GABA.Ek = EK + 10.0e-3
    GABA.tau1 = 4.0e-3
    GABA.tau2 = 9.0e-3
    sh = moose.SimpleSynHandler( GABA.path + '/sh' )
    moose.connect( sh, 'activationOut', GABA, 'activation' )
    sh.numSynapses = 1
    sh.synapse[0].weight = 1


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

def buildCompt( pa, name, RM = 1.0, RA = 1.0, CM = 0.01, dia = 1.0e-6, x = 0.0, y = 0.0, z = 0.0, dx = 10e-6, dy = 0.0, dz = 0.0 ):
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
        EREST_ACT = -0.060 #/* hippocampal cell resting potl */
        ECA = 0.140 + EREST_ACT #// 0.080
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
        soma = buildComptWrapper( elecid, 'soma', dia, dia, 0.0,
            0.33333333, 3000, 0.01 )
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
