DiagonalMsg
-----------

.. py:class:: DiagonalMsg


   .. py:method:: setStride

      (*destination message field*)      Assigns field value.


   .. py:method:: getStride

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: stride

      int (*value field*)      The stride is the increment to the src DataId that gives thedest DataId. It can be positive or negative, but bounds checkingtakes place and it does not wrap around.
