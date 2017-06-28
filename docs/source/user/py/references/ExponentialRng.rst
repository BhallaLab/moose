ExponentialRng
--------------

.. py:class:: ExponentialRng

   Exponentially distributed random number generator.
   Exponential distribution with mean k is defined by the probability density function p(x; k) = k * exp(-k * x) if x >= 0, else 0. By default this class uses the random minimization method described in Knuth's TAOCP Vol II Sec 3.4.1 (Algorithm S).

   .. py:method:: setMean

      (*destination message field*)      Assigns field value.


   .. py:method:: getMean

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMethod

      (*destination message field*)      Assigns field value.


   .. py:method:: getMethod

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: mean

      double (*value field*)      Mean of the exponential distribution.


   .. py:attribute:: method

      int (*value field*)      The algorithm to use for computing the sample. Two methods are supported: 0 - logarithmic and 1 - random minimization. The logarithmic method is slower (it computes a logarithm). Default is random minimization. See Knuth, Vol II Sec 3.4.1 : Algorithm S.
