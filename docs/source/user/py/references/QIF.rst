QIF
---

.. py:class:: QIF

   Leaky Integrate-and-Fire neuron with Quadratic term in Vm.Based on Spiking Neuron Models book by Gerstner and Kistler.Rm*Cm * dVm/dt = a0*(Vm-Em)*(Vm-vCritical) + Rm*I

   .. py:method:: setVCritical

      (*destination message field*)      Assigns field value.


   .. py:method:: getVCritical

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setA0

      (*destination message field*)      Assigns field value.


   .. py:method:: getA0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: vCritical

      double (*value field*)      Critical voltage for spike initiation


   .. py:attribute:: a0

      double (*value field*)      Parameter in Rm*Cm dVm/dt = a0*(Vm-Em)*(Vm-vCritical) + Rm*I, a0>0
