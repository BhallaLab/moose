SynHandlerBase
--------------

.. py:class:: SynHandlerBase

   Base class for handling synapse arrays converging onto a given channel or integrate-and-fire neuron. This class provides the interface for channels/intFires to connect to a range of synapse types, including simple synapses, synapses with different plasticity rules, and variants yet to be implemented.

   .. py:attribute:: proc

      void (*shared message field*)      Shared Finfo to receive Process messages from the clock.


   .. py:method:: setNumSynapses

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumSynapses

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles 'process' call. Checks if any spike events are due forhandling at this timestep, and does learning rule stuff if needed


   .. py:method:: reinit

      (*destination message field*)      Handles 'reinit' call. Initializes all the synapses.


   .. py:attribute:: activationOut

      double (*source message field*)      Sends out level of activation on all synapses converging to this SynHandler


   .. py:attribute:: numSynapses

      unsigned int (*value field*)      Number of synapses on SynHandler. Duplicate field for num\_synapse
