DifShell
--------

.. py:class:: DifShell

   DifShell object: Models diffusion of an ion (typically calcium) within an electric compartment. A DifShell is an iso-concentration region with respect to the ion. Adjoining DifShells exchange flux of this ion, and also keep track of changes in concentration due to pumping, buffering and channel currents, by talking to the appropriate objects.

   .. py:attribute:: process\_0

      void (*shared message field*)      Here we create 2 shared finfos to attach with the Ticks. This is because we want to perform DifShell computations in 2 stages, much as in the Compartment object. In the first stage we send out the concentration value to other DifShells and Buffer elements. We also receive fluxes and currents and sum them up to compute ( dC / dt ). In the second stage we find the new C value using an explicit integration method. This 2-stage procedure eliminates the need to store and send prev\_C values, as was common in GENESIS.


   .. py:attribute:: process\_1

      void (*shared message field*)      Second process call


   .. py:attribute:: buffer

      void (*shared message field*)      This is a shared message from a DifShell to a Buffer (FixBuffer or DifBuffer). During stage 0:
        - DifShell sends ion concentration
       - Buffer updates buffer concentration and sends it back immediately using a call-back.
       - DifShell updates the time-derivative ( dC / dt )

      During stage 1:
       - DifShell advances concentration C

      This scheme means that the Buffer does not need to be scheduled, and it does its computations when it receives a cue from the DifShell. May not be the best idea, but it saves us from doing the above computations in 3 stages instead of 2.


   .. py:attribute:: innerDif

      void (*shared message field*)      This shared message (and the next) is between DifShells: adjoining shells exchange information to find out the flux between them. Using this message, an inner shell sends to, and receives from its outer shell.


   .. py:attribute:: outerDif

      void (*shared message field*)      Using this message, an outer shell sends to, and receives from its inner shell.


   .. py:method:: getC

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCeq

      (*destination message field*)      Assigns field value.


   .. py:method:: getCeq

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setD

      (*destination message field*)      Assigns field value.


   .. py:method:: getD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setValence

      (*destination message field*)      Assigns field value.


   .. py:method:: getValence

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLeak

      (*destination message field*)      Assigns field value.


   .. py:method:: getLeak

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setShapeMode

      (*destination message field*)      Assigns field value.


   .. py:method:: getShapeMode

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiameter

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiameter

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setThickness

      (*destination message field*)      Assigns field value.


   .. py:method:: getThickness

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVolume

      (*destination message field*)      Assigns field value.


   .. py:method:: getVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setOuterArea

      (*destination message field*)      Assigns field value.


   .. py:method:: getOuterArea

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInnerArea

      (*destination message field*)      Assigns field value.


   .. py:method:: getInnerArea

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Reinit happens only in stage 0


   .. py:method:: process

      (*destination message field*)      Handle process call


   .. py:method:: reinit

      (*destination message field*)      Reinit happens only in stage 0


   .. py:method:: reaction

      (*destination message field*)      Here the DifShell receives reaction rates (forward and backward), and concentrations for the free-buffer and bound-buffer molecules.


   .. py:method:: fluxFromOut

      (*destination message field*)      Destination message


   .. py:method:: fluxFromIn

      (*destination message field*)


   .. py:method:: influx

      (*destination message field*)


   .. py:method:: outflux

      (*destination message field*)


   .. py:method:: fInflux

      (*destination message field*)


   .. py:method:: fOutflux

      (*destination message field*)


   .. py:method:: storeInflux

      (*destination message field*)


   .. py:method:: storeOutflux

      (*destination message field*)


   .. py:method:: tauPump

      (*destination message field*)


   .. py:method:: eqTauPump

      (*destination message field*)


   .. py:method:: mmPump

      (*destination message field*)


   .. py:method:: hillPump

      (*destination message field*)


   .. py:attribute:: concentrationOut

      double (*source message field*)      Sends out concentration


   .. py:attribute:: innerDifSourceOut

      double,double (*source message field*)      Sends out source information.


   .. py:attribute:: outerDifSourceOut

      double,double (*source message field*)      Sends out source information.


   .. py:attribute:: C

      double (*value field*)      Concentration C is computed by the DifShell and is read-only


   .. py:attribute:: Ceq

      double (*value field*)


   .. py:attribute:: D

      double (*value field*)


   .. py:attribute:: valence

      double (*value field*)


   .. py:attribute:: leak

      double (*value field*)


   .. py:attribute:: shapeMode

      unsigned int (*value field*)


   .. py:attribute:: length

      double (*value field*)


   .. py:attribute:: diameter

      double (*value field*)


   .. py:attribute:: thickness

      double (*value field*)


   .. py:attribute:: volume

      double (*value field*)


   .. py:attribute:: outerArea

      double (*value field*)


   .. py:attribute:: innerArea

      double (*value field*)      
