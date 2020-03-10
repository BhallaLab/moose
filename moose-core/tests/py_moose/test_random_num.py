# -*- coding: utf-8 -*-
"""test_random_num.py:

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2017-, Dilawar Singh"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import numpy as np
import moose

try:
    reload
except NameError:
    # Python 3
    from importlib import reload

def test_unequal():
    rand1, rand2 = [], []
    N = 10
    for i in range( N ):
        x = moose.rand()
        rand1.append( x )
    reload( moose )
    for i in range( N ):
        x = moose.rand()
        rand2.append( x )

    assert not np.equal( rand1, rand2 ).all()

def test_equal():
    rand1, rand2 = [], []
    N = 10
    moose.seed( 10 )
    for i in range( N ):
        x = moose.rand()
        rand1.append( x )

    reload( moose )
    moose.seed( 10 )
    for i in range( N ):
        x = moose.rand()
        rand2.append( x )

    assert np.equal( rand1, rand2 ).all()

def main( ):
    test_unequal()
    test_equal()

if __name__ == '__main__':
    main()

