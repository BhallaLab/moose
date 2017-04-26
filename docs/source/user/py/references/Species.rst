Species
-------

.. py:class:: Species


   .. py:attribute:: pool

      void (*shared message field*)      Connects to pools of this Species type


   .. py:method:: setMolWt

      (*destination message field*)      Assigns field value.


   .. py:method:: getMolWt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: handleMolWtRequest

      (*destination message field*)      Handle requests for molWt.


   .. py:attribute:: molWtOut

      double (*source message field*)      returns molWt.


   .. py:attribute:: molWt

      double (*value field*)      Molecular weight of species
