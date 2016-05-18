"""
Test Kkit capabilities of PyMOOSE
"""
from __future__ import print_function
    
__author__           = "Dilawar Singh"
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
modeldir = os.path.dirname(__file__)

# All conc in micro-molar. MOOSE units are SI units.
genesisReference = {
        'MAPK_p.Co' : 0.00517021
        , 'PKC_dash_active.Co' : 0.0923311
        , 'nuc_MAPK_p.Co' : 2.3449e-6
        , 'tot_MAPK.Co' : 0.00051749 
        , 'MKP_dash_1.Co' : 0.00051749
        , 'PDGFR.Co' : 0.10833
        , 'PDGF.Co' : 0
        , 'tot_MKP1.Co' : 0.000316181 
        }

def main():
    modelname = os.path.join(modeldir, 'chem_models/mkp1_feedback_effects_acc4.g')
    model = moose.loadModel(modelname, '/model', 'gsl')
    tables = moose.wildcardFind('/##[TYPE=Table2]')
    records = {}
    for t in tables: 
        tabname = t.path.split('/')[-1].replace("[0]", "")
        records[tabname] = t
    moose.reinit()
    moose.start(200)
    check(records)

def check(records):
    assert len(records) > 0, "No moose.Table2 created."
    failed = False
    for tabname in records:
        if tabname in genesisReference:
            genVal =  genesisReference[tabname]
            mooseVal = 1e3 * records[tabname].vector[-1]
            error = 100.0 % ( mooseVal - genVal ) / genVal
            if abs(error) > 0.1:
                failed = True
            print("{:20}: Gensis/MOOSE: {:>10} {:>10}. %error: {}".format(
                tabname, genVal, mooseVal, error)
               )
        else:
            print("[WARN] %s not found in genesis reference" % tabname)
    if failed:
        quit(1)
    else:
        quit(0)

if __name__ == '__main__':
    main()
