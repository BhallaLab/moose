# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

# Bring everything from c++ module to global namespace. 
from moose._moose import *

# Bring everything from moose.py to global namespace. 
# IMP: It will overwrite any c++ function with the same name.  We can override
# some C++ here.
from moose.moose import *
from moose.server import *

# SBML and NML2 support.
from moose.model_utils import *

# create a shorthand for version() call here.
__version__ = version()

# C++ core override
from moose.wrapper import *

# Import moose test.
from moose.moose_test import test
