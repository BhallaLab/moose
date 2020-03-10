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

import os
import moose

print( 'Using moose form %s' % moose.__file__ )
print( '\t Moose version %s' % moose.version( ) )

# the model lives in the same directory as the test script
sdir_ = os.path.dirname( os.path.realpath( __file__ ) )

def test_sbml():
    modelname = os.path.join(sdir_, '..', 'data', '00001-sbml-l3v1.xml')
    model = moose.mooseReadSBML( modelname, '/sbml' )
    if not model:
        print("Most likely python-libsbml is not installed.")
        return 0
    moose.reinit()
    moose.start(200)
    # TODO: Add more tests here.
    p = moose.wildcardFind( '/sbml/##' )
    assert len(p) == 8
    names = ['compartment', 'mesh', 'S1', 'info', 'S2', 'info', 'reaction1']
    for x in p:
        assert x.name in names

def main():
    test_sbml()

if __name__ == '__main__':
    main()
