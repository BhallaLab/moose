# kchans.py --- 
# 
# Filename: kchans.py
# Description: 
# Author: subhasis ray
# Maintainer: 
# Created: Fri Apr 17 23:58:49 2009 (+0530)
# Version: 
# Last-Updated: Wed Dec  9 12:29:24 2015 (-0500)
#           By: Subhasis Ray
#     Update #: 1062
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
# 

# Code:

import moose
from channelbase import *
from numpy import where, linspace, exp, arange, ones, zeros, array
import numpy as np


class KChannel(ChannelBase):
    """This is a dummy base class to keep type information."""
    annotation = {'cno': 'cno_0000047'}
    abstract = True
    Ek = -95e-3
    X = 0.0
    def __init__(self, path):
        ChannelBase.__init__(self, path)


class KDR(KChannel):
    """Delayed rectifier current

    `In hippocampal pyramidal neurons, however, it has been reported
    have relatively slow activation, with a time to peak of some
    50-100 msec and even slower inactivation. Such a slow activation
    would make it ill suited to participate in the repolarization of
    the AP.... An equation that can describe IK(DR) in cortical
    neurons is
    
    IK(DR) = m^3 * h * gbar_K(DR) * (Vm - EK)
    
    where m and h depend on voltage and time.`
        - Johnston & Wu, Foundations of Cellular Neurophysiology (1995).

    But in Traub 2005, the equation used is:
    
    IK(DR) = m^4 * gbar_K(DR) * (Vm - EK)
    """
    annotation = {'cno': 'cno_0000105'}
    abstract = False

    Xpower = 4
    tau_x = where(v_array < -10e-3, \
                      1e-3 * (0.25 + 4.35 * exp((v_array + 10.0e-3) / 10.0e-3)), \
                      1e-3 * (0.25 + 4.35 * exp((- v_array - 10.0e-3) / 10.0e-3)))
    inf_x = 1.0 / (1.0 + exp((- v_array - 29.5e-3) / 10e-3))

    def __init__(self, path):
        KChannel.__init__(self, path)


class KDR_FS(KDR):
    """KDR for fast spiking neurons"""
    annotation = {'cno': 'cno_0000105'}
    abstract = False

    Ek = -100e-3
    inf_x = 1.0 / (1.0 + exp((- v_array - 27e-3) / 11.5e-3))
    tau_x =  where(v_array < -10e-3, \
                       1e-3 * (0.25 + 4.35 * exp((v_array + 10.0e-3) / 10.0e-3)), \
                       1e-3 * (0.25 + 4.35 * exp((- v_array - 10.0e-3) / 10.0e-3)))

    def __init__(self, path):
        KChannel.__init__(self, path)


class KA(KChannel):
    """A type K+ channel"""
    annotation = {'cno': 'cno_0000105'}
    abstract = False

    Xpower = 4
    Ypower = 1
    inf_x = 1 / ( 1 + exp( ( - v_array - 60e-3 ) / 8.5e-3 ) )
    tau_x =  1e-3 * (0.185 + 0.5 / ( exp( ( v_array + 35.8e-3 ) / 19.7e-3 ) + exp( ( - v_array - 79.7e-3 ) / 12.7e-3 ) ))
    inf_y =   1 / ( 1 + exp( ( v_array + 78e-3 ) / 6e-3 ) )
    tau_y = where( v_array <= -63e-3,\
                       1e-3 * 0.5 / ( exp( ( v_array + 46e-3 ) / 5e-3 ) + exp( ( - v_array - 238e-3 ) / 37.5e-3 ) ), \
                       9.5e-3)

    def __init__(self, path):
        KChannel.__init__(self, path)


class KA_IB(KA):
    """A type K+ channel for tufted intrinsically bursting cells -
    multiplies tau_h of KA by 2.6"""

    annotation = {'cno': 'cno_0000105'}
    abstract = False

    inf_y =   1 / ( 1 + exp( ( v_array + 78e-3 ) / 6e-3 ) )
    tau_y = 2.6 * KA.tau_y

    def __init__(self, path):
        KChannel.__init__(self, path)


