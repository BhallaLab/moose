"""
Copyright (C) 2014 Dilawar Singh

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.

"""

"""main.py: 

Last modified: Sat Jan 18, 2014  05:01PM

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
import inspect
from utility import xml_parser

def parseMumbl(mumblFilePath):
    """Parse MUMBL are return the ElementTree """
    return xml_parser.parseWithoutValidation("mumbl", mumblFilePath)
    

def main():
    # standard module for building a command line parser.
    import argparse

    # This section build the command line parser
    argParser = argparse.ArgumentParser(description= 'Mutiscale modelling of neurons')
    argParser.add_argument('--mumbl', metavar='mumbl', required = True, nargs = '+'
        , help = 'Lanaguge to do multi-scale modelling in moose'
        )
    args = argParser.parse_args()

if __name__ == '__main__':
    main()
