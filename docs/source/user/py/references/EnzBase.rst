EnzBase
-------

.. py:class:: EnzBase

   Abstract base class for enzymes.

   .. py:attribute:: sub

      void (*shared message field*)      Connects to substrate molecule


   .. py:attribute:: prd

      void (*shared message field*)      Connects to product molecule


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setKm

      (*destination message field*)      Assigns field value.


   .. py:method:: getKm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumKm

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumKm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setKcat

      (*destination message field*)      Assigns field value.


   .. py:method:: getKcat

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumSubstrates

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: enzDest

      (*destination message field*)      Handles # of molecules of Enzyme


   .. py:method:: subDest

      (*destination message field*)      Handles # of molecules of substrate


   .. py:method:: prdDest

      (*destination message field*)      Handles # of molecules of product. Dummy.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: remesh

      (*destination message field*)      Tells the MMEnz to recompute its numKm after remeshing


   .. py:attribute:: subOut

      double,double (*source message field*)      Sends out increment of molecules on product each timestep


   .. py:attribute:: prdOut

      double,double (*source message field*)      Sends out increment of molecules on product each timestep


   .. py:attribute:: Km

      double (*value field*)      Michaelis-Menten constant in SI conc units (milliMolar)


   .. py:attribute:: numKm

      double (*value field*)      Michaelis-Menten constant in number units, volume dependent


   .. py:attribute:: kcat

      double (*value field*)      Forward rate constant for enzyme, units 1/sec


   .. py:attribute:: numSubstrates

      unsigned int (*value field*)      Number of substrates in this MM reaction. Usually 1.Does not include the enzyme itself
