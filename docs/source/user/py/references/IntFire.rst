IntFire
-------

.. py:class:: IntFire


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setVm

      (*destination message field*)      Assigns field value.


   .. py:method:: getVm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTau

      (*destination message field*)      Assigns field value.


   .. py:method:: getTau

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setThresh

      (*destination message field*)      Assigns field value.


   .. py:method:: getThresh

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRefractoryPeriod

      (*destination message field*)      Assigns field value.


   .. py:method:: getRefractoryPeriod

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: activation

      (*destination message field*)      Handles value of synaptic activation arriving on this IntFire


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: spikeOut

      double (*source message field*)      Sends out spike events. The argument is the timestamp of the spike.


   .. py:attribute:: Vm

      double (*value field*)      Membrane potential


   .. py:attribute:: tau

      double (*value field*)      charging time-course


   .. py:attribute:: thresh

      double (*value field*)      firing threshold


   .. py:attribute:: refractoryPeriod

      double (*value field*)      Minimum time between successive spikes
