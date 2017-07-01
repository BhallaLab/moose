# -*- coding: utf-8 -*-
"""
Test SBML capabilities of PyMOOSE
"""

__author__           = "Dilawar Singh, HarshaRani"
__copyright__        = "Copyright 2015, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os

import moose
import moose.utils as mu

# the model lives in the same directory as the test script
modeldir = os.path.dirname( os.path.realpath( __file__ ) )

def main():
    modelname = os.path.join(modeldir, './chem_models/00001-sbml-l3v1.xml' )
    model = moose.mooseReadSBML( modelname, '/sbml' )
    print( model )
    c = moose.Clock('/clock')
    moose.reinit()
    moose.start(200)
    check(  )

def check( ):
    # TODO: Add more tests here.
    p = moose.wildcardFind( '/sbml/##' )
    for x in p:
        print( x )

if __name__ == '__main__':
    main()
