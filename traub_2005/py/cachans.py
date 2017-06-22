# cachans.py --- 
# 
# Filename: cachans.py
# Description: 
# Author: subhasis ray
# Maintainer: 
# Created: Sat Apr 18 00:18:24 2009 (+0530)
# Version: 
# Last-Updated: Sat Dec  8 15:48:17 2012 (+0530)
#           By: subha
#     Update #: 296
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

from numpy import where, exp, array
import moose
from channelbase import *

class CaChannel(ChannelBase):
    """Base class for Ca channels."""
    annotation = {'cno': 'cno:cno_0000105'}
    abstract = True
    Ek = 125e-3
    Xpower = 2
    X = 0.0
    """This is just a place holder to maintain type information"""
    def __init__(self, path):
        ChannelBase.__init__(self, path)


class CaL(CaChannel):
    """Low threshold calcium channel"""
    abstract = False
    alpha_x = 1.6e3 / (1.0 + exp(-0.072 * (v_array * 1e3 - 5)))
    v = v_array + 8.9e-3
    beta_x = where( abs(v) * 1e3 < 1e-6,
                  1e3 * 0.1 * exp(-v / 5e-3),
                  1e3 * 0.02 * v * 1e3 / (exp(v / 5e-3) - 1))
    mstring = ('addmsg1', '.	IkOut	../CaPool	current')
    def __init__(self, path):
        CaChannel.__init__(self, path)


class CaT(CaChannel):
    abstract = False
    Ypower = 1
    inf_x = 1 / (1 + exp( (- v_array - 56e-3) / 6.2e-3))
    tau_x = 1e-3 * (0.204 + 0.333 / ( exp(( v_array + 15.8e-3) / 18.2e-3 ) + 
                                      exp((- v_array - 131e-3) / 16.7e-3)))
    inf_y = 1 / (1 + exp(( v_array + 80e-3 ) / 4e-3))
    tau_y = where( v_array < -81e-3, 
                   1e-3 * 0.333 * exp( ( v_array + 466e-3 ) / 66.6e-3 ),
                   1e-3 * (9.32 + 0.333 * exp( ( -v_array - 21e-3 ) / 10.5e-3 )))

    def __init__(self, path):
        CaChannel.__init__(self, path)


class CaT_A(CaT):
    inf_x  = 1.0 / ( 1 + exp( ( - v_array * 1e3 - 52 ) / 7.4 ) )
    tau_x  = 1e-3 * (1 + .33 / ( exp( ( v_array * 1e3 + 27.0 ) / 10.0 ) + exp( ( - v_array * 1e3 - 102 ) / 15.0 )))
    
    inf_y  = 1 / ( 1 + exp( ( v_array * 1e3 + 80 ) / 5 ) )
    tau_y = 1e-3 * (28.30 + 0.33 / (exp(( v_array * 1e3 + 48.0)/ 4.0) + exp( ( -v_array * 1e3 - 407.0) / 50.0 ) ))

    def __init__(self, path):
        CaChannel.__init__(self, path)


def initCaChannelPrototypes(libpath='/library'):
    channel_names = ['CaL', 'CaT', 'CaT_A']
    return dict([(key, eval('%s("%s")' % (key, prototypes[key].path))) for key in channel_names])



# 
# cachans.py ends here
