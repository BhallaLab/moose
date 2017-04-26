STDPSynapse
-----------

.. py:class:: STDPSynapse

   Subclass of Synapse including variables for Spike Timing Dependent Plasticity (STDP).

   .. py:method:: setAPlus

      (*destination message field*)      Assigns field value.


   .. py:method:: getAPlus

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: aPlus

      double (*value field*)      aPlus is a pre-synaptic variable that keeps a decaying 'history' of previous pre-spike(s)and is used to update the synaptic weight when a post-synaptic spike appears.It determines the t\_pre < t\_post (pre before post) part of the STDP window.
