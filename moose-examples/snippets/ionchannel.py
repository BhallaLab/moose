# ionchannel.py ---
#
# Filename: ionchannel.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Wed Sep 17 10:33:20 2014 (+0530)
# Version:
# Last-Updated:
#           By:
#     Update #: 0
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
import matplotlib.pyplot as plt
import moose

EREST_ACT = -70e-3 #: Resting membrane potential


#: The parameters for defining m as a function of Vm
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

#: Parameters for defining h gate of Na+ channel
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

#: K+ channel in Hodgkin-Huxley model has only one gate, n and these
#are the parameters for the same
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

#: We define the rate parameters, which are functions of Vm as
#: interpolation tables looked up by membrane potential.
#: Minimum x-value for the interpolation table
VMIN = -30e-3 + EREST_ACT
#: Maximum x-value for the interpolation table
VMAX = 120e-3 + EREST_ACT
#: Number of divisions in the interpolation table
VDIVS = 3000

def create_na_proto():
    """
    Create and return a Na+ channel prototype '/library/na'

    The Na+ channel conductance has the equation::

        g = Gbar * m^3 * h

    For each gate, we use the HHChannel.setupAlpha function to set up
    the interpolation table.

    """
    lib = moose.Neutral('/library')
    na = moose.HHChannel('/library/na')
    na.tick = -1
    #: The exponent for m gate is 3
    na.Xpower = 3
    #: channel/gateX is the m gate
    #: setting Xpower to a positive number automatically creates this gate.
    xGate = moose.element(na.path + '/gateX')
    xGate.setupAlpha(Na_m_params +
                      [VDIVS, VMIN, VMAX])
    #: channel/gateY is the h gate
    #: Exponent for h gate is 1
    na.Ypower = 1
    yGate = moose.element(na.path + '/gateY')
    yGate.setupAlpha(Na_h_params +
                      [VDIVS, VMIN, VMAX])
    return na

def create_k_proto():
    """
    Create and return a K+ channel prototype '/library/k'.

    The K+ channel conductance has the equation::

        g = Gbar * n^4

    """
    lib = moose.Neutral('/library')
    k = moose.HHChannel('/library/k')
    k.tick = -1
    k.Xpower = 4.0
    xGate = moose.HHGate(k.path + '/gateX')
    xGate.setupAlpha(K_n_params +
                      [VDIVS, VMIN, VMAX])
    return k

def create_1comp_neuron(path, number=1):
    """Create single-compartmental neuron with Na+ and K+ channels.

    Parameters
    ----------
    path : str
        path of the compartment to be created

    number : int
        number of compartments to be created. If n is greater than 1,
        we create a vec with that size, each having the same property.

    Returns
    -------
    comp : moose.Compartment
        a compartment vec with `number` elements.

    """
    comps = moose.vec(path=path, n=number, dtype='Compartment')
    diameter = 30e-6
    length = 50e-6
    sarea = np.pi * diameter * length
    xarea = np.pi * diameter * diameter / 4.0
    Em = EREST_ACT + 10.613e-3
    comps.Em = Em
    comps.initVm = EREST_ACT
    #: CM = 1 uF/cm^2
    comps.Cm = 1e-6 * sarea * 1e4
    #: RM = 0.3 mS/cm^2
    comps.Rm = 1 / (0.3e-3 * sarea * 1e4)
    container = comps[0].parent.path
    #: Here we create copies of the prototype channels
    nachan = moose.copy(create_na_proto(), container, 'na_{}'.format(comps.name), number)
    #: Gbar_Na = 120 mS/cm^2
    nachan.Gbar = [120e-3 * sarea * 1e4] * len(nachan)
    nachan.Ek = 115e-3 + EREST_ACT
    moose.connect(nachan, 'channel', comps, 'channel', 'OneToOne')
    kchan = moose.copy(create_k_proto(), container, 'k_{}'.format(comps.name), number)
    #: Gbar_K = 36 mS/cm^2
    kchan.Gbar = 36e-3 * sarea * 1e4
    kchan.Ek = -12e-3 + EREST_ACT
    moose.connect(kchan, 'channel', comps, 'channel', 'OneToOne')
    return comps

def current_step_test(simtime, simdt, plotdt):
    """
    Create a single compartment and set it up for applying a step
    current injection.

    We use a PulseGen object to generate a 40 ms wide 1 nA current
    pulse that starts 20 ms after start of simulation.

    """
    model = moose.Neutral('/model')
    comp = create_1comp_neuron('/model/neuron')
    stim = moose.PulseGen('/model/stimulus')
    stim.delay[0] = 20e-3
    stim.level[0] = 1e-9
    stim.width[0] = 40e-3
    stim.delay[1] = 1e9
    moose.connect(stim, 'output', comp, 'injectMsg')
    data = moose.Neutral('/data')
    current_tab = moose.Table('/data/current')
    moose.connect(current_tab, 'requestOut', stim, 'getOutputValue')
    vm_tab = moose.Table('/data/Vm')
    moose.connect(vm_tab, 'requestOut', comp, 'getVm')
    for i in range(10):
        moose.setClock(i, simdt)
    moose.setClock(8, plotdt)
    moose.reinit()
    moose.start(simtime)
    ts = np.linspace(0, simtime, len(vm_tab.vector))
    return ts, current_tab.vector, vm_tab.vector,

def main():
    """
This demo shows how to set the parameters for a Hodgkin-Huxley type ion channel.

Hodgkin-Huxley type ion channels are composed of one or more gates
that allow ions to cross the membrane. The gates transition between
open and closed states and this, taken over a large population of
ion channels over a patch of membrane has first order kinetics, where
the rate of change of fraction of open gates (n) is given by::

    dn/dt = alpha(Vm) * (1 - n) - beta(Vm) * n

where alpha and beta are rate parameters for gate opening and
closing respectively that depend on the membrane potential.
The final channel conductance is computed as::

    Gbar * m^x * h^y ...

where m, n are the fraction of open gates of different types and x,
y are the number of such gates in each channel. We can define the
channel by specifying the alpha and beta parameters as functions of
membrane potential and the exponents for each gate.
The number gates is commonly one or two.

Gate opening/closing rates have the form::

    y(x) = (A + B * x) / (C + exp((x + D) / F))

where x is membrane voltage and y is the rate parameter for gate
closing or opening.
    """

    simtime = 0.1
    simdt = 0.25e-5
    plotdt = 0.25e-3
    ts, current, vm = current_step_test(simtime, simdt, plotdt)
    plt.plot(ts, vm * 1e3, label='Vm (mV)')
    plt.plot(ts, current * 1e9, label='current (nA)')
    plt.legend()
    plt.show()

if __name__ == '__main__':
    main()

#
# ionchannel.py ends here
