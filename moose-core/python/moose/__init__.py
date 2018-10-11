# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

# Bring everything from moose.py to global namespace.
from moose.moose import *

# Bring everything from c++ module to global namespace. Not everything is
# imported by the pervios import statement.
from moose._moose import *

# create a shorthand for version() call here.
__version__ = version()

