"""
How to use the documentation
----------------------------

MOOSE documentation is split into Python documentation and builtin
documentation. The functions and classes that are only part of the
Python interface can be viewed via Python's builtin ``help``
function::

>>> help(moose.connect)

The documentation built into main C++ code of MOOSE can be accessed
via the module function ``doc``::

>>> moose.doc('Neutral')

To get documentation about a particular field::

>>> moose.doc('Neutral.childMsg')

Builtin functions and classes in moose module (Python only)
-----------------------------------------------------------

"""
from .moose import *

# Wrapper to get moose version information.
__version__ = moose._moose.__version__
VERSION = moose._moose.VERSION
