#!/usr/bin/env python

"""deploy_gh_pages.py: 

Created markdown file and run jekyll.

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2016, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import subprocess

indexMd = 'index.md'

failedTxt_ = ''
if os.path.isfile( 'FAILED' ):
    with open( 'FAILED' ) as f:
        failedTxt_ = f.read( )

def write_to_file( filename, lines ):
    with open( filename, 'a' ) as f:
        f.write( '\n'.join( lines ) )

def create_static_site( ):
    subprocess.call( [ 'jekyll', 'b' ], shell = False )

def main( ):
    global failedTxt_
    write_to_file( indexMd, [ '# Following scripts failed ', failedTxt_ ] )
    create_static_site( )



if __name__ == '__main__':
    main()
