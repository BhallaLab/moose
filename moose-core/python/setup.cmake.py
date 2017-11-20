# -*- coding: utf-8 -*-

"""setup.py:
Script to install python targets. 

NOTE: This script is to be called by CMake. Not intended to be used standalone.

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import os
import sys
import site

# if uid is zero then install in root paths. Else install it in user path.

#print( '[INFO] Overwriting --prefix ' )
#try:
#    prefixLoc = sys.argv.index( '--prefix' )
#    del sys.argv[ prefixLoc ]
#    del sys.argv[ prefixLoc ]
#except Exception as e:
#    pass
#
#uid = os.getuid( )
#if uid == 0:
#    # Here comes the root.
#    print( '   called by sudo' )
#    sys.argv += [ '--prefix', '/usr' ]
#else:
#    sys.argv += [ '--prefix', site.getuserbase( ) ]
#
#print( sys.argv )

from distutils.core import setup

script_dir = os.path.dirname( os.path.abspath( __file__ ) )

version = '3.2.git'
try:
    with open( os.path.join( script_dir, '..', 'VERSION'), 'r' ) as f:
        version = f.read( )
except Exception as e:
    print( 'Failed to read VERSION %s' % e )
    print( 'Using default %s' % version )

try:
    import importlib.machinery
    suffix = importlib.machinery.EXTENSION_SUFFIXES[0]
except Exception as e:
    print( '[WARN] Failed to determine importlib suffix' )
    suffix = '.so'

setup(
        name='moose',
        version=version,
        description='MOOSE python scripting module.',
        author='MOOSERes',
        author_email='bhalla@ncbs.res.in',
        maintainer='Dilawar Singh',
        maintainer_email='dilawars@ncbs.res.in',
        url='http://moose.ncbs.res.in',
        packages=[
            'rdesigneur'
            , 'moose'
            , 'moose.SBML'
            , 'moose.neuroml'
            , 'moose.genesis'
            , 'moose.chemUtil'
            , 'moose.chemMerge'
            ],
        package_dir = {
            'moose' : 'moose'
            , 'rdesigneur' : 'rdesigneur'
            },
        package_data = { 'moose' : ['_moose' + suffix] },
    )
