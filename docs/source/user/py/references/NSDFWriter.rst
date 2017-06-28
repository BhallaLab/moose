NSDFWriter
----------

.. py:class:: NSDFWriter

   NSDF file writer for saving data.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive process and reinit


   .. py:method:: setNumEventInput

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumEventInput

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handle process calls. Collects data in buffer and if number of steps since last write exceeds flushLimit, writes to file.


   .. py:method:: reinit

      (*destination message field*)      Reinitialize the object. If the current file handle is valid, it tries to close that and open the file specified in current filename field.
