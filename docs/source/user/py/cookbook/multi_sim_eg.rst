***************
Simple Examples
***************

.. hidden-code-block:: reStructuredText
   :label: How to run these examples

   Each of the following examples can be run by clicking on the green source button
   on the right side of each example, and running from within a ``.py`` python file
   on a computer where moose is installed.

   Alternatively, all the files mentioned on this page can be found in the main
   moose directory. They can be found under 

       (...)/moose/moose-examples/snippets

   They can be run by typing 

       $ python filename.py

   in your command line, where filename.py is the python file you want to run.

   All of the following examples show one or more methods within each python file.
   For example, in the ``cubeMeshSigNeur`` section, there are two blue tabs
   describing the ``cubeMeshSigNeur.createSquid()`` and ``cubeMeshSigNeur.main()``
   methods.

   The filename is the bit that comes before the ``.`` in the blue boxes, with
   ``.py`` added at the end of it. In this case, the file name would be
   ``cubeMeshSigNeur.py``.
|

Single-compartment multiscale model
-----------------------------------

.. automodule:: multiscaleOneCompt
   :members:

Multi compartment Single Neuron Model
-------------------------------------

.. automodule:: multiComptSigNeur
   :members:

Modeling chemical reactions in neurons
--------------------------------------

.. automodule:: gssaRDspiny
   :members:

.. automodule: rxdSpineSize
   :members:
   (rxdSpineSize does not exist in any appended folders -> gives error while building html)
