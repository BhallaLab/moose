PIDController
-------------

.. py:class:: PIDController

   PID feedback controller.PID stands for Proportional-Integral-Derivative. It is used to feedback control dynamical systems. It tries to create a feedback output such that the sensed (measured) parameter is held at command value. Refer to wikipedia (http://wikipedia.org) for details on PID Controller.

   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: setGain

      (*destination message field*)      Assigns field value.


   .. py:method:: getGain

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSaturation

      (*destination message field*)      Assigns field value.


   .. py:method:: getSaturation

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCommand

      (*destination message field*)      Assigns field value.


   .. py:method:: getCommand

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSensed

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauI

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauI

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauD

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getOutputValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getError

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIntegral

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDerivative

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getE\_previous

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: commandIn

      (*destination message field*)      Command (desired value) input. This is known as setpoint (SP) in control theory.


   .. py:method:: sensedIn

      (*destination message field*)      Sensed parameter - this is the one to be tuned. This is known as process variable (PV) in control theory. This comes from the process we are trying to control.


   .. py:method:: gainDest

      (*destination message field*)      Destination message to control the PIDController gain dynamically.


   .. py:method:: process

      (*destination message field*)      Handle process calls.


   .. py:method:: reinit

      (*destination message field*)      Reinitialize the object.


   .. py:attribute:: output

      double (*source message field*)      Sends the output of the PIDController. This is known as manipulated variable (MV) in control theory. This should be fed into the process which we are trying to control.


   .. py:attribute:: gain

      double (*value field*)      This is the proportional gain (Kp). This tuning parameter scales the proportional term. Larger gain usually results in faster response, but too much will lead to instability and oscillation.


   .. py:attribute:: saturation

      double (*value field*)      Bound on the permissible range of output. Defaults to maximum double value.


   .. py:attribute:: command

      double (*value field*)      The command (desired) value of the sensed parameter. In control theory this is commonly known as setpoint(SP).


   .. py:attribute:: sensed

      double (*value field*)      Sensed (measured) value. This is commonly known as process variable(PV) in control theory.


   .. py:attribute:: tauI

      double (*value field*)      The integration time constant, typically = dt. This is actually proportional gain divided by integral gain (Kp/Ki)). Larger Ki (smaller tauI) usually leads to fast elimination of steady state errors at the cost of larger overshoot.


   .. py:attribute:: tauD

      double (*value field*)      The differentiation time constant, typically = dt / 4. This is derivative gain (Kd) times proportional gain (Kp). Larger Kd (tauD) decreases overshoot at the cost of slowing down transient response and may lead to instability.


   .. py:attribute:: outputValue

      double (*value field*)      Output of the PIDController. This is given by:      gain * ( error + INTEGRAL[ error dt ] / tau\_i   + tau\_d * d(error)/dt )
      Where gain = proportional gain (Kp), tau\_i = integral gain (Kp/Ki) and tau\_d = derivative gain (Kd/Kp). In control theory this is also known as the manipulated variable (MV)


   .. py:attribute:: error

      double (*value field*)      The error term, which is the difference between command and sensed value.


   .. py:attribute:: integral

      double (*value field*)      The integral term. It is calculated as INTEGRAL(error dt) = previous\_integral + dt * (error + e\_previous)/2.


   .. py:attribute:: derivative

      double (*value field*)      The derivative term. This is (error - e\_previous)/dt.


   .. py:attribute:: e\_previous

      double (*value field*)      The error term for previous step.
