SpikeGen
--------

.. py:class:: SpikeGen

   SpikeGen object, for detecting threshold crossings.The threshold detection can work in multiple modes.
    If the refractT < 0.0, then it fires an event only at the rising edge of the input voltage waveform

   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive Process message from scheduler


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: Vm

      (*destination message field*)      Handles Vm message coming in from compartment


   .. py:method:: setThreshold

      (*destination message field*)      Assigns field value.


   .. py:method:: getThreshold

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


   .. py:method:: setEdgeTriggered

      (*destination message field*)      Assigns field value.


   .. py:method:: getEdgeTriggered

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: spikeOut

      double (*source message field*)      Sends out a trigger for an event.


   .. py:attribute:: threshold

      double (*value field*)      Spiking threshold, must cross it going up


   .. py:attribute:: refractT

      double (*value field*)      Refractory Time.


   .. py:attribute:: abs\_refract

      double (*value field*)      Absolute refractory time. Synonym for refractT.


   .. py:attribute:: hasFired

      bool (*value field*)      True if SpikeGen has just fired


   .. py:attribute:: edgeTriggered

      bool (*value field*)      When edgeTriggered = 0, the SpikeGen will fire an event in each timestep while incoming Vm is > threshold and at least abs\_refracttime has passed since last event. This may be problematic if the incoming Vm remains above threshold for longer than abs\_refract. Setting edgeTriggered to 1 resolves this as the SpikeGen generatesan event only on the rising edge of the incoming Vm and will remain idle unless the incoming Vm goes below threshold.


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
