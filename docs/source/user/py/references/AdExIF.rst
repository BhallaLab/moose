AdExIF
------

.. py:class:: AdExIF

   Leaky Integrate-and-Fire neuron with Exponential spike rise and adaptation via an adapting current w.Rm*Cm * dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I - w tau\_w * d w /dt = a0*(Vm-Em) - w at each spike, w -> w + b0

   .. py:method:: setW

      (*destination message field*)      Assigns field value.


   .. py:method:: getW

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauW

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauW

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setA0

      (*destination message field*)      Assigns field value.


   .. py:method:: getA0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setB0

      (*destination message field*)      Assigns field value.


   .. py:method:: getB0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: w

      double (*value field*)      adaptation current with time constant tauW


   .. py:attribute:: tauW

      double (*value field*)      time constant of adaptation current w


   .. py:attribute:: a0

      double (*value field*)      factor for voltage-dependent term in evolution of adaptation current: tau\_w dw/dt = a0*(Vm-Em) - w


   .. py:attribute:: b0

      double (*value field*)      b0 is added to w, the adaptation current on each spike
