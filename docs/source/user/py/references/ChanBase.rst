ChanBase
--------

.. py:class:: ChanBase

   ChanBase: Base class for assorted ion channels.Presents a common interface for all of them.

   .. py:attribute:: channel

      void (*shared message field*)      This is a shared message to couple channel to compartment. The first entry is a MsgSrc to send Gk and Ek to the compartment The second entry is a MsgDest for Vm from the compartment.


   .. py:attribute:: ghk

      void (*shared message field*)      Message to Goldman-Hodgkin-Katz object


   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive Process message from scheduler


   .. py:method:: Vm

      (*destination message field*)      Handles Vm message coming in from compartment


   .. py:method:: Vm

      (*destination message field*)      Handles Vm message coming in from compartment


   .. py:method:: setGbar

      (*destination message field*)      Assigns field value.


   .. py:method:: getGbar

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setModulation

      (*destination message field*)      Assigns field value.


   .. py:method:: getModulation

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setEk

      (*destination message field*)      Assigns field value.


   .. py:method:: getEk

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setGk

      (*destination message field*)      Assigns field value.


   .. py:method:: getGk

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIk

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: channelOut

      double,double (*source message field*)      Sends channel variables Gk and Ek to compartment


   .. py:attribute:: permeabilityOut

      double (*source message field*)      Conductance term going out to GHK object


   .. py:attribute:: IkOut

      double (*source message field*)      Channel current. This message typically goes to concenobjects that keep track of ion concentration.


   .. py:attribute:: Gbar

      double (*value field*)      Maximal channel conductance


   .. py:attribute:: modulation

      double (*value field*)      Modulation, i.e, scale factor for channel conductance.Note that this is a regular parameter, it is not recomputed each timestep. Thus one can use a slow update, say, from a molecule pool, to send a message to set the modulation, and it will stay at the set value even if the channel runs many timesteps before the next assignment. This differs from the GENESIS semantics of a similar message,which required update each timestep.


   .. py:attribute:: Ek

      double (*value field*)      Reversal potential of channel


   .. py:attribute:: Gk

      double (*value field*)      Channel conductance variable


   .. py:attribute:: Ik

      double (*value field*)      Channel current variable
