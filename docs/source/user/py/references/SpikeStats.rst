SpikeStats
----------

.. py:class:: SpikeStats

   Object to do some minimal stats on rate of a spike train. Derived from the Stats object and returns the same set of stats.Can take either predigested spike event input, or can handle a continuous sampling of membrane potential Vm and decide if a spike has occured based on a threshold.

   .. py:method:: setThreshold

      (*destination message field*)      Assigns field value.


   .. py:method:: getThreshold

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: addSpike

      (*destination message field*)      Handles spike event time input, converts into a rate to do stats upon.


   .. py:method:: Vm

      (*destination message field*)      Handles continuous voltage input, can be coming in much than update rate of the SpikeStats. Looks for transitions above threshold to register the arrival of a spike. Doesn't do another spike till Vm falls below threshold.


   .. py:attribute:: threshold

      double (*value field*)      Spiking threshold. If Vm crosses this going up then the SpikeStats object considers that a spike has happened and adds it to the stats.
