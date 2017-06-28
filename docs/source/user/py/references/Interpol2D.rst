Interpol2D
----------

.. py:class:: Interpol2D

   Interpol2D: Interpolation class. Handles lookup from a 2-dimensional grid of real-numbered values. Returns 'z' value based on given 'x' and 'y' values. Can either use interpolation or roundoff to the nearest index.

   .. py:attribute:: lookupReturn2D

      void (*shared message field*)      This is a shared message for doing lookups on the table. Receives 2 doubles: x, y. Sends back a double with the looked-up z value.


   .. py:method:: lookup

      (*destination message field*)      Looks up table value based on indices v1 and v2, and sendsvalue back using the 'lookupOut' message


   .. py:method:: setXmin

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmax

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXdivs

      (*destination message field*)      Assigns field value.


   .. py:method:: getXdivs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDx

      (*destination message field*)      Assigns field value.


   .. py:method:: getDx

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYmin

      (*destination message field*)      Assigns field value.


   .. py:method:: getYmin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYmax

      (*destination message field*)      Assigns field value.


   .. py:method:: getYmax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYdivs

      (*destination message field*)      Assigns field value.


   .. py:method:: getYdivs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDy

      (*destination message field*)      Assigns field value.


   .. py:method:: getDy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTable

      (*destination message field*)      Assigns field value.


   .. py:method:: getTable

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getZ

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTableVector2D

      (*destination message field*)      Assigns field value.


   .. py:method:: getTableVector2D

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: lookupOut

      double (*source message field*)      respond to a request for a value lookup


   .. py:attribute:: xmin

      double (*value field*)      Minimum value for x axis of lookup table


   .. py:attribute:: xmax

      double (*value field*)      Maximum value for x axis of lookup table


   .. py:attribute:: xdivs

      unsigned int (*value field*)      # of divisions on x axis of lookup table


   .. py:attribute:: dx

      double (*value field*)      Increment on x axis of lookup table


   .. py:attribute:: ymin

      double (*value field*)      Minimum value for y axis of lookup table


   .. py:attribute:: ymax

      double (*value field*)      Maximum value for y axis of lookup table


   .. py:attribute:: ydivs

      unsigned int (*value field*)      # of divisions on y axis of lookup table


   .. py:attribute:: dy

      double (*value field*)      Increment on y axis of lookup table


   .. py:attribute:: tableVector2D

      vector< vector<double> > (*value field*)      Get the entire table.


   .. py:attribute:: table

      vector<unsigned int>,double (*lookup field*)      Lookup an entry on the table


   .. py:attribute:: z

      vector<double>,double (*lookup field*)      Interpolated value for specified x and y. This is provided for debugging. Normally other objects will retrieve interpolated values via lookup message.
