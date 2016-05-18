# nachannel.py --- 
# 
# Filename: nachannel.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Tue Jun  3 20:48:17 2014 (+0530)
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

import moose
from settings import *
import numpy as np

"""Converted from the neuron modl file by Zach Mainen 1994.
- Subhasis Ray 2014-06-04

Original comment:

COMMENT

26 Ago 2002 Modification of original channel to allow variable time
step and to correct an initialization error.
    Done by Michael Hines(michael.hines@yale.e) and Ruggero
Scorcioni(rscorcio@gmu.edu) at EU Advance Course in Computational
Neuroscience. Obidos, Portugal
11 Jan 2007
    Glitch in trap where (v/th) was where (v-th)/q is. (thanks Ronald
van Elburg!)

na.mod

Sodium channel, Hodgkin-Huxley style kinetics.  

Kinetics were fit to data from Huguenard et al. (1988) and Hamill et
al. (1991)

qi is not well constrained by the data, since there are no points
between -80 and -55.  So this was fixed at 5 while the thi1,thi2,Rg,Rd
were optimized using a simplex least square proc

voltage dependencies are shifted approximately from the best
fit to give higher threshold

Author: Zach Mainen, Salk Institute, 1994, zach@salk.edu

ENDCOMMENT

"""

def trap0(v, th, a, q):
    return np.where(np.abs((v-th)/q) > 1e-6,
                    a * (v - th) / (1 - exp(-(v - th) / q)),
                    a * q)

def make_na():
    library = moose.Neutral('/library')
    channel = moose.HHChannel('/library/Na')
    channel.Gbar = 0.0 # 1000 * tadj 
    channel.Xpower = 3
    channel.Ypower = 1
    vshift = -5 # -10 # has been set to -5 in init_biophysics.hoc
    vdivs  = 3000
    vmin = -120
    vmax = 100
    vm = np.linspace(vmin, vmax, vdivs) + vshift
    tha  = -35	# (mV)		: v 1/2 for act		(-42)
    qa   = 9	# (mV)		: act slope		
    Ra   = 0.182	# (/ms)		: open (v)		
    Rb   = 0.124	# (/ms)		: close (v)		
                    # 
    thi1  = -50	# (mV)		: v 1/2 for inact 	
    thi2  = -75	# (mV)		: v 1/2 for inact 	
    qi   = 5	# (mV)	        : inact tau slope
    thinf  = -65	# (mV)		: inact inf slope	
    qinf  = 6.2	# (mV)		: inact inf slope
    Rg   = 0.0091	# (/ms)		: inact (v)	
    Rd   = 0.024	# (/ms)		: inact recov (v) 

    # m gate - activation
    a = trap0(vm, tha, Ra, qa)
    b =  trap0(-vm, -tha, Rb, qa)
    mtau = 1e-3 / tadj / (a + b)
    minf = a / (a + b)
    channel.gateX[0].tableA = minf / mtau # this contains alpha
    channel.gateX[0].tableB = 1.0 / mtau  # this contains alpha + beta
    channel.gateX[0].min = vmin * 1e-3
    channel.gateX[0].max = vmax * 1e-3
    
    # h gate - inactivation
    a = trap0(vm, thi1, Rd, qi)
    b = trap0(-vm, -thi2, Rg, qi)
    htau = 1e-3 / tadj / (a + b)
    hinf = 1 / (1 + np.exp((vm - thinf) / qinf))
    channel.gateY[0].tableA = hinf / htau
    channel.gateY[0].tableB = 1 / htau
    channel.gateY[0].min = vmin * 1e-3
    channel.gateY[0].max = vmax * 1e-3
    channel.Ek = 60e-3
    return channel

from pylab import *

# Make the prototype channel and plot minf, hinf, mtau, htau
if __name__ == '__main__':
    a = make_na()
    xa = moose.element(a.gateX).tableA
    xb = moose.element(a.gateX).tableB
    ya = moose.element(a.gateY).tableA
    yb = moose.element(a.gateY).tableB
    minf = xa / xb
    mtau = 1 / xb
    hinf = ya / yb
    htau = 1 / yb
    subplot(211)
    plot(minf, label='minf')
    plot(hinf, label='hinf')
    legend()
    subplot(212)
    plot(mtau, label='mtau')
    plot(htau, label='htau')
    legend()
    show()

# 
# nachannel.py ends here
