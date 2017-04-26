SimpleSynHandler
----------------

.. py:class:: SimpleSynHandler

   The SimpleSynHandler handles simple synapses without plasticity. It uses a priority queue to manage them.

   .. py:method:: setNumSynapse

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumSynapse

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.
