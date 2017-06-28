SeqSynHandler
-------------

.. py:class:: SeqSynHandler

   The SeqSynHandler handles synapses that recognize sequentially ordered input, where the ordering is both in space and time. It assumes that the N input synapses are ordered and equally spaced along a single linear vector.
    To do this it maintains a record of recent synaptic input, for a duration of *historyTime*, at a time interval *seqDt*. *SeqDt* is typically longer than the simulation timestep *dt* for the synapse, and cannot be shorter. *SeqDt* should represent the characteristic time of advance of the sequence.
   The SeqSynHandler uses a 2-D kernel to define how to recognize a sequence, with dependence both on space and history. This kernel is defined by the *kernelEquation* as a mathematical expression in x (synapse number) and t (time).It computes a vector with the local *response* term for each point along all inputs, by taking a 2-d convolution of the kernel with the history[time][synapse#] matrix.
   The local response can affect the synapse in three ways: 1. It can sum the entire response vector, scale by the *responseScale* term, and send to the synapse as a steady activation. Consider this a cell-wide immediate response to a sequence that it likes.
   2. It do an instantaneous scaling of the weight of each individual synapse by the corresponding entry in the response vector. It uses the *weightScale* term to do this. Consider this a short-term plasticity effect on specific synapses.
   3. It can do long-term plasticity of each individual synapse using the matched local entries in the response vector and individual synapse history as inputs to the learning rule. This is not yet implemented.

   .. py:method:: setNumSynapse

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumSynapse

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.


   .. py:method:: setKernelEquation

      (*destination message field*)      Assigns field value.


   .. py:method:: getKernelEquation

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setKernelWidth

      (*destination message field*)      Assigns field value.


   .. py:method:: getKernelWidth

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSeqDt

      (*destination message field*)      Assigns field value.


   .. py:method:: getSeqDt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setHistoryTime

      (*destination message field*)      Assigns field value.


   .. py:method:: getHistoryTime

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setResponseScale

      (*destination message field*)      Assigns field value.


   .. py:method:: getResponseScale

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSeqActivation

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWeightScale

      (*destination message field*)      Assigns field value.


   .. py:method:: getWeightScale

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getWeightScaleVec

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getKernel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getHistory

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: kernelEquation

      string (*value field*)      Equation in x and t to define kernel for sequence recognition


   .. py:attribute:: kernelWidth

      unsigned int (*value field*)      Width of kernel, i.e., number of synapses taking part in seq.


   .. py:attribute:: seqDt

      double (*value field*)      Characteristic time for advancing the sequence.


   .. py:attribute:: historyTime

      double (*value field*)      Duration to keep track of history of inputs to all synapses.


   .. py:attribute:: responseScale

      double (*value field*)      Scaling factor for sustained activation of synapse by seq


   .. py:attribute:: seqActivation

      double (*value field*)      Reports summed activation of synaptic channel by sequence


   .. py:attribute:: weightScale

      double (*value field*)      Scaling factor for weight of each synapse by response vector


   .. py:attribute:: weightScaleVec

      vector<double> (*value field*)      Vector of  weight scaling for each synapse


   .. py:attribute:: kernel

      vector<double> (*value field*)      All entries of kernel, as a linear vector


   .. py:attribute:: history

      vector<double> (*value field*)      All entries of history, as a linear vector
