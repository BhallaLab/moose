Streamer
--------

.. py:class:: Streamer

   Streamer: Stream moose.Table data to out-streams

   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setOutfile

      (*destination message field*)      Assigns field value.


   .. py:method:: getOutfile

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setFormat

      (*destination message field*)      Assigns field value.


   .. py:method:: getFormat

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handle process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: addTable

      (*destination message field*)      Add a table to Streamer


   .. py:method:: addTables

      (*destination message field*)      Add many tables to Streamer


   .. py:method:: removeTable

      (*destination message field*)      Remove a table from Streamer


   .. py:method:: removeTables

      (*destination message field*)      Remove tables -- if found -- from Streamer


   .. py:method:: getNumTables

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: outfile

      string (*value field*)      File/stream to write table data to. Default is is \_\_moose\_tables\_\_.dat.n By default, this object writes data every second



   .. py:attribute:: format

      string (*value field*)      Format of output file, default is csv


   .. py:attribute:: numTables

      unsigned long (*value field*)      Number of Tables handled by Streamer
