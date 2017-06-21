# hchannel.py --- 
# 
# Filename: hchannel.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Thu Jun  5 15:20:30 2014 (+0530)
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

library = moose.Neutral('library')

def make_h():
    """Converted from code for Branco et al 2010 by Subhasis Ray,
    Thu Jun  5 15:32:33 IST 2014

    Original title:

    TITLE I-h channel from Magee 1998 for distal dendrites

    """
    ghdbar = 1.0  # (mho/cm2) does not matter - we are reading from
    vhalfl = -81  # (mV)
    kl = -8
    vhalft = -75  # (mV)
    a0t = 0.011   #   (/ms)
    zetat = 2.2    # (1)
    gmt = .4       # (1)
    q10 = 4.5
    qtl = 1
    qt=q10**((temp - 33) / 10)
    vmin = -120
    vmax = 100
    v = np.linspace(vmin, vmax, 3000)
    a = np.exp(0.0378 * zetat * (v - vhalft))
    linf = 1/(1 + np.exp(-(v-vhalfl)/kl))
    taul = np.exp(0.0378 * zetat * gmt * (v - vhalft)) / (qtl * qt * a0t * (1 + a))
    channel = moose.HHChannel('/library/H')
    channel.Xpower = 1
    channel.gateX[0].tableA = 1e3 * linf / taul # taul: ms -> s
    channel.gateX[0].tableB = 1e3 / taul # taul: ms -> s
    channel.gateX[0].min = 1e-3 * vmin
    channel.gateX[0].max = 1e-3 * vmax
    return channel

# 
# hchannel.py ends here
