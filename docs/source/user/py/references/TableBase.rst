TableBase
---------

.. py:class:: TableBase


   .. py:method:: setVector

      (*destination message field*)      Assigns field value.


   .. py:method:: getVector

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getOutputValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSize

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getY

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: linearTransform

      (*destination message field*)      Linearly scales and offsets data. Scale first, then offset.


   .. py:method:: xplot

      (*destination message field*)      Dumps table contents to xplot-format file. Argument 1 is filename, argument 2 is plotname


   .. py:method:: plainPlot

      (*destination message field*)      Dumps table contents to single-column ascii file. Uses scientific notation. Argument 1 is filename


   .. py:method:: loadCSV

      (*destination message field*)      Reads a single column from a CSV file. Arguments: filename, column#, starting row#, separator


   .. py:method:: loadXplot

      (*destination message field*)      Reads a single plot from an xplot file. Arguments: filename, plotnameWhen the file has 2 columns, the 2nd column is loaded.


   .. py:method:: loadXplotRange

      (*destination message field*)      Reads a single plot from an xplot file, and selects a subset of points from it. Arguments: filename, plotname, startindex, endindexUses C convention: startindex included, endindex not included.When the file has 2 columns, the 2nd column is loaded.


   .. py:method:: compareXplot

      (*destination message field*)      Reads a plot from an xplot file and compares with contents of TableBase.Result is put in 'output' field of table.If the comparison fails (e.g., due to zero entries), the return value is -1.Arguments: filename, plotname, comparison\_operationOperations: rmsd (for RMSDifference), rmsr (RMSratio ), dotp (Dot product, not yet implemented).


   .. py:method:: compareVec

      (*destination message field*)      Compares contents of TableBase with a vector of doubles.Result is put in 'output' field of table.If the comparison fails (e.g., due to zero entries), the return value is -1.Arguments: Other vector, comparison\_operationOperations: rmsd (for RMSDifference), rmsr (RMSratio ), dotp (Dot product, not yet implemented).


   .. py:method:: clearVec

      (*destination message field*)      Handles request to clear the data vector


   .. py:attribute:: vector

      vector<double> (*value field*)      vector with all table entries


   .. py:attribute:: outputValue

      double (*value field*)      Output value holding current table entry or output of a calculation


   .. py:attribute:: size

      unsigned int (*value field*)      size of table. Note that this is the number of x divisions +1since it must represent the largest value as well as thesmallest


   .. py:attribute:: y

      unsigned int,double (*lookup field*)      Value of table at specified index
