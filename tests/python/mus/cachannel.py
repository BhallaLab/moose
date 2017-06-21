# cachannel.py --- 
# 
# Filename: cachannel.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Jun  4 14:59:08 2014 (+0530)
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

import moose
from settings import *

lib = moose.Neutral('/library')

def make_cat():
    """
    Ported to pymoose. - Subhasis Ray. Wed Jun  4 15:25:21 IST 2014
    
    Original comment:
    
    COMMENT
    T-type Ca channel 
    ca.mod to lead to thalamic ca current inspired by destexhe and huguenrd
    Uses fixed eca instead of GHK eqn
    changed from (AS Oct0899)
    changed for use with Ri18  (B.Kampa 2005)
    ENDCOMMENT
    """
    # gbar = 8.0   # (S/m2)	: 0.12 mho/cm2
    vshift = 0	 # (mV)		: voltage shift (affects all)
    cao  = 2.5	 # (mM)	        : external ca concentration
    vmin = -120	 # (mV)
    vmax = 100	 # (mV)
    v12m = 50         # 	(mV)
    v12h = 78         # 	(mV)
    vwm = 7.4        #  	(mV)
    vwh = 5.0         # 	(mV)
    am = 3         	# (mV)
    ah = 85         	# (mV)
    vm1 = 25         	# (mV)
    vm2 = 100         # 	(mV)
    vh1 = 46         	# (mV)
    vh2 = 405         # 	(mV)
    wm1 = 20         	# (mV)
    wm2 = 15         	# (mV)
    wh1 = 4         	# (mV)
    wh2 = 50         	# (mV)
    v = np.linspace(vmin, vmax, 3000) + vshift
    minf = 1.0 / ( 1 + np.exp(- (v + v12m) / vwm))
    hinf = 1.0 / ( 1 + np.exp((v + v12h) / vwh))
    mtau = 1e-3 * (am + 1.0 / ( np.exp((v + vm1) / wm1) + np.exp( - (v + vm2) / wm2))) # second
    htau = 1e-3 * (ah + 1.0 / ( np.exp((v + vh1) / wh1) + np.exp( - (v + vh2) / wh2))) # second
    channel = moose.HHChannel('%s/CaT' % (lib.path))
    channel.Gbar = 0.0
    channel.Xpower = 2
    channel.Ypower = 1
    channel.gateX[0].tableA = minf / mtau
    channel.gateX[0].tableB = 1 / mtau
    channel.gateX[0].min = vmin * 1e-3
    channel.gateX[0].max = vmax * 1e-3
    channel.gateY[0].tableA = hinf / htau
    channel.gateY[0].tableB = 1 / htau
    channel.gateY[0].max = vmax * 1e-3
    channel.Ek = 140e-3
    return channel

def make_cahva():
    """Ported from Neuron mod file. - Subhasis Ray, Thu Jun  5 11:35:46 IST 2014

    COMMENT
    26 Ago 2002 Modification of original channel to allow variable time step and to correct an initialization error.
    Done by Michael Hines(michael.hines@yale.e) and Ruggero Scorcioni(rscorcio@gmu.edu) at EU Advance Course in Computational Neuroscience. Obidos, Portugal
    
    ca.mod
    Uses fixed eca instead of GHK eqn
    
    HVA Ca current
    Based on Reuveni, Friedman, Amitai and Gutnick (1993) J. Neurosci. 13:
    4609-4621.
    
    Author: Zach Mainen, Salk Institute, 1994, zach@salk.edu

    ENDCOMMENT
    """
    gbar = 0.1   	# (pS/um2)	: 0.12 mho/cm2
    vshift = 0	# (mV)		: voltage shift (affects all)
    cao  = 2.5	# (mM)	        : external ca concentration
    vmin = -120	# (mV)
    vmax = 100	# (mV)
    vm = np.linspace(vmin, vmax, 3000) + vshift
    a = 0.055*(-27 - vm)/(np.exp((-27-vm)/3.8) - 1)
    b = 0.94*np.exp((-75-vm)/17)
    mtau = 1/tadj/(a+b)
    minf = a/(a+b)
    # "h" inactivation 
    a = 0.000457*np.exp((-13-vm)/50)
    b = 0.0065/(np.exp((-vm-15)/28) + 1)
    htau = 1/tadj/(a+b)
    hinf = a/(a+b)
    channel = moose.HHChannel('/library/Ca')
    channel.Xpower = 2
    channel.gateX[0].tableA = 1e3 * minf / mtau # mtau: ms to s
    channel.gateX[0].tableB = 1e3 / mtau
    channel.Ypower = 1
    channel.gateY[0].tableA = 1e3 * hinf / htau # htau: ms to s
    channel.gateY[0].tableB = 1e3 / htau
    channel.gateX[0].min = vmin * 1e-3
    channel.gateX[0].max = vmax * 1e-3
    channel.gateY[0].min = vmin * 1e-3
    channel.gateY[0].max = vmax * 1e-3
    channel.Gbar = 0.0
    channel.Ek = 140e-3
    return channel


# 
# cachannel.py ends here
