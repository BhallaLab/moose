# neuronFromDotp.py ---
# Author: Upi Bhalla NCBS Bangalore 2014.
# Commentary:

# Demonstrates how to load a simple neuronal model in GENESIS dotp format.
# The model has branches and a few spines.
# It is adorned just with classic HH squid channels.
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
import math
import pylab
import numpy
import moose

EREST_ACT = -70e-3

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

def makeChannelPrototypes():
    """Create channel prototypes for readcell."""
    library = moose.Neutral( '/library' )
    moose.setCwe( '/library' )
    compt = moose.SymCompartment( '/library/symcompartment' )
    Em = EREST_ACT + 10.613e-3
    compt.Em = Em
    compt.initVm = EREST_ACT
    compt.Cm = 7.85e-9 * 0.5
    compt.Rm = 4.2e5 * 5.0
    compt.Ra = 7639.44e3
    nachan = moose.HHChannel( '/library/Na' )
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

    kchan = moose.HHChannel( '/library/K' )
    kchan.Xpower = 4.0
    xGate = moose.HHGate(kchan.path + '/gateX')
    xGate.setupAlpha(K_n_params + [VDIVS, VMIN, VMAX])
    xGate.useInterpolation = 1
    kchan.Gbar = 0.2836e-3
    kchan.Ek = -12e-3+EREST_ACT

def addPlot( objpath, field, plot ):
    assert moose.exists( objpath )
    tab = moose.Table( '/graphs/' + plot )
    obj = moose.element( objpath )
    moose.connect( tab, 'requestOut', obj, field )
    return tab

def dumpPlots():
    plots = moose.wildcardFind( '/graphs/##[ISA=Table]' )
    for x in moose.wildcardFind( '/graphs/##[ISA=Table]' ):
        t = numpy.arange( 0, x.size ) * x.dt # msec
        pylab.plot( t, x.vector, label=x.name)

    pylab.legend()
    pylab.show()

def makeModel():
    makeChannelPrototypes()
    cellId = moose.loadModel( 'dotp.p', '/model', 'Neutral' )
    moose.element( '/model/soma' ).inject = 1.7e-9
    graphs = moose.Neutral( '/graphs' )
    addPlot( '/model/soma', 'getVm', 'somaVm' )
    addPlot( '/model/apical_14', 'getVm', 'midVm' )
    addPlot( '/model/lat_15_2', 'getVm', 'latVm' )
    addPlot( '/model/apical_19', 'getVm', 'tipVm' )

def testModel( useSolver ):
    plotDt = 2e-4
    if ( useSolver ):
            elecDt = 50e-6
            chemDt = 2e-3

    makeModel()
    moose.setClock( 18, plotDt )

    moose.reinit()
    moose.start( 0.1 )
    dumpPlots()

def main():
    """
    Demonstrates how to load a simple neuronal model in GENESIS dotp format.
    The model has branches and a few spines.\n
    It is adorned just with classic HH squid channels.\n

    Gate equations have the form::

        y(x) = (A + B * x) / (C + exp((x + D) / F))

    where x is membrane voltage and y is the rate constant for gate closing or opening.

    """
    testModel( 1 )

if __name__ == '__main__':
    main()

#
# neuronFromDotp.py ends here
