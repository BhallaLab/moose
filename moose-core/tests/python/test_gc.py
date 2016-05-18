# test_gc.py --- 
# 
# Filename: test_gc.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Mon May 19 10:25:13 2014 (+0530)
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
"""Test script for memory allocation and garbage collection issues."""

from __future__ import print_function
try:
    from future_builtins import zip, range
except ImportError:
    pass
import random
from collections import defaultdict
from itertools import cycle

import moose

classes = [
    'Reac',
    'IzhikevichNrn',
    'PIDController',
    'HHChannel',
    'PulseGen',
    'Pool',
    'RC',
    'BufPool',
    'DiffAmp',
    'IntFire',
    'MgBlock',]
           
def allocate_large_vecs(m, n):
    """Allocate m vecs with n elements each"""
    test = moose.Neutral('/test')
    ret = []    
    for jj, mclass in zip(range(m), cycle(classes)):
        eval_str = 'moose.vec(path="%s/%s_%d", n=%d, dtype="%s")' % (test.path,
                                                                     mclass,
                                                                     jj,
                                                                     n,
                                                                     mclass)        
        mobj = eval(eval_str)
        print('Created', mobj.path)
        ret.append(mobj)
    return ret

def create_finfos():
    fields = defaultdict(list)
    for el in moose.wildcardFind('/test/#'):
        print(el)
        mobj = moose.element(el)
        for f in moose.getFieldNames(mobj.className, 'lookup'):
            fields[mobj].append(getattr(mobj, f))
        for f in moose.getFieldNames(mobj.className, 'element'):
            fields[mobj].append(getattr(mobj, f))
    return fields

def check_vector_field(m, n):
    test = moose.Neutral('/test')
    tabs = []
    for ii in range(m):
        comp = moose.Compartment('/test/comp_%d' % (ii), n=n)
        for jj in range(n):
            tab = moose.Table('/test/data_%d_%d' % (ii, jj))
            moose.connect(tab, 'requestOut', comp.vec[jj], 'getVm')
            tabs.append(tab)
    moose.setClock(0, 1e-3)
    moose.setClock(1, 1e-3)
    moose.setClock(2, 1e-3)
    moose.useClock(0, '/##[ISA=Compartment]', 'init')
    moose.useClock(1, '/##[ISA=Compartment]', 'process')
    moose.useClock(2, '/##[ISA=Table]', 'process')
    moose.reinit()
    moose.start(0.01)
    return tabs

import numpy as np
    
if __name__ == '__main__':
    np_arrays = []
    for ii in range(3):
        print('Creating elements')
        allocate_large_vecs(100, 100)
        print('Created elements. Creating field dict now')
        create_finfos()
        moose.delete(moose.element('/test'))
        tabs = check_vector_field(100, 100)
        for t in tabs:
            np_arrays.append(np.array(t.vec))
        moose.delete('/test')
    print('Finished')



# 
# test_gc.py ends here
