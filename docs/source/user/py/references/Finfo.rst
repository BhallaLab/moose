Finfo
-----

.. py:class:: Finfo


   .. py:method:: getFieldName

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDocs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getType

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSrc

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDest

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: fieldName

      string (*value field*)      Name of field handled by Finfo


   .. py:attribute:: docs

      string (*value field*)      Documentation for Finfo


   .. py:attribute:: type

      string (*value field*)      RTTI type info for this Finfo


   .. py:attribute:: src

      vector<string> (*value field*)      Subsidiary SrcFinfos. Useful for SharedFinfos


   .. py:attribute:: dest

      vector<string> (*value field*)      Subsidiary DestFinfos. Useful for SharedFinfos
