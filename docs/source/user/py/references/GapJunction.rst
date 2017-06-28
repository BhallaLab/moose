GapJunction
-----------

.. py:class:: GapJunction

   Implementation of gap junction between two compartments. The shared
   fields, 'channel1' and 'channel2' can be connected to the 'channel'
   message of the compartments at either end of the gap junction. The
   compartments will send their Vm to the gap junction and receive the
   conductance 'Gk' of the gap junction and the Vm of the other
   compartment.

   .. py:attribute:: channel1

      void (*shared message field*)      This is a shared message to couple the conductance and Vm from
      terminal 2 to the compartment at terminal 1. The first entry is source
      sending out Gk and Vm2, the second entry is destination for Vm1.


   .. py:attribute:: channel2

      void (*shared message field*)      This is a shared message to couple the conductance and Vm from
      terminal 1 to the compartment at terminal 2. The first entry is source
      sending out Gk and Vm1, the second entry is destination for Vm2.


   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects. The Process should be called \_second\_ in each clock tick, after the Init message.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: Vm1

      (*destination message field*)      Handles Vm message from compartment


   .. py:method:: Vm2

      (*destination message field*)      Handles Vm message from another compartment


   .. py:method:: setGk

      (*destination message field*)      Assigns field value.


   .. py:method:: getGk

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles 'process' call


   .. py:method:: reinit

      (*destination message field*)      Handles 'reinit' call


   .. py:attribute:: channel1Out

      double,double (*source message field*)      Sends Gk and Vm from one compartment to the other


   .. py:attribute:: channel2Out

      double,double (*source message field*)      Sends Gk and Vm from one compartment to the other


   .. py:attribute:: Gk

      double (*value field*)      Conductance of the gap junction
