SparseMsg
---------

.. py:class:: SparseMsg


   .. py:method:: getNumRows

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumColumns

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumEntries

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setProbability

      (*destination message field*)      Assigns field value.


   .. py:method:: getProbability

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSeed

      (*destination message field*)      Assigns field value.


   .. py:method:: getSeed

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRandomConnectivity

      (*destination message field*)      Assigns connectivity with specified probability and seed


   .. py:method:: setEntry

      (*destination message field*)      Assigns single row,column value


   .. py:method:: unsetEntry

      (*destination message field*)      Clears single row,column entry


   .. py:method:: clear

      (*destination message field*)      Clears out the entire matrix


   .. py:method:: transpose

      (*destination message field*)      Transposes the sparse matrix


   .. py:method:: pairFill

      (*destination message field*)      Fills entire matrix using pairs of (x,y) indices to indicate presence of a connection. If the target is a FieldElement itautomagically assigns FieldIndices.


   .. py:method:: tripletFill

      (*destination message field*)      Fills entire matrix using triplets of (x,y,fieldIndex) to fully specify every connection in the sparse matrix.


   .. py:attribute:: numRows

      unsigned int (*value field*)      Number of rows in matrix.


   .. py:attribute:: numColumns

      unsigned int (*value field*)      Number of columns in matrix.


   .. py:attribute:: numEntries

      unsigned int (*value field*)      Number of Entries in matrix.


   .. py:attribute:: probability

      double (*value field*)      connection probability for random connectivity.


   .. py:attribute:: seed

      long (*value field*)      Random number seed for generating probabilistic connectivity.
