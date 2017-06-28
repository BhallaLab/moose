Synapse
-------

.. py:class:: Synapse

   Synapse using ring buffer for events.

   .. py:method:: setWeight

      (*destination message field*)      Assigns field value.


   .. py:method:: getWeight

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDelay

      (*destination message field*)      Assigns field value.


   .. py:method:: getDelay

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: addSpike

      (*destination message field*)      Handles arriving spike messages, inserts into event queue.


   .. py:attribute:: weight

      double (*value field*)      Synaptic weight


   .. py:attribute:: delay

      double (*value field*)      Axonal propagation delay to this synapse
