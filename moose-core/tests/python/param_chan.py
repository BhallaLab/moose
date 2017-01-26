# -*- coding: utf-8 -*-

from util import NamedDict
from chan_proto import (
    SSTauChannelParams,
    AlphaBetaChannelParams,
    ZChannelParams,
    ChannelSettings,
    TypicalOneDalpha,)

#chanDictSP.py
#contains all gating parameters and reversal potentials
# Gate equations have the form:
#
# y(x) = (rate + B * x) / (C + exp((x + vhalf) / vslope))
#
# OR
# y(x) = tau_min + tau_vdep / (1 + exp((x + vhalf) / vslope))
#
# where x is membrane voltage and y is the rate constant
#KDr params used by Sriram, RE paper1, Krp params used by RE paper 2
#Parameters for Ca channels may need to be shifted - see Dorman model
krev=-87e-3
narev=50e-3
carev=140e-3 #assumes CaExt=2 mM and CaIn=50e-3
ZpowCDI=0

VMIN = -100e-3
VMAX = 50e-3
VDIVS = 3000 #0.5 mV steps
CAMIN=0.01e-3   #10 nM
CAMAX=40e-3  #40 uM, might want to go up to 100 uM with spines
CADIVS=3000 #10 nM steps

#mtau: Ogata fig 5, no qfactor accounted in mtau, 1.2 will improve spike shape
#activation minf fits Ogata 1990 figure 3C (which is cubed root)
#inactivation hinf fits Ogata 1990 figure 6B
#htau fits the main -50 through -10 slope of Ogata figure 9 (log tau), but a qfact of 2 is already taken into account.


CaTparam = ChannelSettings(Xpow=3, Ypow=1, Zpow=1, Erev=carev, name='CaT')
qfactCaT = 1

#Original inactivation ws too slow compared to activation, made closder the alpha1G
CaT_X_params = AlphaBetaChannelParams(A_rate = 5342.4*qfactCaT,
                                      A_B = 2100.*qfactCaT,
                                      A_C = 11.9,
                                      Avhalf = 1e-3,
                                      A_vslope = -12e-3,
                                      B_rate = 289.7*qfactCaT,
                                      B_B = 0.*qfactCaT,
                                      B_C = 1.,
                                      Bvhalf = 0.0969,
                                      B_vslope = 0.0141)
#CaT_Y_params = CaT_X_params
CaT_Y_params = AlphaBetaChannelParams(A_rate = 0*qfactCaT,
                                      A_B = -74.*qfactCaT,
                                      A_C = 1.,
                                      Avhalf = 9e-2,
                                      A_vslope = 5e-3,
                                      B_rate = 15*qfactCaT,
                                      B_B = -1.5*qfactCaT,
                                      B_C = 1.5,
                                      Bvhalf = 0.05,
                                      B_vslope = -15e-3)

#These CDI params can be used with every channel, make ZpowCDI=2
#If ZpowCDI=0 the CDI will not be used, power=-4 is to transform
#(Ca/Kd)^pow/(1+(Ca/Kd)^pow) to 1/(1+(ca/Kd)^-pow)
CDI_Z_params = ZChannelParams(Kd = 0.12e-3,
                              power = -4,
                              tau = 142e-3)

#Dictionary of "standard" channels, to create channels using a loop
#NaF doesn't fit since it uses different prototype form
#will need separate dictionary for BK

ChanDict = NamedDict(
    'ChannelParams',
    CaT =   TypicalOneDalpha(CaTparam, CaT_X_params, CaT_Y_params, CDI_Z_params, calciumPermeable=True))
