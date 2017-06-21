# archan.py --- 
# 
# Filename: archan.py
# Description: 
# Author: subhasis ray
# Maintainer: 
# Created: Mon Apr 27 15:34:07 2009 (+0530)
# Version: 
# Last-Updated: Sat Dec  8 15:44:58 2012 (+0530)
#           By: subha
#     Update #: 50
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
# 

# Code:

from numpy import exp
import moose
from channelbase import *

class AR(ChannelBase):
    """Combined cation current."""
    annotation = {'cno': 'cno:cno_0000105'}
    abstract = False
    Xpower = 1
    inf_x  = 1 / ( 1 + exp( ( v_array * 1e3 + 75 ) / 5.5 ) )
    tau_x = 1e-3 / ( exp( -14.6 - 0.086 * v_array * 1e3) + exp( -1.87 + 0.07 * v_array * 1e3))
    X = 0.25
    Ek = -35e-3

    def __init__(self, path, Ek=-35e-3):
        ChannelBase.__init__(self, path)


def initARChannelPrototypes(libpath='/library'):
    return {'AR': AR(prototypes['AR'])}


# 
# archan.py ends here
