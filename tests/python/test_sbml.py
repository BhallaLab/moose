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
from moose.SBML import *
# the model lives in the same directory as the test script
modeldir = os.path.dirname(__file__)

def main():
    modelname = os.path.join(modeldir, 'chem_models/00001-sbml-l3v1.xml')
    model = mooseReadSBML(modelname, '/sbml')
    tables = moose.wildcardFind('/sbml/##[TYPE=Table2]')
    records = {}
    for t in tables: records[t.path.split('/')[-1]] = t
    c = moose.Clock('/clock')
    moose.reinit()
    moose.start(200)
    #check(tables)

def check(tables):
    assert len(tables) > 0, "No moose.Table2 created."
    for t in tables:
        assert len(t.vec) > 100
    # TODO: Add more tests here.

if __name__ == '__main__':
    main()
