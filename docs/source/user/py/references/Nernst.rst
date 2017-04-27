Nernst
------

.. py:class:: Nernst

   Nernst: Calculates Nernst potential for a given ion based on Cin and Cout, the inside and outside concentrations. Immediately sends out the potential to all targets.

   .. py:method:: getE

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTemperature

      (*destination message field*)      Assigns field value.


   .. py:method:: getTemperature

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setValence

      (*destination message field*)      Assigns field value.


   .. py:method:: getValence

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCin

      (*destination message field*)      Assigns field value.


   .. py:method:: getCin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCout

      (*destination message field*)      Assigns field value.


   .. py:method:: getCout

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setScale

      (*destination message field*)      Assigns field value.


   .. py:method:: getScale

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: ci

      (*destination message field*)      Set internal conc of ion, and immediately send out the updated E


   .. py:method:: co

      (*destination message field*)      Set external conc of ion, and immediately send out the updated E


   .. py:attribute:: Eout

      double (*source message field*)      Computed reversal potential


   .. py:attribute:: E

      double (*value field*)      Computed reversal potential


   .. py:attribute:: Temperature

      double (*value field*)      Temperature of cell


   .. py:attribute:: valence

      int (*value field*)      Valence of ion in Nernst calculation


   .. py:attribute:: Cin

      double (*value field*)      Internal conc of ion


   .. py:attribute:: Cout

      double (*value field*)      External conc of ion


   .. py:attribute:: scale

      double (*value field*)      Voltage scale factor
