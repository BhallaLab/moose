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

Connecting two cells via a synapse
----------------------------------

Below is the connectivity diagram for setting up a synaptic connection
from one neuron to another. The PulseGen object is there for
stimulating the presynaptic cell as part of experimental setup. The
cells are defined as single-compartments with Hodgkin-Huxley type Na+
and K+ channels.

.. figure:: ../../../images/twoCells.png
   :scale: 50%
   :alt: Two cells connected via synapse

.. automodule:: twocells
   :members:

Multi Compartmental Leaky Neurons
---------------------------------

.. automodule:: multicomp_lif
   :members:

Providing random input to a cell
--------------------------------

.. automodule:: randomspike
   :members:

.. figure:: ../../../images/randomSpike.png
   :scale: 50%
   :alt: Random spike input to a cell

Plastic synapse
---------------

.. automodule:: STDP
   :members:

Synapse Handler for Spikes
--------------------------

.. automodule:: RandSpikeStats
   :members:

Recurrent integrate-and-fire network
------------------------------------

.. automodule:: recurrentIntFire
   :members:

Recurrent integrate-and-fire network with plasticity
----------------------------------------------------

.. automodule:: recurrentLIF
   :members:

Demonstration Models
--------------------

.. automodule:: compartment_net
   :members:

.. automodule:: compartment_net_no_array
   :members:

Building Models
---------------

.. automodule:: synapse_tutorial
   :members:
