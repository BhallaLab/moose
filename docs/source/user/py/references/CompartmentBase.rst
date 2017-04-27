CompartmentBase
---------------

.. py:class:: CompartmentBase

   CompartmentBase object, for branching neuron models.

   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects. The Process should be called \_second\_ in each clock tick, after the Init message.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:attribute:: init

      void (*shared message field*)      This is a shared message to receive Init messages from the scheduler objects. Its job is to separate the compartmental calculations from the message passing. It doesn't really need to be shared, as it does not use the reinit part, but the scheduler objects expect this form of message for all scheduled output. The first entry is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a dummy MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:attribute:: channel

      void (*shared message field*)      This is a shared message from a compartment to channels. The first entry is a MsgDest for the info coming from the channel. It expects Gk and Ek from the channel as args. The second entry is a MsgSrc sending Vm


   .. py:attribute:: axial

      void (*shared message field*)      This is a shared message between asymmetric compartments. axial messages (this kind) connect up to raxial messages (defined below). The soma should use raxial messages to connect to the axial message of all the immediately adjacent dendritic compartments.This puts the (low) somatic resistance in series with these dendrites. Dendrites should then use raxial messages toconnect on to more distal dendrites. In other words, raxial messages should face outward from the soma. The first entry is a MsgSrc sending Vm to the axialFuncof the target compartment. The second entry is a MsgDest for the info coming from the other compt. It expects Ra and Vm from the other compt as args. Note that the message is named after the source type.


   .. py:attribute:: raxial

      void (*shared message field*)      This is a raxial shared message between asymmetric compartments. The first entry is a MsgDest for the info coming from the other compt. It expects Vm from the other compt as an arg. The second is a MsgSrc sending Ra and Vm to the raxialFunc of the target compartment.


   .. py:method:: setVm

      (*destination message field*)      Assigns field value.


   .. py:method:: getVm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCm

      (*destination message field*)      Assigns field value.


   .. py:method:: getCm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setEm

      (*destination message field*)      Assigns field value.


   .. py:method:: getEm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInject

      (*destination message field*)      Assigns field value.


   .. py:method:: getInject

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInitVm

      (*destination message field*)      Assigns field value.


   .. py:method:: getInitVm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRm

      (*destination message field*)      Assigns field value.


   .. py:method:: getRm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRa

      (*destination message field*)      Assigns field value.


   .. py:method:: getRa

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiameter

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiameter

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setX0

      (*destination message field*)      Assigns field value.


   .. py:method:: getX0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setY0

      (*destination message field*)      Assigns field value.


   .. py:method:: getY0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setZ0

      (*destination message field*)      Assigns field value.


   .. py:method:: getZ0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setX

      (*destination message field*)      Assigns field value.


   .. py:method:: getX

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setY

      (*destination message field*)      Assigns field value.


   .. py:method:: getY

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setZ

      (*destination message field*)      Assigns field value.


   .. py:method:: getZ

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: injectMsg

      (*destination message field*)      The injectMsg corresponds to the INJECT message in the GENESIS compartment. Unlike the 'inject' field, any value assigned by handleInject applies only for a single timestep.So it needs to be updated every dt for a steady (or varying)injection current


   .. py:method:: randInject

      (*destination message field*)      Sends a random injection current to the compartment. Must beupdated each timestep.Arguments to randInject are probability and current.


   .. py:method:: injectMsg

      (*destination message field*)      The injectMsg corresponds to the INJECT message in the GENESIS compartment. Unlike the 'inject' field, any value assigned by handleInject applies only for a single timestep.So it needs to be updated every dt for a steady (or varying)injection current


   .. py:method:: cable

      (*destination message field*)      Message for organizing compartments into groups, calledcables. Doesn't do anything.


   .. py:method:: displace

      (*destination message field*)      Displaces compartment by specified vector


   .. py:method:: setGeomAndElec

      (*destination message field*)      Assigns length and dia and accounts for any electrical scaling needed as a result.


   .. py:method:: process

      (*destination message field*)      Handles 'process' call


   .. py:method:: reinit

      (*destination message field*)      Handles 'reinit' call


   .. py:method:: initProc

      (*destination message field*)      Handles Process call for the 'init' phase of the CompartmentBase calculations. These occur as a separate Tick cycle from the regular proc cycle, and should be called before the proc msg.


   .. py:method:: initReinit

      (*destination message field*)      Handles Reinit call for the 'init' phase of the CompartmentBase calculations.


   .. py:method:: handleChannel

      (*destination message field*)      Handles conductance and Reversal potential arguments from Channel


   .. py:method:: handleRaxial

      (*destination message field*)      Handles Raxial info: arguments are Ra and Vm.


   .. py:method:: handleAxial

      (*destination message field*)      Handles Axial information. Argument is just Vm.


   .. py:attribute:: VmOut

      double (*source message field*)      Sends out Vm value of compartment on each timestep


   .. py:attribute:: axialOut

      double (*source message field*)      Sends out Vm value of compartment to adjacent compartments,on each timestep


   .. py:attribute:: raxialOut

      double,double (*source message field*)      Sends out Raxial information on each timestep, fields are Ra and Vm


   .. py:attribute:: Vm

      double (*value field*)      membrane potential


   .. py:attribute:: Cm

      double (*value field*)      Membrane capacitance


   .. py:attribute:: Em

      double (*value field*)      Resting membrane potential


   .. py:attribute:: Im

      double (*value field*)      Current going through membrane


   .. py:attribute:: inject

      double (*value field*)      Current injection to deliver into compartment


   .. py:attribute:: initVm

      double (*value field*)      Initial value for membrane potential


   .. py:attribute:: Rm

      double (*value field*)      Membrane resistance


   .. py:attribute:: Ra

      double (*value field*)      Axial resistance of compartment


   .. py:attribute:: diameter

      double (*value field*)      Diameter of compartment


   .. py:attribute:: length

      double (*value field*)      Length of compartment


   .. py:attribute:: x0

      double (*value field*)      X coordinate of start of compartment


   .. py:attribute:: y0

      double (*value field*)      Y coordinate of start of compartment


   .. py:attribute:: z0

      double (*value field*)      Z coordinate of start of compartment


   .. py:attribute:: x

      double (*value field*)      x coordinate of end of compartment


   .. py:attribute:: y

      double (*value field*)      y coordinate of end of compartment


   .. py:attribute:: z

      double (*value field*)      z coordinate of end of compartment
