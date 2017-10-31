# -*- coding: utf-8 -*-
"""test_docs.py:

Test if moose.doc is working.

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2017-, Dilawar Singh"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import moose
print( '[INFO] Using moose from %s' % moose.__file__ )
print( '[INFO] Version : %s' % moose.version( ) )

def main( ):
    moose.doc( 'Compartment' )

if __name__ == '__main__':
    main()
