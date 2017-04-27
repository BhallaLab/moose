IzhikevichNrn
-------------

.. py:class:: IzhikevichNrn

   Izhikevich model of spiking neuron (Izhikevich,EM. 2003. Simple model of spiking neurons. Neural Networks, IEEE Transactions on 14(6). pp 1569-1572).
    This class obeys the equations (in physiological units):
     dVm/dt = 0.04 * Vm^2 + 5 * Vm + 140 - u + inject
     du/dt = a * (b * Vm - u)
    if Vm >= Vmax then Vm = c and u = u + d
    Vmax = 30 mV in the paper.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive Process message from scheduler


   .. py:attribute:: channel

      void (*shared message field*)      This is a shared message from a IzhikevichNrn to channels.The first entry is a MsgDest for the info coming from the channel. It expects Gk and Ek from the channel as args. The second entry is a MsgSrc sending Vm


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: setVmax

      (*destination message field*)      Assigns field value.


   .. py:method:: getVmax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setC

      (*destination message field*)      Assigns field value.


   .. py:method:: getC

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setD

      (*destination message field*)      Assigns field value.


   .. py:method:: getD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setA

      (*destination message field*)      Assigns field value.


   .. py:method:: getA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setB

      (*destination message field*)      Assigns field value.


   .. py:method:: getB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getU

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVm

      (*destination message field*)      Assigns field value.


   .. py:method:: getVm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInject

      (*destination message field*)      Assigns field value.


   .. py:method:: getInject

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRmByTau

      (*destination message field*)      Assigns field value.


   .. py:method:: getRmByTau

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAccommodating

      (*destination message field*)      Assigns field value.


   .. py:method:: getAccommodating

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setU0

      (*destination message field*)      Assigns field value.


   .. py:method:: getU0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInitVm

      (*destination message field*)      Assigns field value.


   .. py:method:: getInitVm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInitU

      (*destination message field*)      Assigns field value.


   .. py:method:: getInitU

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAlpha

      (*destination message field*)      Assigns field value.


   .. py:method:: getAlpha

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setBeta

      (*destination message field*)      Assigns field value.


   .. py:method:: getBeta

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setGamma

      (*destination message field*)      Assigns field value.


   .. py:method:: getGamma

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: injectMsg

      (*destination message field*)      Injection current into the neuron.


   .. py:method:: cDest

      (*destination message field*)      Destination message to modify parameter c at runtime.


   .. py:method:: dDest

      (*destination message field*)      Destination message to modify parameter d at runtime.


   .. py:method:: bDest

      (*destination message field*)      Destination message to modify parameter b at runtime


   .. py:method:: aDest

      (*destination message field*)      Destination message modify parameter a at runtime.


   .. py:method:: handleChannel

      (*destination message field*)      Handles conductance and reversal potential arguments from Channel


   .. py:attribute:: VmOut

      double (*source message field*)      Sends out Vm


   .. py:attribute:: spikeOut

      double (*source message field*)      Sends out spike events


   .. py:attribute:: VmOut

      double (*source message field*)      Sends out Vm


   .. py:attribute:: Vmax

      double (*value field*)      Maximum membrane potential. Membrane potential is reset to c whenever it reaches Vmax. NOTE: Izhikevich model specifies the PEAK voltage, rather than THRSHOLD voltage. The threshold depends on the previous history.


   .. py:attribute:: c

      double (*value field*)      Reset potential. Membrane potential is reset to c whenever it reaches Vmax.


   .. py:attribute:: d

      double (*value field*)      Parameter d in Izhikevich model. Unit is V/s.


   .. py:attribute:: a

      double (*value field*)      Parameter a in Izhikevich model. Unit is s^{-1}


   .. py:attribute:: b

      double (*value field*)      Parameter b in Izhikevich model. Unit is s^{-1}


   .. py:attribute:: u

      double (*value field*)      Parameter u in Izhikevich equation. Unit is V/s


   .. py:attribute:: Vm

      double (*value field*)      Membrane potential, equivalent to v in Izhikevich equation.


   .. py:attribute:: Im

      double (*value field*)      Total current going through the membrane. Unit is A.


   .. py:attribute:: inject

      double (*value field*)      External current injection into the neuron


   .. py:attribute:: RmByTau

      double (*value field*)      Hidden coefficient of input current term (I) in Izhikevich model. Defaults to 1e9 Ohm/s.


   .. py:attribute:: accommodating

      bool (*value field*)      True if this neuron is an accommodating one. The equation for recovery variable u is special in this case.


   .. py:attribute:: u0

      double (*value field*)      This is used for accommodating neurons where recovery variables u is computed as: u += tau*a*(b*(Vm-u0))


   .. py:attribute:: initVm

      double (*value field*)      Initial membrane potential. Unit is V.


   .. py:attribute:: initU

      double (*value field*)      Initial value of u.


   .. py:attribute:: alpha

      double (*value field*)      Coefficient of v^2 in Izhikevich equation. Defaults to 0.04 in physiological unit. In SI it should be 40000.0. Unit is V^-1 s^{-1}


   .. py:attribute:: beta

      double (*value field*)      Coefficient of v in Izhikevich model. Defaults to 5 in physiological unit, 5000.0 for SI units. Unit is s^{-1}


   .. py:attribute:: gamma

      double (*value field*)      Constant term in Izhikevich model. Defaults to 140 in both physiological and SI units. unit is V/s.
