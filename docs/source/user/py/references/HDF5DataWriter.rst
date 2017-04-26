HDF5DataWriter
--------------

.. py:class:: HDF5DataWriter

   HDF5 file writer for saving field values from multiple objects.
   Connect the `requestOut` field of this object to the `get{Fieldname}` of other objects where `fieldname` is the target value field of type double. The HDF5DataWriter collects the current values of the fields in all the targets at each time step in a local buffer. When the buffer size exceeds `flushLimit` (default 4M), it will write the data into the HDF5 file specified in its `filename` field (default moose\_output.h5). You can explicitly force writing by calling the `flush` function.
   The dataset location in the output file replicates the MOOSE element tree structure. Thus, if you record the Vm field from `/model[0]/neuron[0]/soma[0], the dataset path will be `/model[0]/neuron[0]/soma[0]/vm`
   NOTE: The output file remains open until this object is destroyed, or `close()` is called explicitly.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive process and reinit


   .. py:method:: setFlushLimit

      (*destination message field*)      Assigns field value.


   .. py:method:: getFlushLimit

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handle process calls. Gets data from connected fields into a local buffer and dumps them to `filename` if the buffer length exceeds `flushLimit`


   .. py:method:: reinit

      (*destination message field*)      Reinitialize the object. If the current file handle is valid, it tries to close that and open the file specified in current filename field.


   .. py:attribute:: requestOut

      PSt6vectorIdSaIdEE (*source message field*)      Sends request for a field to target object


   .. py:attribute:: flushLimit

      unsigned int (*value field*)      Buffer size limit for flushing the data from memory to file. Default is 4M doubles.
