# -*- coding: utf-8 -*-
"""\
Create general Channel Proto, pass in name, x and y power, and params

Also, create the library of channels
Might need a few other chan_proto types, such as
     inf-tau channels
     Ca dep channels
chan_proto quires alpha and beta params for both activation and inactivation
If no inactivation, just send in empty Yparam array.
"""

from __future__ import print_function, division
import moose
import numpy as np
from collections import namedtuple

ChannelSettings = namedtuple('''ChannelSettings''', '''
Xpow
Ypow
Zpow
Erev
name''')
ZChannelParams = namedtuple('''ZChannelParams''',
                           '''Kd
                           power
                           tau''')

ChannelParams = namedtuple('''ChannelParams''', '''channel
X
Y
Z
''')

GateParams = namedtuple('''GateParams''', '''
A_rate
A_B
A_C
Avhalf
A_vslope
B_rate
B_B
B_C
Bvhalf
B_vslope''')


def interpolate_values_in_table(tabA,V_0,l=40):
    import param_chan
    '''This function interpolates values in the table
    around tabA[V_0]. '''
    V = np.linspace(param_chan.VMIN, param_chan.VMAX, len(tabA))
    idx =  abs(V-V_0).argmin()
    A_min = tabA[idx-l]
    V_min = V[idx-l]
    A_max = tabA[idx+l]
    V_max = V[idx+l]
    a = (A_max-A_min)/(V_max-V_min)
    b = A_max - a*V_max
    tabA[idx-l:idx+l] = V[idx-l:idx+l]*a+b
    return tabA

def fix_singularities(Params,Gate):
    import param_chan

    if Params.A_C < 0:

        V_0 = Params.A_vslope*np.log(-Params.A_C)-Params.Avhalf

        if V_0 > param_chan.VMIN and V_0 < param_chan.VMAX:
            #change values in tableA and tableB, because tableB contains sum of alpha and beta
            tabA = interpolate_values_in_table(Gate.tableA,V_0)
            tabB = interpolate_values_in_table(Gate.tableB,V_0)
            Gate.tableA = tabA
            Gate.tableB = tabB

    if Params.B_C < 0:

        V_0 = Params.B_vslope*np.log(-Params.B_C)-Params.Bvhalf

        if V_0 > param_chan.VMIN and V_0 < param_chan.VMAX:
            #change values in tableB
            tabB = interpolate_values_in_table(Gate.tableB,V_0)
            Gate.tableB = tabB

    return Gate

#may need a CaV channel if X gate uses alpha,beta and Ygate uses inf tau
#Or, have Y form an option - if in tau, do something like NaF
def chan_proto(chanpath, params):
    import param_chan

    chan = moose.HHChannel(chanpath)
    chan.Xpower = params.channel.Xpow
    if params.channel.Xpow > 0:
        xGate = moose.HHGate(chan.path + '/gateX')
        xGate.setupAlpha(params.X + (param_chan.VDIVS, param_chan.VMIN, param_chan.VMAX))
        xGate = fix_singularities(params.X, xGate)

    chan.Ypower = params.channel.Ypow
    if params.channel.Ypow > 0:
        yGate = moose.HHGate(chan.path + '/gateY')
        yGate.setupAlpha(params.Y + (param_chan.VDIVS, param_chan.VMIN, param_chan.VMAX))
        yGate = fix_singularities(params.Y, yGate)
    if params.channel.Zpow > 0:
        chan.Zpower = params.channel.Zpow
        zgate = moose.HHGate(chan.path + '/gateZ')
        ca_array = np.linspace(param_chan.CAMIN, param_chan.CAMAX, param_chan.CADIVS)
        zgate.min = param_chan.CAMIN
        zgate.max = param_chan.CAMAX
        caterm = (ca_array/params.Z.Kd) ** params.Z.power
        inf_z = caterm / (1 + caterm)
        tau_z = params.Z.tau * np.ones(len(ca_array))
        zgate.tableA = inf_z / tau_z
        zgate.tableB = 1 / tau_z
        chan.useConcentration = True
    chan.Ek = params.channel.Erev
    return chan


