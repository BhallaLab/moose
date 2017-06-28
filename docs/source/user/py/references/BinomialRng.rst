BinomialRng
-----------

.. py:class:: BinomialRng

   Binomially distributed random number generator.

   .. py:method:: setN

      (*destination message field*)      Assigns field value.


   .. py:method:: getN

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setP

      (*destination message field*)      Assigns field value.


   .. py:method:: getP

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: n

      double (*value field*)      Parameter n of the binomial distribution. In a coin toss experiment, this is the number of tosses.


   .. py:attribute:: p

      double (*value field*)      Parameter p of the binomial distribution. In a coin toss experiment, this is the probability of one of the two sides of the coin being on top.
