PoolBase
--------

.. py:class:: PoolBase

   Abstract base class for pools.

   .. py:attribute:: reac

      void (*shared message field*)      Connects to reaction


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:attribute:: species

      void (*shared message field*)      Shared message for connecting to species objects


   .. py:method:: setN

      (*destination message field*)      Assigns field value.


   .. py:method:: getN

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNInit

      (*destination message field*)      Assigns field value.


   .. py:method:: getNInit

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiffConst

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiffConst

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMotorConst

      (*destination message field*)      Assigns field value.


   .. py:method:: getMotorConst

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setConc

      (*destination message field*)      Assigns field value.


   .. py:method:: getConc

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setConcInit

      (*destination message field*)      Assigns field value.


   .. py:method:: getConcInit

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVolume

      (*destination message field*)      Assigns field value.


   .. py:method:: getVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSpeciesId

      (*destination message field*)      Assigns field value.


   .. py:method:: getSpeciesId

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: reacDest

      (*destination message field*)      Handles reaction input


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: handleMolWt

      (*destination message field*)      Separate finfo to assign molWt, and consequently diffusion const.Should only be used in SharedMsg with species.


   .. py:attribute:: nOut

      double (*source message field*)      Sends out # of molecules in pool on each timestep


   .. py:attribute:: requestMolWt

      void (*source message field*)      Requests Species object for mol wt


   .. py:attribute:: n

      double (*value field*)      Number of molecules in pool


   .. py:attribute:: nInit

      double (*value field*)      Initial value of number of molecules in pool


   .. py:attribute:: diffConst

      double (*value field*)      Diffusion constant of molecule


   .. py:attribute:: motorConst

      double (*value field*)      Motor transport rate molecule. + is away from soma, - is towards soma. Only relevant for ZombiePool subclasses.


   .. py:attribute:: conc

      double (*value field*)      Concentration of molecules in this pool


   .. py:attribute:: concInit

      double (*value field*)      Initial value of molecular concentration in pool


   .. py:attribute:: volume

      double (*value field*)      Volume of compartment. Units are SI. Utility field, the actual volume info is stored on a volume mesh entry in the parent compartment.This mapping is implicit: the parent compartment must be somewhere up the element tree, and must have matching mesh entries. If the compartment isn'tavailable the volume is just taken as 1


   .. py:attribute:: speciesId

      unsigned int (*value field*)      Species identifier for this mol pool. Eventually link to ontology.
