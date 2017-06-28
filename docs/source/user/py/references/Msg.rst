Msg
---

.. py:class:: Msg


   .. py:method:: getE1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getE2

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSrcFieldsOnE1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDestFieldsOnE2

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSrcFieldsOnE2

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDestFieldsOnE1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getAdjacent

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: e1

      Id (*value field*)      Id of source Element.


   .. py:attribute:: e2

      Id (*value field*)      Id of source Element.


   .. py:attribute:: srcFieldsOnE1

      vector<string> (*value field*)      Names of SrcFinfos for messages going from e1 to e2. There arematching entries in the destFieldsOnE2 vector


   .. py:attribute:: destFieldsOnE2

      vector<string> (*value field*)      Names of DestFinfos for messages going from e1 to e2. There arematching entries in the srcFieldsOnE1 vector


   .. py:attribute:: srcFieldsOnE2

      vector<string> (*value field*)      Names of SrcFinfos for messages going from e2 to e1. There arematching entries in the destFieldsOnE1 vector


   .. py:attribute:: destFieldsOnE1

      vector<string> (*value field*)      Names of destFinfos for messages going from e2 to e1. There arematching entries in the srcFieldsOnE2 vector


   .. py:attribute:: adjacent

      ObjId,ObjId (*lookup field*)      The element adjacent to the specified element
