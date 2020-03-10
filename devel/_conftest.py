# -*- coding: utf-8 -*-

# Top level file to run and generate report.

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2019-, Dilawar Singh"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"

import pytest
try:
    import reload
except ImportError:
    from importlib import reload

@pytest.fixture(scope="module", autouse=True)
def reload_moose():
    import moose
    print('reloading moose')
    for p in ['/model', '/library']:
        if moose.exists(p):
            moose.delete(p)
    # If someone has changed the clock-ticks then we need to reset them. 
    # And relaod just to make sure and other values are initialized. 
    reload(moose)
