ExIF
----

.. py:class:: ExIF

   Leaky Integrate-and-Fire neuron with Exponential spike rise.Rm*Cm dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I

   .. py:method:: setDeltaThresh

      (*destination message field*)      Assigns field value.


   .. py:method:: getDeltaThresh

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVPeak

      (*destination message field*)      Assigns field value.


   .. py:method:: getVPeak

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: deltaThresh

      double (*value field*)      Parameter in Vm evolution equation:Rm*Cm * dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I


   .. py:attribute:: vPeak

      double (*value field*)      Vm is reset on reaching vPeak, different from spike thresh below:Rm*Cm dVm/dt = -(Vm-Em) + deltaThresh * exp((Vm-thresh)/deltaThresh) + Rm*I
