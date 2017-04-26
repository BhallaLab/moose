STDPSynHandler
--------------

.. py:class:: STDPSynHandler

   The STDPSynHandler handles synapses with spike timing dependent plasticity (STDP). It uses two priority queues to manage pre and post spikes.

   .. py:method:: setNumSynapse

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumSynapse

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.


   .. py:method:: addPostSpike

      (*destination message field*)      Handles arriving spike messages from post-synaptic neuron, inserts into postEvent queue.


   .. py:method:: setAMinus0

      (*destination message field*)      Assigns field value.


   .. py:method:: getAMinus0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAMinus

      (*destination message field*)      Assigns field value.


   .. py:method:: getAMinus

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauMinus

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauMinus

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAPlus0

      (*destination message field*)      Assigns field value.


   .. py:method:: getAPlus0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauPlus

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauPlus

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWeightMax

      (*destination message field*)      Assigns field value.


   .. py:method:: getWeightMax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWeightMin

      (*destination message field*)      Assigns field value.


   .. py:method:: getWeightMin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: aMinus0

      double (*value field*)      aMinus0 is added to aMinus on every pre-spike


   .. py:attribute:: aMinus

      double (*value field*)      aMinus is a post-synaptic variable that keeps a decaying 'history' of previous post-spike(s)and is used to update the synaptic weight when a pre-synaptic spike appears.It determines the t\_pre > t\_post (pre after post) part of the STDP window.


   .. py:attribute:: tauMinus

      double (*value field*)      aMinus decays with tauMinus time constant


   .. py:attribute:: aPlus0

      double (*value field*)      aPlus0 is added to aPlus on every pre-spike


   .. py:attribute:: tauPlus

      double (*value field*)      aPlus decays with tauPlus time constant


   .. py:attribute:: weightMax

      double (*value field*)      an upper bound on the weight


   .. py:attribute:: weightMin

      double (*value field*)      a lower bound on the weight
