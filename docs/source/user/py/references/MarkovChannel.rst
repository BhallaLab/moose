MarkovChannel
-------------

.. py:class:: MarkovChannel

   MarkovChannel : Multistate ion channel class.It deals with ion channels which can be found in one of multiple states, some of which are conducting. This implementation assumes the occurence of first order kinetics to calculate the probabilities of the channel being found in all states. Further, the rates of transition between these states can be constant, voltage-dependent or ligand dependent (only one ligand species). The current flow obtained from the channel is calculated in a deterministic method by solving the system of differential equations obtained from the assumptions above.

   .. py:method:: setLigandConc

      (*destination message field*)      Assigns field value.


   .. py:method:: getLigandConc

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVm

      (*destination message field*)      Assigns field value.


   .. py:method:: getVm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumStates

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumStates

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumOpenStates

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumOpenStates

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getState

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInitialState

      (*destination message field*)      Assigns field value.


   .. py:method:: getInitialState

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLabels

      (*destination message field*)      Assigns field value.


   .. py:method:: getLabels

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setGbar

      (*destination message field*)      Assigns field value.


   .. py:method:: getGbar

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: handleLigandConc

      (*destination message field*)      Deals with incoming messages containing information of ligand concentration


   .. py:method:: handleState

      (*destination message field*)      Deals with incoming message from MarkovSolver object containing state information of the channel.



   .. py:attribute:: ligandConc

      double (*value field*)      Ligand concentration.


   .. py:attribute:: Vm

      double (*value field*)      Membrane voltage.


   .. py:attribute:: numStates

      unsigned int (*value field*)      The number of states that the channel can occupy.


   .. py:attribute:: numOpenStates

      unsigned int (*value field*)      The number of states which are open/conducting.


   .. py:attribute:: state

      vector<double> (*value field*)      This is a row vector that contains the probabilities of finding the channel in each state.


   .. py:attribute:: initialState

      vector<double> (*value field*)      This is a row vector that contains the probabilities of finding the channel in each state at t = 0. The state of the channel is reset to this value during a call to reinit()


   .. py:attribute:: labels

      vector<string> (*value field*)      Labels for each state.


   .. py:attribute:: gbar

      vector<double> (*value field*)      A row vector containing the conductance associated with each of the open/conducting states.
