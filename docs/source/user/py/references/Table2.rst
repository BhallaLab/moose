Table2
------

.. py:class:: Table2

   Table for accumulating data values, or spike timings. Can either receive incoming doubles, or can explicitly request values from fields provided they are doubles. The latter mode of use is preferable if you wish to have independent control of how often you sample from the output variable.
   Typically used for storing simulation output into memory, or to file when stream is set to True
   There are two functionally identical variants of the Table class: Table and Table2. Their only difference is that the default scheduling of the Table (Clock Tick 8, dt = 0.1 ms ) makes it suitable for tracking electrical compartmental models of neurons and networks.
   Table2 (Clock Tick 18, dt = 1.0 s) is good for tracking biochemical signaling pathway outputs.
   These are just the default values and Tables can be assigned to any Clock Tick and timestep in the usual manner.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setThreshold

      (*destination message field*)      Assigns field value.


   .. py:method:: getThreshold

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setFormat

      (*destination message field*)      Assigns field value.


   .. py:method:: getFormat

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setName

      (*destination message field*)      Assigns field value.


   .. py:method:: getName

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setOutfile

      (*destination message field*)      Assigns field value.


   .. py:method:: getOutfile

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setUseStreamer

      (*destination message field*)      Assigns field value.


   .. py:method:: getUseStreamer

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: input

      (*destination message field*)      Fills data into table. Also handles data sent back following request


   .. py:method:: spike

      (*destination message field*)      Fills spike timings into the Table. Signal has to exceed thresh


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: requestOut

      PSt6vectorIdSaIdEE (*source message field*)      Sends request for a field to target object


   .. py:attribute:: threshold

      double (*value field*)      threshold used when Table acts as a buffer for spikes


   .. py:attribute:: format

      string (*value field*)      Data format for table: default csv


   .. py:attribute:: name

      string (*value field*)      Name of the table.


   .. py:attribute:: outfile

      string (*value field*)      Set the name of file to which data is written to. If set,  streaming support is automatically enabled.


   .. py:attribute:: useStreamer

      bool (*value field*)      When set to true, write to a file instead writing in memory. If `outfile` is not set, streamer writes to default path.
