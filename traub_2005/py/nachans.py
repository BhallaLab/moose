# nachans.py --- 
# 
# Filename: nachans.py
# Description: 
# Author: subhasis ray
# Maintainer: 
# Created: Fri Apr 17 23:58:13 2009 (+0530)
# Version: 
# Last-Updated: Sun Jun 25 01:54:48 2017 (-0400)
#           By: subha
#     Update #: 403
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

# Code:

from numpy import where, linspace, exp
#import pylab
import moose

import config
from channelbase import *

class NaChannel(ChannelBase):
    """Dummy base class for all Na+ channels"""
    annotation = {'cno': 'cno:cno_0000105'}
    abstract = True
    Ek = 50e-3
    def __init__(self, path):
        ChannelBase.__init__(self, path)
        

class NaF(NaChannel):
    """Fast Na channel.
    """
    abstract = False
    Xpower = 3
    Ypower = 1
    X = 0.0
    # Y =  0.54876953
    shift = -3.5e-3
    tau_x = where((v_array + shift) < -30e-3, \
                      1.0e-3 * (0.025 + 0.14 * exp((v_array + shift + 30.0e-3) / 10.0e-3)), \
                      1.0e-3 * (0.02 + 0.145 * exp(( - v_array - shift - 30.0e-3) / 10.0e-3)))
    inf_x = 1.0 / (1.0 + exp(( - v_array - shift - 38e-3) / 10e-3))
    tau_y = 1.0e-3 * (0.15 + 1.15 / ( 1.0 + exp(( v_array + 37.0e-3) / 15.0e-3)))
    inf_y = 1.0 / (1.0 + exp((v_array + 62.9e-3) / 10.7e-3))
        
    def __init__(self, path, shift=-3.5e-3, Ek=50e-3):
        NaChannel.__init__(self, path )


class NaF_TCR(NaF):    
    """Fast Na+ channel for TCR cells. This is almost identical to
    NaF, but there is a nasty voltage shift in the tables."""
    abstract = False
    shift_x = -5.5e-3
    shift_y = -7e-3
    tau_y = 1.0e-3 * (0.15 + 1.15 / ( 1.0 + exp(( v_array + 37.0e-3) / 15.0e-3)))        
    inf_y = 1.0 / (1.0 + exp((v_array + shift_y + 62.9e-3) / 10.7e-3))
    tau_x = where((v_array + shift_x) < -30e-3, \
                      1.0e-3 * (0.025 + 0.14 * exp((v_array + shift_x + 30.0e-3) / 10.0e-3)), \
                      1.0e-3 * (0.02 + 0.145 * exp(( - v_array - shift_x - 30.0e-3) / 10.0e-3)))
    inf_x = 1.0 / (1.0 + exp(( - v_array - shift_x - 38e-3) / 10e-3))

    def __init__(self, path):
        NaChannel.__init__(self, path)

        
class NaF2(NaF):
    abstract = False
    # shift=-2.5 for all cortical interneurons including spiny stellates
    # In neuron cell templates fastNa_shift_naf2=-2.5
    shift = -2.5e-3
    tau_x = where((v_array + shift) < -30e-3, \
                      1.0e-3 * (0.0125 + 0.1525 * exp ((v_array + shift + 30e-3) / 10e-3)), \
                      1.0e-3 * (0.02 + 0.145 * exp((- v_array - shift - 30e-3) / 10e-3)))        
    inf_x = 1.0 / (1.0 + exp(( - v_array - shift - 38e-3) / 10e-3))
    tau_y = 1e-3 * (0.225 + 1.125 / ( 1 + exp( (  v_array + 37e-3 ) / 15e-3 ) ))        
    inf_y = 1.0 / (1.0 + exp((v_array + 58.3e-3) / 6.7e-3))

    def __init__(self, path):
        NaChannel.__init__(self, path)

