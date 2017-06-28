Stats
-----

.. py:class:: Stats


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: getMean

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSdev

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSum

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNum

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getWmean

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getWsdev

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getWsum

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getWnum

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWindowLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getWindowLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: input

      (*destination message field*)      Handles continuous value input as a time-series. Multiple inputs are allowed, they will be merged.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: requestOut

      PSt6vectorIdSaIdEE (*source message field*)      Sends request for a field to target object


   .. py:attribute:: mean

      double (*value field*)      Mean of all sampled values or of spike rate.


   .. py:attribute:: sdev

      double (*value field*)      Standard Deviation of all sampled values, or of rate.


   .. py:attribute:: sum

      double (*value field*)      Sum of all sampled values, or total number of spikes.


   .. py:attribute:: num

      unsigned int (*value field*)      Number of all sampled values, or total number of spikes.


   .. py:attribute:: wmean

      double (*value field*)      Mean of sampled values or of spike rate within window.


   .. py:attribute:: wsdev

      double (*value field*)      Standard Deviation of sampled values, or rate, within window.


   .. py:attribute:: wsum

      double (*value field*)      Sum of all sampled values, or total number of spikes, within window.


   .. py:attribute:: wnum

      unsigned int (*value field*)      Number of all sampled values, or total number of spikes, within window.


   .. py:attribute:: windowLength

      unsigned int (*value field*)      Number of bins for windowed stats. Ignores windowing if this value is zero.
