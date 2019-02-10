# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

# Use this format in all logger inside logger. Define it before any moose
# related module is imported.
LOGGING_FORMAT = '%(asctime)s %(message)s'

# Bring everything from c++ module to global namespace. Not everything is
# imported by the pervios import statement.
from moose._moose import *

# Bring everything from moose.py to global namespace. 
# IMP: It will overwrite any c++ function with the same name. 
from moose.moose import *
from moose.server import *

# create a shorthand for version() call here.
__version__ = version()