class NaF2_nRT(NaF2):
    """This is a version of NaF2 without the fastNa_shift - applicable to nRT cell."""
    # for nRT cells, fastNa_shift_naf2 is not set in the cell
    # template. In naf2.mod it is set to 0 in PARAMETERS section.
    abstract = False
    tau_x = where(v_array < -30e-3, \
                      1.0e-3 * (0.0125 + 0.1525 * exp ((v_array + 30e-3) / 10e-3)), \
                      1.0e-3 * (0.02 + 0.145 * exp((-v_array - 30e-3) / 10e-3)))        
    inf_x = 1.0 / (1.0 + exp(( - v_array - 38e-3) / 10e-3))
    tau_y = 1e-3 * (0.225 + 1.125 / ( 1 + exp( (  v_array + 37e-3 ) / 15e-3 ) ))        
    inf_y = 1.0 / (1.0 + exp((v_array + 58.3e-3) / 6.7e-3))
    def __init__(self, path):
        NaF2.__init__(self, path)


class NaP(NaChannel):
    abstract = False
    Xpower = 1.0
    Ypower = 0.0
    tau_x = where(v_array < -40e-3, \
                      1.0e-3 * (0.025 + 0.14 * exp((v_array + 40e-3) / 10e-3)), \
                      1.0e-3 * (0.02 + 0.145 * exp((-v_array - 40e-3) / 10e-3)))
    inf_x = 1.0 / (1.0 + exp((-v_array - 48e-3) / 10e-3))

    def __init__(self, path, Ek=50e-3):
        NaChannel.__init__(self, path)


class NaPF(NaChannel):
    """Persistent Na+ current, fast"""
    abstract = False
    Xpower = 3
    tau_x = where(v_array < -30e-3, \
                      1.0e-3 * (0.025 + 0.14 * exp((v_array  + 30.0e-3) / 10.0e-3)), \
                      1.0e-3 * (0.02 + 0.145 * exp((- v_array - 30.0e-3) / 10.0e-3)))
    inf_x = 1.0 / (1.0 + exp((-v_array - 38e-3) / 10e-3))
    
    def __init__(self, path):
        NaChannel.__init__(self, path)


class NaPF_SS(NaPF):
    abstract = False
    shift = -2.5e-3
    tau_x = where((v_array + shift) < -30e-3, \
                       1.0e-3 * (0.025 + 0.14 * exp(((v_array + shift)  + 30.0e-3) / 10.0e-3)), \
                       1.0e-3 * (0.02 + 0.145 * exp((- (v_array + shift) - 30.0e-3) / 10.0e-3)))
    inf_x = 1.0 / (1.0 + exp((- (v_array + shift) - 38e-3) / 10e-3))

    def __init__(self, path):
        NaChannel.__init__(self, path)


class NaPF_TCR(NaPF):
    """Persistent Na+ channel specific to TCR cells. Only difference
    with NaPF is power of m is 1 as opposed 3."""
    abstract = False
    shift = 7e-3
    Xpower = 1
    tau_x = where((v_array + shift) < -30e-3, \
                       1.0e-3 * (0.025 + 0.14 * exp(((v_array + shift)  + 30.0e-3) / 10.0e-3)), \
                       1.0e-3 * (0.02 + 0.145 * exp((- (v_array + shift) - 30.0e-3) / 10.0e-3)))
    inf_x = 1.0 / (1.0 + exp((-(v_array + shift) - 38e-3) / 10e-3))
    def __init__(self, path):
        NaChannel.__init__(self, path)

        
def initNaChannelPrototypes():
    channel_names = [
        'NaF',
        'NaF2',
        'NaF2_nRT',
        'NaP',
        'NaPF',
        'NaPF_SS',
        'NaPF_TCR',
        'NaF_TCR',
        ]
    _proto = {}
    for name in channel_names:
        chanclass = eval(name)
        _proto[name] = chanclass(prototypes[name])
    return _proto


# 
# nachans.py ends here
