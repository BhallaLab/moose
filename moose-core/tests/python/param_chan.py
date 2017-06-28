# -*- coding: utf-8 -*-
from chan_proto import (ChannelSettings, ZChannelParams,ChannelParams,GateParams)

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

CaL12param = ChannelSettings(Xpow=1, Ypow=0, Zpow=0, Erev=0.048, name='CaL12')
qfactCaL = 1
CaL12_X_params = GateParams(A_rate=-880,A_B= -220e3,A_C = -1.0,Avhalf= 4.0003e-3, A_vslope=-7.5e-3,B_rate=-284,B_B=71e3,B_C=-1.0,Bvhalf=-4.0003e-3,B_vslope=5e-3)

SKparam = ChannelSettings(Xpow=0, Ypow=0, Zpow=1, Erev=-.087, name='SKCa')

SK_Z_params = ZChannelParams(Kd = 0.57e-3,
                             power = 5.2,
                             tau = 4.9e-3)


Cal = ChannelParams(channel=CaL12param,X=CaL12_X_params,Y=[],Z=[])
SK = ChannelParams(channel=SKparam,X=[],Y=[],Z=SK_Z_params)
