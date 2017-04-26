PostMaster
----------

.. py:class:: PostMaster


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: getNumNodes

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMyNode

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setBufferSize

      (*destination message field*)      Assigns field value.


   .. py:method:: getBufferSize

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: numNodes

      unsigned int (*value field*)      Returns number of nodes that simulation runs on.


   .. py:attribute:: myNode

      unsigned int (*value field*)      Returns index of current node.


   .. py:attribute:: bufferSize

      unsigned int (*value field*)      Size of the send a receive buffers for each node.
