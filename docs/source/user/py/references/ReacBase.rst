ReacBase
--------

.. py:class:: ReacBase

   Base class for reactions. Provides the MOOSE APIfunctions, but ruthlessly refers almost all of them to derivedclasses, which have to provide the man page output.

   .. py:attribute:: sub

      void (*shared message field*)      Connects to substrate pool


   .. py:attribute:: prd

      void (*shared message field*)      Connects to substrate pool


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setNumKf

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumKf

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumKb

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumKb

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setKf

      (*destination message field*)      Assigns field value.


   .. py:method:: getKf

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setKb

      (*destination message field*)      Assigns field value.


   .. py:method:: getKb

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumSubstrates

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumProducts

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: subDest

      (*destination message field*)      Handles # of molecules of substrate


   .. py:method:: prdDest

      (*destination message field*)      Handles # of molecules of product


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: subOut

      double,double (*source message field*)      Sends out increment of molecules on product each timestep


   .. py:attribute:: prdOut

      double,double (*source message field*)      Sends out increment of molecules on product each timestep


   .. py:attribute:: numKf

      double (*value field*)      Forward rate constant, in # units


   .. py:attribute:: numKb

      double (*value field*)      Reverse rate constant, in # units


   .. py:attribute:: Kf

      double (*value field*)      Forward rate constant, in concentration units


   .. py:attribute:: Kb

      double (*value field*)      Reverse rate constant, in concentration units


   .. py:attribute:: numSubstrates

      unsigned int (*value field*)      Number of substrates of reaction


   .. py:attribute:: numProducts

      unsigned int (*value field*)      Number of products of reaction
