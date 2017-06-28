TimeTable
---------

.. py:class:: TimeTable

   TimeTable: Read in spike times from file and send out eventOut messages
   at the specified times.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setFilename

      (*destination message field*)      Assigns field value.


   .. py:method:: getFilename

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMethod

      (*destination message field*)      Assigns field value.


   .. py:method:: getMethod

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getState

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handle process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: eventOut

      double (*source message field*)      Sends out spike time if it falls in current timestep.


   .. py:attribute:: filename

      string (*value field*)      File to read lookup data from. The file should be contain two columns
      separated by any space character.


   .. py:attribute:: method

      int (*value field*)      Method to use for filling up the entries. Currently only method 4
      (loading from file) is supported.


   .. py:attribute:: state

      double (*value field*)      Current state of the time table.
