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

try:
    from setuptools import setup
except Exception as e:
    from distutils.core import setup

script_dir = os.path.dirname( os.path.abspath( __file__ ) )
version = '3.1.3'

try:
    with open( os.path.join( script_dir, 'VERSION'), 'r' ) as f:
        version = f.read( )
except Exception as e:
    print( 'Failed to read VERSION %s' % e )
    print( 'Using default %s' % version )

import importlib.machinery
suffix = importlib.machinery.EXTENSION_SUFFIXES[-1]

setup(
        name='pymoose',
        version=version,
        description='Python scripting interface of MOOSE Simulator (https://moose.ncbs.res.in)',
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
            ],
	install_requires = [ 'python-libsbml', 'numpy' ],
        package_dir = { 'moose' : 'moose', 'rdesigneur' : 'rdesigneur' },
        package_data = { 'moose' : ['_moose*%s' % suffix ] },
    )
