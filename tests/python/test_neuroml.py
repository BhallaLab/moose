"""ca1_test.py: 

    Testing scripts for CA1.

"""
    
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
# the model lives in the same directory as the test script
modeldir = os.path.dirname(__file__)

from neuroml.FvsI_CA1 import ca1_main, loadModel
from neuroml.CA1 import loadGran98NeuroML_L123

def test_all():
    test_ca1()
    test_gran()

def test_ca1():
    p = os.path.join(modeldir, 'neuroml/cells_channels/CA1soma.morph.xml')
    loadModel(p)
    assert 10 == ca1_main(4e-13)
    assert 20 == ca1_main(8e-13)
    assert 29 == ca1_main(14e-13)
    assert 34 == ca1_main(18e-13)

def test_gran():
    p = os.path.join(modeldir, 'neuroml/CA1soma.net.xml')
    assert loadGran98NeuroML_L123(p) in [8,9]

if __name__ == '__main__':
    #unittest.main()
    test_all()
