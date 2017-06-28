RandGenerator
-------------

.. py:class:: RandGenerator

   Base class for random number generators for sampling various probability distributions. This class should not be used directly. Instead, its subclasses named after specific distributions should be used.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: getSample

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMean

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getVariance

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: output

      double (*source message field*)      Generated random number.


   .. py:attribute:: sample

      double (*value field*)      Generated pseudorandom number.


   .. py:attribute:: mean

      double (*value field*)      Mean of the distribution.


   .. py:attribute:: variance

      double (*value field*)      Variance of the distribution.
