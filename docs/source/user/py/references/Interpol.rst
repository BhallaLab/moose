Interpol
--------

.. py:class:: Interpol

   Interpol: Interpolation class. Handles lookup from a 1-dimensional array of real-numbered values.Returns 'y' value based on given 'x' value. Can either use interpolation or roundoff to the nearest index.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setXmin

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmax

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getY

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: input

      (*destination message field*)      Interpolates using the input as x value.


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: lookupOut

      double (*source message field*)      respond to a request for a value lookup


   .. py:attribute:: xmin

      double (*value field*)      Minimum value of x. x below this will result in y[0] being returned.


   .. py:attribute:: xmax

      double (*value field*)      Maximum value of x. x above this will result in y[last] being returned.


   .. py:attribute:: y

      double (*value field*)      Looked up value.
