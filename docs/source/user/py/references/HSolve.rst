HSolve
------

.. py:class:: HSolve

   HSolve: Hines solver, for solving branching neuron models.

   .. py:attribute:: proc

      void (*shared message field*)      Handles 'reinit' and 'process' calls from a clock.


   .. py:method:: setSeed

      (*destination message field*)      Assigns field value.


   .. py:method:: getSeed

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTarget

      (*destination message field*)      Assigns field value.


   .. py:method:: getTarget

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDt

      (*destination message field*)      Assigns field value.


   .. py:method:: getDt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaAdvance

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaAdvance

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVDiv

      (*destination message field*)      Assigns field value.


   .. py:method:: getVDiv

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVMin

      (*destination message field*)      Assigns field value.


   .. py:method:: getVMin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVMax

      (*destination message field*)      Assigns field value.


   .. py:method:: getVMax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaDiv

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaDiv

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaMin

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaMin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaMax

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaMax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles 'process' call: Solver advances by one time-step.


   .. py:method:: reinit

      (*destination message field*)      Handles 'reinit' call: Solver reads in model.


   .. py:attribute:: seed

      Id (*value field*)      Use this field to specify path to a 'seed' compartment, that is, any compartment within a neuron. The HSolve object uses this seed as a handle to discover the rest of the neuronal model, which means all the remaining compartments, channels, synapses, etc.


   .. py:attribute:: target

      string (*value field*)      Specifies the path to a compartmental model to be taken over. This can be the path to any container object that has the model under it (found by performing a deep search). Alternatively, this can also be the path to any compartment within the neuron. This compartment will be used as a handle to discover the rest of the model, which means all the remaining compartments, channels, synapses, etc.


   .. py:attribute:: dt

      double (*value field*)      The time-step for this solver.


   .. py:attribute:: caAdvance

      int (*value field*)      This flag determines how current flowing into a calcium pool is computed. A value of 0 means that the membrane potential at the beginning of the time-step is used for the calculation. This is how GENESIS does its computations. A value of 1 means the membrane potential at the middle of the time-step is used. This is the correct way of integration, and is the default way.


   .. py:attribute:: vDiv

      int (*value field*)      Specifies number of divisions for lookup tables of voltage-sensitive channels.


   .. py:attribute:: vMin

      double (*value field*)      Specifies the lower bound for lookup tables of voltage-sensitive channels. Default is to automatically decide based on the tables of the channels that the solver reads in.


   .. py:attribute:: vMax

      double (*value field*)      Specifies the upper bound for lookup tables of voltage-sensitive channels. Default is to automatically decide based on the tables of the channels that the solver reads in.


   .. py:attribute:: caDiv

      int (*value field*)      Specifies number of divisions for lookup tables of calcium-sensitive channels.


   .. py:attribute:: caMin

      double (*value field*)      Specifies the lower bound for lookup tables of calcium-sensitive channels. Default is to automatically decide based on the tables of the channels that the solver reads in.


   .. py:attribute:: caMax

      double (*value field*)      Specifies the upper bound for lookup tables of calcium-sensitive channels. Default is to automatically decide based on the tables of the channels that the solver reads in.
