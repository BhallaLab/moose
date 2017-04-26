StimulusTable
-------------

.. py:class:: StimulusTable


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setStartTime

      (*destination message field*)      Assigns field value.


   .. py:method:: getStartTime

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setStopTime

      (*destination message field*)      Assigns field value.


   .. py:method:: getStopTime

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLoopTime

      (*destination message field*)      Assigns field value.


   .. py:method:: getLoopTime

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setStepSize

      (*destination message field*)      Assigns field value.


   .. py:method:: getStepSize

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setStepPosition

      (*destination message field*)      Assigns field value.


   .. py:method:: getStepPosition

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDoLoop

      (*destination message field*)      Assigns field value.


   .. py:method:: getDoLoop

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: output

      double (*source message field*)      Sends out tabulated data according to lookup parameters.


   .. py:attribute:: startTime

      double (*value field*)      Start time used when table is emitting values. For lookupvalues below this, the table just sends out its zero entry.Corresponds to zeroth entry of table.


   .. py:attribute:: stopTime

      double (*value field*)      Time to stop emitting values.If time exceeds this, then the table sends out its last entry.The stopTime corresponds to the last entry of table.


   .. py:attribute:: loopTime

      double (*value field*)      If looping, this is the time between successive cycle starts.Defaults to the difference between stopTime and startTime, so that the output waveform cycles with precisely the same duration as the table contents.If larger than stopTime - startTime, then it pauses at the last table value till it is time to go around again.If smaller than stopTime - startTime, then it begins the next cycle even before the first one has reached the end of the table.


   .. py:attribute:: stepSize

      double (*value field*)      Increment in lookup (x) value on every timestep. If it isless than or equal to zero, the StimulusTable uses the current timeas the lookup value.


   .. py:attribute:: stepPosition

      double (*value field*)      Current value of lookup (x) value.If stepSize is less than or equal to zero, this is set tothe current time to use as the lookup value.


   .. py:attribute:: doLoop

      bool (*value field*)      Flag: Should it loop around to startTime once it has reachedstopTime. Default (zero) is to do a single pass.
