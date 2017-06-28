HDF5WriterBase
--------------

.. py:class:: HDF5WriterBase

   HDF5 file writer base class. This is not to be used directly. Instead, it should be subclassed to provide specific data writing functions. This class provides most basic properties like filename, file opening mode, file open status.

   .. py:method:: setFilename

      (*destination message field*)      Assigns field value.


   .. py:method:: getFilename

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIsOpen

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMode

      (*destination message field*)      Assigns field value.


   .. py:method:: getMode

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setChunkSize

      (*destination message field*)      Assigns field value.


   .. py:method:: getChunkSize

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCompressor

      (*destination message field*)      Assigns field value.


   .. py:method:: getCompressor

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCompression

      (*destination message field*)      Assigns field value.


   .. py:method:: getCompression

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setStringAttr

      (*destination message field*)      Assigns field value.


   .. py:method:: getStringAttr

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDoubleAttr

      (*destination message field*)      Assigns field value.


   .. py:method:: getDoubleAttr

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLongAttr

      (*destination message field*)      Assigns field value.


   .. py:method:: getLongAttr

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setStringVecAttr

      (*destination message field*)      Assigns field value.


   .. py:method:: getStringVecAttr

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDoubleVecAttr

      (*destination message field*)      Assigns field value.


   .. py:method:: getDoubleVecAttr

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLongVecAttr

      (*destination message field*)      Assigns field value.


   .. py:method:: getLongVecAttr

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: flush

      (*destination message field*)      Write all buffer contents to file and clear the buffers.


   .. py:method:: close

      (*destination message field*)      Close the underlying file. This is a safety measure so that file is not in an invalid state even if a crash happens at exit.


   .. py:attribute:: filename

      string (*value field*)      Name of the file associated with this HDF5 writer object.


   .. py:attribute:: isOpen

      bool (*value field*)      True if this object has an open file handle.


   .. py:attribute:: mode

      unsigned int (*value field*)      Depending on mode, if file already exists, if mode=1, data will be appended to existing file, if mode=2, file will be truncated, if  mode=4, no writing will happen.


   .. py:attribute:: chunkSize

      unsigned int (*value field*)      Chunksize for writing array data. Defaults to 100.


   .. py:attribute:: compressor

      string (*value field*)      Compression type for array data. zlib and szip are supported. Defaults to zlib.


   .. py:attribute:: compression

      unsigned int (*value field*)      Compression level for array data. Defaults to 6.


   .. py:attribute:: stringAttr

      string,string (*lookup field*)      String attributes. The key is attribute name, value is attribute value (string).


   .. py:attribute:: doubleAttr

      string,double (*lookup field*)      Double precision floating point attributes. The key is attribute name, value is attribute value (double).


   .. py:attribute:: longAttr

      string,long (*lookup field*)      Long integer attributes. The key is attribute name, value is attribute value (long).


   .. py:attribute:: stringVecAttr

      string,vector<string> (*lookup field*)      String vector attributes. The key is attribute name, value is attribute value (string).


   .. py:attribute:: doubleVecAttr

      string,vector<double> (*lookup field*)      Double vector attributes. The key is attribute name, value is attribute value (vector of double).


   .. py:attribute:: longVecAttr

      string,vector<long> (*lookup field*)      Long integer vector attributes. The key is attribute name, value is attribute value (vector of long).
