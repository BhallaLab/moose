Variable
--------

.. py:class:: Variable

   Variable for storing double values. This is used in Function class.

   .. py:method:: setValue

      (*destination message field*)      Assigns field value.


   .. py:method:: getValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: input

      (*destination message field*)      Handles incoming variable value.


   .. py:attribute:: value

      double (*value field*)      Variable value
