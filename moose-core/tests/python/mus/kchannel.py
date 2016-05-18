# kchannel.py --- 
# 
# Filename: kchannel.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Jun  4 12:30:31 2014 (+0530)
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

def rates_kv(v, Ra, Rb, tha, qa): 
    a = Ra * (v - tha) / (1 - np.exp(-(v - tha)/qa))
    b = -Rb * (v - tha) / (1 - np.exp((v - tha)/qa))
    ntau = 1e-3/tadj/(a+b)
    ninf = a/(a+b)
    return ntau, ninf

def make_kv():
    """Translated to pymoose from original model by Zach Mainen. - Subhasis Ray 2014-06-04

    Original comment:
    
    COMMENT
    26 Ago 2002 Modification of original channel to allow variable time step and to correct an initialization error.
    Done by Michael Hines(michael.hines@yale.e) and Ruggero Scorcioni(rscorcio@gmu.edu) at EU Advance Course in Computational Neuroscience. Obidos, Portugal
    
    kv.mod
    
    Potassium channel, Hodgkin-Huxley style kinetics
    Kinetic rates based roughly on Sah et al. and Hamill et al. (1991)
    
    Author: Zach Mainen, Salk Institute, 1995, zach@salk.edu
    
    ENDCOMMENT
    """

    gbar = 5 * tadj  	# (pS/um2)	: 0.03 mho/cm2
    tha  = 25	# (mV)		: v 1/2 for inf
    qa   = 9	# (mV)		: inf slope		
    Ra   = 0.02	# (/ms)		: max act rate
    Rb   = 0.002	# (/ms)		: max deact rate	
    vmin = -120	# (mV)
    vmax = 100	# (mV)
    v = np.linspace(vmin, vmax, 3000)
    ntau, ninf = rates_kv(v, Ra, Rb, tha, qa)
    channel = moose.HHChannel('/library/Kv')
    channel.Xpower = 1
    channel.gateX[0].tableA = 1e3 * ninf / ntau
    channel.gateX[0].tableB = 1e3 / ntau
    channel.gateX[0].min = vmin * 1e-3
    channel.gateX[0].max = vmax * 1e-3
    channel.Gbar = 0.0
    channel.Ek = -90e-3
    return channel

def make_km():
    """Ported to pymoose. - Subhasis Ray, Thu Jun 5 15:07:58 IST 2014

    Original comment:

    COMMENT
    26 Ago 2002 Modification of original channel to allow variable time step and to correct an initialization error.
    Done by Michael Hines(michael.hines@yale.e) and Ruggero Scorcioni(rscorcio@gmu.edu) at EU Advance Course in Computational Neuroscience. Obidos, Portugal
    
    km.mod
    
    Potassium channel, Hodgkin-Huxley style kinetics
    Based on I-M (muscarinic K channel)
    Slow, noninactivating
    
    Author: Zach Mainen, Salk Institute, 1995, zach@salk.edu
    
    ENDCOMMENT

    """
    gbar = 10   	# (pS/um2) = S/m2	: 0.03 mho/cm2
    tha  = -30	# (mV)		: v 1/2 for inf
    qa   = 9	# (mV)		: inf slope		
    Ra   = 0.001	# (/ms)		: max act rate  (slow)
    Rb   = 0.001	# (/ms)		: max deact rate  (slow)
    vmin = -120	# (mV)
    vmax = 100	# (mV)
    v = np.linspace(vmin, vmax, 3000)        
    a = Ra * (v - tha) / (1 - np.exp(-(v - tha)/qa))
    b = -Rb * (v - tha) / (1 - np.exp((v - tha)/qa))
    ntau = 1 / tadj /(a + b)
    ninf = a/(a+b)
    channel = moose.HHChannel('/library/Km')
    channel.Gbar = 0.0 # tadj * gbar # not really used
    channel.Xpower = 1
    channel.gateX[0].min = vmin * 1e-3
    channel.gateX[0].max = vmax * 1e-3
    channel.gateX[0].tableA = 1e3 * ninf / ntau
    channel.gateX[0].tableB = 1e3 / ntau
    channel.Ek = -90e-3
    return channel

def make_kca():
    """Ported from NEURON code. - Subhasis Ray, Thu Jun  5 16:18:14 IST 2014

    original file comment:

    COMMENT
    26 Ago 2002 Modification of original channel to allow variable time step and to correct an initialization error.
    Done by Michael Hines(michael.hines@yale.e) and Ruggero Scorcioni(rscorcio@gmu.edu) at EU Advance Course in Computational Neuroscience. Obidos, Portugal

    kca.mod
    
    Calcium-dependent potassium channel
    Based on
    Pennefather (1990) -- sympathetic ganglion cells
    taken from
    Reuveni et al (1993) -- neocortical cells
    
    Author: Zach Mainen, Salk Institute, 1995, zach@salk.edu
    
    ENDCOMMENT
    """
    gbar = 10   	# (pS/um2)	: 0.03 mho/cm2
    caix = 1	# 
    Ra   = 0.01	# (/ms)		: max act rate  
    Rb   = 0.02	# (/ms)		: max deact rate 
    vmin = -120	# (mV)
    vmax = 100	# (mV)
    # In MOOSE we set up lookup tables beforehand
    camin = 0.0
    camax = 500.0
    cai = np.linspace(camin, camax)
    a = Ra * cai**caix
    b = Rb
    ntau = 1 / tadj / ( a + b)
    ninf = a / (a + b)
    channel = moose.HHChannel('/library/KCa')
    channel.Zpower = 1
    channel.useConcentration = 1
    channel.gateZ[0].min = camin
    channel.gateZ[0].max = camax
    channel.gateZ[0].tableA = 1e3 * ninf / ntau
    channel.gateZ[0].tableB = 1e3 / ntau
    channel.Gbar = 0.0
    channel.Ek = -90e-3
    # addmsg1 = moose.Mstring('%s/addmsg1' % (channel.path))
    # addmsg1.value = '../Ca_conc concOut . concen'
    capool = moose.BufPool('%s/CaPool' % (channel.path)) # send a constant [Ca2+] to this channel
    capool.n = 5e-5 # n is actually number of molecules (in
                    # floating point though, but I am using it for
                    # conc, because only nOut src msg is available
    moose.connect(capool, 'nOut', channel, 'concen')
    


# 
# kchannel.py ends here