class K2(KChannel):
    annotation = {'cno': 'cno_0000105'}
    Xpower = 1
    Ypower = 1
    inf_x  = 1 / ( 1 + exp( ( - v_array *1e3 - 10 ) / 17 ) )
    tau_x = 1e-3 * (4.95 + 0.5 / ( exp( ( v_array * 1e3 - 81 ) / 25.6 ) + exp( ( - v_array * 1e3 - 132 ) / 18 ) ))
    inf_y  = 1 / ( 1 + exp( ( v_array*1e3 + 58 ) / 10.6 ) )
    tau_y  = 1e-3 * (60 + 0.5 / ( exp( ( v_array*1e3 - 1.33 ) / 200 ) + exp( ( - v_array*1e3 - 130 ) / 7.1 ) ))

    def __init__(self, path):
        KChannel.__init__(self, path)
	

class KM(KChannel):
    """Mascarinic sensitive K channel"""
    annotation = {'cno': 'cno_0000105'}
    abstract = False
    Xpower = 1
    alpha_x =  1e3 * 0.02 / ( 1 + exp((-v_array - 20e-3 ) / 5e-3))
    beta_x = 1e3 * 0.01 * exp((-v_array - 43e-3) / 18e-3)

    def __init__(self, path):
        KChannel.__init__(self, path)
        
        
class KCaChannel(KChannel):
    """[Ca+2] dependent K+ channel base class."""
    annotation = {'cno': 'cno_0000047'}
    abstract = True
    Zpower = 1
    mstring = ('addmsg1', '../CaPool concOut . concen')
    def __init__(self, path):
        KChannel.__init__(self, path)


class KAHPBase(KCaChannel):
    annotation = {'cno': 'cno_0000108'}
    abstract = True
    Z = 0.0

    def __init__(self, path):
        KCaChannel.__init__(self, path)
        

class KAHP(KAHPBase):
    """AHP type K+ current"""
    annotation = {'cno': 'cno_0000108'}
    abstract = False
    alpha_z = where(ca_conc < 100.0, 0.1 * ca_conc, 10.0)
    beta_z =  ones(ca_divs + 1) * 10.0

    def __init__(self, path):
        KAHPBase.__init__(self, path)


class KAHP_SLOWER(KAHPBase):
    annotation = {'cno': 'cno_0000108'}
    abstract = False
    alpha_z = where(ca_conc < 500.0, 1e3 * ca_conc / 50000, 10.0)
    beta_z =  ones(ca_divs + 1) * 1.0

    def __init__(self, path):
        KAHPBase.__init__(self, path)


class KAHP_DP(KAHPBase):
    """KAHP for deep pyramidal cell"""
    annotation = {'cno': 'cno_0000108'}
    abstract = False
    alpha_z = where(ca_conc < 100.0, 1e-1 * ca_conc, 10.0)
    beta_z =  ones(ca_divs + 1)

    def __init__(self, path):
        KAHPBase.__init__(self, path)


class KC(KCaChannel):
    """C type K+ channel
    """
    annotation = {'cno': 'cno_0000106'}
    abstract = False

    Xpower = 1
    Zpower = 1
    tableA_z = where(ca_conc < 250.0, ca_conc / 250.0, 1.0)
    tableB_z = ones(ca_divs + 1)
    alpha_x = where(v_array < -10e-3, 
                      2e3 / 37.95 * ( exp( ( v_array * 1e3 + 50 ) / 11 - ( v_array * 1e3 + 53.5 ) / 27 ) ),
                      2e3 * exp(( - v_array * 1e3 - 53.5) / 27))
    beta_x = where(v_array < -10e-3,
                   2e3 * exp(( - v_array * 1e3 - 53.5) / 27 - alpha_x), 
                   0.0)
    instant = 4

    def __init__(self, path):
        KCaChannel.__init__(self, path)

        
class KC_FAST(KC):
    """Fast KC channel
    """
    annotation = {'cno': 'cno_0000106'}
    abstract = False

    alpha_x = KC.alpha_x * 2
    beta_x = KC.beta_x * 2

    def __init__(self, path):
        KC.__init__(self, path)

        
def initKChannelPrototypes(libpath='/library'):
    channel_names = ['KDR', 
                     'KDR_FS', 
                     'KA', 
                     'KA_IB',
                     'K2', 
                     'KM', 
                     'KAHP',
                     'KAHP_SLOWER',
                     'KAHP_DP',
                     'KC',
                     'KC_FAST']    
    return dict([(key, eval('%s("%s")' % (key, prototypes[key].path))) for key in channel_names])
        

# 
# kchans.py ends here
