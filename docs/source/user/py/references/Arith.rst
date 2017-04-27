Arith
-----

.. py:class:: Arith


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setFunction

      (*destination message field*)      Assigns field value.


   .. py:method:: getFunction

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setOutputValue

      (*destination message field*)      Assigns field value.


   .. py:method:: getOutputValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getArg1Value

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAnyValue

      (*destination message field*)      Assigns field value.


   .. py:method:: getAnyValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: arg1

      (*destination message field*)      Handles argument 1. This just assigns it


   .. py:method:: arg2

      (*destination message field*)      Handles argument 2. This just assigns it


   .. py:method:: arg3

      (*destination message field*)      Handles argument 3. This sums in each input, and clears each clock tick.


   .. py:method:: arg1x2

      (*destination message field*)      Store the product of the two arguments in output\_


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: output

      double (*source message field*)      Sends out the computed value


   .. py:attribute:: function

      string (*value field*)      Arithmetic function to perform on inputs.


   .. py:attribute:: outputValue

      double (*value field*)      Value of output as computed last timestep.


   .. py:attribute:: arg1Value

      double (*value field*)      Value of arg1 as computed last timestep.


   .. py:attribute:: anyValue

      unsigned int,double (*lookup field*)      Value of any of the internal fields, output, arg1, arg2, arg3,as specified by the index argument from 0 to 3.
