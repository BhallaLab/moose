MgBlock
-------

.. py:class:: MgBlock

   MgBlock: Hodgkin-Huxley type voltage-gated Ion channel. Something like the old tabchannel from GENESIS, but also presents a similar interface as hhchan from GENESIS.

   .. py:method:: origChannel

      (*destination message field*)


   .. py:method:: setKMg\_A

      (*destination message field*)      Assigns field value.


   .. py:method:: getKMg\_A

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setKMg\_B

      (*destination message field*)      Assigns field value.


   .. py:method:: getKMg\_B

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCMg

      (*destination message field*)      Assigns field value.


   .. py:method:: getCMg

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setZk

      (*destination message field*)      Assigns field value.


   .. py:method:: getZk

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: KMg\_A

      double (*value field*)      1/eta


   .. py:attribute:: KMg\_B

      double (*value field*)      1/gamma


   .. py:attribute:: CMg

      double (*value field*)      [Mg] in mM


   .. py:attribute:: Zk

      double (*value field*)      Charge on ion
