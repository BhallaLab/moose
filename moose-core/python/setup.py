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

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013-17, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPLv3"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"

import os
from distutils.core import setup

script_dir = os.path.dirname( os.path.abspath( __file__ ) )

version = '3.1.2'

try:
    import importlib.machinery
    suffix = importlib.machinery.EXTENSION_SUFFIXES[-1]
except Exception as e:
    suffix = '.so'

print( 'Python module suffix is %s' % suffix )

setup(
        name='moose',
        version=version,
        description='MOOSE python scripting module.',
        author='See AUTHORS file',
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
        package_dir = { 
            'moose' : 'moose' 
            , 'rdesigneur' : 'rdesigneur'
            },
        package_data = { 'moose' : ['_moose' + suffix] },
    ) 
