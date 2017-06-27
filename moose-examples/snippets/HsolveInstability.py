# HsolveInstability.py ---

# Commentary:
#
# A toy compartmental neuronal + chemical model that causes bad things
# to happen to the hsolver, as of 28 May 2013. Hopefully this will
# become irrelevant soon.
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

#import sys
#sys.path.append('../../python')
#import os
#os.environ['NUMPTHREADS'] = '1'
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

def create_na_proto():
    lib = moose.Neutral('/library')
    na = moose.HHChannel('/library/na')
    na.Xpower = 3
    xGate = moose.HHGate(na.path + '/gateX')
    xGate.setupAlpha(Na_m_params +
                      [VDIVS, VMIN, VMAX])
    na.Ypower = 1
    yGate = moose.HHGate(na.path + '/gateY')
    yGate.setupAlpha(Na_h_params +
                      [VDIVS, VMIN, VMAX])
    return na

def create_k_proto():
    lib = moose.Neutral('/library')
    k = moose.HHChannel('/library/k')
    k.Xpower = 4.0
    xGate = moose.HHGate(k.path + '/gateX')
    xGate.setupAlpha(K_n_params +
                      [VDIVS, VMIN, VMAX])
    return k

def create_squid():
    """Create a single compartment squid model."""
    parent = moose.Neutral ('/n' )
    compt = moose.Compartment( '/n/compt' )
    Em = EREST_ACT + 10.613e-3
    compt.Em = Em
    compt.initVm = EREST_ACT
    compt.Cm = 7.85e-9
    compt.Rm = 4.2e5
    compt.Ra = 7639.44e3
    nachan = moose.HHChannel( '/n/compt/Na' )
    nachan.Xpower = 3
    xGate = moose.HHGate(nachan.path + '/gateX')
    xGate.setupAlpha(Na_m_params +
                      [VDIVS, VMIN, VMAX])
    nachan.Ypower = 1
    yGate = moose.HHGate(nachan.path + '/gateY')
    yGate.setupAlpha(Na_h_params +
                      [VDIVS, VMIN, VMAX])
    nachan.Gbar = 0.942e-3
    nachan.Ek = 115e-3+EREST_ACT
    moose.connect(nachan, 'channel', compt, 'channel', 'OneToOne')

    kchan = moose.HHChannel( '/n/compt/K' )
    kchan.Xpower = 4.0
    xGate = moose.HHGate(kchan.path + '/gateX')
    xGate.setupAlpha(K_n_params +
                      [VDIVS, VMIN, VMAX])
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
    shaft = moose.Compartment( parentObj.path + '/' + sname )
    moose.connect( parentCompt, 'raxial', shaft, 'axial', 'single' )
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
    shaft.diameter = dia / 10.0
    shaft.length = length
    xa = math.pi * dia * dia / 400.0
    circumference = math.pi * dia / 10.0
    shaft.Ra = RA * length / xa
    shaft.Rm = RM / ( length * circumference )
    shaft.Cm = CM * length * circumference
    shaft.Em = EREST_ACT
    shaft.initVm = EREST_ACT

    head = moose.Compartment( parentObj.path + '/' + hname )
    moose.connect( shaft, 'raxial', head, 'axial', 'single' )
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
    FaradayConst = 96485.3415                        # s A / mol
    spineLength = 5.0e-6
    spineDia = 4.0e-6
    head = create_spine( compt, cell, index, frac, spineLength, spineDia, 0.0 )
    gluR = moose.SynChan( head.path + '/gluR' )
    gluR.tau1 = 1e-3
    gluR.tau2 = 1e-3
    gluR.Gbar = 1e-5
    gluR.Ek= 10.0e-3
    moose.connect( head, 'channel', gluR, 'channel', 'single' )

    caPool = moose.CaConc( head.path + '/ca' )
    caPool.CaBasal = 1e-4                 # 0.1 micromolar
    caPool.tau = 0.01
    B = 1.0 / ( FaradayConst * spineLength * spineDia * spineDia *math.pi/4)
    B = B / 20.0                                 # scaling factor for Ca buffering
    caPool.B = B
    moose.connect( gluR, 'IkOut', caPool, 'current', 'single' )

    synh = moose.SimpleSynHandler( gluR.path + '/synh' )
    moose.connect( synh, "activationOut", gluR, "activation" )
    return synh

def add_plot( objpath, field, plot ):
    if moose.exists( objpath ):
        tab = moose.Table( '/graphs/' + plot )
        obj = moose.element( objpath )
        moose.connect( tab, 'requestOut', obj, field )
        return tab

def make_plots():
    graphs = moose.Neutral( '/graphs' )
    add_plot( '/n/compt', 'getVm', 'dendVm' )
    add_plot( '/n/head2', 'getVm', 'head2Vm' )
    add_plot( '/n/head2/ca', 'getCa', 'head2Ca' )

def display_plots( name ):
    pylab.figure()
    for x in moose.wildcardFind( '/graphs/#' ):
        pos = numpy.arange( 0, x.vector.size ) * x.dt
        print((len( pos ), len( x.vector )))
        pylab.plot( pos, x.vector, label=x.name )
    pylab.legend()
    pylab.title( name )

def make_spiny_compt():
    comptLength = 100e-6
    comptDia = 4e-6
    numSpines = 5
    compt = create_squid()
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
        r = create_spine_with_receptor( compt, cell, i, i/float(numSpines) )
        r.synapse.num = 1
        syn = moose.element( r.path + '/synapse' )
        moose.connect( synInput, 'spikeOut', syn, 'addSpike', 'single' )
        syn.weight = 0.2
        syn.delay = i * 1.0e-4

def main():
    """
    A toy compartmental neuronal + chemical model that causes bad things
    to happen to the hsolver, as of 28 May 2013.
    Hopefully this will become irrelevant soon

    """
    fineDt = 1e-5
    coarseDt = 5e-5
    make_spiny_compt()
    make_plots()
    for i in range( 8 ):
        moose.setClock( i, fineDt )
    moose.setClock( 8, coarseDt )
    moose.reinit()
    moose.start( 0.1 )
    display_plots( 'instab.plot' )
    # make Hsolver and rerun
    hsolve = moose.HSolve( '/n/hsolve' )
    for i in range( 8 ):
        moose.setClock( i, coarseDt )
    hsolve.dt = coarseDt
    hsolve.target = '/n/compt'
    moose.reinit()
    moose.start( 0.1 )
    display_plots( 'h_instab.plot' )
    pylab.show()

if __name__ == '__main__':
    main()

#
# HsolveInstability.py ends here
