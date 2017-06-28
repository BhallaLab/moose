References
==========

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

.. toctree::
   :maxdepth: 2

   moose_functions
   moose_classes
