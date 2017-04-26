AdThreshIF
----------

.. py:class:: AdThreshIF

   Leaky Integrate-and-Fire neuron with adaptive threshold.Based on Rossant, C., Goodman, D.F.M., Platkiewicz, J., and Brette, R. (2010).Rm*Cm * dVm/dt = -(Vm-Em) + Rm*ItauThresh * d threshAdaptive / dt = a0*(Vm-Em) - threshAdaptive at each spike, threshAdaptive is increased by threshJump the spiking threshold adapts as thresh + threshAdaptive

   .. py:method:: setThreshAdaptive

      (*destination message field*)      Assigns field value.


   .. py:method:: getThreshAdaptive

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauThresh

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauThresh

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setA0

      (*destination message field*)      Assigns field value.


   .. py:method:: getA0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setThreshJump

      (*destination message field*)      Assigns field value.


   .. py:method:: getThreshJump

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: threshAdaptive

      double (*value field*)      adaptative part of the threshold that decays with time constant tauThresh


   .. py:attribute:: tauThresh

      double (*value field*)      time constant of adaptative part of the threshold


   .. py:attribute:: a0

      double (*value field*)      factor for voltage-dependent term in evolution of adaptative threshold: tauThresh * d threshAdaptive / dt = a0*(Vm-Em) - threshAdaptive


   .. py:attribute:: threshJump

      double (*value field*)      threshJump is added to threshAdaptive on each spike
