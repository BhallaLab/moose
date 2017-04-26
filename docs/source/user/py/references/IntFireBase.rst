IntFireBase
-----------

.. py:class:: IntFireBase

   Base class for Integrate-and-fire compartment.

   .. py:method:: setThresh

      (*destination message field*)      Assigns field value.


   .. py:method:: getThresh

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVReset

      (*destination message field*)      Assigns field value.


   .. py:method:: getVReset

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRefractoryPeriod

      (*destination message field*)      Assigns field value.


   .. py:method:: getRefractoryPeriod

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getHasFired

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getLastEventTime

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: activation

      (*destination message field*)      Handles value of synaptic activation arriving on this object


   .. py:attribute:: spikeOut

      double (*source message field*)      Sends out spike events. The argument is the timestamp of the spike.


   .. py:attribute:: thresh

      double (*value field*)      firing threshold


   .. py:attribute:: vReset

      double (*value field*)      voltage is set to vReset after firing


   .. py:attribute:: refractoryPeriod

      double (*value field*)      Minimum time between successive spikes


   .. py:attribute:: hasFired

      bool (*value field*)      The object has fired within the last timestep


   .. py:attribute:: lastEventTime

      double (*value field*)      Timestamp of last firing.
