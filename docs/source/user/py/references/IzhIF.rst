IzhIF
-----

.. py:class:: IzhIF

   Izhikevich neuron (integrate and fire).d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm d u / dt = a * ( b * Vm - u ) at each spike, u -> u + d by default, a0 = 0.04e6/V/s, b0 = 5e3/s, c0 = 140 V/s are set to SI units, so use SI consistently, or change a0, b0, c0 also if you wish to use other units. Rm, Em from Compartment are not used here, vReset is same as c in the usual formalism. At rest, u0 = b V0, and V0 = ( -(-b0-b) +/- sqrt((b0-b)^2 - 4*a0*c0)) / (2*a0) equivalently, to obtain resting Em, set b = (a0*Em^2 + b0*Em + c0)/Em

   .. py:method:: setA0

      (*destination message field*)      Assigns field value.


   .. py:method:: getA0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setB0

      (*destination message field*)      Assigns field value.


   .. py:method:: getB0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setC0

      (*destination message field*)      Assigns field value.


   .. py:method:: getC0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setA

      (*destination message field*)      Assigns field value.


   .. py:method:: getA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setB

      (*destination message field*)      Assigns field value.


   .. py:method:: getB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setD

      (*destination message field*)      Assigns field value.


   .. py:method:: getD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setU

      (*destination message field*)      Assigns field value.


   .. py:method:: getU

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setUInit

      (*destination message field*)      Assigns field value.


   .. py:method:: getUInit

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVPeak

      (*destination message field*)      Assigns field value.


   .. py:method:: getVPeak

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: a0

      double (*value field*)      factor for Vm^2 term in evolution equation for Vm: d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm


   .. py:attribute:: b0

      double (*value field*)      factor for Vm term in evolution equation for Vm: d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm


   .. py:attribute:: c0

      double (*value field*)      constant term in evolution equation for Vm: d Vm /dt = a0 * Vm^2 + b0 * Vm + c0 - u + I/Cm


   .. py:attribute:: a

      double (*value field*)      a as in d u / dt = a * ( b * Vm - u )


   .. py:attribute:: b

      double (*value field*)      b as in d u / dt = a * ( b * Vm - u )


   .. py:attribute:: d

      double (*value field*)      u jumps by d at every spike


   .. py:attribute:: u

      double (*value field*)      u is an adaptation variable


   .. py:attribute:: uInit

      double (*value field*)      Initial value of u. It is reset at reinit()


   .. py:attribute:: vPeak

      double (*value field*)      Vm is reset when Vm > vPeak
