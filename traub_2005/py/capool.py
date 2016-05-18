# CaPool.py --- 
# 
# Filename: capool.py
# Description: 
# Author: subhasis ray
# Maintainer: 
# Created: Wed Apr 22 22:21:11 2009 (+0530)
# Version: 
# Last-Updated: Sat Dec  8 15:46:00 2012 (+0530)
#           By: subha
#     Update #: 189
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Implements the Ca2+ pool
# 
# 

# Change log:
# 
# 
# 
# 

# Code:

import config
import moose

class CaPool(moose.CaConc):
    annotation = {'cno': 'cno_0000056'}
    prototype = moose.CaConc(config.modelSettings.libpath + '/CaPool')
    prototype.CaBasal = 0.0
    prototype.ceiling = 1e6
    prototype.floor = 0.0
    prototypes = {}
    def __init__(self, path):
        moose.CaConc.__init__(self, path)
        

def initCaPoolPrototypes(libpath='/library'):
    if CaPool.prototypes:
        return CaPool.prototypes
    path = '%s/CaPool' % (libpath)
    CaPool.prototypes['CaPool'] = CaPool(path)
    return CaPool.prototypes


# 
# capool.py ends here
