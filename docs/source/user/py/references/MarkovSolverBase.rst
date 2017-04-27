MarkovSolverBase
----------------

.. py:class:: MarkovSolverBase

   Solver for Markov Channel.

   .. py:attribute:: channel

      void (*shared message field*)      This message couples the MarkovSolverBase to the Compartment. The compartment needs Vm in order to look up the correct matrix exponential for computing the state.


   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process message from thescheduler. The first entry is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt andso on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: handleVm

      (*destination message field*)      Handles incoming message containing voltage information.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: ligandConc

      (*destination message field*)      Handles incoming message containing ligand concentration.


   .. py:method:: init

      (*destination message field*)      Setups the table of matrix exponentials associated with the solver object.


   .. py:method:: getQ

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getState

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInitialState

      (*destination message field*)      Assigns field value.


   .. py:method:: getInitialState

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmin

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmax

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXdivs

      (*destination message field*)      Assigns field value.


   .. py:method:: getXdivs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getInvdx

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYmin

      (*destination message field*)      Assigns field value.


   .. py:method:: getYmin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYmax

      (*destination message field*)      Assigns field value.


   .. py:method:: getYmax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYdivs

      (*destination message field*)      Assigns field value.


   .. py:method:: getYdivs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getInvdy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: stateOut

      vector<double> (*source message field*)      Sends updated state to the MarkovChannel class.


   .. py:attribute:: Q

      vector< vector<double> > (*value field*)      Instantaneous rate matrix.


   .. py:attribute:: state

      vector<double> (*value field*)      Current state of the channel.


   .. py:attribute:: initialState

      vector<double> (*value field*)      Initial state of the channel.


   .. py:attribute:: xmin

      double (*value field*)      Minimum value for x axis of lookup table


   .. py:attribute:: xmax

      double (*value field*)      Maximum value for x axis of lookup table


   .. py:attribute:: xdivs

      unsigned int (*value field*)      # of divisions on x axis of lookup table


   .. py:attribute:: invdx

      double (*value field*)      Reciprocal of increment on x axis of lookup table


   .. py:attribute:: ymin

      double (*value field*)      Minimum value for y axis of lookup table


   .. py:attribute:: ymax

      double (*value field*)      Maximum value for y axis of lookup table


   .. py:attribute:: ydivs

      unsigned int (*value field*)      # of divisions on y axis of lookup table


   .. py:attribute:: invdy

      double (*value field*)      Reciprocal of increment on y axis of lookup table
