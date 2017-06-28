VClamp
------

.. py:class:: VClamp

   Voltage clamp object for holding neuronal compartments at a specific voltage.
   This implementation uses a builtin RC circuit to filter the  command input and then use a PID to bring the sensed voltage (Vm from compartment) to the filtered command potential.
   Usage: Connect the `currentOut` source of VClamp to `injectMsg` dest of Compartment. Connect the `VmOut` source of Compartment to `set\_sensed` dest of VClamp. Either set `command` field to a fixed value, or connect an appropriate source of command potential (like the `outputOut` message of an appropriately configured PulseGen) to `set\_command` dest.
   The default settings for the RC filter and PID controller should be fine. For step change in command voltage, good defaults withintegration time step dt are as follows:
       time constant of RC filter, tau = 5 * dt
       proportional gain of PID, gain = Cm/dt where Cm is the membrane    capacitance of the compartment
       integration time of PID, ti = dt
       derivative time  of PID, td = 0

   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive Process messages from the scheduler


   .. py:method:: getCommand

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getCurrent

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSensed

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMode

      (*destination message field*)      Assigns field value.


   .. py:method:: getMode

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTi

      (*destination message field*)      Assigns field value.


   .. py:method:: getTi

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTd

      (*destination message field*)      Assigns field value.


   .. py:method:: getTd

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTau

      (*destination message field*)      Assigns field value.


   .. py:method:: getTau

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setGain

      (*destination message field*)      Assigns field value.


   .. py:method:: getGain

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: sensedIn

      (*destination message field*)      The `VmOut` message of the Compartment object should be connected here.


   .. py:method:: commandIn

      (*destination message field*)        The command voltage source should be connected to this.


   .. py:method:: process

      (*destination message field*)      Handles 'process' call on each time step.


   .. py:method:: reinit

      (*destination message field*)      Handles 'reinit' call


   .. py:attribute:: currentOut

      double (*source message field*)      Sends out current output of the clamping circuit. This should be connected to the `injectMsg` field of a compartment to voltage clamp it.


   .. py:attribute:: command

      double (*value field*)      Command input received by the clamp circuit.


   .. py:attribute:: current

      double (*value field*)      The amount of current injected by the clamp into the membrane.


   .. py:attribute:: sensed

      double (*value field*)      Membrane potential read from compartment.


   .. py:attribute:: mode

      unsigned int (*value field*)      Working mode of the PID controller.

         mode = 0, standard PID with proportional, integral and derivative all acting on the error.

         mode = 1, derivative action based on command input

         mode = 2, proportional action and derivative action are based on command input.


   .. py:attribute:: ti

      double (*value field*)      Integration time of the PID controller. Defaults to 1e9, i.e. integral action is negligibly small.


   .. py:attribute:: td

      double (*value field*)      Derivative time of the PID controller. This defaults to 0,i.e. derivative action is unused.


   .. py:attribute:: tau

      double (*value field*)      Time constant of the lowpass filter at input of the PID controller. This smooths out abrupt changes in the input. Set it to  5 * dt or more to avoid overshoots.


   .. py:attribute:: gain

      double (*value field*)      Proportional gain of the PID controller.
