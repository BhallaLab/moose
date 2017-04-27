RandSpike
---------

.. py:class:: RandSpike

   RandSpike object, generates random spikes at.specified mean rate. Based closely on GENESIS randspike.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive Process message from scheduler


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: setRate

      (*destination message field*)      Assigns field value.


   .. py:method:: getRate

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRefractT

      (*destination message field*)      Assigns field value.


   .. py:method:: getRefractT

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAbs\_refract

      (*destination message field*)      Assigns field value.


   .. py:method:: getAbs\_refract

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getHasFired

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: spikeOut

      double (*source message field*)      Sends out a trigger for an event.


   .. py:attribute:: rate

      double (*value field*)      Specifies rate for random spike train. Note that this isprobabilistic, so the instantaneous rate may differ. If the rate is assigned be message and it varies slowly then the average firing rate will approach the specified rate


   .. py:attribute:: refractT

      double (*value field*)      Refractory Time.


   .. py:attribute:: abs\_refract

      double (*value field*)      Absolute refractory time. Synonym for refractT.


   .. py:attribute:: hasFired

      bool (*value field*)      True if RandSpike has just fired
