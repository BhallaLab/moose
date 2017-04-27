NormalRng
---------

.. py:class:: NormalRng

   Normally distributed random number generator.

   .. py:method:: setMean

      (*destination message field*)      Assigns field value.


   .. py:method:: getMean

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVariance

      (*destination message field*)      Assigns field value.


   .. py:method:: getVariance

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMethod

      (*destination message field*)      Assigns field value.


   .. py:method:: getMethod

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: mean

      double (*value field*)      Mean of the normal distribution


   .. py:attribute:: variance

      double (*value field*)      Variance of the normal distribution


   .. py:attribute:: method

      int (*value field*)      Algorithm used for computing the sample. The default is 0 = alias method by Ahrens and Dieter. Other options are: 1 = Box-Mueller method and 2 = ziggurat method.
