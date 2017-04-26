SynChan
-------

.. py:class:: SynChan

   SynChan: Synaptic channel incorporating  weight and delay. Does not handle actual arrival of synaptic  events, that is done by one of the derived classes of SynHandlerBase.
   In use, the SynChan sits on the compartment connected to it by the **channel** message. One or more of the SynHandler objects connects to the SynChan through the **activation** message. The SynHandlers each manage multiple synapses, and the handlers can be fixed weight or have a learning rule.

   .. py:method:: setTau1

      (*destination message field*)      Assigns field value.


   .. py:method:: getTau1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTau2

      (*destination message field*)      Assigns field value.


   .. py:method:: getTau2

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNormalizeWeights

      (*destination message field*)      Assigns field value.


   .. py:method:: getNormalizeWeights

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: activation

      (*destination message field*)      Sometimes we want to continuously activate the channel


   .. py:attribute:: tau1

      double (*value field*)      Decay time constant for the synaptic conductance, tau1 >= tau2.


   .. py:attribute:: tau2

      double (*value field*)      Rise time constant for the synaptic conductance, tau1 >= tau2.


   .. py:attribute:: normalizeWeights

      bool (*value field*)      Flag. If true, the overall conductance is normalized by the number of individual synapses in this SynChan object.
