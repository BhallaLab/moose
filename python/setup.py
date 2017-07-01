# -*- coding: utf-8 -*-
# This file is part of MOOSE simulator: http://moose.ncbs.res.in.

# MOOSE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# MOOSE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with MOOSE.  If not, see <http://www.gnu.org/licenses/>.


"""setup.py:

    Script to install python targets.

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
from distutils.core import setup

script_dir = os.path.dirname( os.path.abspath( __file__ ) )

version = '3.1'
try:
    with open( os.path.join( script_dir, '..', 'VERSION'), 'r' ) as f:
        version = f.read( )
except Exception as e:
    print( 'Failed to read VERSION %s' % e )
    print( 'Using default 3.1' )

try:
    import importlib.machinery
    suffix = importlib.machinery.EXTENSION_SUFFIXES[0]
except Exception as e:
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
            , 'moose.merge'
            ],
        package_dir = {
            'moose' : 'moose'
            , 'rdesigneur' : 'rdesigneur'
            },
        package_data = { 'moose' : ['_moose' + suffix] },
    )
