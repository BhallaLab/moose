DiffAmp
-------

.. py:class:: DiffAmp

   A difference amplifier. Output is the difference between the total plus inputs and the total minus inputs multiplied by gain. Gain can be set statically as a field or can be a destination message and thus dynamically determined by the output of another object. Same as GENESIS diffamp object.

   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: setGain

      (*destination message field*)      Assigns field value.


   .. py:method:: getGain

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSaturation

      (*destination message field*)      Assigns field value.


   .. py:method:: getSaturation

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getOutputValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: gainIn

      (*destination message field*)      Destination message to control gain dynamically.


   .. py:method:: plusIn

      (*destination message field*)      Positive input terminal of the amplifier. All the messages connected here are summed up to get total positive input.


   .. py:method:: minusIn

      (*destination message field*)      Negative input terminal of the amplifier. All the messages connected here are summed up to get total positive input.


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: output

      double (*source message field*)      Current output level.


   .. py:attribute:: gain

      double (*value field*)      Gain of the amplifier. The output of the amplifier is the difference between the totals in plus and minus inputs multiplied by the gain. Defaults to 1


   .. py:attribute:: saturation

      double (*value field*)      Saturation is the bound on the output. If output goes beyond the +/-saturation range, it is truncated to the closer of +saturation and -saturation. Defaults to the maximum double precision floating point number representable on the system.


   .. py:attribute:: outputValue

      double (*value field*)      Output of the amplifier, i.e. gain * (plus - minus).
